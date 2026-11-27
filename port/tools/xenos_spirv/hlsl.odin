package xenos

import "core:fmt"
import "core:strings"

// Xenos microcode -> HLSL (Shader Model 6.0, for DXC -> DXIL / Direct3D 12): the second backend of this package. It
// shares everything above the code generator with the SPIR-V backend (translate.odin): the container (container.odin),
// the decoded program and its facts (program.odin, ucode.odin), the host stage (exec_model), the register file
// (register_count), the literal constants (loop_literal_ids), the product zero rule (zero_rule), the sampler variants
// (sampler_variant), the VPOS sample position (param_gen_centred) and the structured control flow (structure_walk,
// through the sw_* hooks below). It emits the same operations in the same order as the SPIR-V backend, statement for
// instruction, with the same IEEE choices (ordered comparisons but `!=`, NaN-ignoring min / max / clamp, the Xenos
// zero rule on products), and fills the same Meta, which the pack builder compares field by field with the SPIR-V
// translation's (shaderpack --dxil). DXC compiles the text with -Gis (IEEE strictness: no fast-math, the HLSL
// counterpart of the SPIR-V NoContraction / no fast-math modules) and HLSL 2021.
//
// The binding ABI is abi.odin's, register = binding and space = set (D3D12 section there); the Vulkan buffer device
// addresses become raw buffer views (vertex streams t0-t15 space2, memexport ranges u0-u3 space2). Control flow:
//   straight-line programs: `do { ... } while (false)` (an exec_end before the last cf breaks out of it);
//   structured programs (structure.odin): nested `if` / `[loop] while (true)` as the SPIR-V selections and loops;
//   any other program: the dispatcher, `[loop] while (true) { switch (xe_pc) { case ci: { ... } } break; }` where a
//   jump is `xe_pc = target; continue;` and the end of the program a `break;` (out of the switch, then the loop).

// An HLSL translation. text: the whole module source; profile: the DXC target (vs_6_0 / ps_6_0 / cs_6_0 / ds_6_0).
HLSL_Result :: struct {
	text:    string,
	profile: string,
	meta:    Meta,
	error:   string,
}

// the interface slot of each ABI location (abi.odin D3D12 interface): 28 float4 slots, locations 0-27
HLSL_INTERFACE_SLOTS :: 28

@(private)
H :: struct {
	c:          ^Container,
	p:          ^Program,
	stage:      Stage,
	model:      Exec_Model,
	meta:       Meta,
	err:        string,
	// text: file-scope declarations (first use order), helper functions, the body of main
	decls:      strings.Builder,
	fns:        strings.Builder,
	body:       strings.Builder,
	indent:     int,
	tmp:        int,
	n_regs:     u32,
	reg_array:  bool, // aL-relative temp addressing: `float4 r[64]`, else r0..rN
	int_defs:   map[u32]u32,
	tex:        map[u32]bool,
	smp:        map[u32]bool,
	structured: bool,
	dispatch:   bool,
	open:       bool, // dispatcher: the current position is reachable (a jump did not just leave it)
	fn_vdecode, fn_mstore, fn_gamma: bool,
}

// ---- text ----

@(private)
hl_line :: proc(h: ^H, f: string, args: ..any) {
	for _ in 0 ..< h.indent {strings.write_byte(&h.body, '\t')}
	fmt.sbprintf(&h.body, f, ..args)
	strings.write_byte(&h.body, '\n')
}

@(private)
hl_open :: proc(h: ^H, f: string, args: ..any) {
	hl_line(h, f, ..args)
	h.indent += 1
}

@(private)
hl_close :: proc(h: ^H, s := "}") {
	h.indent -= 1
	hl_line(h, "%s", s)
}

// a new temporary of type ty holding expr
@(private)
hl_tmp :: proc(h: ^H, ty: string, expr: string) -> string {
	n := fmt.tprintf("t%d", h.tmp)
	h.tmp += 1
	hl_line(h, "%s %s = %s;", ty, n, expr)
	return n
}

// a float literal with exactly the bits of v (as the SPIR-V backend's const_f32)
hl_flit :: proc(v: f32) -> string {
	bits := transmute(u32)v
	if bits == 0 {return "0.0"}
	// integers below 2^24 in decimal (the range test first: converting a larger float to an integer is undefined)
	if bits != 0x80000000 && v > -16777216 && v < 16777216 {
		i := i64(v)
		if f32(i) == v {return fmt.tprintf("%d.0", i)}
	}
	return fmt.tprintf("asfloat(0x%08Xu)", bits)
}

@(private)
hl_ulit :: proc(v: u32) -> string {return fmt.tprintf("%du", v)}

@(private)
hl_vtype :: proc(n: int) -> string {
	switch n {
	case 1: return "float"
	case 2: return "float2"
	case 3: return "float3"
	}
	return "float4"
}

@(private)
hl_splat :: proc(n: int, v: f32) -> string {
	if n == 1 {return hl_flit(v)}
	return fmt.tprintf("((%s)%s)", hl_vtype(n), hl_flit(v))
}

@(rodata)
XYZW_STR := [4]string{"x", "y", "z", "w"}

@(private)
hl_comp :: proc(v: string, k: u32) -> string {
	c := XYZW_STR
	return fmt.tprintf("%s.%s", v, c[k & 3])
}

// ---- entry ----

translate_hlsl :: proc(c: ^Container, p: ^Program, allocator := context.allocator) -> (res: HLSL_Result) {
	context.allocator = allocator
	h: H
	h.c, h.p, h.stage = c, p, c.stage
	h.meta.param_gen = -1
	h.model = exec_model(c, p)
	h.meta.model = h.model
	h.meta.labels = p.has_labels
	h.meta.memexport = p.memexport
	h.meta.kills = p.kills
	h.meta.float_defs = len(c.float_defs)
	for d in c.float_defs {append(&h.meta.defs, d)}
	for v in c.vertex_elements {append(&h.meta.elements, v)}
	h.meta.uses_bda = (c.stage == .Vertex && p.vfetch_count > 0) || p.memexport
	h.int_defs = make(map[u32]u32, 4, context.temp_allocator)
	h.tex = make(map[u32]bool, 16, context.temp_allocator)
	h.smp = make(map[u32]bool, 16, context.temp_allocator)
	h.decls = strings.builder_make(context.temp_allocator)
	h.fns = strings.builder_make(context.temp_allocator)
	h.body = strings.builder_make(context.temp_allocator)
	{
		ids := loop_literal_ids(p)
		defer delete(ids)
		for d, k in c.int_defs {if k < len(ids) {h.int_defs[ids[k]] = d.value}}
	}
	h.n_regs = register_count(c, p)
	h.reg_array = p.temp_rel
	h.meta.registers = int(h.n_regs)
	h.structured = p.has_labels && structure_walk(&h, 0, len(p.cf), false)
	h.dispatch = p.has_labels && !h.structured

	h.indent = 1
	// state
	if h.reg_array {
		hl_line(&h, "float4 r[%d] = (float4[%d])0;", h.n_regs, h.n_regs)
	} else {
		for i in 0 ..< h.n_regs {hl_line(&h, "float4 r%d = 0.0;", i)}
	}
	hl_line(&h, "float xe_ps = 0.0;")
	hl_line(&h, "bool xe_p0 = false;")
	hl_line(&h, "int xe_a0 = 0;")
	hl_line(&h, "int4 xe_aL = 0;")
	hl_line(&h, "uint4 xe_lc = 0u;")
	hl_line(&h, "float xe_lod = 0.0;")
	hl_line(&h, "float3 xe_grad_h = 0.0;")
	hl_line(&h, "float3 xe_grad_v = 0.0;")
	switch h.model {
	case .Vertex, .Tess_Eval:
		for i in 0 ..< 16 {hl_line(&h, "float4 o%d = 0.0;", i)}
		hl_line(&h, "float4 oPos = 0.0;")
		hl_line(&h, "float4 oPts = 0.0;")
	case .Fragment:
		for i in 0 ..< 4 {hl_line(&h, "float4 oC%d = 0.0;", i)}
		hl_line(&h, "float4 oDepth = 0.0;")
	case .Compute:
	}
	if p.memexport {
		hl_line(&h, "float4 eA = 0.0;")
		for i in 0 ..< 5 {hl_line(&h, "float4 eM%d = 0.0;", i)}
		hl_line(&h, "uint xe_em_mask = 0u;")
	}
	if h.dispatch {hl_line(&h, "int xe_pc = 0;")}

	hl_stage_inputs(&h)

	// control flow
	if h.dispatch {
		hl_open(&h, "[loop] while (true) {{")
		hl_open(&h, "switch (xe_pc) {{")
		hl_open(&h, "case 0: {{")
		h.open = true
	} else if !h.structured {
		hl_open(&h, "do {{")
	}
	if h.structured {
		if !structure_walk(&h, 0, len(p.cf), true) && h.err == "" {h.err = "internal: the structured control flow did not translate"}
	} else {
		for &cf, ci in p.cf {
			if ci > 0 && h.dispatch && p.labels[ci] {
				// fall into the next case through the loop: pc = ci
				if h.open {
					hl_line(&h, "xe_pc = %d;", ci)
					hl_line(&h, "continue;")
				}
				hl_close(&h)
				hl_open(&h, "case %d: {{", ci)
			}
			h.open = true // (unreachable code after an exec_end / jump runs nowhere)
			hlsl_emit_cf(&h, &cf, ci)
			if h.err != "" {break}
		}
	}
	if h.dispatch {
		// the program ran off the end of the control flow: end
		hl_line(&h, "break;")
		hl_close(&h)
		hl_line(&h, "default:")
		hl_line(&h, "\tbreak;")
		hl_close(&h)
		hl_line(&h, "break;")
		hl_close(&h)
	} else if !h.structured {
		hl_close(&h, "} while (false);")
	}
	if p.memexport {hl_memexport_flush(&h)}
	hl_stage_outputs(&h)
	if h.err != "" {
		res.error = h.err
		return
	}
	res.meta = h.meta
	res.profile = hl_profile(h.model)
	res.text = hl_assemble(&h, allocator)
	return
}

@(private)
hl_profile :: proc(m: Exec_Model) -> string {
	switch m {
	case .Vertex: return "vs_6_0"
	case .Tess_Eval: return "ds_6_0"
	case .Fragment: return "ps_6_0"
	case .Compute: return "cs_6_0"
	}
	return ""
}

// One-call entry, as translate_shader.
translate_shader_hlsl :: proc(object: []u8, physical: []u8, stage: Stage, allocator := context.allocator) -> HLSL_Result {
	c, perr := container_parse(object, physical, stage, allocator)
	if perr != .None {return HLSL_Result{error = "container"}}
	defer container_destroy(&c)
	p, ok := program_build(&c, allocator)
	defer program_destroy(&p)
	if !ok {return HLSL_Result{error = p.error}}
	return translate_hlsl(&c, &p, allocator)
}

// ---- operands ----

// the register's lvalue
@(private)
hl_reg :: proc(h: ^H, reg: u32, rel: bool) -> string {
	if !h.reg_array {return fmt.tprintf("r%d", min(reg, h.n_regs - 1))}
	if rel {
		i := hl_tmp(h, "int", fmt.tprintf("clamp(%d + xe_aL.x, 0, %d)", reg, h.n_regs - 1))
		return fmt.tprintf("r[%s]", i)
	}
	return fmt.tprintf("r[%d]", reg >= h.n_regs ? h.n_regs - 1 : reg)
}

@(private)
hl_swizzle_str :: proc(swz: [4]u8) -> string {
	c := XYZW_STR
	return fmt.tprintf("%s%s%s%s", c[swz[0]], c[swz[1]], c[swz[2]], c[swz[3]])
}

// the operand's register / constant, unswizzled (a temporary, or an expression free of side effects)
@(private)
hl_operand_raw :: proc(h: ^H, o: ^Operand) -> string {
	if o.is_temp {return hl_tmp(h, "float4", hl_reg(h, o.reg, o.rel != 0))}
	if o.rel == 0 {
		// the last def of the register wins, as the SPIR-V backend's map
		lit := ""
		for d in h.c.float_defs {
			if d.reg != o.reg {continue}
			lit = fmt.tprintf("asfloat(uint4(0x%08Xu, 0x%08Xu, 0x%08Xu, 0x%08Xu))", d.value[0], d.value[1], d.value[2], d.value[3])
		}
		if lit != "" {return hl_tmp(h, "float4", lit)}
		return fmt.tprintf("xe_c[%d]", o.reg)
	}
	base := o.rel == 2 ? "xe_a0" : "xe_aL.x"
	i := hl_tmp(h, "int", fmt.tprintf("%s + %d", base, i32(o.reg)))
	return hl_tmp(h, "float4", fmt.tprintf("xe_c[clamp(%s, 0, 255)]", i))
}

@(private)
hl_modifiers :: proc(v: string, o: ^Operand) -> string {
	r := v
	if o.abs {r = fmt.tprintf("abs(%s)", r)}
	if o.negate {r = fmt.tprintf("-(%s)", r)}
	return r
}

@(private)
hl_operand :: proc(h: ^H, o: ^Operand) -> string {
	raw := hl_operand_raw(h, o)
	v := raw
	if o.swizzle != {0, 1, 2, 3} {v = fmt.tprintf("%s.%s", raw, hl_swizzle_str(o.swizzle))}
	if v == raw && !o.abs && !o.negate && strings.has_prefix(raw, "t") {return raw}
	return hl_tmp(h, "float4", hl_modifiers(v, o))
}

// scalar component k of the operand's swizzle (scalar ops: k 0 = `a`, 1 = `b`)
@(private)
hl_operand_scalar :: proc(h: ^H, raw: string, o: ^Operand, k: int) -> string {
	return hl_tmp(h, "float", hl_modifiers(hl_comp(raw, u32(o.swizzle[k])), o))
}

// Xenos multiplication: 0 * anything (including inf / NaN) = 0. The SPIR-V backend's test NMin(|a|, |x|) == 0 is
// written `a == 0 || x == 0`, the same for every input (NMin returns the non-NaN operand; both NaN: false either
// way) without depending on how a target treats NaN in min (DXIL FMin is minNum, DXC's SPIR-V FMin is undefined).
@(private)
hl_xmul :: proc(h: ^H, n: int, a, x: string, exact := true) -> string {
	ty := hl_vtype(n)
	prod := hl_tmp(h, ty, fmt.tprintf("%s * %s", a, x))
	if !exact {return prod}
	if n == 1 {return hl_tmp(h, ty, fmt.tprintf("((%s == 0.0) || (%s == 0.0)) ? 0.0 : %s", a, x, prod))}
	z := hl_tmp(h, fmt.tprintf("bool%d", n), fmt.tprintf("or(%s == 0.0, %s == 0.0)", a, x))
	return hl_tmp(h, ty, fmt.tprintf("select(%s, %s, %s)", z, hl_splat(n, 0), prod))
}

@(private)
hl_cmp_select :: proc(h: ^H, n: int, op: string, x, y, if_true, if_false: string) -> string {
	if n == 1 {return hl_tmp(h, "float", fmt.tprintf("(%s %s %s) ? %s : %s", x, op, y, if_true, if_false))}
	return hl_tmp(h, hl_vtype(n), fmt.tprintf("select(%s %s %s, %s, %s)", x, op, y, if_true, if_false))
}

@(private)
hl_demote :: proc(h: ^H, cond: string) {
	if h.model != .Fragment {return}
	hl_open(h, "if (%s) {{", cond)
	if h.p.memexport {hl_memexport_flush(h)}
	hl_line(h, "discard;")
	hl_close(h)
}

// ---- control flow ----

@(private)
hl_bool_constant :: proc(index: u32) -> string {
	dw := index >> 5
	return fmt.tprintf("((xe_b[%d].%s & 0x%08Xu) != 0u)", dw >> 2, XYZW_STR[dw & 3], u32(1) << (index & 31))
}

@(private)
hl_loop_constant :: proc(h: ^H, id_: u32) -> string {
	if v, ok := h.int_defs[id_]; ok {return hl_ulit(v)}
	return fmt.tprintf("xe_i[%d].%s", id_ >> 2, XYZW_STR[id_ & 3])
}

// the cf's condition as a bool expression, "" when unconditional
@(private)
hl_condition :: proc(h: ^H, cf: ^Cf) -> string {
	#partial switch cf.op {
	case .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		c := hl_bool_constant(cf.bool_index)
		return cf.condition ? c : fmt.tprintf("!%s", c)
	case .Cond_Exec_Pred, .Cond_Exec_Pred_End:
		return cf.condition ? "xe_p0" : "!xe_p0"
	case .Cond_Jmp, .Cond_Call:
		if cf.unconditional {return ""}
		if cf.predicated {return cf.condition ? "xe_p0" : "!xe_p0"}
		c := hl_bool_constant(cf.bool_index)
		return cf.condition ? c : fmt.tprintf("!%s", c)
	}
	return ""
}

@(private)
hl_goto_pc :: proc(h: ^H, target: u32) {
	hl_line(h, "xe_pc = %d;", target)
	hl_line(h, "continue;")
	h.open = false
}

// the end of the program: out of the dispatcher's switch (then its loop), or out of the straight-line do / while
@(private)
hl_end_program :: proc(h: ^H) {
	hl_line(h, "break;")
	h.open = false
}

hlsl_emit_cf :: proc(h: ^H, cf: ^Cf, ci: int) {
	p := h.p
	switch cf.op {
	case .Exec, .Exec_End, .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		c := hl_condition(h, cf)
		if c != "" {hl_open(h, "if (%s) {{", c)}
		hl_emit_instrs(h, p.exec_first[ci], int(cf.count))
		if h.err != "" {return}
		// a conditional *_end ends the program only when its condition holds (Xenia ProcessExecInstructionEnd)
		// (structured: nothing executable follows the end, so falling through ends the program)
		if cf_ends_shader(cf.op) && c != "" && !h.structured {hl_end_program(h)}
		if c != "" {hl_close(h); h.open = true}
		if cf_ends_shader(cf.op) && c == "" && !h.structured {hl_end_program(h)}
	case .Loop_Start:
		if !p.has_labels {h.err = "loop without labels"; return}
		old_lc, old_al := hl_loop_push(h, cf)
		hl_open(h, "if (%s == 0u) {{", old_lc.count)
		// skipping the loop pops what was pushed (the loop_end that would pop is jumped over)
		hl_line(h, "xe_lc = %s;", old_lc.old)
		hl_line(h, "xe_aL = %s;", old_al)
		hl_goto_pc(h, cf.address)
		hl_close(h)
		h.open = true
	case .Loop_End:
		if !p.has_labels {h.err = "loop without labels"; return}
		old_lc, count, cont, old_al := hl_loop_end_test(h, cf)
		hl_open(h, "if (%s) {{", cont)
		hl_loop_step(h, cf, old_lc, count, old_al)
		hl_goto_pc(h, cf.address)
		hl_close(h)
		h.open = true
		// leave: pop
		hl_line(h, "xe_lc = uint4(%s.yzw, 0u);", old_lc)
		hl_line(h, "xe_aL = int4(%s.yzw, 0);", old_al)
	case .Cond_Jmp:
		c := hl_condition(h, cf)
		if c == "" {
			hl_goto_pc(h, cf.address)
		} else {
			hl_open(h, "if (%s) {{", c)
			hl_goto_pc(h, cf.address)
			hl_close(h)
			h.open = true
		}
	case .Cond_Call, .Return:
		h.err = "call / return (not used by Halo 3)"
	case .Alloc:
		// every alloc commits the pending eM# (Xenia ProcessAllocInstruction); a new `alloc export` also starts with an
		// invalid eA so a stream that is never addressed writes nothing
		if p.memexport {
			hl_memexport_flush(h)
			if cf.alloc_type == 3 {hl_line(h, "eA = 0.0;")}
		}
	case .Nop, .Mark_Vs_Fetch_Done:
	}
}

@(private)
HL_Loop_Push :: struct {
	count: string, // the new count (uint)
	old:   string, // the loop counter stack before the push
}

// loop_start: push the loop counter (count) and aL (start, or aL itself for a repeat loop)
@(private)
hl_loop_push :: proc(h: ^H, cf: ^Cf) -> (HL_Loop_Push, string) {
	lc := hl_tmp(h, "uint", hl_loop_constant(h, cf.loop_id))
	count := hl_tmp(h, "uint", fmt.tprintf("%s & 0xFFu", lc))
	old_lc := hl_tmp(h, "uint4", "xe_lc")
	hl_line(h, "xe_lc = uint4(%s, %s.xyz);", count, old_lc)
	old_al := hl_tmp(h, "int4", "xe_aL")
	start := cf.is_repeat ? fmt.tprintf("%s.x", old_al) : fmt.tprintf("int((%s >> 8u) & 0xFFu)", lc)
	hl_line(h, "xe_aL = int4(%s, %s.xyz);", start, old_al)
	return HL_Loop_Push{count = count, old = old_lc}, old_al
}

// loop_end's decision: count - 1 != 0 and no predicated break continues the loop
@(private)
hl_loop_end_test :: proc(h: ^H, cf: ^Cf) -> (old_lc, count, cont, old_al: string) {
	old_lc = hl_tmp(h, "uint4", "xe_lc")
	count = hl_tmp(h, "uint", fmt.tprintf("%s.x - 1u", old_lc))
	again := fmt.tprintf("%s != 0u", count)
	if cf.pred_break {
		brk := cf.condition ? "xe_p0" : "!xe_p0"
		again = fmt.tprintf("(%s) && !(%s)", again, brk)
	}
	cont = hl_tmp(h, "bool", again)
	old_al = hl_tmp(h, "int4", "xe_aL")
	return
}

// the loop continues: the counter and aL stepped
@(private)
hl_loop_step :: proc(h: ^H, cf: ^Cf, old_lc, count, old_al: string) {
	hl_line(h, "xe_lc = uint4(%s, %s.yzw);", count, old_lc)
	lcv := hl_tmp(h, "uint", hl_loop_constant(h, cf.loop_id))
	step := hl_tmp(h, "int", fmt.tprintf("int(%s << 8u) >> 24", lcv)) // bits 16-23, signed
	hl_line(h, "xe_aL = int4(%s.x + %s, %s.yzw);", old_al, step, old_al)
}

// ---- the structured control flow hooks (structure.odin) ----

hlsl_sw_if_not_begin :: proc(h: ^H, cf: ^Cf) -> Sw_Token {
	hl_open(h, "if (!(%s)) {{", hl_condition(h, cf))
	return {}
}

hlsl_sw_if_end :: proc(h: ^H, f: Sw_Token) {hl_close(h)}

hlsl_sw_if_not_else_begin :: proc(h: ^H, cf: ^Cf) -> Sw_Token {
	hl_open(h, "if (!(%s)) {{", hl_condition(h, cf))
	return {}
}

hlsl_sw_if_else_middle :: proc(h: ^H, f: Sw_Token) {
	h.indent -= 1
	hl_line(h, "}} else {{")
	h.indent += 1
}

hlsl_sw_if_else_end :: proc(h: ^H, f: Sw_Token) {hl_close(h)}

// loop_start: as the dispatcher's push, then `if (count == 0) pop else <loop>`
hlsl_sw_loop_begin :: proc(h: ^H, cf: ^Cf) -> Sw_Token {
	push, old_al := hl_loop_push(h, cf)
	hl_open(h, "if (%s == 0u) {{", push.count)
	hl_line(h, "xe_lc = %s;", push.old)
	hl_line(h, "xe_aL = %s;", old_al)
	h.indent -= 1
	hl_line(h, "}} else {{")
	h.indent += 1
	hl_open(h, "[loop] while (true) {{")
	return {}
}

// loop_end: the dispatcher's decision continues the loop with the counter and aL stepped, or leaves it popping both
hlsl_sw_loop_end :: proc(h: ^H, cf: ^Cf, f: Sw_Token) {
	old_lc, count, cont, old_al := hl_loop_end_test(h, cf)
	hl_line(h, "if (!%s) break;", cont)
	hl_loop_step(h, cf, old_lc, count, old_al)
	hl_close(h)
	lc2 := hl_tmp(h, "uint4", "xe_lc")
	al2 := hl_tmp(h, "int4", "xe_aL")
	hl_line(h, "xe_lc = uint4(%s.yzw, 0u);", lc2)
	hl_line(h, "xe_aL = int4(%s.yzw, 0);", al2)
	hl_close(h)
}

// ---- instructions ----

@(private)
hl_emit_instr_body :: proc(h: ^H, in_: ^Instr) {
	switch in_.kind {
	case .Alu: hl_emit_alu(h, &in_.alu)
	case .Vfetch: hl_emit_vfetch(h, in_)
	case .Tfetch: hl_emit_tfetch(h, in_)
	}
}

// The instructions of an exec in order; a run predicated on the same p0 value shares one `if` (as emit_instrs).
@(private)
hl_emit_instrs :: proc(h: ^H, first, count: int) {
	k := 0
	for k < count {
		in_ := &h.p.instrs[first + k]
		predicated, condition := instr_predicate(in_)
		if !predicated {
			hl_emit_instr_body(h, in_)
			k += 1
		} else {
			hl_open(h, "if (%sxe_p0) {{", condition ? "" : "!")
			for k < count {
				run := &h.p.instrs[first + k]
				p2, c2 := instr_predicate(run)
				if !p2 || c2 != condition {break}
				hl_emit_instr_body(h, run)
				k += 1
				if instr_writes_predicate(run) || h.err != "" {break}
			}
			hl_close(h)
		}
		if h.err != "" {return}
	}
}

// write `value` (a float4) into the components of `mask` of the lvalue
@(private)
hl_store_masked :: proc(h: ^H, lv: string, value: string, mask: u32) {
	if mask == 0 {return}
	if mask == 0xf {hl_line(h, "%s = %s;", lv, value); return}
	sb := strings.builder_make(context.temp_allocator)
	c := XYZW_STR
	for k in u32(0) ..< 4 {if mask & (1 << k) != 0 {strings.write_string(&sb, c[k])}}
	m := strings.to_string(sb)
	hl_line(h, "%s.%s = %s.%s;", lv, m, value, m)
}

// write the scalar `value` into the components of `mask` (a splat)
@(private)
hl_store_splat :: proc(h: ^H, lv: string, value: string, mask: u32) {
	if mask == 0 {return}
	if mask == 0xf {hl_line(h, "%s = %s;", lv, value); return}
	sb := strings.builder_make(context.temp_allocator)
	c := XYZW_STR
	for k in u32(0) ..< 4 {if mask & (1 << k) != 0 {strings.write_string(&sb, c[k])}}
	hl_line(h, "%s.%s = %s;", lv, strings.to_string(sb), value)
}

@(private)
hl_emit_alu :: proc(h: ^H, a: ^Alu) {
	// all sources are read before any result is written (co-issue)
	nv := int(vec_op_operands[a.vec_op])
	vm := alu_vec_result_mask(a)
	sm := alu_sca_result_mask(a)
	c0 := alu_const0_mask(a)
	c1 := alu_const1_mask(a)
	vec_state := a.vec_op == .Max_A || (a.vec_op >= .Setp_Eq_Push && a.vec_op <= .Setp_Ge_Push) || (a.vec_op >= .Kill_Eq && a.vec_op <= .Kill_Ne)
	vres := ""
	if vm != 0 || vec_state {
		src: [3]string
		for i in 0 ..< nv {src[i] = hl_operand(h, &a.vec_src[i])}
		vres = hl_vector_op(h, a, src)
		if vres != "" && a.vec_clamp {vres = hl_tmp(h, "float4", fmt.tprintf("saturate(%s)", vres))}
	}
	sres := "" // scalar result (float)
	if a.sca_op != .Retain_Prev {
		sres = hl_scalar_op(h, a)
		if sres != "" {
			hl_line(h, "xe_ps = %s;", sres) // the previous scalar is the unclamped result (Xenia)
			if a.sca_clamp {sres = hl_tmp(h, "float", fmt.tprintf("saturate(%s)", sres))}
		}
	} else if sm != 0 {
		sres = hl_tmp(h, "float", "xe_ps")
		if a.sca_clamp {sres = hl_tmp(h, "float", fmt.tprintf("saturate(%s)", sres))}
	}
	if h.err != "" {return}
	if a.is_export {
		// the float constant holding a memexport stream constant (as emit_alu)
		if a.vec_dest == EXPORT_ADDRESS && h.p.memexport {
			for i := nv - 1; i >= 0; i -= 1 {
				o := &a.vec_src[i]
				if o.is_temp || o.rel != 0 {continue}
				found := false
				for r in h.meta.memexport_streams {if r == o.reg {found = true}}
				if !found {append(&h.meta.memexport_streams, o.reg)}
				break
			}
		}
		lv := hl_export_lvalue(h, a.vec_dest)
		if lv == "" {return}
		if vres != "" {hl_store_masked(h, lv, vres, vm)}
		if sres != "" && sm != 0 {hl_store_splat(h, lv, sres, sm)}
		if c0 != 0 {hl_store_splat(h, lv, "0.0", c0)}
		if c1 != 0 {hl_store_splat(h, lv, "1.0", c1)}
		if a.vec_dest >= EXPORT_DATA0 && a.vec_dest <= EXPORT_DATA0 + 4 && h.p.memexport {
			hl_line(h, "xe_em_mask = xe_em_mask | %s;", hl_ulit(1 << (a.vec_dest - EXPORT_DATA0)))
		}
		return
	}
	if vres != "" && vm != 0 {hl_store_masked(h, hl_reg(h, a.vec_dest, a.vec_dest_rel), vres, vm)}
	if sres != "" && sm != 0 {hl_store_splat(h, hl_reg(h, a.sca_dest, a.sca_dest_rel), sres, sm)}
}

// the export register's variable ("" for none), recording the outputs in the meta as export_ptr does
@(private)
hl_export_lvalue :: proc(h: ^H, reg: u32) -> string {
	switch {
	case reg == EXPORT_ADDRESS:
		return h.p.memexport ? "eA" : ""
	case reg >= EXPORT_DATA0 && reg <= EXPORT_DATA0 + 4:
		return h.p.memexport ? fmt.tprintf("eM%d", reg - EXPORT_DATA0) : ""
	}
	switch h.model {
	case .Vertex, .Tess_Eval:
		if reg < 16 {return fmt.tprintf("o%d", reg)}
		if reg == EXPORT_VS_POSITION {return "oPos"}
		if reg == EXPORT_VS_POINT_SIZE {h.meta.point_size = true; return "oPts"}
	case .Fragment:
		if reg < 4 {h.meta.color_outputs |= 1 << reg; return fmt.tprintf("oC%d", reg)}
		if reg == EXPORT_PS_DEPTH {h.meta.depth_output = true; return "oDepth"}
	case .Compute:
	}
	return ""
}

@(private)
hl_vector_op :: proc(h: ^H, a: ^Alu, s: [3]string) -> string {
	one4 := hl_splat(4, 1)
	zero4 := hl_splat(4, 0)
	switch a.vec_op {
	case .Add: return hl_tmp(h, "float4", fmt.tprintf("%s + %s", s[0], s[1]))
	case .Mul: return hl_xmul(h, 4, s[0], s[1], zero_rule(h.c.float_defs[:], &a.vec_src[0], &a.vec_src[1]))
	case .Mad:
		m := hl_xmul(h, 4, s[0], s[1], zero_rule(h.c.float_defs[:], &a.vec_src[0], &a.vec_src[1]))
		return hl_tmp(h, "float4", fmt.tprintf("%s + %s", m, s[2]))
	case .Max: return hl_cmp_select(h, 4, ">=", s[0], s[1], s[0], s[1])
	case .Min: return hl_cmp_select(h, 4, "<", s[0], s[1], s[0], s[1])
	case .Seq: return hl_cmp_select(h, 4, "==", s[0], s[1], one4, zero4)
	case .Sgt: return hl_cmp_select(h, 4, ">", s[0], s[1], one4, zero4)
	case .Sge: return hl_cmp_select(h, 4, ">=", s[0], s[1], one4, zero4)
	case .Sne: return hl_cmp_select(h, 4, "!=", s[0], s[1], one4, zero4)
	case .Frc: return hl_tmp(h, "float4", fmt.tprintf("frac(%s)", s[0]))
	case .Trunc: return hl_tmp(h, "float4", fmt.tprintf("trunc(%s)", s[0]))
	case .Floor: return hl_tmp(h, "float4", fmt.tprintf("floor(%s)", s[0]))
	case .Cnd_Eq: return hl_cmp_select(h, 4, "==", s[0], zero4, s[1], s[2])
	case .Cnd_Ge: return hl_cmp_select(h, 4, ">=", s[0], zero4, s[1], s[2])
	case .Cnd_Gt: return hl_cmp_select(h, 4, ">", s[0], zero4, s[1], s[2])
	case .Dp4, .Dp3, .Dp2_Add:
		n := a.vec_op == .Dp4 ? 4 : (a.vec_op == .Dp3 ? 3 : 2)
		prod := hl_xmul(h, 4, s[0], s[1], zero_rule(h.c.float_defs[:], &a.vec_src[0], &a.vec_src[1]))
		sum := hl_tmp(h, "float", hl_comp(prod, 0))
		for k in 1 ..< n {sum = hl_tmp(h, "float", fmt.tprintf("%s + %s", sum, hl_comp(prod, u32(k))))}
		if a.vec_op == .Dp2_Add {sum = hl_tmp(h, "float", fmt.tprintf("%s + %s", sum, hl_comp(s[2], 0)))}
		return hl_tmp(h, "float4", fmt.tprintf("(float4)%s", sum))
	case .Cube:
		// operand 0: x = src.z, y = src.w, z = src.x (Xenia ProcessVectorAluOperation kCube)
		x := hl_tmp(h, "float", hl_comp(s[0], 2))
		y := hl_tmp(h, "float", hl_comp(s[0], 3))
		z := hl_tmp(h, "float", hl_comp(s[0], 0))
		ax := hl_tmp(h, "float", fmt.tprintf("abs(%s)", x))
		ay := hl_tmp(h, "float", fmt.tprintf("abs(%s)", y))
		az := hl_tmp(h, "float", fmt.tprintf("abs(%s)", z))
		nx := hl_tmp(h, "float", fmt.tprintf("-%s", x))
		ny := hl_tmp(h, "float", fmt.tprintf("-%s", y))
		nz := hl_tmp(h, "float", fmt.tprintf("-%s", z))
		is_z := hl_tmp(h, "bool", fmt.tprintf("(%s >= %s) && (%s >= %s)", az, ax, az, ay))
		is_y := hl_tmp(h, "bool", fmt.tprintf("%s >= %s", ay, ax))
		zneg := hl_tmp(h, "bool", fmt.tprintf("%s < 0.0", z))
		yneg := hl_tmp(h, "bool", fmt.tprintf("%s < 0.0", y))
		xneg := hl_tmp(h, "bool", fmt.tprintf("%s < 0.0", x))
		sel :: proc(h: ^H, c, x, y: string) -> string {return hl_tmp(h, "float", fmt.tprintf("%s ? %s : %s", c, x, y))}
		// z major: (-y, z<0 ? -x : x, z, z<0 ? 5 : 4)
		zr := [4]string{ny, sel(h, zneg, nx, x), z, sel(h, zneg, "5.0", "4.0")}
		// y major: (y<0 ? -z : z, x, y, y<0 ? 3 : 2)
		yr := [4]string{sel(h, yneg, nz, z), x, y, sel(h, yneg, "3.0", "2.0")}
		// x major: (-y, x<0 ? z : -z, x, x<0 ? 1 : 0)
		xr := [4]string{ny, sel(h, xneg, z, nz), x, sel(h, xneg, "1.0", "0.0")}
		r: [4]string
		for k in 0 ..< 4 {r[k] = sel(h, is_z, zr[k], sel(h, is_y, yr[k], xr[k]))}
		r[2] = hl_tmp(h, "float", fmt.tprintf("%s * 2.0", r[2]))
		return hl_tmp(h, "float4", fmt.tprintf("float4(%s, %s, %s, %s)", r[0], r[1], r[2], r[3]))
	case .Max4:
		m := hl_tmp(h, "float", hl_comp(s[0], 0))
		for k in u32(1) ..< 4 {m = hl_tmp(h, "float", fmt.tprintf("max(%s, %s)", m, hl_comp(s[0], k)))}
		return hl_tmp(h, "float4", fmt.tprintf("(float4)%s", m))
	case .Setp_Eq_Push, .Setp_Ne_Push, .Setp_Gt_Push, .Setp_Ge_Push:
		ops := [4]string{"==", "!=", ">", ">="}
		op := ops[int(a.vec_op) - int(Vec_Op.Setp_Eq_Push)]
		hl_line(h, "xe_p0 = (%s == 0.0) && (%s %s 0.0);", hl_comp(s[0], 3), hl_comp(s[1], 3), op)
		c := hl_tmp(h, "bool", fmt.tprintf("(%s == 0.0) && (%s %s 0.0)", hl_comp(s[0], 0), hl_comp(s[1], 0), op))
		v := hl_tmp(h, "float", fmt.tprintf("(%s ? -1.0 : %s) + 1.0", c, hl_comp(s[0], 0)))
		return hl_tmp(h, "float4", fmt.tprintf("(float4)%s", v))
	case .Kill_Eq, .Kill_Gt, .Kill_Ge, .Kill_Ne:
		ops := [4]string{"==", ">", ">=", "!="}
		c := hl_tmp(h, "bool", fmt.tprintf("any(%s %s %s)", s[0], ops[int(a.vec_op) - int(Vec_Op.Kill_Eq)], s[1]))
		hl_demote(h, c)
		return hl_tmp(h, "float4", fmt.tprintf("(float4)(%s ? 1.0 : 0.0)", c))
	case .Dst:
		y := hl_xmul(h, 1, hl_comp(s[0], 1), hl_comp(s[1], 1))
		return hl_tmp(h, "float4", fmt.tprintf("float4(1.0, %s, %s, %s)", y, hl_comp(s[0], 2), hl_comp(s[1], 3)))
	case .Max_A:
		fl := hl_tmp(h, "float", fmt.tprintf("floor(%s + 0.5)", hl_comp(s[0], 3)))
		hl_line(h, "xe_a0 = int(clamp(%s, -256.0, 255.0));", fl)
		return hl_cmp_select(h, 4, ">=", s[0], s[1], s[0], s[1])
	case .Op30, .Op31:
		return ""
	}
	return ""
}

@(private)
hl_scalar_op :: proc(h: ^H, a: ^Alu) -> string {
	av, bv: string
	if a.sca_src_count == 1 {
		raw := hl_operand_raw(h, &a.sca_src[0])
		av = hl_operand_scalar(h, raw, &a.sca_src[0], 0)
		if sca_op_operands[a.sca_op] == 2 {bv = hl_operand_scalar(h, raw, &a.sca_src[0], 1)}
	} else if a.sca_src_count == 2 {
		av = hl_operand_scalar(h, hl_operand_raw(h, &a.sca_src[0]), &a.sca_src[0], 0)
		bv = hl_operand_scalar(h, hl_operand_raw(h, &a.sca_src[1]), &a.sca_src[1], 0)
	}
	f :: proc(h: ^H, e: string, args: ..any) -> string {return hl_tmp(h, "float", fmt.tprintf(e, ..args))}
	ps :: proc(h: ^H) -> string {return hl_tmp(h, "float", "xe_ps")}
	FLT_MAX :: f32(3.4028234663852886e38)
	INF := transmute(f32)u32(0x7f800000)
	switch a.sca_op {
	case .Adds: return f(h, "%s + %s", av, bv)
	case .Adds_Prev: return f(h, "%s + %s", av, ps(h))
	case .Muls: return hl_xmul(h, 1, av, bv)
	case .Muls_Prev: return hl_xmul(h, 1, av, ps(h))
	case .Muls_Prev2:
		// Xenia: -FLT_MAX unless ps != -FLT_MAX, ps and b finite, and b > 0; then a * ps
		pv := ps(h)
		nmax := hl_flit(-FLT_MAX)
		c := hl_tmp(h, "bool", fmt.tprintf("(%s != %s) && (-abs(%s) >= %s)", pv, nmax, pv, nmax))
		c = hl_tmp(h, "bool", fmt.tprintf("%s && ((-abs(%s) >= %s) && (%s > 0.0))", c, bv, nmax, bv))
		x := a.sca_src[0].swizzle[0] == a.sca_src[0].swizzle[1] ? bv : av
		m := hl_xmul(h, 1, x, pv)
		return f(h, "%s ? %s : %s", c, m, nmax)
	case .Maxs: return f(h, "(%s >= %s) ? %s : %s", av, bv, av, bv)
	case .Mins: return f(h, "(%s < %s) ? %s : %s", av, bv, av, bv)
	case .Seqs: return f(h, "(%s == 0.0) ? 1.0 : 0.0", av)
	case .Sgts: return f(h, "(%s > 0.0) ? 1.0 : 0.0", av)
	case .Sges: return f(h, "(%s >= 0.0) ? 1.0 : 0.0", av)
	case .Snes: return f(h, "(%s != 0.0) ? 1.0 : 0.0", av)
	case .Frcs: return f(h, "frac(%s)", av)
	case .Truncs: return f(h, "trunc(%s)", av)
	case .Floors: return f(h, "floor(%s)", av)
	case .Exp: return f(h, "exp2(%s)", av)
	case .Log: return f(h, "log2(%s)", av)
	case .Logc:
		r := f(h, "log2(%s)", av)
		return f(h, "(%s == %s) ? %s : %s", r, hl_flit(-INF), hl_flit(-FLT_MAX), r)
	case .Rcp: return f(h, "1.0 / %s", av)
	case .Rcpc, .Rsqc:
		r := a.sca_op == .Rcpc ? f(h, "1.0 / %s", av) : f(h, "rsqrt(%s)", av)
		r = f(h, "(%s == %s) ? %s : %s", r, hl_flit(-INF), hl_flit(-FLT_MAX), r)
		return f(h, "(%s == %s) ? %s : %s", r, hl_flit(INF), hl_flit(FLT_MAX), r)
	case .Rcpf, .Rsqf:
		r := a.sca_op == .Rcpf ? f(h, "1.0 / %s", av) : f(h, "rsqrt(%s)", av)
		r = f(h, "(%s == %s) ? 0.0 : %s", r, hl_flit(INF), r)
		return f(h, "(%s == %s) ? %s : %s", r, hl_flit(-INF), hl_flit(-0.0), r)
	case .Rsq: return f(h, "rsqrt(%s)", av)
	case .Max_As, .Max_Asf:
		addr := a.sca_op == .Max_As ? f(h, "%s + 0.5", av) : av
		hl_line(h, "xe_a0 = int(clamp(floor(%s), 0.0, 255.0));", addr)
		return f(h, "(%s >= %s) ? %s : %s", av, bv, av, bv)
	case .Subs: return f(h, "%s - %s", av, bv)
	case .Subs_Prev: return f(h, "%s - %s", av, ps(h))
	case .Setp_Eq, .Setp_Ne, .Setp_Gt, .Setp_Ge:
		ops := [4]string{"==", "!=", ">", ">="}
		pr := hl_tmp(h, "bool", fmt.tprintf("%s %s 0.0", av, ops[int(a.sca_op) - int(Sca_Op.Setp_Eq)]))
		hl_line(h, "xe_p0 = %s;", pr)
		return f(h, "%s ? 0.0 : 1.0", pr)
	case .Setp_Inv:
		pr := hl_tmp(h, "bool", fmt.tprintf("%s == 1.0", av))
		hl_line(h, "xe_p0 = %s;", pr)
		inner := f(h, "(%s == 0.0) ? 1.0 : %s", av, av)
		return f(h, "%s ? 0.0 : %s", pr, inner)
	case .Setp_Pop:
		m := f(h, "%s - 1.0", av)
		pr := hl_tmp(h, "bool", fmt.tprintf("%s <= 0.0", m))
		hl_line(h, "xe_p0 = %s;", pr)
		return f(h, "%s ? 0.0 : %s", pr, m)
	case .Setp_Clr:
		hl_line(h, "xe_p0 = false;")
		return hl_flit(FLT_MAX)
	case .Setp_Rstr:
		pr := hl_tmp(h, "bool", fmt.tprintf("%s == 0.0", av))
		hl_line(h, "xe_p0 = %s;", pr)
		return f(h, "%s ? 0.0 : %s", pr, av)
	case .Kills_Eq, .Kills_Gt, .Kills_Ge, .Kills_Ne, .Kills_One:
		ops := [5]string{"==", ">", ">=", "!=", "=="}
		c := hl_tmp(h, "bool", fmt.tprintf("%s %s %s", av, ops[int(a.sca_op) - int(Sca_Op.Kills_Eq)], a.sca_op == .Kills_One ? "1.0" : "0.0"))
		hl_demote(h, c)
		return f(h, "%s ? 1.0 : 0.0", c)
	case .Sqrt: return f(h, "sqrt(%s)", av)
	case .Mulsc0, .Mulsc1: return hl_xmul(h, 1, av, bv)
	case .Addsc0, .Addsc1: return f(h, "%s + %s", av, bv)
	case .Subsc0, .Subsc1: return f(h, "%s - %s", av, bv)
	case .Sin: return f(h, "sin(%s)", av)
	case .Cos: return f(h, "cos(%s)", av)
	case .Retain_Prev: return ""
	case .Op41, .Op51, .Op52, .Op53, .Op54, .Op55, .Op56, .Op57, .Op58, .Op59, .Op60, .Op61, .Op62, .Op63:
		return "0.0"
	}
	return ""
}

// fetch destination swizzle: 0-3 component, 4 zero, 5 one, 6 (invalid) zero, 7 keep (as store_fetch_result)
@(private)
hl_store_fetch_result :: proc(h: ^H, dst: u32, rel: bool, dst_swz: u32, value: string) {
	mask: u32
	comps: [4]string
	for k in u32(0) ..< 4 {
		s := fetch_dst_component(dst_swz, k)
		switch s {
		case 0 ..= 3: comps[k] = hl_comp(value, s); mask |= 1 << k
		case 5: comps[k] = "1.0"; mask |= 1 << k
		case 7: comps[k] = "0.0"
		case: comps[k] = "0.0"; mask |= 1 << k
		}
	}
	if mask == 0 {return}
	v := hl_tmp(h, "float4", fmt.tprintf("float4(%s, %s, %s, %s)", comps[0], comps[1], comps[2], comps[3]))
	hl_store_masked(h, hl_reg(h, dst, rel), v, mask)
}
