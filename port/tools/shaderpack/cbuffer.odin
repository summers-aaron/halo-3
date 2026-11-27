package main

// Declarative cbuffer gathers: how the PC renderer fills each DX11 cbuffer of the pack's shaders from the Xbox
// register files the game writes (D3DDevice's float4 c#, bool b#, integer i# shadows).
//
// Bungie's DX11 tree declares every cbuffer member with the Xbox register it replaces:
//   CBUFFER_CONST(CopyTargetPS, float4, intensity, k_ps_copy_target_intensity)
//   SHADER_CONST_ALIAS(ViewVS, float4, View_Projection_X, ..., k_viewproj_xform_x, k_viewproj_xform, 0)
// and the DX9 (Xbox) branch of the matching C++ header gives the register:
//   #define k_ps_copy_target_intensity POSTPROCESS_DEFAULT_PIXEL_CONSTANT  ->  CONSTANT_NAME(2)
//   enum { k_viewproj_xform_x = k_first_view_constant, ... }
// The builder preprocesses both with DXC (-P) and evaluates the register expressions; the member layouts come from
// the compiled SPIR-V (D3D packing). The result is one table of copies per cbuffer name; no HLSL text goes into the
// pack. A member maps to consecutive registers, one per 16-byte slot, as the D3D9 constant table did; an aliased
// member (a matrix whose columns had their own register names) takes each alias's register at its byte offset.
//
// Register file of a member: BOOL_CONSTANT_NAME / INT_CONSTANT_NAME tags in the header, else the member's HLSL type
// (bool -> b#, int/uint -> c# read as integers, otherwise c#).

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

Cb_Decl :: struct {
	buffer, type, name, kname: string,
}
Cb_Alias :: struct {
	buffer, type, name, kname, member_kname: string, // alias register name, the member it is part of
	offset:                                  u32,
}

Kreg :: struct {
	file:  u8, // 'c', 'b', 'i', or 0 when the expression carried no tag
	value: int,
}

Cbuffer_Table :: struct {
	name:       string,
	stage:      Stage,
	layout:     ^Reflect_Cbuffer,
	size:       u32,
	gathers:    [dynamic]Gather,
	members:    int,
	resolved:   int,
	unresolved: [dynamic]string,
}

Cbuffer_Stats :: struct {
	buffers, members, resolved, padding, layout_conflicts: int,
	resolved_retail:                                        int,
	no_declaration:                                         int,
}

// ---- preprocessing ----

cb_preprocess :: proc(dir, name, text: string, includes: []string) -> (string, bool) {
	src := fmt.tprintf("%s/%s.hlsl", dir, name)
	out := fmt.tprintf("%s/%s.pp", dir, name)
	if !write_file(src, text) {return "", false}
	cmd: [dynamic]string
	cmd.allocator = context.temp_allocator
	append(&cmd, opts.dxc, "-P", "-flegacy-macro-expansion")
	for inc in includes {append(&cmd, "-I", inc)}
	append(&cmd, "-Fi", out, src)
	if ok, msg := run(cmd[:]); !ok {
		fmt.eprintfln("shaderpack: preprocessing %s: %s", src, msg)
		return "", false
	}
	pp, err := os.read_entire_file(out, context.allocator)
	if err != nil {return "", false}
	return string(pp), true
}

// The register headers' DX9 branch, preprocessed. Copies of the headers go to the work directory first: the DX11
// branch includes a C++-only file (an empty stand-in is provided), and implicit_hill_registers.h spells one #define
// as "#deifne" (never compiled: the DX9 branch was the Xbox build's), read here as the #define it means.
cb_register_text :: proc(work: string, knames: []string) -> (string, bool) {
	dir := fmt.tprintf("%s/cbuffer", work)
	hdir := fmt.tprintf("%s/h", dir)
	stub := fmt.tprintf("%s/inc/rasterizer/dx11", dir)
	for d in ([]string{dir, hdir, stub}) {
		if !os.exists(d) && os.make_directory_all(d) != nil {return "", false}
	}
	write_file(fmt.tprintf("%s/rasterizer_dx11_define_fx_constants.h", stub), "")
	entries, err := os.read_directory_by_path(opts.hlsl, -1, context.allocator)
	if err != nil {return "", false}
	slice.sort_by(entries, proc(a, b: os.File_Info) -> bool {return a.name < b.name})
	b := strings.builder_make(context.temp_allocator)
	fmt.sbprintln(&b, "#define DX_VERSION 9")
	fmt.sbprintln(&b, "#define XENON 1")
	fmt.sbprintln(&b, "#define CONSTANT_NAME(n) __c(n)")
	fmt.sbprintln(&b, "#define BOOL_CONSTANT_NAME(n) __b(n)")
	fmt.sbprintln(&b, "#define INT_CONSTANT_NAME(n) __i(n)")
	for e in entries {
		if !strings.has_suffix(e.name, ".h") {continue}
		data, rerr := os.read_entire_file(strings.concatenate({opts.hlsl, "/", e.name}, context.temp_allocator), context.temp_allocator)
		if rerr != nil {continue}
		text := string(data)
		fixed, _ := strings.replace_all(text, "\n#deifne ", "\n#define ", context.temp_allocator)
		write_file(fmt.tprintf("%s/%s", hdir, e.name), fixed)
		if strings.has_suffix(e.name, "registers.h") || strings.has_prefix(e.name, "hlsl_constant_") {
			fmt.sbprintfln(&b, "#include \"%s\"", e.name)
		}
	}
	for k in knames {fmt.sbprintfln(&b, "@@K \"%s\" = %s ;", k, k)}
	return cb_preprocess(dir, "registers", strings.to_string(b), {fmt.tprintf("%s/inc", dir), hdir})
}

// Every CBUFFER_CONST / CBUFFER_CONST_ARRAY / SHADER_CONST_ALIAS of the DX11 tree (pc build), preprocessed.
cb_declaration_text :: proc(work: string) -> (string, bool) {
	dir := fmt.tprintf("%s/cbuffer", work)
	if !os.exists(dir) && os.make_directory_all(dir) != nil {return "", false}
	entries, err := os.read_directory_by_path(opts.hlsl, -1, context.allocator)
	if err != nil {return "", false}
	slice.sort_by(entries, proc(a, b: os.File_Info) -> bool {return a.name < b.name})
	b := strings.builder_make(context.temp_allocator)
	fmt.sbprintln(&b, "#define DX_VERSION 11")
	fmt.sbprintln(&b, "#define pc 1")
	fmt.sbprintln(&b, "#include \"global_cbuffer.fx\"")
	for m in ([]string{"CBUFFER_BEGIN", "CBUFFER_BEGIN_FIXED", "CBUFFER_END", "CBUFFER_CONST", "CBUFFER_CONST_ARRAY", "SHADER_CONST_ALIAS"}) {fmt.sbprintfln(&b, "#undef %s", m)}
	fmt.sbprintln(&b, "#define CBUFFER_BEGIN(_n)")
	fmt.sbprintln(&b, "#define CBUFFER_BEGIN_FIXED(_n, _s)")
	fmt.sbprintln(&b, "#define CBUFFER_END")
	fmt.sbprintln(&b, "#define CBUFFER_CONST(_b, _t, _h, _c) @@C _b | _t | _h | _c ;")
	fmt.sbprintln(&b, "#define CBUFFER_CONST_ARRAY(_b, _t, _h, _d, _c) @@C _b | _t | _h | _c ;")
	fmt.sbprintln(&b, "#define SHADER_CONST_ALIAS(_b, _t, _h, _hd, _c, _cd, _o) @@A _b | _t | _h | _c | _cd | _o ;")
	for e in entries {
		if !strings.has_suffix(e.name, ".fx") || e.name == "global_cbuffer.fx" {continue}
		data, rerr := os.read_entire_file(strings.concatenate({opts.hlsl, "/", e.name}, context.temp_allocator), context.temp_allocator)
		if rerr != nil || !strings.contains(string(data), "CBUFFER_CONST") {continue}
		fmt.sbprintfln(&b, "#include \"%s\"", e.name)
	}
	return cb_preprocess(dir, "declarations", strings.to_string(b), {opts.hlsl})
}

// "@@X a | b | c ;" records of preprocessed text
cb_records :: proc(text, marker: string) -> [dynamic][]string {
	out: [dynamic][]string
	rest := text
	for {
		i := strings.index(rest, marker)
		if i < 0 {break}
		rest = rest[i + len(marker):]
		e := strings.index_byte(rest, ';')
		if e < 0 {break}
		fields := strings.split(rest[:e], "|")
		for &f in fields {f = strings.trim_space(f)}
		append(&out, fields)
		rest = rest[e:]
	}
	return out
}

// ---- register expressions ----

Cb_Eval :: struct {
	tokens: []string,
	pos:    int,
	names:  ^map[string]int,
	file:   u8,
	ok:     bool,
}

cb_tokenize :: proc(text: string) -> []string {
	out: [dynamic]string
	i := 0
	for i < len(text) {
		c := text[i]
		if c == ' ' || c == '\t' || c == '\n' || c == '\r' {i += 1; continue}
		if (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9') {
			j := i
			for j < len(text) && ((text[j] >= 'a' && text[j] <= 'z') || (text[j] >= 'A' && text[j] <= 'Z') || text[j] == '_' || (text[j] >= '0' && text[j] <= '9')) {j += 1}
			append(&out, text[i:j])
			i = j
			continue
		}
		append(&out, text[i:i + 1])
		i += 1
	}
	return out[:]
}

cb_primary :: proc(e: ^Cb_Eval) -> int {
	if e.pos >= len(e.tokens) {e.ok = false; return 0}
	t := e.tokens[e.pos]
	e.pos += 1
	switch {
	case t == "(":
		v := cb_sum(e)
		if e.pos < len(e.tokens) && e.tokens[e.pos] == ")" {e.pos += 1} else {e.ok = false}
		return v
	case t == "-":
		return -cb_primary(e)
	case t == "__c" || t == "__b" || t == "__i":
		e.file = t[2]
		return cb_primary(e)
	case t[0] >= '0' && t[0] <= '9':
		s := strings.trim_right(t, "uUlL")
		v, ok := strconv.parse_int(s)
		if !ok {e.ok = false}
		return v
	}
	if v, ok := e.names[t]; ok {return v}
	e.ok = false
	return 0
}

cb_product :: proc(e: ^Cb_Eval) -> int {
	v := cb_primary(e)
	for e.pos < len(e.tokens) && (e.tokens[e.pos] == "*" || e.tokens[e.pos] == "/") {
		op := e.tokens[e.pos]
		e.pos += 1
		r := cb_primary(e)
		if op == "*" {v *= r} else if r != 0 {v /= r} else {e.ok = false}
	}
	return v
}

cb_sum :: proc(e: ^Cb_Eval) -> int {
	v := cb_product(e)
	for e.pos < len(e.tokens) && (e.tokens[e.pos] == "+" || e.tokens[e.pos] == "-") {
		op := e.tokens[e.pos]
		e.pos += 1
		r := cb_product(e)
		v = op == "+" ? v + r : v - r
	}
	return v
}

cb_evaluate :: proc(tokens: []string, names: ^map[string]int) -> (Kreg, bool) {
	e := Cb_Eval{tokens = tokens, names = names, ok = true}
	v := cb_sum(&e)
	if !e.ok || e.pos != len(tokens) {return {}, false}
	return Kreg{file = e.file, value = v}, true
}

// enumerators of the preprocessed headers: `enum [name] { a = expr, b, ... };`
cb_enumerators :: proc(text: string, names: ^map[string]int) {
	toks := cb_tokenize(text)
	for i := 0; i < len(toks); i += 1 {
		if toks[i] != "enum" {continue}
		j := i + 1
		if j < len(toks) && toks[j] != "{" {j += 1}
		if j >= len(toks) || toks[j] != "{" {continue}
		j += 1
		next := 0
		for j < len(toks) && toks[j] != "}" {
			name := toks[j]
			j += 1
			if j < len(toks) && toks[j] == "=" {
				j += 1
				start := j
				depth := 0
				for j < len(toks) && !(depth == 0 && (toks[j] == "," || toks[j] == "}")) {
					if toks[j] == "(" {depth += 1} else if toks[j] == ")" {depth -= 1}
					j += 1
				}
				if v, ok := cb_evaluate(toks[start:j], names); ok {next = v.value}
			}
			names[name] = next
			next += 1
			if j < len(toks) && toks[j] == "," {j += 1}
		}
		i = j
	}
}

// ---- the tables ----

cbuffer_build :: proc(jobs: []Job, mc: ^Microcode_Set, stats: ^Cbuffer_Stats) -> (tables: [dynamic]Cbuffer_Table, ok: bool) {
	retail := retail_registers(mc)
	// layouts by cbuffer name, from every compiled graphics shader (they must agree)
	layouts := make(map[string]^Reflect_Cbuffer)
	defer delete(layouts)
	layout_stage := make(map[string]Stage)
	defer delete(layout_stage)
	for &j in jobs {
		if j.status != .Ok {continue}
		for &c in j.refl.cbuffers {
			// a compute shader's cbuffers (the DX11 memexport updates: Beam, ParticleUpdateVS, ContrailState, ..)
			// carry the constants the Xbox update vertex shader read: vertex registers
			stage := j.stage == .CS ? Stage.VS : j.stage
			if old_stage, seen := layout_stage[c.name]; seen && old_stage != stage {continue}
			layout_stage[c.name] = stage
			old, seen := layouts[c.name]
			if !seen {layouts[c.name] = &c; continue}
			same := old.size == c.size && len(old.members) == len(c.members)
			for m, mi in c.members {
				if !same {break}
				om := old.members[mi]
				same = om.name == m.name && len(om.slots) == len(m.slots)
				for sl, si in m.slots {if same {same = sl == om.slots[si]}}
			}
			if !same {
				stats.layout_conflicts += 1
				fmt.eprintfln("shaderpack: cbuffer %s: layout differs between shaders (%s)", c.name, j.name)
			}
		}
	}

	decl_text := cb_declaration_text(opts.work) or_return
	decls := make(map[string]Cb_Decl) // buffer + "." + hlsl name
	aliases := make(map[string][dynamic]Cb_Alias) // buffer + "." + member kname
	knames: [dynamic]string
	for r in cb_records(decl_text, "@@C") {
		if len(r) != 4 {continue}
		d := Cb_Decl{buffer = strings.clone(r[0]), type = strings.clone(r[1]), name = strings.clone(r[2]), kname = strings.clone(r[3])}
		decls[strings.concatenate({d.buffer, ".", d.name})] = d
		append(&knames, d.kname)
	}
	for r in cb_records(decl_text, "@@A") {
		if len(r) != 6 {continue}
		off, _ := strconv.parse_int(r[5])
		a := Cb_Alias{buffer = strings.clone(r[0]), type = strings.clone(r[1]), name = strings.clone(r[2]), kname = strings.clone(r[3]), member_kname = strings.clone(r[4]), offset = u32(off)}
		key := strings.concatenate({a.buffer, ".", a.member_kname})
		if !(key in aliases) {aliases[key] = {}}
		append(&aliases[key], a)
		append(&knames, a.kname)
	}
	slice.sort(knames[:])
	reg_text := cb_register_text(opts.work, knames[:]) or_return
	names := make(map[string]int)
	cb_enumerators(reg_text, &names)
	kregs := make(map[string]Kreg)
	for r in cb_records(reg_text, "@@K") {
		if len(r) != 1 {continue}
		eq := strings.index_byte(r[0], '=')
		if eq < 0 {continue}
		k := strings.trim(strings.trim_space(r[0][:eq]), "\"")
		if v, vok := cb_evaluate(cb_tokenize(r[0][eq + 1:]), &names); vok {kregs[strings.clone(k)] = v}
	}

	keys: [dynamic]string
	for k in layouts {append(&keys, k)}
	slice.sort(keys[:])
	for k in keys {
		c := layouts[k]
		t := Cbuffer_Table{name = c.name, size = c.size, stage = layout_stage[c.name], layout = c}
		for &m in c.members {
			t.members += 1
			d, has := decls[strings.concatenate({c.name, ".", m.name}, context.temp_allocator)]
			if !has {
				stats.no_declaration += 1
				append(&t.unresolved, m.name)
				continue
			}
			if strings.has_suffix(m.name, "_pad") || strings.has_suffix(d.kname, "_pad") {stats.padding += 1; t.resolved += 1; continue}
			if kr, kok := kregs[d.kname]; kok {
				file := kr.file
				if file == 0 {
					file = 'c'
					if strings.has_prefix(d.type, "bool") {file = 'b'}
				}
				member_gathers(&m, u32(kr.value), file, &t.gathers)
				t.resolved += 1
				continue
			}
			// a member the DX9 branch has no register name for: the retail shaders' constant tables, by name
			if rc, rok := retail_register(retail, layout_stage[c.name], m.name); rok {
				member_gathers(&m, u32(rc.reg), rc.set == REGSET_BOOL ? 'b' : rc.set == REGSET_INT4 ? 'i' : 'c', &t.gathers)
				t.resolved += 1
				stats.resolved_retail += 1
				continue
			}
			// a member known only through aliases of its parts (ViewVS View_Projection: k_viewproj_xform_x ..)
			if list, aok := aliases[strings.concatenate({c.name, ".", d.kname}, context.temp_allocator)]; aok && len(m.slots) > 0 {
				all := true
				base := m.slots[0].offset
				for a in list {
					kr, kok := kregs[a.kname]
					if !kok {all = false; continue}
					append(&t.gathers, Gather{dst = base + a.offset, size = 16, src = .Float, reg = u32(kr.value)})
				}
				if all {t.resolved += 1; continue}
			}
			append(&t.unresolved, m.name)
		}
		stats.buffers += 1
		stats.members += t.members
		stats.resolved += t.resolved
		slice.sort_by(t.gathers[:], proc(a, b: Gather) -> bool {return a.dst < b.dst})
		append(&tables, t)
	}
	return tables, true
}

// The explicit shaders' retail constant tables against the DX11 declarations: a cbuffer member whose name the retail
// shader also declares should sit at the same register. Returns (agree, disagree) and prints the disagreements.
cbuffer_check_retail :: proc(tables: []Cbuffer_Table, mc: []Microcode_Entry, set: ^Microcode_Set, entries: []Pack_Shader) -> (agree, disagree: int) {
	by_name := make(map[string]^Cbuffer_Table)
	defer delete(by_name)
	for &t in tables {by_name[t.name] = &t}
	shown := 0
	for &ms in set.shaders {
		key := microcode_key(ms.hash, ms.stage)
		_ = key
		src := &ms.sources[0]
		if src.kind != .Explicit {continue}
		// the pack shader of this explicit slot and stage
		pe: ^Pack_Shader
		for &e in entries {if e.kind == 0 && Stage(e.stage) == ms.stage && (src.explicit >= 0 ? int(e.explicit_index) == src.explicit : e.name == src.name) {pe = &e; break}}
		if pe == nil {continue}
		for &c in pe.job.refl.cbuffers {
			t, ok := by_name[c.name]
			if !ok {continue}
			for &m in c.members {
				for k in src.constants {
					if k.name != m.name || k.set != REGSET_FLOAT4 || len(m.slots) == 0 {continue}
					reg := -1
					for g in t.gathers {if g.dst == m.slots[0].offset && g.src == .Float {reg = int(g.reg); break}}
					if reg < 0 {continue}
					if reg == int(k.reg) {agree += 1} else {
						disagree += 1
						if shown < 12 {fmt.eprintfln("shaderpack: %s %s: %s.%s at c%d in the DX11 tree, c%d in the retail shader", src.name, stage_name[ms.stage], c.name, m.name, reg, k.reg); shown += 1}
					}
				}
			}
		}
	}
	return
}

// The gathers of one member whose first register is `reg` in register file `file` ('c', 'b', 'i').
member_gathers :: proc(m: ^Reflect_Member, reg: u32, file: u8, out: ^[dynamic]Gather) {
	for sl in m.slots {
		g := Gather{dst = sl.offset, size = sl.size, reg = reg + sl.reg}
		switch file {
		case 'b': g.src = .Bool; g.size = 4
		case 'i': g.src = sl.kind == 0 ? .Int_Float : .Int
		case: g.src = sl.kind == 0 ? .Float : .Float_Int
		}
		append(out, g)
	}
}

// Members the MCC tree renamed from the 2007 HLSL (the retail constant tables use the old names). Each pair is
// checked against the retail tables at build time: the old name must have one register across all retail shaders.
k_member_renames := [?][2]string{
	{"ps_postprocess_pixel_size", "pixel_size"},
	{"ps_postprocess_scale", "scale"},
	{"ps_postprocess_hue_saturation_matrix", "p_postprocess_hue_saturation_matrix"},
	{"ps_postprocess_contrast", "p_postprocess_contrast"},
	// render_target.fx's encodings of the LDR / HDR targets (MiscPS): the 2007 tables name them ldr_gamma2 b14 and
	// hdr_gamma2 b15, which render_player_view_set_exposure_constants and rasterizer_main_setup_hdr_render_targets
	// set per pass (g_*_render_mode == 2, the square-root encoding, unless the pass alpha-blends into the target)
	{"LDR_gamma2", "ldr_gamma2"},
	{"HDR_gamma2", "hdr_gamma2"},
	// displacement_registers.fx (DisplacementPS, DisplacementMotionBlurPS): the MCC tree prefixed them and has no
	// DX9 register names; the retail displacement / displacement_motion_blur tables (explicit 37 / 81) use the 2007
	// names: screen_constants c203, previous_view_projection c164, screen_to_world c168, misc_values c172,
	// blur_max_and_scale c173, crosshair_center c174, num_taps i2, do_distortion b2 (the registers
	// rasterizer_render_displacement_pass and the motion blur constants setup write).
	{"ps_displacement_screen_constants", "screen_constants"},
	{"ps_displacement_previous_view_projection", "previous_view_projection"},
	{"ps_displacement_screen_to_world", "screen_to_world"},
	{"ps_displacement_num_taps", "num_taps"},
	{"ps_displacement_misc_values", "misc_values"},
	{"ps_displacement_blur_max_and_scale", "blur_max_and_scale"},
	{"ps_displacement_crosshair_center", "crosshair_center"},
	{"ps_displacement_do_distortion", "do_distortion"},
}

retail_name :: proc(member: string) -> string {
	for r in k_member_renames {if r[0] == member {return r[1]}}
	return member
}

Retail_Register :: struct {
	reg, count: u16,
	set:        u8,
	conflict:   bool,
}

// every retail constant name -> its register, per stage; names that sit at different registers in different
// shaders are marked and never used
retail_registers :: proc(mc: ^Microcode_Set) -> [Stage]map[string]Retail_Register {
	out: [Stage]map[string]Retail_Register
	for &s in mc.shaders {
		for &src in s.sources {
			if src.kind == .Function {continue} // a function's constants are its own (MaterialParameters by name)
			for c in src.constants {
				if c.set == REGSET_SAMPLER {continue}
				old, seen := out[s.stage][c.name]
				if !seen {
					out[s.stage][c.name] = Retail_Register{reg = c.reg, count = u16(c.count), set = c.set}
				} else if old.reg != c.reg || old.set != c.set {
					old.conflict = true
					out[s.stage][c.name] = old
				}
			}
		}
	}
	return out
}

retail_register :: proc(retail: [Stage]map[string]Retail_Register, stage: Stage, member: string) -> (Retail_Register, bool) {
	r, ok := retail[stage][retail_name(member)]
	if !ok || r.conflict {return {}, false}
	return r, true
}

// Per-microcode cbuffer overrides: a member the retail shader's own constant table places at another register than
// the global table (the MCC tree moved some, e.g. patchy_fog's atmosphere constants), or that the global table could
// not resolve. Gathers carry the cbuffer's table index.
microcode_cbuffer_overrides :: proc(constants: []Xbox_Constant, refl: ^Reflection, tables: []Cbuffer_Table, index: map[string]int, out: ^[dynamic]Gather) -> (overrides: int) {
	for &c in refl.cbuffers {
		ti, ok := index[c.name]
		if !ok {continue}
		t := &tables[ti]
		for &m in c.members {
			if len(m.slots) == 0 {continue}
			want := retail_name(m.name)
			for k in constants {
				if k.name != want || k.set == REGSET_SAMPLER {continue}
				file: u8 = k.set == REGSET_BOOL ? 'b' : k.set == REGSET_INT4 ? 'i' : 'c'
				mine: [dynamic]Gather
				defer delete(mine)
				member_gathers(&m, u32(k.reg), file, &mine)
				differs := false
				for g in mine {
					found := false
					for tg in t.gathers {if tg.dst == g.dst && tg.reg == g.reg && tg.src == g.src {found = true; break}}
					if !found {differs = true}
				}
				if differs {
					for g in mine {
						x := g
						x.buffer = u32(ti)
						append(out, x)
					}
					overrides += 1
				}
				break
			}
		}
	}
	return
}
