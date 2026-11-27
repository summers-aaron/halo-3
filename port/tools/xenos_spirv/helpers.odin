package xenos

// Fixed host modules the device pairs with translated shaders.
//
// Tessellated draws (DrawIndexedTessellatedVertices: Halo 3's water, D3DTPT_TRIPATCH in D3DTM_ADAPTIVE): host
// pipeline = tess_vs -> tess_tcs -> the translated Tessellation_Eval module (the guest vertex shader, which fetches
// everything itself by patch index, Xenia's kTriangleDomainPatchIndexed) -> the translated pixel shader. As Xenia's
// tessellation_adaptive.vs + adaptive_triangle.hs (3-Clause BSD):
//   - the draw's "index buffer" is the factor buffer (three float32 edge factors per patch, in the guest byte order
//     XeSystem.tessellation.z names), bound as 32-bit indices with patchControlPoints 3: gl_VertexIndex is a factor's
//     bits, and tess_vs passes factor + 1, clamped to XeSystem.tessellation.xy (VGT_HOS_MIN / MAX_TESS_LEVEL + 1);
//   - tess_tcs (OutputVertices 1): guest edge 0 is v0 -> v1, 1 is v1 -> v2, 2 is v2 -> v0; Vulkan's TessLevelOuter 0
//     is the U0 edge (v1 -> v2), 1 the V0 edge (v2 -> v0), 2 the W0 edge (v0 -> v1); the inner level is the least of
//     the edges. The TES numbers its patch by gl_PrimitiveID + XeSystem.vertex.x.

@(private)
helper_system_block :: proc(b: ^Builder) -> u32 {
	uv4 := b.t_uvec[4]
	v4 := b.t_vec[4]
	mem4 := type_array(b, uv4, 4, 16)
	planes := type_array(b, v4, SYS_CLIP_PLANES, 16)
	st := type_struct(b, v4, v4, v4, uv4, v4, mem4, v4, uv4, v4, planes)
	decorate(b, st, .Block)
	for o, i in SYS_MEMBER_OFFSETS {member_decorate(b, st, u32(i), .Offset, o)}
	name(b, st, "XeSystem")
	sys := global_var(b, .Uniform, st)
	decorate(b, sys, .DescriptorSet, SET_CONSTANTS)
	decorate(b, sys, .Binding, BINDING_SYSTEM)
	return sys
}

helper_tess_vs :: proc(allocator := context.allocator, meta: ^Module_Meta = nil) -> []u32 {
	b: Builder
	builder_init(&b, allocator)
	capability(&b, 1)
	emit(&b.memory_model, .MemoryModel, 0, 1)
	sys := helper_system_block(&b)
	vi := global_var(&b, .Input, b.t_int)
	decorate(&b, vi, .BuiltIn, u32(BuiltIn.VertexIndex))
	out := global_var(&b, .Output, b.t_float)
	decorate(&b, out, .Location, 0)
	name(&b, out, "out_edge_factor")
	fn := id(&b)
	ft := type_get(&b, .TypeFunction, b.t_void)
	op0(&b, .Function, b.t_void, fn, 0, ft)
	label(&b, id(&b))
	if meta != nil {meta.system_rows |= 1 << sys_row(.Tessellation)}
	tess := load(&b, b.t_vec[4], access(&b, type_ptr(&b, .Uniform, b.t_vec[4]), sys, const_u32(&b, u32(Sys.Tessellation))))
	endian := bitcast(&b, b.t_uint, extract(&b, b.t_float, tess, 2))
	w := bitcast(&b, b.t_uint, load(&b, b.t_int, vi))
	u := b.t_uint
	shl :: proc(b: ^Builder, x: u32, n: u32) -> u32 {return op1(b, .ShiftLeftLogical, b.t_uint, x, const_u32(b, n))}
	shr :: proc(b: ^Builder, x: u32, n: u32) -> u32 {return op1(b, .ShiftRightLogical, b.t_uint, x, const_u32(b, n))}
	and :: proc(b: ^Builder, x: u32, m: u32) -> u32 {return op1(b, .BitwiseAnd, b.t_uint, x, const_u32(b, m))}
	or :: proc(b: ^Builder, x, y: u32) -> u32 {return op1(b, .BitwiseOr, b.t_uint, x, y)}
	s8in16 := or(&b, shl(&b, and(&b, w, 0x00ff00ff), 8), and(&b, shr(&b, w, 8), 0x00ff00ff))
	s16in32 := or(&b, shl(&b, w, 16), shr(&b, w, 16))
	s8in32 := or(&b, shl(&b, s8in16, 16), shr(&b, s8in16, 16))
	is :: proc(b: ^Builder, e: u32, v: u32) -> u32 {return op1(b, .IEqual, b.t_bool, e, const_u32(b, v))}
	r := select(&b, u, is(&b, endian, 1), s8in16, w)
	r = select(&b, u, is(&b, endian, 2), s8in32, r)
	r = select(&b, u, is(&b, endian, 3), s16in32, r)
	factor := arith(&b, .FAdd, b.t_float, bitcast(&b, b.t_float, r), const_f32(&b, 1))
	factor = ext(&b, b.t_float, .NClamp, factor, extract(&b, b.t_float, tess, 0), extract(&b, b.t_float, tess, 1))
	store(&b, out, factor)
	op0(&b, .Return)
	op0(&b, .FunctionEnd)
	ep := str_words("main")
	head := make([dynamic]u32, context.temp_allocator)
	append(&head, 0, fn) // Vertex
	append(&head, ..ep[:])
	append(&head, vi, out, sys)
	emitv(&b.entry_points, .EntryPoint, {}, head[:])
	words := builder_finish(&b, allocator)
	builder_destroy(&b)
	return words
}

helper_tess_tcs :: proc(allocator := context.allocator) -> []u32 {
	b: Builder
	builder_init(&b, allocator)
	capability(&b, 1)
	capability(&b, 3) // Tessellation
	emit(&b.memory_model, .MemoryModel, 0, 1)
	f := b.t_float
	in_t := type_array(&b, f, 32) // gl_MaxPatchVertices
	factors := global_var(&b, .Input, in_t)
	decorate(&b, factors, .Location, 0)
	name(&b, factors, "in_edge_factor")
	outer := global_var(&b, .Output, type_array(&b, f, 4))
	inner := global_var(&b, .Output, type_array(&b, f, 2))
	decorate(&b, outer, .BuiltIn, 11); decorate(&b, outer, Decoration(15)) // TessLevelOuter, Patch
	decorate(&b, inner, .BuiltIn, 12); decorate(&b, inner, Decoration(15)) // TessLevelInner, Patch
	fn := id(&b)
	ft := type_get(&b, .TypeFunction, b.t_void)
	op0(&b, .Function, b.t_void, fn, 0, ft)
	label(&b, id(&b))
	pin := type_ptr(&b, .Input, f)
	pout := type_ptr(&b, .Output, f)
	e: [3]u32
	for k in u32(0) ..< 3 {e[k] = load(&b, f, access(&b, pin, factors, const_u32(&b, k)))}
	store(&b, access(&b, pout, outer, const_u32(&b, 0)), e[1]) // U0: v1 -> v2
	store(&b, access(&b, pout, outer, const_u32(&b, 1)), e[2]) // V0: v2 -> v0
	store(&b, access(&b, pout, outer, const_u32(&b, 2)), e[0]) // W0: v0 -> v1
	store(&b, access(&b, pout, outer, const_u32(&b, 3)), e[0]) // (quads only)
	m := ext(&b, f, .FMin, ext(&b, f, .FMin, e[0], e[1]), e[2])
	store(&b, access(&b, pout, inner, const_u32(&b, 0)), m)
	store(&b, access(&b, pout, inner, const_u32(&b, 1)), m) // (quads only)
	op0(&b, .Return)
	op0(&b, .FunctionEnd)
	ep := str_words("main")
	head := make([dynamic]u32, context.temp_allocator)
	append(&head, 1, fn) // TessellationControl
	append(&head, ..ep[:])
	append(&head, factors, outer, inner)
	emitv(&b.entry_points, .EntryPoint, {}, head[:])
	emit(&b.exec_modes, .ExecutionMode, fn, 26, 1) // OutputVertices 1
	words := builder_finish(&b, allocator)
	builder_destroy(&b)
	return words
}

// Rectangle lists (D3DPT_RECTLIST): the Xenos makes each three vertices a screen-aligned rectangle, the fourth corner
// mirrored across the longest edge (the diagonal). A host geometry shader does it, as Xenia's built-in rectangle-list
// geometry shader (spirv_builtin_geometry_shader.cc, 3-Clause BSD): the longest edge picks the strip order (12: 0123,
// 20: 1203, 01: 2013) and the fourth vertex is v1 - v0 + v2 for the position, the clip distances and every
// interpolator. One module per set of vertex shader outputs (`locations`, the translated VS's interface mask).
helper_rect_gs :: proc(locations: u32, allocator := context.allocator) -> []u32 {
	b: Builder
	builder_init(&b, allocator)
	capability(&b, 1)
	capability(&b, 2) // Geometry
	capability(&b, 32) // ClipDistance
	emit(&b.memory_model, .MemoryModel, 0, 1)
	v4 := b.t_vec[4]
	f := b.t_float
	i := b.t_int
	interface := make([dynamic]u32, context.temp_allocator)
	in_pos := global_var(&b, .Input, type_array(&b, v4, 3))
	decorate(&b, in_pos, .BuiltIn, u32(BuiltIn.Position))
	cd_t := type_array(&b, f, SYS_CLIP_PLANES)
	in_cd := global_var(&b, .Input, type_array(&b, cd_t, 3))
	decorate(&b, in_cd, .BuiltIn, 3) // ClipDistance
	out_pos := global_var(&b, .Output, v4)
	decorate(&b, out_pos, .BuiltIn, u32(BuiltIn.Position))
	out_cd := global_var(&b, .Output, cd_t)
	decorate(&b, out_cd, .BuiltIn, 3)
	append(&interface, in_pos, in_cd, out_pos, out_cd)
	ins, outs: [32]u32
	for l in u32(0) ..< 32 {
		if locations & (1 << l) == 0 {continue}
		ins[l] = global_var(&b, .Input, type_array(&b, v4, 3))
		decorate(&b, ins[l], .Location, l)
		outs[l] = global_var(&b, .Output, v4)
		decorate(&b, outs[l], .Location, l)
		append(&interface, ins[l], outs[l])
	}
	fn := id(&b)
	ft := type_get(&b, .TypeFunction, b.t_void)
	op0(&b, .Function, b.t_void, fn, 0, ft)
	label(&b, id(&b))
	piv4 := type_ptr(&b, .Input, v4)
	picd := type_ptr(&b, .Input, cd_t)
	pin_v4 := piv4
	ci :: proc(b: ^Builder, v: i32) -> u32 {return const_i32(b, v)}
	// squared edge lengths: [0] 12, [1] 20, [2] 01
	edge: [3]u32
	for k in 0 ..< 3 {
		p0 := load(&b, v4, access(&b, piv4, in_pos, ci(&b, i32((1 + k) % 3))))
		p1 := load(&b, v4, access(&b, piv4, in_pos, ci(&b, i32((2 + k) % 3))))
		dx := arith(&b, .FSub, f, extract(&b, f, p1, 0), extract(&b, f, p0, 0))
		dy := arith(&b, .FSub, f, extract(&b, f, p1, 1), extract(&b, f, p0, 1))
		edge[k] = arith(&b, .FAdd, f, arith(&b, .FMul, f, dx, dx), arith(&b, .FMul, f, dy, dy))
	}
	gt :: proc(b: ^Builder, x, y: u32) -> u32 {return op1(b, .FOrdGreaterThan, b.t_bool, x, y)}
	first := select(&b, i, op1(&b, .LogicalAnd, b.t_bool, gt(&b, edge[0], edge[1]), gt(&b, edge[0], edge[2])), ci(&b, 0),
		select(&b, i, gt(&b, edge[1], edge[2]), ci(&b, 1), ci(&b, 2)))
	vi: [3]u32
	vi[0] = first
	for k in 1 ..< 3 {
		n := op1(&b, .IAdd, i, first, ci(&b, i32(k)))
		vi[k] = select(&b, i, op1(&b, .SLessThan, b.t_bool, n, ci(&b, 3)), n, op1(&b, .ISub, i, n, ci(&b, 3)))
	}
	for k in 0 ..< 3 {
		for l in u32(0) ..< 32 {
			if ins[l] == 0 {continue}
			store(&b, outs[l], load(&b, v4, access(&b, pin_v4, ins[l], vi[k])))
		}
		store(&b, out_pos, load(&b, v4, access(&b, piv4, in_pos, vi[k])))
		store(&b, out_cd, load(&b, cd_t, access(&b, picd, in_cd, vi[k])))
		op0(&b, .EmitVertex)
	}
	mirror :: proc(b: ^Builder, ty: u32, a, c, d: u32) -> u32 {
		// c - a + d (v1 - v0 + v2); arith marks it NoContraction, as Xenia
		return arith(b, .FAdd, ty, arith(b, .FSub, ty, c, a), d)
	}
	for l in u32(0) ..< 32 {
		if ins[l] == 0 {continue}
		a := load(&b, v4, access(&b, pin_v4, ins[l], vi[0]))
		c := load(&b, v4, access(&b, pin_v4, ins[l], vi[1]))
		d := load(&b, v4, access(&b, pin_v4, ins[l], vi[2]))
		store(&b, outs[l], mirror(&b, v4, a, c, d))
	}
	{
		a := load(&b, v4, access(&b, piv4, in_pos, vi[0]))
		c := load(&b, v4, access(&b, piv4, in_pos, vi[1]))
		d := load(&b, v4, access(&b, piv4, in_pos, vi[2]))
		store(&b, out_pos, mirror(&b, v4, a, c, d))
		pf := type_ptr(&b, .Input, f)
		po := type_ptr(&b, .Output, f)
		for k in u32(0) ..< SYS_CLIP_PLANES {
			ca := load(&b, f, access(&b, pf, in_cd, vi[0], const_u32(&b, k)))
			cc := load(&b, f, access(&b, pf, in_cd, vi[1], const_u32(&b, k)))
			cdd := load(&b, f, access(&b, pf, in_cd, vi[2], const_u32(&b, k)))
			store(&b, access(&b, po, out_cd, const_u32(&b, k)), mirror(&b, f, ca, cc, cdd))
		}
	}
	op0(&b, .EmitVertex)
	op0(&b, .EndPrimitive)
	op0(&b, .Return)
	op0(&b, .FunctionEnd)
	ep := str_words("main")
	head := make([dynamic]u32, context.temp_allocator)
	append(&head, 3, fn) // Geometry
	append(&head, ..ep[:])
	append(&head, ..interface[:])
	emitv(&b.entry_points, .EntryPoint, {}, head[:])
	emit(&b.exec_modes, .ExecutionMode, fn, 22) // Triangles
	emit(&b.exec_modes, .ExecutionMode, fn, 0, 1) // Invocations 1
	emit(&b.exec_modes, .ExecutionMode, fn, 29) // OutputTriangleStrip
	emit(&b.exec_modes, .ExecutionMode, fn, 26, 4) // OutputVertices 4
	words := builder_finish(&b, allocator)
	builder_destroy(&b)
	return words
}
