// tmp/probe/retailfill (scratch, never committed): define retail data the source does not define yet, for the
// diagnostic probe link only. Reads the retail image and writes i686 ELF assembly that defines each requested datum
// with its retail contents in host byte order, pointers relocated to the PC build's symbols.
//
// usage: retailfill SPEC RETAIL.pe REPO DEFINED.txt > fill.s
//   SPEC lines: `datum SYMBOL RVA LAYOUT...` (`#` comments). LAYOUT tokens, each with an optional count:
//     q  64-bit scalar (a double, an __int64: byte swapped as one value)
//     p  pointer word (0, a function, another datum of this spec (+offset), a named datum, or a string)
//     w  32-bit scalar word (byte swapped)     h  16-bit scalar      b  byte      z  zero byte
//   `name RVA SYMBOL` lines name an address for pointer resolution (a datum defined elsewhere).
//   `pool SYMBOL LO HI`: one datum SYMBOL holding every 32-bit word of the image from RVA LO to HI, byte swapped,
//   and each datum of this spec inside [LO, HI) whose layout is words only (`w`) becomes a name inside it, at its
//   retail offset, instead of a datum of its own: source that reads a retail constant pool at fixed offsets from
//   one of its names (`(&k_real_zero_storage)[418]`, GAME-CHANGES F62) then reads the retail word, however the
//   offset is spelled.
//   DEFINED.txt: the link's defined symbols (ELF spelling, `!` for `@`), to prefer names the PC build defines.
package main

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

IMAGE_BASE :: 0x82000000
TEXT_LO :: 0x80000
TEXT_HI :: 0x720000

Tok :: struct {
	k: u8,
	n: int,
	target: string, // `tN=SYMBOL`: this slot calls SYMBOL instead of the retail function (a fold of another class's member)
}

thunks: strings.Builder
thunk_count := 0

// A thiscall vtable slot with n stack-argument bytes (x86 virtual calls: `this` in ecx, callee pops). PC members
// are thiscall; a folded member taking fewer arguments, a free cdecl function f(this, ...) or a missing retail
// function gets an adapter.
thiscall_slot :: proc(target: string, n: int) -> string {
	if target == "0" do return target
	name := strings.trim(target, "\"")
	is_member := false
	for i := 0; i + 4 < len(name); i += 1 {
		if name[i] == '!' && name[i + 1] == '!' && strings.index_byte("QUAIMEC", name[i + 2]) >= 0 && (name[i + 3] == 'A' || name[i + 3] == 'B') && name[i + 4] == 'E' {
			is_member = true
			own := member_arg_bytes(name[i + 5:])
			if own >= 0 && own != n {
				// a member taking a different argument list: re-push the arguments it takes, it pops them, pop the slot's
				label := fmt.aprintf(".Lthunk_%d", thunk_count); thunk_count += 1
				fmt.sbprintf(&thunks, "%s:\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n", label)
				for k := own - 4; k >= 0; k -= 4 do fmt.sbprintf(&thunks, "\tpushl %d(%%ebp)\n", 8 + (k < n ? k : 0))
				fmt.sbprintf(&thunks, "\tcalll %s\n\tpopl %%ebp\n\tretl $%d\n", target, n)
				return label
			}
			if own < 0 do fmt.eprintf("retailfill: note: argument size of %s unknown; slot used as is\n", name)
			break
		}
	}
	if is_member do return target
	label := fmt.aprintf(".Lthunk_%d", thunk_count); thunk_count += 1
	fmt.sbprintf(&thunks, "%s:\n\tpushl %%ebp\n\tmovl %%esp, %%ebp\n", label)
	for k := n - 4; k >= 0; k -= 4 do fmt.sbprintf(&thunks, "\tpushl %d(%%ebp)\n", 8 + k)
	fmt.sbprintf(&thunks, "\tpushl %%ecx\n\tcalll %s\n\taddl $%d, %%esp\n\tpopl %%ebp\n\tretl $%d\n", target, n + 4, n)
	return label
}

Datum :: struct {
	sym:        string,
	rva:        u32,
	layout:     [dynamic]Tok,
	size:       u32,
	pc_size:    u32,
	pc_offsets: map[u32]u32,
}

is64 := false
win64 := false
image: []u8
names: map[u32][dynamic]string
defined: map[string]bool
defined_by_32: map[string]string
datums: [dynamic]Datum
strings_out: strings.Builder
string_labels: map[u32]string
missing_fns: map[u32]bool
spec_names: map[u32]string
pool_sym: string
pool_lo, pool_hi: u32
errors := 0
data_name_rvas: [dynamic]u32 // named data addresses (sorted) for pointers into the middle of a datum

be32 :: proc(rva: u32) -> u32 {
	k := int(rva)
	return u32(image[k]) << 24 | u32(image[k + 1]) << 16 | u32(image[k + 2]) << 8 | u32(image[k + 3])
}

// Xbox member functions are cdecl (`@@UAA`); the PC build compiles them thiscall (`@@UAE`).
pc_member_name :: proc(n: string) -> string {
	b := transmute([]u8)strings.clone(n)
	for i := 0; i + 4 < len(b); i += 1 {
		if b[i] == '!' && b[i + 1] == '!' && strings.index_byte("QUAIMECOGW", b[i + 2]) >= 0 && (b[i + 3] == 'A' || b[i + 3] == 'B') && b[i + 4] == 'A' do b[i + 4] = 'E'
	}
	return string(b)
}

// Map a 64-bit MSVC mangled symbol (ELF spelling, `!` for `@`) back to 32-bit Xbox mangling (`@@UAA`, no `__ptr64` `E`).
mangle64_to_32 :: proc(s: string) -> string {
	if !strings.has_prefix(s, "?") do return s
	out := strings.builder_make()
	i := 0
	for i < len(s) {
		// Member function qualifiers: !!<access>EAA / !!<access>EBA -> !!<access>AA / !!<access>BA
		if i + 5 < len(s) && s[i] == '!' && s[i + 1] == '!' &&
			strings.index_byte("QUAIMECOGW", s[i + 2]) >= 0 && s[i + 3] == 'E' &&
			(s[i + 4] == 'A' || s[i + 4] == 'B') && s[i + 5] == 'A' {
			strings.write_string(&out, "!!")
			strings.write_byte(&out, s[i + 2])
			strings.write_byte(&out, s[i + 4])
			strings.write_byte(&out, 'A')
			i += 6
			continue
		}
		// Trailing pointer-variable storage qualifier: !!EA / !!EB / !!EC / !!ED -> !!A / !!B / !!C / !!D
		if i + 4 == len(s) && s[i] == '!' && s[i + 1] == '!' && s[i + 2] == 'E' &&
			strings.index_byte("ABCD", s[i + 3]) >= 0 {
			strings.write_string(&out, "!!")
			strings.write_byte(&out, s[i + 3])
			i += 4
			continue
		}
		// Pointer/reference __ptr64 marker: PE[ABCDI] / QE[ABCDI] / AE[ABCDI] -> P[ABCDI] / Q[ABCDI] / A[ABCDI]
		if i + 2 < len(s) && (s[i] == 'P' || s[i] == 'Q' || s[i] == 'A') && s[i + 1] == 'E' &&
			strings.index_byte("ABCDI", s[i + 2]) >= 0 {
			strings.write_byte(&out, s[i])
			strings.write_byte(&out, s[i + 2])
			i += 3
			continue
		}
		strings.write_byte(&out, s[i])
		i += 1
	}
	return strings.to_string(out)
}

// Normalize 32-bit PC thiscall (`!!UAE`, `!!QAE`, `!!UBE`, `!!EAE`, `!!EBE`) back to Xbox `!!UAA` for defined_by_32 lookup.
thiscall_to_xbox :: proc(n: string) -> string {
	b := transmute([]u8)strings.clone(n)
	for i := 0; i + 4 < len(b); i += 1 {
		if b[i] == '!' && b[i + 1] == '!' && strings.index_byte("QUAIMECOGW", b[i + 2]) >= 0 && (b[i + 3] == 'A' || b[i + 3] == 'B') && b[i + 4] == 'E' do b[i + 4] = 'A'
	}
	return string(b)
}

lookup64 :: proc(e: string) -> (string, bool) {
	if s64, ok := defined_by_32[e]; ok do return s64, true
	x := thiscall_to_xbox(e)
	if s64, ok := defined_by_32[x]; ok do return s64, true
	return "", false
}

elf_name :: proc(n: string) -> string {
	s, _ := strings.replace_all(n, "@", "!")
	return s
}

pick_name :: proc(rva: u32, want_function: bool) -> (string, bool) {
	if n, ok := spec_names[rva]; ok {
		if is64 && n not_in defined {
			if s64, ok64 := lookup64(n); ok64 do return s64, true
		}
		return n, true
	}
	list, ok := names[rva]
	if !ok do return "", false
	best := ""
	score := -1
	for n in list {
		e := elf_name(n)
		if e not_in defined {
			if is64 {
				if s64, ok64 := lookup64(e); ok64 do e = s64
			} else {
				t := pc_member_name(e)
				if t in defined do e = t
			}
		}
		sc := 0
		if e in defined do sc += 4
		if !strings.has_prefix(n, "?") do sc += 2 // C name
		if strings.contains(n, "@@YA") do sc += 2 // free cdecl function
		if sc > score { score = sc; best = e }
	}
	return best, true
}

looks_like_string :: proc(rva: u32) -> bool {
	k := int(rva)
	n := 0
	for k + n < len(image) && image[k + n] != 0 {
		c := image[k + n]
		if c < 0x20 && c != '\n' && c != '\t' && c != '\r' do return false
		if c >= 0x7f do return false
		n += 1
		if n > 4096 do return false
	}
	return n > 0
}

string_label :: proc(rva: u32) -> string {
	if l, ok := string_labels[rva]; ok do return l
	l := fmt.aprintf(".Lretail_str_%x", rva)
	string_labels[rva] = l
	fmt.sbprintf(&strings_out, "%s:\n\t.byte ", l)
	k := int(rva)
	for image[k] != 0 { fmt.sbprintf(&strings_out, "%d,", image[k]); k += 1 }
	fmt.sbprintf(&strings_out, "0\n")
	return l
}

resolve :: proc(word: u32, where_sym: string, off: u32) -> string {
	if word == 0 do return "0"
	if word < IMAGE_BASE || word >= IMAGE_BASE + u32(len(image)) {
		fmt.eprintf("retailfill: %s+0x%x: 0x%08x is not an image address\n", where_sym, off, word)
		errors += 1
		return fmt.aprintf("0x%x", word)
	}
	rva := word - IMAGE_BASE
	for d in datums do if rva >= d.rva && rva < d.rva + d.size {
		if rva == d.rva do return fmt.aprintf("\"%s\"", d.sym)
		delta := rva - d.rva
		if is64 {
			if mapped, ok := d.pc_offsets[delta]; ok do delta = mapped
		}
		return fmt.aprintf("\"%s\"+%d", d.sym, delta)
	}
	if rva >= TEXT_LO && rva < TEXT_HI {
		if n, ok := pick_name(rva, true); ok do return fmt.aprintf("\"%s\"", n)
		fmt.eprintf("retailfill: note: %s+0x%x: function 0x%x is not in source (retail_fn_%x)\n", where_sym, off, rva, rva)
		if rva not_in missing_fns do missing_fns[rva] = true
		return fmt.aprintf("retail_fn_%x", rva)
	}
	if n, ok := pick_name(rva, false); ok do return fmt.aprintf("\"%s\"", n)
	if looks_like_string(rva) do return string_label(rva)
	// a pointer into a named datum (a member of a global struct): the PC build asserts the retail 32-bit layouts
	if rva < TEXT_LO || rva >= TEXT_HI {
		lo, hi := 0, len(data_name_rvas)
		for lo < hi { mid := (lo + hi) / 2; if data_name_rvas[mid] <= rva do lo = mid + 1; else do hi = mid }
		if lo > 0 {
			start := data_name_rvas[lo - 1]
			if rva - start < 0x10000 {
				n, ok := pick_name(start, false)
				// a pointer variable (`3PA` / `3PB` / `3QA` / `3QB` data) holds 4 bytes (8 on 64-bit)
				if ok && rva - start >= 4 && (strings.contains(n, "!!3PA") || strings.contains(n, "!!3PB") || strings.contains(n, "!!3QA") || strings.contains(n, "!!3QB") || strings.contains(n, "!!3PE") || strings.contains(n, "!!3QE")) do ok = false
				if ok && n in defined {
					fmt.eprintf("retailfill: note: %s+0x%x: data 0x%x is %s+0x%x\n", where_sym, off, rva, n, rva - start)
					return fmt.aprintf("\"%s\"+%d", n, rva - start)
				}
			}
		}
	}
	if looks_like_string(rva) do return string_label(rva)
	fmt.eprintf("retailfill: warning: %s+0x%x: data 0x%x is neither a datum, a name nor a string (0)\n", where_sym, off, rva)
	return "0"
}

main :: proc() {
	arg0 := 1
	if len(os.args) == 6 && (os.args[1] == "-64" || os.args[1] == "-win64") {
		is64 = true
		win64 = os.args[1] == "-win64"
		arg0 = 2
	} else if len(os.args) != 5 {
		fmt.eprintln("usage: retailfill [-64|-win64] SPEC RETAIL.pe REPO DEFINED.txt > fill.s")
		os.exit(2)
	}
	spec, e1 := os.read_entire_file(os.args[arg0], context.allocator)
	img, e2 := os.read_entire_file(os.args[arg0 + 1], context.allocator)
	if e1 != nil || e2 != nil { fmt.eprintln("retailfill: cannot read inputs"); os.exit(1) }
	image = img
	repo := os.args[arg0 + 2]
	if def, e := os.read_entire_file(os.args[arg0 + 3], context.allocator); e == nil {
		for l in strings.split_lines(string(def)) {
			if len(l) == 0 do continue
			defined[l] = true
			if is64 && strings.has_prefix(l, "?") {
				c32 := mangle64_to_32(l)
				if c32 not_in defined_by_32 do defined_by_32[c32] = l
				if strings.contains(c32, "_J") {
					c32j, _ := strings.replace_all(c32, "_J", "J")
					if c32j not_in defined_by_32 do defined_by_32[c32j] = l
				}
				if strings.contains(c32, "V?$c_basic_buffer!X!!") {
					c32b, _ := strings.replace_all(c32, "V?$c_basic_buffer!X!!", "_K")
					if c32b not_in defined_by_32 do defined_by_32[c32b] = l
				}
			}
		}
	}
	add_name :: proc(rva: u32, n: string) {
		if rva not_in names do names[rva] = make([dynamic]string)
		append(&names[rva], n)
	}
	if owners, e := os.read_entire_file(strings.concatenate({repo, "/metadata/matching/data-owners.tsv"}), context.allocator); e == nil {
		for line, i in strings.split_lines(string(owners)) {
			if i == 0 do continue
			cols := strings.split(line, "\t")
			if len(cols) < 7 || cols[5] != "0" do continue
			va, ok := strconv.parse_u64(strings.trim_prefix(cols[0], "0x"), 16)
			if ok && va >= IMAGE_BASE do add_name(u32(va - IMAGE_BASE), cols[4])
		}
	}
	walk :: proc(dir: string) {
		h, err := os.open(dir)
		if err != nil do return
		infos, _ := os.read_dir(h, -1, context.allocator)
		os.close(h)
		for fi in infos {
			if fi.type == .Directory { walk(fi.fullpath); continue }
			if !strings.has_suffix(fi.name, ".cpp") && !strings.has_suffix(fi.name, ".c") do continue
			data, rerr := os.read_entire_file(fi.fullpath, context.allocator)
			if rerr != nil do continue
			for line in strings.split_lines(string(data)) {
				rest := strings.trim_space(line)
				if !strings.has_prefix(rest, "// @address 0x") do continue
				fields := strings.fields(rest)
				if len(fields) < 4 do continue
				rva, ok := strconv.parse_u64(strings.trim_prefix(fields[2], "0x"), 16)
				if !ok do continue
				if rva >= IMAGE_BASE do rva -= IMAGE_BASE
				add_name(u32(rva), fields[3])
			}
		}
	}
	walk(strings.concatenate({repo, "/tu"}))
	// retail function names (judge inputs: name, then identical-code aliases) for functions the source defines
	// without an `@address` tag; pick_name keeps the ones the PC link defines
	if rn, e := os.read_entire_file(strings.concatenate({repo, "/metadata/judge-inputs/retail-names.tsv"}), context.allocator); e == nil {
		for line, i in strings.split_lines(string(rn)) {
			if i == 0 do continue
			cols := strings.split(line, "\t")
			if len(cols) < 7 do continue
			rva, ok := strconv.parse_u64(strings.trim_prefix(cols[0], "0x"), 16)
			if !ok || rva < TEXT_LO || rva >= TEXT_HI do continue
			if u32(rva) in names do continue // the source's tag wins
			add_name(u32(rva), cols[6])
			if len(cols) >= 11 do for a in strings.fields(cols[10]) do add_name(u32(rva), a)
		}
	}
	for rva in names do if rva < TEXT_LO || rva >= TEXT_HI do append(&data_name_rvas, rva)
	slice.sort(data_name_rvas[:])

	for raw in strings.split_lines(string(spec)) {
		line := strings.trim_space(raw)
		if len(line) == 0 || line[0] == '#' do continue
		f := strings.fields(line)
		if f[0] == "name" && len(f) == 3 {
			rva, _ := strconv.parse_u64(strings.trim_prefix(f[1], "0x"), 16)
			if rva >= IMAGE_BASE do rva -= IMAGE_BASE
			spec_names[u32(rva)] = elf_name(f[2])
			add_name(u32(rva), f[2])
			if !is64 && u32(rva) in names {
				// a spec name wins: put it first by making it the only C-scored defined candidate
				defined[elf_name(f[2])] = true
			}
			continue
		}
		// `patch RVA FILE`: the image words from RVA on as a dynamic initializer leaves them (FILE: one big-endian
		// hex word per line, relative to the repo), for data the retail image only fills at run time
		if f[0] == "pool" && len(f) == 4 {
			lo, _ := strconv.parse_u64(strings.trim_prefix(f[2], "0x"), 16)
			hi, _ := strconv.parse_u64(strings.trim_prefix(f[3], "0x"), 16)
			if lo >= IMAGE_BASE do lo -= IMAGE_BASE
			if hi >= IMAGE_BASE do hi -= IMAGE_BASE
			if pool_sym != "" || lo >= hi || (hi - lo) % 4 != 0 || hi > u64(len(image)) { fmt.eprintln("retailfill: bad pool line:", line); os.exit(1) }
			pool_sym, pool_lo, pool_hi = elf_name(f[1]), u32(lo), u32(hi)
			continue
		}
		if f[0] == "patch" && len(f) == 3 {
			prva, _ := strconv.parse_u64(strings.trim_prefix(f[1], "0x"), 16)
			if prva >= IMAGE_BASE do prva -= IMAGE_BASE
			words, perr := os.read_entire_file(strings.concatenate({repo, "/", f[2]}), context.allocator)
			if perr != nil { fmt.eprintln("retailfill: cannot read", f[2]); os.exit(1) }
			at := int(prva)
			for w in strings.split_lines(string(words)) {
				if len(w) == 0 do continue
				v, ok := strconv.parse_u64(w, 16)
				if !ok { fmt.eprintln("retailfill: bad word in", f[2], w); os.exit(1) }
				image[at] = u8(v >> 24); image[at + 1] = u8(v >> 16); image[at + 2] = u8(v >> 8); image[at + 3] = u8(v)
				at += 4
			}
			continue
		}
		if f[0] != "datum" || len(f) < 4 { fmt.eprintln("retailfill: bad line:", line); os.exit(1) }
		rva, _ := strconv.parse_u64(strings.trim_prefix(f[2], "0x"), 16)
		if rva >= IMAGE_BASE do rva -= IMAGE_BASE
		d := Datum{sym = f[1], rva = u32(rva)}
		for tok in f[3:] {
			count := 1
			if len(tok) > 1 { c, ok := strconv.parse_int(tok[1:]); if ok do count = c }
			if tok[0] == 't' {
				if eq := strings.index_byte(tok, '='); eq > 0 {
					c, _ := strconv.parse_int(tok[1:eq])
					tgt := elf_name(tok[eq + 1:])
					if is64 && tgt not_in defined {
						if s64, ok64 := lookup64(tgt); ok64 do tgt = s64
					}
					append(&d.layout, Tok{'t', c, fmt.aprintf("\"%s\"", tgt)})
				} else do append(&d.layout, Tok{'t', count, ""})
				d.size += 4
				continue
			}
			width: u32
			switch tok[0] {
			case 'a': width = 4
			case 'p', 'w': width = 4
			case 'q': width = 8
			case 'h': width = 2
			case 'b', 'z': width = 1
			case: fmt.eprintln("retailfill: bad token", tok); os.exit(1)
			}
			for _ in 0 ..< count { append(&d.layout, Tok{tok[0], 0, ""}); d.size += width }
		}
		if is64 {
			// Compute 64-bit pc_offsets for each retail offset (for interior pointer resolution).
			roff: u32 = 0
			poff: u32 = 0
			for t in d.layout {
				switch t.k {
				case 't', 'p':
					if poff % 8 != 0 do poff += 8 - (poff % 8)
					d.pc_offsets[roff] = poff
					roff += 4; poff += 8
				case 'q':
					if poff % 8 != 0 do poff += 8 - (poff % 8)
					d.pc_offsets[roff] = poff
					roff += 8; poff += 8
				case 'a':
					v := be32(d.rva + roff)
					if v >= IMAGE_BASE && v < IMAGE_BASE + u32(len(image)) {
						if poff % 8 != 0 do poff += 8 - (poff % 8)
						d.pc_offsets[roff] = poff
						poff += 8
					} else {
						d.pc_offsets[roff] = poff
						poff += 4
					}
					roff += 4
				case 'w': d.pc_offsets[roff] = poff; roff += 4; poff += 4
				case 'h': d.pc_offsets[roff] = poff; roff += 2; poff += 2
				case 'b', 'z': d.pc_offsets[roff] = poff; roff += 1; poff += 1
				}
			}
			d.pc_size = poff
		} else {
			d.pc_size = d.size
		}
		append(&datums, d)
	}

	b := strings.builder_make()
	fmt.sbprintln(&b, "\t.data")
	if pool_sym != "" {
		fmt.sbprintf(&b, "\t.globl \"%s\"\n\t.p2align 4\n\"%s\":\n", pool_sym, pool_sym)
		for at := pool_lo; at < pool_hi; at += 4 do fmt.sbprintf(&b, "\t.long 0x%x\n", be32(at))
		fmt.sbprintf(&b, "\t.size \"%s\", %d\n", pool_sym, pool_hi - pool_lo)
	}
	for d in datums {
		in_pool := pool_sym != "" && d.rva >= pool_lo && d.rva + d.size <= pool_hi && d.rva % 4 == 0
		for t in d.layout do if t.k != 'w' do in_pool = false
		if in_pool {
			fmt.sbprintf(&b, "\t.globl \"%s\"\n\t.type \"%s\", @object\n\t.set \"%s\", \"%s\" + 0x%x\n\t.size \"%s\", %d\n",
				d.sym, d.sym, d.sym, pool_sym, d.rva - pool_lo, d.sym, d.size)
			continue
		}
		fmt.sbprintf(&b, "\t.globl \"%s\"\n\t.p2align 3\n\"%s\":\n", d.sym, d.sym)
		off: u32 = 0
		poff: u32 = 0
		for t in d.layout {
			at := d.rva + off
			switch t.k {
			case 't':
				target := t.target != "" ? t.target : resolve(be32(at), d.sym, off)
				if is64 {
					if poff % 8 != 0 { pad := 8 - (poff % 8); fmt.sbprintf(&b, "\t.zero %d\n", pad); poff += pad }
					fmt.sbprintf(&b, "\t.quad %s\n", target)
					off += 4; poff += 8
				} else {
					fmt.sbprintf(&b, "\t.long %s\n", thiscall_slot(target, t.n)); off += 4; poff += 4
				}
			case 'p':
				if is64 {
					if poff % 8 != 0 { pad := 8 - (poff % 8); fmt.sbprintf(&b, "\t.zero %d\n", pad); poff += pad }
					fmt.sbprintf(&b, "\t.quad %s\n", resolve(be32(at), d.sym, off))
					off += 4; poff += 8
				} else {
					fmt.sbprintf(&b, "\t.long %s\n", resolve(be32(at), d.sym, off)); off += 4; poff += 4
				}
			case 'a':
				v := be32(at)
				if v >= IMAGE_BASE && v < IMAGE_BASE + u32(len(image)) {
					if is64 {
						if poff % 8 != 0 { pad := 8 - (poff % 8); fmt.sbprintf(&b, "\t.zero %d\n", pad); poff += pad }
						fmt.sbprintf(&b, "\t.quad %s\n", resolve(v, d.sym, off))
						poff += 8
					} else {
						fmt.sbprintf(&b, "\t.long %s\n", resolve(v, d.sym, off))
						poff += 4
					}
				} else {
					fmt.sbprintf(&b, "\t.long 0x%x\n", v)
					poff += 4
				}
				off += 4
			case 'w': fmt.sbprintf(&b, "\t.long 0x%x\n", be32(at)); off += 4; poff += 4
			case 'q':
				if is64 && poff % 8 != 0 { pad := 8 - (poff % 8); fmt.sbprintf(&b, "\t.zero %d\n", pad); poff += pad }
				fmt.sbprintf(&b, "\t.quad 0x%x\n", u64(be32(at)) << 32 | u64(be32(at + 4))); off += 8; poff += 8
			case 'h': fmt.sbprintf(&b, "\t.short 0x%x\n", u32(image[at]) << 8 | u32(image[at + 1])); off += 2; poff += 2
			case 'b': fmt.sbprintf(&b, "\t.byte 0x%x\n", image[at]); off += 1; poff += 1
			case 'z': fmt.sbprintf(&b, "\t.byte 0\n"); off += 1; poff += 1
			}
		}
		fmt.sbprintf(&b, "\t.size \"%s\", %d\n", d.sym, d.pc_size)
	}
	fmt.sbprintln(&b, "\t.text")
	fmt.sbprint(&b, strings.to_string(thunks))
	for rva in missing_fns {
		if is64 {
			if win64 {
				fmt.sbprintf(&b, "\t.globl retail_fn_%x\n\t.p2align 4\nretail_fn_%x:\n\tsubq $40, %%rsp\n\tleaq .Lretail_fn_name_%x(%%rip), %%rcx\n\tmovl $1, %%edx\n\tcallq port_stub_called\n\taddq $40, %%rsp\n\txorl %%eax, %%eax\n\tretq\n", rva, rva, rva)
			} else {
				fmt.sbprintf(&b, "\t.globl retail_fn_%x\n\t.p2align 4\nretail_fn_%x:\n\tsubq $8, %%rsp\n\tleaq .Lretail_fn_name_%x(%%rip), %%rdi\n\tmovl $1, %%esi\n\tcallq port_stub_called\n\taddq $8, %%rsp\n\txorl %%eax, %%eax\n\tretq\n", rva, rva, rva)
			}
		} else {
			fmt.sbprintf(&b, "\t.globl retail_fn_%x\n\t.p2align 4\nretail_fn_%x:\n\tpushl %%ecx\n\tpushl $1\n\tpushl $.Lretail_fn_name_%x\n\tcalll port_stub_called\n\taddl $8, %%esp\n\tpopl %%ecx\n\txorl %%eax, %%eax\n\txorl %%edx, %%edx\n\tretl\n", rva, rva, rva)
		}
		fmt.sbprintf(&strings_out, ".Lretail_fn_name_%x:\n\t.asciz \"retail function 0x%x (not in source)\"\n", rva, rva)
	}
	fmt.sbprintln(&b, "\t.section .rodata")
	fmt.sbprint(&b, strings.to_string(strings_out))
	fmt.print(strings.to_string(b))
	if errors > 0 { fmt.eprintf("retailfill: %d errors\n", errors); os.exit(1) }
}

// Stack bytes a thiscall member's parameters take, from its MS-mangled type after `!!QAE` (return type, parameters,
// `Z`), -1 when a parameter is not a simple type (a class by value, a function pointer).
member_arg_bytes :: proc(s: string) -> int {
	i := 0
	backrefs: [dynamic]int
	parse :: proc(s: string, i: ^int, backrefs: ^[dynamic]int, record: bool) -> int {
		if i^ >= len(s) do return -1
		c := s[i^]
		i^ += 1
		switch c {
		case 'X': return 0
		case 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'M': return 4
		case 'N': return 8
		case '_':
			if i^ >= len(s) do return -1
			d := s[i^]; i^ += 1
			switch d { case 'N', 'W': return 4; case 'J', 'K': return 8 }
			return -1
		case 'P', 'Q', 'A':
			if i^ < len(s) && s[i^] == '6' do return -1
			i^ += 1 // cv
			start := i^
			if parse(s, i, backrefs, false) == -1 && i^ == start do return -1
			if record && i^ - start + 2 > 1 do append(backrefs, 4)
			return 4
		case 'W':
			i^ += 1 // enum underlying type
			e := strings.index(s[i^:], "!!")
			if e < 0 do return -1
			i^ += e + 2
			if record do append(backrefs, 4)
			return 4
		case 'V', 'U':
			e := strings.index(s[i^:], "!!")
			if e < 0 do return -1
			i^ += e + 2
			return -1
		case '0' ..= '9':
			k := int(c - '0')
			if k < len(backrefs^) do return backrefs^[k]
			return -1
		case '?':
			i^ += 1 // ?A / ?B: class return by value qualifier
			return parse(s, i, backrefs, false)
		}
		return -1
	}
	if parse(s, &i, &backrefs, false) == -1 && !(i > 0) do return -1 // return type
	if i < len(s) && s[i] == 'X' do return 0
	total := 0
	for i < len(s) {
		if s[i] == 'Z' || (s[i] == '!' && i + 1 < len(s) && s[i + 1] == 'Z') do return total
		b := parse(s, &i, &backrefs, true)
		if b < 0 do return -1
		total += b
	}
	return -1
}

