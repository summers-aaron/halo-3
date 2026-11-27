// port/tools/stubgen: stage-1 stubs. Writes one assembly file (GNU syntax; COFF or ELF) that defines every symbol
// the host link leaves undefined, so the binary links before port/sdk implements the SDK and before the game's
// missing definitions land:
//   function  pushes its own name and calls port_stub_called (port/sdk/runtime), which reports it and stops;
//   data      zeroed storage in .bss, sized from metadata/matching/data-owners.tsv (gap to the next owned address,
//             at most 64 KiB), else 4 KiB or the retail survey's size (RETAIL_DATA.tsv) if larger;
//   import    COFF only: `__imp_X`, the import slot through which code reads what the game declares
//             __declspec(dllimport) (the Xbox kernel's exported data: XboxKrnlVersion, KeDebugMonitorData). ELF drops
//             the attribute and binds the reference to X itself, so on both hosts X is the stub (by the rules here;
//             data when its C name says nothing else) and the COFF slot holds X's address. A zeroed slot would make the
//             first read through it a fault at address 0.
// A symbol is a function when the MS mangling says so, or, for C names, when any relocation to it is a call
// (IMAGE_REL_I386_REL32, R_386_PC32, R_386_PLT32) or when no game object refers to it (a library pulled it in);
// otherwise it is data.
//
// usage: stubgen coff|elf UNDEFINED.txt RELOCS.txt DATA_OWNERS.tsv [DEFINED.txt [ALIASES.ld [MANIFEST.tsv [RETAIL_DATA.tsv]]]] > stubs.s
//   UNDEFINED.txt  one raw symbol name per line (as the linker reports it with demangling off)
//   RELOCS.txt     `llvm-objdump -r` over the objects
//   DEFINED.txt    external symbols the objects define, one per line (`llvm-nm --defined-only --extern-only`). A data
//                  name becomes an alternate name for a definition when its datum has exactly one global C++
//                  definition, or when it names the same retail address (data-owners.tsv, `// @address` lines in tu/)
//                  as exactly one defined datum; otherwise it is a stub.
//   ALIASES.ld     ELF only: the alternate names, as linker-script assignments (`"name" = "definition";`). COFF
//                  puts them in the stubs' .drectve section (/alternatename) instead.
//   MANIFEST.tsv   one row per stub: kind (function, quiet, data, poisoned, constant, alias, tls, import) and the name in its
//                  MS spelling, without COFF's C underscore (the input of port/tools/datagen).
//   RETAIL_DATA.tsv  `datagen survey` of this link's data stubs (the manifest's `data` rows): which have retail
//                  values. Without it nothing is poisoned, so port/build/link-stage1.sh runs stubgen once for the
//                  manifest, surveys it, and runs it again with the survey. A name the survey places at a retail
//                  code address becomes a function stub: the game only takes its address (atexit, callbacks).
// ELF objects name MS-mangled symbols with `!` for `@` (port/build/elf-names.sh): the analysis reads `!` as `@`, the
// assembly keeps the linker's spelling, and a stub reports the MS spelling.
//
// Retail data the game has not defined in source yet must not read as zeros without anyone noticing. A datum whose
// retail bytes are not all zero (RETAIL_DATA.tsv) is "poisoned": it gets pages of its own and a row in
// port_poison_table, and the runtime protects those pages before the static constructors run, so the first access
// stops the run and names the datum, as a function stub does (port/sdk/runtime/stub_report.c). The survey is made
// from the tree being linked: a datum the game renames is surveyed under its new name.
//
// usage: stubgen forward FORWARD.txt > forward.s
//   Windows host only. FORWARD.txt lists Win32 functions as `Name@bytes` (the stdcall decoration), one per line;
//   `#` starts a comment. For each, a cdecl `_Name` (how the game calls the Xbox API, without __stdcall) that
//   pushes its arguments again and calls the host's `__imp__Name@bytes`. Only for calls whose Xbox and Win32 forms
//   mean the same thing.
package main

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

Kind :: enum { Function, Data }

main :: proc() {
	if len(os.args) == 3 && os.args[1] == "forward" {
		forward(os.args[2])
		return
	}
	if len(os.args) < 5 || len(os.args) > 9 ||
	   (os.args[1] != "coff" && os.args[1] != "elf" && os.args[1] != "coff64" && os.args[1] != "elf64") {
		fmt.eprintln("usage: stubgen coff|elf|coff64|elf64 UNDEFINED.txt RELOCS.txt DATA_OWNERS.tsv [DEFINED.txt [ALIASES.ld [MANIFEST.tsv [RETAIL_DATA.tsv]]]] > stubs.s")
		os.exit(2)
	}
	is64 := os.args[1] == "coff64" || os.args[1] == "elf64"
	coff := os.args[1] == "coff" || os.args[1] == "coff64"
	have_defined := len(os.args) >= 6
	undefined := read_lines(os.args[2])
	wanted := make(map[string]bool)
	for name in undefined do wanted[name] = true

	// COFF import slots (`__imp_X`, see "import" above): X is what the slot points at, a stub of its own unless the
	// objects define it or the link already wants it under that name.
	imported := make(map[string]bool)
	if coff {
		defined_names := make(map[string]bool)
		if have_defined do for name in read_lines(os.args[5]) do defined_names[name] = true
		targets: [dynamic]string
		for name in undefined {
			target := strings.trim_prefix(name, "__imp_")
			if target == name do continue
			imported[target] = true
			if wanted[target] || defined_names[target] do continue
			wanted[target] = true
			append(&targets, target)
		}
		undefined = slice.concatenate([][]string{undefined, targets[:]})
	}

	// Relocation kinds per wanted symbol: a call anywhere makes it a function.
	// A name no game object refers to was pulled in by a library archive member, which calls it.
	// Thread-local data is reached from code by its offset in the TLS block (COFF SECREL from a non-debug section;
	// ELF TLS relocations), so its stub goes in the TLS section; debug info uses SECREL for every global.
	called := make(map[string]bool)
	referenced := make(map[string]bool)
	thread_local := make(map[string]bool)
	debug_section := false
	for line in read_lines(os.args[3]) {
		if strings.has_prefix(line, "RELOCATION RECORDS FOR [") {
			debug_section = strings.contains(line, "[.debug")
			continue
		}
		fields := strings.fields(line)
		if len(fields) < 3 do continue
		sym := fields[2]
		if !coff {
			// ELF RELA relocations print `symbol+0x...` or `symbol-0x...`; strip the addend.
			if idx := strings.last_index_any(sym, "+-"); idx > 0 && strings.has_prefix(sym[idx + 1:], "0x") {
				sym = sym[:idx]
			}
		}
		if !wanted[sym] do continue
		referenced[sym] = true
		switch fields[1] {
		case "IMAGE_REL_I386_REL32", "R_386_PC32", "R_386_PLT32",
		     "IMAGE_REL_AMD64_REL32", "R_X86_64_PLT32":
			called[sym] = true
		case "IMAGE_REL_I386_SECREL", "IMAGE_REL_AMD64_SECREL":
			if !debug_section do thread_local[sym] = true
		case "R_386_TLS_LE", "R_386_TLS_LE_32", "R_386_TLS_IE", "R_386_TLS_GOTIE", "R_386_TLS_GD", "R_386_TLS_LDM",
		     "R_386_TLS_LDO_32", "R_386_TLS_GOTDESC",
		     "R_X86_64_TPOFF32", "R_X86_64_GOTTPOFF", "R_X86_64_TLSGD", "R_X86_64_TLSLD", "R_X86_64_DTPOFF32":
			thread_local[sym] = true
		}
	}

	// Data sizes: owned addresses in order; a datum runs to the next owned address. Only a row at offset 0 starts a
	// datum: the other rows of a name are references into it (a member of a global struct, an array element), and
	// ending the datum at one of them made it as small as the gap after its last referenced member.
	Owner :: struct { va: u64, name: string }
	owners: [dynamic]Owner
	for line, i in read_lines(os.args[4]) {
		if i == 0 do continue
		cols := strings.split(line, "\t")
		if len(cols) < 5 do continue
		if len(cols) >= 6 && cols[5] != "0" do continue
		va, ok := strconv.parse_u64(strings.trim_prefix(cols[0], "0x"), 16)
		if ok do append(&owners, Owner{va, cols[4]})
	}
	slice.sort_by(owners[:], proc(a, b: Owner) -> bool { return a.va < b.va })
	size_by_name := make(map[string]u64)
	size_by_va := make(map[u64]u64)
	for o, i in owners {
		size: u64 = 4096
		if i + 1 < len(owners) && owners[i + 1].va > o.va do size = min(owners[i + 1].va - o.va, 0x10000)
		size_by_name[o.name] = size
		size_by_va[o.va] = size
	}

	// One datum, two spellings: a unit declares it `extern "C"` (`_name` in COFF) while its owner defines the C++ name
	// (`?name@@3<type>`). The Xbox build binds both spellings to the retail address; here the C name becomes an
	// alternate name for the one definition (GAME-CHANGES B9), instead of a stub of its own.
	cpp_by_base := make(map[string][dynamic]string)
	if have_defined {
		for name in read_lines(os.args[5]) {
			ms := ms_name(name, coff)
			if strings.has_prefix(ms, "?") {
				at := strings.index(ms, "@@3")
				if at > 1 && !strings.contains(ms[1:at], "@") {
					list := cpp_by_base[ms[1:at]]
					append(&list, name)
					cpp_by_base[ms[1:at]] = list
				}
			}
		}
	}
	c_prefix := (coff && !is64) ? "_" : ""

	// One datum, many names: units name a retail address their own way (`// @address 0xRVA name`), and the Xbox build
	// binds every name of an address to the datum there. When exactly one object defines a datum at that address, the
	// other names become alternate names for it.
	// A floating-point constant MSVC names by its bits (`__real@3f800000`, `__real@<16 hex digits>` for a double)
	// gives its value away: a datum at its address that nothing defines (ELF objects have no `__real@` symbols) is
	// that value rather than zero.
	va_of := make(map[string]u64)
	real_at := make(map[u64]string)
	for line, i in read_lines(os.args[4]) {
		if i == 0 do continue
		cols := strings.split(line, "\t")
		if len(cols) < 6 || cols[5] != "0" do continue
		va, ok := strconv.parse_u64(strings.trim_prefix(cols[0], "0x"), 16)
		if !ok do continue
		if cols[4] not_in va_of do va_of[cols[4]] = va
		if bits := strings.trim_prefix(cols[4], "__real@"); bits != cols[4] && (len(bits) == 8 || len(bits) == 16) do real_at[va] = bits
	}
	repo := strings.trim_suffix(os.args[4], "/metadata/matching/data-owners.tsv")
	for unit in list_units(strings.concatenate({repo, "/tu"})) {
		text, err := os.read_entire_file(unit, context.allocator)
		if err != nil do continue
		for line in strings.split_lines(string(text)) {
			rest := strings.trim_space(line)
			if !strings.has_prefix(rest, "// @address 0x") do continue
			fields := strings.fields(rest)
			if len(fields) < 4 do continue
			va, ok := strconv.parse_u64(strings.trim_prefix(fields[2], "0x"), 16)
			if !ok do continue
			if va < IMAGE_BASE do va += IMAGE_BASE
			if fields[3] not_in va_of do va_of[fields[3]] = va
		}
	}
	defined_at := make(map[u64][dynamic]string)
	if have_defined {
		for name in read_lines(os.args[5]) {
			va, ok := lookup_va(ms_name(name, coff), c_prefix, va_of)
			if !ok || (va >= IMAGE_BASE + 0x80000 && va < IMAGE_BASE + 0x720000) do continue // code
			list := defined_at[va]
			append(&list, name)
			defined_at[va] = list
		}
	}

	// The null graphics backend (port/sdk/d3d/null-functions.txt): functions whose C name starts with a listed prefix
	// return 0 quietly.
	quiet_prefixes: [dynamic]string
	if text, err := os.read_entire_file(strings.concatenate({repo, "/port/sdk/d3d/null-functions.txt"}), context.allocator); err == nil {
		for line in strings.split_lines(string(text)) {
			entry := strings.trim_space(line)
			if len(entry) > 0 && !strings.has_prefix(entry, "#") do append(&quiet_prefixes, entry)
		}
	}
	quiet := 0

	// Retail data with real values (RETAIL_DATA.tsv: any class but zero and unknown) is poisoned.
	retail_valued := make(map[string]bool)
	// A name at a retail code address is a function, even when the game only takes its address (an atexit handler,
	// a callback in a table) and never calls it directly.
	retail_code := make(map[string]bool)
	// The survey's size of a datum (the gap to the next named retail address): data the owners table does not know
	// get at least that much (the gap can be short of the datum when a neighbour is named, so never less than 4 KiB).
	retail_size := make(map[string]u64)
	if len(os.args) == 9 {
		text, err := os.read_entire_file(os.args[8], context.allocator)
		if err != nil {
			fmt.eprintln("stubgen: cannot read", os.args[8])
			os.exit(1)
		}
		for line, i in strings.split_lines(string(text)) {
			cols := strings.split(line, "\t")
			if i == 0 || len(cols) < 12 do continue
			if strings.trim_space(cols[2]) == "code" do retail_code[cols[0]] = true
			if n, ok := strconv.parse_u64(strings.trim_space(cols[3]), 10); ok && n > 0 do retail_size[cols[0]] = min(n, 0x10000)
			switch strings.trim_space(cols[11]) {
			case "text", "wtext", "scalar", "strings", "pointers", "vtable":
				retail_valued[cols[0]] = true
			}
		}
	}

	aliases := strings.builder_make()
	alias_count := 0
	add_alias := proc(aliases: ^strings.Builder, coff: bool, name, target: string) {
		if coff do fmt.sbprintf(aliases, " /alternatename:%s=%s", name, target)
		else do fmt.sbprintf(aliases, "\"%s\" = \"%s\";\n", name, target)
	}
	manifest := strings.builder_make()
	fmt.sbprintf(&manifest, "kind\tname\n")
	poison_table := strings.builder_make()
	handler := (coff && !is64) ? "_port_stub_called" : "port_stub_called"
	rodata := coff ? ".rdata,\"dr\"" : ".rodata"
	ptr_dir := is64 ? ".quad" : ".long"
	ptr_align := is64 ? 3 : 2
	b := strings.builder_make()
	fmt.sbprintf(&b, "# Generated by port/tools/stubgen (%s): %d stubs. Do not edit.\n", os.args[1], len(undefined))
	functions, data, constants, poisoned, imports := 0, 0, 0, 0, 0
	aliased: map[string]bool // names the link defines through aliases.ld rather than here
	for name, i in undefined {
		ms := ms_name(name, coff) // the analysis reads the MS spelling; the assembly keeps the linker's
		neutral := strings.has_prefix(ms, "?") ? ms : strings.trim_prefix(ms, c_prefix) // the manifest's spelling
		if neutral == "port_poison_table" || neutral == "port_poison_count" do continue // defined below
		if target := strings.trim_prefix(name, "__imp_"); coff && target != name {
			imports += 1
			fmt.sbprintf(&manifest, "import\t%s\n", strings.has_prefix(target, "?") ? target : strings.trim_prefix(target, c_prefix))
			fmt.sbprintf(&b, "\t.data\n\t.globl\t\"%s\"\n\t.p2align\t%d\n\"%s\":\n\t%s\t\"%s\"\n", name, ptr_align, name, ptr_dir, target)
			continue
		}
		// An import slot's target has no relocations of its own: it was not pulled in by a library.
		// On Win64 COFF, RIP-relative data references also use IMAGE_REL_AMD64_REL32; a symbol with a known
		// non-code VA in data-owners.tsv or // @address is data, not a function.
		is_known_data := false
		if va, ok := lookup_va(ms, c_prefix, va_of); ok && (va < IMAGE_BASE + 0x80000 || va >= IMAGE_BASE + 0x720000) {
			is_known_data = true
		}
		kind := classify(ms, (!is_known_data && (called[name] || (!referenced[name] && !imported[name]))) || retail_code[neutral])
		if kind == .Function {
			functions += 1
			if is_quiet(ms, c_prefix, quiet_prefixes[:]) {
				quiet += 1
				fmt.sbprintf(&manifest, "quiet\t%s\n", neutral)
				if is64 {
					fmt.sbprintf(&b, "\t.text\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\txorl\t%%eax, %%eax\n\tretq\n", name, name)
				} else {
					fmt.sbprintf(&b, "\t.text\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\txorl\t%%eax, %%eax\n\txorl\t%%edx, %%edx\n\tretl\n", name, name)
				}
				continue
			}
			fmt.sbprintf(&manifest, "function\t%s\n", neutral)
			mangled := strings.has_prefix(ms, "?")
			free_function := !mangled || strings.contains(ms, "@@YA")
			no_arg_member := mangled && !free_function && strings.has_suffix(ms, "XZ")
			can_return := is64 || free_function || no_arg_member
			if is64 {
				if coff {
					fmt.sbprintf(&b, "\t.text\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\tpushq\t%%rcx\n\tsubq\t$32, %%rsp\n\tleaq\t.Lstub_name_%d(%%rip), %%rcx\n\tmovl\t$%d, %%edx\n\tcallq\t%s\n\taddq\t$32, %%rsp\n\tpopq\t%%rcx\n", name, name, i, can_return ? 1 : 0, handler)
				} else {
					// Called from Win64 ms_abi game code; port_stub_called is SysV AMD64 on Linux.
					fmt.sbprintf(&b, "\t.text\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\tpushq\t%%rcx\n\tleaq\t.Lstub_name_%d(%%rip), %%rdi\n\tmovl\t$%d, %%esi\n\tcallq\t%s\n\tpopq\t%%rcx\n", name, name, i, can_return ? 1 : 0, handler)
				}
				fmt.sbprintf(&b, no_arg_member ? "\tmovq\t%%rcx, %%rax\n\tretq\n" : "\txorl\t%%eax, %%eax\n\tretq\n")
			} else {
				fmt.sbprintf(&b, "\t.text\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\tpushl\t%%ecx\n\tpushl\t$%d\n\tpushl\t$.Lstub_name_%d\n\tcalll\t%s\n\taddl\t$8, %%esp\n\tpopl\t%%ecx\n", name, name, can_return ? 1 : 0, i, handler)
				fmt.sbprintf(&b, no_arg_member ? "\tmovl\t%%ecx, %%eax\n\tretl\n" : "\txorl\t%%eax, %%eax\n\txorl\t%%edx, %%edx\n\tretl\n")
			}
			fmt.sbprintf(&b, "\t.section\t%s\n.Lstub_name_%d:\n\t.asciz\t\"%s\"\n", rodata, i, ms)
			continue
		}
		if target, ok := alias_target(ms, c_prefix, cpp_by_base); ok {
			add_alias(&aliases, coff, name, target)
			aliased[name] = true
			alias_count += 1
			fmt.sbprintf(&manifest, "alias\t%s\n", neutral)
			continue
		}
		if va, ok := lookup_va(ms, c_prefix, va_of); ok && !thread_local[name] {
			if list := defined_at[va]; len(list) == 1 {
				add_alias(&aliases, coff, name, list[0])
				aliased[name] = true
				alias_count += 1
				fmt.sbprintf(&manifest, "alias\t%s\n", neutral)
				continue
			}
			if bits, found := real_at[va]; found {
				fmt.sbprintf(&b, "\t.data\n\t.globl\t\"%s\"\n\t.p2align\t3\n\"%s\":\n\t%s\t0x%s\n", name, name,
					len(bits) == 8 ? ".long" : ".quad", bits)
				constants += 1
				fmt.sbprintf(&manifest, "constant\t%s\n", neutral)
				continue
			}
		}
		size, known := data_size(ms, coff, size_by_name, size_by_va)
		if n, surveyed := retail_size[neutral]; !known && surveyed do size = max(size, n)
		if thread_local[name] {
			// TLS globals are mostly pointers; every thread copies the block.
			data += 1
			fmt.sbprintf(&manifest, "tls\t%s\n", neutral)
			fmt.sbprintf(&b, "%s\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\t.zero\t%d\n",
				coff ? "\t.section\t.tls$PORT,\"dw\"\n" : "\t.section\t.tbss,\"awT\",@nobits\n", name, name, min(size, 64))
			continue
		}
		if retail_valued[neutral] {
			// Pages of its own, protected at startup: the first access names it.
			poisoned += 1
			pages := (size + 4095) / 4096 * 4096
			fmt.sbprintf(&manifest, "poisoned\t%s\n", neutral)
			fmt.sbprintf(&b, "%s\t.globl\t\"%s\"\n\t.p2align\t12\n\"%s\":\n\t.zero\t%d\n",
				coff ? "\t.section\t.bss$port_poison,\"bw\"\n" : "\t.section\t.bss.port_poison,\"aw\",@nobits\n", name, name, pages)
			fmt.sbprintf(&b, "\t.section\t%s\n.Lpoison_name_%d:\n\t.asciz\t\"%s\"\n", rodata, i, ms)
			fmt.sbprintf(&poison_table, "\t%s\t\"%s\", %d, .Lpoison_name_%d\n", ptr_dir, name, pages, i)
			continue
		}
		data += 1
		fmt.sbprintf(&manifest, "data\t%s\n", neutral)
		fmt.sbprintf(&b, "\t.bss\n\t.globl\t\"%s\"\n\t.p2align\t4\n\"%s\":\n\t.zero\t%d\n", name, name, size)
	}
	// ELF: the stubs are the game's placeholders and must never interpose on a shared library (the executable would
	// otherwise export, say, its stub `socket` to libvulkan's ICDs, which glibc's dynamic linker would bind to it).
	if !coff {
		for name in undefined {
			if aliased[name] do continue
			fmt.sbprintf(&b, "\t.hidden\t\"%s\"\n", name)
		}
	}
	// port_poison_table: { address, size, name } per poisoned datum; port_poison_count rows.
	table, count := (coff && !is64) ? "_port_poison_table" : "port_poison_table", (coff && !is64) ? "_port_poison_count" : "port_poison_count"
	fmt.sbprintf(&b, "\t.section\t%s\n\t.globl\t%s\n\t.p2align\t%d\n%s:\n%s\t%s\t0, 0, 0\n", rodata, table, ptr_align, table,
		strings.to_string(poison_table), ptr_dir)
	fmt.sbprintf(&b, "\t.globl\t%s\n\t.p2align\t%d\n%s:\n\t%s\t%d\n", count, ptr_align, count, ptr_dir, poisoned)
	if coff {
		if alias_count > 0 do fmt.sbprintf(&b, "\t.section\t.drectve,\"yn\"\n\t.ascii\t\"%s \"\n", strings.to_string(aliases))
	} else {
		fmt.sbprintf(&b, "\t.section\t.note.GNU-stack,\"\",@progbits\n")
		if len(os.args) >= 7 {
			script := strings.concatenate({"/* Generated by port/tools/stubgen: alternate names. Do not edit. */\n", strings.to_string(aliases)})
			write_file(os.args[6], script)
		}
	}
	if len(os.args) >= 8 do write_file(os.args[7], strings.to_string(manifest))
	os.write_string(os.stdout, strings.to_string(b))
	fmt.eprintf("stubgen: %d functions (%d quiet), %d data (%d poisoned), %d constants, %d alternate names\n", functions,
		quiet, data + poisoned, poisoned, constants, alias_count)
	if coff do fmt.eprintf("stubgen: %d import slots\n", imports)
}

write_file :: proc(path, text: string) {
	if err := os.write_entire_file(path, transmute([]u8)text); err != nil {
		fmt.eprintln("stubgen: cannot write", path)
		os.exit(1)
	}
}

// The MS spelling of a linker name: ELF objects spell `@` as `!` (port/build/elf-names.sh).
ms_name :: proc(name: string, coff: bool) -> string {
	if coff || !strings.contains(name, "!") do return name
	result, _ := strings.replace_all(name, "!", "@")
	return result
}

// MS mangling: after the name scope's closing "@@" (or "@" before a scope), 0-4 mark a variable, 6/7 a vftable or
// vbtable, a letter a function. C names fall back to their relocations.
classify :: proc(name: string, called: bool) -> Kind {
	if strings.has_prefix(name, "?") {
		if strings.has_prefix(name, "??_7") || strings.has_prefix(name, "??_8") do return .Data
		at := strings.index(name, "@@")
		if at >= 0 && at + 2 < len(name) {
			c := name[at + 2]
			if c >= '0' && c <= '4' do return .Data
		}
		return .Function
	}
	base := strings.trim_prefix(name, "_")
	if strings.has_prefix(base, "DAT_") do return .Data
	if strings.has_prefix(base, "FUN_") do return .Function
	return called ? .Function : .Data
}

data_size :: proc(name: string, coff: bool, by_name: map[string]u64, by_va: map[u64]u64) -> (u64, bool) {
	if size, ok := by_name[name]; ok do return size, true
	c_name := coff ? strings.trim_prefix(name, "_") : name
	if size, ok := by_name[c_name]; ok do return size, true
	if strings.has_prefix(c_name, "DAT_") {
		if va, ok := strconv.parse_u64(c_name[4:], 16); ok {
			if size, found := by_va[va]; found do return size, true
		}
	}
	return 4096, false
}

read_lines :: proc(path: string) -> []string {
	data, err := os.read_entire_file(path, context.allocator)
	if err != nil {
		fmt.eprintln("stubgen: cannot read", path)
		os.exit(1)
	}
	lines: [dynamic]string
	for line in strings.split_lines(string(data)) {
		trimmed := strings.trim_right(line, "\r ")
		if len(trimmed) > 0 do append(&lines, trimmed)
	}
	return lines[:]
}

forward :: proc(path: string) {
	b := strings.builder_make()
	fmt.sbprintf(&b, "# Generated by port/tools/stubgen forward from %s. Do not edit.\n", path)
	count := 0
	for line in read_lines(path) {
		entry := strings.trim_space(line)
		if hash := strings.index(entry, "#"); hash >= 0 do entry = strings.trim_space(entry[:hash])
		if len(entry) == 0 do continue
		at := strings.index(entry, "@")
		bytes, ok := 0, false
		if at > 0 do bytes, ok = strconv.parse_int(entry[at + 1:])
		if !ok || bytes % 4 != 0 {
			fmt.eprintln("stubgen: bad forward entry:", entry)
			os.exit(1)
		}
		name := entry[:at]
		fmt.sbprintf(&b, "\t.text\n\t.globl\t\"_%s\"\n\t.p2align\t4\n\"_%s\":\n\tpushl\t%%ebp\n\tmovl\t%%esp, %%ebp\n", name, name)
		for i := bytes / 4 - 1; i >= 0; i -= 1 do fmt.sbprintf(&b, "\tpushl\t%d(%%ebp)\n", 8 + 4 * i)
		fmt.sbprintf(&b, "\tcalll\t*\"__imp__%s@%d\"\n\tmovl\t%%ebp, %%esp\n\tpopl\t%%ebp\n\tretl\n", name, bytes)
		count += 1
	}
	os.write_string(os.stdout, strings.to_string(b))
	fmt.eprintf("stubgen: %d forwarders\n", count)
}

// The one global C++ definition (`?name@@3<type>`, no scope) of a C name's datum, when there is exactly one.
alias_target :: proc(name, c_prefix: string, cpp_by_base: map[string][dynamic]string) -> (string, bool) {
	if strings.has_prefix(name, "?") || !strings.has_prefix(name, c_prefix) do return "", false
	list, found := cpp_by_base[name[len(c_prefix):]]
	if found && len(list) == 1 do return list[0], true
	return "", false
}

IMAGE_BASE :: 0x82000000

// The retail address a linker name was given: by its own spelling, its C spelling without the COFF underscore, or,
// for a C++ name, the identifier a `// @address` line uses.
lookup_va :: proc(name, c_prefix: string, va_of: map[string]u64) -> (u64, bool) {
	if va, ok := va_of[name]; ok do return va, true
	if len(c_prefix) > 0 && strings.has_prefix(name, c_prefix) {
		if va, ok := va_of[name[len(c_prefix):]]; ok do return va, true
	}
	if strings.has_prefix(name, "?") {
		if at := strings.index(name, "@@"); at > 1 {
			if va, ok := va_of[name[1:at]]; ok do return va, true
		}
	}
	return 0, false
}

list_units :: proc(dir: string) -> []string {
	out: [dynamic]string
	libs, err := os.read_all_directory_by_path(dir, context.allocator)
	if err != nil do return out[:]
	for lib in libs {
		if lib.type != .Directory do continue
		files, ferr := os.read_all_directory_by_path(lib.fullpath, context.allocator)
		if ferr != nil do continue
		for f in files do if strings.has_suffix(f.name, ".cpp") || strings.has_suffix(f.name, ".c") do append(&out, f.fullpath)
	}
	return out[:]
}

// A C function name the null graphics backend answers quietly.
is_quiet :: proc(name, c_prefix: string, prefixes: []string) -> bool {
	if strings.has_prefix(name, "?") || !strings.has_prefix(name, c_prefix) do return false
	base := name[len(c_prefix):]
	for p in prefixes do if strings.has_prefix(base, p) do return true
	return false
}
