package main

// The retail Xenos shaders, by microcode: every compiled shader the game can bind, with the Xbox constant table the
// tag stores beside it, attributed to the pack shader (DX11 SPIR-V) that implements it. The pack keeps a
// (stage, FNV-1a 64 of the microcode) -> pack shader table with each entry's MaterialParameters gather and sampler
// map, so the PC D3D device resolves a retail D3DVertexShader / D3DPixelShader (or a CreateVertexShader /
// CreatePixelShader function) to its SPIR-V without knowing anything about tags.
//
// Where the game's shaders come from (tu/graphicslibxbox/render_method_submit.cpp, rasterizer_main.cpp):
//   - explicit shaders: rasterizer_globals ('rasg') explicit slot -> vtsh + pixl tags;
//   - render methods: the render_method_definition ('rmdf') shared vertex shader ('glvs', +0x44) by vertex type and
//     entry point, and its shared pixel shader ('glps', +0x34) by entry point (shader index, or per option of one
//     category); an entry of -1 falls back to the template's ('rmt2') own vtsh (+0x00) / pixl (+0x10).
//
// Layouts (include/rasterizer/rasterizer_shader_definitions.h, rasterizer_shader_tag_types.h,
// rasterizer_compiled_shader_types.h, render_methods/render_method_shader_definition_types.h; checked against every
// retail map):
//   vtsh / pixl (0x20): flags, entry point block +0x04, version +0x10, compiled shader block +0x14
//     pixl entry point element: (start, count) bytes, indexed by e_entry_point
//     vtsh entry point element (0xc): a block of (start, count) bytes indexed by e_vertex_type
//   glvs (0x1c): vertex type block +0x00 (elements: a block of entry points), compiled shader block +0x10
//   glps (0x1c): entry point block +0x00, compiled shader block +0x10
//     entry point (0x10): category dependency block +0x00 (0x10: category +0, block of int32 shader index per
//     option +4), shader index +0x0c
//   compiled shader (0x50): Xbox constant table block at +0x28 (8-byte elements: string id name, u16 register,
//     u8 register count, u8 D3DXREGISTER_SET), the D3D shader object pointer at +0x4c.
//   D3D shader object (Xenos, big-endian; port_cache_swap_tags leaves it so): D3DResource header with the physical
//     microcode pointer in Identifier (+0x10, what XGRegister*Shader receives); the microcode container (magic
//     0x102a11xx, virtual size, physical size) right after the object: +0x28 for D3DPixelShader, +0x368 for
//     D3DVertexShader (its bind state).
// The microcode hashed is the physical part: the Xenos instruction words, byte for byte as on the Xbox (big-endian).
// A CreateVertexShader / CreatePixelShader function is the container (virtual part) followed by the physical part.

import "core:fmt"
import "core:hash"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"

// D3DXREGISTER_SET
REGSET_BOOL :: 0
REGSET_INT4 :: 1
REGSET_FLOAT4 :: 2
REGSET_SAMPLER :: 3

Xbox_Constant :: struct {
	name:  string, // the parameter's string id text
	reg:   u16,
	count: u8,
	set:   u8, // D3DXREGISTER_SET
}

Microcode_Kind :: enum u8 {
	Explicit = 0, // rasterizer_globals explicit shader (vtsh / pixl)
	Template = 1, // render-method template's own vtsh / pixl (rmt2)
	Shared   = 2, // render-method definition's shared shaders (glvs / glps)
	Function = 3, // a D3D shader function the executable embeds (CreateVertexShader / CreatePixelShader)
}

// What one compiled shader implements, as the tags say.
Microcode_Source :: struct {
	kind:      Microcode_Kind,
	name:      string, // explicit shader, template, or render method definition (shaders\<type>)
	explicit:  int,    // explicit slot, -1 otherwise
	ep, vt:    int,    // entry point, vertex type (-1 when the tag does not say)
	category:  int,    // glps entry point selected per option of this category, -1 otherwise
	option:    int,
	constants: [dynamic]Xbox_Constant,
}

Microcode_Shader :: struct {
	hash:    u64,
	size:    int,
	stage:   Stage,
	sources: [dynamic]Microcode_Source,
	// --xenos (keep_code): the bytes the translator reads (port/tools/xenos_spirv translate_shader): the D3D shader
	// object (0x28 PS / 0x368 VS; zeros for an executable function, which has none) followed by the container's
	// virtual part, and the physical part (the hashed bytes)
	object:  []u8,
	ucode:   []u8,
}

Microcode_Set :: struct {
	shaders: [dynamic]Microcode_Shader,
	index:   map[u64]int, // hash ~ stage -> shaders index
	read:    map[string]bool, // compiled shader elements already read (by map path and address)
	errors:  int,
	keep_code: bool, // keep each shader's object and physical bytes (--xenos)
}

microcode_key :: proc(h: u64, stage: Stage) -> u64 {return h ~ (u64(stage) + 1) * 0x9e3779b97f4a7c15}

// microcode and constants of the compiled shader element at tag-space address `element`
microcode_read_compiled :: proc(m: ^Map, set: ^Microcode_Set, element: u32, stage: Stage, src: Microcode_Source) -> bool {
	e := map_at(m, element, 0x50) or_return
	b := m.b
	rtp := be32(b, e + 0x4c)
	if rtp == 0 {return true} // no D3D object: nothing the game could bind
	ch := stage == .PS ? 0x28 : 0x368
	rt := map_at(m, rtp, ch + 12) or_return
	if be32(b, rt + ch) & 0xffffff00 != 0x102a1100 {
		fmt.eprintfln("shaderpack: %s: shader object %x has no microcode container at +%x", m.path, rtp, ch)
		set.errors += 1
		return false
	}
	psize := int(be32(b, rt + ch + 8))
	if psize <= 0 || psize % 4 != 0 {
		fmt.eprintfln("shaderpack: %s: shader object %x: physical size %d", m.path, rtp, psize)
		set.errors += 1
		return false
	}
	phys := map_at(m, be32(b, rt + 0x10), psize) or_return
	s := src
	s.constants = {}
	nc, cp := int(be32(b, e + 0x28)), be32(b, e + 0x2c)
	for c in 0 ..< nc {
		k := map_at(m, cp + u32(8 * c), 8) or_return
		append(&s.constants, Xbox_Constant{name = map_sid(m, be32(b, k)), reg = u16(be16(b, k + 4)), count = b[k + 6], set = b[k + 7]})
	}
	h := hash.fnv64a(b[phys:][:psize])
	key := microcode_key(h, stage)
	i, found := set.index[key]
	if !found {
		i = len(set.shaders)
		set.index[key] = i
		sh := Microcode_Shader{hash = h, size = psize, stage = stage}
		if set.keep_code {
			vsize := int(be32(b, rt + ch + 4))
			obj := map_at(m, rtp, ch + vsize) or_return
			sh.object = slice.clone(b[obj:][:ch + vsize])
			sh.ucode = slice.clone(b[phys:][:psize])
		}
		append(&set.shaders, sh)
	}
	// one source per distinct attribution (the same tag is read from every map that has it)
	for &o in set.shaders[i].sources {
		if o.kind == s.kind && o.name == s.name && o.explicit == s.explicit && o.ep == s.ep && o.vt == s.vt && o.category == s.category && o.option == s.option {return true}
	}
	s.name = strings.clone(s.name)
	append(&set.shaders[i].sources, s)
	return true
}

// The D3D shader functions an executable image embeds (--executable: the retail PE, whose data is big-endian): every
// microcode container in it, found by its magic and accepted when its sizes fit and its D3DX constant table parses.
// Container (big-endian words): magic 0x102a1100 | stage (0 pixel, 1 vertex), virtual size, physical size, ..,
// +0x10 the offset of the constant table's size word; the D3DXSHADER_CONSTANTTABLE follows that word (Size 0x1c,
// Creator, Version 0xfffe/0xffff...., Constants, ConstantInfo, Flags, Target; offsets from the table) with 20-byte
// D3DXSHADER_CONSTANTINFO records (Name, RegisterSet u16, RegisterIndex u16, RegisterCount u16, ..). The physical
// microcode follows the virtual part. The HLSL reconstruction that implements a function names its microcode hash
// (//@function vs|ps <fnv64>, make_jobs).
microcode_read_executable :: proc(path: string, set: ^Microcode_Set) -> (found: int, ok: bool) {
	data, err := os.read_entire_file(path, context.allocator)
	if err != nil {
		fmt.eprintfln("shaderpack: cannot read %s", path)
		return 0, false
	}
	defer delete(data)
	b := data
	n := len(b)
	for at := 0; at + 0x28 <= n; at += 4 {
		magic := be32(b, at)
		if magic & 0xffffff00 != 0x102a1100 || magic & 0xff > 1 {continue}
		stage: Stage = magic & 0xff == 1 ? .VS : .PS
		vsize, psize := int(be32(b, at + 4)), int(be32(b, at + 8))
		if vsize < 0x28 || vsize % 4 != 0 || vsize > 0x10000 || psize <= 0 || psize % 4 != 0 || psize > 0x10000 || at + vsize + psize > n {continue}
		ct_size_at := int(be32(b, at + 0x10))
		if ct_size_at < 0x14 || ct_size_at + 4 + 0x1c > vsize {continue}
		ct := at + ct_size_at + 4
		ct_size := int(be32(b, ct - 4))
		version := be32(b, ct + 8)
		if be32(b, ct) != 0x1c || ct_size < 0x1c || ct_size_at + 4 + ct_size > vsize || version >> 16 != (stage == .VS ? 0xfffe : 0xffff) {continue}
		count, info := int(be32(b, ct + 12)), int(be32(b, ct + 16))
		if count < 0 || count > 256 || (count > 0 && (info < 0x1c || info + 20 * count > ct_size)) {continue}
		src := Microcode_Source{kind = .Function, explicit = -1, ep = -1, vt = -1, category = -1, option = -1}
		good := true
		for c in 0 ..< count {
			k := ct + info + 20 * c
			name_at := int(be32(b, k))
			if name_at <= 0 || name_at >= ct_size {good = false; break}
			end := ct + name_at
			for end < ct + ct_size && b[end] != 0 {end += 1}
			if end >= ct + ct_size {good = false; break}
			append(&src.constants, Xbox_Constant{name = strings.clone(string(b[ct + name_at:end])), reg = u16(be16(b, k + 6)), count = u8(be16(b, k + 8)), set = u8(be16(b, k + 4))})
		}
		if !good {
			delete(src.constants)
			continue
		}
		phys := b[at + vsize:][:psize]
		h := hash.fnv64a(phys)
		key := microcode_key(h, stage)
		if _, seen := set.index[key]; seen {
			delete(src.constants) // the same microcode is already known (a tag, or an earlier copy in the image)
			continue
		}
		src.name = fmt.aprintf("executable+0x%x", at)
		set.index[key] = len(set.shaders)
		sh := Microcode_Shader{hash = h, size = psize, stage = stage}
		if set.keep_code {
			// a function is the container alone: the translator reads it after an (absent, zeroed) shader object
			ch := stage == .PS ? 0x28 : 0x368
			sh.object = make([]u8, ch + vsize)
			copy(sh.object[ch:], b[at:][:vsize])
			sh.ucode = slice.clone(phys)
		}
		append(&set.shaders, sh)
		append(&set.shaders[len(set.shaders) - 1].sources, src)
		found += 1
		at += vsize + psize - 4
	}
	return found, true
}

// compiled shader block of a vtsh / pixl (at +0x14) or glvs / glps (at +0x10): (count, address)
microcode_shader_block :: proc(m: ^Map, addr: u32, at: int) -> (count: int, block: u32, ok: bool) {
	base := map_at(m, addr, at + 0xc) or_return
	return int(be32(m.b, base + at)), be32(m.b, base + at + 4), true
}

microcode_read_pixl :: proc(m: ^Map, set: ^Microcode_Set, t: int, src: Microcode_Source) -> bool {
	g, addr := map_tag_group(m, t) or_return
	if !group_is(g, "pixl") {return false}
	base := map_at(m, addr, 0x20) or_return
	count, blk := microcode_shader_block(m, addr, 0x14) or_return
	ne, eb := int(be32(m.b, base + 4)), be32(m.b, base + 8)
	for e in 0 ..< ne {
		k := map_at(m, eb + u32(2 * e), 2) or_return
		start, n := int(m.b[k]), int(m.b[k + 1])
		for p in 0 ..< n {
			if start + p >= count {set.errors += 1; continue}
			s := src
			s.ep = e
			microcode_read_compiled(m, set, blk + u32(0x50 * (start + p)), .PS, s)
		}
	}
	return true
}

microcode_read_vtsh :: proc(m: ^Map, set: ^Microcode_Set, t: int, src: Microcode_Source) -> bool {
	g, addr := map_tag_group(m, t) or_return
	if !group_is(g, "vtsh") {return false}
	base := map_at(m, addr, 0x20) or_return
	count, blk := microcode_shader_block(m, addr, 0x14) or_return
	ne, eb := int(be32(m.b, base + 4)), be32(m.b, base + 8)
	for e in 0 ..< ne {
		ee := map_at(m, eb + u32(0xc * e), 0xc) or_return
		nv, vb := int(be32(m.b, ee)), be32(m.b, ee + 4)
		for v in 0 ..< nv {
			k := map_at(m, vb + u32(2 * v), 2) or_return
			start, n := int(m.b[k]), int(m.b[k + 1])
			for p in 0 ..< n {
				if start + p >= count {set.errors += 1; continue}
				s := src
				s.ep = e
				s.vt = v
				microcode_read_compiled(m, set, blk + u32(0x50 * (start + p)), .VS, s)
			}
		}
	}
	return true
}

// one glvs / glps entry point element (0x10) at file offset o: the compiled shader it selects (or per option)
microcode_read_global_entry :: proc(m: ^Map, set: ^Microcode_Set, o: int, count: int, blk: u32, stage: Stage, src: Microcode_Source) -> bool {
	nd, dp := int(be32(m.b, o)), be32(m.b, o + 4)
	if nd > 0 {
		d := map_at(m, dp, 0x10) or_return
		cat := int(be32(m.b, d))
		no, op := int(be32(m.b, d + 4)), be32(m.b, d + 8)
		for opt in 0 ..< no {
			k := map_at(m, op + u32(4 * opt), 4) or_return
			idx := int(i32(be32(m.b, k)))
			if idx < 0 {continue}
			if idx >= count {set.errors += 1; continue}
			s := src
			s.category = cat
			s.option = opt
			microcode_read_compiled(m, set, blk + u32(0x50 * idx), stage, s)
		}
		return true
	}
	idx := int(i32(be32(m.b, o + 0xc)))
	if idx < 0 {return true}
	if idx >= count {set.errors += 1; return true}
	microcode_read_compiled(m, set, blk + u32(0x50 * idx), stage, src)
	return true
}

// the shared shaders of a render method definition (rmdf +0x34 glps, +0x44 glvs)
microcode_read_rmdf :: proc(m: ^Map, set: ^Microcode_Set, name: string, addr: u32) -> bool {
	base := map_at(m, addr, 0x54) or_return
	src := Microcode_Source{kind = .Shared, name = name, explicit = -1, ep = -1, vt = -1, category = -1, option = -1}
	if glps := map_tag_ref(m, base + 0x34); glps >= 0 {
		g, gaddr := map_tag_group(m, glps) or_return
		if !group_is(g, "glps") {return false}
		gb := map_at(m, gaddr, 0x1c) or_return
		count, blk := microcode_shader_block(m, gaddr, 0x10) or_return
		ne, ep := int(be32(m.b, gb)), be32(m.b, gb + 4)
		for e in 0 ..< ne {
			o := map_at(m, ep + u32(0x10 * e), 0x10) or_return
			s := src
			s.ep = e
			microcode_read_global_entry(m, set, o, count, blk, .PS, s) or_return
		}
	}
	if glvs := map_tag_ref(m, base + 0x44); glvs >= 0 {
		g, gaddr := map_tag_group(m, glvs) or_return
		if !group_is(g, "glvs") {return false}
		gb := map_at(m, gaddr, 0x1c) or_return
		count, blk := microcode_shader_block(m, gaddr, 0x10) or_return
		nv, vp := int(be32(m.b, gb)), be32(m.b, gb + 4)
		for v in 0 ..< nv {
			vo := map_at(m, vp + u32(0xc * v), 0xc) or_return
			ne, ep := int(be32(m.b, vo)), be32(m.b, vo + 4)
			for e in 0 ..< ne {
				o := map_at(m, ep + u32(0x10 * e), 0x10) or_return
				s := src
				s.ep = e
				s.vt = v
				microcode_read_global_entry(m, set, o, count, blk, .VS, s) or_return
			}
		}
	}
	return true
}

// the retail shaders of one map
map_read_microcode :: proc(m: ^Map, set: ^Microcode_Set) -> bool {
	// the vertex / pixel shader tags the rasterizer_globals slots reference; the others named rasterizer\shaders\<name>
	// (the HUD's chud_*) are explicit shaders known by name only (explicit -1)
	in_rasg := make(map[int]bool)
	defer delete(in_rasg)
	for t in 0 ..< m.ntags {
		g, addr, ok := map_tag_group(m, t)
		if !ok || !group_is(g, "rasg") {continue}
		base := map_at(m, addr, 0xa4) or_return
		count, ptr := int(be32(m.b, base + 0x38)), be32(m.b, base + 0x3c)
		for i in 0 ..< count {
			e := map_at(m, ptr + u32(0x20 * i), 0x20) or_return
			if vs := map_tag_ref(m, e); vs >= 0 {in_rasg[vs] = true}
			if ps := map_tag_ref(m, e + 0x10); ps >= 0 {in_rasg[ps] = true}
		}
	}
	for t in 0 ..< m.ntags {
		g, _, ok := map_tag_group(m, t)
		if !ok || t in in_rasg || !(group_is(g, "vtsh") || group_is(g, "pixl")) {continue}
		name := map_tag_name(m, t)
		if !strings.has_prefix(name, "rasterizer\\shaders\\") {continue}
		src := Microcode_Source{kind = .Explicit, name = name, explicit = -1, ep = -1, vt = -1, category = -1, option = -1}
		if group_is(g, "vtsh") {microcode_read_vtsh(m, set, t, src) or_return} else {microcode_read_pixl(m, set, t, src) or_return}
	}
	for t in 0 ..< m.ntags {
		g, addr, ok := map_tag_group(m, t)
		if !ok {continue}
		switch string(g[:]) {
		case "rasg":
			base := map_at(m, addr, 0xa4) or_return
			count, ptr := int(be32(m.b, base + 0x38)), be32(m.b, base + 0x3c)
			for i in 0 ..< count {
				e := map_at(m, ptr + u32(0x20 * i), 0x20) or_return
				vs, ps := map_tag_ref(m, e), map_tag_ref(m, e + 0x10)
				name := ""
				if vs >= 0 {name = map_tag_name(m, vs)} else if ps >= 0 {name = map_tag_name(m, ps)}
				src := Microcode_Source{kind = .Explicit, name = name, explicit = i, ep = -1, vt = -1, category = -1, option = -1}
				if vs >= 0 {microcode_read_vtsh(m, set, vs, src) or_return}
				if ps >= 0 {microcode_read_pixl(m, set, ps, src) or_return}
			}
		case "rmt2":
			base := map_at(m, addr, 0x20) or_return
			src := Microcode_Source{kind = .Template, name = map_tag_name(m, t), explicit = -1, ep = -1, vt = -1, category = -1, option = -1}
			if vs := map_tag_ref(m, base); vs >= 0 {microcode_read_vtsh(m, set, vs, src) or_return}
			if ps := map_tag_ref(m, base + 0x10); ps >= 0 {microcode_read_pixl(m, set, ps, src) or_return}
		case "rmdf":
			microcode_read_rmdf(m, set, map_tag_name(m, t), addr) or_return
		}
	}
	return true
}

// ---- resolution to pack shaders ----

// template option tuple ("shaders\shader_templates\_0_1_0" -> [0, 1, 0])
template_options :: proc(name: string) -> []int {
	i := strings.last_index(name, "\\_")
	if i < 0 {return nil}
	parts := strings.split(name[i + 2:], "_", context.temp_allocator)
	out := make([]int, len(parts), context.temp_allocator)
	for p, k in parts {
		v, ok := strconv.parse_int(p)
		out[k] = ok ? v : -1
	}
	return out
}

Pack_Key :: struct {
	kind, stage, ep, vt: u8,
	explicit:            u16,
	name:                string,
}

// Gather: one copy from the Xbox register files into a uniform buffer (s_shader_pack_file_gather).
Gather_Source :: enum u8 {
	Float      = 0, // float4 register c#: `size` bytes from .x
	Bool       = 1, // bool register b#: one uint32 0 / 1
	Int        = 2, // integer register i#: `size` bytes of int32 components (count, start, step, 0)
	Float_Int  = 3, // float4 register c# read as int32 components (HLSL int / uint members on Xbox c registers)
	Int_Float  = 4, // integer register i# converted to float components (the DX11 tree declares Xbox int parameters float: PARAM(float, layers_of_4), CATEGORY_PARAM)
}
Gather :: struct {
	dst:    u32,
	size:   u32,
	src:    Gather_Source,
	reg:    u32,
	buffer: u32, // microcode entries: GATHER_MATERIAL_PARAMETERS or the cbuffer table index of an override
}
GATHER_MATERIAL_PARAMETERS :: 0xffff

Sampler_Map :: struct {
	binding: u32, // index in the pack shader's binding list
	sampler: u32, // Xbox sampler register
}

Microcode_Entry :: struct {
	hash:         u64,
	source:       string, // the first source tag name (manifest only)
	size:         int,
	stage:        Stage,
	kind:         Microcode_Kind,
	ep, vt:       int,
	shader_index: int, // -1: no pack shader
	gathers:      [dynamic]Gather,
	samplers:     [dynamic]Sampler_Map,
	unmatched:    int, // MaterialParameters members / textures with no Xbox constant of their name
	unmatched_names: [dynamic]string, // their names (manifest only)
}

Microcode_Stats :: struct {
	resolved, unresolved:           [Microcode_Kind][Stage]int,
	vertex_type_mismatch:           int,
	template_dependent:             int, // shared vertex shaders whose DX11 module differs between templates
	mp_members, mp_matched:         int,
	cbuffer_overrides:              int,
	shaders_with_overrides:         int,
	textures, textures_matched:     int,
}

// The binding name without the generator's / HLSL macros' prefixes: what the Xbox constant table calls it.
binding_base_name :: proc(name: string) -> string {
	for p in ([]string{"LocalTexture_", "LocalSampler_", "GlobalTexture_", "GlobalSampler_"}) {
		if strings.has_prefix(name, p) {return name[len(p):]}
	}
	return name
}

microcode_resolve :: proc(set: ^Microcode_Set, sh: ^Map_Shaders, entries: []Pack_Shader, module_of: map[^Job]int, tables: []Cbuffer_Table, table_index: map[string]int, stats: ^Microcode_Stats) -> [dynamic]Microcode_Entry {
	by_key := make(map[Pack_Key]int)
	defer delete(by_key)
	for &e, i in entries {
		k := Pack_Key{kind = e.kind, stage = e.stage, ep = e.ep, vt = e.vt, explicit = e.kind == 0 ? e.explicit_index : 0xffff, name = e.kind == 0 ? "" : e.name}
		if !(k in by_key) {by_key[k] = i}
	}
	// templates of each render method definition, in name order
	by_rmdf := make(map[string][dynamic]string)
	defer delete(by_rmdf)
	names: [dynamic]string
	defer delete(names)
	for k in sh.templates {append(&names, k)}
	slice.sort(names[:])
	for n in names {
		rest := n[len("shaders\\"):]
		us := strings.index(rest, "_templates\\")
		if us < 0 {continue}
		r := strings.concatenate({"shaders\\", rest[:us]})
		if !(r in by_rmdf) {by_rmdf[r] = {}}
		append(&by_rmdf[r], n)
	}
	find :: proc(by_key: ^map[Pack_Key]int, kind: u8, stage: Stage, ep, vt: int, explicit: int, name: string) -> int {
		k := Pack_Key{kind = kind, stage = u8(stage), ep = u8(max(ep, 0)), vt = u8(max(vt, 0)), explicit = kind == 0 ? u16(explicit) : 0xffff, name = kind == 0 ? "" : name}
		if i, ok := by_key[k]; ok {return i}
		return -1
	}

	out: [dynamic]Microcode_Entry
	for &ms in set.shaders {
		me := Microcode_Entry{hash = ms.hash, size = ms.size, stage = ms.stage, shader_index = -1, ep = -1, vt = -1}
		chosen: ^Microcode_Source
		for &src in ms.sources {
			idx := -1
			switch src.kind {
			case .Explicit:
				// the pack has one shader per explicit slot and stage (its //@generate vertex type)
				// (a shader outside rasterizer_globals, explicit -1: by its name)
				// the entry of the microcode's own entry point and vertex type (//@entry, extra vertex types), else
				// the slot's default entry point
				best := -1
				for &e, i in entries {
					if e.kind == 0 && Stage(e.stage) == ms.stage &&
					   (src.explicit >= 0 ? int(e.explicit_index) == src.explicit : e.name == src.name) {
						rank := 0
						if int(e.ep) == max(src.ep, 0) {rank += 2} else if e.ep != 0 {continue}
						if ms.stage != .VS || src.vt < 0 || int(e.vt) == src.vt {rank += 1}
						if rank > best {best, idx = rank, i}
					}
				}
				// a non-default entry point the pack has no module for: a memexport pass (water_ripple's ripple_add),
				// left to the compute shader below as before
				if idx >= 0 && src.ep > 0 && int(entries[idx].ep) != src.ep && ms.stage == .VS {
					for &e in entries {if e.kind == 0 && int(e.explicit_index) == src.explicit && Stage(e.stage) == .CS {idx = -1; break}}
				}
				// a memexport update (particle/contrail spawn and update, beam, light volume, water ripple): the
				// Xbox vertex shader writes memory, DX11 replaces it with the slot's compute shader. The microcode
				// entry points at that compute shader, so the D3D device runs its draws as dispatches; its gathers
				// below resolve against the compute shader's cbuffers (the same Xbox vertex constants).
				if idx < 0 && ms.stage == .VS {
					for &e, i in entries {
						if e.kind == 0 && int(e.explicit_index) == src.explicit && Stage(e.stage) == .CS {idx = i; break}
					}
				}
			case .Template:
				if ms.stage == .PS {
					idx = find(&by_key, 1, .PS, src.ep, -1, -1, src.name)
					if idx < 0 { // pixel shaders are compiled for the first vertex type of the render method
						for &e, i in entries {if e.kind == 1 && Stage(e.stage) == .PS && int(e.ep) == src.ep && e.name == src.name {idx = i; break}}
					}
				} else {
					idx = find(&by_key, 1, .VS, src.ep, src.vt, -1, src.name)
				}
			case .Shared:
				list := by_rmdf[src.name]
				modules: map[int]bool
				defer delete(modules)
				for t in list {
					if src.category >= 0 {
						opts := template_options(t)
						if src.category >= len(opts) || opts[src.category] != src.option {continue}
					}
					i := -1
					if ms.stage == .VS {
						i = find(&by_key, 1, .VS, src.ep, src.vt, -1, t)
					} else {
						for &e, k in entries {if e.kind == 1 && Stage(e.stage) == .PS && int(e.ep) == src.ep && e.name == t {i = k; break}}
					}
					if i < 0 {continue}
					modules[module_of[entries[i].job]] = true
					if idx < 0 {idx = i}
				}
				if len(modules) > 1 && ms.stage == .VS {stats.template_dependent += 1}
			case .Function:
				// the reconstruction that names this microcode (//@function), its vertex type from //@generate
				for &e, i in entries {
					if e.kind == 2 && Stage(e.stage) == ms.stage && e.job.function_hash == ms.hash {idx = i; break}
				}
			}
			if idx >= 0 {
				me.shader_index = idx
				me.kind = src.kind
				me.ep = src.ep
				me.vt = src.vt
				chosen = &src
				break
			}
		}
		me.source = ms.sources[0].name
		if chosen != nil && chosen.kind == .Function {me.source = entries[me.shader_index].name}
		if chosen == nil {
			me.kind = ms.sources[0].kind
			me.ep = ms.sources[0].ep
			me.vt = ms.sources[0].vt
			stats.unresolved[me.kind][ms.stage] += 1
			append(&out, me)
			continue
		}
		stats.resolved[me.kind][ms.stage] += 1
		pe := &entries[me.shader_index]
		if me.vt >= 0 && int(pe.vt) != me.vt && ms.stage == .VS {stats.vertex_type_mismatch += 1}
		// MaterialParameters: each member from the Xbox constant of its name
		r := &pe.job.refl
		for p in r.params {
			stats.mp_members += 1
			found := false
			for c in chosen.constants {
				if c.name != p.name {continue}
				switch c.set {
				case REGSET_FLOAT4:
					n := min(u32(c.count), (p.size + 15) / 16)
					for k in 0 ..< n {append(&me.gathers, Gather{dst = p.offset + 16 * k, size = 16, src = p.kind == 0 ? .Float : .Float_Int, reg = u32(c.reg) + k, buffer = GATHER_MATERIAL_PARAMETERS})}
					found = true
				case REGSET_BOOL:
					n := min(u32(c.count), (p.size + 15) / 16)
					for k in 0 ..< n {append(&me.gathers, Gather{dst = p.offset + 16 * k, size = 4, src = .Bool, reg = u32(c.reg) + k, buffer = GATHER_MATERIAL_PARAMETERS})}
					found = true
				case REGSET_INT4:
					n := min(u32(c.count), (p.size + 15) / 16)
					for k in 0 ..< n {append(&me.gathers, Gather{dst = p.offset + 16 * k, size = 16, src = p.kind == 0 ? .Int_Float : .Int, reg = u32(c.reg) + k, buffer = GATHER_MATERIAL_PARAMETERS})}
					found = true
				}
				break
			}
			if found {stats.mp_matched += 1} else {me.unmatched += 1; append(&me.unmatched_names, p.name)}
		}
		// cbuffer members this shader keeps at other registers than the global table
		n := microcode_cbuffer_overrides(chosen.constants[:], r, tables, table_index, &me.gathers)
		stats.cbuffer_overrides += n
		if n > 0 {stats.shaders_with_overrides += 1}
		// textures and samplers: the Xbox sampler register of the same name
		for b, bi in r.bindings {
			if b.descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE && b.descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLER && b.descriptor_type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER {continue}
			if strings.has_prefix(b.name, "MT__unused_") || strings.has_prefix(b.name, "MS__unused_") {continue}
			base := binding_base_name(b.name)
			if b.descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLER {stats.textures += 1}
			found := false
			// the MCC tree aliases some vertex textures with a stage prefix (water_registers.fx:
			// #define vs_tex_ripple_buffer_slope_height tex_ripple_buffer_slope_height); the retail table has the 2007 name
			alias := base
			if strings.has_prefix(base, "vs_") || strings.has_prefix(base, "ps_") {alias = base[3:]}
			for want in ([]string{base, alias}) {
				for c in chosen.constants {
					if c.set == REGSET_SAMPLER && c.name == want {
						append(&me.samplers, Sampler_Map{binding = u32(bi), sampler = u32(c.reg)})
						found = true
						break
					}
				}
				if found {break}
			}
			if b.descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLER {
				if found {stats.textures_matched += 1} else {me.unmatched += 1; append(&me.unmatched_names, base)}
			}
		}
		append(&out, me)
	}
	slice.sort_by(out[:], proc(a, b: Microcode_Entry) -> bool {
		if a.hash != b.hash {return a.hash < b.hash}
		return a.stage < b.stage
	})
	return out
}

// --survey-microcode: what the maps hold, without compiling anything
microcode_survey :: proc(set: ^Microcode_Set) {
	by_kind: [Microcode_Kind][Stage]int
	sets: [256]int
	multi := 0
	for &s in set.shaders {
		by_kind[s.sources[0].kind][s.stage] += 1
		for c in s.sources[0].constants {sets[c.set] += 1}
		if len(s.sources) > 1 {multi += 1}
	}
	for k in Microcode_Kind {fmt.printfln("microcode: %v: vs %d, ps %d", k, by_kind[k][.VS], by_kind[k][.PS])}
	fmt.printfln("microcode: %d unique (stage, microcode), %d with more than one source, %d errors", len(set.shaders), multi, set.errors)
	for n, i in sets {if n > 0 {fmt.printfln("microcode: register set %d: %d constants", i, n)}}
}

// --dump-microcode NAME: the sources and Xbox constant tables of every retail shader whose source name contains NAME
microcode_dump :: proc(set: ^Microcode_Set, filter: string) {
	for &s in set.shaders {
		for &src in s.sources {
			if !strings.contains(src.name, filter) {continue}
			fmt.printf("%016x %s %v %s explicit %d ep %d vt %d cat %d opt %d:", s.hash, stage_name[s.stage], src.kind, src.name, src.explicit, src.ep, src.vt, src.category, src.option)
			for c in src.constants {set_chars := "bifs"; fmt.printf(" %s=%c%d[%d]", c.name, set_chars[c.set & 3], c.reg, c.count)}
			fmt.println()
		}
	}
}
