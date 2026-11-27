package xenos

import "core:fmt"

// Stage interface, vertex fetch, texture fetch and memory export.

@(private)
input_var :: proc(t: ^T, ty: u32, builtin: i32 = -1, location: i32 = -1, flat := false) -> u32 {
	b := &t.b
	v := global_var(b, .Input, ty)
	if builtin >= 0 {decorate(b, v, .BuiltIn, u32(builtin))}
	if location >= 0 {decorate(b, v, .Location, u32(location))}
	if flat {decorate(b, v, .Flat)}
	append(&t.interface, v)
	return v
}

@(private)
output_var :: proc(t: ^T, ty: u32, builtin: i32 = -1, location: i32 = -1) -> u32 {
	b := &t.b
	v := global_var(b, .Output, ty)
	if builtin >= 0 {decorate(b, v, .BuiltIn, u32(builtin))}
	if location >= 0 {decorate(b, v, .Location, u32(location))}
	append(&t.interface, v)
	return v
}

@(private)
stage_inputs :: proc(t: ^T) {
	b := &t.b
	switch t.model {
	case .Vertex:
		vi := input_var(t, t.t_i, i32(BuiltIn.VertexIndex))
		name(b, vi, "gl_VertexIndex")
		off := bitcast(b, t.t_i, extract(b, t.t_u, sys_uvec4(t, .Vertex), 0))
		idx := op1(b, .IAdd, t.t_i, load(b, t.t_i, vi), off)
		r0 := reg_ptr(t, 0, false)
		store(b, r0, op1(b, .CompositeConstruct, t.t_v4, op1(b, .ConvertSToF, t.t_f, idx), const_f32(b, 0), const_f32(b, 0), const_f32(b, 0)))
	case .Tess_Eval:
		tc := input_var(t, b.t_vec[3], 13) // TessCoord
		pid := input_var(t, t.t_i, 7) // PrimitiveId
		name(b, tc, "gl_TessCoord"); name(b, pid, "gl_PrimitiveID")
		c := load(b, b.t_vec[3], tc)
		store(b, reg_ptr(t, 0, false), op1(b, .CompositeConstruct, t.t_v4, extract(b, t.t_f, c, 2), extract(b, t.t_f, c, 1), extract(b, t.t_f, c, 0), const_f32(b, 1)))
		off := bitcast(b, t.t_i, extract(b, t.t_u, sys_uvec4(t, .Vertex), 0))
		patch := op1(b, .ConvertSToF, t.t_f, op1(b, .IAdd, t.t_i, load(b, t.t_i, pid), off))
		if t.n_regs > 1 {store(b, reg_ptr(t, 1, false), op1(b, .CompositeConstruct, t.t_v4, patch, const_f32(b, 0), const_f32(b, 0), const_f32(b, 0)))}
	case .Compute:
		gid := input_var(t, b.t_uvec[3], i32(BuiltIn.GlobalInvocationId))
		name(b, gid, "gl_GlobalInvocationID")
		vx := sys_uvec4(t, .Vertex)
		x := extract(b, t.t_u, load(b, b.t_uvec[3], gid), 0)
		f := if_begin(b, op1(b, .UGreaterThanEqual, t.t_b, x, extract(b, t.t_u, vx, 2)))
		op0(b, .Return)
		b.block_open = false
		if_end(b, f)
		idx := op1(b, .IAdd, t.t_u, x, extract(b, t.t_u, vx, 1))
		store(b, reg_ptr(t, 0, false), op1(b, .CompositeConstruct, t.t_v4, op1(b, .ConvertUToF, t.t_f, idx), const_f32(b, 0), const_f32(b, 0), const_f32(b, 0)))
	case .Fragment:
		used: u32
		for v in t.c.interpolators {
			loc := interface_location(v.usage, v.index)
			if used & (1 << loc) != 0 || v.reg >= t.n_regs {continue}
			used |= 1 << loc
			iv := input_var(t, t.t_v4, -1, i32(loc))
			name(b, iv, fmt_name("in_%v%d", v.usage, v.index))
			store(b, reg_ptr(t, v.reg, false), load(b, t.t_v4, iv))
		}
		t.meta.ps_inputs = used
		// param gen (the Xenos PsParamGen: VPOS / VFACE / SPRITETEXCOORD) into the register after the interpolators:
		// |x| = guest pixel x, sign: back face; |y| = guest pixel y, sign: point primitive; |zw| = the point sprite
		// coordinates in [0, 1] (s left to right, t top to bottom) for a point list (the device sets
		// SYS_FLAG_POINT_PRIMITIVE), else 0 (a line would only set the sign of z: not modelled). Xbox shaders read
		// abs(SPRITETEXCOORD.zw), as Halo 3's loading screen sparkles do.
		pg := u32(len(t.c.interpolators))
		if pg < t.n_regs {
			t.meta.param_gen = int(pg)
			fc := input_var(t, t.t_v4, i32(BuiltIn.FragCoord))
			ff := input_var(t, t.t_b, i32(BuiltIn.FrontFacing))
			pcv := input_var(t, b.t_vec[2], i32(BuiltIn.PointCoord))
			name(b, fc, "gl_FragCoord"); name(b, ff, "gl_FrontFacing"); name(b, pcv, "gl_PointCoord")
			pos := load(b, t.t_v4, fc)
			sc := sys_vec4(t, .Pixel_Scale)
			fl := ext(b, b.t_vec[2], .Floor, op1(b, .VectorShuffle, b.t_vec[2], pos, pos, 0, 1))
			g := arith(b, .FAdd, b.t_vec[2], arith(b, .FMul, b.t_vec[2], fl, op1(b, .VectorShuffle, b.t_vec[2], sc, sc, 0, 1)), op1(b, .VectorShuffle, b.t_vec[2], sc, sc, 2, 3))
			if param_gen_centred(t.p, pg) {
				// HALO3_RES_SCALE: the host pixel's centre less half a guest pixel (param_gen_centred); VPOS is never
				// negative (its signs carry the face and point flags), so the first host column / row at N > 1 keeps 0
				h := op1(b, .VectorShuffle, b.t_vec[2], sc, sc, 0, 1)
				h = arith(b, .FSub, b.t_vec[2], arith(b, .VectorTimesScalar, b.t_vec[2], h, const_f32(b, 0.5)), const_splat(b, 2, 0.5))
				g = ext(b, b.t_vec[2], .NMax, arith(b, .FAdd, b.t_vec[2], g, h), const_splat(b, 2, 0))
			}
			g = ext(b, b.t_vec[2], .FAbs, g)
			gx := bitcast(b, t.t_u, extract(b, t.t_f, g, 0))
			back := op1(b, .LogicalNot, t.t_b, load(b, t.t_b, ff))
			gx = select(b, t.t_u, back, op1(b, .BitwiseOr, t.t_u, gx, const_u32(b, 0x80000000)), gx)
			flags := extract(b, t.t_u, sys_uvec4(t, .Alpha_Test), 2)
			point := op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, flags, const_u32(b, SYS_FLAG_POINT_PRIMITIVE)), const_u32(b, 0))
			gy := bitcast(b, t.t_u, extract(b, t.t_f, g, 1))
			gy = select(b, t.t_u, point, op1(b, .BitwiseOr, t.t_u, gy, const_u32(b, 0x80000000)), gy)
			// gl_PointCoord is undefined for other primitives (selected away); the clamp only guards the [0, 1] range the
			// shaders expect (xenia saturates its interpolated sprite coordinates the same way)
			pc := ext(b, b.t_vec[2], .NClamp, load(b, b.t_vec[2], pcv), const_splat(b, 2, 0), const_splat(b, 2, 1))
			ps := select(b, t.t_f, point, extract(b, t.t_f, pc, 0), const_f32(b, 0))
			pt := select(b, t.t_f, point, extract(b, t.t_f, pc, 1), const_f32(b, 0))
			store(b, reg_ptr(t, pg, false), op1(b, .CompositeConstruct, t.t_v4, bitcast(b, t.t_f, gx), bitcast(b, t.t_f, gy), ps, pt))
		}
	}
}

@(private)
fmt_name :: proc(f: string, args: ..any) -> string {
	return fmt.tprintf(f, ..args)
}

@(private)
stage_outputs :: proc(t: ^T) {
	b := &t.b
	switch t.model {
	case .Vertex, .Tess_Eval:
		pos := load(b, t.t_v4, t.v_opos)
		sc := sys_vec4(t, .Ndc_Scale)
		of := sys_vec4(t, .Ndc_Offset)
		w := extract(b, t.t_f, pos, 3)
		p := arith(b, .FAdd, t.t_v4, arith(b, .FMul, t.t_v4, pos, sc), arith(b, .VectorTimesScalar, t.t_v4, of, w))
		p = op1(b, .CompositeInsert, t.t_v4, w, p, 3)
		gp := output_var(t, t.t_v4, i32(BuiltIn.Position))
		decorate(b, gp, .Invariant)
		name(b, gp, "gl_Position")
		store(b, gp, p)
		// user clip planes: fixed function on the Xenos (PA_CL_UCP, PA_CL_CLIP_CNTL), here distances from the clip-space
		// position (as Xenia's user_clip_planes); a disabled plane is zero, distance 0, never clipped
		capability(b, 32) // ClipDistance
		cd_t := type_array(b, t.t_f, SYS_CLIP_PLANES)
		cd := output_var(t, cd_t, 3) // ClipDistance
		name(b, cd, "gl_ClipDistance")
		pf := type_ptr(b, .Output, t.t_f)
		for i in u32(0) ..< SYS_CLIP_PLANES {
			if t.abi6 {t.meta.module_meta.system_rows |= 1 << (sys_row(.Clip_Planes) + i)}
			plane := load(b, t.t_v4, access(b, type_ptr(b, .Uniform, t.t_v4), t.u_sys, const_u32(b, u32(Sys.Clip_Planes)), const_u32(b, i)))
			store(b, access(b, pf, cd, const_u32(b, i)), op1(b, .Dot, t.t_f, p, plane))
		}
		if t.meta.point_size {
			ps := output_var(t, t.t_f, i32(BuiltIn.PointSize))
			pt := sys_vec4(t, .Point)
			s := extract(b, t.t_f, load(b, t.t_v4, t.v_opts), 0)
			s = ext(b, t.t_f, .NClamp, s, extract(b, t.t_f, pt, 0), extract(b, t.t_f, pt, 1))
			s = arith(b, .FDiv, t.t_f, s, extract(b, t.t_f, sys_vec4(t, .Pixel_Scale), 0))
			store(b, ps, s)
		}
		used: u32
		for v in t.c.interpolators {
			loc := interface_location(v.usage, v.index)
			if used & (1 << loc) != 0 || v.reg >= 16 {continue}
			used |= 1 << loc
			ov := output_var(t, t.t_v4, -1, i32(loc))
			name(b, ov, fmt_name("out_%v%d", v.usage, v.index))
			store(b, ov, load(b, t.t_v4, access(b, t.t_pf_v4, t.v_o, const_u32(b, v.reg))))
		}
		t.meta.vs_outputs = used
	case .Fragment:
		if t.meta.color_outputs & 1 != 0 {
			// alpha test on oC0.a: gated by SpecId 0 (alpha_test_enable, default false) so opaque draws
			// dead-code-eliminate OpDemoteToHelperInvocation and keep hardware Early-Z / Hi-Z
			alpha_test_enable := spec_bool(b, 0, false)
			name(b, alpha_test_enable, "alpha_test_enable")
			at_if := if_begin(b, alpha_test_enable)
			// Xenos CompareFunction bits (1 less, 2 equal, 4 greater), 7 = always
			at := sys_uvec4(t, .Alpha_Test)
			fn := extract(b, t.t_u, at, 0)
			ref := bitcast(b, t.t_f, extract(b, t.t_u, at, 1))
			a := extract(b, t.t_f, load(b, t.t_v4, access(b, t.t_pf_v4, t.v_oc, const_u32(b, 0))), 3)
			bit :: proc(t: ^T, fn: u32, m: u32) -> u32 {return op1(&t.b, .INotEqual, t.t_b, op1(&t.b, .BitwiseAnd, t.t_u, fn, const_u32(&t.b, m)), const_u32(&t.b, 0))}
			lt := op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdLessThan, t.t_b, a, ref), bit(t, fn, 1))
			eq := op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdEqual, t.t_b, a, ref), bit(t, fn, 2))
			gt := op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdGreaterThan, t.t_b, a, ref), bit(t, fn, 4))
			pass := op1(b, .LogicalOr, t.t_b, op1(b, .LogicalOr, t.t_b, lt, eq), gt)
			always := op1(b, .IEqual, t.t_b, fn, const_u32(b, 7))
			pass = op1(b, .LogicalOr, t.t_b, pass, always)
			capability(b, 5379)
			f := if_begin(b, op1(b, .LogicalNot, t.t_b, pass))
			op0(b, .DemoteToHelperInvocation)
			if_end(b, f)
			if_end(b, at_if)
		}
		bias := sys_vec4(t, .Color_Exp_Bias)
		for i in u32(0) ..< 4 {
			if t.meta.color_outputs & (1 << i) == 0 {continue}
			ov := output_var(t, t.t_v4, -1, i32(i))
			name(b, ov, fmt_name("oC%d", i))
			c := load(b, t.t_v4, access(b, t.t_pf_v4, t.v_oc, const_u32(b, i)))
			store(b, ov, arith(b, .VectorTimesScalar, t.t_v4, c, extract(b, t.t_f, bias, i)))
		}
		if t.meta.depth_output {
			d := output_var(t, t.t_f, i32(BuiltIn.FragDepth))
			name(b, d, "gl_FragDepth")
			// clamped: Vulkan leaves FragDepth outside [0, 1] undefined without VK_EXT_depth_range_unrestricted (Xenia)
			dz := extract(b, t.t_f, load(b, t.t_v4, t.v_odepth), 0)
			store(b, d, ext(b, t.t_f, .NClamp, dz, const_f32(b, 0), const_f32(b, 1)))
		}
	case .Compute:
	}
}

// ---- endian ----

@(private)
endian_swap :: proc(t: ^T, n: int, w: u32, e: u32) -> u32 {
	b := &t.b
	ty := b.t_uvec[n]
	// 8in16
	lo := op1(b, .ShiftLeftLogical, ty, op1(b, .BitwiseAnd, ty, w, const_usplat(b, n, 0x00ff00ff)), const_usplat(b, n, 8))
	hi := op1(b, .BitwiseAnd, ty, op1(b, .ShiftRightLogical, ty, w, const_usplat(b, n, 8)), const_usplat(b, n, 0x00ff00ff))
	s1 := op1(b, .BitwiseOr, ty, lo, hi)
	rot :: proc(b: ^Builder, ty: u32, n: int, x: u32) -> u32 {
		return op1(b, .BitwiseOr, ty, op1(b, .ShiftLeftLogical, ty, x, const_usplat(b, n, 16)), op1(b, .ShiftRightLogical, ty, x, const_usplat(b, n, 16)))
	}
	s2 := rot(b, ty, n, s1) // 8in32
	s3 := rot(b, ty, n, w) // 16in32
	bv := b.t_bvec[n]
	splat_e :: proc(t: ^T, n: int, e: u32) -> u32 {
		if n == 1 {return e}
		parts := [4]u32{e, e, e, e}
		return op1(&t.b, .CompositeConstruct, t.b.t_uvec[n], ..parts[:n])
	}
	ev := splat_e(t, n, e)
	r := select(b, ty, op1(b, .IEqual, bv, ev, const_usplat(b, n, 1)), s1, w)
	r = select(b, ty, op1(b, .IEqual, bv, ev, const_usplat(b, n, 2)), s2, r)
	r = select(b, ty, op1(b, .IEqual, bv, ev, const_usplat(b, n, 3)), s3, r)
	return r
}

// ---- vertex fetch ----

@(private)
VF_Vars :: struct {
	raw, width, fval: u32,
}

@(private)
vf_vars :: proc(t: ^T) -> VF_Vars {
	if t.vf.raw == 0 {t.vf = {local_var(&t.b, t.b.t_uvec[4]), local_var(&t.b, t.b.t_uvec[4]), local_var(&t.b, t.t_v4)}}
	return t.vf
}

@(private)
emit_vfetch :: proc(t: ^T, in_: ^Instr) {
	b := &t.b
	if t.u_vf == 0 {return}
	f := &in_.vf
	full := &in_.vf_full
	// the vertex element the D3D runtime patches into this vfetch
	k := -1
	for v, i in t.c.vertex_elements {if v.address == in_.address {k = i}}
	if k < 0 {k = 31} // not in the patch list: element 31 (left unbound = zeros unless the device fills it)
	if t.abi6 && k >= 32 {
		t.err = "vertex element outside XeVertexFetch"
		return
	}
	t.meta.vertex_elements = max(t.meta.vertex_elements, k + 1)
	uv4 := b.t_uvec[4]
	element :: proc(t: ^T, member: u32, ty: u32, k: int) -> u32 {
		b := &t.b
		if t.abi6 {
			row := 16 + 32 * (member - 1) + u32(k)
			t.meta.module_meta.vertex_fetch_rows[row >> 5] |= 1 << (row & 31)
		}
		return load(b, ty, access(b, type_ptr(b, .Uniform, ty), t.u_vf, const_u32(b, member), const_u32(b, u32(k))))
	}
	el := element(t, 1, uv4, k)
	stream := op1(b, .BitwiseAnd, t.t_u, extract(b, t.t_u, el, 0), const_u32(b, 15))
	if t.abi6 {t.meta.module_meta.vertex_fetch_rows[0] |= 0xffff} // Any of the 16 stream descriptors.
	st := load(b, uv4, access(b, type_ptr(b, .Uniform, uv4), t.u_vf, const_u32(b, 0), stream))
	offset := extract(b, t.t_u, el, 1)
	flags := extract(b, t.t_u, el, 3)
	size := extract(b, t.t_u, st, 2)
	sw := extract(b, t.t_u, st, 3)
	stride := op1(b, .BitwiseAnd, t.t_u, sw, const_u32(b, 0xffff))
	endian := op1(b, .BitFieldUExtract, t.t_u, sw, const_u32(b, 16), const_u32(b, 2))
	// index from the source register
	src := load(b, t.t_v4, reg_ptr(t, full.src_reg, full.src_rel))
	fi := extract(b, t.t_f, src, full.src_swz)
	if full.index_rounded {fi = arith(b, .FAdd, t.t_f, fi, const_f32(b, 0.5))}
	idx := op1(b, .ConvertFToS, t.t_i, ext(b, t.t_f, .Floor, fi))
	byte_addr := op1(b, .IAdd, t.t_i, op1(b, .IMul, t.t_i, idx, bitcast(b, t.t_i, stride)), bitcast(b, t.t_i, offset))
	nonneg := op1(b, .SGreaterThanEqual, t.t_b, idx, const_i32(b, 0))
	ba := bitcast(b, t.t_u, byte_addr)
	ptr := bitcast(b, t.t_words_ptr, op1(b, .CompositeConstruct, b.t_uvec[2], extract(b, t.t_u, st, 0), extract(b, t.t_u, st, 1)))
	words: [4]u32
	for j in u32(0) ..< 4 {
		end_ := op1(b, .IAdd, t.t_u, ba, const_u32(b, 4 * (j + 1)))
		ok := op1(b, .LogicalAnd, t.t_b, nonneg, op1(b, .ULessThanEqual, t.t_b, end_, size))
		wi := op1(b, .IAdd, t.t_u, op1(b, .ShiftRightLogical, t.t_u, ba, const_u32(b, 2)), const_u32(b, j))
		wi = select(b, t.t_u, ok, wi, const_u32(b, 0))
		p := access(b, t.t_word_ptr, ptr, const_u32(b, 0), wi)
		w := op1(b, .Load, t.t_u, p, 2, 4) // Aligned 4
		words[j] = select(b, t.t_u, ok, w, const_u32(b, 0))
	}
	wv := op1(b, .CompositeConstruct, uv4, words[0], words[1], words[2], words[3])
	wv = endian_swap(t, 4, wv, endian)
	val := call(b, t.t_v4, vfetch_decode_fn(t), wv, element(t, 2, uv4, k), element(t, 3, t.t_v4, k))
	val = vfetch_declaration_swizzle(t, val, flags)
	store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, val)
}

// The declaration type's swizzle (VF_SWIZZLE_SHIFT, when VF_HAS_SWIZZLE): the Xbox D3D runtime composes it with the
// vfetch instruction's destination swizzle when it patches the instruction, so the shader sees the fetched value
// through it first (D3DCOLOR: ZYXW, FLOAT3: XYZ1, FLOAT2: XY01). Dynamically uniform per draw.
@(private)
vfetch_declaration_swizzle :: proc(t: ^T, val: u32, flags: u32) -> u32 {
	b := &t.b
	has := op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, flags, const_u32(b, VF_HAS_SWIZZLE)), const_u32(b, 0))
	comps: [4]u32
	for k in u32(0) ..< 4 {
		s := op1(b, .BitFieldUExtract, t.t_u, flags, const_u32(b, VF_SWIZZLE_SHIFT + 3 * k), const_u32(b, 3))
		// 0-3: that component, 5: one, 4 / 6 / 7: zero
		picked := op1(b, .VectorExtractDynamic, t.t_f, val, op1(b, .BitwiseAnd, t.t_u, s, const_u32(b, 3)))
		constant := select(b, t.t_f, op1(b, .IEqual, t.t_b, s, const_u32(b, 5)), const_f32(b, 1), const_f32(b, 0))
		c := select(b, t.t_f, op1(b, .ULessThan, t.t_b, s, const_u32(b, 4)), picked, constant)
		comps[k] = select(b, t.t_f, has, c, extract(b, t.t_f, val, k))
	}
	return op1(b, .CompositeConstruct, t.t_v4, comps[0], comps[1], comps[2], comps[3])
}

// vec4 xe_vfetch_decode(uvec4 words, uvec4 unpack, vec4 denominator): one helper per module, every vfetch calls it.
// The element's layout comes from the device (XeVertexFetch.unpack / .denominator, abi.odin VF_MODE_*), so every
// format decodes with the same code: each component is a bit field of one of the four words; fixed point is
// normalized by its divisor (signed: clamped to -1, "no zero": (2x + 1) / (2^w - 1)) or kept as an integer, 16-bit
// floats are widened, 32-bit floats are the words; then the exp_adjust scale.
@(private)
vfetch_decode_fn :: proc(t: ^T) -> u32 {
	if t.fn_vdecode != 0 {return t.fn_vdecode}
	b := &t.b
	uv4, iv4, v4, bv4 := b.t_uvec[4], b.t_ivec[4], t.t_v4, b.t_bvec[4]
	h := helper_begin(b, v4, {uv4, uv4, v4})
	wv, un, den := h.params[0], h.params[1], h.params[2]
	z := const_u32(b, 0)
	layout := extract(b, t.t_u, un, 0)
	widths := extract(b, t.t_u, un, 1)
	mode_flags := extract(b, t.t_u, un, 2)
	raw, sx, width: [4]u32
	for c in u32(0) ..< 4 {
		at := op1(b, .BitFieldUExtract, t.t_u, layout, const_u32(b, 8 * c), const_u32(b, 8))
		bit := op1(b, .BitwiseAnd, t.t_u, at, const_u32(b, 31))
		word := op1(b, .VectorExtractDynamic, t.t_u, wv, op1(b, .ShiftRightLogical, t.t_u, at, const_u32(b, 5)))
		width[c] = op1(b, .BitFieldUExtract, t.t_u, widths, const_u32(b, 8 * c), const_u32(b, 8))
		raw[c] = op1(b, .BitFieldUExtract, t.t_u, word, bit, width[c])
		sx[c] = op1(b, .BitFieldSExtract, t.t_i, bitcast(b, t.t_i, word), bit, width[c])
	}
	rawv := op1(b, .CompositeConstruct, uv4, raw[0], raw[1], raw[2], raw[3])
	sxv := op1(b, .CompositeConstruct, iv4, sx[0], sx[1], sx[2], sx[3])
	flag :: proc(b: ^Builder, w: u32, bit: u32) -> u32 {
		return op1(b, .INotEqual, b.t_bool, op1(b, .BitwiseAnd, b.t_uint, w, const_u32(b, bit)), const_u32(b, 0))
	}
	signed := flag(b, mode_flags, VF_SIGNED)
	integer := flag(b, mode_flags, VF_INTEGER)
	no_zero := flag(b, mode_flags, VF_NO_ZERO)
	mode := op1(b, .BitFieldUExtract, t.t_u, mode_flags, const_u32(b, VF_MODE_SHIFT), const_u32(b, 2))
	sv :: proc(t: ^T, c, x, y: u32) -> u32 {
		cv := op1(&t.b, .CompositeConstruct, t.b.t_bvec[4], c, c, c, c)
		return select(&t.b, t.t_v4, cv, x, y)
	}
	one4 := const_splat(b, 4, 1)
	// fixed point
	x := sv(t, signed, op1(b, .ConvertSToF, v4, sxv), op1(b, .ConvertUToF, v4, rawv))
	xn := sv(t, no_zero, arith(b, .FAdd, v4, arith(b, .FMul, v4, x, const_splat(b, 4, 2)), one4), x)
	q := arith(b, .FDiv, v4, xn, den)
	clamped := op1(b, .LogicalAnd, t.t_b, signed, op1(b, .LogicalNot, t.t_b, no_zero))
	q = sv(t, clamped, ext(b, v4, .FMax, q, const_splat(b, 4, -1)), q)
	fixed := sv(t, integer, x, q)
	has := op1(b, .INotEqual, bv4, op1(b, .CompositeConstruct, uv4, width[0], width[1], width[2], width[3]), const_usplat(b, 4, 0))
	fixed = select(b, v4, has, fixed, const_splat(b, 4, 0))
	// floats
	half: [4]u32
	for c in 0 ..< 4 {half[c] = extract(b, t.t_f, ext(b, b.t_vec[2], .UnpackHalf2x16, raw[c]), 0)}
	halves := op1(b, .CompositeConstruct, v4, half[0], half[1], half[2], half[3])
	is :: proc(b: ^Builder, mode: u32, m: u32) -> u32 {return op1(b, .IEqual, b.t_bool, mode, const_u32(b, m))}
	val := sv(t, is(b, mode, VF_MODE_HALF), halves, sv(t, is(b, mode, VF_MODE_FLOAT), bitcast(b, v4, rawv), fixed))
	_ = z
	val = arith(b, .VectorTimesScalar, v4, val, bitcast(b, t.t_f, extract(b, t.t_u, un, 3)))
	op0(b, .ReturnValue, val)
	b.block_open = false
	helper_end(b, &h)
	t.fn_vdecode = h.id
	return h.id
}
// ---- texture fetch ----

// host image kinds (binding bases in abi.odin)
TEX_2D :: 0
TEX_3D :: 1
TEX_CUBE :: 2
TEX_STACKED :: 3 // 2D array: a Xenos "3D" fetch constant whose data dimension is stacked 2D

@(private)
image_type :: proc(t: ^T, kind: u32) -> u32 {
	dims := [4]u32{1, 2, 3, 1} // SPIR-V Dim 2D, 3D, Cube, 2D (arrayed)
	return type_get(&t.b, .TypeImage, t.t_f, dims[kind], 0, kind == TEX_STACKED ? 1 : 0, 0, 1, 0)
}

@(private)
declare_bindless :: proc(t: ^T) {
	b := &t.b
	capability(b, 29) // SampledImageArrayDynamicIndexing
	capability(b, 5302) // RuntimeDescriptorArray (SPIR-V 1.5)
	declare :: proc(t: ^T, binding, ty: u32, var_name: string) -> u32 {
		b := &t.b
		v := global_var(b, .UniformConstant, type_get(b, .TypeRuntimeArray, ty))
		decorate(b, v, .DescriptorSet, SET_TEXTURES)
		decorate(b, v, .Binding, binding)
		name(b, v, var_name)
		append(&t.interface, v)
		return v
	}
	for var_name, kind in ([?]string{"xe_images_2d", "xe_images_3d", "xe_images_cube", "xe_images_2d_array"}) {
		t.t_img[kind] = image_type(t, u32(kind))
		t.bindless_images[kind] = declare(t, u32(kind), t.t_img[kind], var_name)
	}
	t.t_smp = type_get(b, .TypeSampler)
	t.bindless_samplers = declare(t, BINDLESS_SAMPLER, t.t_smp, "xe_samplers")
}

@(private)
bindless_descriptor :: proc(t: ^T, ty, var: u32, table: Push_Member, offset_id: ^u32) -> u32 {
	b := &t.b
	// Reserve a constant ID on first use. Its value is set once all samples have been emitted.
	if offset_id^ == 0 {offset_id^ = id(b)}
	table_offset := stage_table_offset(t, table)
	offset := op1(b, .IAdd, t.t_u, table_offset, offset_id^)
	slot := arena_load(t, offset, t.t_u, t.t_word_ptr, 4)
	return access(b, type_ptr(b, .UniformConstant, ty), var, slot)
}

@(private)
assign_bindless_offsets :: proc(t: ^T) {
	assign :: proc(b: ^Builder, offsets: []u32, keys: ^[dynamic]u32) {
		for offset_id, key in offsets {
			if offset_id == 0 {continue}
			emit(&b.globals, .Constant, b.t_uint, offset_id, 4 * u32(len(keys^)))
			append(keys, u32(key))
		}
	}
	assign(&t.b, t.image_offsets[:], &t.meta.image_keys)
	assign(&t.b, t.sampler_offsets[:], &t.meta.sampler_keys)
}

@(private)
texture_var :: proc(t: ^T, fetch: u32, kind: u32) -> u32 {
	b := &t.b
	key := fetch | kind << 8
	if v, ok := t.tex[key]; ok {return v}
	if t.t_img[kind] == 0 {t.t_img[kind] = image_type(t, kind)}
	v := global_var(b, .UniformConstant, t.t_img[kind])
	bases := [4]u32{BINDING_TEX_2D, BINDING_TEX_3D, BINDING_TEX_CUBE, BINDING_TEX_STACKED}
	decorate(b, v, .DescriptorSet, SET_TEXTURES)
	decorate(b, v, .Binding, bases[kind] + fetch)
	dn := [4]string{"2d", "3d", "cube", "stacked"}
	name(b, v, fmt_name("tf%d_%s", fetch, dn[kind]))
	append(&t.interface, v)
	t.tex[key] = v
	return v
}

@(private)
sampler_var :: proc(t: ^T, fetch: u32, variant: u32) -> u32 {
	b := &t.b
	key := fetch | variant << 8
	if v, ok := t.smp[key]; ok {return v}
	if t.t_smp == 0 {t.t_smp = type_get(b, .TypeSampler)}
	v := global_var(b, .UniformConstant, t.t_smp)
	decorate(b, v, .DescriptorSet, SET_TEXTURES)
	decorate(b, v, .Binding, BINDING_SAMPLER + SAMPLER_VARIANT_STRIDE * variant + fetch)
	name(b, v, fmt_name("tf%d_sampler%d", fetch, variant))
	append(&t.interface, v)
	t.smp[key] = v
	return v
}

// the instruction's filter overrides -> sampler variant (abi.odin Sampler_Variant)
@(private)
sampler_variant :: proc(f: ^Texture_Fetch) -> u32 {
	if f.mag_filter == 0 && f.min_filter == 0 {
		return (f.mip_filter == 0 || f.aniso_filter == 0) ? u32(Sampler_Variant.Point_Mip_Point) : u32(Sampler_Variant.Point)
	}
	if f.mag_filter == 1 && f.min_filter == 1 {
		return f.mip_filter == 1 ? u32(Sampler_Variant.Linear_Mip_Linear) : u32(Sampler_Variant.Linear)
	}
	// Xenia: anisotropy is off for a fetch without a computed LOD, or one that overrides it off
	if !f.use_comp_lod || f.aniso_filter == 0 {return u32(Sampler_Variant.No_Aniso)}
	return u32(Sampler_Variant.Fetch_Constant)
}

// guest texture size (texels) for an image kind from fetch constant dword 2 (stored minus one)
@(private)
fetch_sizes :: proc(t: ^T, fc: u32, kind: u32) -> (s: [3]u32) {
	b := &t.b
	d2 := ubo_dword(t, t.u_fetch, 0, fc * 6 + 2)
	switch kind {
	case TEX_3D: s = {ux1(t, d2, 0, 11), ux1(t, d2, 11, 11), ux1(t, d2, 22, 10)}
	case TEX_STACKED: s = {ux1(t, d2, 0, 13), ux1(t, d2, 13, 13), ux1(t, d2, 26, 6)}
	case: s = {ux1(t, d2, 0, 13), ux1(t, d2, 13, 13), const_f32(b, 1)}
	}
	return
}

// Normalize the guest coordinates and apply the instruction's offsets in guest texels.
@(private)
fetch_coords :: proc(t: ^T, f: ^Texture_Fetch, c: [3]u32, sizes: [3]u32, n: int) -> (out: [3]u32) {
	b := &t.b
	out = c
	offs := [3]i32{f.offset_x, f.offset_y, f.offset_z}
	epsilon := TEXEL_EPSILON
	if f.dimension == 1 && f.unnormalized && f.mag_filter == 0 && f.min_filter == 0 {
		// A point fetch at integer VPOS belongs to the texel starting there. At 3840 wide,
		// 1.5/1024 texel can round back across that boundary in the normalized sampler,
		// reading the wall's G-buffer at the weapon edge. Use one 8-bit subtexel step.
		epsilon = 1.0 / 256
	}
	for k in 0 ..< n {
		if f.unnormalized {out[k] = arith(b, .FDiv, t.t_f, out[k], sizes[k])}
		out[k] = arith(b, .FAdd, t.t_f, out[k], arith(b, .FDiv, t.t_f, const_f32(b, f32(offs[k]) * 0.5 + epsilon), sizes[k]))
	}
	return
}

@(private)
sample :: proc(t: ^T, f: ^Texture_Fetch, kind: u32, coord: u32) -> u32 {
	b := &t.b
	fc := f.const_index
	variant := sampler_variant(f)
	found := false
	for u in t.meta.textures {if u.fetch == fc && u.dim == kind && u.sampler == variant {found = true}}
	if !found {append(&t.meta.textures, Texture_Use{fetch = fc, dim = kind, sampler = variant})}
	iv, sv: u32
	if t.abi6 {
		iv = bindless_descriptor(t, t.t_img[kind], t.bindless_images[kind], .VS_Tex_Table, &t.image_offsets[(kind << 5) | fc])
		sv = bindless_descriptor(t, t.t_smp, t.bindless_samplers, .VS_Samp_Table, &t.sampler_offsets[(variant << 5) | fc])
	} else {
		iv = texture_var(t, fc, kind)
		sv = sampler_var(t, fc, variant)
	}
	si := op1(b, .SampledImage, type_get(b, .TypeSampledImage, t.t_img[kind]), load(b, t.t_img[kind], iv), load(b, t.t_smp, sv))
	frag := t.model == .Fragment
	// LOD: instruction LODBias (+ register LOD); the fetch constant's LOD bias is the host sampler's mipLodBias
	// (Vulkan adds it to implicit and explicit LODs alike)
	bias := f32(f.lod_bias) / 16
	lod := const_f32(b, bias)
	if f.use_reg_lod {lod = arith(b, .FAdd, t.t_f, load(b, t.t_f, t.v_lod), lod)}
	if f.use_comp_lod && f.use_reg_gradients {
		// register gradients (setGradientH/V), scaled by 2^(bias + register LOD) as Xenia does
		gn := kind == TEX_2D || kind == TEX_STACKED ? 2 : 3
		scale := ext(b, t.t_f, .Exp2, lod)
		gh := arith(b, .VectorTimesScalar, b.t_vec[3], load(b, b.t_vec[3], t.v_grad_h), scale)
		gv := arith(b, .VectorTimesScalar, b.t_vec[3], load(b, b.t_vec[3], t.v_grad_v), scale)
		if gn == 2 {
			gh = op1(b, .VectorShuffle, b.t_vec[2], gh, gh, 0, 1)
			gv = op1(b, .VectorShuffle, b.t_vec[2], gv, gv, 0, 1)
		}
		return op1(b, .ImageSampleExplicitLod, t.t_v4, si, coord, 0x4, gh, gv)
	}
	if frag && f.use_comp_lod {
		if f.use_reg_lod || bias != 0 {return op1(b, .ImageSampleImplicitLod, t.t_v4, si, coord, 0x1, lod)}
		return op1(b, .ImageSampleImplicitLod, t.t_v4, si, coord)
	}
	if frag && kind == TEX_CUBE {
		// Halo 3 PC MCC (environment_mapping.fx): bias explicit-LOD cubemap fetches by screen-space
		// derivative of the normalized direction to eliminate 1px bevel/silhouette rim aliasing.
		ff := t.t_f
		v3 := b.t_vec[3]
		dir := ext(b, v3, .Normalize, coord)
		gx := ext(b, ff, .Length, op1(b, .DPdx, v3, dir))
		gy := ext(b, ff, .Length, op1(b, .DPdy, v3, dir))
		gmax := ext(b, ff, .NMax, gx, gy)
		base_lod := arith(b, .FSub, ff, arith(b, .FMul, ff, const_f32(b, 6.0), ext(b, ff, .Sqrt, gmax)), const_f32(b, 0.6))
		lod = ext(b, ff, .NMax, lod, base_lod)
	}
	return op1(b, .ImageSampleExplicitLod, t.t_v4, si, coord, 0x2, lod)
}

// A tfetch3D of stacked 2D layers (normalized coordinates c, layer count `depth`). The layers are addressed as the
// slices of a 3D texture, not as Vulkan array layers: the layer coordinate is z * depth, filtered between layers by
// the volume filter — the instruction's VolMag/VolMinFilter (0 point, 1 linear) or, when it defers (3), the fetch
// constant's (dword 4 bits 0 / 1, which Xbox D3D sets from MAGFILTER / MINFILTER, or MAGFILTERZ / MINFILTERZ with
// SEPARATEZFILTERENABLE) — point taking the layer floor(z * depth), linear lerping the layers on either side of
// z * depth - 0.5 by its fraction (Xenia's stacked fetch; Halo 3's water vertex shader samples its 16-layer wave
// displacement array with the time as z, MCC's DX11 build of water_shading.fx doing this lerp by hand). The
// minification filter applies where the layer coordinate changes by more than one layer per pixel (computed
// gradients only: a fetch without them magnifies). Layer indices follow the fetch constant's ClampZ (dword 0 bits
// 16-18): wrap repeats them (the wave animation's last layer blends into its first); the other modes clamp to the
// first / last layer.
@(private)
sample_stacked :: proc(t: ^T, f: ^Texture_Fetch, c: [3]u32, depth: u32) -> u32 {
	b := &t.b
	ff := t.t_f
	fc := f.const_index
	vol_linear :: proc(t: ^T, fc: u32, filter: u32, bit: u32) -> (id: u32, static_point: bool) {
		b := &t.b
		switch filter {
		case 1: return const_bool(b, true), false
		case 3:
			d4 := ubo_dword(t, t.u_fetch, 0, fc * 6 + 4)
			return op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, d4, const_u32(b, 1 << bit)), const_u32(b, 0)), false
		}
		return const_bool(b, false), true
	}
	gradients := t.model == .Fragment && f.use_comp_lod
	mag, mag_point := vol_linear(t, fc, f.vol_mag_filter, 0)
	linear := mag
	static_point := mag_point
	if gradients && (f.vol_min_filter != f.vol_mag_filter || f.vol_mag_filter == 3) { // the filters may differ
		min, min_point := vol_linear(t, fc, f.vol_min_filter, 1)
		// layers per pixel: the larger screen-space change of the layer coordinate
		gz_h, gz_v: u32
		if f.use_reg_gradients {
			gz_h = extract(b, ff, load(b, b.t_vec[3], t.v_grad_h), 2)
			gz_v = extract(b, ff, load(b, b.t_vec[3], t.v_grad_v), 2)
		} else {
			gz_h = op1(b, .DPdx, ff, c[2])
			gz_v = op1(b, .DPdy, ff, c[2])
		}
		g := ext(b, ff, .NMax, ext(b, ff, .FAbs, gz_h), ext(b, ff, .FAbs, gz_v))
		if f.use_reg_gradients && f.unnormalized {g = arith(b, .FDiv, ff, g, depth)}
		minifying := op1(b, .FOrdGreaterThan, t.t_b, arith(b, .FMul, ff, g, depth), const_f32(b, 1)) // NaN magnifies
		linear = select(b, t.t_b, minifying, min, mag)
		static_point = mag_point && min_point
	}
	z := arith(b, .FMul, ff, c[2], depth)
	if !static_point {z = select(b, ff, linear, arith(b, .FSub, ff, z, const_f32(b, 0.5)), z)}
	l0 := ext(b, ff, .Floor, z)
	d0 := ubo_dword(t, t.u_fetch, 0, fc * 6 + 0)
	wrap := op1(b, .IEqual, t.t_b, op1(b, .BitFieldUExtract, t.t_u, d0, const_u32(b, 16), const_u32(b, 3)), const_u32(b, 0))
	layer := proc(t: ^T, l, depth, wrap: u32) -> u32 {
		// wrap: l mod depth (floored); otherwise as is, the sampler clamping it to 0 .. depth - 1
		b := &t.b
		m := arith(b, .FSub, t.t_f, l, arith(b, .FMul, t.t_f, depth, ext(b, t.t_f, .Floor, arith(b, .FDiv, t.t_f, l, depth))))
		return select(b, t.t_f, wrap, m, l)
	}
	s0 := sample(t, f, TEX_STACKED, op1(b, .CompositeConstruct, b.t_vec[3], c[0], c[1], layer(t, l0, depth, wrap)))
	if static_point {return s0}
	l1 := arith(b, .FAdd, ff, l0, const_f32(b, 1))
	s1 := sample(t, f, TEX_STACKED, op1(b, .CompositeConstruct, b.t_vec[3], c[0], c[1], layer(t, l1, depth, wrap)))
	frac := select(b, ff, linear, arith(b, .FSub, ff, z, l0), const_f32(b, 0))
	return arith(b, .FAdd, t.t_v4, s0, arith(b, .VectorTimesScalar, t.t_v4, arith(b, .FSub, t.t_v4, s1, s0), frac))
}

@(private)
emit_tfetch :: proc(t: ^T, in_: ^Instr) {
	b := &t.b
	f := &in_.tf
	src := load(b, t.t_v4, reg_ptr(t, f.src_reg, f.src_rel))
	cx := extract(b, t.t_f, src, f.src_swz & 3)
	cy := extract(b, t.t_f, src, (f.src_swz >> 2) & 3)
	cz := extract(b, t.t_f, src, (f.src_swz >> 4) & 3)
	zf := const_f32(b, 0)
	frag := t.model == .Fragment
	#partial switch f.op {
	case .Set_Tex_Lod:
		store(b, t.v_lod, cx)
		return
	case .Set_Grad_H, .Set_Grad_V:
		store(b, f.op == .Set_Grad_H ? t.v_grad_h : t.v_grad_v, op1(b, .CompositeConstruct, b.t_vec[3], cx, cy, cz))
		return
	case .Get_Gradients:
		r: u32
		if frag {
			r = op1(b, .CompositeConstruct, t.t_v4, op1(b, .DPdx, t.t_f, cx), op1(b, .DPdy, t.t_f, cx), op1(b, .DPdx, t.t_f, cy), op1(b, .DPdy, t.t_f, cy))
		} else {
			r = const_splat(b, 4, 0)
		}
		store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, r)
		return
	case .Get_Border_Frac, .Get_Comp_Tex_Lod:
		store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, const_splat(b, 4, 0))
		return
	case .Texture_Fetch, .Get_Weights:
	case:
		store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, const_splat(b, 4, 0))
		return
	}
	dim: u32 = 0
	switch f.dimension {
	case 2: dim = 1
	case 3: dim = 2
	}
	// guest texture size from the fetch constant (dword 2)
	fc := f.const_index
	coords := [3]u32{cx, cy, cz}
	r: u32
	switch dim {
	case 2:
		if f.op == .Get_Weights {
			store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, const_splat(b, 4, 0)) // cube lerp factors: unused by Halo 3
			return
		}
		// cube: SC / TC in 1..2 and the face index (cube ALU instruction output) back to a direction (Xenia)
		ff := t.t_f
		sc := arith(b, .FSub, ff, arith(b, .FMul, ff, cx, const_f32(b, 2)), const_f32(b, 3))
		tc := arith(b, .FSub, ff, arith(b, .FMul, ff, cy, const_f32(b, 2)), const_f32(b, 3))
		face := op1(b, .ConvertFToU, t.t_u, ext(b, ff, .NClamp, ext(b, ff, .Floor, cz), zf, const_f32(b, 5)))
		axis := op1(b, .ShiftRightLogical, t.t_u, face, const_u32(b, 1))
		neg := op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, face, const_u32(b, 1)), const_u32(b, 0))
		sign := select(b, ff, neg, const_f32(b, -1), const_f32(b, 1))
		nsc, ntc := op1(b, .FNegate, ff, sc), op1(b, .FNegate, ff, tc)
		is_x := op1(b, .IEqual, t.t_b, axis, const_u32(b, 0))
		is_y := op1(b, .IEqual, t.t_b, axis, const_u32(b, 1))
		// x major: (sign, -tc, neg ? sc : -sc); y major: (sc, sign, neg ? -tc : tc); z major: (neg ? -sc : sc, -tc, sign)
		x := select(b, ff, is_x, sign, select(b, ff, is_y, sc, select(b, ff, neg, nsc, sc)))
		y := select(b, ff, is_x, ntc, select(b, ff, is_y, sign, ntc))
		zz := select(b, ff, is_x, select(b, ff, neg, sc, nsc), select(b, ff, is_y, select(b, ff, neg, ntc, tc), sign))
		r = sample(t, f, TEX_CUBE, op1(b, .CompositeConstruct, b.t_vec[3], x, y, zz))
	case 0:
		sizes := fetch_sizes(t, fc, TEX_2D)
		if f.op == .Get_Weights {
			store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, weights(t, f, coords, sizes, 2))
			return
		}
		c := fetch_coords(t, f, coords, sizes, 2)
		r = sample(t, f, TEX_2D, op1(b, .CompositeConstruct, b.t_vec[2], c[0], c[1]))
	case 1:
		// tfetch3D: a real 3D texture or stacked 2D layers, by the fetch constant's data dimension (dword 5 bits
		// 9-10: 2 = 3D) — both views are bound, the branch is dynamically uniform
		d5 := ubo_dword(t, t.u_fetch, 0, fc * 6 + 5)
		is3d := op1(b, .IEqual, t.t_b, op1(b, .BitFieldUExtract, t.t_u, d5, const_u32(b, 9), const_u32(b, 2)), const_u32(b, 2))
		res := local_var(b, t.t_v4)
		then_l, else_l, merge := id(b), id(b), id(b)
		op0(b, .SelectionMerge, merge, 0)
		branch_cond(b, is3d, then_l, else_l)
		label(b, then_l)
		{
			s3 := fetch_sizes(t, fc, TEX_3D)
			if f.op == .Get_Weights {
				store(b, res, weights(t, f, coords, s3, 3))
			} else {
				c := fetch_coords(t, f, coords, s3, 3)
				store(b, res, sample(t, f, TEX_3D, op1(b, .CompositeConstruct, b.t_vec[3], c[0], c[1], c[2])))
			}
		}
		branch(b, merge)
		label(b, else_l)
		{
			ss := fetch_sizes(t, fc, TEX_STACKED)
			if f.op == .Get_Weights {
				store(b, res, weights(t, f, coords, ss, 3))
			} else {
				store(b, res, sample_stacked(t, f, fetch_coords(t, f, coords, ss, 3), ss[2]))
			}
		}
		branch(b, merge)
		label(b, merge)
		r = load(b, t.t_v4, res)
		if f.op == .Get_Weights {
			store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, r)
			return
		}
	}
	// GPUSIGN_GAMMA: degammaed here, after the swizzle, as Xenia does (the host image holds the raw codes)
	r = fetch_gamma(t, fc, r)
	// the fetch constant's exp_adjust (dword 3 bits 13-18, signed) as the scale 2^exp_adjust (XeFetch.derived[n].y)
	r = arith(b, .VectorTimesScalar, t.t_v4, r, bitcast(b, t.t_f, ubo_dword(t, t.u_fetch, 1, fc * 4 + 1)))
	store_fetch_result(t, f.dst_reg, f.dst_rel, f.dst_swz, r)
}

// The Xenos piecewise-linear degamma of a GPUSIGN_GAMMA fetch (Xenia PWLGammaToLinear: codes 0-63 slope 1/4, 64-95
// slope 1/2, 96-191 slope 1, 192-255 slope 2, in 10-bit units), componentwise on a vec4.
@(private)
pwl_gamma_to_linear :: proc(t: ^T, gamma: u32) -> u32 {
	b := &t.b
	v4, bv := t.t_v4, b.t_bvec[4]
	k :: proc(b: ^Builder, v: f32) -> u32 {return const_splat(b, 4, v)}
	g := ext(b, v4, .NClamp, gamma, k(b, 0), k(b, 1)) // saturated, NaN to 0
	ge3 := op1(b, .FOrdGreaterThanEqual, bv, g, k(b, 192.0 / 255.0))
	scale32 := select(b, v4, ge3, k(b, 8.0 / 1024.0), k(b, 4.0 / 1024.0))
	offset32 := select(b, v4, ge3, k(b, -1024), k(b, -256))
	ge1 := op1(b, .FOrdGreaterThanEqual, bv, g, k(b, 64.0 / 255.0))
	scale10 := select(b, v4, ge1, k(b, 2.0 / 1024.0), k(b, 1.0 / 1024.0))
	offset10 := select(b, v4, ge1, k(b, -64), k(b, 0))
	ge2 := op1(b, .FOrdGreaterThanEqual, bv, g, k(b, 96.0 / 255.0))
	scale := select(b, v4, ge2, scale32, scale10)
	offset := select(b, v4, ge2, offset32, offset10)
	// linear = gamma * (255 * 1024) * scale + offset; linear += trunc(linear * scale); linear /= 1023
	lin := arith(b, .FAdd, v4, arith(b, .FMul, v4, arith(b, .VectorTimesScalar, v4, g, const_f32(b, 255.0 * 1024.0)), scale), offset)
	lin = arith(b, .FAdd, v4, lin, ext(b, v4, .Trunc, arith(b, .FMul, v4, lin, scale)))
	return arith(b, .VectorTimesScalar, v4, lin, const_f32(b, 1.0 / 1023.0))
}

// GPUSIGN_GAMMA per result component (Xenia SwizzleSigns): component c is degammaed when the channel the fetch
// constant's swizzle selects for it is a gamma channel. The device derives that mask from the fetch constant
// (XeFetch.derived[n].x bit c; xe_fetch_derived in xenos_spirv_abi.h), the same for every fetch of the draw.
@(private)
fetch_gamma :: proc(t: ^T, fc: u32, r: u32) -> u32 {
	b := &t.b
	uv4 := b.t_uvec[4]
	m := ubo_dword(t, t.u_fetch, 1, fc * 4 + 0)
	bits := op1(b, .BitwiseAnd, uv4, op1(b, .CompositeConstruct, uv4, m, m, m, m),
		const_composite(b, uv4, const_u32(b, 1), const_u32(b, 2), const_u32(b, 4), const_u32(b, 8)))
	mask := op1(b, .INotEqual, b.t_bvec[4], bits, const_usplat(b, 4, 0))
	return select(b, t.t_v4, mask, pwl_gamma_to_linear(t, r), r)
}

// getWeights: bilinear lerp factors fract(coord * size + offset - 0.5) (Xenia: the 0.5 is folded into the offsets)
@(private)
weights :: proc(t: ^T, f: ^Texture_Fetch, c: [3]u32, sizes: [3]u32, n: int) -> u32 {
	b := &t.b
	offs := [3]i32{f.offset_x, f.offset_y, f.offset_z}
	w := [3]u32{const_f32(b, 0), const_f32(b, 0), const_f32(b, 0)}
	for k in 0 ..< n {
		texel := f.unnormalized ? c[k] : arith(b, .FMul, t.t_f, c[k], sizes[k])
		w[k] = ext(b, t.t_f, .Fract, arith(b, .FAdd, t.t_f, texel, const_f32(b, f32(offs[k]) * 0.5 + TEXEL_EPSILON - 0.5)))
	}
	return op1(b, .CompositeConstruct, t.t_v4, w[0], w[1], w[2], const_f32(b, 0))
}

// field + 1 as float (fetch constant sizes are stored minus one)
@(private)
ux1 :: proc(t: ^T, w: u32, off, n: u32) -> u32 {
	b := &t.b
	v := op1(b, .IAdd, t.t_u, op1(b, .BitFieldUExtract, t.t_u, w, const_u32(b, off), const_u32(b, n)), const_u32(b, 1))
	return op1(b, .ConvertUToF, t.t_f, v)
}

// ---- memory export ----

@(private)
pack_fixed :: proc(t: ^T, v: u32, widths: [4]u32, numfmt: u32) -> u32 {
	// one dword from up to four fixed-point components (Direct3D conversion rules)
	b := &t.b
	out := const_u32(b, 0)
	shift: u32 = 0
	for k in u32(0) ..< 4 {
		wd := widths[k]
		if wd == 0 {continue}
		c := extract(b, t.t_f, v, k)
		c = select(b, t.t_f, op1(b, .IsNan, t.t_b, c), const_f32(b, 0), c)
		umax := f32((u64(1) << wd) - 1)
		smax := f32((u64(1) << (wd - 1)) - 1)
		// 0 unorm, 1 snorm, 2 uint, 3 sint
		un := op1(b, .ConvertFToU, t.t_u, arith(b, .FAdd, t.t_f, arith(b, .FMul, t.t_f, ext(b, t.t_f, .FClamp, c, const_f32(b, 0), const_f32(b, 1)), const_f32(b, umax)), const_f32(b, 0.5)))
		sc := arith(b, .FMul, t.t_f, ext(b, t.t_f, .FClamp, c, const_f32(b, -1), const_f32(b, 1)), const_f32(b, smax))
		sc = arith(b, .FAdd, t.t_f, sc, select(b, t.t_f, op1(b, .FOrdGreaterThanEqual, t.t_b, sc, const_f32(b, 0)), const_f32(b, 0.5), const_f32(b, -0.5)))
		sn := bitcast(b, t.t_u, op1(b, .ConvertFToS, t.t_i, sc))
		// integer: round half away from zero like the Direct3D conversion rules (Xenia), clamped to the field only to
		// keep the float -> int conversion defined
		ui := op1(b, .ConvertFToU, t.t_u, ext(b, t.t_f, .FClamp, arith(b, .FAdd, t.t_f, c, const_f32(b, 0.5)), const_f32(b, 0), const_f32(b, umax)))
		sr := arith(b, .FAdd, t.t_f, c, select(b, t.t_f, op1(b, .FOrdGreaterThanEqual, t.t_b, c, const_f32(b, 0)), const_f32(b, 0.5), const_f32(b, -0.5)))
		si := bitcast(b, t.t_u, op1(b, .ConvertFToS, t.t_i, ext(b, t.t_f, .FClamp, sr, const_f32(b, -smax - 1), const_f32(b, smax))))
		q := select(b, t.t_u, op1(b, .IEqual, t.t_b, numfmt, const_u32(b, 1)), sn, un)
		q = select(b, t.t_u, op1(b, .IEqual, t.t_b, numfmt, const_u32(b, 2)), ui, q)
		q = select(b, t.t_u, op1(b, .IEqual, t.t_b, numfmt, const_u32(b, 3)), si, q)
		mask := u32((u64(1) << wd) - 1)
		q = op1(b, .BitwiseAnd, t.t_u, q, const_u32(b, mask))
		out = op1(b, .BitwiseOr, t.t_u, out, op1(b, .ShiftLeftLogical, t.t_u, q, const_u32(b, shift)))
		shift += wd
	}
	return out
}

// at `alloc export` and at the end: store every eM# written since the previous flush (Xenia ExportToMemory)
@(private)
memexport_flush :: proc(t: ^T) {
	b := &t.b
	z := const_u32(b, 0)
	mask := load(b, t.t_u, t.v_em_mask)
	outer := if_begin(b, op1(b, .INotEqual, t.t_b, mask, z))
	ea := bitcast(b, b.t_uvec[4], load(b, t.t_v4, t.v_ea))
	fn := memexport_store_fn(t)
	for k in u32(0) ..< 5 {
		if t.p.exports & (1 << u64(EXPORT_DATA0 + k)) == 0 {continue} // eM# the program never writes
		g := if_begin(b, op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, mask, const_u32(b, 1 << k)), z))
		v := load(b, t.t_v4, access(b, t.t_pf_v4, t.v_em, const_u32(b, k)))
		call(b, b.t_void, fn, ea, v, const_u32(b, k))
		if_end(b, g)
	}
	store(b, t.v_em_mask, z)
	if_end(b, outer)
}

// void xe_memexport_store(uvec4 eA, vec4 value, uint element): one formatted element of the stream eA describes,
// written through the XeSystem memexport table (guest base dwords -> device address, size)
@(private)
memexport_store_fn :: proc(t: ^T) -> u32 {
	if t.fn_mstore != 0 {return t.fn_mstore}
	b := &t.b
	uv4 := b.t_uvec[4]
	h := helper_begin(b, b.t_void, {uv4, t.t_v4, t.t_u})
	ea, v, k := h.params[0], h.params[1], h.params[2]
	z := const_u32(b, 0)
	ex :: proc(b: ^Builder, v: u32, i: u32) -> u32 {return extract(b, b.t_uint, v, i)}
	base := op1(b, .BitwiseAnd, t.t_u, ex(b, ea, 0), const_u32(b, 0x3fffffff))
	index := op1(b, .BitwiseAnd, t.t_u, ex(b, ea, 1), const_u32(b, 0x7fffff))
	fw := ex(b, ea, 2)
	count := op1(b, .BitwiseAnd, t.t_u, ex(b, ea, 3), const_u32(b, 0x7fffff))
	// the stream constant's fixed bits: all((eA >> (30, 23, 20, 23)) == (1, 0x96, 0x4b0, 0x96))
	shifted := op1(b, .ShiftRightLogical, uv4, ea, const_composite(b, uv4, const_u32(b, 30), const_u32(b, 23), const_u32(b, 20), const_u32(b, 23)))
	valid := op1(b, .All, t.t_b, op1(b, .IEqual, b.t_bvec[4], shifted, const_composite(b, uv4, const_u32(b, 1), const_u32(b, 0x96), const_u32(b, 0x4b0), const_u32(b, 0x96))))
	// the table entry whose range holds the stream's base (one entry per exported buffer: a game exporting several
	// arrays of one buffer, as the particle systems' records, has a stream constant per array inside it)
	found := const_bool(b, false)
	size, skip := z, z
	alo, ahi := z, z
	for e_ in u32(0) ..< 4 {
		if t.abi6 {t.meta.module_meta.system_rows |= 1 << (sys_row(.Memexport) + e_)}
		e := load(b, uv4, access(b, type_ptr(b, .Uniform, uv4), t.u_sys, const_u32(b, u32(Sys.Memexport)), const_u32(b, e_)))
		d := op1(b, .ISub, t.t_u, base, ex(b, e, 0))
		m := op1(b, .LogicalAnd, t.t_b, op1(b, .ULessThan, t.t_b, d, ex(b, e, 1)), op1(b, .LogicalNot, t.t_b, found))
		size = select(b, t.t_u, m, ex(b, e, 1), size)
		skip = select(b, t.t_u, m, d, skip)
		alo = select(b, t.t_u, m, ex(b, e, 2), alo)
		ahi = select(b, t.t_u, m, ex(b, e, 3), ahi)
		found = op1(b, .LogicalOr, t.t_b, found, m)
	}
	format := op1(b, .BitFieldUExtract, t.t_u, fw, const_u32(b, 8), const_u32(b, 6))
	// SurfaceNumberFormat: bit 16 signed, bit 17 integer (0 unorm, 1 snorm, 2 uint, 3 sint; float formats ignore it)
	numfmt := op1(b, .BitFieldUExtract, t.t_u, fw, const_u32(b, 16), const_u32(b, 2))
	swap_rb := op1(b, .INotEqual, t.t_b, op1(b, .BitwiseAnd, t.t_u, fw, const_u32(b, 1 << 19)), z)
	endian := op1(b, .BitwiseAnd, t.t_u, fw, const_u32(b, 7))
	is4 := op1(b, .IEqual, t.t_b, format, const_u32(b, 38))
	is2 := op1(b, .LogicalOr, t.t_b, op1(b, .LogicalOr, t.t_b, op1(b, .IEqual, t.t_b, format, const_u32(b, 37)), op1(b, .IEqual, t.t_b, format, const_u32(b, 32))), op1(b, .IEqual, t.t_b, format, const_u32(b, 26)))
	elem := select(b, t.t_u, is4, const_u32(b, 4), select(b, t.t_u, is2, const_u32(b, 2), const_u32(b, 1)))
	ik := op1(b, .IAdd, t.t_u, index, k)
	off := op1(b, .IAdd, t.t_u, skip, op1(b, .IMul, t.t_u, ik, elem)) // dwords from the entry's base
	in_count := op1(b, .ULessThan, t.t_b, ik, count)
	in_size := op1(b, .ULessThanEqual, t.t_b, op1(b, .IAdd, t.t_u, off, elem), size)
	go := op1(b, .LogicalAnd, t.t_b, op1(b, .LogicalAnd, t.t_b, valid, found), op1(b, .LogicalAnd, t.t_b, in_count, in_size))
	// formats this export packs; any other writes nothing (Xenia writes nothing for formats it cannot pack)
	known := const_bool(b, false)
	for kf in ([]u32{6, 7, 25, 26, 31, 32, 36, 37, 38}) {known = op1(b, .LogicalOr, t.t_b, known, op1(b, .IEqual, t.t_b, format, const_u32(b, kf)))}
	go = op1(b, .LogicalAnd, t.t_b, go, known)
	g := if_begin(b, go)
	vs := op1(b, .VectorShuffle, t.t_v4, v, v, 2, 1, 0, 3)
	vv := select(b, t.t_v4, op1(b, .CompositeConstruct, b.t_bvec[4], swap_rb, swap_rb, swap_rb, swap_rb), vs, v)
	bits := bitcast(b, uv4, vv)
	h01 := ext(b, t.t_u, .PackHalf2x16, op1(b, .VectorShuffle, b.t_vec[2], vv, vv, 0, 1))
	h23 := ext(b, t.t_u, .PackHalf2x16, op1(b, .VectorShuffle, b.t_vec[2], vv, vv, 2, 3))
	p8888 := pack_fixed(t, vv, {8, 8, 8, 8}, numfmt)
	p2101010 := pack_fixed(t, vv, {10, 10, 10, 2}, numfmt)
	p1616 := pack_fixed(t, vv, {16, 16, 0, 0}, numfmt)
	p1616b := pack_fixed(t, op1(b, .VectorShuffle, t.t_v4, vv, vv, 2, 3, 0, 1), {16, 16, 0, 0}, numfmt)
	fsel :: proc(t: ^T, format: u32, f: u32, x, y: u32) -> u32 {return select(&t.b, t.t_u, op1(&t.b, .IEqual, t.t_b, format, const_u32(&t.b, f)), x, y)}
	w0 := ex(b, bits, 0)
	w0 = fsel(t, format, 6, p8888, w0)
	w0 = fsel(t, format, 7, p2101010, w0)
	w0 = fsel(t, format, 25, p1616, w0)
	w0 = fsel(t, format, 26, p1616, w0)
	w0 = fsel(t, format, 31, h01, w0)
	w0 = fsel(t, format, 32, h01, w0)
	w1 := fsel(t, format, 26, p1616b, ex(b, bits, 1))
	w1 = fsel(t, format, 32, h23, w1)
	words := op1(b, .CompositeConstruct, uv4, w0, w1, ex(b, bits, 2), ex(b, bits, 3))
	// Endian128: 0-3 per dword, 4 8in64 (dword pairs swapped), 5 8in128 (dwords reversed)
	e32 := select(b, t.t_u, op1(b, .UGreaterThanEqual, t.t_b, endian, const_u32(b, 4)), const_u32(b, 2), endian)
	words = endian_swap(t, 4, words, e32)
	pair := op1(b, .VectorShuffle, uv4, words, words, 1, 0, 3, 2)
	rev := op1(b, .VectorShuffle, uv4, words, words, 3, 2, 1, 0)
	is64 := op1(b, .IEqual, t.t_b, endian, const_u32(b, 4))
	is128 := op1(b, .IEqual, t.t_b, endian, const_u32(b, 5))
	words = select(b, uv4, op1(b, .CompositeConstruct, b.t_bvec[4], is64, is64, is64, is64), pair, words)
	words = select(b, uv4, op1(b, .CompositeConstruct, b.t_bvec[4], is128, is128, is128, is128), rev, words)
	ptr := bitcast(b, t.t_words_ptr, op1(b, .CompositeConstruct, b.t_uvec[2], alo, ahi))
	for j in u32(0) ..< 4 {
		if j > 0 {
			jf := if_begin(b, op1(b, .ULessThan, t.t_b, const_u32(b, j), elem))
			op0(b, .Store, access(b, t.t_word_ptr, ptr, const_u32(b, 0), op1(b, .IAdd, t.t_u, off, const_u32(b, j))), ex(b, words, j), 2, 4)
			if_end(b, jf)
		} else {
			op0(b, .Store, access(b, t.t_word_ptr, ptr, const_u32(b, 0), off), ex(b, words, 0), 2, 4)
		}
	}
	if_end(b, g)
	op0(b, .Return)
	b.block_open = false
	helper_end(b, &h)
	t.fn_mstore = h.id
	return h.id
}
TEXEL_EPSILON :: f32(1.5) / 1024 // Xenia shader.h kTextureCoordEpsilon

// HALO3_RES_SCALE: where in its guest pixel a host pixel's VPOS (the PsParamGen position) is taken. On the Xbox VPOS
// is the pixel's integer position, its top-left corner; at N x N host pixels per guest pixel (pixel_scale 1 / N) it
// gets a fraction, and Halo 3's pixel shaders use it two ways that need different ones:
// - as a texel or pixel index: unnormalized fetches of the screen buffers at VPOS itself (the material lighting
//   passes read the albedo and normal buffers so; Xenos point sampling takes the texel whose corner that is) and
//   floor(VPOS / block) patterns (stencil_stipple). The host pixel's top-left corner, floor(FragCoord) / N (as
//   Xenia), reads the host pixel's own texel and keeps the guest pixel boundaries.
// - as a sample position: normalized fetches at (VPOS + 0.5) / size (shadow_apply's depth and normal buffers,
//   static_per_vertex_color's albedo, active camo, displacement) and positions reconstructed from it. With the
//   corner, VPOS + 0.5 is (N - 1) / 2 host pixels right of and below the host pixel's centre, so at 3x those passes
//   shaded each pixel with its lower-right neighbour's depth, normal or albedo (a one-pixel fringe along silhouettes,
//   the real-time shadows' edges). The host pixel's centre less half a guest pixel, (floor(FragCoord) + 0.5) / N -
//   0.5, puts VPOS + 0.5 on the host pixel's centre, and VPOS where the Xbox has it relative to that centre.
// A shader gets the centre when one of its normalized texture fetches takes coordinates derived from VPOS and none
// is unnormalized (straight-line per-component data flow from the param gen register; a fetch's result is data, not
// position). Every other shader keeps the corner. At N = 1 both are the integer position.
@(private)
param_gen_centred :: proc(p: ^Program, pg: u32) -> bool {
	if pg >= 64 || p.unfed_reads & (1 << pg) == 0 {return false}
	derived: [64]u8 // per register, a bit per component derived from VPOS (x, y)
	derived[pg] = 0x3
	from :: proc(derived: ^[64]u8, o: ^Operand) -> bool {
		if !o.is_temp || o.rel != 0 || o.reg >= 64 {return false}
		for k in 0 ..< 4 {if derived[o.reg] & (1 << o.swizzle[k]) != 0 {return true}}
		return false
	}
	sample := false
	for &in_ in p.instrs {
		switch in_.kind {
		case .Alu:
			a := &in_.alu
			if a.is_export {continue}
			v, s := false, false
			for k in 0 ..< int(vec_op_operands[a.vec_op]) {v ||= from(&derived, &a.vec_src[k])}
			for k in 0 ..< a.sca_src_count {s ||= from(&derived, &a.sca_src[k])}
			if a.vec_mask != 0 && a.vec_dest < 64 {
				if v {derived[a.vec_dest] |= u8(a.vec_mask)} else {derived[a.vec_dest] &~= u8(a.vec_mask)}
			}
			if a.sca_mask != 0 && a.sca_dest < 64 {
				if s {derived[a.sca_dest] |= u8(a.sca_mask)} else {derived[a.sca_dest] &~= u8(a.sca_mask)}
			}
		case .Tfetch:
			f := &in_.tf
			if f.op == .Texture_Fetch || f.op == .Get_Weights {
				if f.unnormalized {return false}
				if f.src_reg < 64 {
					for k in u32(0) ..< 2 {if derived[f.src_reg] & (1 << ((f.src_swz >> (2 * k)) & 3)) != 0 {sample = true}}
				}
			}
			if f.dst_reg < 64 && !f.dst_rel {
				for k in u32(0) ..< 4 {if fetch_dst_component(f.dst_swz, k) != 7 {derived[f.dst_reg] &~= u8(1) << k}}
			}
		case .Vfetch:
		}
	}
	return sample
}
