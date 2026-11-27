package xenos

import "core:fmt"
import "core:strings"

// A decoded shader program: the control flow, every ALU / fetch instruction reached from it, and the facts the
// translator and the census need.

Instr_Kind :: enum u8 {
	Alu,
	Vfetch,
	Tfetch,
}

Instr :: struct {
	kind:      Instr_Kind,
	address:   u32, // instruction slot (3 dwords)
	serialize: bool,
	w:         [3]u32,
	alu:       Alu,
	vf:        Vertex_Fetch,
	tf:        Texture_Fetch,
	vf_full:   Vertex_Fetch, // the vfetch_full a vfetch_mini reuses (src, constant, stride)
}

Program :: struct {
	stage:        Stage,
	cf:           [dynamic]Cf,
	instrs:       [dynamic]Instr, // per exec in cf order
	exec_first:   [dynamic]int, // cf index -> first instrs index (-1 for non-exec)
	labels:       [dynamic]bool, // cf index is a jump / loop / call target
	has_labels:   bool,
	// facts
	max_temp:     int, // highest temp register referenced (-1 none)
	temp_rel:     bool, // aL-relative temp addressing
	const_rel:    bool, // relative constant addressing
	uses_a0:      bool,
	uses_aL:      bool,
	uses_pred:    bool,
	kills:        bool,
	memexport:    bool,
	exports:      u64, // bit per export register written (0..63)
	vfetch_count: int,
	tfetch_count: int,
	max_float_const: int,
	unfed_reads:  u64, // registers read before any write (straight-line approximation): inputs the hardware provides
	error:        string,
}

program_build :: proc(c: ^Container, allocator := context.allocator) -> (p: Program, ok: bool) {
	context.allocator = allocator
	p.stage = c.stage
	p.max_temp = -1
	p.max_float_const = -1
	p.cf = cf_program(c.ucode)
	resize(&p.labels, len(p.cf))
	resize(&p.exec_first, len(p.cf))
	last_full: Vertex_Fetch
	written: u64 = c.stage == .Vertex ? 1 : 0
	if c.stage == .Pixel {for v in c.interpolators {written |= 1 << v.reg}}
	have_full := false
	n_slots := u32(len(c.ucode) / 3)
	for &cf, ci in p.cf {
		p.exec_first[ci] = -1
		#partial switch cf.op {
		case .Loop_Start:
			if int(cf.address) < len(p.labels) {p.labels[cf.address] = true}
			if ci + 1 < len(p.labels) {p.labels[ci + 1] = true}
			p.has_labels = true
			p.uses_aL = true
		case .Loop_End:
			if int(cf.address) < len(p.labels) {p.labels[cf.address] = true}
			if ci + 1 < len(p.labels) {p.labels[ci + 1] = true}
			p.has_labels = true
		case .Cond_Jmp, .Cond_Call:
			if int(cf.address) >= len(p.labels) {
				p.error = "jump target outside the control flow program"
				return p, false
			}
			p.labels[cf.address] = true
			if ci + 1 < len(p.labels) {p.labels[ci + 1] = true}
			p.has_labels = true
			if cf.predicated && !cf.unconditional {p.uses_pred = true}
		case .Cond_Exec_Pred, .Cond_Exec_Pred_End:
			p.uses_pred = true
		case .Alloc:
			if cf.alloc_type == 3 {p.memexport = true}
		}
		if !cf_is_exec(cf.op) {continue}
		p.exec_first[ci] = len(p.instrs)
		for k in 0 ..< cf.count {
			addr := cf.address + k
			if addr >= n_slots {
				p.error = "exec reads past the microcode"
				return p, false
			}
			seq := (cf.sequence >> (2 * k)) & 3
			w := [3]u32{c.ucode[3 * addr], c.ucode[3 * addr + 1], c.ucode[3 * addr + 2]}
			in_ := Instr{address = addr, serialize = seq & 2 != 0, w = w}
			if seq & 1 != 0 {
				if fetch_op(w) == .Vertex_Fetch {
					in_.kind = .Vfetch
					in_.vf = vfetch_decode(w)
					if !in_.vf.is_mini {last_full = in_.vf; have_full = true}
					in_.vf_full = last_full
					if in_.vf.is_mini && !have_full {
						p.error = "vfetch_mini without a preceding vfetch_full"
						return p, false
					}
					p.vfetch_count += 1
					use_temp(&p, int(in_.vf_full.src_reg), in_.vf_full.src_rel)
					use_temp(&p, int(in_.vf.dst_reg), in_.vf.dst_rel)
					if in_.vf.predicated {p.uses_pred = true}
				} else {
					in_.kind = .Tfetch
					in_.tf = tfetch_decode(w)
					p.tfetch_count += 1
					use_temp(&p, int(in_.tf.src_reg), in_.tf.src_rel)
					use_temp(&p, int(in_.tf.dst_reg), in_.tf.dst_rel)
					if in_.tf.predicated {p.uses_pred = true}
				}
			} else {
				in_.kind = .Alu
				in_.alu = alu_decode(w)
				a := &in_.alu
				if a.predicated {p.uses_pred = true}
				nv := int(vec_op_operands[a.vec_op])
				for i in 0 ..< nv {use_operand(&p, &a.vec_src[i])}
				for i in 0 ..< a.sca_src_count {use_operand(&p, &a.sca_src[i])}
				if a.is_export {
					p.exports |= 1 << u64(a.vec_dest)
					if a.vec_dest >= EXPORT_ADDRESS && a.vec_dest <= EXPORT_DATA0 + 4 {p.memexport = true}
				} else {
					if alu_vec_result_mask(a) != 0 {use_temp(&p, int(a.vec_dest), a.vec_dest_rel)}
					if alu_sca_result_mask(a) != 0 {use_temp(&p, int(a.sca_dest), a.sca_dest_rel)}
				}
				#partial switch a.vec_op {
				case .Kill_Eq, .Kill_Gt, .Kill_Ge, .Kill_Ne: p.kills = true
				case .Max_A: p.uses_a0 = true
				case .Setp_Eq_Push, .Setp_Ne_Push, .Setp_Gt_Push, .Setp_Ge_Push: p.uses_pred = true
				}
				#partial switch a.sca_op {
				case .Kills_Eq, .Kills_Gt, .Kills_Ge, .Kills_Ne, .Kills_One: p.kills = true
				case .Max_As, .Max_Asf: p.uses_a0 = true
				case .Setp_Eq, .Setp_Ne, .Setp_Gt, .Setp_Ge, .Setp_Inv, .Setp_Pop, .Setp_Clr, .Setp_Rstr: p.uses_pred = true
				}
			}
			note_unfed(&p, &in_, &written)
			append(&p.instrs, in_)
		}
	}
	return p, true
}

@(private)
use_temp :: proc(p: ^Program, r: int, rel: bool) {
	if r > p.max_temp {p.max_temp = r}
	if rel {p.temp_rel = true; p.uses_aL = true}
}

@(private)
use_operand :: proc(p: ^Program, o: ^Operand) {
	if o.is_temp {
		use_temp(p, int(o.reg), o.rel != 0)
	} else {
		if int(o.reg) > p.max_float_const {p.max_float_const = int(o.reg)}
		if o.rel != 0 {
			p.const_rel = true
			if o.rel == 2 {p.uses_a0 = true} else {p.uses_aL = true}
		}
	}
}

program_destroy :: proc(p: ^Program) {
	delete(p.cf)
	delete(p.instrs)
	delete(p.exec_first)
	delete(p.labels)
}

// ---- disassembly (debugging / census examples) ----

@(private)
swz_str :: proc(sb: ^strings.Builder, o: ^Operand) {
	xyzw := "xyzw"
	if o.swizzle == {0, 1, 2, 3} {return}
	strings.write_byte(sb, '.')
	for k in 0 ..< 4 {strings.write_byte(sb, xyzw[o.swizzle[k]])}
}

@(private)
operand_str :: proc(sb: ^strings.Builder, o: ^Operand) {
	if o.negate {strings.write_byte(sb, '-')}
	if o.abs {strings.write_string(sb, "|")}
	if o.is_temp {
		fmt.sbprintf(sb, "r%d", o.reg)
		if o.rel != 0 {strings.write_string(sb, "[aL]")}
	} else {
		fmt.sbprintf(sb, "c%d", o.reg)
		if o.rel == 1 {strings.write_string(sb, "[aL]")} else if o.rel == 2 {strings.write_string(sb, "[a0]")}
	}
	if o.abs {strings.write_string(sb, "|")}
	swz_str(sb, o)
}

@(private)
dest_str :: proc(sb: ^strings.Builder, stage: Stage, export: bool, reg: u32, rel: bool, mask: u32) {
	if export {
		switch {
		case reg == EXPORT_ADDRESS: strings.write_string(sb, "eA")
		case reg >= EXPORT_DATA0 && reg <= EXPORT_DATA0 + 4: fmt.sbprintf(sb, "eM%d", reg - EXPORT_DATA0)
		case stage == .Vertex && reg == EXPORT_VS_POSITION: strings.write_string(sb, "oPos")
		case stage == .Vertex && reg == EXPORT_VS_POINT_SIZE: strings.write_string(sb, "oPts")
		case stage == .Pixel && reg == EXPORT_PS_DEPTH: strings.write_string(sb, "oDepth")
		case stage == .Pixel: fmt.sbprintf(sb, "oC%d", reg)
		case: fmt.sbprintf(sb, "o%d", reg)
		}
	} else {
		fmt.sbprintf(sb, "r%d", reg)
		if rel {strings.write_string(sb, "[aL]")}
	}
	strings.write_byte(sb, '.')
	xyzw := "xyzw"
	for k in 0 ..< u32(4) {strings.write_byte(sb, mask & (1 << k) != 0 ? xyzw[k] : '_')}
}

instr_disasm :: proc(sb: ^strings.Builder, p: ^Program, in_: ^Instr) {
	fmt.sbprintf(sb, "      %3d ", in_.address)
	switch in_.kind {
	case .Vfetch:
		f := &in_.vf
		if f.predicated {fmt.sbprintf(sb, "(%sp) ", f.pred_condition ? "" : "!")}
		fmt.sbprintf(sb, "%s r%d.", f.is_mini ? "vfetch_mini" : "vfetch_full", f.dst_reg)
		comp := "xyzw01?_"
		for k in 0 ..< u32(4) {strings.write_byte(sb, comp[fetch_dst_component(f.dst_swz, k)])}
		fmt.sbprintf(sb, ", r%d.%c, vf%d  format=%s(%d) stride=%d offset=%d %s%s rf=%d exp=%d prefetch=%d%s",
			in_.vf_full.src_reg, XYZW[in_.vf_full.src_swz], in_.vf_full.const_index, vertex_format_name(f.format), f.format, in_.vf_full.stride, f.offset,
			f.is_signed ? "signed" : "unsigned", f.is_normalized ? " norm" : " int", f.signed_rf_mode, f.exp_adjust, f.prefetch_count, in_.vf_full.index_rounded ? " rounded" : "")
	case .Tfetch:
		f := &in_.tf
		if f.predicated {fmt.sbprintf(sb, "(%sp) ", f.pred_condition ? "" : "!")}
		dims := [4]string{"1D", "2D", "3D", "Cube"}
		#partial switch f.op {
		case .Texture_Fetch: fmt.sbprintf(sb, "tfetch%s", dims[f.dimension])
		case .Get_Border_Frac: fmt.sbprintf(sb, "getBCF%s", dims[f.dimension])
		case .Get_Comp_Tex_Lod: fmt.sbprintf(sb, "getCompTexLOD%s", dims[f.dimension])
		case .Get_Gradients: strings.write_string(sb, "getGradients")
		case .Get_Weights: fmt.sbprintf(sb, "getWeights%s", dims[f.dimension])
		case .Set_Tex_Lod: strings.write_string(sb, "setTexLOD")
		case .Set_Grad_H: strings.write_string(sb, "setGradientH")
		case .Set_Grad_V: strings.write_string(sb, "setGradientV")
		case: fmt.sbprintf(sb, "fetch_op%d", u32(f.op))
		}
		comp := "xyzw01?_"
		fmt.sbprintf(sb, " r%d.", f.dst_reg)
		for k in 0 ..< u32(4) {strings.write_byte(sb, comp[fetch_dst_component(f.dst_swz, k)])}
		fmt.sbprintf(sb, ", r%d.%c%c%c, tf%d", f.src_reg, XYZW[f.src_swz & 3], XYZW[(f.src_swz >> 2) & 3], XYZW[(f.src_swz >> 4) & 3], f.const_index)
		if f.use_reg_lod {strings.write_string(sb, " UseRegisterLOD")}
		if f.use_reg_gradients {strings.write_string(sb, " UseRegisterGradients")}
		if !f.use_comp_lod {strings.write_string(sb, " UseComputedLOD=false")}
		if f.lod_bias != 0 {fmt.sbprintf(sb, " LODBias=%v", f32(f.lod_bias) / 16)}
		if f.offset_x != 0 || f.offset_y != 0 || f.offset_z != 0 {fmt.sbprintf(sb, " Offset=(%v,%v,%v)", f32(f.offset_x) / 2, f32(f.offset_y) / 2, f32(f.offset_z) / 2)}
		if f.unnormalized {strings.write_string(sb, " Unnormalized")}
		if f.mag_filter != 3 || f.min_filter != 3 || f.mip_filter != 3 {fmt.sbprintf(sb, " filt=%d/%d/%d", f.mag_filter, f.min_filter, f.mip_filter)}
		if f.aniso_filter != 7 {fmt.sbprintf(sb, " aniso=%d", f.aniso_filter)}
	case .Alu:
		a := &in_.alu
		if a.predicated {fmt.sbprintf(sb, "(%sp) ", a.pred_condition ? "" : "!")}
		vm := alu_vec_result_mask(a) | alu_const0_mask(a) | alu_const1_mask(a)
		fmt.sbprintf(sb, "%s%s ", vec_op_names[a.vec_op], a.vec_clamp ? "_sat" : "")
		dest_str(sb, p.stage, a.is_export, a.vec_dest, a.vec_dest_rel, vm)
		for i in 0 ..< int(vec_op_operands[a.vec_op]) {
			strings.write_string(sb, ", ")
			operand_str(sb, &a.vec_src[i])
		}
		if a.sca_op != .Retain_Prev || alu_sca_result_mask(a) != 0 {
			fmt.sbprintf(sb, "   + %s%s ", sca_op_names[a.sca_op], a.sca_clamp ? "_sat" : "")
			dest_str(sb, p.stage, a.is_export, a.is_export ? a.vec_dest : a.sca_dest, a.sca_dest_rel && !a.is_export, alu_sca_result_mask(a))
			for i in 0 ..< a.sca_src_count {
				strings.write_string(sb, ", ")
				operand_str(sb, &a.sca_src[i])
			}
		}
	}
	strings.write_byte(sb, '\n')
}

program_disasm :: proc(sb: ^strings.Builder, p: ^Program) {
	for &cf, ci in p.cf {
		if p.labels[ci] {fmt.sbprintf(sb, "label_%d:\n", ci)}
		fmt.sbprintf(sb, "  %3d %s", ci, cf_op_names[cf.op])
		#partial switch cf.op {
		case .Exec, .Exec_End, .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
			fmt.sbprintf(sb, " addr=%d count=%d seq=%03x", cf.address, cf.count, cf.sequence)
			#partial switch cf.op {
			case .Cond_Exec_Pred, .Cond_Exec_Pred_End: fmt.sbprintf(sb, " %sp", cf.condition ? "" : "!")
			case .Exec, .Exec_End:
			case: fmt.sbprintf(sb, " %sb%d", cf.condition ? "" : "!", cf.bool_index)
			}
		case .Loop_Start: fmt.sbprintf(sb, " i%d skip=%d%s", cf.loop_id, cf.address, cf.is_repeat ? " repeat" : "")
		case .Loop_End: fmt.sbprintf(sb, " i%d body=%d%s", cf.loop_id, cf.address, cf.pred_break ? (cf.condition ? " break_p" : " break_!p") : "")
		case .Cond_Jmp, .Cond_Call:
			fmt.sbprintf(sb, " target=%d", cf.address)
			if cf.unconditional {strings.write_string(sb, " always")} else if cf.predicated {fmt.sbprintf(sb, " %sp", cf.condition ? "" : "!")} else {fmt.sbprintf(sb, " %sb%d", cf.condition ? "" : "!", cf.bool_index)}
		case .Alloc:
			types := [4]string{"none", "position", p.stage == .Vertex ? "interpolators" : "colors", "memory"}
			fmt.sbprintf(sb, " %s size=%d", types[cf.alloc_type], cf.alloc_size)
		}
		strings.write_byte(sb, '\n')
		if p.exec_first[ci] >= 0 {
			for k in 0 ..< int(cf.count) {instr_disasm(sb, p, &p.instrs[p.exec_first[ci] + k])}
		}
	}
}
XYZW := [4]u8{'x', 'y', 'z', 'w'}

@(private)
note_unfed :: proc(p: ^Program, in_: ^Instr, written: ^u64) {
	reads: u64
	switch in_.kind {
	case .Alu:
		a := &in_.alu
		for i in 0 ..< int(vec_op_operands[a.vec_op]) {if a.vec_src[i].is_temp {reads |= 1 << a.vec_src[i].reg}}
		for i in 0 ..< a.sca_src_count {if a.sca_src[i].is_temp {reads |= 1 << a.sca_src[i].reg}}
		p.unfed_reads |= reads & ~written^
		if !a.is_export {
			if alu_vec_result_mask(a) != 0 {written^ |= 1 << a.vec_dest}
			if alu_sca_result_mask(a) != 0 {written^ |= 1 << a.sca_dest}
		}
	case .Vfetch:
		p.unfed_reads |= (1 << in_.vf_full.src_reg) & ~written^
		written^ |= 1 << in_.vf.dst_reg
	case .Tfetch:
		p.unfed_reads |= (1 << in_.tf.src_reg) & ~written^
		if in_.tf.op != .Set_Tex_Lod && in_.tf.op != .Set_Grad_H && in_.tf.op != .Set_Grad_V {written^ |= 1 << in_.tf.dst_reg}
	}
}
