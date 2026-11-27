package xenos

// Xenos (Xbox 360 GPU) shader microcode decoding. Bit layouts follow Xenia's src/xenia/gpu/ucode.h (3-Clause BSD,
// has207/xenia-edge 15f7f1a): every instruction is 32-bit dwords in host order after a byte swap of the guest
// (big-endian) words, bitfields allocated from the least significant bit.
//
// Control flow: pairs of 48-bit instructions packed in 3 dwords. ALU / fetch: 96-bit (3 dwords), addressed in
// units of 3 dwords from the start of the microcode.

Cf_Op :: enum u8 {
	Nop                   = 0,
	Exec                  = 1,
	Exec_End              = 2,
	Cond_Exec             = 3,
	Cond_Exec_End         = 4,
	Cond_Exec_Pred        = 5,
	Cond_Exec_Pred_End    = 6,
	Loop_Start            = 7,
	Loop_End              = 8,
	Cond_Call             = 9,
	Return                = 10,
	Cond_Jmp              = 11,
	Alloc                 = 12,
	Cond_Exec_Pred_Clean  = 13,
	Cond_Exec_Pred_Clean_End = 14,
	Mark_Vs_Fetch_Done    = 15,
}

cf_op_names := [16]string{"nop", "exec", "exece", "cexec", "cexece", "pexec", "pexece", "loop", "endloop", "ccall", "ret", "cjmp", "alloc", "cexec_pc", "cexece_pc", "vfetch_done"}

Cf :: struct {
	op:            Cf_Op,
	w0, w1:        u32, // w1: 16 bits
	// exec family
	address:       u32,
	count:         u32,
	sequence:      u32,
	is_yield:      bool,
	pred_clean:    bool,
	bool_index:    u32,
	condition:     bool,
	// loop / call / jump
	loop_id:       u32,
	is_repeat:     bool,
	pred_break:    bool,
	unconditional: bool,
	predicated:    bool,
	// alloc
	alloc_size:    u32,
	alloc_type:    u32, // 1 position, 2 interpolators / colors, 3 memory
	addr_mode:     u32,
}

cf_is_exec :: proc(op: Cf_Op) -> bool {
	#partial switch op {
	case .Exec, .Exec_End, .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		return true
	}
	return false
}

cf_ends_shader :: proc(op: Cf_Op) -> bool {
	#partial switch op {
	case .Exec_End, .Cond_Exec_End, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean_End:
		return true
	}
	return false
}

@(private)
bits :: #force_inline proc(v: u32, lo, n: u32) -> u32 {return (v >> lo) & ((1 << n) - 1)}

@(private)
sbits :: #force_inline proc(v: u32, lo, n: u32) -> i32 {return i32(v << (32 - lo - n)) >> (32 - n)}

cf_decode :: proc(w0, w1: u32) -> (cf: Cf) {
	cf.w0, cf.w1 = w0, w1
	cf.op = Cf_Op(bits(w1, 12, 4))
	cf.addr_mode = bits(w1, 11, 1)
	switch cf.op {
	case .Exec, .Exec_End, .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		cf.address = bits(w0, 0, 12)
		cf.count = bits(w0, 12, 3)
		cf.is_yield = bits(w0, 15, 1) != 0
		cf.sequence = bits(w0, 16, 12)
		#partial switch cf.op {
		case .Exec, .Exec_End:
			cf.pred_clean = bits(w1, 9, 1) != 0
		case .Cond_Exec_Pred, .Cond_Exec_Pred_End:
			cf.pred_clean = bits(w1, 9, 1) != 0
			cf.condition = bits(w1, 10, 1) != 0
		case:
			cf.bool_index = bits(w1, 2, 8)
			cf.condition = bits(w1, 10, 1) != 0
		}
	case .Loop_Start:
		cf.address = bits(w0, 0, 13)
		cf.is_repeat = bits(w0, 13, 1) != 0
		cf.loop_id = bits(w0, 16, 5)
	case .Loop_End:
		cf.address = bits(w0, 0, 13)
		cf.loop_id = bits(w0, 16, 5)
		cf.pred_break = bits(w0, 21, 1) != 0
		cf.condition = bits(w1, 10, 1) != 0
	case .Cond_Call, .Cond_Jmp:
		cf.address = bits(w0, 0, 13)
		cf.unconditional = bits(w0, 13, 1) != 0
		cf.predicated = bits(w0, 14, 1) != 0
		cf.bool_index = bits(w1, 2, 8)
		cf.condition = bits(w1, 10, 1) != 0
	case .Alloc:
		cf.alloc_size = bits(w0, 0, 3)
		cf.alloc_type = bits(w1, 9, 2)
	case .Nop, .Return, .Mark_Vs_Fetch_Done:
	}
	return
}

// the control flow program: every pair before the ALU / fetch area, which starts at the lowest exec address (code
// after an exec_end is still reachable by jumps; Xenia Shader::AnalyzeUcode bounds it the same way)
cf_program :: proc(ucode: []u32, allocator := context.allocator) -> [dynamic]Cf {
	out := make([dynamic]Cf, allocator)
	limit := len(ucode) / 3 // in 3-dword units
	for i := 0; i + 3 <= len(ucode) && i / 3 < limit; i += 3 {
		d0, d1, d2 := ucode[i], ucode[i + 1], ucode[i + 2]
		a := cf_decode(d0, d1 & 0xffff)
		b := cf_decode((d1 >> 16) | (d2 << 16), d2 >> 16)
		append(&out, a, b)
		if cf_is_exec(a.op) && a.count > 0 {limit = min(limit, int(a.address))}
		if cf_is_exec(b.op) && b.count > 0 {limit = min(limit, int(b.address))}
	}
	return out
}

// ---- ALU ----

Vec_Op :: enum u8 {
	Add = 0, Mul, Max, Min, Seq, Sgt, Sge, Sne, Frc, Trunc, Floor, Mad, Cnd_Eq, Cnd_Ge, Cnd_Gt, Dp4, Dp3, Dp2_Add, Cube, Max4,
	Setp_Eq_Push, Setp_Ne_Push, Setp_Gt_Push, Setp_Ge_Push, Kill_Eq, Kill_Gt, Kill_Ge, Kill_Ne, Dst, Max_A, Op30, Op31,
}
vec_op_names := [32]string{"add", "mul", "max", "min", "seq", "sgt", "sge", "sne", "frc", "trunc", "floor", "mad", "cndeq", "cndge", "cndgt", "dp4", "dp3", "dp2add", "cube", "max4", "setp_eq_push", "setp_ne_push", "setp_gt_push", "setp_ge_push", "kill_eq", "kill_gt", "kill_ge", "kill_ne", "dst", "maxa", "op30", "op31"}
vec_op_operands := [32]u8{2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 3, 3, 3, 3, 2, 2, 3, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0}

Sca_Op :: enum u8 {
	Adds = 0, Adds_Prev, Muls, Muls_Prev, Muls_Prev2, Maxs, Mins, Seqs, Sgts, Sges, Snes, Frcs, Truncs, Floors, Exp, Logc, Log, Rcpc, Rcpf, Rcp,
	Rsqc, Rsqf, Rsq, Max_As, Max_Asf, Subs, Subs_Prev, Setp_Eq, Setp_Ne, Setp_Gt, Setp_Ge, Setp_Inv, Setp_Pop, Setp_Clr, Setp_Rstr,
	Kills_Eq, Kills_Gt, Kills_Ge, Kills_Ne, Kills_One, Sqrt, Op41, Mulsc0, Mulsc1, Addsc0, Addsc1, Subsc0, Subsc1, Sin, Cos, Retain_Prev,
	Op51, Op52, Op53, Op54, Op55, Op56, Op57, Op58, Op59, Op60, Op61, Op62, Op63,
}
sca_op_names := [64]string{"adds", "adds_prev", "muls", "muls_prev", "muls_prev2", "maxs", "mins", "seqs", "sgts", "sges", "snes", "frcs", "truncs", "floors", "exp", "logc", "log", "rcpc", "rcpf", "rcp", "rsqc", "rsqf", "rsq", "maxas", "maxasf", "subs", "subs_prev", "setp_eq", "setp_ne", "setp_gt", "setp_ge", "setp_inv", "setp_pop", "setp_clr", "setp_rstr", "kills_eq", "kills_gt", "kills_ge", "kills_ne", "kills_one", "sqrt", "op41", "mulsc", "mulsc", "addsc", "addsc", "subsc", "subsc", "sin", "cos", "retain_prev", "op51", "op52", "op53", "op54", "op55", "op56", "op57", "op58", "op59", "op60", "op61", "op62", "op63"}
// 0: none, 1: one operand (src3), 2: two-component single operand (a = W, b = X/Z), 3: c#.w and r#.x
sca_op_operands := [64]u8{2, 1, 2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 3, 3, 3, 3, 3, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

// export register numbers (alu vector_dest when export_data)
EXPORT_VS_POSITION :: 62
EXPORT_VS_POINT_SIZE :: 63
EXPORT_PS_DEPTH :: 61
EXPORT_ADDRESS :: 32
EXPORT_DATA0 :: 33

Operand :: struct {
	is_temp:  bool,
	reg:      u32, // temp 0..63 / constant 0..255
	rel:      u32, // 0 absolute, 1 aL-relative, 2 a0-relative
	abs:      bool,
	negate:   bool,
	swizzle:  [4]u8, // absolute components 0..3
}

Alu :: struct {
	w:                [3]u32,
	vec_op:           Vec_Op,
	sca_op:           Sca_Op,
	vec_dest:         u32,
	vec_dest_rel:     bool,
	sca_dest:         u32,
	sca_dest_rel:     bool,
	is_export:        bool,
	vec_mask:         u32,
	sca_mask:         u32,
	vec_clamp:        bool,
	sca_clamp:        bool,
	abs_constants:    bool,
	predicated:       bool,
	pred_condition:   bool,
	const_a0_rel:     bool,
	vec_src:          [3]Operand,
	sca_src:          [2]Operand,
	sca_src_count:    int,
}

// relative swizzle -> absolute component
alu_swz :: #force_inline proc(swizzle: u32, i: u32) -> u8 {return u8(((swizzle >> (2 * i)) + i) & 3)}

@(private)
alu_operand :: proc(w: [3]u32, i: u32) -> (op: Operand) {
	// i: 1..3
	reg: u32
	switch i {
	case 1: reg = bits(w[2], 16, 8)
	case 2: reg = bits(w[2], 8, 8)
	case 3: reg = bits(w[2], 0, 8)
	}
	sel: u32
	switch i {
	case 1: sel = bits(w[2], 31, 1)
	case 2: sel = bits(w[2], 30, 1)
	case 3: sel = bits(w[2], 29, 1)
	}
	swz: u32
	neg: u32
	switch i {
	case 1: swz = bits(w[1], 16, 8); neg = bits(w[1], 26, 1)
	case 2: swz = bits(w[1], 8, 8); neg = bits(w[1], 25, 1)
	case 3: swz = bits(w[1], 0, 8); neg = bits(w[1], 24, 1)
	}
	op.negate = neg != 0
	op.is_temp = sel != 0
	const_0_rel := bits(w[1], 31, 1) != 0
	const_1_rel := bits(w[1], 30, 1) != 0
	a0_rel := bits(w[1], 29, 1) != 0
	if op.is_temp {
		op.reg = reg & 0x3f
		op.rel = reg & 0x40 != 0 ? 1 : 0
		op.abs = reg & 0x80 != 0
	} else {
		op.reg = reg
		src1_temp := bits(w[2], 31, 1) != 0
		src2_temp := bits(w[2], 30, 1) != 0
		addressed: bool
		switch i {
		case 1: addressed = const_0_rel
		case 2: addressed = src1_temp ? const_0_rel : const_1_rel
		case 3: addressed = (src1_temp && src2_temp) ? const_0_rel : const_1_rel
		}
		if addressed {op.rel = a0_rel ? 2 : 1}
		op.abs = bits(w[0], 7, 1) != 0
	}
	for k in 0 ..< u32(4) {op.swizzle[k] = alu_swz(swz, k)}
	return
}

alu_decode :: proc(w: [3]u32, vector_uses_src3: bool = false) -> (a: Alu) {
	a.w = w
	a.vec_dest = bits(w[0], 0, 6)
	a.vec_dest_rel = bits(w[0], 6, 1) != 0
	a.abs_constants = bits(w[0], 7, 1) != 0
	a.sca_dest = bits(w[0], 8, 6)
	a.sca_dest_rel = bits(w[0], 14, 1) != 0
	a.is_export = bits(w[0], 15, 1) != 0
	a.vec_mask = bits(w[0], 16, 4)
	a.sca_mask = bits(w[0], 20, 4)
	a.vec_clamp = bits(w[0], 24, 1) != 0
	a.sca_clamp = bits(w[0], 25, 1) != 0
	a.sca_op = Sca_Op(bits(w[0], 26, 6))
	a.pred_condition = bits(w[1], 27, 1) != 0
	a.predicated = bits(w[1], 28, 1) != 0
	a.const_a0_rel = bits(w[1], 29, 1) != 0
	a.vec_op = Vec_Op(bits(w[2], 24, 5))
	nv := int(vec_op_operands[a.vec_op])
	for i in 0 ..< nv {a.vec_src[i] = alu_operand(w, u32(i + 1))}
	switch sca_op_operands[a.sca_op] {
	case 1, 2:
		a.sca_src_count = 1
		a.sca_src[0] = alu_operand(w, 3)
		// the scalar `b` component: Z when the vector operation reads source 3, else X (Xenia ParseAluInstruction)
		src3_swz := bits(w[1], 0, 8)
		a.sca_src[0].swizzle[0] = alu_swz(src3_swz, 3)
		a.sca_src[0].swizzle[1] = alu_swz(src3_swz, nv == 3 ? 2 : 0)
	case 3:
		// mulsc / addsc / subsc: constant c#.w (a) and temporary r#.x (b)
		a.sca_src_count = 2
		src3_swz := bits(w[1], 0, 8)
		neg := bits(w[1], 24, 1) != 0
		c := Operand{is_temp = false, reg = bits(w[2], 0, 8), rel = 0, abs = a.abs_constants, negate = neg}
		src1_temp := bits(w[2], 31, 1) != 0
		src2_temp := bits(w[2], 30, 1) != 0
		const_0_rel := bits(w[1], 31, 1) != 0
		const_1_rel := bits(w[1], 30, 1) != 0
		addressed := (src1_temp && src2_temp) ? const_0_rel : const_1_rel
		if addressed {c.rel = a.const_a0_rel ? 2 : 1}
		c.swizzle = {alu_swz(src3_swz, 3), 0, 0, 0}
		t := Operand{is_temp = true, abs = a.abs_constants, negate = neg}
		t.reg = (u32(a.sca_op) & 1) | (bits(w[2], 29, 1) << 1) | (src3_swz & 0x3c)
		t.swizzle = {alu_swz(src3_swz, 0), 0, 0, 0}
		a.sca_src[0] = c
		a.sca_src[1] = t
	}
	return
}

// write masks of the vector and scalar results, and of the constant 0 / 1 export components (ucode.h)
alu_vec_result_mask :: proc(a: ^Alu) -> u32 {
	m := a.vec_mask
	if a.is_export {m &= ~a.sca_mask}
	return m
}
alu_sca_result_mask :: proc(a: ^Alu) -> u32 {
	m := a.sca_mask
	if a.is_export {m &= ~a.vec_mask}
	return m
}
alu_const0_mask :: proc(a: ^Alu) -> u32 {
	if !a.is_export || !a.sca_dest_rel {return 0}
	return 0xf & ~(a.vec_mask | a.sca_mask)
}
alu_const1_mask :: proc(a: ^Alu) -> u32 {
	if !a.is_export {return 0}
	return a.vec_mask & a.sca_mask
}

// ---- fetch ----

Fetch_Op :: enum u8 {
	Vertex_Fetch      = 0,
	Texture_Fetch     = 1,
	Get_Border_Frac   = 16,
	Get_Comp_Tex_Lod  = 17,
	Get_Gradients     = 18,
	Get_Weights       = 19,
	Set_Tex_Lod       = 24,
	Set_Grad_H        = 25,
	Set_Grad_V        = 26,
}

Vertex_Fetch :: struct {
	src_reg:        u32,
	src_rel:        bool,
	src_swz:        u32, // 2 bits: component of the index
	dst_reg:        u32,
	dst_rel:        bool,
	dst_swz:        u32, // 12 bits, 3 per component: 0-3 xyzw, 4 zero, 5 one, 7 keep
	const_index:    u32, // fetch constant 0..95
	prefetch_count: u32,
	is_signed:      bool,
	is_normalized:  bool, // num_format_all == 0
	signed_rf_mode: u32,
	index_rounded:  bool,
	format:         u32,
	exp_adjust:     i32,
	is_mini:        bool,
	predicated:     bool,
	pred_condition: bool,
	stride:         u32, // dwords
	offset:         i32, // dwords
}

Texture_Fetch :: struct {
	op:             Fetch_Op,
	src_reg:        u32,
	src_rel:        bool,
	src_swz:        u32, // 6 bits, 2 per component (x, y, z)
	dst_reg:        u32,
	dst_rel:        bool,
	dst_swz:        u32,
	fetch_valid_only: bool,
	const_index:    u32, // 0..31
	unnormalized:   bool,
	mag_filter, min_filter, mip_filter, aniso_filter: u32,
	vol_mag_filter, vol_min_filter: u32,
	use_comp_lod:   bool,
	use_reg_lod:    bool,
	use_reg_gradients: bool,
	sample_location: u32, // 0 centroid, 1 center
	lod_bias:       i32, // 1/16
	dimension:      u32, // 0 1D, 1 2D, 2 3D / stacked, 3 cube
	offset_x, offset_y, offset_z: i32, // 1/2
	predicated:     bool,
	pred_condition: bool,
}

fetch_op :: #force_inline proc(w: [3]u32) -> Fetch_Op {return Fetch_Op(bits(w[0], 0, 5))}

vfetch_decode :: proc(w: [3]u32) -> (f: Vertex_Fetch) {
	f.src_reg = bits(w[0], 5, 6)
	f.src_rel = bits(w[0], 11, 1) != 0
	f.dst_reg = bits(w[0], 12, 6)
	f.dst_rel = bits(w[0], 18, 1) != 0
	f.const_index = bits(w[0], 20, 5) * 3 + bits(w[0], 25, 2)
	f.prefetch_count = bits(w[0], 27, 3)
	f.src_swz = bits(w[0], 30, 2)
	f.dst_swz = bits(w[1], 0, 12)
	f.is_signed = bits(w[1], 12, 1) != 0
	f.is_normalized = bits(w[1], 13, 1) == 0
	f.signed_rf_mode = bits(w[1], 14, 1)
	f.index_rounded = bits(w[1], 15, 1) != 0
	f.format = bits(w[1], 16, 6)
	f.exp_adjust = sbits(w[1], 24, 6)
	f.is_mini = bits(w[1], 30, 1) != 0
	f.predicated = bits(w[1], 31, 1) != 0
	f.stride = bits(w[2], 0, 8)
	f.offset = sbits(w[2], 8, 23)
	f.pred_condition = bits(w[2], 31, 1) != 0
	return
}

tfetch_decode :: proc(w: [3]u32) -> (f: Texture_Fetch) {
	f.op = Fetch_Op(bits(w[0], 0, 5))
	f.src_reg = bits(w[0], 5, 6)
	f.src_rel = bits(w[0], 11, 1) != 0
	f.dst_reg = bits(w[0], 12, 6)
	f.dst_rel = bits(w[0], 18, 1) != 0
	f.fetch_valid_only = bits(w[0], 19, 1) != 0
	f.const_index = bits(w[0], 20, 5)
	f.unnormalized = bits(w[0], 25, 1) != 0
	f.src_swz = bits(w[0], 26, 6)
	f.dst_swz = bits(w[1], 0, 12)
	f.mag_filter = bits(w[1], 12, 2)
	f.min_filter = bits(w[1], 14, 2)
	f.mip_filter = bits(w[1], 16, 2)
	f.aniso_filter = bits(w[1], 18, 3)
	// arbitrary filter 21..23
	f.vol_mag_filter = bits(w[1], 24, 2)
	f.vol_min_filter = bits(w[1], 26, 2)
	f.use_comp_lod = bits(w[1], 28, 1) != 0
	f.use_reg_lod = bits(w[1], 29, 1) != 0
	f.predicated = bits(w[1], 31, 1) != 0
	f.use_reg_gradients = bits(w[2], 0, 1) != 0
	f.sample_location = bits(w[2], 1, 1)
	f.lod_bias = sbits(w[2], 2, 7)
	f.dimension = bits(w[2], 14, 2)
	f.offset_x = sbits(w[2], 16, 5)
	f.offset_y = sbits(w[2], 21, 5)
	f.offset_z = sbits(w[2], 26, 5)
	f.pred_condition = bits(w[2], 31, 1) != 0
	return
}

fetch_dst_component :: #force_inline proc(dst_swz: u32, i: u32) -> u32 {return (dst_swz >> (3 * i)) & 7}

// Xenos vertex formats (xenos.h VertexFormat)
vertex_format_name :: proc(f: u32) -> string {
	switch f {
	case 6: return "8_8_8_8"
	case 7: return "2_10_10_10"
	case 16: return "10_11_11"
	case 17: return "11_11_10"
	case 25: return "16_16"
	case 26: return "16_16_16_16"
	case 31: return "16_16_FLOAT"
	case 32: return "16_16_16_16_FLOAT"
	case 33: return "32"
	case 34: return "32_32"
	case 35: return "32_32_32_32"
	case 36: return "32_FLOAT"
	case 37: return "32_32_FLOAT"
	case 38: return "32_32_32_32_FLOAT"
	case 57: return "32_32_32_FLOAT"
	}
	return "unknown"
}

// components a vertex format provides
vertex_format_components :: proc(f: u32) -> int {
	switch f {
	case 33, 36: return 1
	case 25, 31, 34, 37: return 2
	case 16, 17, 57: return 3
	case 6, 7, 26, 32, 35, 38: return 4
	}
	return 0
}

// dwords per element
vertex_format_dwords :: proc(f: u32) -> int {
	switch f {
	case 6, 7, 16, 17, 25, 31, 33, 36: return 1
	case 26, 32, 34, 37: return 2
	case 57: return 3
	case 35, 38: return 4
	}
	return 0
}
