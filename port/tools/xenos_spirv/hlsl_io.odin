package xenos

import "core:fmt"
import "core:strings"

// HLSL backend: stage interface, vertex fetch, texture fetch, memory export and the module text. Each procedure is
// the HLSL form of the io.odin procedure of the same name without the hl_ prefix, statement for operation.

// ---- stage interface ----

@(private)
hl_stage_inputs :: proc(h: ^H) {
	switch h.model {
	case .Vertex:
		// SV_VertexID excludes BaseVertexLocation / StartVertexLocation (gl_VertexIndex includes them): the D3D12
		// device folds them into xe_vertex.x (abi.odin D3D12)
		idx := hl_tmp(h, "int", "int(xe_vertex_id) + int(xe_vertex.x)")
		hl_line(h, "%s = float4(float(%s), 0.0, 0.0, 0.0);", hl_reg(h, 0, false), idx)
	case .Tess_Eval:
		// SV_DomainLocation.xyz -> r0.zyx as gl_TessCoord (Xenia, both backends)
		hl_line(h, "%s = float4(xe_domain.z, xe_domain.y, xe_domain.x, 1.0);", hl_reg(h, 0, false))
		patch := hl_tmp(h, "float", "float(int(xe_primitive_id) + int(xe_vertex.x))")
		if h.n_regs > 1 {hl_line(h, "%s = float4(%s, 0.0, 0.0, 0.0);", hl_reg(h, 1, false), patch)}
	case .Compute:
		x := hl_tmp(h, "uint", "xe_global_id.x")
		hl_line(h, "if (%s >= xe_vertex.z) return;", x)
		idx := hl_tmp(h, "uint", fmt.tprintf("%s + xe_vertex.y", x))
		hl_line(h, "%s = float4(float(%s), 0.0, 0.0, 0.0);", hl_reg(h, 0, false), idx)
	case .Fragment:
		used: u32
		for v in h.c.interpolators {
			loc := interface_location(v.usage, v.index)
			if used & (1 << loc) != 0 || v.reg >= h.n_regs {continue}
			used |= 1 << loc
			if loc >= HLSL_INTERFACE_SLOTS {
				h.err = fmt.tprintf("D3D12 interface: location %d (%v%d) has no slot", loc, v.usage, v.index)
				return
			}
			hl_line(h, "%s = xe_in.l%d; // %v%d", hl_reg(h, v.reg, false), loc, v.usage, v.index)
		}
		h.meta.ps_inputs = used
		// param gen (the Xenos PsParamGen: VPOS / VFACE / SPRITETEXCOORD) into the register after the interpolators
		pg := u32(len(h.c.interpolators))
		if pg < h.n_regs {
			h.meta.param_gen = int(pg)
			pos := hl_tmp(h, "float4", "xe_in.position")
			fl := hl_tmp(h, "float2", fmt.tprintf("floor(%s.xy)", pos))
			g := hl_tmp(h, "float2", fmt.tprintf("(%s * xe_pixel_scale.xy) + xe_pixel_scale.zw", fl))
			if param_gen_centred(h.p, pg) {
				// HALO3_RES_SCALE: the host pixel's centre less half a guest pixel (param_gen_centred)
				hh := hl_tmp(h, "float2", "(xe_pixel_scale.xy * 0.5) - ((float2)0.5)")
				g = hl_tmp(h, "float2", fmt.tprintf("max(%s + %s, (float2)0.0)", g, hh))
			}
			g = hl_tmp(h, "float2", fmt.tprintf("abs(%s)", g))
			point := hl_tmp(h, "bool", fmt.tprintf("(xe_alpha_test.z & %s) != 0u", hl_ulit(SYS_FLAG_POINT_PRIMITIVE)))
			// Vulkan rasterizes points front-facing; the D3D12 point sprite quads (helper point GS) are too, whatever
			// FrontCounterClockwise says
			back := hl_tmp(h, "bool", fmt.tprintf("!xe_front_face && !%s", point))
			gx := hl_tmp(h, "uint", fmt.tprintf("asuint(%s.x)", g))
			gx = hl_tmp(h, "uint", fmt.tprintf("%s ? (%s | 0x80000000u) : %s", back, gx, gx))
			gy := hl_tmp(h, "uint", fmt.tprintf("asuint(%s.y)", g))
			gy = hl_tmp(h, "uint", fmt.tprintf("%s ? (%s | 0x80000000u) : %s", point, gy, gy))
			// the sprite coordinates come from the point GS (zero for other primitives, selected away)
			pc := hl_tmp(h, "float2", "clamp(xe_in.point_coord, (float2)0.0, (float2)1.0)")
			ps := hl_tmp(h, "float", fmt.tprintf("%s ? %s.x : 0.0", point, pc))
			pt := hl_tmp(h, "float", fmt.tprintf("%s ? %s.y : 0.0", point, pc))
			hl_line(h, "%s = float4(asfloat(%s), asfloat(%s), %s, %s);", hl_reg(h, pg, false), gx, gy, ps, pt)
		}
	}
}

@(private)
hl_stage_outputs :: proc(h: ^H) {
	switch h.model {
	case .Vertex, .Tess_Eval:
		pos := hl_tmp(h, "float4", "oPos")
		w := hl_tmp(h, "float", fmt.tprintf("%s.w", pos))
		hl_line(h, "precise float4 xe_position = (%s * xe_ndc_scale) + (xe_ndc_offset * %s);", pos, w)
		hl_line(h, "xe_position.w = %s;", w)
		hl_line(h, "xe_out.position = xe_position;")
		// user clip planes (as the SPIR-V ClipDistance): a disabled plane is zero, distance 0, never clipped
		hl_line(h, "xe_out.clip_distance_0 = float4(dot(xe_position, xe_clip_planes[0]), dot(xe_position, xe_clip_planes[1]), dot(xe_position, xe_clip_planes[2]), dot(xe_position, xe_clip_planes[3]));")
		hl_line(h, "xe_out.clip_distance_1 = float2(dot(xe_position, xe_clip_planes[4]), dot(xe_position, xe_clip_planes[5]));")
		if h.meta.point_size {
			// the host point diameter in host pixels (as PointSize), for the point sprite GS
			s := hl_tmp(h, "float", "clamp(oPts.x, xe_point.x, xe_point.y)")
			hl_line(h, "xe_out.point_size = %s / xe_pixel_scale.x;", s)
		}
		used: u32
		for v in h.c.interpolators {
			loc := interface_location(v.usage, v.index)
			if used & (1 << loc) != 0 || v.reg >= 16 {continue}
			used |= 1 << loc
			if loc >= HLSL_INTERFACE_SLOTS {
				h.err = fmt.tprintf("D3D12 interface: location %d (%v%d) has no slot", loc, v.usage, v.index)
				return
			}
			hl_line(h, "xe_out.l%d = o%d; // %v%d", loc, v.reg, v.usage, v.index)
		}
		h.meta.vs_outputs = used
		hl_line(h, "return xe_out;")
	case .Fragment:
		if h.meta.color_outputs & 1 != 0 {
			// alpha test on oC0.a: Xenos CompareFunction bits (1 less, 2 equal, 4 greater), 7 = always
			fn := hl_tmp(h, "uint", "xe_alpha_test.x")
			ref := hl_tmp(h, "float", "asfloat(xe_alpha_test.y)")
			a := hl_tmp(h, "float", "oC0.w")
			lt := hl_tmp(h, "bool", fmt.tprintf("(%s < %s) && ((%s & 1u) != 0u)", a, ref, fn))
			eq := hl_tmp(h, "bool", fmt.tprintf("(%s == %s) && ((%s & 2u) != 0u)", a, ref, fn))
			gt := hl_tmp(h, "bool", fmt.tprintf("(%s > %s) && ((%s & 4u) != 0u)", a, ref, fn))
			pass := hl_tmp(h, "bool", fmt.tprintf("((%s || %s) || %s) || (%s == 7u)", lt, eq, gt, fn))
			hl_line(h, "if (!%s) discard;", pass)
		}
		if h.meta.color_outputs != 0 || h.meta.depth_output {hl_line(h, "XePSOutput xe_out;")}
		for i in u32(0) ..< 4 {
			if h.meta.color_outputs & (1 << i) == 0 {continue}
			hl_line(h, "xe_out.oC%d = oC%d * xe_color_exp_bias.%s;", i, i, XYZW_STR[i])
		}
		// clamped as the SPIR-V FragDepth
		if h.meta.depth_output {hl_line(h, "xe_out.depth = saturate(oDepth.x);")}
		if h.meta.color_outputs != 0 || h.meta.depth_output {hl_line(h, "return xe_out;")}
	case .Compute:
	}
}

// ---- vertex fetch ----

@(private)
hl_emit_vfetch :: proc(h: ^H, in_: ^Instr) {
	if !(h.stage == .Vertex && h.p.vfetch_count > 0) {return}
	f := &in_.vf
	full := &in_.vf_full
	// the vertex element the D3D runtime patches into this vfetch
	k := -1
	for v, i in h.c.vertex_elements {if v.address == in_.address {k = i}}
	if k < 0 {k = 31} // not in the patch list: element 31 (left unbound = zeros unless the device fills it)
	h.meta.vertex_elements = max(h.meta.vertex_elements, k + 1)
	el := hl_tmp(h, "uint4", fmt.tprintf("xe_element[%d]", k))
	stream := hl_tmp(h, "uint", fmt.tprintf("%s.x & 15u", el))
	st := hl_tmp(h, "uint4", fmt.tprintf("xe_stream[%s]", stream))
	offset := hl_tmp(h, "uint", fmt.tprintf("%s.y", el))
	flags := hl_tmp(h, "uint", fmt.tprintf("%s.w", el))
	size := hl_tmp(h, "uint", fmt.tprintf("%s.z", st))
	sw := hl_tmp(h, "uint", fmt.tprintf("%s.w", st))
	stride := hl_tmp(h, "uint", fmt.tprintf("%s & 0xFFFFu", sw))
	endian := hl_tmp(h, "uint", fmt.tprintf("(%s >> 16u) & 3u", sw))
	// index from the source register
	src := hl_tmp(h, "float4", hl_reg(h, full.src_reg, full.src_rel))
	fi := hl_tmp(h, "float", hl_comp(src, full.src_swz))
	if full.index_rounded {fi = hl_tmp(h, "float", fmt.tprintf("%s + 0.5", fi))}
	idx := hl_tmp(h, "int", fmt.tprintf("int(floor(%s))", fi))
	byte_addr := hl_tmp(h, "int", fmt.tprintf("(%s * int(%s)) + int(%s)", idx, stride, offset))
	nonneg := hl_tmp(h, "bool", fmt.tprintf("%s >= 0", idx))
	ba := hl_tmp(h, "uint", fmt.tprintf("uint(%s)", byte_addr))
	// stream.x: the byte offset of vertex 0 in the stream's raw view (D3D12; the Vulkan address low half)
	words: [4]string
	for j in u32(0) ..< 4 {
		end_ := hl_tmp(h, "uint", fmt.tprintf("%s + %du", ba, 4 * (j + 1)))
		ok := hl_tmp(h, "bool", fmt.tprintf("%s && (%s <= %s)", nonneg, end_, size))
		wi := hl_tmp(h, "uint", fmt.tprintf("%s ? ((%s >> 2u) + %du) : 0u", ok, ba, j))
		w := hl_tmp(h, "uint", fmt.tprintf("xe_vertex_streams[%s].Load(%s.x + (%s << 2u))", stream, st, wi))
		words[j] = hl_tmp(h, "uint", fmt.tprintf("%s ? %s : 0u", ok, w))
	}
	wv := hl_tmp(h, "uint4", fmt.tprintf("uint4(%s, %s, %s, %s)", words[0], words[1], words[2], words[3]))
	wv = hl_tmp(h, "uint4", fmt.tprintf("xe_endian_swap(%s, %s)", wv, endian))
	hl_vfetch_decode_fn(h)
	val := hl_tmp(h, "float4", fmt.tprintf("xe_vfetch_decode(%s, xe_unpack[%d], xe_denominator[%d])", wv, k, k))
	val = hl_vfetch_declaration_swizzle(h, val, flags)
	hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, val)
}

// The declaration type's swizzle (as vfetch_declaration_swizzle)
@(private)
hl_vfetch_declaration_swizzle :: proc(h: ^H, val: string, flags: string) -> string {
	has := hl_tmp(h, "bool", fmt.tprintf("(%s & %s) != 0u", flags, hl_ulit(VF_HAS_SWIZZLE)))
	comps: [4]string
	for k in u32(0) ..< 4 {
		s := hl_tmp(h, "uint", fmt.tprintf("(%s >> %du) & 7u", flags, VF_SWIZZLE_SHIFT + 3 * k))
		// 0-3: that component, 5: one, 4 / 6 / 7: zero
		picked := hl_tmp(h, "float", fmt.tprintf("%s[%s & 3u]", val, s))
		constant := hl_tmp(h, "float", fmt.tprintf("(%s == 5u) ? 1.0 : 0.0", s))
		c := hl_tmp(h, "float", fmt.tprintf("(%s < 4u) ? %s : %s", s, picked, constant))
		comps[k] = hl_tmp(h, "float", fmt.tprintf("%s ? %s : %s", has, c, hl_comp(val, k)))
	}
	return hl_tmp(h, "float4", fmt.tprintf("float4(%s, %s, %s, %s)", comps[0], comps[1], comps[2], comps[3]))
}

// float4 xe_vfetch_decode(uint4 words, uint4 unpack, float4 denominator): as vfetch_decode_fn. (The word index of a
// component is masked to the four words: the device never sets bit 7 of a layout byte, where SPIR-V's dynamic
// extract would be undefined.)
@(private)
hl_vfetch_decode_fn :: proc(h: ^H) {
	if h.fn_vdecode {return}
	h.fn_vdecode = true
	s := &h.fns
	strings.write_string(s, "float4 xe_vfetch_decode(uint4 wv, uint4 un, float4 den) {\n")
	strings.write_string(s, "\tuint layout = un.x;\n\tuint widths = un.y;\n\tuint mode_flags = un.z;\n\tuint4 raw;\n\tint4 sx;\n\tuint4 width;\n")
	for c in u32(0) ..< 4 {
		fmt.sbprintf(s, "\t{{\n\t\tuint at = (layout >> %du) & 0xFFu;\n\t\tuint bit = at & 31u;\n\t\tuint word = wv[(at >> 5u) & 3u];\n", 8 * c)
		fmt.sbprintf(s, "\t\twidth.%s = (widths >> %du) & 0xFFu;\n", XYZW_STR[c], 8 * c)
		fmt.sbprintf(s, "\t\traw.%s = xe_ubfe(word, bit, width.%s);\n", XYZW_STR[c], XYZW_STR[c])
		fmt.sbprintf(s, "\t\tsx.%s = xe_sbfe(word, bit, width.%s);\n\t}}\n", XYZW_STR[c], XYZW_STR[c])
	}
	fmt.sbprintf(s, "\tbool is_signed = (mode_flags & %s) != 0u;\n", hl_ulit(VF_SIGNED))
	fmt.sbprintf(s, "\tbool integer = (mode_flags & %s) != 0u;\n", hl_ulit(VF_INTEGER))
	fmt.sbprintf(s, "\tbool no_zero = (mode_flags & %s) != 0u;\n", hl_ulit(VF_NO_ZERO))
	fmt.sbprintf(s, "\tuint mode = (mode_flags >> %du) & 3u;\n", u32(VF_MODE_SHIFT))
	// fixed point
	strings.write_string(s, "\tfloat4 x = is_signed ? float4(sx) : float4(raw);\n")
	strings.write_string(s, "\tfloat4 xn = no_zero ? ((x * 2.0) + 1.0) : x;\n")
	strings.write_string(s, "\tfloat4 q = xn / den;\n")
	strings.write_string(s, "\tbool clamped = is_signed && !no_zero;\n")
	strings.write_string(s, "\tq = clamped ? max(q, (float4)-1.0) : q;\n")
	strings.write_string(s, "\tfloat4 fixed_point = integer ? x : q;\n")
	strings.write_string(s, "\tfixed_point = select(width != 0u, fixed_point, (float4)0.0);\n")
	// floats
	strings.write_string(s, "\tfloat4 halves = float4(f16tof32(raw.x), f16tof32(raw.y), f16tof32(raw.z), f16tof32(raw.w));\n")
	fmt.sbprintf(s, "\tfloat4 val = (mode == %du) ? halves : ((mode == %du) ? asfloat(raw) : fixed_point);\n", u32(VF_MODE_HALF), u32(VF_MODE_FLOAT))
	strings.write_string(s, "\treturn val * asfloat(un.w);\n}\n\n")
}

// ---- texture fetch ----

@(private)
hl_texture_var :: proc(h: ^H, fetch: u32, kind: u32) -> string {
	dn := [4]string{"2d", "3d", "cube", "stacked"}
	name := fmt.tprintf("xe_tf%d_%s", fetch, dn[kind])
	key := fetch | kind << 8
	if !(key in h.tex) {
		h.tex[key] = true
		types := [4]string{"Texture2D<float4>", "Texture3D<float4>", "TextureCube<float4>", "Texture2DArray<float4>"}
		bases := [4]u32{BINDING_TEX_2D, BINDING_TEX_3D, BINDING_TEX_CUBE, BINDING_TEX_STACKED}
		fmt.sbprintf(&h.decls, "%s %s : register(t%d, space%d);\n", types[kind], name, bases[kind] + fetch, SET_TEXTURES)
	}
	return name
}

@(private)
hl_sampler_var :: proc(h: ^H, fetch: u32, variant: u32) -> string {
	name := fmt.tprintf("xe_tf%d_sampler%d", fetch, variant)
	key := fetch | variant << 8
	if !(key in h.smp) {
		h.smp[key] = true
		fmt.sbprintf(&h.decls, "SamplerState %s : register(s%d, space%d);\n", name, BINDING_SAMPLER + SAMPLER_VARIANT_STRIDE * variant + fetch, SET_TEXTURES)
	}
	return name
}

// a dword of the fetch constants (XeFetch.f, dword dw)
@(private)
hl_fetch_dword :: proc(h: ^H, dw: u32) -> string {return hl_tmp(h, "uint", fmt.tprintf("xe_f[%d].%s", dw >> 2, XYZW_STR[dw & 3]))}

@(private)
hl_sample :: proc(h: ^H, f: ^Texture_Fetch, kind: u32, coord: string) -> string {
	fc := f.const_index
	variant := sampler_variant(f)
	found := false
	for u in h.meta.textures {if u.fetch == fc && u.dim == kind && u.sampler == variant {found = true}}
	if !found {append(&h.meta.textures, Texture_Use{fetch = fc, dim = kind, sampler = variant})}
	iv := hl_texture_var(h, fc, kind)
	sv := hl_sampler_var(h, fc, variant)
	frag := h.model == .Fragment
	// LOD: instruction LODBias (+ register LOD); the fetch constant's LOD bias is the host sampler's MipLODBias
	bias := f32(f.lod_bias) / 16
	lod := hl_flit(bias)
	if f.use_reg_lod {lod = hl_tmp(h, "float", fmt.tprintf("xe_lod + %s", lod))}
	if f.use_comp_lod && f.use_reg_gradients {
		// register gradients (setGradientH/V), scaled by 2^(bias + register LOD) as Xenia does
		gn := kind == TEX_2D || kind == TEX_STACKED ? 2 : 3
		scale := hl_tmp(h, "float", fmt.tprintf("exp2(%s)", lod))
		gh := hl_tmp(h, "float3", fmt.tprintf("xe_grad_h * %s", scale))
		gv := hl_tmp(h, "float3", fmt.tprintf("xe_grad_v * %s", scale))
		if gn == 2 {
			gh = fmt.tprintf("%s.xy", gh)
			gv = fmt.tprintf("%s.xy", gv)
		}
		return hl_tmp(h, "float4", fmt.tprintf("%s.SampleGrad(%s, %s, %s, %s)", iv, sv, coord, gh, gv))
	}
	if frag && f.use_comp_lod {
		if f.use_reg_lod || bias != 0 {return hl_tmp(h, "float4", fmt.tprintf("%s.SampleBias(%s, %s, %s)", iv, sv, coord, lod))}
		return hl_tmp(h, "float4", fmt.tprintf("%s.Sample(%s, %s)", iv, sv, coord))
	}
	if frag && kind == TEX_CUBE {
		// Halo 3 PC MCC (environment_mapping.fx): bias explicit-LOD cubemap fetches by screen-space
		// derivative of the normalized direction to eliminate 1px bevel/silhouette rim aliasing.
		dir := hl_tmp(h, "float3", fmt.tprintf("normalize(%s)", coord))
		gx := hl_tmp(h, "float", fmt.tprintf("length(ddx(%s))", dir))
		gy := hl_tmp(h, "float", fmt.tprintf("length(ddy(%s))", dir))
		base_lod := hl_tmp(h, "float", fmt.tprintf("(6.0 * sqrt(max(%s, %s))) - 0.6", gx, gy))
		lod = hl_tmp(h, "float", fmt.tprintf("max(%s, %s)", lod, base_lod))
	}
	return hl_tmp(h, "float4", fmt.tprintf("%s.SampleLevel(%s, %s, %s)", iv, sv, coord, lod))
}

// A tfetch3D of stacked 2D layers (as sample_stacked)
@(private)
hl_sample_stacked :: proc(h: ^H, f: ^Texture_Fetch, c: [3]string, depth: string) -> string {
	fc := f.const_index
	vol_linear :: proc(h: ^H, fc: u32, filter: u32, bit: u32) -> (string, bool) {
		switch filter {
		case 1: return "true", false
		case 3:
			d4 := hl_fetch_dword(h, fc * 6 + 4)
			return hl_tmp(h, "bool", fmt.tprintf("(%s & %s) != 0u", d4, hl_ulit(1 << bit))), false
		}
		return "false", true
	}
	gradients := h.model == .Fragment && f.use_comp_lod
	mag, mag_point := vol_linear(h, fc, f.vol_mag_filter, 0)
	linear := mag
	static_point := mag_point
	if gradients && (f.vol_min_filter != f.vol_mag_filter || f.vol_mag_filter == 3) { // the filters may differ
		min_, min_point := vol_linear(h, fc, f.vol_min_filter, 1)
		// layers per pixel: the larger screen-space change of the layer coordinate
		gz_h, gz_v: string
		if f.use_reg_gradients {
			gz_h = hl_tmp(h, "float", "xe_grad_h.z")
			gz_v = hl_tmp(h, "float", "xe_grad_v.z")
		} else {
			gz_h = hl_tmp(h, "float", fmt.tprintf("ddx(%s)", c[2]))
			gz_v = hl_tmp(h, "float", fmt.tprintf("ddy(%s)", c[2]))
		}
		g := hl_tmp(h, "float", fmt.tprintf("max(abs(%s), abs(%s))", gz_h, gz_v))
		if f.use_reg_gradients && f.unnormalized {g = hl_tmp(h, "float", fmt.tprintf("%s / %s", g, depth))}
		minifying := hl_tmp(h, "bool", fmt.tprintf("(%s * %s) > 1.0", g, depth)) // NaN magnifies
		linear = hl_tmp(h, "bool", fmt.tprintf("%s ? %s : %s", minifying, min_, mag))
		static_point = mag_point && min_point
	}
	z := hl_tmp(h, "float", fmt.tprintf("%s * %s", c[2], depth))
	if !static_point {z = hl_tmp(h, "float", fmt.tprintf("%s ? (%s - 0.5) : %s", linear, z, z))}
	l0 := hl_tmp(h, "float", fmt.tprintf("floor(%s)", z))
	d0 := hl_fetch_dword(h, fc * 6 + 0)
	wrap := hl_tmp(h, "bool", fmt.tprintf("((%s >> 16u) & 7u) == 0u", d0))
	layer :: proc(h: ^H, l, depth, wrap: string) -> string {
		// wrap: l mod depth (floored); otherwise as is, the sampler clamping it to 0 .. depth - 1
		m := hl_tmp(h, "float", fmt.tprintf("%s - (%s * floor(%s / %s))", l, depth, l, depth))
		return hl_tmp(h, "float", fmt.tprintf("%s ? %s : %s", wrap, m, l))
	}
	s0 := hl_sample(h, f, TEX_STACKED, fmt.tprintf("float3(%s, %s, %s)", c[0], c[1], layer(h, l0, depth, wrap)))
	if static_point {return s0}
	l1 := hl_tmp(h, "float", fmt.tprintf("%s + 1.0", l0))
	s1 := hl_sample(h, f, TEX_STACKED, fmt.tprintf("float3(%s, %s, %s)", c[0], c[1], layer(h, l1, depth, wrap)))
	frac_ := hl_tmp(h, "float", fmt.tprintf("%s ? (%s - %s) : 0.0", linear, z, l0))
	return hl_tmp(h, "float4", fmt.tprintf("%s + ((%s - %s) * %s)", s0, s1, s0, frac_))
}

@(private)
hl_emit_tfetch :: proc(h: ^H, in_: ^Instr) {
	f := &in_.tf
	src := hl_tmp(h, "float4", hl_reg(h, f.src_reg, f.src_rel))
	cx := hl_tmp(h, "float", hl_comp(src, f.src_swz & 3))
	cy := hl_tmp(h, "float", hl_comp(src, (f.src_swz >> 2) & 3))
	cz := hl_tmp(h, "float", hl_comp(src, (f.src_swz >> 4) & 3))
	frag := h.model == .Fragment
	zero4 := hl_splat(4, 0)
	#partial switch f.op {
	case .Set_Tex_Lod:
		hl_line(h, "xe_lod = %s;", cx)
		return
	case .Set_Grad_H, .Set_Grad_V:
		hl_line(h, "%s = float3(%s, %s, %s);", f.op == .Set_Grad_H ? "xe_grad_h" : "xe_grad_v", cx, cy, cz)
		return
	case .Get_Gradients:
		r := zero4
		if frag {r = hl_tmp(h, "float4", fmt.tprintf("float4(ddx(%s), ddy(%s), ddx(%s), ddy(%s))", cx, cx, cy, cy))}
		hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, r)
		return
	case .Get_Border_Frac, .Get_Comp_Tex_Lod:
		hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, zero4)
		return
	case .Texture_Fetch, .Get_Weights:
	case:
		hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, zero4)
		return
	}
	dim: u32 = 0
	switch f.dimension {
	case 2: dim = 1
	case 3: dim = 2
	}
	fc := f.const_index
	coords := [3]string{cx, cy, cz}
	r: string
	switch dim {
	case 2:
		if f.op == .Get_Weights {
			hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, zero4) // cube lerp factors: unused by Halo 3
			return
		}
		// cube: SC / TC in 1..2 and the face index (cube ALU instruction output) back to a direction (Xenia)
		sc := hl_tmp(h, "float", fmt.tprintf("(%s * 2.0) - 3.0", cx))
		tc := hl_tmp(h, "float", fmt.tprintf("(%s * 2.0) - 3.0", cy))
		face := hl_tmp(h, "uint", fmt.tprintf("uint(clamp(floor(%s), 0.0, 5.0))", cz))
		axis := hl_tmp(h, "uint", fmt.tprintf("%s >> 1u", face))
		neg := hl_tmp(h, "bool", fmt.tprintf("(%s & 1u) != 0u", face))
		sign := hl_tmp(h, "float", fmt.tprintf("%s ? -1.0 : 1.0", neg))
		nsc := hl_tmp(h, "float", fmt.tprintf("-%s", sc))
		ntc := hl_tmp(h, "float", fmt.tprintf("-%s", tc))
		is_x := hl_tmp(h, "bool", fmt.tprintf("%s == 0u", axis))
		is_y := hl_tmp(h, "bool", fmt.tprintf("%s == 1u", axis))
		sel :: proc(h: ^H, c, x, y: string) -> string {return hl_tmp(h, "float", fmt.tprintf("%s ? %s : %s", c, x, y))}
		// x major: (sign, -tc, neg ? sc : -sc); y major: (sc, sign, neg ? -tc : tc); z major: (neg ? -sc : sc, -tc, sign)
		x := sel(h, is_x, sign, sel(h, is_y, sc, sel(h, neg, nsc, sc)))
		y := sel(h, is_x, ntc, sel(h, is_y, sign, ntc))
		zz := sel(h, is_x, sel(h, neg, sc, nsc), sel(h, is_y, sel(h, neg, ntc, tc), sign))
		r = hl_sample(h, f, TEX_CUBE, fmt.tprintf("float3(%s, %s, %s)", x, y, zz))
	case 0:
		sizes := hl_fetch_sizes(h, fc, TEX_2D)
		if f.op == .Get_Weights {
			hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, hl_weights(h, f, coords, sizes, 2))
			return
		}
		c := hl_fetch_coords(h, f, coords, sizes, 2)
		r = hl_sample(h, f, TEX_2D, fmt.tprintf("float2(%s, %s)", c[0], c[1]))
	case 1:
		// tfetch3D: a real 3D texture or stacked 2D layers, by the fetch constant's data dimension (dword 5 bits
		// 9-10: 2 = 3D) — both views are bound, the branch is dynamically uniform
		d5 := hl_fetch_dword(h, fc * 6 + 5)
		is3d := hl_tmp(h, "bool", fmt.tprintf("((%s >> 9u) & 3u) == 2u", d5))
		res := hl_tmp(h, "float4", "0.0")
		hl_open(h, "[branch] if (%s) {{", is3d)
		{
			s3 := hl_fetch_sizes(h, fc, TEX_3D)
			if f.op == .Get_Weights {
				hl_line(h, "%s = %s;", res, hl_weights(h, f, coords, s3, 3))
			} else {
				c := hl_fetch_coords(h, f, coords, s3, 3)
				hl_line(h, "%s = %s;", res, hl_sample(h, f, TEX_3D, fmt.tprintf("float3(%s, %s, %s)", c[0], c[1], c[2])))
			}
		}
		h.indent -= 1
		hl_line(h, "}} else {{")
		h.indent += 1
		{
			ss := hl_fetch_sizes(h, fc, TEX_STACKED)
			if f.op == .Get_Weights {
				hl_line(h, "%s = %s;", res, hl_weights(h, f, coords, ss, 3))
			} else {
				hl_line(h, "%s = %s;", res, hl_sample_stacked(h, f, hl_fetch_coords(h, f, coords, ss, 3), ss[2]))
			}
		}
		hl_close(h)
		r = hl_tmp(h, "float4", res)
		if f.op == .Get_Weights {
			hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, r)
			return
		}
	}
	// GPUSIGN_GAMMA: degammaed here, after the swizzle, as Xenia does (the host image holds the raw codes)
	r = hl_fetch_gamma(h, fc, r)
	// the fetch constant's exp_adjust as the scale 2^exp_adjust (XeFetch.derived[n].y)
	r = hl_tmp(h, "float4", fmt.tprintf("%s * asfloat(xe_fetch_derived[%d].y)", r, fc))
	hl_store_fetch_result(h, f.dst_reg, f.dst_rel, f.dst_swz, r)
}

// float4 xe_pwl_gamma_to_linear(float4 gamma): as pwl_gamma_to_linear
@(private)
hl_pwl_gamma_fn :: proc(h: ^H) {
	if h.fn_gamma {return}
	h.fn_gamma = true
	s := &h.fns
	strings.write_string(s, "float4 xe_pwl_gamma_to_linear(float4 gamma) {\n")
	strings.write_string(s, "\tfloat4 g = saturate(gamma); // NaN to 0\n")
	fmt.sbprintf(s, "\tbool4 ge3 = g >= %s;\n", hl_flit(192.0 / 255.0))
	fmt.sbprintf(s, "\tfloat4 scale32 = select(ge3, (float4)%s, (float4)%s);\n", hl_flit(8.0 / 1024.0), hl_flit(4.0 / 1024.0))
	fmt.sbprintf(s, "\tfloat4 offset32 = select(ge3, (float4)%s, (float4)%s);\n", hl_flit(-1024), hl_flit(-256))
	fmt.sbprintf(s, "\tbool4 ge1 = g >= %s;\n", hl_flit(64.0 / 255.0))
	fmt.sbprintf(s, "\tfloat4 scale10 = select(ge1, (float4)%s, (float4)%s);\n", hl_flit(2.0 / 1024.0), hl_flit(1.0 / 1024.0))
	fmt.sbprintf(s, "\tfloat4 offset10 = select(ge1, (float4)%s, (float4)%s);\n", hl_flit(-64), hl_flit(0))
	fmt.sbprintf(s, "\tbool4 ge2 = g >= %s;\n", hl_flit(96.0 / 255.0))
	strings.write_string(s, "\tfloat4 scale = select(ge2, scale32, scale10);\n")
	strings.write_string(s, "\tfloat4 offset = select(ge2, offset32, offset10);\n")
	// linear = gamma * (255 * 1024) * scale + offset; linear += trunc(linear * scale); linear /= 1023
	fmt.sbprintf(s, "\tfloat4 lin = ((g * %s) * scale) + offset;\n", hl_flit(255.0 * 1024.0))
	strings.write_string(s, "\tlin = lin + trunc(lin * scale);\n")
	fmt.sbprintf(s, "\treturn lin * %s;\n}}\n\n", hl_flit(1.0 / 1023.0))
}

// GPUSIGN_GAMMA per result component (as fetch_gamma)
@(private)
hl_fetch_gamma :: proc(h: ^H, fc: u32, r: string) -> string {
	hl_pwl_gamma_fn(h)
	m := hl_tmp(h, "uint", fmt.tprintf("xe_fetch_derived[%d].x", fc))
	mask := hl_tmp(h, "bool4", fmt.tprintf("(uint4(%s, %s, %s, %s) & uint4(1u, 2u, 4u, 8u)) != 0u", m, m, m, m))
	return hl_tmp(h, "float4", fmt.tprintf("select(%s, xe_pwl_gamma_to_linear(%s), %s)", mask, r, r))
}

// getWeights: bilinear lerp factors fract(coord * size + offset - 0.5) (as weights)
@(private)
hl_weights :: proc(h: ^H, f: ^Texture_Fetch, c: [3]string, sizes: [3]string, n: int) -> string {
	offs := [3]i32{f.offset_x, f.offset_y, f.offset_z}
	w := [3]string{"0.0", "0.0", "0.0"}
	for k in 0 ..< n {
		texel := f.unnormalized ? c[k] : hl_tmp(h, "float", fmt.tprintf("%s * %s", c[k], sizes[k]))
		w[k] = hl_tmp(h, "float", fmt.tprintf("frac(%s + %s)", texel, hl_flit(f32(offs[k]) * 0.5 + TEXEL_EPSILON - 0.5)))
	}
	return hl_tmp(h, "float4", fmt.tprintf("float4(%s, %s, %s, 0.0)", w[0], w[1], w[2]))
}

// guest texture size (texels) for an image kind from fetch constant dword 2 (as fetch_sizes)
@(private)
hl_fetch_sizes :: proc(h: ^H, fc: u32, kind: u32) -> (s: [3]string) {
	d2 := hl_fetch_dword(h, fc * 6 + 2)
	ux1 :: proc(h: ^H, w: string, off, n: u32) -> string {
		return hl_tmp(h, "float", fmt.tprintf("float(((%s >> %du) & 0x%Xu) + 1u)", w, off, (u32(1) << n) - 1))
	}
	switch kind {
	case TEX_3D: s = {ux1(h, d2, 0, 11), ux1(h, d2, 11, 11), ux1(h, d2, 22, 10)}
	case TEX_STACKED: s = {ux1(h, d2, 0, 13), ux1(h, d2, 13, 13), ux1(h, d2, 26, 6)}
	case: s = {ux1(h, d2, 0, 13), ux1(h, d2, 13, 13), "1.0"}
	}
	return
}

// normalized coordinates of a 2D / 3D / stacked fetch (as fetch_coords)
@(private)
hl_fetch_coords :: proc(h: ^H, f: ^Texture_Fetch, c: [3]string, sizes: [3]string, n: int) -> (out: [3]string) {
	out = c
	offs := [3]i32{f.offset_x, f.offset_y, f.offset_z}
	for k in 0 ..< n {
		if f.unnormalized {out[k] = hl_tmp(h, "float", fmt.tprintf("%s / %s", out[k], sizes[k]))}
		d := hl_tmp(h, "float", fmt.tprintf("%s / %s", hl_flit(f32(offs[k]) * 0.5 + TEXEL_EPSILON), sizes[k]))
		out[k] = hl_tmp(h, "float", fmt.tprintf("%s + %s", out[k], d))
	}
	return
}

// ---- memory export ----

// at `alloc export` and at the end: store every eM# written since the previous flush (as memexport_flush)
@(private)
hl_memexport_flush :: proc(h: ^H) {
	hl_memexport_store_fn(h)
	mask := hl_tmp(h, "uint", "xe_em_mask")
	hl_open(h, "if (%s != 0u) {{", mask)
	ea := hl_tmp(h, "uint4", "asuint(eA)")
	for k in u32(0) ..< 5 {
		if h.p.exports & (1 << u64(EXPORT_DATA0 + k)) == 0 {continue} // eM# the program never writes
		hl_line(h, "if ((%s & %s) != 0u) xe_memexport_store(%s, eM%d, %du);", mask, hl_ulit(1 << k), ea, k, k)
	}
	hl_line(h, "xe_em_mask = 0u;")
	hl_close(h)
}

// void xe_memexport_store(uint4 eA, float4 value, uint element): as memexport_store_fn, writing through the raw UAV
// of the matched XeSystem.memexport entry (entry e -> u{e} space2 at byte offset entry.z + dwords * 4)
@(private)
hl_memexport_store_fn :: proc(h: ^H) {
	if h.fn_mstore {return}
	h.fn_mstore = true
	s := &h.fns
	strings.write_string(s, "void xe_memexport_write(uint entry, uint address, uint value) {\n")
	strings.write_string(s, "\t[branch] if (entry == 0u) xe_memexport_buffers[0].Store(address, value);\n")
	strings.write_string(s, "\telse if (entry == 1u) xe_memexport_buffers[1].Store(address, value);\n")
	strings.write_string(s, "\telse if (entry == 2u) xe_memexport_buffers[2].Store(address, value);\n")
	strings.write_string(s, "\telse xe_memexport_buffers[3].Store(address, value);\n}\n\n")
	strings.write_string(s, "void xe_memexport_store(uint4 ea, float4 v, uint k) {\n")
	strings.write_string(s, "\tuint base = ea.x & 0x3FFFFFFFu;\n\tuint index = ea.y & 0x7FFFFFu;\n\tuint fw = ea.z;\n\tuint count = ea.w & 0x7FFFFFu;\n")
	// the stream constant's fixed bits
	strings.write_string(s, "\tbool valid = all((ea >> uint4(30u, 23u, 20u, 23u)) == uint4(1u, 0x96u, 0x4B0u, 0x96u));\n")
	// the table entry whose range holds the stream's base (the first)
	strings.write_string(s, "\tbool found = false;\n\tuint size = 0u;\n\tuint skip = 0u;\n\tuint alo = 0u;\n\tuint entry = 0u;\n")
	for e in 0 ..< 4 {
		fmt.sbprintf(s, "\t{{\n\t\tuint4 me = xe_memexport_ranges[%d];\n\t\tuint d = base - me.x;\n\t\tbool m = (d < me.y) && !found;\n", e)
		fmt.sbprintf(s, "\t\tsize = m ? me.y : size;\n\t\tskip = m ? d : skip;\n\t\talo = m ? me.z : alo;\n\t\tentry = m ? %du : entry;\n\t\tfound = found || m;\n\t}}\n", e)
	}
	strings.write_string(s, "\tuint format = (fw >> 8u) & 0x3Fu;\n")
	// SurfaceNumberFormat: bit 16 signed, bit 17 integer (0 unorm, 1 snorm, 2 uint, 3 sint; float formats ignore it)
	strings.write_string(s, "\tuint numfmt = (fw >> 16u) & 3u;\n\tbool swap_rb = (fw & 0x80000u) != 0u;\n\tuint endian = fw & 7u;\n")
	strings.write_string(s, "\tbool is4 = format == 38u;\n\tbool is2 = ((format == 37u) || (format == 32u)) || (format == 26u);\n")
	strings.write_string(s, "\tuint elem = is4 ? 4u : (is2 ? 2u : 1u);\n\tuint ik = index + k;\n\tuint off = skip + (ik * elem);\n")
	strings.write_string(s, "\tbool in_count = ik < count;\n\tbool in_size = (off + elem) <= size;\n")
	strings.write_string(s, "\tbool go = (valid && found) && (in_count && in_size);\n")
	// formats this export packs; any other writes nothing
	strings.write_string(s, "\tbool known = false;\n")
	for kf in ([]u32{6, 7, 25, 26, 31, 32, 36, 37, 38}) {fmt.sbprintf(s, "\tknown = known || (format == %du);\n", kf)}
	strings.write_string(s, "\tgo = go && known;\n\t[branch] if (go) {\n")
	strings.write_string(s, "\t\tfloat4 vv = swap_rb ? v.zyxw : v;\n\t\tuint4 bits = asuint(vv);\n")
	strings.write_string(s, "\t\tuint h01 = f32tof16(vv.x) | (f32tof16(vv.y) << 16u);\n\t\tuint h23 = f32tof16(vv.z) | (f32tof16(vv.w) << 16u);\n")
	hl_pack_fixed(s, "p8888", "vv", {8, 8, 8, 8})
	hl_pack_fixed(s, "p2101010", "vv", {10, 10, 10, 2})
	hl_pack_fixed(s, "p1616", "vv", {16, 16, 0, 0})
	hl_pack_fixed(s, "p1616b", "vv.zwxy", {16, 16, 0, 0})
	strings.write_string(s, "\t\tuint w0 = bits.x;\n")
	strings.write_string(s, "\t\tw0 = (format == 6u) ? p8888 : w0;\n\t\tw0 = (format == 7u) ? p2101010 : w0;\n")
	strings.write_string(s, "\t\tw0 = (format == 25u) ? p1616 : w0;\n\t\tw0 = (format == 26u) ? p1616 : w0;\n")
	strings.write_string(s, "\t\tw0 = (format == 31u) ? h01 : w0;\n\t\tw0 = (format == 32u) ? h01 : w0;\n")
	strings.write_string(s, "\t\tuint w1 = (format == 26u) ? p1616b : bits.y;\n\t\tw1 = (format == 32u) ? h23 : w1;\n")
	strings.write_string(s, "\t\tuint4 words = uint4(w0, w1, bits.z, bits.w);\n")
	// Endian128: 0-3 per dword, 4 8in64 (dword pairs swapped), 5 8in128 (dwords reversed)
	strings.write_string(s, "\t\tuint e32 = (endian >= 4u) ? 2u : endian;\n\t\twords = xe_endian_swap(words, e32);\n")
	strings.write_string(s, "\t\tuint4 pair = words.yxwz;\n\t\tuint4 rev = words.wzyx;\n")
	strings.write_string(s, "\t\twords = (endian == 4u) ? pair : words;\n\t\twords = (endian == 5u) ? rev : words;\n")
	strings.write_string(s, "\t\txe_memexport_write(entry, alo + (off << 2u), words.x);\n")
	for j in u32(1) ..< 4 {
		fmt.sbprintf(s, "\t\tif (%du < elem) xe_memexport_write(entry, alo + ((off + %du) << 2u), words.%s);\n", j, j, XYZW_STR[j])
	}
	strings.write_string(s, "\t}\n}\n\n")
}

// one dword from up to four fixed-point components (Direct3D conversion rules), as pack_fixed
@(private)
hl_pack_fixed :: proc(s: ^strings.Builder, name: string, v: string, widths: [4]u32) {
	fmt.sbprintf(s, "\t\tuint %s = 0u;\n", name)
	shift: u32 = 0
	for k in u32(0) ..< 4 {
		wd := widths[k]
		if wd == 0 {continue}
		umax := f32((u64(1) << wd) - 1)
		smax := f32((u64(1) << (wd - 1)) - 1)
		fmt.sbprintf(s, "\t\t{{\n\t\t\tfloat c = %s.%s;\n\t\t\tc = isnan(c) ? 0.0 : c;\n", v, XYZW_STR[k])
		// 0 unorm, 1 snorm, 2 uint, 3 sint
		fmt.sbprintf(s, "\t\t\tuint un = uint((clamp(c, 0.0, 1.0) * %s) + 0.5);\n", hl_flit(umax))
		fmt.sbprintf(s, "\t\t\tfloat sc = clamp(c, -1.0, 1.0) * %s;\n", hl_flit(smax))
		strings.write_string(s, "\t\t\tsc = sc + ((sc >= 0.0) ? 0.5 : -0.5);\n\t\t\tuint sn = asuint(int(sc));\n")
		// integer: round half away from zero (Direct3D conversion rules, Xenia), clamped to keep the conversion defined
		fmt.sbprintf(s, "\t\t\tuint ui = uint(clamp(c + 0.5, 0.0, %s));\n", hl_flit(umax))
		strings.write_string(s, "\t\t\tfloat sr = c + ((c >= 0.0) ? 0.5 : -0.5);\n")
		fmt.sbprintf(s, "\t\t\tuint si = asuint(int(clamp(sr, %s, %s)));\n", hl_flit(-smax - 1), hl_flit(smax))
		strings.write_string(s, "\t\t\tuint q = (numfmt == 1u) ? sn : un;\n\t\t\tq = (numfmt == 2u) ? ui : q;\n\t\t\tq = (numfmt == 3u) ? si : q;\n")
		fmt.sbprintf(s, "\t\t\t%s = %s | ((q & 0x%Xu) << %du);\n\t\t}}\n", name, name, u32((u64(1) << wd) - 1), shift)
		shift += wd
	}
}

// ---- the module text ----

@(private)
hl_assemble :: proc(h: ^H, allocator := context.allocator) -> string {
	sb := strings.builder_make(allocator)
	fmt.sbprintf(&sb, "// Xenos microcode -> HLSL (port/tools/xenos_spirv hlsl.odin), ABI %d, %s, %d registers\n", ABI_VERSION, hl_profile(h.model), h.n_regs)
	strings.write_string(&sb, HLSL_PRELUDE)
	strings.write_string(&sb, h.stage == .Vertex ? HLSL_FLOAT_VS : HLSL_FLOAT_PS)
	if h.stage == .Vertex && h.p.vfetch_count > 0 {strings.write_string(&sb, HLSL_VERTEX_FETCH)}
	if h.p.memexport {strings.write_string(&sb, HLSL_MEMEXPORT)}
	strings.write_string(&sb, strings.to_string(h.decls))
	strings.write_string(&sb, "\n")
	strings.write_string(&sb, h.model == .Fragment ? HLSL_PS_INPUT : HLSL_INTERFACE)
	ps_outputs := h.model == .Fragment && (h.meta.color_outputs != 0 || h.meta.depth_output)
	if h.model == .Tess_Eval {strings.write_string(&sb, HLSL_TESS_TYPES)}
	if ps_outputs {
		strings.write_string(&sb, "struct XePSOutput {\n")
		for i in u32(0) ..< 4 {if h.meta.color_outputs & (1 << i) != 0 {fmt.sbprintf(&sb, "\tfloat4 oC%d : SV_Target%d;\n", i, i)}}
		if h.meta.depth_output {strings.write_string(&sb, "\tfloat depth : SV_Depth;\n")}
		strings.write_string(&sb, "};\n\n")
	}
	strings.write_string(&sb, strings.to_string(h.fns))
	strings.write_string(&sb, "[RootSignature(XE_ROOT_SIGNATURE)]\n")
	switch h.model {
	case .Vertex:
		strings.write_string(&sb, "XeInterface main(uint xe_vertex_id : SV_VertexID) {\n\tXeInterface xe_out = (XeInterface)0;\n")
	case .Tess_Eval:
		strings.write_string(&sb, "[domain(\"tri\")]\nXeInterface main(XeTessFactors xe_factors, float3 xe_domain : SV_DomainLocation, uint xe_primitive_id : SV_PrimitiveID, const OutputPatch<XeTessControlPoint, 1> xe_patch) {\n\tXeInterface xe_out = (XeInterface)0;\n")
	case .Fragment:
		fmt.sbprintf(&sb, "%s main(XePSInput xe_in, bool xe_front_face : SV_IsFrontFace) {{\n", ps_outputs ? "XePSOutput" : "void")
	case .Compute:
		strings.write_string(&sb, "[numthreads(64, 1, 1)]\nvoid main(uint3 xe_global_id : SV_DispatchThreadID) {\n")
	}
	strings.write_string(&sb, strings.to_string(h.body))
	strings.write_string(&sb, "}\n")
	return strings.to_string(sb)
}
