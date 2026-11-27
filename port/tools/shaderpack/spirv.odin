package main

// Minimal SPIR-V reader for the pack: names, decorations, types, variables, constants and access chains, enough to
//   - find the MaterialParameters members a module reads and the material textures it keeps (pass A -> pass B), and
//   - reflect the final module's descriptor bindings, MaterialParameters layout and vertex inputs,
//   - move a module's resources to another descriptor set (pixel shaders: set 1).

import "core:strings"

SPV_MAGIC :: 0x07230203

Op_Name :: 5
Op_MemberName :: 6
Op_TypeVoid :: 19
Op_TypeBool :: 20
Op_TypeInt :: 21
Op_TypeFloat :: 22
Op_TypeVector :: 23
Op_TypeMatrix :: 24
Op_TypeImage :: 25
Op_TypeSampler :: 26
Op_TypeSampledImage :: 27
Op_TypeArray :: 28
Op_TypeRuntimeArray :: 29
Op_TypeStruct :: 30
Op_TypePointer :: 32
Op_Constant :: 43
Op_Variable :: 59
Op_AccessChain :: 65
Op_InBoundsAccessChain :: 66
Op_Decorate :: 71
Op_MemberDecorate :: 72

Dec_RowMajor :: 4
Dec_ColMajor :: 5
Dec_ArrayStride :: 6
Dec_MatrixStride :: 7
Dec_Block :: 2
Dec_BufferBlock :: 3
Dec_BuiltIn :: 11
Dec_Location :: 30
Dec_Binding :: 33
Dec_DescriptorSet :: 34
Dec_Offset :: 35

SC_UniformConstant :: 0
SC_Input :: 1
SC_Uniform :: 2
SC_StorageBuffer :: 12

// VkDescriptorType values
VK_DESCRIPTOR_TYPE_SAMPLER :: 0
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :: 1
VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :: 2
VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :: 3
VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER :: 4
VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER :: 5
VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :: 6
VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :: 7

Spv_Type :: struct {
	op:        u32,
	a, b, c:   u32, // op-specific operands (see spv_parse)
	members:   []u32,
}

Spv_Var :: struct {
	id:      u32,
	type_id: u32, // pointer type
	storage: u32,
}

Spv :: struct {
	words:          []u32,
	names:          map[u32]string,
	member_names:   map[u64]string, // struct id << 32 | member
	member_offsets: map[u64]u32,
	member_decs:    map[[3]u32]u32, // (struct id, member, decoration) -> first literal (or 1)
	decorations:    map[u64]u32,    // id << 32 | decoration -> first literal (or 1)
	types:          map[u32]Spv_Type,
	constants:      map[u32]u32,
	vars:           [dynamic]Spv_Var,
	set_words:      [dynamic]int,   // word positions of DescriptorSet literals
	chains:         [dynamic][2]u32, // (base variable id, first index constant id)
}

spv_string :: proc(words: []u32) -> string {
	bytes := ([^]u8)(raw_data(words))[:4 * len(words)]
	n := 0
	for n < len(bytes) && bytes[n] != 0 {n += 1}
	return string(bytes[:n])
}

spv_parse :: proc(code: []u8, s: ^Spv) -> bool {
	if len(code) < 20 || len(code) % 4 != 0 {return false}
	s.words = ([^]u32)(raw_data(code))[:len(code) / 4]
	w := s.words
	if w[0] != SPV_MAGIC {return false}
	i := 5
	for i < len(w) {
		op, n := w[i] & 0xffff, int(w[i] >> 16)
		if n == 0 || i + n > len(w) {return false}
		o := w[i + 1:i + n]
		switch op {
		case Op_Name:
			s.names[o[0]] = spv_string(o[1:])
		case Op_MemberName:
			s.member_names[u64(o[0]) << 32 | u64(o[1])] = spv_string(o[2:])
		case Op_TypeVoid, Op_TypeBool, Op_TypeSampler:
			s.types[o[0]] = {op = op}
		case Op_TypeInt:
			s.types[o[0]] = {op = op, a = o[1], b = o[2]} // width, signedness
		case Op_TypeFloat:
			s.types[o[0]] = {op = op, a = o[1]}
		case Op_TypeVector, Op_TypeMatrix:
			s.types[o[0]] = {op = op, a = o[1], b = o[2]} // component type, count
		case Op_TypeImage:
			s.types[o[0]] = {op = op, a = o[2], b = o[4], c = o[6]} // dim, arrayed, sampled
		case Op_TypeSampledImage, Op_TypeRuntimeArray:
			s.types[o[0]] = {op = op, a = o[1]}
		case Op_TypeArray:
			s.types[o[0]] = {op = op, a = o[1], b = o[2]} // element type, length constant id
		case Op_TypeStruct:
			s.types[o[0]] = {op = op, members = o[1:]}
		case Op_TypePointer:
			s.types[o[0]] = {op = op, a = o[1], b = o[2]} // storage class, pointee
		case Op_Constant:
			if len(o) >= 3 {s.constants[o[1]] = o[2]}
		case Op_Variable:
			append(&s.vars, Spv_Var{id = o[1], type_id = o[0], storage = o[2]})
		case Op_AccessChain, Op_InBoundsAccessChain:
			if len(o) >= 4 {append(&s.chains, [2]u32{o[2], o[3]})}
		case Op_Decorate:
			v: u32 = 1
			if len(o) >= 3 {v = o[2]}
			s.decorations[u64(o[0]) << 32 | u64(o[1])] = v
			if o[1] == Dec_DescriptorSet {append(&s.set_words, i + 3)}
		case Op_MemberDecorate:
			if o[2] == Dec_Offset {s.member_offsets[u64(o[0]) << 32 | u64(o[1])] = o[3]}
			s.member_decs[{o[0], o[1], o[2]}] = len(o) >= 4 ? o[3] : 1
		}
		i += n
	}
	return true
}

spv_destroy :: proc(s: ^Spv) {
	delete(s.names)
	delete(s.member_names)
	delete(s.member_offsets)
	delete(s.member_decs)
	delete(s.decorations)
	delete(s.types)
	delete(s.constants)
	delete(s.vars)
	delete(s.set_words)
	delete(s.chains)
}

spv_decoration :: proc(s: ^Spv, id: u32, dec: u32) -> (u32, bool) {
	v, ok := s.decorations[u64(id) << 32 | u64(dec)]
	return v, ok
}

spv_var_by_name :: proc(s: ^Spv, name: string) -> (Spv_Var, bool) {
	for v in s.vars {if s.names[v.id] == name {return v, true}}
	return {}, false
}

// the MaterialParameters members a module reads (by member name, MP_ prefix removed)
spv_used_material_parameters :: proc(s: ^Spv, out: ^[dynamic]string) {
	mp, ok := spv_var_by_name(s, "MaterialParameters")
	if !ok {return}
	ptr := s.types[mp.type_id]
	struct_id := ptr.b
	seen: map[u32]bool
	defer delete(seen)
	for c in s.chains {
		if c[0] != mp.id {continue}
		idx, cok := s.constants[c[1]]
		if !cok || idx in seen {continue}
		seen[idx] = true
		name := s.member_names[u64(struct_id) << 32 | u64(idx)]
		if strings.has_prefix(name, "MP_") {append(out, name[3:])}
	}
}

// the material textures (MT_<name> variables) a module keeps
spv_kept_material_textures :: proc(s: ^Spv, out: ^[dynamic]string) {
	for v in s.vars {
		name := s.names[v.id]
		if strings.has_prefix(name, "MT_") {append(out, name[3:])}
	}
}

// every DescriptorSet decoration -> set (in place, in the module and in the parsed decorations)
spv_set_descriptor_set :: proc(s: ^Spv, set: u32) {
	for p in s.set_words {
		s.words[p] = set
		s.decorations[u64(s.words[p - 2]) << 32 | Dec_DescriptorSet] = set // OpDecorate target, DescriptorSet, set
	}
}

Reflect_Param :: struct {
	name:   string,
	offset: u32,
	size:   u32,
	kind:   u32, // component type of the slot: 0 float, 1 int, 2 uint (bool4 slots are uint4)
}
Reflect_Binding :: struct {
	name:            string,
	set, binding:    u32,
	descriptor_type: u32,
	role:            u32,
	image_dim:       u32,
	count:           u32,
}
Reflect_Input :: struct {
	semantic:        string,
	location:        u32,
	component_count: u32,
	base_type:       u32,
}
Reflection :: struct {
	material_size: u32,
	params:        [dynamic]Reflect_Param,
	bindings:      [dynamic]Reflect_Binding,
	inputs:        [dynamic]Reflect_Input,
	cbuffers:      [dynamic]Reflect_Cbuffer, // every uniform buffer but MaterialParameters
}

// bytes of a member type under D3D (dx) layout: arrays of 16-byte slots as the generator declares them
spv_type_size :: proc(s: ^Spv, id: u32) -> u32 {
	t := s.types[id]
	switch t.op {
	case Op_TypeArray:
		return s.constants[t.b] * max(spv_type_size(s, t.a), 16)
	case Op_TypeVector:
		return t.b * spv_type_size(s, t.a)
	case Op_TypeMatrix:
		return t.b * 16
	case Op_TypeInt, Op_TypeFloat:
		return t.a / 8
	case Op_TypeBool:
		return 4
	}
	return 16
}

spv_reflect :: proc(s: ^Spv, r: ^Reflection, stage: Stage) -> bool {
	for v in s.vars {
		name := s.names[v.id]
		ptr, ok := s.types[v.type_id]
		if !ok || ptr.op != Op_TypePointer {continue}
		if v.storage == SC_Input {
			if stage != .VS {continue}
			if _, builtin := spv_decoration(s, v.id, Dec_BuiltIn); builtin {continue}
			loc, has_loc := spv_decoration(s, v.id, Dec_Location)
			if !has_loc {continue}
			sem := name
			if strings.has_prefix(sem, "in.var.") {sem = sem[len("in.var."):]}
			t := s.types[ptr.b]
			count: u32 = 1
			base := t
			if t.op == Op_TypeVector {count = t.b; base = s.types[t.a]}
			bt: u32 = 0
			if base.op == Op_TypeInt {bt = base.b != 0 ? 1 : 2}
			append(&r.inputs, Reflect_Input{semantic = strings.to_upper(sem), location = loc, component_count = count, base_type = bt})
			continue
		}
		set, has_set := spv_decoration(s, v.id, Dec_DescriptorSet)
		binding, has_binding := spv_decoration(s, v.id, Dec_Binding)
		if !has_set || !has_binding {continue}
		b := Reflect_Binding{name = name, set = set, binding = binding, count = 1}
		tid := ptr.b // the variable type, then its array element type
		t := s.types[tid]
		if t.op == Op_TypeArray {
			b.count = s.constants[t.b]
			tid = t.a
		} else if t.op == Op_TypeRuntimeArray {
			b.count = 0
			tid = t.a
		}
		t = s.types[tid]
		switch t.op {
		case Op_TypeSampler:
			b.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER
		case Op_TypeSampledImage:
			b.descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			t = s.types[t.a]
			b.image_dim = spv_image_dim(t)
		case Op_TypeImage:
			b.image_dim = spv_image_dim(t)
			if t.a == 5 { // Buffer
				b.descriptor_type = t.c == 2 ? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
			} else {
				b.descriptor_type = t.c == 2 ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
			}
		case Op_TypeStruct:
			struct_id := tid
			if v.storage == SC_StorageBuffer {
				b.descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			} else if _, bb := spv_decoration(s, struct_id, Dec_BufferBlock); bb {
				b.descriptor_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			} else {
				b.descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			}
			// a (RW)StructuredBuffer<T>: DXC wraps the runtime array of T in a block struct; the array's stride is
			// sizeof(T), the record the D3D device sizes and converts storage by. It goes where image bindings keep
			// their dimension, in dwords (shader_pack.h, s_shader_pack_binding::stride).
			if b.descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER && len(t.members) == 1 {
				if at, ok := s.types[t.members[0]]; ok && at.op == Op_TypeRuntimeArray {
					if stride, has := spv_decoration(s, t.members[0], Dec_ArrayStride); has && stride % 4 == 0 && stride / 4 <= 255 {
						b.image_dim = stride / 4
					}
				}
			}
			if name == "MaterialParameters" {
				b.role = 1
				size: u32 = 0
				for m, mi in t.members {
					mname := s.member_names[u64(struct_id) << 32 | u64(mi)]
					off := s.member_offsets[u64(struct_id) << 32 | u64(mi)]
					msize := spv_type_size(s, m)
					size = max(size, off + max(msize, 16))
					if !strings.has_prefix(mname, "MP_") || mname == "MP__empty" {continue}
					append(&r.params, Reflect_Param{name = mname[3:], offset = off, size = max(msize, 16), kind = spv_scalar_kind(s, m)})
				}
				r.material_size = size
			} else if b.descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER {
				append(&r.cbuffers, spv_reflect_cbuffer(s, name, struct_id))
			}
		case:
			return false
		}
		if strings.has_prefix(name, "MT_") && !strings.has_prefix(name, "MT__unused_") {
			b.role = 2
			b.name = name[3:]
		} else if strings.has_prefix(name, "MS_") && !strings.has_prefix(name, "MS__unused_") {
			b.role = 3
			b.name = name[3:]
		}
		append(&r.bindings, b)
	}
	return true
}

spv_image_dim :: proc(t: Spv_Type) -> u32 {
	switch t.a {
	case 0: return 1
	case 1: return t.b != 0 ? 5 : 2
	case 2: return 3
	case 3: return 4
	case 5: return 6
	}
	return 0
}

// ---- uniform buffer layouts (cbuffer gathers, cbuffer.odin) ----

// One 16-byte register slot of a uniform buffer member: `size` bytes at `offset`, register `reg` after the member's
// first register (D3D9 gives every slot of a member its own consecutive register).
Reflect_Slot :: struct {
	offset: u32,
	size:   u32,
	kind:   u32, // 0 float, 1 int, 2 uint
	reg:    u32,
}
Reflect_Member :: struct {
	name:  string,
	slots: [dynamic]Reflect_Slot,
}
Reflect_Cbuffer :: struct {
	name:    string,
	size:    u32, // bytes, rounded up to 16
	members: [dynamic]Reflect_Member,
}

spv_scalar_kind :: proc(s: ^Spv, id: u32) -> u32 {
	t := s.types[id]
	for t.op == Op_TypeArray || t.op == Op_TypeVector || t.op == Op_TypeMatrix {t = s.types[t.a]}
	if t.op == Op_TypeInt {return t.b != 0 ? 1 : 2}
	if t.op == Op_TypeBool {return 2}
	return 0
}

// the slots of a value of type `id` at byte `offset`, its registers counted from `reg`; returns the registers used
spv_type_slots :: proc(s: ^Spv, id: u32, offset: u32, reg: u32, matrix_stride: u32, row_major: bool, out: ^[dynamic]Reflect_Slot) -> u32 {
	t := s.types[id]
	switch t.op {
	case Op_TypeFloat, Op_TypeInt, Op_TypeBool:
		append(out, Reflect_Slot{offset = offset, size = 4, kind = spv_scalar_kind(s, id), reg = reg})
		return 1
	case Op_TypeVector:
		append(out, Reflect_Slot{offset = offset, size = 4 * t.b, kind = spv_scalar_kind(s, id), reg = reg})
		return 1
	case Op_TypeMatrix:
		// SPIR-V matrix: t.b column vectors of s.types[t.a].b components; RowMajor puts each row in one slot
		comps := s.types[t.a].b
		slots, per := t.b, comps
		if row_major {slots, per = comps, t.b}
		stride := matrix_stride != 0 ? matrix_stride : 16
		for i in 0 ..< slots {append(out, Reflect_Slot{offset = offset + i * stride, size = 4 * per, kind = 0, reg = reg + i})}
		return slots
	case Op_TypeArray:
		n := s.constants[t.b]
		stride, _ := spv_decoration(s, id, Dec_ArrayStride)
		used: u32 = 0
		for i in 0 ..< n {used += spv_type_slots(s, t.a, offset + i * stride, reg + used, matrix_stride, row_major, out)}
		return used
	case Op_TypeStruct:
		used: u32 = 0
		for m, mi in t.members {
			off := s.member_offsets[u64(id) << 32 | u64(mi)]
			ms := s.member_decs[{id, u32(mi), Dec_MatrixStride}]
			_, rm := s.member_decs[{id, u32(mi), Dec_RowMajor}]
			used += spv_type_slots(s, m, offset + off, reg + used, ms, rm, out)
		}
		return used
	}
	return 0
}

spv_reflect_cbuffer :: proc(s: ^Spv, name: string, struct_id: u32) -> Reflect_Cbuffer {
	c := Reflect_Cbuffer{name = name}
	t := s.types[struct_id]
	for m, mi in t.members {
		mem := Reflect_Member{name = s.member_names[u64(struct_id) << 32 | u64(mi)]}
		off := s.member_offsets[u64(struct_id) << 32 | u64(mi)]
		ms := s.member_decs[{struct_id, u32(mi), Dec_MatrixStride}]
		_, rm := s.member_decs[{struct_id, u32(mi), Dec_RowMajor}]
		spv_type_slots(s, m, off, 0, ms, rm, &mem.slots)
		for sl in mem.slots {c.size = max(c.size, (sl.offset + sl.size + 15) / 16 * 16)}
		append(&c.members, mem)
	}
	return c
}
