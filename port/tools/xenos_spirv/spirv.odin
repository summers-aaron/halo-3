package xenos

// Minimal SPIR-V 1.6 module builder: logical sections, type / constant deduplication, a linear function body with
// a separate variable block for the entry block (OpVariable Function must come first).

Op :: enum u16 {
	Nop = 0, Name = 5, MemberName = 6, Extension = 10, ExtInstImport = 11, ExtInst = 12, MemoryModel = 14, EntryPoint = 15,
	ExecutionMode = 16, Capability = 17, TypeVoid = 19, TypeBool = 20, TypeInt = 21, TypeFloat = 22, TypeVector = 23,
	TypeImage = 25, TypeSampler = 26, TypeSampledImage = 27, TypeArray = 28, TypeRuntimeArray = 29, TypeStruct = 30,
	TypePointer = 32, TypeFunction = 33, ConstantTrue = 41, ConstantFalse = 42, Constant = 43, ConstantComposite = 44,
	SpecConstantTrue = 48, SpecConstantFalse = 49, SpecConstant = 50, SpecConstantOp = 52, Function = 54, FunctionEnd = 56, Variable = 59, Load = 61, Store = 62,
	AccessChain = 65, Decorate = 71, MemberDecorate = 72, VectorExtractDynamic = 77, VectorShuffle = 79, CompositeConstruct = 80, CompositeExtract = 81,
	CompositeInsert = 82, SampledImage = 86, ImageSampleImplicitLod = 87, ImageSampleExplicitLod = 88,
	ImageQuerySizeLod = 103, ImageQueryLod = 105, ConvertFToU = 109, ConvertFToS = 110, ConvertSToF = 111,
	ConvertUToF = 112, Bitcast = 124, SNegate = 126, FNegate = 127, IAdd = 128, FAdd = 129, ISub = 130, FSub = 131,
	IMul = 132, FMul = 133, UDiv = 134, FDiv = 136, UMod = 137, VectorTimesScalar = 142, Dot = 148, IAddCarry = 149, Any = 154, All = 155,
	IsNan = 156, LogicalOr = 166, LogicalAnd = 167, LogicalNot = 168, Select = 169, IEqual = 170, INotEqual = 171,
	UGreaterThan = 172, SGreaterThan = 173, UGreaterThanEqual = 174, SGreaterThanEqual = 175, ULessThan = 176,
	SLessThan = 177, ULessThanEqual = 178, SLessThanEqual = 179, FOrdEqual = 180, FUnordNotEqual = 183,
	FOrdLessThan = 184, FOrdGreaterThan = 186, FOrdLessThanEqual = 188, FOrdGreaterThanEqual = 190,
	ShiftRightLogical = 194, ShiftRightArithmetic = 195, ShiftLeftLogical = 196, BitwiseOr = 197, BitwiseXor = 198,
	BitwiseAnd = 199, Not = 200, BitFieldInsert = 201, BitFieldSExtract = 202, BitFieldUExtract = 203, DPdx = 207,
	DPdy = 208, Fwidth = 209, EmitVertex = 218, EndPrimitive = 219, Phi = 245, LoopMerge = 246, SelectionMerge = 247,
	Label = 248, Branch = 249,
	BranchConditional = 250, Switch = 251, Return = 253, ReturnValue = 254, Unreachable = 255, FunctionParameter = 55, FunctionCall = 57, DemoteToHelperInvocation = 5380,
}

// GLSL.std.450
GLSL :: enum u32 {
	Trunc = 3, FAbs = 4, Floor = 8, Fract = 10, Sin = 13, Cos = 14, Exp2 = 29, Log2 = 30, Sqrt = 31, InverseSqrt = 32,
	FMin = 37, FMax = 40, FClamp = 43, NMin = 79, NMax = 80, NClamp = 81, PackHalf2x16 = 58, UnpackHalf2x16 = 62,
	Ldexp = 53, Length = 66, Normalize = 69,
}

Storage_Class :: enum u32 {
	UniformConstant = 0, Input = 1, Uniform = 2, Output = 3, Private = 6, Function = 7, PushConstant = 9,
	StorageBuffer = 12, PhysicalStorageBuffer = 5349,
}

Decoration :: enum u32 {
	SpecId = 1, Block = 2, ArrayStride = 6, BuiltIn = 11, NoPerspective = 13, Flat = 14, Centroid = 16,
	Invariant = 18, Restrict = 19, Aliased = 20, NonWritable = 24, NonReadable = 25, Location = 30, Binding = 33,
	DescriptorSet = 34, Offset = 35, NoContraction = 42,
}

BuiltIn :: enum u32 {
	Position = 0, PointSize = 1, VertexIndex = 42, InstanceIndex = 43, FragCoord = 15, PointCoord = 16,
	FrontFacing = 17, FragDepth = 22, GlobalInvocationId = 28,
}

Type_Key :: struct {
	op: Op,
	a:  [8]u32,
}

Const_Key :: struct {
	type:  u32,
	n:     u32,
	v:     [4]u32,
	spec:  bool,
}

Builder :: struct {
	bound:        u32,
	capabilities: [dynamic]u32,
	extensions:   [dynamic]u32,
	imports:      [dynamic]u32,
	memory_model: [dynamic]u32,
	entry_points: [dynamic]u32,
	exec_modes:   [dynamic]u32,
	debug:        [dynamic]u32,
	annotations:  [dynamic]u32,
	globals:      [dynamic]u32, // types, constants, global variables
	fn_vars:      [dynamic]u32,
	body:         [dynamic]u32,
	funcs:        [dynamic]u32, // helper functions, after main
	types:        map[Type_Key]u32,
	consts:       map[Const_Key]u32,
	glsl:         u32,
	block_open:   bool, // the current block has not been terminated
	// frequently used ids
	t_void, t_bool, t_int, t_uint, t_float: u32,
	t_vec:        [5]u32, // float vectors 1..4 (index 1 = float)
	t_uvec:       [5]u32,
	t_ivec:       [5]u32,
	t_bvec:       [5]u32,
}

id :: proc(b: ^Builder) -> u32 {
	b.bound += 1
	return b.bound
}

emit :: proc(s: ^[dynamic]u32, op: Op, args: ..u32) {
	append(s, u32(len(args) + 1) << 16 | u32(op))
	append(s, ..args)
}

// an instruction with fixed operands followed by a variable list
emitv :: proc(s: ^[dynamic]u32, op: Op, head: []u32, tail: []u32) {
	append(s, u32(len(head) + len(tail) + 1) << 16 | u32(op))
	append(s, ..head)
	append(s, ..tail)
}

// string literal words
str_words :: proc(s: string) -> [dynamic]u32 {
	out := make([dynamic]u32, context.temp_allocator)
	n := len(s) / 4 + 1
	for i in 0 ..< n {
		w: u32
		for k in 0 ..< 4 {
			j := 4 * i + k
			if j < len(s) {w |= u32(s[j]) << u32(8 * k)}
		}
		append(&out, w)
	}
	return out
}

builder_init :: proc(b: ^Builder, allocator := context.allocator) {
	context.allocator = allocator
	b.types = make(map[Type_Key]u32)
	b.consts = make(map[Const_Key]u32)
	b.t_void = type_get(b, .TypeVoid)
	b.t_bool = type_get(b, .TypeBool)
	b.t_int = type_get(b, .TypeInt, 32, 1)
	b.t_uint = type_get(b, .TypeInt, 32, 0)
	b.t_float = type_get(b, .TypeFloat, 32)
	b.t_vec[1], b.t_uvec[1], b.t_ivec[1], b.t_bvec[1] = b.t_float, b.t_uint, b.t_int, b.t_bool
	for n in u32(2) ..= 4 {
		b.t_vec[n] = type_get(b, .TypeVector, b.t_float, n)
		b.t_uvec[n] = type_get(b, .TypeVector, b.t_uint, n)
		b.t_ivec[n] = type_get(b, .TypeVector, b.t_int, n)
		b.t_bvec[n] = type_get(b, .TypeVector, b.t_bool, n)
	}
	b.glsl = id(b)
	ws := str_words("GLSL.std.450")
	emitv(&b.imports, .ExtInstImport, {b.glsl}, ws[:])
}

builder_destroy :: proc(b: ^Builder) {
	delete(b.capabilities); delete(b.extensions); delete(b.imports); delete(b.memory_model); delete(b.entry_points)
	delete(b.exec_modes); delete(b.debug); delete(b.annotations); delete(b.globals); delete(b.fn_vars); delete(b.body); delete(b.funcs)
	delete(b.types); delete(b.consts)
}

type_get :: proc(b: ^Builder, op: Op, args: ..u32) -> u32 {
	k := Type_Key{op = op}
	for a, i in args {k.a[i] = a}
	if v, ok := b.types[k]; ok {return v}
	r := id(b)
	emitv(&b.globals, op, {r}, args)
	b.types[k] = r
	return r
}

type_ptr :: proc(b: ^Builder, sc: Storage_Class, t: u32) -> u32 {return type_get(b, .TypePointer, u32(sc), t)}

type_array :: proc(b: ^Builder, elem: u32, n: u32, stride: u32 = 0) -> u32 {
	k := Type_Key{op = .TypeArray, a = {elem, n, stride, 0, 0, 0, 0, 0}}
	if v, ok := b.types[k]; ok {return v}
	r := id(b)
	emit(&b.globals, .TypeArray, r, elem, const_u32(b, n))
	if stride != 0 {emit(&b.annotations, .Decorate, r, u32(Decoration.ArrayStride), stride)}
	b.types[k] = r
	return r
}

// a new (never deduplicated) struct type
type_struct :: proc(b: ^Builder, members: ..u32) -> u32 {
	r := id(b)
	emitv(&b.globals, .TypeStruct, {r}, members)
	return r
}

type_vec :: proc(b: ^Builder, n: int) -> u32 {return b.t_vec[n]}

@(private)
const_scalar :: proc(b: ^Builder, t: u32, v: u32) -> u32 {
	k := Const_Key{type = t, n = 1, v = {v, 0, 0, 0}}
	if c, ok := b.consts[k]; ok {return c}
	r := id(b)
	emit(&b.globals, .Constant, t, r, v)
	b.consts[k] = r
	return r
}

const_u32 :: proc(b: ^Builder, v: u32) -> u32 {return const_scalar(b, b.t_uint, v)}
const_i32 :: proc(b: ^Builder, v: i32) -> u32 {return const_scalar(b, b.t_int, transmute(u32)v)}
const_f32 :: proc(b: ^Builder, v: f32) -> u32 {return const_scalar(b, b.t_float, transmute(u32)v)}
const_f32_bits :: proc(b: ^Builder, v: u32) -> u32 {return const_scalar(b, b.t_float, v)}

const_bool :: proc(b: ^Builder, v: bool) -> u32 {
	k := Const_Key{type = b.t_bool, n = 1, v = {v ? 1 : 0, 0, 0, 0}}
	if c, ok := b.consts[k]; ok {return c}
	r := id(b)
	emit(&b.globals, v ? .ConstantTrue : .ConstantFalse, b.t_bool, r)
	b.consts[k] = r
	return r
}

const_composite :: proc(b: ^Builder, t: u32, parts: ..u32) -> u32 {
	if len(parts) > 4 {
		r := id(b)
		emitv(&b.globals, .ConstantComposite, {t, r}, parts)
		return r
	}
	k := Const_Key{type = t, n = u32(len(parts)) | 0x100}
	for p, i in parts {k.v[i] = p}
	if c, ok := b.consts[k]; ok {return c}
	r := id(b)
	emitv(&b.globals, .ConstantComposite, {t, r}, parts)
	b.consts[k] = r
	return r
}

const_vec4 :: proc(b: ^Builder, x, y, z, w: f32) -> u32 {
	return const_composite(b, b.t_vec[4], const_f32(b, x), const_f32(b, y), const_f32(b, z), const_f32(b, w))
}
const_vec4_bits :: proc(b: ^Builder, v: [4]u32) -> u32 {
	return const_composite(b, b.t_vec[4], const_f32_bits(b, v[0]), const_f32_bits(b, v[1]), const_f32_bits(b, v[2]), const_f32_bits(b, v[3]))
}
const_splat :: proc(b: ^Builder, n: int, v: f32) -> u32 {
	if n == 1 {return const_f32(b, v)}
	c := const_f32(b, v)
	parts: [4]u32 = {c, c, c, c}
	return const_composite(b, b.t_vec[n], ..parts[:n])
}
const_usplat :: proc(b: ^Builder, n: int, v: u32) -> u32 {
	if n == 1 {return const_u32(b, v)}
	c := const_u32(b, v)
	parts: [4]u32 = {c, c, c, c}
	return const_composite(b, b.t_uvec[n], ..parts[:n])
}

// a specialization constant with its SpecId
spec_u32 :: proc(b: ^Builder, spec_id: u32, default_value: u32) -> u32 {
	r := id(b)
	emit(&b.globals, .SpecConstant, b.t_uint, r, default_value)
	emit(&b.annotations, .Decorate, r, u32(Decoration.SpecId), spec_id)
	return r
}

spec_bool :: proc(b: ^Builder, spec_id: u32, default_value: bool) -> u32 {
	r := id(b)
	emit(&b.globals, default_value ? .SpecConstantTrue : .SpecConstantFalse, b.t_bool, r)
	emit(&b.annotations, .Decorate, r, u32(Decoration.SpecId), spec_id)
	return r
}

capability :: proc(b: ^Builder, c: u32) {
	for i := 0; i < len(b.capabilities); i += 2 {if b.capabilities[i + 1] == c {return}}
	emit(&b.capabilities, .Capability, c)
}

decorate :: proc(b: ^Builder, target: u32, d: Decoration, args: ..u32) {
	emitv(&b.annotations, .Decorate, {target, u32(d)}, args)
}

member_decorate :: proc(b: ^Builder, t: u32, member: u32, d: Decoration, args: ..u32) {
	emitv(&b.annotations, .MemberDecorate, {t, member, u32(d)}, args)
}

name :: proc(b: ^Builder, target: u32, s: string) {
	ws := str_words(s)
	append(&b.debug, u32(2 + len(ws)) << 16 | u32(Op.Name), target)
	append(&b.debug, ..ws[:])
}

member_name :: proc(b: ^Builder, t: u32, member: u32, s: string) {
	ws := str_words(s)
	append(&b.debug, u32(3 + len(ws)) << 16 | u32(Op.MemberName), t, member)
	append(&b.debug, ..ws[:])
}

global_var :: proc(b: ^Builder, sc: Storage_Class, t: u32) -> u32 {
	r := id(b)
	emit(&b.globals, .Variable, type_ptr(b, sc, t), r, u32(sc))
	return r
}

global_var_init :: proc(b: ^Builder, sc: Storage_Class, t: u32, init: u32) -> u32 {
	r := id(b)
	emit(&b.globals, .Variable, type_ptr(b, sc, t), r, u32(sc), init)
	return r
}

local_var :: proc(b: ^Builder, t: u32, init: u32 = 0) -> u32 {
	r := id(b)
	if init != 0 {
		emit(&b.fn_vars, .Variable, type_ptr(b, .Function, t), r, u32(Storage_Class.Function), init)
	} else {
		emit(&b.fn_vars, .Variable, type_ptr(b, .Function, t), r, u32(Storage_Class.Function))
	}
	return r
}

// ---- function body ----

op1 :: proc(b: ^Builder, op: Op, t: u32, args: ..u32) -> u32 {
	r := id(b)
	append(&b.body, u32(len(args) + 3) << 16 | u32(op), t, r)
	append(&b.body, ..args)
	return r
}

op0 :: proc(b: ^Builder, op: Op, args: ..u32) {
	append(&b.body, u32(len(args) + 1) << 16 | u32(op))
	append(&b.body, ..args)
}

ext :: proc(b: ^Builder, t: u32, f: GLSL, args: ..u32) -> u32 {
	r := id(b)
	append(&b.body, u32(len(args) + 5) << 16 | u32(Op.ExtInst), t, r, b.glsl, u32(f))
	append(&b.body, ..args)
	return r
}

// arithmetic that must not be fused or reordered (NoContraction, as Xenia does for invariance across passes)
arith :: proc(b: ^Builder, op: Op, t: u32, x, y: u32) -> u32 {
	r := op1(b, op, t, x, y)
	decorate(b, r, .NoContraction)
	return r
}

load :: proc(b: ^Builder, t: u32, ptr: u32) -> u32 {return op1(b, .Load, t, ptr)}
store :: proc(b: ^Builder, ptr, v: u32) {op0(b, .Store, ptr, v)}
access :: proc(b: ^Builder, t_ptr: u32, base: u32, idx: ..u32) -> u32 {
	r := id(b)
	append(&b.body, u32(len(idx) + 4) << 16 | u32(Op.AccessChain), t_ptr, r, base)
	append(&b.body, ..idx)
	return r
}
extract :: proc(b: ^Builder, t: u32, v: u32, i: u32) -> u32 {return op1(b, .CompositeExtract, t, v, i)}
select :: proc(b: ^Builder, t: u32, c, x, y: u32) -> u32 {return op1(b, .Select, t, c, x, y)}
bitcast :: proc(b: ^Builder, t: u32, v: u32) -> u32 {return op1(b, .Bitcast, t, v)}

label :: proc(b: ^Builder, l: u32) {
	op0(b, .Label, l)
	b.block_open = true
}

branch :: proc(b: ^Builder, target: u32) {
	if !b.block_open {return}
	op0(b, .Branch, target)
	b.block_open = false
}

branch_cond :: proc(b: ^Builder, c, t, f: u32) {
	op0(b, .BranchConditional, c, t, f)
	b.block_open = false
}

// structured if: returns the merge label; the then-block is current
If :: struct {
	merge: u32,
	else_: u32,
}

if_begin :: proc(b: ^Builder, cond: u32) -> If {
	then := id(b)
	merge := id(b)
	op0(b, .SelectionMerge, merge, 0)
	branch_cond(b, cond, then, merge)
	label(b, then)
	return If{merge = merge}
}

if_end :: proc(b: ^Builder, f: If) {
	branch(b, f.merge)
	label(b, f.merge)
}

// assemble the module
builder_finish :: proc(b: ^Builder, allocator := context.allocator) -> []u32 {
	out := make([dynamic]u32, allocator)
	append(&out, 0x07230203, 0x00010600, 0x00080001 /* generator: none registered */, b.bound + 1, 0)
	append(&out, ..b.capabilities[:])
	append(&out, ..b.extensions[:])
	append(&out, ..b.imports[:])
	append(&out, ..b.memory_model[:])
	append(&out, ..b.entry_points[:])
	append(&out, ..b.exec_modes[:])
	append(&out, ..b.debug[:])
	append(&out, ..b.annotations[:])
	append(&out, ..b.globals[:])
	append(&out, ..b.body[:])
	append(&out, ..b.funcs[:])
	return out[:]
}

// ---- helper functions (emitted into `funcs` while main is being built) ----

Helper :: struct {
	id:          u32,
	params:      [4]u32,
	saved_body:  [dynamic]u32,
	saved_open:  bool,
}

helper_begin :: proc(b: ^Builder, ret: u32, param_types: []u32) -> (h: Helper) {
	ft_args := make([dynamic]u32, context.temp_allocator)
	append(&ft_args, ret)
	append(&ft_args, ..param_types)
	ft := type_get(b, .TypeFunction, ..ft_args[:])
	h.saved_body = b.body
	h.saved_open = b.block_open
	b.body = b.funcs
	h.id = id(b)
	op0(b, .Function, ret, h.id, 0, ft)
	for p, i in param_types {
		h.params[i] = id(b)
		op0(b, .FunctionParameter, p, h.params[i])
	}
	label(b, id(b))
	return
}

// a Function variable in the helper's entry block (call right after helper_begin)
helper_var :: proc(b: ^Builder, t: u32) -> u32 {
	r := id(b)
	op0(b, .Variable, type_ptr(b, .Function, t), r, u32(Storage_Class.Function))
	return r
}

helper_end :: proc(b: ^Builder, h: ^Helper) {
	op0(b, .FunctionEnd)
	b.funcs = b.body
	b.body = h.saved_body
	b.block_open = h.saved_open
}

call :: proc(b: ^Builder, ret: u32, f: u32, args: ..u32) -> u32 {
	r := id(b)
	append(&b.body, u32(len(args) + 4) << 16 | u32(Op.FunctionCall), ret, r, f)
	append(&b.body, ..args)
	return r
}

If_Else :: struct {
	else_: u32,
	merge: u32,
}

if_else_begin :: proc(b: ^Builder, cond: u32) -> If_Else {
	then := id(b)
	f := If_Else{else_ = id(b), merge = id(b)}
	op0(b, .SelectionMerge, f.merge, 0)
	branch_cond(b, cond, then, f.else_)
	label(b, then)
	return f
}

if_else_middle :: proc(b: ^Builder, f: If_Else) {
	branch(b, f.merge)
	label(b, f.else_)
}

if_else_end :: proc(b: ^Builder, f: If_Else) {
	branch(b, f.merge)
	label(b, f.merge)
}
