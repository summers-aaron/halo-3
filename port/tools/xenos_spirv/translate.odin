package xenos

// Xenos microcode -> SPIR-V 1.6 (Vulkan 1.3). Semantics follow Xenia's SpirvShaderTranslator (3-Clause BSD); the
// structure is our own: every value is a vec4 (scalar results are splat), registers are a Function array; control
// flow with labels becomes nested selections and loops where the program has their shapes (structure.odin), else it
// runs in a `loop { switch (pc) }` like Xenia; straight-line programs need no loop at all.

Exec_Model :: enum u8 {
	Vertex   = 0,
	Tess_Eval = 2,
	Fragment = 4,
	Compute  = 5,
}

Texture_Use :: struct {
	fetch:   u32,
	dim:     u32, // TEX_*: 0 2D (1D/2D), 1 3D, 2 cube, 3 stacked (a tfetch3D records 1 and 3)
	sampler: u32, // 0 fetch constant, 1 point override, 2 linear override
}

Meta :: struct {
	model:          Exec_Model,
	registers:      int,
	labels:         bool,
	textures:       [dynamic]Texture_Use,
	vertex_elements: int,
	vs_outputs:     u32, // location mask
	ps_inputs:      u32,
	param_gen:      int, // PS: register receiving VPOS/VFACE, -1 none
	color_outputs:  u32,
	depth_output:   bool,
	kills:          bool,
	memexport:      bool,
	memexport_streams: [dynamic]u32, // float constants holding the stream constants (mad eA, r, c_0100, c#)
	uses_bda:       bool,
	point_size:     bool,
	float_defs:     int,
	defs:           [dynamic]Float_Def, // the literal constants (def), which the device writes over the bound float constants
	elements:       [dynamic]Vertex_Element, // XeVertexFetch.element[k] = the vertex declaration element of this usage / index
	module_meta:    Module_Meta,
	overlay:        Entry_Overlay,
	image_keys:     [dynamic]u32,
	sampler_keys:   [dynamic]u32,
}

Result :: struct {
	words: []u32,
	meta:  Meta,
	error: string,
}

@(private)
T :: struct {
	b:            Builder,
	c:            ^Container,
	p:            ^Program,
	stage:        Stage,
	model:        Exec_Model,
	abi6:         bool,
	meta:         Meta,
	err:          string,
	// types
	t_v4, t_f, t_u, t_i, t_b: u32,
	t_pf_v4:      u32, // pointer Function vec4
	// state variables
	v_r:          u32,
	v_regs:       [dynamic]u32, // one Function variable per register when nothing indexes the register file
	n_regs:       u32,
	v_ps, v_pred, v_a0, v_al, v_lc, v_pc, v_lod, v_grad_h, v_grad_v: u32,
	v_o:          u32, // VS interpolator exports o0..o15 (Function array)
	v_opos, v_opts: u32,
	v_oc:         u32, // PS oC0..3
	v_odepth:     u32,
	v_ea:         u32,
	v_em:         u32,
	v_em_mask:    u32,
	// uniforms
	u_sys, u_float, u_bl, u_fetch, u_vf: u32,
	u_push:       u32,
	defs:         map[u32]u32, // c# -> constant vec4
	int_defs:     map[u32]u32, // loop id -> constant
	tex:          map[u32]u32, // key fetch | dim << 8 -> image variable
	smp:          map[u32]u32, // key fetch | variant << 8 -> sampler variable
	t_img:        [4]u32,
	t_smp:        u32,
	bindless_images:   [4]u32,
	bindless_samplers: u32,
	image_offsets:    [IMAGE_KEY_LIMIT]u32, // key -> SPIR-V ID of its compact table byte offset
	sampler_offsets:  [SAMPLER_KEY_LIMIT]u32,
	interface:    [dynamic]u32,
	// buffer device address
	t_words_ptr:  u32, // pointer PhysicalStorageBuffer to struct { uint d[]; }
	t_word_ptr:   u32, // pointer PhysicalStorageBuffer uint
	t_row_ptr:    u32, // pointer PhysicalStorageBuffer vec4
	t_address_sum: u32, // pair of uint (sum, carry)
	// control flow
	l_header, l_continue, l_merge, l_sw_merge, l_end: u32,
	case_labels:  map[int]u32,
	sw_cases:     [dynamic]u32,
	main_fn:      u32,
	vf:           VF_Vars,
	fn_vdecode, fn_mstore: u32,
	// structured control flow (structure.odin): the dispatcher is not used
	structured:   bool,
	loop_skip, loop_old_lc, loop_old_al: u32,
}

translate :: proc(c: ^Container, p: ^Program, allocator := context.allocator, abi: u32 = ABI_VERSION) -> (res: Result) {
	context.allocator = allocator
	if abi != ABI_VERSION && abi != ABI_VERSION_BINDLESS {
		return Result{error = "unsupported Xenos SPIR-V ABI"}
	}
	t: T
	t.c, t.p, t.stage = c, p, c.stage
	t.abi6 = abi == ABI_VERSION_BINDLESS
	t.meta.param_gen = -1
	t.model = exec_model(c, p)
	if t.model == .Tess_Eval {capability(&t.b, 3)}
	t.meta.model = t.model
	t.meta.labels = p.has_labels
	t.meta.memexport = p.memexport
	t.meta.kills = p.kills
	if t.abi6 {
		rows: [256][4]u32
		for d in c.float_defs {
			if d.reg >= len(rows) {return Result{error = "float definition outside the stage bank"}}
			rows[d.reg] = d.value
			t.meta.overlay.literal_row_mask[d.reg >> 5] |= 1 << (d.reg & 31)
		}
		for value, reg in rows {
			if t.meta.overlay.literal_row_mask[reg >> 5] & (1 << u32(reg & 31)) != 0 {
				append(&t.meta.defs, Float_Def{reg = u32(reg), value = value})
			}
		}
	} else {
		for d in c.float_defs {append(&t.meta.defs, d)}
	}
	t.meta.float_defs = len(t.meta.defs)
	for v in c.vertex_elements {append(&t.meta.elements, v)}
	t.defs = make(map[u32]u32)
	t.int_defs = make(map[u32]u32)
	t.tex = make(map[u32]u32)
	t.smp = make(map[u32]u32)
	t.case_labels = make(map[int]u32)
	b := &t.b
	builder_init(b)
	t.t_v4, t.t_f, t.t_u, t.t_i, t.t_b = b.t_vec[4], b.t_float, b.t_uint, b.t_int, b.t_bool
	t.t_pf_v4 = type_ptr(b, .Function, t.t_v4)

	capability(b, 1) // Shader
	needs_bda := t.abi6 || (c.stage == .Vertex && p.vfetch_count > 0) || p.memexport
	t.meta.uses_bda = needs_bda
	if needs_bda {
		capability(b, 5347) // PhysicalStorageBufferAddresses
		emit(&b.memory_model, .MemoryModel, 5348, 1) // PhysicalStorageBuffer64, GLSL450
		w := type_struct(b, type_get(b, .TypeRuntimeArray, t.t_u))
		// the runtime array type is shared: decorate once
		decorate(b, type_get(b, .TypeRuntimeArray, t.t_u), .ArrayStride, 4)
		decorate(b, w, .Block)
		member_decorate(b, w, 0, .Offset, 0)
		name(b, w, "XeWords")
		t.t_words_ptr = type_ptr(b, .PhysicalStorageBuffer, w)
		t.t_word_ptr = type_ptr(b, .PhysicalStorageBuffer, t.t_u)
	} else {
		emit(&b.memory_model, .MemoryModel, 0, 1) // Logical, GLSL450
	}

	declare_uniforms(&t)
	if t.abi6 {
		declare_push_constants(&t)
		declare_bindless(&t)
	}
	// literal constants
	for d in t.meta.defs {t.defs[d.reg] = const_vec4_bits(b, d.value)}
	// integer literal constants: the k-th defi feeds the k-th distinct loop constant the program uses (loop_literals)
	{
		ids := loop_literal_ids(p)
		defer delete(ids)
		for d, k in c.int_defs {if k < len(ids) {t.int_defs[ids[k]] = const_u32(b, d.value)}}
	}

	// registers
	t.n_regs = register_count(c, p)
	t.meta.registers = int(t.n_regs)

	// main
	fn_type := type_get(b, .TypeFunction, b.t_void)
	t.main_fn = id(b)
	name(b, t.main_fn, "main")
	entry_label := id(b)

	if p.temp_rel {
		t.v_r = local_var(b, type_array(b, t.t_v4, t.n_regs))
		name(b, t.v_r, "r")
	} else {
		for i in 0 ..< t.n_regs {
			v := local_var(b, t.t_v4, const_splat(b, 4, 0))
			name(b, v, fmt_name("r%d", i))
			append(&t.v_regs, v)
		}
	}
	t.v_ps = local_var(b, t.t_f, const_f32(b, 0))
	t.v_pred = local_var(b, t.t_b, const_bool(b, false))
	t.v_a0 = local_var(b, t.t_i, const_i32(b, 0))
	t.v_al = local_var(b, b.t_ivec[4], const_composite(b, b.t_ivec[4], const_i32(b, 0), const_i32(b, 0), const_i32(b, 0), const_i32(b, 0)))
	t.v_lc = local_var(b, b.t_uvec[4], const_usplat(b, 4, 0))
	t.v_lod = local_var(b, t.t_f, const_f32(b, 0))
	zero3 := const_splat(b, 3, 0)
	t.v_grad_h = local_var(b, b.t_vec[3], zero3)
	t.v_grad_v = local_var(b, b.t_vec[3], zero3)
	name(b, t.v_ps, "ps"); name(b, t.v_pred, "p0"); name(b, t.v_a0, "a0"); name(b, t.v_al, "aL")
	zero4 := const_splat(b, 4, 0)
	switch t.model {
	case .Vertex, .Tess_Eval:
		t.v_o = local_var(b, type_array(b, t.t_v4, 16), const_composite(b, type_array(b, t.t_v4, 16), ..splat16(b, zero4)))
		t.v_opos = local_var(b, t.t_v4, zero4)
		t.v_opts = local_var(b, t.t_v4, zero4)
	case .Fragment:
		t.v_oc = local_var(b, type_array(b, t.t_v4, 4), const_composite(b, type_array(b, t.t_v4, 4), zero4, zero4, zero4, zero4))
		t.v_odepth = local_var(b, t.t_v4, zero4)
	case .Compute:
	}
	if p.memexport {
		t.v_ea = local_var(b, t.t_v4, zero4)
		t.v_em = local_var(b, type_array(b, t.t_v4, 5), const_composite(b, type_array(b, t.t_v4, 5), zero4, zero4, zero4, zero4, zero4))
		t.v_em_mask = local_var(b, t.t_u, const_u32(b, 0))
	}
	// labels: nested selections and loops when the program has their shapes (structure.odin), else the dispatcher
	t.structured = p.has_labels && structure_walk(&t, 0, len(p.cf), false)
	dispatch := p.has_labels && !t.structured
	if dispatch {t.v_pc = local_var(b, t.t_i, const_i32(b, 0))}

	op0(b, .Function, b.t_void, t.main_fn, 0, fn_type)
	label(b, entry_label)
	// fn_vars are spliced after this label at assembly time
	body_start := len(b.body)

	// zero the registers, then the stage inputs
	if p.temp_rel {store(b, t.v_r, const_composite(b, type_array(b, t.t_v4, t.n_regs), ..splatn(b, zero4, int(t.n_regs))))}
	stage_inputs(&t)

	// control flow
	t.l_end = id(b)
	if dispatch {
		t.l_header, t.l_continue, t.l_merge, t.l_sw_merge = id(b), id(b), id(b), id(b)
		for lbl, ci in p.labels {if lbl {t.case_labels[ci] = id(b)}}
		t.case_labels[0] = t.case_labels[0] or_else id(b)
		branch(b, t.l_header)
		label(b, t.l_header)
		op0(b, .LoopMerge, t.l_merge, t.l_continue, 0)
		body := id(b)
		branch(b, body)
		label(b, body)
		pc := load(b, t.t_i, t.v_pc)
		op0(b, .SelectionMerge, t.l_sw_merge, 0)
		// OpSwitch selector default (literal label)*
		sw := make([dynamic]u32, context.temp_allocator)
		append(&sw, pc, t.l_sw_merge)
		keys := make([dynamic]int, context.temp_allocator)
		for k in t.case_labels {append(&keys, k)}
		sort_ints(keys[:])
		for k in keys {append(&sw, u32(k), t.case_labels[k])}
		op0(b, .Switch, ..sw[:])
		b.block_open = false
		label(b, t.case_labels[0])
	}
	if t.structured {
		if !structure_walk(&t, 0, len(p.cf), true) && t.err == "" {t.err = "internal: the structured control flow did not translate"}
	} else {
		for &cf, ci in p.cf {
			if ci > 0 && dispatch && p.labels[ci] {
				// fall into the next case through the loop: pc = ci
				if b.block_open {
					store(b, t.v_pc, const_i32(b, i32(ci)))
					branch(b, t.l_continue)
				}
				label(b, t.case_labels[ci])
			}
			if !b.block_open {label(b, id(b))} // unreachable code after an exec_end / jump
			emit_cf(&t, &cf, ci)
			if t.err != "" {break}
		}
	}
	if dispatch {
		// the program ran off the end of the control flow: end
		branch(b, t.l_sw_merge)
		label(b, t.l_sw_merge)
		branch(b, t.l_merge)
		label(b, t.l_continue)
		branch(b, t.l_header)
		label(b, t.l_merge)
		branch(b, t.l_end)
	} else {
		branch(b, t.l_end)
	}
	label(b, t.l_end)
	if p.memexport {memexport_flush(&t)}
	stage_outputs(&t)
	op0(b, .Return)
	b.block_open = false
	op0(b, .FunctionEnd)
	if t.err != "" {
		res.error = t.err
		builder_destroy(b)
		return
	}
	if t.abi6 {assign_bindless_offsets(&t)}
	// splice the function variables after the entry label
	{
		tail := make([dynamic]u32, context.temp_allocator)
		append(&tail, ..b.body[body_start:])
		resize(&b.body, body_start)
		append(&b.body, ..b.fn_vars[:])
		append(&b.body, ..tail[:])
	}
	// entry point and execution modes
	{
		ep := make([dynamic]u32, context.temp_allocator)
		append(&ep, u32(t.model), t.main_fn)
		ep_name := str_words("main")
		append(&ep, ..ep_name[:])
		append(&ep, ..t.interface[:])
		emitv(&b.entry_points, .EntryPoint, {}, ep[:])
		switch t.model {
		case .Fragment:
			emit(&b.exec_modes, .ExecutionMode, t.main_fn, 7) // OriginUpperLeft
			if t.meta.depth_output {emit(&b.exec_modes, .ExecutionMode, t.main_fn, 12)} // DepthReplacing
		case .Compute:
			emit(&b.exec_modes, .ExecutionMode, t.main_fn, 17, 64, 1, 1) // LocalSize
		case .Tess_Eval:
			emit(&b.exec_modes, .ExecutionMode, t.main_fn, 22) // Triangles
			emit(&b.exec_modes, .ExecutionMode, t.main_fn, 2) // SpacingFractionalEven (Xenia: continuous / adaptive)
			emit(&b.exec_modes, .ExecutionMode, t.main_fn, 4) // VertexOrderCw
		case .Vertex:
		}
	}
	res.words = builder_finish(b)
	res.meta = t.meta
	builder_destroy(b)
	delete(t.defs); delete(t.int_defs); delete(t.tex); delete(t.smp); delete(t.case_labels); delete(t.interface)
	return
}

// The host stage a guest shader becomes (both backends).
exec_model :: proc(c: ^Container, p: ^Program) -> Exec_Model {
	model: Exec_Model = c.stage == .Pixel ? .Fragment : .Vertex
	// a vertex shader that only writes memory (particles, contrails, beams, light volumes, ripples, water
	// tessellation factors) becomes a compute shader: one invocation per guest vertex
	if c.stage == .Vertex && p.memexport && p.exports & 0xffff == 0 {model = .Compute}
	// a vertex shader reading r1 before writing it runs behind the Xenos tessellator (Halo 3 water): r0.xyz =
	// barycentrics (zyx), r1.x = patch index (Xenia kTriangleDomainPatchIndexed) -> tessellation evaluation shader
	if c.stage == .Vertex && model == .Vertex && p.unfed_reads & 2 != 0 {model = .Tess_Eval}
	return model
}

// The loop constants the program uses, in first-use order: the k-th integer literal constant (defi) feeds the k-th
// (key layout unresolved; every retail use has one defi and one loop constant). Both backends.
loop_literal_ids :: proc(p: ^Program) -> [dynamic]u32 {
	ids: [dynamic]u32
	for &cf in p.cf {
		if cf.op != .Loop_Start && cf.op != .Loop_End {continue}
		found := false
		for x in ids {if x == cf.loop_id {found = true}}
		if !found {append(&ids, cf.loop_id)}
	}
	return ids
}

// The register file a translation declares (both backends): every register the program names, a pixel shader's
// interpolators and the param gen register after them, the whole file when aL-relative addressing indexes it.
register_count :: proc(c: ^Container, p: ^Program) -> u32 {
	n := u32(max(p.max_temp + 1, 1))
	if c.stage == .Pixel {
		n = max(n, u32(len(c.interpolators)) + 1)
		for v in c.interpolators {n = max(n, v.reg + 1)}
	}
	if p.temp_rel {n = 64}
	return n
}

@(private)
sort_ints :: proc(a: []int) {
	for i in 1 ..< len(a) {
		for j := i; j > 0 && a[j - 1] > a[j]; j -= 1 {a[j - 1], a[j] = a[j], a[j - 1]}
	}
}

@(private)
splat16 :: proc(b: ^Builder, v: u32) -> []u32 {return splatn(b, v, 16)}
@(private)
splatn :: proc(b: ^Builder, v: u32, n: int) -> []u32 {
	out := make([]u32, n, context.temp_allocator)
	for i in 0 ..< n {out[i] = v}
	return out
}

// ---- declarations ----

@(private)
ubo :: proc(t: ^T, binding: u32, block_name: string, members: []u32, offsets: []u32, names: []string) -> u32 {
	b := &t.b
	st := type_struct(b, ..members)
	decorate(b, st, .Block)
	for o, i in offsets {
		member_decorate(b, st, u32(i), .Offset, o)
		member_name(b, st, u32(i), names[i])
	}
	name(b, st, block_name)
	v := global_var(b, .Uniform, st)
	decorate(b, v, .DescriptorSet, SET_CONSTANTS)
	decorate(b, v, .Binding, binding)
	append(&t.interface, v)
	return v
}

@(private)
declare_uniforms :: proc(t: ^T) {
	b := &t.b
	if !t.abi6 {
		a4_256 := type_array(b, t.t_v4, 256, 16)
		t.u_float = ubo(t, t.stage == .Vertex ? BINDING_FLOAT_VS : BINDING_FLOAT_PS, t.stage == .Vertex ? "XeFloatVS" : "XeFloatPS", {a4_256}, {0}, {"c"})
	}
	uv4 := b.t_uvec[4]
	t.u_bl = ubo(t, BINDING_BOOL_LOOP, "XeBoolLoop", {type_array(b, uv4, 2, 16), type_array(b, uv4, 8, 16)}, {0, 32}, {"b", "i"})
	t.u_fetch = ubo(t, BINDING_FETCH, "XeFetch", {type_array(b, uv4, 48, 16), type_array(b, uv4, 32, 16)}, {0, 768}, {"f", "derived"})
	mem4 := type_array(b, uv4, 4, 16)
	sys_offsets := SYS_MEMBER_OFFSETS
	t.u_sys = ubo(t, BINDING_SYSTEM, "XeSystem", {t.t_v4, t.t_v4, t.t_v4, uv4, t.t_v4, mem4, t.t_v4, uv4, t.t_v4, type_array(b, t.t_v4, SYS_CLIP_PLANES, 16)},
		sys_offsets[:],
		{"ndc_scale", "ndc_offset", "pixel_scale", "alpha_test", "color_exp_bias", "memexport", "point", "vertex", "tessellation", "clip_planes"})
	if t.stage == .Vertex && t.p.vfetch_count > 0 {
		t.u_vf = ubo(t, BINDING_VERTEX_FETCH, "XeVertexFetch", {type_array(b, uv4, 16, 16), type_array(b, uv4, 32, 16), type_array(b, uv4, 32, 16), type_array(b, t.t_v4, 32, 16)},
			{0, 256, 768, 1280}, {"stream", "element", "unpack", "denominator"})
	}
}

@(private)
declare_push_constants :: proc(t: ^T) {
	b := &t.b
	st := type_struct(b, b.t_uvec[2], t.t_u, t.t_u, t.t_u, t.t_u, t.t_u, t.t_u)
	decorate(b, st, .Block)
	names := [?]string{"arena_bda", "vs_chunk_table_off", "ps_chunk_table_off", "vs_tex_table_off", "ps_tex_table_off", "vs_samp_table_off", "ps_samp_table_off"}
	for offset, i in PUSH_MEMBER_OFFSETS {
		member_decorate(b, st, u32(i), .Offset, offset)
		member_name(b, st, u32(i), names[i])
	}
	name(b, st, "XePush")
	t.u_push = global_var(b, .PushConstant, st)
	append(&t.interface, t.u_push)
	t.t_row_ptr = type_ptr(b, .PhysicalStorageBuffer, t.t_v4)
	t.t_address_sum = type_struct(b, t.t_u, t.t_u)
}

@(private)
push_load :: proc(t: ^T, member: Push_Member, ty: u32) -> u32 {
	b := &t.b
	return load(b, ty, access(b, type_ptr(b, .PushConstant, ty), t.u_push, const_u32(b, u32(member))))
}

@(private)
stage_table_offset :: proc(t: ^T, vs_member: Push_Member) -> u32 {
	member := t.stage == .Pixel ? Push_Member(u32(vs_member) + 1) : vs_member
	return push_load(t, member, t.t_u)
}

@(private)
arena_load :: proc(t: ^T, offset, ty, ptr_ty, alignment: u32) -> u32 {
	b := &t.b
	base := push_load(t, .Arena_BDA, b.t_uvec[2])
	sum := op1(b, .IAddCarry, t.t_address_sum, extract(b, t.t_u, base, 0), offset)
	lo := extract(b, t.t_u, sum, 0)
	hi := op1(b, .IAdd, t.t_u, extract(b, t.t_u, base, 1), extract(b, t.t_u, sum, 1))
	ptr := bitcast(b, ptr_ty, op1(b, .CompositeConstruct, b.t_uvec[2], lo, hi))
	return op1(b, .Load, ty, ptr, 2, alignment) // Aligned
}

@(private)
load_float_row :: proc(t: ^T, reg: u32) -> u32 {
	b := &t.b
	u := t.t_u
	chunk := op1(b, .ShiftRightLogical, u, reg, const_u32(b, 2))
	word_index := op1(b, .ShiftRightLogical, u, chunk, const_u32(b, 1))
	word_offset := op1(b, .ShiftLeftLogical, u, word_index, const_u32(b, 2))
	table_offset := stage_table_offset(t, .VS_Chunk_Table)
	word := arena_load(t, op1(b, .IAdd, u, table_offset, word_offset), u, t.t_word_ptr, 4)
	half := op1(b, .BitwiseAnd, u, chunk, const_u32(b, 1))
	shift := op1(b, .ShiftLeftLogical, u, half, const_u32(b, 4))
	chunk_id := op1(b, .BitwiseAnd, u, op1(b, .ShiftRightLogical, u, word, shift), const_u32(b, 0xffff))
	chunk_offset := op1(b, .ShiftLeftLogical, u, chunk_id, const_u32(b, 6))
	row := op1(b, .BitwiseAnd, u, reg, const_u32(b, 3))
	row_offset := op1(b, .ShiftLeftLogical, u, row, const_u32(b, 4))
	return arena_load(t, op1(b, .IAdd, u, chunk_offset, row_offset), t.t_v4, t.t_row_ptr, 16)
}

@(private)
sys_vec4 :: proc(t: ^T, m: Sys) -> u32 {
	b := &t.b
	if t.abi6 {t.meta.module_meta.system_rows |= 1 << sys_row(m)}
	return load(b, t.t_v4, access(b, type_ptr(b, .Uniform, t.t_v4), t.u_sys, const_u32(b, u32(m))))
}
@(private)
sys_uvec4 :: proc(t: ^T, m: Sys) -> u32 {
	b := &t.b
	if t.abi6 {t.meta.module_meta.system_rows |= 1 << sys_row(m)}
	return load(b, b.t_uvec[4], access(b, type_ptr(b, .Uniform, b.t_uvec[4]), t.u_sys, const_u32(b, u32(m))))
}

// A dword of a uvec4 array in a UBO, with a constant dword index inside the member.
@(private)
ubo_dword :: proc(t: ^T, v: u32, member: u32, dw: u32) -> u32 {
	b := &t.b
	if t.abi6 && v == t.u_fetch {
		row := (member == 0 ? 0 : 48) + (dw >> 2)
		t.meta.module_meta.fetch_rows[row >> 5] |= 1 << (row & 31)
	}
	return load(b, t.t_u, access(b, type_ptr(b, .Uniform, t.t_u), v, const_u32(b, member), const_u32(b, dw >> 2), const_u32(b, dw & 3)))
}

// ---- operands ----

@(private)
reg_ptr :: proc(t: ^T, reg: u32, rel: bool) -> u32 {
	b := &t.b
	if !rel && len(t.v_regs) > 0 {return t.v_regs[min(reg, t.n_regs - 1)]}
	idx := const_u32(b, reg)
	if rel {
		al := extract(b, t.t_i, load(b, b.t_ivec[4], t.v_al), 0)
		i := op1(b, .IAdd, t.t_i, bitcast(b, t.t_i, idx), al)
		idx = bitcast(b, t.t_u, ext(b, t.t_i, GLSL(45) /* SClamp */, i, const_i32(b, 0), const_i32(b, i32(t.n_regs) - 1)))
	} else if reg >= t.n_regs {
		idx = const_u32(b, t.n_regs - 1) // out of the declared range (never reached by retail code)
	}
	return access(b, t.t_pf_v4, t.v_r, idx)
}

@(private)
shuffle :: proc(t: ^T, v: u32, swz: [4]u8) -> u32 {
	if swz == {0, 1, 2, 3} {return v}
	return op1(&t.b, .VectorShuffle, t.t_v4, v, v, u32(swz[0]), u32(swz[1]), u32(swz[2]), u32(swz[3]))
}

@(private)
load_operand_raw :: proc(t: ^T, o: ^Operand) -> u32 {
	b := &t.b
	if o.is_temp {return load(b, t.t_v4, reg_ptr(t, o.reg, o.rel != 0))}
	if o.rel == 0 {
		if c, ok := t.defs[o.reg]; ok {return c}
		if t.abi6 {
			t.meta.module_meta.direct_float_mask[o.reg >> 5] |= 1 << (o.reg & 31)
			return load_float_row(t, const_u32(b, o.reg))
		}
		return load(b, t.t_v4, access(b, type_ptr(b, .Uniform, t.t_v4), t.u_float, const_u32(b, 0), const_u32(b, o.reg)))
	}
	base: u32
	if o.rel == 2 {
		base = load(b, t.t_i, t.v_a0)
	} else {
		base = extract(b, t.t_i, load(b, b.t_ivec[4], t.v_al), 0)
	}
	i := op1(b, .IAdd, t.t_i, base, const_i32(b, i32(o.reg)))
	idx := bitcast(b, t.t_u, ext(b, t.t_i, GLSL(45) /* SClamp */, i, const_i32(b, 0), const_i32(b, 255)))
	if t.abi6 {
		t.meta.module_meta.relative_hi_exclusive = 256
		return load_float_row(t, idx)
	}
	return load(b, t.t_v4, access(b, type_ptr(b, .Uniform, t.t_v4), t.u_float, const_u32(b, 0), idx))
}

@(private)
modifiers :: proc(t: ^T, v: u32, o: ^Operand) -> u32 {
	r := v
	if o.abs {r = ext(&t.b, t.t_v4, .FAbs, r)}
	if o.negate {r = op1(&t.b, .FNegate, t.t_v4, r)}
	return r
}

@(private)
operand :: proc(t: ^T, o: ^Operand) -> u32 {return modifiers(t, shuffle(t, load_operand_raw(t, o), o.swizzle), o)}

// scalar component k of the operand's swizzle (scalar ops: k 0 = `a`, 1 = `b`)
@(private)
operand_scalar :: proc(t: ^T, raw: u32, o: ^Operand, k: int) -> u32 {
	b := &t.b
	s := extract(b, t.t_f, raw, u32(o.swizzle[k]))
	if o.abs {s = ext(b, t.t_f, .FAbs, s)}
	if o.negate {s = op1(b, .FNegate, t.t_f, s)}
	return s
}

@(private)
splat4 :: proc(t: ^T, s: u32) -> u32 {return op1(&t.b, .CompositeConstruct, t.t_v4, s, s, s, s)}

// Xenos multiplication: 0 * anything (including inf / NaN) = 0
@(private)
xmul :: proc(t: ^T, n: int, a, x: u32, exact := true) -> u32 {
	b := &t.b
	ty := b.t_vec[n]
	prod := arith(b, .FMul, ty, a, x)
	if !exact {return prod}
	mn := ext(b, ty, .NMin, ext(b, ty, .FAbs, a), ext(b, ty, .FAbs, x))
	z := op1(b, .FOrdEqual, b.t_bvec[n], mn, const_splat(b, n, 0))
	return select(b, ty, z, const_splat(b, n, 0), prod)
}

@(private)
cmp_select :: proc(t: ^T, n: int, op: Op, x, y, if_true, if_false: u32) -> u32 {
	b := &t.b
	return select(b, b.t_vec[n], op1(b, op, b.t_bvec[n], x, y), if_true, if_false)
}

@(private)
demote :: proc(t: ^T, cond: u32) {
	if t.model != .Fragment {return}
	b := &t.b
	capability(b, 5379) // DemoteToHelperInvocation
	f := if_begin(b, cond)
	if t.p.memexport {memexport_flush(t)}
	op0(b, .DemoteToHelperInvocation)
	if_end(b, f)
}

// ---- control flow ----

@(private)
bool_constant :: proc(t: ^T, index: u32) -> u32 {
	b := &t.b
	if t.abi6 {t.meta.module_meta.bool_mask[index >> 5] |= 1 << (index & 31)}
	w := ubo_dword(t, t.u_bl, 0, index >> 5)
	bit := op1(b, .BitwiseAnd, t.t_u, w, const_u32(b, 1 << (index & 31)))
	return op1(b, .INotEqual, t.t_b, bit, const_u32(b, 0))
}

@(private)
loop_constant :: proc(t: ^T, id_: u32) -> u32 {
	if c, ok := t.int_defs[id_]; ok {return c}
	if t.abi6 {t.meta.module_meta.loop_mask |= 1 << id_}
	return ubo_dword(t, t.u_bl, 1, id_)
}

@(private)
condition :: proc(t: ^T, cf: ^Cf) -> u32 {
	b := &t.b
	#partial switch cf.op {
	case .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		c := bool_constant(t, cf.bool_index)
		return cf.condition ? c : op1(b, .LogicalNot, t.t_b, c)
	case .Cond_Exec_Pred, .Cond_Exec_Pred_End:
		p := load(b, t.t_b, t.v_pred)
		return cf.condition ? p : op1(b, .LogicalNot, t.t_b, p)
	case .Cond_Jmp, .Cond_Call:
		if cf.unconditional {return 0}
		if cf.predicated {
			p := load(b, t.t_b, t.v_pred)
			return cf.condition ? p : op1(b, .LogicalNot, t.t_b, p)
		}
		c := bool_constant(t, cf.bool_index)
		return cf.condition ? c : op1(b, .LogicalNot, t.t_b, c)
	}
	return 0
}

@(private)
goto_pc :: proc(t: ^T, target: u32) {
	b := &t.b
	store(b, t.v_pc, const_i32(b, i32(target)))
	branch(b, t.l_continue)
}

@(private)
end_program :: proc(t: ^T) {
	branch(&t.b, t.p.has_labels ? t.l_sw_merge : t.l_end)
}

@(private)
emit_cf :: proc(t: ^T, cf: ^Cf, ci: int) {
	b := &t.b
	p := t.p
	switch cf.op {
	case .Exec, .Exec_End, .Cond_Exec, .Cond_Exec_End, .Cond_Exec_Pred, .Cond_Exec_Pred_End, .Cond_Exec_Pred_Clean, .Cond_Exec_Pred_Clean_End:
		c := condition(t, cf)
		f: If
		if c != 0 {f = if_begin(b, c)}
		first := p.exec_first[ci]
		emit_instrs(t, first, int(cf.count))
		if t.err != "" {return}
		// a conditional *_end ends the program only when its condition holds (Xenia ProcessExecInstructionEnd)
		// (structured: nothing executable follows the end, so falling through ends the program)
		if cf_ends_shader(cf.op) && c != 0 && !t.structured {end_program(t)}
		if c != 0 {if_end(b, f)}
		if cf_ends_shader(cf.op) && c == 0 && !t.structured {end_program(t)}
	case .Loop_Start:
		if !p.has_labels {t.err = "loop without labels"; return}
		lc := loop_constant(t, cf.loop_id)
		count := op1(b, .BitwiseAnd, t.t_u, lc, const_u32(b, 0xff))
		old_lc := load(b, b.t_uvec[4], t.v_lc)
		store(b, t.v_lc, op1(b, .CompositeConstruct, b.t_uvec[4], count, extract(b, t.t_u, old_lc, 0), extract(b, t.t_u, old_lc, 1), extract(b, t.t_u, old_lc, 2)))
		old_al := load(b, b.t_ivec[4], t.v_al)
		start := bitcast(b, t.t_i, op1(b, .BitFieldUExtract, t.t_u, lc, const_u32(b, 8), const_u32(b, 8)))
		if cf.is_repeat {start = extract(b, t.t_i, old_al, 0)}
		store(b, t.v_al, op1(b, .CompositeConstruct, b.t_ivec[4], start, extract(b, t.t_i, old_al, 0), extract(b, t.t_i, old_al, 1), extract(b, t.t_i, old_al, 2)))
		skip := op1(b, .IEqual, t.t_b, count, const_u32(b, 0))
		f := if_begin(b, skip)
		// skipping the loop pops what was pushed (the loop_end that would pop is jumped over)
		store(b, t.v_lc, old_lc)
		store(b, t.v_al, old_al)
		goto_pc(t, cf.address)
		if_end(b, f)
	case .Loop_End:
		if !p.has_labels {t.err = "loop without labels"; return}
		old_lc := load(b, b.t_uvec[4], t.v_lc)
		count := op1(b, .ISub, t.t_u, extract(b, t.t_u, old_lc, 0), const_u32(b, 1))
		cont := op1(b, .INotEqual, t.t_b, count, const_u32(b, 0))
		if cf.pred_break {
			pv := load(b, t.t_b, t.v_pred)
			brk := cf.condition ? pv : op1(b, .LogicalNot, t.t_b, pv)
			cont = op1(b, .LogicalAnd, t.t_b, cont, op1(b, .LogicalNot, t.t_b, brk))
		}
		old_al := load(b, b.t_ivec[4], t.v_al)
		f := if_begin(b, cont)
		store(b, t.v_lc, op1(b, .CompositeInsert, b.t_uvec[4], count, old_lc, 0))
		step := op1(b, .BitFieldSExtract, t.t_i, bitcast(b, t.t_i, loop_constant(t, cf.loop_id)), const_u32(b, 16), const_u32(b, 8))
		store(b, t.v_al, op1(b, .CompositeInsert, b.t_ivec[4], op1(b, .IAdd, t.t_i, extract(b, t.t_i, old_al, 0), step), old_al, 0))
		goto_pc(t, cf.address)
		if_end(b, f)
		// leave: pop
		store(b, t.v_lc, op1(b, .CompositeConstruct, b.t_uvec[4], extract(b, t.t_u, old_lc, 1), extract(b, t.t_u, old_lc, 2), extract(b, t.t_u, old_lc, 3), const_u32(b, 0)))
		store(b, t.v_al, op1(b, .CompositeConstruct, b.t_ivec[4], extract(b, t.t_i, old_al, 1), extract(b, t.t_i, old_al, 2), extract(b, t.t_i, old_al, 3), const_i32(b, 0)))
	case .Cond_Jmp:
		c := condition(t, cf)
		if c == 0 {
			goto_pc(t, cf.address)
		} else {
			f := if_begin(b, c)
			goto_pc(t, cf.address)
			if_end(b, f)
		}
	case .Cond_Call, .Return:
		t.err = "call / return (not used by Halo 3)"
	case .Alloc:
		// every alloc commits the pending eM# (Xenia ProcessAllocInstruction); a new `alloc export` also starts with an
		// invalid eA so a stream that is never addressed writes nothing
		if p.memexport {
			memexport_flush(t)
			if cf.alloc_type == 3 {store(b, t.v_ea, const_splat(b, 4, 0))}
		}
	case .Nop, .Mark_Vs_Fetch_Done:
	}
}

// ---- instructions ----

@(private)
predicate_begin :: proc(t: ^T, predicated, cond: bool) -> (If, bool) {
	if !predicated {return {}, false}
	b := &t.b
	p := load(b, t.t_b, t.v_pred)
	return if_begin(b, cond ? p : op1(b, .LogicalNot, t.t_b, p)), true
}

// An instruction's predicate: whether it has one, and the p0 value it runs on.
@(private)
instr_predicate :: proc(in_: ^Instr) -> (predicated: bool, condition: bool) {
	switch in_.kind {
	case .Alu: return in_.alu.predicated, in_.alu.pred_condition
	case .Vfetch: return in_.vf.predicated, in_.vf.pred_condition
	case .Tfetch: return in_.tf.predicated, in_.tf.pred_condition
	}
	return false, false
}

// Whether an instruction may write p0 (setp*, vector or scalar).
@(private)
instr_writes_predicate :: proc(in_: ^Instr) -> bool {
	if in_.kind != .Alu {return false}
	a := &in_.alu
	if a.vec_op >= .Setp_Eq_Push && a.vec_op <= .Setp_Ge_Push {return true}
	#partial switch a.sca_op {
	case .Setp_Eq, .Setp_Ne, .Setp_Gt, .Setp_Ge, .Setp_Inv, .Setp_Pop, .Setp_Clr, .Setp_Rstr: return true
	}
	return false
}

@(private)
emit_instr_body :: proc(t: ^T, in_: ^Instr) {
	switch in_.kind {
	case .Alu: emit_alu(t, &in_.alu)
	case .Vfetch: emit_vfetch(t, in_)
	case .Tfetch: emit_tfetch(t, in_)
	}
}

// The instructions of an exec in order. A run of instructions predicated on the same p0 value shares one selection
// (p0 read once, as each would read it): the run ends after an instruction that may write p0, so every instruction
// still sees the value it would have seen.
@(private)
emit_instrs :: proc(t: ^T, first, count: int) {
	k := 0
	for k < count {
		in_ := &t.p.instrs[first + k]
		predicated, condition := instr_predicate(in_)
		if !predicated {
			emit_instr_body(t, in_)
			k += 1
		} else {
			f, _ := predicate_begin(t, true, condition)
			for k < count {
				run := &t.p.instrs[first + k]
				p2, c2 := instr_predicate(run)
				if !p2 || c2 != condition {break}
				emit_instr_body(t, run)
				k += 1
				if instr_writes_predicate(run) || t.err != "" {break}
			}
			if_end(&t.b, f)
		}
		if t.err != "" {return}
	}
}

// write `value` into the components of `mask` of the vec4 behind ptr
@(private)
store_masked :: proc(t: ^T, ptr: u32, value: u32, mask: u32) {
	b := &t.b
	if mask == 0 {return}
	if mask == 0xf {store(b, ptr, value); return}
	old := load(b, t.t_v4, ptr)
	sel: [4]u32
	for k in u32(0) ..< 4 {sel[k] = mask & (1 << k) != 0 ? 4 + k : k}
	store(b, ptr, op1(b, .VectorShuffle, t.t_v4, old, value, sel[0], sel[1], sel[2], sel[3]))
}

@(private)
saturate :: proc(t: ^T, v: u32) -> u32 {return ext(&t.b, t.t_v4, .NClamp, v, const_splat(&t.b, 4, 0), const_splat(&t.b, 4, 1))}

@(private)
emit_alu :: proc(t: ^T, a: ^Alu) {
	b := &t.b
	// all sources are read before any result is written (co-issue)
	nv := int(vec_op_operands[a.vec_op])
	vm := alu_vec_result_mask(a)
	sm := alu_sca_result_mask(a)
	c0 := alu_const0_mask(a)
	c1 := alu_const1_mask(a)
	vec_state := a.vec_op == .Max_A || (a.vec_op >= .Setp_Eq_Push && a.vec_op <= .Setp_Ge_Push) || (a.vec_op >= .Kill_Eq && a.vec_op <= .Kill_Ne)
	vres: u32
	if vm != 0 || vec_state {
		src: [3]u32
		for i in 0 ..< nv {src[i] = operand(t, &a.vec_src[i])}
		vres = vector_op(t, a, src)
		if vres != 0 && a.vec_clamp {vres = saturate(t, vres)}
	}
	sres: u32 // scalar result (float)
	if a.sca_op != .Retain_Prev {
		sres = scalar_op(t, a)
		if sres != 0 {
			store(b, t.v_ps, sres) // the previous scalar is the unclamped result (Xenia)
			if a.sca_clamp {sres = ext(b, t.t_f, .NClamp, sres, const_f32(b, 0), const_f32(b, 1))}
		}
	} else if sm != 0 {
		sres = load(b, t.t_f, t.v_ps)
		if a.sca_clamp {sres = ext(b, t.t_f, .NClamp, sres, const_f32(b, 0), const_f32(b, 1))}
	}
	if t.err != "" {return}
	if a.is_export {
		// the float constant holding a memexport stream constant (mad eA, r#, c_0100, c#: its last constant source),
		// for the device's XeSystem.memexport table
		if a.vec_dest == EXPORT_ADDRESS && t.p.memexport {
			for i := nv - 1; i >= 0; i -= 1 {
				o := &a.vec_src[i]
				if o.is_temp || o.rel != 0 {continue}
				found := false
				for r in t.meta.memexport_streams {if r == o.reg {found = true}}
				if !found {append(&t.meta.memexport_streams, o.reg)}
				break
			}
		}
		ptr := export_ptr(t, a.vec_dest)
		if ptr == 0 {return}
		if vres != 0 {store_masked(t, ptr, vres, vm)}
		if sres != 0 && sm != 0 {store_masked(t, ptr, splat4(t, sres), sm)}
		if c0 != 0 {store_masked(t, ptr, const_splat(b, 4, 0), c0)}
		if c1 != 0 {store_masked(t, ptr, const_splat(b, 4, 1), c1)}
		if a.vec_dest >= EXPORT_DATA0 && a.vec_dest <= EXPORT_DATA0 + 4 && t.p.memexport {
			m := load(b, t.t_u, t.v_em_mask)
			store(b, t.v_em_mask, op1(b, .BitwiseOr, t.t_u, m, const_u32(b, 1 << (a.vec_dest - EXPORT_DATA0))))
		}
		return
	}
	if vres != 0 && vm != 0 {store_masked(t, reg_ptr(t, a.vec_dest, a.vec_dest_rel), vres, vm)}
	if sres != 0 && sm != 0 {store_masked(t, reg_ptr(t, a.sca_dest, a.sca_dest_rel), splat4(t, sres), sm)}
}

@(private)
export_ptr :: proc(t: ^T, reg: u32) -> u32 {
	b := &t.b
	switch {
	case reg == EXPORT_ADDRESS:
		return t.p.memexport ? t.v_ea : 0
	case reg >= EXPORT_DATA0 && reg <= EXPORT_DATA0 + 4:
		return t.p.memexport ? access(b, t.t_pf_v4, t.v_em, const_u32(b, reg - EXPORT_DATA0)) : 0
	}
	switch t.model {
	case .Vertex, .Tess_Eval:
		if reg < 16 {return access(b, t.t_pf_v4, t.v_o, const_u32(b, reg))}
		if reg == EXPORT_VS_POSITION {return t.v_opos}
		if reg == EXPORT_VS_POINT_SIZE {t.meta.point_size = true; return t.v_opts}
	case .Fragment:
		if reg < 4 {t.meta.color_outputs |= 1 << reg; return access(b, t.t_pf_v4, t.v_oc, const_u32(b, reg))}
		if reg == EXPORT_PS_DEPTH {t.meta.depth_output = true; return t.v_odepth}
	case .Compute:
	}
	return 0
}

@(private)
vector_op :: proc(t: ^T, a: ^Alu, s: [3]u32) -> u32 {
	b := &t.b
	v4 := t.t_v4
	one4 := const_splat(b, 4, 1)
	zero4 := const_splat(b, 4, 0)
	switch a.vec_op {
	case .Add: return arith(b, .FAdd, v4, s[0], s[1])
	case .Mul: return xmul(t, 4, s[0], s[1], zero_rule(t.meta.defs[:], &a.vec_src[0], &a.vec_src[1]))
	case .Mad: return arith(b, .FAdd, v4, xmul(t, 4, s[0], s[1], zero_rule(t.meta.defs[:], &a.vec_src[0], &a.vec_src[1])), s[2])
	case .Max: return cmp_select(t, 4, .FOrdGreaterThanEqual, s[0], s[1], s[0], s[1])
	case .Min: return cmp_select(t, 4, .FOrdLessThan, s[0], s[1], s[0], s[1])
	case .Seq: return cmp_select(t, 4, .FOrdEqual, s[0], s[1], one4, zero4)
	case .Sgt: return cmp_select(t, 4, .FOrdGreaterThan, s[0], s[1], one4, zero4)
	case .Sge: return cmp_select(t, 4, .FOrdGreaterThanEqual, s[0], s[1], one4, zero4)
	case .Sne: return cmp_select(t, 4, .FUnordNotEqual, s[0], s[1], one4, zero4)
	case .Frc: return ext(b, v4, .Fract, s[0])
	case .Trunc: return ext(b, v4, .Trunc, s[0])
	case .Floor: return ext(b, v4, .Floor, s[0])
	case .Cnd_Eq: return cmp_select(t, 4, .FOrdEqual, s[0], zero4, s[1], s[2])
	case .Cnd_Ge: return cmp_select(t, 4, .FOrdGreaterThanEqual, s[0], zero4, s[1], s[2])
	case .Cnd_Gt: return cmp_select(t, 4, .FOrdGreaterThan, s[0], zero4, s[1], s[2])
	case .Dp4, .Dp3, .Dp2_Add:
		n := a.vec_op == .Dp4 ? 4 : (a.vec_op == .Dp3 ? 3 : 2)
		prod := xmul(t, 4, s[0], s[1], zero_rule(t.meta.defs[:], &a.vec_src[0], &a.vec_src[1]))
		sum := extract(b, t.t_f, prod, 0)
		for k in 1 ..< n {sum = arith(b, .FAdd, t.t_f, sum, extract(b, t.t_f, prod, u32(k)))}
		if a.vec_op == .Dp2_Add {sum = arith(b, .FAdd, t.t_f, sum, extract(b, t.t_f, s[2], 0))}
		return splat4(t, sum)
	case .Cube:
		// operand 0: x = src.z, y = src.w, z = src.x (Xenia ProcessVectorAluOperation kCube)
		f := t.t_f
		x := extract(b, f, s[0], 2)
		y := extract(b, f, s[0], 3)
		z := extract(b, f, s[0], 0)
		ax, ay, az := ext(b, f, .FAbs, x), ext(b, f, .FAbs, y), ext(b, f, .FAbs, z)
		nx, ny, nz := op1(b, .FNegate, f, x), op1(b, .FNegate, f, y), op1(b, .FNegate, f, z)
		zf := const_f32(b, 0)
		is_z := op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdGreaterThanEqual, t.t_b, az, ax), op1(b, .FOrdGreaterThanEqual, t.t_b, az, ay))
		is_y := op1(b, .FOrdGreaterThanEqual, t.t_b, ay, ax)
		zneg := op1(b, .FOrdLessThan, t.t_b, z, zf)
		yneg := op1(b, .FOrdLessThan, t.t_b, y, zf)
		xneg := op1(b, .FOrdLessThan, t.t_b, x, zf)
		// z major: (-y, z<0 ? -x : x, z, z<0 ? 5 : 4)
		zr := [4]u32{ny, select(b, f, zneg, nx, x), z, select(b, f, zneg, const_f32(b, 5), const_f32(b, 4))}
		// y major: (y<0 ? -z : z, x, y, y<0 ? 3 : 2)
		yr := [4]u32{select(b, f, yneg, nz, z), x, y, select(b, f, yneg, const_f32(b, 3), const_f32(b, 2))}
		// x major: (-y, x<0 ? z : -z, x, x<0 ? 1 : 0)
		xr := [4]u32{ny, select(b, f, xneg, z, nz), x, select(b, f, xneg, const_f32(b, 1), zf)}
		r: [4]u32
		for k in 0 ..< 4 {r[k] = select(b, f, is_z, zr[k], select(b, f, is_y, yr[k], xr[k]))}
		r[2] = arith(b, .FMul, f, r[2], const_f32(b, 2))
		return op1(b, .CompositeConstruct, v4, r[0], r[1], r[2], r[3])
	case .Max4:
		m := extract(b, t.t_f, s[0], 0)
		for k in u32(1) ..< 4 {m = ext(b, t.t_f, .NMax, m, extract(b, t.t_f, s[0], k))}
		return splat4(t, m)
	case .Setp_Eq_Push, .Setp_Ne_Push, .Setp_Gt_Push, .Setp_Ge_Push:
		ops := [4]Op{.FOrdEqual, .FUnordNotEqual, .FOrdGreaterThan, .FOrdGreaterThanEqual}
		op := ops[int(a.vec_op) - int(Vec_Op.Setp_Eq_Push)]
		zf := const_f32(b, 0)
		aw, bw := extract(b, t.t_f, s[0], 3), extract(b, t.t_f, s[1], 3)
		store(b, t.v_pred, op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdEqual, t.t_b, aw, zf), op1(b, op, t.t_b, bw, zf)))
		ax, bx := extract(b, t.t_f, s[0], 0), extract(b, t.t_f, s[1], 0)
		c := op1(b, .LogicalAnd, t.t_b, op1(b, .FOrdEqual, t.t_b, ax, zf), op1(b, op, t.t_b, bx, zf))
		return splat4(t, arith(b, .FAdd, t.t_f, select(b, t.t_f, c, const_f32(b, -1), ax), const_f32(b, 1)))
	case .Kill_Eq, .Kill_Gt, .Kill_Ge, .Kill_Ne:
		ops := [4]Op{.FOrdEqual, .FOrdGreaterThan, .FOrdGreaterThanEqual, .FUnordNotEqual}
		c := op1(b, .Any, t.t_b, op1(b, ops[int(a.vec_op) - int(Vec_Op.Kill_Eq)], b.t_bvec[4], s[0], s[1]))
		demote(t, c)
		return splat4(t, select(b, t.t_f, c, const_f32(b, 1), const_f32(b, 0)))
	case .Dst:
		y := xmul(t, 1, extract(b, t.t_f, s[0], 1), extract(b, t.t_f, s[1], 1))
		return op1(b, .CompositeConstruct, v4, const_f32(b, 1), y, extract(b, t.t_f, s[0], 2), extract(b, t.t_f, s[1], 3))
	case .Max_A:
		w := extract(b, t.t_f, s[0], 3)
		fl := ext(b, t.t_f, .Floor, arith(b, .FAdd, t.t_f, w, const_f32(b, 0.5)))
		store(b, t.v_a0, op1(b, .ConvertFToS, t.t_i, ext(b, t.t_f, .NClamp, fl, const_f32(b, -256), const_f32(b, 255))))
		return cmp_select(t, 4, .FOrdGreaterThanEqual, s[0], s[1], s[0], s[1])
	case .Op30, .Op31:
		return 0
	}
	return 0
}

@(private)
scalar_op :: proc(t: ^T, a: ^Alu) -> u32 {
	b := &t.b
	f := t.t_f
	av, bv: u32
	if a.sca_src_count == 1 {
		raw := load_operand_raw(t, &a.sca_src[0])
		av = operand_scalar(t, raw, &a.sca_src[0], 0)
		if sca_op_operands[a.sca_op] == 2 {bv = operand_scalar(t, raw, &a.sca_src[0], 1)}
	} else if a.sca_src_count == 2 {
		av = operand_scalar(t, load_operand_raw(t, &a.sca_src[0]), &a.sca_src[0], 0)
		bv = operand_scalar(t, load_operand_raw(t, &a.sca_src[1]), &a.sca_src[1], 0)
	}
	zf, onef := const_f32(b, 0), const_f32(b, 1)
	ps :: proc(t: ^T) -> u32 {return load(&t.b, t.t_f, t.v_ps)}
	sel :: proc(t: ^T, op: Op, x, y, tv, fv: u32) -> u32 {return select(&t.b, t.t_f, op1(&t.b, op, t.t_b, x, y), tv, fv)}
	FLT_MAX :: f32(3.4028234663852886e38)
	INF := transmute(f32)u32(0x7f800000)
	switch a.sca_op {
	case .Adds: return arith(b, .FAdd, f, av, bv)
	case .Adds_Prev: return arith(b, .FAdd, f, av, ps(t))
	case .Muls: return xmul(t, 1, av, bv)
	case .Muls_Prev: return xmul(t, 1, av, ps(t))
	case .Muls_Prev2:
		// Xenia: -FLT_MAX unless ps != -FLT_MAX, ps and b finite, and b > 0; then a * ps
		pv := ps(t)
		nmax := const_f32(b, -FLT_MAX)
		finite :: proc(t: ^T, x, nmax: u32) -> u32 {return op1(&t.b, .FOrdGreaterThanEqual, t.t_b, op1(&t.b, .FNegate, t.t_f, ext(&t.b, t.t_f, .FAbs, x)), nmax)}
		c := op1(b, .LogicalAnd, t.t_b, op1(b, .FUnordNotEqual, t.t_b, pv, nmax), finite(t, pv, nmax))
		c = op1(b, .LogicalAnd, t.t_b, c, op1(b, .LogicalAnd, t.t_b, finite(t, bv, nmax), op1(b, .FOrdGreaterThan, t.t_b, bv, zf)))
		x := a.sca_src[0].swizzle[0] == a.sca_src[0].swizzle[1] ? bv : av
		return select(b, f, c, xmul(t, 1, x, pv), nmax)
	case .Maxs: return sel(t, .FOrdGreaterThanEqual, av, bv, av, bv)
	case .Mins: return sel(t, .FOrdLessThan, av, bv, av, bv)
	case .Seqs: return sel(t, .FOrdEqual, av, zf, onef, zf)
	case .Sgts: return sel(t, .FOrdGreaterThan, av, zf, onef, zf)
	case .Sges: return sel(t, .FOrdGreaterThanEqual, av, zf, onef, zf)
	case .Snes: return sel(t, .FUnordNotEqual, av, zf, onef, zf)
	case .Frcs: return ext(b, f, .Fract, av)
	case .Truncs: return ext(b, f, .Trunc, av)
	case .Floors: return ext(b, f, .Floor, av)
	case .Exp: return ext(b, f, .Exp2, av)
	case .Log: return ext(b, f, .Log2, av)
	case .Logc:
		r := ext(b, f, .Log2, av)
		return sel(t, .FOrdEqual, r, const_f32(b, -INF), const_f32(b, -FLT_MAX), r)
	case .Rcp: return arith(b, .FDiv, f, onef, av)
	case .Rcpc, .Rsqc:
		r := a.sca_op == .Rcpc ? arith(b, .FDiv, f, onef, av) : ext(b, f, .InverseSqrt, av)
		r = sel(t, .FOrdEqual, r, const_f32(b, -INF), const_f32(b, -FLT_MAX), r)
		return sel(t, .FOrdEqual, r, const_f32(b, INF), const_f32(b, FLT_MAX), r)
	case .Rcpf, .Rsqf:
		r := a.sca_op == .Rcpf ? arith(b, .FDiv, f, onef, av) : ext(b, f, .InverseSqrt, av)
		r = sel(t, .FOrdEqual, r, const_f32(b, INF), zf, r)
		return sel(t, .FOrdEqual, r, const_f32(b, -INF), const_f32(b, -0.0), r)
	case .Rsq: return ext(b, f, .InverseSqrt, av)
	case .Max_As, .Max_Asf:
		addr := a.sca_op == .Max_As ? arith(b, .FAdd, f, av, const_f32(b, 0.5)) : av
		store(b, t.v_a0, op1(b, .ConvertFToS, t.t_i, ext(b, f, .NClamp, ext(b, f, .Floor, addr), const_f32(b, 0), const_f32(b, 255))))
		return sel(t, .FOrdGreaterThanEqual, av, bv, av, bv)
	case .Subs: return arith(b, .FSub, f, av, bv)
	case .Subs_Prev: return arith(b, .FSub, f, av, ps(t))
	case .Setp_Eq, .Setp_Ne, .Setp_Gt, .Setp_Ge:
		ops := [4]Op{.FOrdEqual, .FUnordNotEqual, .FOrdGreaterThan, .FOrdGreaterThanEqual}
		p := op1(b, ops[int(a.sca_op) - int(Sca_Op.Setp_Eq)], t.t_b, av, zf)
		store(b, t.v_pred, p)
		return select(b, f, p, zf, onef)
	case .Setp_Inv:
		p := op1(b, .FOrdEqual, t.t_b, av, onef)
		store(b, t.v_pred, p)
		return select(b, f, p, zf, sel(t, .FOrdEqual, av, zf, onef, av))
	case .Setp_Pop:
		m := arith(b, .FSub, f, av, onef)
		p := op1(b, .FOrdLessThanEqual, t.t_b, m, zf)
		store(b, t.v_pred, p)
		return select(b, f, p, zf, m)
	case .Setp_Clr:
		store(b, t.v_pred, const_bool(b, false))
		return const_f32(b, FLT_MAX)
	case .Setp_Rstr:
		p := op1(b, .FOrdEqual, t.t_b, av, zf)
		store(b, t.v_pred, p)
		return select(b, f, p, zf, av)
	case .Kills_Eq, .Kills_Gt, .Kills_Ge, .Kills_Ne, .Kills_One:
		ops := [5]Op{.FOrdEqual, .FOrdGreaterThan, .FOrdGreaterThanEqual, .FUnordNotEqual, .FOrdEqual}
		c := op1(b, ops[int(a.sca_op) - int(Sca_Op.Kills_Eq)], t.t_b, av, a.sca_op == .Kills_One ? onef : zf)
		demote(t, c)
		return select(b, f, c, onef, zf)
	case .Sqrt: return ext(b, f, .Sqrt, av)
	case .Mulsc0, .Mulsc1: return xmul(t, 1, av, bv)
	case .Addsc0, .Addsc1: return arith(b, .FAdd, f, av, bv)
	case .Subsc0, .Subsc1: return arith(b, .FSub, f, av, bv)
	case .Sin: return ext(b, f, .Sin, av)
	case .Cos: return ext(b, f, .Cos, av)
	case .Retain_Prev: return 0
	case .Op41, .Op51, .Op52, .Op53, .Op54, .Op55, .Op56, .Op57, .Op58, .Op59, .Op60, .Op61, .Op62, .Op63:
		return zf
	}
	return 0
}

// fetch destination swizzle: 0-3 component, 4 zero, 5 one, 6 (invalid) zero, 7 keep
@(private)
store_fetch_result :: proc(t: ^T, dst: u32, rel: bool, dst_swz: u32, value: u32) {
	b := &t.b
	mask: u32
	comps: [4]u32
	for k in u32(0) ..< 4 {
		s := fetch_dst_component(dst_swz, k)
		switch s {
		case 0 ..= 3: comps[k] = extract(b, t.t_f, value, s); mask |= 1 << k
		case 5: comps[k] = const_f32(b, 1); mask |= 1 << k
		case 7: comps[k] = const_f32(b, 0)
		case: comps[k] = const_f32(b, 0); mask |= 1 << k
		}
	}
	if mask == 0 {return}
	v := op1(b, .CompositeConstruct, t.t_v4, comps[0], comps[1], comps[2], comps[3])
	store_masked(t, reg_ptr(t, dst, rel), v, mask)
}

// One-call entry: the D3D shader object (+ container virtual part) and the physical microcode as stored in a tag.
translate_shader :: proc(object: []u8, physical: []u8, stage: Stage, allocator := context.allocator, abi: u32 = ABI_VERSION) -> Result {
	c, perr := container_parse(object, physical, stage, allocator)
	if perr != .None {return Result{error = "container"}}
	defer container_destroy(&c)
	p, ok := program_build(&c, allocator)
	defer program_destroy(&p)
	if !ok {return Result{error = p.error}}
	return translate(&c, &p, allocator, abi)
}

// Whether a product of these operands needs the Xenos zero rule: not when both are the same value (x * x is 0 only
// for x = 0, and inf * inf = inf either way), nor when one is a literal constant whose used components are finite
// and non-zero (then IEEE and Xenos agree but for the sign of a zero result). Shared by both backends.
@(private)
zero_rule :: proc(defs: []Float_Def, x, y: ^Operand) -> bool {
	if x^ == y^ {return false}
	lit :: proc(defs: []Float_Def, o: ^Operand) -> bool {
		if o.is_temp || o.rel != 0 {return false}
		for d in defs {
			if d.reg != o.reg {continue}
			for k in 0 ..< 4 {
				bits := d.value[o.swizzle[k]] & 0x7fffffff
				if bits == 0 || bits >= 0x7f800000 {return false}
			}
			return true
		}
		return false
	}
	return !lit(defs, x) && !lit(defs, y)
}
