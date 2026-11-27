// port/tools/datagen: retail data and functions the game refers to but no unit defines (GAME-CHANGES B5, plan
// criterion P3). `survey` sorts data stubs (below); `functions` sorts function stubs (see functions_survey).
// For data, the Xbox build copies the bytes from the retail image; the PC build needs them defined in source. This
// tool reads the retail image and sorts the data by what defining it in source takes.
//
// usage: datagen survey NAMES.txt RETAIL.pe REPO > survey.tsv
//   NAMES.txt  data symbols to survey, one raw linker name per line (`_name` for C, `?name@@3...` for C++)
//   RETAIL.pe  local/reference/default.pe: the loaded image, indexed by RVA (its section headers' raw offsets do not
//              apply)
//   REPO       repository root: addresses from every `// @address 0xRVA name` line in tu/ and from
//              metadata/matching/data-owners.tsv (offset-0 rows)
//
// Columns: name, rva, section, size (to the next known address, at most 64 KiB), nonzero bytes, image-pointer words
// (big-endian words inside the image), pointer targets by kind (function, string, vtable, data, other), class.
// Classes: zero (retail holds zeros: the zero stub is right), text or wtext (a narrow or wide character array), scalar
// (other data with no pointers), strings (pointers to strings
// only, plus scalars), pointers (other pointers), vtable (the first word points at a vtable: an object of a class
// with virtual functions), unknown (no retail address).
package main

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

IMAGE_BASE :: 0x82000000
IMAGE_END :: 0x83200000
RDATA_LO :: 0x400
RDATA_HI :: 0x5d880
TEXT_LO :: 0x80000
TEXT_HI :: 0x720000 // .text, BINK
DATA_LO :: 0x720000
DATA_INIT_HI :: 0x7c8a00

Known :: struct {
	rva:  u32,
	name: string,
}

main :: proc() {
	if len(os.args) == 6 && os.args[1] == "functions" {
		functions_survey(os.args[2], os.args[3], os.args[4], os.args[5])
		return
	}
	if len(os.args) != 5 || os.args[1] != "survey" {
		fmt.eprintln("usage: datagen survey NAMES.txt RETAIL.pe REPO > survey.tsv\n       datagen functions NAMES.txt JUDGE_BUILD RETAIL_NAMES.tsv REPO > function-gaps.tsv")
		os.exit(2)
	}
	names := read_lines(os.args[2])
	image, err := os.read_entire_file(os.args[3], context.allocator)
	if err != nil {
		fmt.eprintln("datagen: cannot read", os.args[3])
		os.exit(1)
	}
	repo := os.args[4]

	// Known addresses: name -> rva, and every rva that starts something (for sizes and target kinds).
	rva_of := make(map[string]u32)
	name_at := make(map[u32]string)
	starts: [dynamic]u32
	add := proc(rva_of: ^map[string]u32, name_at: ^map[u32]string, starts: ^[dynamic]u32, name: string, rva: u32) {
		if name not_in rva_of^ do rva_of^[name] = rva
		if rva not_in name_at^ do name_at^[rva] = name
		append(starts, rva)
	}
	for line, i in read_lines(strings.concatenate({repo, "/metadata/matching/data-owners.tsv"})) {
		if i == 0 do continue
		cols := strings.split(line, "\t")
		if len(cols) < 7 || cols[5] != "0" do continue
		va, ok := strconv.parse_u64(strings.trim_prefix(cols[0], "0x"), 16)
		if ok && va >= IMAGE_BASE do add(&rva_of, &name_at, &starts, cols[4], u32(va - IMAGE_BASE))
	}
	units := list_units(strings.concatenate({repo, "/tu"}))
	for unit in units {
		data, rerr := os.read_entire_file(unit, context.allocator)
		if rerr != nil do continue
		for line in strings.split_lines(string(data)) {
			rest := strings.trim_space(line)
			if !strings.has_prefix(rest, "// @address 0x") do continue
			fields := strings.fields(rest)
			if len(fields) < 4 do continue
			rva, ok := strconv.parse_u64(strings.trim_prefix(fields[2], "0x"), 16)
			if !ok do continue
			if rva >= IMAGE_BASE do rva -= IMAGE_BASE
			add(&rva_of, &name_at, &starts, fields[3], u32(rva))
		}
	}
	slice.sort(starts[:])

	// A vtable: a data-owners name `??_7...`.
	is_vtable := make(map[u32]bool)
	for rva, name in name_at do if strings.has_prefix(name, "??_7") do is_vtable[rva] = true

	b := strings.builder_make()
	fmt.sbprintln(&b, "name\trva\tsection\tsize\tnonzero\tpointers\tfunction\tstring\tvtable\tdata\tother\tclass")
	counts := make(map[string]int)
	for raw in names {
		rva, found := lookup(raw, rva_of)
		if !found {
			fmt.sbprintf(&b, "%s\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\tunknown\n", raw)
			counts["unknown"] += 1
			continue
		}
		size := next_start(starts[:], rva) - rva
		size = min(size, 0x10000)
		section := section_of(rva)
		nonzero, pointers := 0, 0
		kinds: [5]int // function, string, vtable, data, other
		first_vtable := false
		end := min(int(rva + size), len(image))
		for k := int(rva); k < end; k += 1 do if image[k] != 0 do nonzero += 1
		for k := int(rva); k + 4 <= end; k += 4 {
			word := u32(image[k]) << 24 | u32(image[k + 1]) << 16 | u32(image[k + 2]) << 8 | u32(image[k + 3])
			if word < IMAGE_BASE || word >= IMAGE_END do continue
			pointers += 1
			target := word - IMAGE_BASE
			switch {
			case target >= TEXT_LO && target < TEXT_HI:
				kinds[0] += 1
			case is_vtable[target]:
				kinds[2] += 1
				if k == int(rva) do first_vtable = true
			case target in name_at:
				kinds[3] += 1
			case target >= RDATA_LO && target < RDATA_HI && looks_like_string(image, int(target)):
				kinds[1] += 1
			case:
				kinds[4] += 1
			}
		}
		class := "scalar"
		switch {
		case nonzero == 0:
			class = "zero"
		case first_vtable:
			class = "vtable"
		case pointers == 0 && is_text(image[rva:end], false):
			class = "text"
		case pointers == 0 && is_text(image[rva:end], true):
			class = "wtext"
		case pointers == 0:
			class = "scalar"
		case kinds[1] == pointers:
			class = "strings"
		case:
			class = "pointers"
		}
		counts[class] += 1
		fmt.sbprintf(&b, "%s\t0x%x\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n", raw, rva, section, size, nonzero, pointers,
			kinds[0], kinds[1], kinds[2], kinds[3], kinds[4], class)
	}
	os.write_string(os.stdout, strings.to_string(b))
	keys: [dynamic]string
	for k in counts do append(&keys, k)
	slice.sort(keys[:])
	for k in keys do fmt.eprintf("datagen: %s %d\n", k, counts[k])
}

// The rva a raw linker name was given: by its own spelling, its C spelling without the COFF underscore, or, for a
// C++ name, the identifier its `// @address` line uses.
lookup :: proc(raw: string, rva_of: map[string]u32) -> (u32, bool) {
	if rva, ok := rva_of[raw]; ok do return rva, true
	if strings.has_prefix(raw, "_") {
		if rva, ok := rva_of[raw[1:]]; ok do return rva, true
	}
	if strings.has_prefix(raw, "?") {
		if at := strings.index(raw, "@@"); at > 1 {
			if rva, ok := rva_of[raw[1:at]]; ok do return rva, true
		}
	}
	return 0, false
}

next_start :: proc(starts: []u32, rva: u32) -> u32 {
	lo, hi := 0, len(starts)
	for lo < hi {
		mid := (lo + hi) / 2
		if starts[mid] <= rva do lo = mid + 1
		else do hi = mid
	}
	return lo < len(starts) ? starts[lo] : rva + 4096
}

section_of :: proc(rva: u32) -> string {
	switch {
	case rva >= RDATA_LO && rva < RDATA_HI:
		return ".rdata"
	case rva >= TEXT_LO && rva < TEXT_HI:
		return "code"
	case rva >= DATA_LO && rva < DATA_INIT_HI:
		return ".data"
	case rva >= DATA_INIT_HI && rva < 0x11db518:
		return ".bss"
	}
	return "other"
}

// A NUL-terminated run of printable bytes (at least one).
looks_like_string :: proc(image: []u8, at: int) -> bool {
	n := 0
	for k := at; k < len(image) && n < 512; k += 1 {
		c := image[k]
		if c == 0 do return n > 0
		if c < 0x09 || (c > 0x0d && c < 0x20) || c == 0x7f do return false
		n += 1
	}
	return false
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

read_lines :: proc(path: string) -> []string {
	data, err := os.read_entire_file(path, context.allocator)
	if err != nil {
		fmt.eprintln("datagen: cannot read", path)
		os.exit(1)
	}
	lines: [dynamic]string
	for line in strings.split_lines(string(data)) {
		trimmed := strings.trim_right(line, "\r ")
		if len(trimmed) > 0 do append(&lines, trimmed)
	}
	return lines[:]
}

// A character array: printable text up to a NUL, then only NULs (narrow), or the same in big-endian UTF-16 (wide).
is_text :: proc(bytes: []u8, wide: bool) -> bool {
	step := wide ? 2 : 1
	n, k := 0, 0
	for ; k + step <= len(bytes); k += step {
		c := wide ? u32(bytes[k]) << 8 | u32(bytes[k + 1]) : u32(bytes[k])
		if c == 0 do break
		if c < 0x09 || (c > 0x0d && c < 0x20) || c == 0x7f || (!wide && c >= 0x80 && c < 0xa0) do return false
		n += 1
	}
	if n == 0 || k + step > len(bytes) do return false
	for ; k < len(bytes); k += 1 do if bytes[k] != 0 do return false
	return true
}

// datagen functions NAMES.txt JUDGE_BUILD RETAIL_NAMES.tsv REPO > function-gaps.tsv
//   NAMES.txt         function stubs, one name per line (stubgen's manifest spelling)
//   JUDGE_BUILD       a judge run's build directory: link.tsv (every function the image compiles from tu/, by
//                     retail address) and placed-alternates.tsv (caller names the Xbox build binds by address)
//   RETAIL_NAMES.tsv  local/debugnames/retail-names.tsv (dev-map names, demangled names and ICF aliases by rva)
//   REPO              metadata/matching/library-functions.tsv and the `// @address` lines in tu/
// Columns: name, rva, class, defined_as (split: the symbol tu/ compiles at that address), object (library code: the
// retail object file). Classes:
//   folded     retail folded the function into another with identical code (ICF), which tu/ compiles: the source
//              must define this one too (the same body);
//   split      tu/ compiles the function under another name: the caller's name or signature differs from the
//              definition's (the fix is in source: one name);
//   generated  compiler-generated retail code (a constructor, an initializer, a vector iterator): the source must
//              give the compiler what it needs to generate it (a real class, a real object);
//   library    prebuilt SDK or middleware code the Xbox build links as is: the port implements it;
//   virtual    no retail address: a virtual method a compiled vtable refers to, which the decomp defines as a free
//              function or not at all (the fix is in source: a real member function);
//   unknown    no retail address and not virtual.
functions_survey :: proc(names_path, judge, retail_names_path, repo: string) {
	names := read_lines(names_path)
	placed := make(map[string]string) // rva -> symbol compiled from tu/
	rva_of := make(map[string]string) // any known spelling -> rva
	folded_rva := make(map[string]string) // names retail folded into another function (identical code, ICF) -> rva
	for line, i in read_lines(strings.concatenate({judge, "/link.tsv"})) {
		cols := strings.split(line, "\t")
		if i > 0 && len(cols) >= 2 do placed[cols[0]] = cols[1]
	}
	for line, i in read_lines(strings.concatenate({judge, "/placed-alternates.tsv"})) {
		cols := strings.split(line, "\t")
		if i > 0 && len(cols) >= 2 && cols[1] not_in rva_of do rva_of[cols[1]] = cols[0]
	}
	for line, i in read_lines(retail_names_path) {
		cols := strings.split(line, "\t")
		if i == 0 || len(cols) < 8 do continue
		for spelling in ([]string{cols[6], cols[7]}) do if len(spelling) > 0 && spelling not_in rva_of do rva_of[spelling] = cols[0]
		if len(cols) >= 11 {
			for alias in strings.fields(strings.join(strings.split(cols[10], ","), " ")) do if alias not_in folded_rva do folded_rva[alias] = cols[0]
		}
	}
	for unit in list_units(strings.concatenate({repo, "/tu"})) {
		data, err := os.read_entire_file(unit, context.allocator)
		if err != nil do continue
		for line in strings.split_lines(string(data)) {
			rest := strings.trim_space(line)
			if !strings.has_prefix(rest, "// @address 0x") do continue
			fields := strings.fields(rest)
			if len(fields) < 4 do continue
			rva, ok := strconv.parse_u64(strings.trim_prefix(fields[2], "0x"), 16)
			if !ok do continue
			if rva >= IMAGE_BASE do rva -= IMAGE_BASE
			if fields[3] not_in rva_of do rva_of[fields[3]] = fmt.aprintf("0x%x", rva)
		}
	}
	library := make(map[string][2]string) // rva -> state, object
	for line, i in read_lines(strings.concatenate({repo, "/metadata/matching/library-functions.tsv"})) {
		cols := strings.split(line, "\t")
		if i > 0 && len(cols) >= 3 do library[cols[0]] = {cols[1], cols[2]}
	}

	b := strings.builder_make()
	fmt.sbprintln(&b, "name\trva\tclass\tdefined_as\tobject")
	counts := make(map[string]int)
	for name in names {
		rva, found := rva_of[name]
		if !found {
			if scoped, ok := ms_scoped_name(name); ok do rva, found = rva_of[scoped]
		}
		folded := false
		if !found {
			rva, found = folded_rva[name]
			if !found {
				if scoped, ok := ms_scoped_name(name); ok do rva, found = folded_rva[scoped]
			}
			folded = found
		}
		class, defined_as, object := "unknown", "-", "-"
		switch {
		case !found:
			rva = "-"
			if ms_is_virtual(name) do class = "virtual"
		case rva in placed:
			class, defined_as = folded ? "folded" : "split", placed[rva]
		case rva in library:
			entry := library[rva]
			class, object = entry[0] == "generated" ? "generated" : "library", entry[1]
		}
		counts[class] += 1
		fmt.sbprintf(&b, "%s\t%s\t%s\t%s\t%s\n", name, rva, class, defined_as, object)
	}
	os.write_string(os.stdout, strings.to_string(b))
	keys: [dynamic]string
	for k in counts do append(&keys, k)
	slice.sort(keys[:])
	for k in keys do fmt.eprintf("datagen: %s %d\n", k, counts[k])
}

// `?name@scope@outer@@...` -> `outer::scope::name`, the demangled spelling retail-names.tsv uses: retail mangles
// members with the PowerPC convention (QAA) where clang's x86 build mangles thiscall (QAE), so the mangled spellings
// differ while the scoped name does not.
ms_scoped_name :: proc(name: string) -> (string, bool) {
	if !strings.has_prefix(name, "?") || strings.has_prefix(name, "??") do return "", false
	end := strings.index(name, "@@")
	if end < 0 do return "", false
	parts := strings.split(name[1:end], "@")
	slice.reverse(parts)
	return strings.join(parts, "::"), true
}

// MS mangling: the function-class letter after `@@` marks a virtual member (E/F private, M/N protected, U/V public).
ms_is_virtual :: proc(name: string) -> bool {
	if !strings.has_prefix(name, "?") do return false
	at := strings.index(name, "@@")
	if at < 0 || at + 2 >= len(name) do return false
	switch name[at + 2] {
	case 'E', 'F', 'M', 'N', 'U', 'V':
		return true
	}
	return false
}
