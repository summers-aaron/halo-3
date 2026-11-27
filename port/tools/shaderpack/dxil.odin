package main

// shaderpack --dxil: the shader pack of version 4, for Direct3D 12. The retail Xenos microcode of the given maps (and
// executable) translated by port/tools/xenos_spirv's HLSL backend (hlsl.odin) and compiled to DXIL by DXC: the same
// shaders, keys, tables and helper set as version 3 (xenos.odin), so the D3D12 device binds them as the Vulkan device
// binds version 3 (abi.odin, D3D12 section).
//
// Every shader is translated by both backends and the two Metas (interface, textures, vertex elements, constants,
// flags) must agree field for field; the HLSL modules must group the shaders exactly as the SPIR-V modules do, so the
// version 4 tables are version 3's. Each unique module compiles with
//   dxc -T <vs|ps|cs|ds|hs|gs>_6_0 -E main -HV 2021 -Gis -O3 -WX -Qstrip_debug -Qstrip_reflect
// (warnings are errors), the root signature alone with -T rootsig_1_1. DXC's internal validator validates and hashes
// (signs) every container; the build checks each one (DXBC, non-zero digest, DXIL / RTS0 / signature parts).
//
// File: the version 3 layout (port/shaders/shader_pack.h) with version 4 and a 116-byte header: the version 3 header,
// then module_format ('DXIL'), point_gs_module (the point-sprite GS D3D12 needs) and root_signature_module. Module
// table entries name DXIL containers (the spirv_size field holds the container size). Work files: --dxil-work DIR
// (default tmp/shaders-work/dxil): hlsl/<module>.hlsl, dxil/<module>.dxil and its DXC diagnostics dxil/<module>.log
// (-Fe; empty when clean), cache/<key>.dxil (keyed by the DXC version, flags, profile and source; --rebuild ignores
// it). DXC runs through dxil_spawn (dxil_spawn_posix.odin: posix_spawnp, no fork of this multi-gigabyte process).

import "core:fmt"
import "core:hash"
import "core:hash/xxhash"
import "core:os"
import "core:slice"
import "core:strings"
import "core:sync"
import "core:thread"
import "core:time"
import "vendor:zlib"
import xenos "../xenos_spirv"

DXIL_PACK_VERSION :: 4
DXIL_HEADER_SIZE :: 116
DXIL_FORMAT :: 0x4C495844 // "DXIL" little-endian
DXIL_DXC_FLAGS :: [?]string{"-HV", "2021", "-Gis", "-O3", "-WX", "-Qstrip_debug", "-Qstrip_reflect"}

Dxil_Result :: struct {
	spirv:      []u32,
	text:       string,
	profile:    string,
	meta:       xenos.Meta,
	error:      string,
	spv_module: int,
	module:     int,
}

Dxil_Pool :: struct {
	set:     ^Microcode_Set,
	results: []Dxil_Result,
	next:    int,
}

// The two backends' Metas: "" when equal, else what differs.
dxil_meta_diff :: proc(a, b: ^xenos.Meta) -> string {
	if a.model != b.model {return "model"}
	if a.registers != b.registers {return "registers"}
	if a.labels != b.labels {return "labels"}
	if !slice.equal(a.textures[:], b.textures[:]) {return "textures"}
	if a.vertex_elements != b.vertex_elements {return "vertex_elements"}
	if a.vs_outputs != b.vs_outputs {return "vs_outputs"}
	if a.ps_inputs != b.ps_inputs {return "ps_inputs"}
	if a.param_gen != b.param_gen {return "param_gen"}
	if a.color_outputs != b.color_outputs {return "color_outputs"}
	if a.depth_output != b.depth_output {return "depth_output"}
	if a.kills != b.kills {return "kills"}
	if a.memexport != b.memexport {return "memexport"}
	if !slice.equal(a.memexport_streams[:], b.memexport_streams[:]) {return "memexport_streams"}
	if a.uses_bda != b.uses_bda {return "uses_bda"}
	if a.point_size != b.point_size {return "point_size"}
	if a.float_defs != b.float_defs {return "float_defs"}
	if !slice.equal(a.defs[:], b.defs[:]) {return "defs"}
	if !slice.equal(a.elements[:], b.elements[:]) {return "elements"}
	return ""
}

dxil_translate_worker :: proc(pool: ^Dxil_Pool) {
	for {
		i := sync.atomic_add(&pool.next, 1)
		if i >= len(pool.results) {return}
		s := &pool.set.shaders[i]
		stage: xenos.Stage = s.stage == .PS ? .Pixel : .Vertex
		r := xenos.translate_shader(s.object, s.ucode, stage)
		hr := xenos.translate_shader_hlsl(s.object, s.ucode, stage)
		out := &pool.results[i]
		out.spirv = r.words
		out.text = hr.text
		out.profile = hr.profile
		out.meta = hr.meta
		switch {
		case r.error != "": out.error = fmt.aprintf("SPIR-V: %s", r.error)
		case hr.error != "": out.error = fmt.aprintf("HLSL: %s", hr.error)
		case:
			if d := dxil_meta_diff(&r.meta, &hr.meta); d != "" {out.error = fmt.aprintf("the backends' metadata differ: %s", d)}
		}
		free_all(context.temp_allocator)
	}
}

Dxil_Module :: struct {
	text:       string,
	profile:    string,
	entry:      string,
	hash:       u64,
	label:      string, // manifest: what the module is
	code:       []u8,
	compressed: []u8,
	cached:     bool,
	error:      string,
	parts:      u32, // DXIL_PART_* found in the container
}

DXIL_PART_DXIL :: 1
DXIL_PART_RTS0 :: 2
DXIL_PART_PSV0 :: 4
DXIL_PART_SIGNATURES :: 8 // ISG1 / OSG1

Dxil_Compile_Pool :: struct {
	modules: []Dxil_Module,
	next:    int,
	done:    int,
	dxc_version: string,
	work:    string,
}

dxil_compile_worker :: proc(pool: ^Dxil_Compile_Pool) {
	for {
		i := sync.atomic_add(&pool.next, 1)
		if i >= len(pool.modules) {return}
		dxil_compile_module(pool, i)
		free_all(context.temp_allocator)
		d := sync.atomic_add(&pool.done, 1) + 1
		if d % 100 == 0 || d == len(pool.modules) {fmt.printf("\rshaderpack --dxil: compiled %d / %d", d, len(pool.modules))}
	}
}

dxil_compile_module :: proc(pool: ^Dxil_Compile_Pool, i: int) {
	m := &pool.modules[i]
	flags := DXIL_DXC_FLAGS
	key_text := strings.concatenate({pool.dxc_version, "\x00", strings.join(flags[:], " ", context.temp_allocator), "\x00", m.profile, "\x00", m.entry, "\x00", m.text}, context.temp_allocator)
	h := xxhash.XXH3_128(transmute([]u8)key_text)
	key := fmt.tprintf("%016x%016x", u64(h >> 64), u64(h))
	cache_path := fmt.tprintf("%s/cache/%s.dxil", pool.work, key)
	src := fmt.tprintf("%s/hlsl/%d.hlsl", pool.work, i)
	out := fmt.tprintf("%s/dxil/%d.dxil", pool.work, i)
	if !write_file(src, m.text) {m.error = "cannot write the source"; return}
	if !opts.rebuild {
		if data, err := os.read_entire_file(cache_path, context.allocator); err == nil {
			m.code = data
			m.cached = true
			_ = os.write_entire_file(out, data)
			return
		}
	}
	cmd := make([dynamic]string, context.temp_allocator)
	append(&cmd, opts.dxc, "-T", m.profile, "-E", m.entry)
	if m.profile != "rootsig_1_1" {append(&cmd, ..flags[:])}
	// diagnostics: DXC writes them to <i>.log (-Fe), not to its stdout / stderr (removed first: a crash before DXC
	// opens it must not leave an earlier build's); dxil_spawn waits without polling and without forking this process
	log := fmt.tprintf("%s/dxil/%d.log", pool.work, i)
	_ = os.remove(log)
	append(&cmd, "-Fe", log, "-Fo", out, src)
	exited, code, serr := dxil_spawn(cmd[:])
	if serr != "" {m.error = strings.clone(serr); return}
	diagnostics, _ := os.read_entire_file(log, context.temp_allocator) // absent if DXC never got to open it
	if !exited || code != 0 || len(diagnostics) > 0 {
		text := string(diagnostics)
		m.error = strings.clone(text[:min(len(text), 600)])
		if m.error == "" {m.error = fmt.aprintf("dxc failed without a diagnostic (exited %v, code %d)", exited, code)}
		if exited && code == 0 {m.error = fmt.aprintf("dxc printed diagnostics: %s", m.error)}
		return
	}
	data, rerr := os.read_entire_file(out, context.allocator)
	if rerr != nil {m.error = "dxc wrote no output"; return}
	m.code = data
	_ = os.write_entire_file(cache_path, data)
}

// The DXBC container of a DXIL module: magic, a non-zero validator digest (DXC's internal validator hashes what it
// validated: the "signature" the D3D12 runtime checks), its size, and its parts.
dxil_check_container :: proc(m: ^Dxil_Module) -> string {
	c := m.code
	if len(c) < 32 || string(c[0:4]) != "DXBC" {return "not a DXBC container"}
	digest := false
	for b in c[4:20] {if b != 0 {digest = true}}
	if !digest {return "no validator digest (unsigned)"}
	le32 :: proc(b: []u8, o: int) -> u32 {return u32(b[o]) | u32(b[o + 1]) << 8 | u32(b[o + 2]) << 16 | u32(b[o + 3]) << 24}
	if int(le32(c, 24)) != len(c) {return "container size mismatch"}
	n := int(le32(c, 28))
	if 32 + 4 * n > len(c) {return "bad part table"}
	for k in 0 ..< n {
		o := int(le32(c, 32 + 4 * k))
		if o + 8 > len(c) {return "bad part offset"}
		switch string(c[o:o + 4]) {
		case "DXIL": m.parts |= DXIL_PART_DXIL
		case "RTS0": m.parts |= DXIL_PART_RTS0
		case "PSV0": m.parts |= DXIL_PART_PSV0
		case "ISG1", "OSG1": m.parts |= DXIL_PART_SIGNATURES
		}
	}
	if m.profile == "rootsig_1_1" {
		if m.parts & DXIL_PART_RTS0 == 0 {return "no root signature part"}
	} else if m.parts & (DXIL_PART_DXIL | DXIL_PART_RTS0 | DXIL_PART_PSV0) != DXIL_PART_DXIL | DXIL_PART_RTS0 | DXIL_PART_PSV0 {
		return "missing DXIL / RTS0 / PSV0 part"
	}
	if len(c) % 4 != 0 {return "container size not a multiple of 4"}
	return ""
}

// Translates every shader of the set with both backends, compiles the HLSL modules and writes the version 4 pack.
dxil_build :: proc(mc: ^Microcode_Set, maps: int) {
	n := len(mc.shaders)
	if n == 0 {fail("no retail shaders in the given maps")}
	t0 := time.tick_now()
	pool := Dxil_Pool{set = mc, results = make([]Dxil_Result, n)}
	threads: [dynamic]^thread.Thread
	for _ in 0 ..< max(opts.jobs, 1) {append(&threads, thread.create_and_start_with_poly_data(&pool, dxil_translate_worker))}
	for th in threads {thread.join(th); thread.destroy(th)}
	clear(&threads)
	translate_time := time.tick_since(t0)
	failed := 0
	by_model: [6]int
	for &r, i in pool.results {
		if r.error != "" {
			failed += 1
			s := &mc.shaders[i]
			fmt.eprintfln("FAIL %016x %s %s: %s", s.hash, stage_name[s.stage], len(s.sources) > 0 ? s.sources[0].name : "?", r.error)
			continue
		}
		by_model[r.meta.model] += 1
	}
	if failed > 0 {fail("%d of %d shaders did not translate (or the backends disagree); no pack written", failed, n)}

	// modules: the translated shaders deduplicated by source, which must group them as the SPIR-V modules do
	modules: [dynamic]Dxil_Module
	by_hash: map[u64][dynamic]int
	add_module :: proc(modules: ^[dynamic]Dxil_Module, by_hash: ^map[u64][dynamic]int, text, profile, entry, label: string) -> int {
		h := hash.fnv64a(transmute([]u8)text)
		if list, ok := by_hash[h]; ok {
			for mi in list {if modules[mi].text == text && modules[mi].profile == profile {return mi}}
		}
		mi := len(modules)
		append(modules, Dxil_Module{text = text, profile = profile, entry = entry, hash = h, label = label})
		if !(h in by_hash) {by_hash[h] = {}}
		append(&by_hash[h], mi)
		return mi
	}
	spv_index: map[u64][dynamic]int
	spv_codes: [dynamic][]u8
	for &r, i in pool.results {
		r.module = add_module(&modules, &by_hash, r.text, r.profile, "main", fmt.aprintf("%s %016x", r.profile, mc.shaders[i].hash))
		code := slice.to_bytes(r.spirv)
		hs := hash.fnv64a(code)
		r.spv_module = -1
		if list, ok := spv_index[hs]; ok {for mi in list {if slice.equal(spv_codes[mi], code) {r.spv_module = mi}}}
		if r.spv_module < 0 {
			r.spv_module = len(spv_codes)
			append(&spv_codes, code)
			if !(hs in spv_index) {spv_index[hs] = {}}
			append(&spv_index[hs], r.spv_module)
		}
		if r.spv_module != r.module {fail("shader %016x: HLSL module %d, SPIR-V module %d (the backends group the shaders differently)", mc.shaders[i].hash, r.module, r.spv_module)}
	}
	translated := len(modules)
	tess_vs := add_module(&modules, &by_hash, xenos.hlsl_helper_tess_vs(), "vs_6_0", "main", "tessellation VS")
	tess_hs := add_module(&modules, &by_hash, xenos.hlsl_helper_tess_hs(), "hs_6_0", "main", "tessellation HS")
	rect_masks: [dynamic]u32
	for &r in pool.results {
		if r.meta.model != .Vertex {continue}
		if !slice.contains(rect_masks[:], r.meta.vs_outputs) {append(&rect_masks, r.meta.vs_outputs)}
	}
	slice.sort(rect_masks[:])
	rect_table: [dynamic]u8
	for mask in rect_masks {
		text, err := xenos.hlsl_helper_rect_gs(mask)
		if err != "" {fail("rectangle-list GS %08x: %s", mask, err)}
		put32(&rect_table, mask)
		put32(&rect_table, u32(add_module(&modules, &by_hash, text, "gs_6_0", "main", fmt.aprintf("rectangle-list GS %08x", mask))))
	}
	point_gs := add_module(&modules, &by_hash, xenos.hlsl_helper_point_gs(), "gs_6_0", "main", "point-sprite GS")
	root_sig := add_module(&modules, &by_hash, xenos.hlsl_root_signature(), "rootsig_1_1", "XE_ROOT_SIGNATURE", "root signature")
	fmt.printfln("shaderpack --dxil: %d shaders translated by both backends in %v (metadata and module grouping agree): %d translated modules + %d helpers", n, translate_time, translated, len(modules) - translated)
	for &r in pool.results {delete(r.spirv)}

	// compile
	work := opts.dxil_work
	for sub in ([]string{"hlsl", "dxil", "cache"}) {
		d := fmt.tprintf("%s/%s", work, sub)
		if !os.exists(d) {_ = os.make_directory_all(d)}
		if !os.exists(d) {fail("cannot create %s", d)}
	}
	cpool := Dxil_Compile_Pool{modules = modules[:], work = work, dxc_version = cas_tool_version(opts.dxc)}
	t1 := time.tick_now()
	for _ in 0 ..< max(opts.jobs, 1) {append(&threads, thread.create_and_start_with_poly_data(&cpool, dxil_compile_worker))}
	for th in threads {thread.join(th); thread.destroy(th)}
	clear(&threads)
	fmt.println()
	compile_time := time.tick_since(t1)
	cfailed, cached, profiles := 0, 0, map[string]int{}
	for &m, mi in modules {
		if m.error == "" {m.error = dxil_check_container(&m)}
		if m.error != "" {
			cfailed += 1
			fmt.eprintfln("DXC FAIL module %d (%s, %s, %s/hlsl/%d.hlsl): %s", mi, m.label, m.profile, work, mi, m.error)
			continue
		}
		if m.cached {cached += 1}
		profiles[m.profile] += 1
	}
	fmt.printfln("shaderpack --dxil: %d / %d modules compiled clean by %s (%d from the cache) in %v on %d workers", len(modules) - cfailed, len(modules), opts.dxc, cached, compile_time, max(opts.jobs, 1))
	if cfailed > 0 {fail("%d modules did not compile; no pack written", cfailed)}

	// compress (zlib level 9, as version 3)
	Compress_Pool :: struct {
		modules: []Dxil_Module,
		next:    int,
	}
	compress_worker :: proc(pool: ^Compress_Pool) {
		for {
			i := sync.atomic_add(&pool.next, 1)
			if i >= len(pool.modules) {return}
			m := &pool.modules[i]
			bound := zlib.compressBound(zlib.uLong(len(m.code)))
			out := make([]u8, int(bound))
			out_len := zlib.uLongf(bound)
			if zlib.compress2(raw_data(out), &out_len, raw_data(m.code), zlib.uLong(len(m.code)), 9) != zlib.OK {fail("zlib compress2 failed")}
			m.compressed = out[:int(out_len)]
		}
	}
	cp := Compress_Pool{modules = modules[:]}
	for _ in 0 ..< max(opts.jobs, 1) {append(&threads, thread.create_and_start_with_poly_data(&cp, compress_worker))}
	for th in threads {thread.join(th); thread.destroy(th)}

	// the xenos table, by (hash_hi, hash_lo, stage), exactly as version 3
	order := make([]int, n)
	for i in 0 ..< n {order[i] = i}
	context.user_ptr = mc
	slice.sort_by(order, proc(a, b: int) -> bool {
		set := (^Microcode_Set)(context.user_ptr)
		x, y := &set.shaders[a], &set.shaders[b]
		if x.hash >> 32 != y.hash >> 32 {return x.hash >> 32 < y.hash >> 32}
		if u32(x.hash) != u32(y.hash) {return u32(x.hash) < u32(y.hash)}
		return x.stage < y.stage
	})
	st: Strings
	intern(&st, "")
	xtable, textures, elements, defs, streams: [dynamic]u8
	ntex, nel, ndef, nstream: u32
	for i in order {
		s := &mc.shaders[i]
		r := &pool.results[i]
		m := &r.meta
		flags: u32
		if m.kills {flags |= XENOS_FLAG_KILLS}
		if m.memexport {flags |= XENOS_FLAG_MEMEXPORT}
		if m.depth_output {flags |= XENOS_FLAG_DEPTH}
		if m.labels {flags |= XENOS_FLAG_LABELS}
		if m.point_size {flags |= XENOS_FLAG_POINT_SIZE}
		if m.uses_bda {flags |= XENOS_FLAG_BDA}
		put32(&xtable, u32(s.hash)); put32(&xtable, u32(s.hash >> 32))
		put32(&xtable, u32(r.module))
		append(&xtable, u8(s.stage), u8(m.model), m.param_gen < 0 ? 0xff : u8(m.param_gen), u8(m.color_outputs))
		put32(&xtable, flags)
		put32(&xtable, m.vs_outputs)
		put32(&xtable, m.ps_inputs)
		put32(&xtable, u32(s.size))
		put32(&xtable, intern(&st, len(s.sources) > 0 ? s.sources[0].name : ""))
		put32(&xtable, ntex)
		put16(&xtable, u16(len(m.textures))); put16(&xtable, u16(len(m.elements)))
		put32(&xtable, nel)
		put32(&xtable, ndef)
		put16(&xtable, u16(len(m.defs))); put16(&xtable, u16(len(m.memexport_streams)))
		put32(&xtable, nstream)
		for u in m.textures {append(&textures, u8(u.fetch), u8(u.dim), u8(u.sampler), 0); ntex += 1}
		for e in m.elements {append(&elements, u8(e.usage), u8(e.index), u8(e.group), 0); nel += 1}
		for d in m.defs {
			put32(&defs, d.reg)
			for v in d.value {put32(&defs, v)}
			ndef += 1
		}
		for reg in m.memexport_streams {put32(&streams, reg); nstream += 1}
	}
	align4(&st.data)
	module_table, blob: [dynamic]u8
	blob_off: u32 = 0
	unique_raw := 0
	for &m in modules {
		put32(&module_table, blob_off)
		put32(&module_table, u32(len(m.compressed)))
		put32(&module_table, u32(len(m.code)))
		put32(&module_table, hash.crc32(m.code))
		h := hash.fnv64a(m.code)
		put32(&module_table, u32(h))
		put32(&module_table, u32(h >> 32))
		append(&blob, ..m.compressed)
		blob_off += u32(len(m.compressed))
		unique_raw += len(m.code)
	}
	file: [dynamic]u8
	resize(&file, DXIL_HEADER_SIZE)
	module_offset := u32(len(file)); append(&file, ..module_table[:])
	xenos_offset := u32(len(file)); append(&file, ..xtable[:])
	texture_offset := u32(len(file)); append(&file, ..textures[:])
	element_offset := u32(len(file)); append(&file, ..elements[:])
	def_offset := u32(len(file)); append(&file, ..defs[:])
	stream_offset := u32(len(file)); append(&file, ..streams[:])
	rect_offset := u32(len(file)); append(&file, ..rect_table[:])
	string_offset := u32(len(file)); append(&file, ..st.data[:])
	align4(&file)
	blob_offset := u32(len(file)); append(&file, ..blob[:])
	header: [dynamic]u8
	for v in ([?]u32{0x50533348, DXIL_PACK_VERSION, DXIL_HEADER_SIZE, u32(len(file)), xenos.ABI_VERSION, u32(len(modules)), module_offset, u32(n), xenos_offset, ntex, texture_offset, nel, element_offset, ndef, def_offset, nstream, stream_offset, u32(len(st.data)), string_offset, u32(len(blob)), blob_offset}) {put32(&header, v)}
	put32(&header, hash.crc32(file[DXIL_HEADER_SIZE:blob_offset]))
	put32(&header, u32(tess_vs)); put32(&header, u32(tess_hs))
	put32(&header, u32(len(rect_masks))); put32(&header, rect_offset)
	put32(&header, DXIL_FORMAT)
	put32(&header, u32(point_gs))
	put32(&header, u32(root_sig))
	if len(header) != DXIL_HEADER_SIZE {fail("internal: version 4 header is %d bytes", len(header))}
	copy(file[:DXIL_HEADER_SIZE], header[:])
	if os.write_entire_file(opts.out, file[:]) != nil {fail("cannot write %s", opts.out)}

	// readable manifests: the shaders (as version 3, + the DXC profile) and the modules
	mb := strings.builder_make()
	fmt.sbprintln(&mb, "# microcode_fnv64\tstage\tmodel\tmodule\tflags\tparam_gen\tcolor_out\tvs_out\tps_in\ttextures\telements\tdefs\tmemexport_streams\tprofile\tsource")
	models := [6]string{"vertex", "", "tess_eval", "", "fragment", "compute"}
	dim_names := [4]string{"2d", "3d", "cube", "stacked"}
	for i in order {
		s := &mc.shaders[i]
		r := &pool.results[i]
		m := &r.meta
		fmt.sbprintf(&mb, "%016x\t%s\t%s\t%d\t", s.hash, stage_name[s.stage], models[m.model], r.module)
		fmt.sbprintf(&mb, "%s%s%s%s%s\t%d\t%x\t%x\t%x\t", m.kills ? "k" : "", m.memexport ? "m" : "", m.depth_output ? "d" : "", m.labels ? "l" : "", m.point_size ? "p" : "", m.param_gen, m.color_outputs, m.vs_outputs, m.ps_inputs)
		for u, k in m.textures {fmt.sbprintf(&mb, "%stf%d:%s/%d", k > 0 ? "," : "", u.fetch, dim_names[u.dim & 3], u.sampler)}
		fmt.sbprint(&mb, "\t")
		for e, k in m.elements {fmt.sbprintf(&mb, "%s%v%d", k > 0 ? "," : "", e.usage, e.index)}
		fmt.sbprintf(&mb, "\t%d\t", len(m.defs))
		for reg, k in m.memexport_streams {fmt.sbprintf(&mb, "%sc%d", k > 0 ? "," : "", reg)}
		fmt.sbprintfln(&mb, "\t%s\t%s", r.profile, len(s.sources) > 0 ? s.sources[0].name : "")
	}
	write_file(strings.concatenate({opts.out, ".tsv"}), strings.to_string(mb))
	mm := strings.builder_make()
	fmt.sbprintln(&mm, "# module\tprofile\thlsl_bytes\tdxil_bytes\tcompressed\tcached\tlabel")
	for &m, mi in modules {fmt.sbprintfln(&mm, "%d\t%s\t%d\t%d\t%d\t%v\t%s", mi, m.profile, len(m.text), len(m.code), len(m.compressed), m.cached, m.label)}
	write_file(strings.concatenate({opts.out, ".modules.tsv"}), strings.to_string(mm))

	stages: [2]int
	for &s in mc.shaders {stages[min(int(s.stage), 1)] += 1}
	fmt.printfln("shaderpack --dxil: %d maps: %d unique retail shaders (vs %d, ps %d): vertex %d, tessellation evaluation (domain) %d, fragment %d, compute %d", maps, n, stages[0], stages[1], by_model[0], by_model[2], by_model[4], by_model[5])
	keys: [dynamic]string
	for k in profiles {append(&keys, k)}
	slice.sort(keys[:])
	pb := strings.builder_make()
	for k in keys {fmt.sbprintf(&pb, " %s %d", k, profiles[k])}
	fmt.printfln("shaderpack --dxil: modules by profile:%s", strings.to_string(pb))
	fmt.printfln("shaderpack --dxil: %s: %d modules (%d translated + tess VS/HS + %d rectangle-list GS + point GS + root signature), DXIL %.1f MB, %.2f MB compressed; pack %.2f MB (version %d, ABI %d)", opts.out, len(modules), translated, len(rect_masks), f64(unique_raw) / 1e6, f64(len(blob)) / 1e6, f64(len(file)) / 1e6, DXIL_PACK_VERSION, xenos.ABI_VERSION)
}
