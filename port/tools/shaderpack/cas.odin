// The content-addressed compile cache (--cache-dir DIR).
//
// A compile job's result (the final SPIR-V module after the descriptor set move, or "this entry function does not
// exist") is stored under the XXH3-128 hash of everything that can change it:
//   - this cache's version, the builder version, and the source of the compile code itself: the region of main.odin
//     between the "compile cache: begin/end" markers (defines, wrappers, the DXC passes, the set move) and spirv.odin,
//     both embedded at build time, so editing the generator invalidates exactly what it can change;
//   - `dxc --version` and `spirv-val --version`;
//   - the job's identity (kind, stage, entry point, vertex type, name, entry function), its DXC defines and its
//     option wrapper;
//   - the content hash of every file of the HLSL tree the wrapper reaches through #include, transitively.
//     Includes are found by a scan of `#include "file"` lines that ignores preprocessor conditions, so the set can
//     only be larger than what DXC reads: a change to a file no job includes invalidates nothing, a change to any
//     file a job may include invalidates that job.
// Modification times and paths are not part of the key: re-staging the tree (stage.sh copies every file) keeps the
// cache, and two trees with the same content share it. Reflection is not stored; finish_job derives it from the
// module, as for the per-job cache. Failed compiles are never stored.
// Entries are written under a temporary name and renamed, so concurrent workers and interrupted runs never leave a
// partial entry.
package main

import "core:fmt"
import "core:hash/xxhash"
import "core:os"
import "core:slice"
import "core:strings"
import "core:sync"

CAS_VERSION :: "shaderpack-cas 1"

MAIN_SOURCE :: #load("main.odin", string)
SPIRV_SOURCE :: #load("spirv.odin", string)
CAS_BEGIN_MARK :: "// ---- compile cache: begin"
CAS_END_MARK :: "// ---- compile cache: end"

Cas_Source :: struct {
	hash:     xxhash.XXH3_128_hash,
	includes: []string,
}

Cas :: struct {
	dir:     string,
	sources: map[string]Cas_Source, // the HLSL tree, by file name (the tree is flat); read-only once built
	base:    []u8, // key material shared by every job
	hits:    int,
	stores:  int,
}

cas: Cas

// `#include "name"` targets of a source text, in order; commented-out lines are skipped, conditions are not
cas_scan_includes :: proc(text: string, allocator := context.allocator) -> []string {
	out := make([dynamic]string, allocator)
	for line in strings.split_lines(text, context.temp_allocator) {
		s := strings.trim_left_space(line)
		if !strings.has_prefix(s, "#") {continue}
		s = strings.trim_left_space(s[1:])
		if !strings.has_prefix(s, "include") {continue}
		s = strings.trim_left_space(s[len("include"):])
		if !strings.has_prefix(s, "\"") {continue}
		e := strings.index_byte(s[1:], '"')
		if e < 0 {continue}
		append(&out, strings.clone(s[1:1 + e], allocator))
	}
	return out[:]
}

cas_hex :: proc(h: xxhash.XXH3_128_hash) -> string {
	return fmt.aprintf("%016x%016x", u64(h >> 64), u64(h))
}

cas_tool_version :: proc(tool: string) -> string {
	state, stdout, stderr, err := os.process_exec(os.Process_Desc{command = {tool, "--version"}}, context.temp_allocator)
	if err != nil || !state.exited || state.exit_code != 0 {fail("--cache-dir: cannot run %s --version", tool)}
	return strings.clone(strings.concatenate({string(stdout), string(stderr)}, context.temp_allocator))
}

// the compile code between the markers: a change outside it (the pack writer, the map reader) keeps the cache
cas_generator_source :: proc() -> string {
	src := MAIN_SOURCE
	b := strings.index(src, CAS_BEGIN_MARK)
	e := strings.index(src, CAS_END_MARK)
	if b < 0 || e < b {fail("--cache-dir: main.odin has no compile cache markers")}
	return src[b:e]
}

cas_init :: proc(dir, hlsl: string) {
	cas.dir = dir
	for attempt := 0; attempt < 4 && !os.exists(dir); attempt += 1 {_ = os.make_directory_all(dir)}
	if !os.exists(dir) {fail("--cache-dir: cannot create %s", dir)}
	for i in 0 ..< 256 {
		sub := fmt.tprintf("%s/%02x", dir, i)
		if !os.exists(sub) {_ = os.make_directory(sub)}
	}
	entries, lerr := os.read_directory_by_path(hlsl, -1, context.allocator)
	if lerr != nil {fail("cannot list %s", hlsl)}
	for e in entries {
		text, err := os.read_entire_file(strings.concatenate({hlsl, "/", e.name}, context.temp_allocator), context.allocator)
		if err != nil {continue} // a directory or a dangling link: no #include can reach it
		cas.sources[strings.clone(e.name)] = Cas_Source{hash = xxhash.XXH3_128(text), includes = cas_scan_includes(string(text))}
		delete(text)
	}
	b: [dynamic]u8
	add :: proc(b: ^[dynamic]u8, s: string) {
		n := u32(len(s))
		append(b, ..([^]u8)(&n)[:4])
		append(b, ..transmute([]u8)s)
	}
	add(&b, CAS_VERSION)
	add(&b, BUILDER_VERSION)
	add(&b, cas_generator_source())
	add(&b, SPIRV_SOURCE)
	add(&b, cas_tool_version(opts.dxc))
	add(&b, cas_tool_version("spirv-val"))
	cas.base = b[:]
	fmt.printfln("shaderpack: compile cache %s (%d tree files hashed)", dir, len(cas.sources))
}

// the job's key; defs are its DXC defines, wrapper its g.hlsl
cas_job_key :: proc(j: ^Job, wrapper: string, defs: []string) -> string {
	b := make([dynamic]u8, context.temp_allocator)
	append(&b, ..cas.base)
	add :: proc(b: ^[dynamic]u8, s: string) {
		n := u32(len(s))
		append(b, ..([^]u8)(&n)[:4])
		append(b, ..transmute([]u8)s)
	}
	add(&b, fmt.tprintf("%d %d %d %d %x", j.kind, j.stage, j.ep, j.vt, j.function_hash))
	add(&b, j.name)
	add(&b, job_function(j))
	for d in defs {add(&b, d)}
	add(&b, wrapper)
	// the include closure of the wrapper and of the final wrapper's own include (emit_final)
	seen := make(map[string]bool, allocator = context.temp_allocator)
	stack := make([dynamic]string, context.temp_allocator)
	append(&stack, "global_texture.fx")
	for inc in cas_scan_includes(wrapper, context.temp_allocator) {append(&stack, inc)}
	for len(stack) > 0 {
		name := pop(&stack)
		if name in seen {continue}
		seen[name] = true
		if src, ok := cas.sources[name]; ok {
			for inc in src.includes {if !(inc in seen) {append(&stack, inc)}}
		}
	}
	names := make([dynamic]string, context.temp_allocator)
	for n in seen {append(&names, n)}
	slice.sort(names[:])
	for n in names {
		add(&b, n)
		if src, ok := cas.sources[n]; ok {
			h := src.hash
			append(&b, ..([^]u8)(&h)[:16])
		} else {
			add(&b, "<not in the tree>")
		}
	}
	key := cas_hex(xxhash.XXH3_128(b[:]))
	return fmt.tprintf("%s/%s/%s", cas.dir, key[:2], key)
}

// a stored result for the key: the job is finished (Ok or No_Entry) and true
cas_lookup :: proc(j: ^Job, key: string) -> bool {
	if os.exists(fmt.tprintf("%s.none", key)) {
		j.status = .No_Entry
		j.cached = true
		sync.atomic_add(&cas.hits, 1)
		return true
	}
	spv, err := os.read_entire_file(fmt.tprintf("%s.spv", key), context.allocator)
	if err != nil {return false}
	if !finish_job(j, spv, false) {
		// an unreadable entry: compile again (and overwrite it)
		j.status = .Pending
		j.message = ""
		return false
	}
	j.cached = true
	sync.atomic_add(&cas.hits, 1)
	return true
}

cas_store :: proc(j: ^Job, key: string, ext: string, data: []u8) {
	path := fmt.tprintf("%s.%s", key, ext)
	tmp := fmt.tprintf("%s.%x.tmp", path, uintptr(j))
	if os.write_entire_file(tmp, data) != nil {return}
	if os.rename(tmp, path) != nil {
		os.remove(tmp)
		return
	}
	sync.atomic_add(&cas.stores, 1)
}
