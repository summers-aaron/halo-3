package xenos

// Structured control flow for programs with labels. The Xenos control flow program jumps (cjmp) and loops
// (loop_start / loop_end) by cf index; the general translation runs it as Xenia does, in a `loop { switch (pc) }`
// dispatcher, which is correct for any program but leaves the driver's compiler an irreducible-looking state machine
// whose every case keeps every register live (the slowest modules of the pack to compile, by far). Halo 3's programs
// only use the shapes the shader compiler emits for structured HLSL, so most of them translate to nested SPIR-V
// selections and loops instead:
//
//   cjmp !c -> T  (forward, T inside the enclosing region)          if (!c) { [i+1, T) }
//   cjmp !c -> T  ... [T-1] jmp -> T2  (T2 > T)                      if (!c) { [i+1, T-1) } else { [T, T2) }
//   jmp -> T  with nothing else entering [i+1, T)                    [i+1, T) is unreachable: skipped
//   loop_start iN skip=e+1 / [e] loop_end iN body=s+1                the loop counter / aL push, then a SPIR-V loop
//                                                                    whose last block continues or breaks
//   exec_end (or a conditional one) with nothing executable after it: falls through to the end
//
// Each region must be single-entry: no jump from outside it lands inside (its first cf only through the construct).
// structure_walk checks a program once without emitting (emit = false); any other shape (a backward jump, a jump out
// of a loop, an end in the middle of the program, a call) keeps the dispatcher. Both translations run the same
// instructions in the same order with the same condition evaluations: only the branching between cf blocks differs.

// No cf outside [owner_lo, owner_hi] but `except` transfers control to a cf in [lo, hi).
@(private)
structure_no_entry :: proc(p: ^Program, owner_lo, owner_hi: int, lo, hi: int, except := -1) -> bool {
	for &cf, j in p.cf {
		if (j >= owner_lo && j <= owner_hi) || j == except {continue}
		target := -1
		#partial switch cf.op {
		case .Cond_Jmp: target = int(cf.address)
		case .Loop_Start: target = int(cf.address) // the skip address (after its loop_end)
		case .Loop_End: target = int(cf.address) // the body
		case .Cond_Call: target = int(cf.address)
		}
		if target >= lo && target < hi {return false}
	}
	return true
}

// Whether anything after cf index i would execute an instruction or transfer control.
@(private)
structure_tail_empty :: proc(p: ^Program, i: int) -> bool {
	for k in i + 1 ..< len(p.cf) {
		#partial switch p.cf[k].op {
		case .Nop, .Alloc, .Mark_Vs_Fetch_Done:
		case:
			if cf_is_exec(p.cf[k].op) && p.cf[k].count == 0 {continue}
			return false
		}
	}
	return true
}

// The walker is shared by both backends (translate.odin: SPIR-V, hlsl.odin: HLSL for DXIL): it is parametric in the
// translator state E (^T or ^H), which has the fields `p` (the program) and `err`, and emits through the hooks below,
// one procedure group per construct (a backend's hook gets its own state). A hook that opens a construct returns
// what the matching hooks need to close it (Sw_Token: the SPIR-V merge / continue labels; HLSL needs nothing).
Sw_Token :: [4]u32

sw_if_not_begin :: proc{spv_sw_if_not_begin, hlsl_sw_if_not_begin} // if (!condition(cf)) {
sw_if_end :: proc{spv_sw_if_end, hlsl_sw_if_end} // }
sw_if_not_else_begin :: proc{spv_sw_if_not_else_begin, hlsl_sw_if_not_else_begin} // if (!condition(cf)) {
sw_if_else_middle :: proc{spv_sw_if_else_middle, hlsl_sw_if_else_middle} // } else {
sw_if_else_end :: proc{spv_sw_if_else_end, hlsl_sw_if_else_end} // }
sw_loop_begin :: proc{spv_sw_loop_begin, hlsl_sw_loop_begin} // loop_start: push, skip test, the loop header
sw_loop_end :: proc{spv_sw_loop_end, hlsl_sw_loop_end} // loop_end: continue or leave, pop
sw_emit_cf :: proc{emit_cf, hlsl_emit_cf} // any other cf

// Walks (and with emit, translates) the cf region [a, b). false: the region is not structured.
@(private)
structure_walk :: proc(t: ^$E, a, b: int, emit: bool) -> bool {
	p := t.p
	i := a
	for i < b {
		cf := &p.cf[i]
		#partial switch cf.op {
		case .Cond_Jmp:
			target := int(cf.address)
			if target <= i || target > b {return false}
			if cf.unconditional {
				// [i+1, target) runs only if something jumps into it
				if !structure_no_entry(p, i + 1, target - 1, i + 1, target) {return false}
				i = target
				continue
			}
			if target == i + 1 {
				i += 1 // a jump to the next cf: nothing to skip (the condition has no side effects)
				continue
			}
			// if / else: the taken branch (the else part) starts at target, and the fall-through part ends with an
			// unconditional jump over it
			last := &p.cf[target - 1]
			if target - 1 > i && last.op == .Cond_Jmp && last.unconditional && int(last.address) > target &&
			   int(last.address) <= b {
				join := int(last.address)
				// nothing but this jump enters the else part (its first cf included), nothing enters the then part
				if !structure_no_entry(p, target, join - 1, target, join, except = i) {return false}
				if !structure_no_entry(p, i + 1, target - 2, i + 1, target - 1) {return false}
				f: Sw_Token
				if emit {f = sw_if_not_else_begin(t, cf)}
				if !structure_walk(t, i + 1, target - 1, emit) {return false}
				if emit {sw_if_else_middle(t, f)}
				if !structure_walk(t, target, join, emit) {return false}
				if emit {sw_if_else_end(t, f)}
				i = join
				continue
			}
			if !structure_no_entry(p, i + 1, target - 1, i + 1, target) {return false}
			f: Sw_Token
			if emit {f = sw_if_not_begin(t, cf)}
			if !structure_walk(t, i + 1, target, emit) {return false}
			if emit {sw_if_end(t, f)}
			i = target
			continue
		case .Loop_Start:
			// the matching loop_end (loops nest)
			e := -1
			depth := 0
			for k in i + 1 ..< b {
				if p.cf[k].op == .Loop_Start {depth += 1}
				if p.cf[k].op == .Loop_End {
					if depth == 0 {e = k; break}
					depth -= 1
				}
			}
			if e < 0 {return false}
			end := &p.cf[e]
			if end.loop_id != cf.loop_id || int(end.address) != i + 1 || int(cf.address) != e + 1 {return false}
			if !structure_no_entry(p, i, e, i + 1, e + 1) {return false}
			f: Sw_Token
			if emit {f = sw_loop_begin(t, cf)}
			if !structure_walk(t, i + 1, e, emit) {return false}
			if emit {sw_loop_end(t, end, f)}
			i = e + 1
			continue
		case .Loop_End, .Cond_Call, .Return:
			return false
		}
		if cf_is_exec(cf.op) && cf_ends_shader(cf.op) && !structure_tail_empty(p, i) {return false}
		if emit {sw_emit_cf(t, cf, i)}
		if t.err != "" {return false}
		i += 1
	}
	return true
}

// ---- the SPIR-V hooks ----

@(private)
spv_sw_if_not_begin :: proc(t: ^T, cf: ^Cf) -> Sw_Token {
	bld := &t.b
	f := if_begin(bld, op1(bld, .LogicalNot, t.t_b, condition(t, cf)))
	return {f.merge, f.else_, 0, 0}
}

@(private)
spv_sw_if_end :: proc(t: ^T, f: Sw_Token) {if_end(&t.b, If{merge = f[0], else_ = f[1]})}

@(private)
spv_sw_if_not_else_begin :: proc(t: ^T, cf: ^Cf) -> Sw_Token {
	bld := &t.b
	f := if_else_begin(bld, op1(bld, .LogicalNot, t.t_b, condition(t, cf)))
	return {f.else_, f.merge, 0, 0}
}

@(private)
spv_sw_if_else_middle :: proc(t: ^T, f: Sw_Token) {if_else_middle(&t.b, If_Else{else_ = f[0], merge = f[1]})}

@(private)
spv_sw_if_else_end :: proc(t: ^T, f: Sw_Token) {if_else_end(&t.b, If_Else{else_ = f[0], merge = f[1]})}

@(private)
spv_sw_loop_begin :: proc(t: ^T, cf: ^Cf) -> Sw_Token {
	structure_loop_begin(t, cf)
	header, cont, merge, outer := structure_loop_header(t)
	return {header, cont, merge, outer}
}

@(private)
spv_sw_loop_end :: proc(t: ^T, cf: ^Cf, f: Sw_Token) {structure_loop_end(t, cf, f[0], f[1], f[2], f[3])}

// loop_start: as the dispatcher's (push the loop counter and aL), then `if (count == 0) pop else <loop>`.
@(private)
structure_loop_begin :: proc(t: ^T, cf: ^Cf) {
	b := &t.b
	lc := loop_constant(t, cf.loop_id)
	count := op1(b, .BitwiseAnd, t.t_u, lc, const_u32(b, 0xff))
	old_lc := load(b, b.t_uvec[4], t.v_lc)
	store(b, t.v_lc, op1(b, .CompositeConstruct, b.t_uvec[4], count, extract(b, t.t_u, old_lc, 0), extract(b, t.t_u, old_lc, 1), extract(b, t.t_u, old_lc, 2)))
	old_al := load(b, b.t_ivec[4], t.v_al)
	start := bitcast(b, t.t_i, op1(b, .BitFieldUExtract, t.t_u, lc, const_u32(b, 8), const_u32(b, 8)))
	if cf.is_repeat {start = extract(b, t.t_i, old_al, 0)}
	store(b, t.v_al, op1(b, .CompositeConstruct, b.t_ivec[4], start, extract(b, t.t_i, old_al, 0), extract(b, t.t_i, old_al, 1), extract(b, t.t_i, old_al, 2)))
	skip := op1(b, .IEqual, t.t_b, count, const_u32(b, 0))
	t.loop_skip = skip
	t.loop_old_lc = old_lc
	t.loop_old_al = old_al
}

// The selection around the loop (its skip side pops) and the loop header; the body follows in a block of its own.
@(private)
structure_loop_header :: proc(t: ^T) -> (header, cont, merge, outer: u32) {
	b := &t.b
	f := if_else_begin(b, t.loop_skip)
	store(b, t.v_lc, t.loop_old_lc)
	store(b, t.v_al, t.loop_old_al)
	if_else_middle(b, f)
	outer = f.merge
	header, cont, merge = id(b), id(b), id(b)
	branch(b, header)
	label(b, header)
	op0(b, .LoopMerge, merge, cont, 0)
	body := id(b)
	branch(b, body)
	label(b, body)
	return
}

// loop_end: the dispatcher's decision (count - 1 != 0, and no predicated break) continues the loop with the counter
// and aL stepped, or leaves it popping both.
@(private)
structure_loop_end :: proc(t: ^T, cf: ^Cf, header, cont, merge, outer: u32) {
	b := &t.b
	old_lc := load(b, b.t_uvec[4], t.v_lc)
	count := op1(b, .ISub, t.t_u, extract(b, t.t_u, old_lc, 0), const_u32(b, 1))
	again := op1(b, .INotEqual, t.t_b, count, const_u32(b, 0))
	if cf.pred_break {
		pv := load(b, t.t_b, t.v_pred)
		brk := cf.condition ? pv : op1(b, .LogicalNot, t.t_b, pv)
		again = op1(b, .LogicalAnd, t.t_b, again, op1(b, .LogicalNot, t.t_b, brk))
	}
	old_al := load(b, b.t_ivec[4], t.v_al)
	branch_cond(b, again, cont, merge)
	label(b, cont)
	store(b, t.v_lc, op1(b, .CompositeInsert, b.t_uvec[4], count, old_lc, 0))
	step := op1(b, .BitFieldSExtract, t.t_i, bitcast(b, t.t_i, loop_constant(t, cf.loop_id)), const_u32(b, 16), const_u32(b, 8))
	store(b, t.v_al, op1(b, .CompositeInsert, b.t_ivec[4], op1(b, .IAdd, t.t_i, extract(b, t.t_i, old_al, 0), step), old_al, 0))
	branch(b, header)
	label(b, merge)
	lc2 := load(b, b.t_uvec[4], t.v_lc)
	al2 := load(b, b.t_ivec[4], t.v_al)
	store(b, t.v_lc, op1(b, .CompositeConstruct, b.t_uvec[4], extract(b, t.t_u, lc2, 1), extract(b, t.t_u, lc2, 2), extract(b, t.t_u, lc2, 3), const_u32(b, 0)))
	store(b, t.v_al, op1(b, .CompositeConstruct, b.t_ivec[4], extract(b, t.t_i, al2, 1), extract(b, t.t_i, al2, 2), extract(b, t.t_i, al2, 3), const_i32(b, 0)))
	branch(b, outer)
	label(b, outer)
}
