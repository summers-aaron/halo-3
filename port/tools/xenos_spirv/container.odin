package xenos

// Xbox 360 D3D compiled-shader container, as stored in retail Halo 3 tags (big-endian):
//
//   D3D shader object: D3DPixelShader (0x28 bytes) / D3DVertexShader (0x368 bytes incl. bind state), then
//   container (virtual part), at object + 0x28 / + 0x368:
//     +0x00 flags 0x102a11xx (xx: 0 pixel, 1 vertex)
//     +0x04 virtual size        +0x08 physical size
//     +0x0c offset of the .updb debug path record (u32 length + "\\bungiecache01\shader_symbols\...updb")
//     +0x10 constant table offset (0 in tags: stripped)
//     +0x14 definition table offset (0: none)
//     +0x18 shader record offset
//   definition table (literal constants, `def` / `defi`):
//     +0x00 0, +0x04 float def block mask (bit k: c[252-4k .. 255-4k]), +0x08 int def count, +0x0c 0,
//     +0x10 size of what follows, then float defs {u16 register (PS: 256+), u16 float count, u32 physical offset}
//     terminated by 0, then int defs {u16 key, u16 count, count x u32 loop constant}, terminated by 0.
//   shader record:
//     +0x00 physical offset of the microcode (after the literal constant pool), +0x04 microcode size,
//     +0x08 VS: bits 20-23 interpolator count - 1; PS: program flags
//     +0x0c, +0x10 flags, +0x14 register info,
//     VS: +0x18 1, +0x1c vertex element count, +0x20 patch address count, then u32 0x290?, vertex elements
//         {address:12 (vfetch instruction), usage:4, usage index:4, group:4}, interpolators
//         {usage index:4, usage:4, o register:4, mask:4, slot:4}, then the export patch addresses
//     PS: +0x18 interpolator masks (low 16: registers fed, high 16: ?), +0x1c PS flags (bit 5: param gen),
//         then interpolators {usage index:4, usage:4, r register:4, mask:4}
//   physical part: literal float pool (float defs point into it), then the microcode.
//
// Layout recovered from the retail corpus (tmp/ucode_spirv/reports/01_halo3_microcode_census.md): every one of
// the 8,327 retail Halo 3 shaders parses with these rules.

Stage :: enum u8 {
	Vertex = 0,
	Pixel  = 1,
}

Decl_Usage :: enum u8 {
	Position      = 0,
	Blend_Weight  = 1,
	Blend_Indices = 2,
	Normal        = 3,
	PSize         = 4,
	Texcoord      = 5,
	Tangent       = 6,
	Binormal      = 7,
	Tess_Factor   = 8,
	PositionT     = 9,
	Color         = 10,
	Fog           = 11,
	Depth         = 12,
	Sample        = 13,
}

Float_Def :: struct {
	reg:    u32, // stage-relative c#
	value:  [4]u32, // float bits
}

Int_Def :: struct {
	key:   u32,
	value: u32, // loop constant (count | start << 8 | step << 16)
}

Vertex_Element :: struct {
	address: u32, // vfetch instruction address (ALU/fetch slot index)
	usage:   Decl_Usage,
	index:   u32,
	group:   u32,
}

Interpolator :: struct {
	usage: Decl_Usage,
	index: u32,
	reg:   u32, // VS: o#, PS: r#
	mask:  u32,
	slot:  u32, // VS only
}

Container :: struct {
	stage:           Stage,
	ucode:           []u32, // host-order microcode dwords
	float_defs:      [dynamic]Float_Def,
	int_defs:        [dynamic]Int_Def,
	vertex_elements: [dynamic]Vertex_Element,
	interpolators:   [dynamic]Interpolator,
	patch_addresses: [dynamic]u32,
	flags8:          u32, // record +0x08
	flags_c:         u32,
	flags10:         u32,
	reg_info:        u32, // record +0x14
	ps_masks:        u32, // PS record +0x18
	ps_flags:        u32, // PS record +0x1c
	def_mask:        u32,
	magic_flags:     u32, // container flags low byte: bit 0 vertex; others seen on memexport vertex shaders
}

@(private)
rd32 :: #force_inline proc(b: []u8, o: int) -> u32 {
	return u32(b[o]) << 24 | u32(b[o + 1]) << 16 | u32(b[o + 2]) << 8 | u32(b[o + 3])
}

Parse_Error :: enum {
	None,
	Bad_Magic,
	Bad_Size,
	Bad_Record,
	Bad_Defs,
}

// object: the D3D shader object bytes followed by the container's virtual part; phys: the physical part
container_parse :: proc(object: []u8, phys: []u8, stage: Stage, allocator := context.allocator) -> (c: Container, err: Parse_Error) {
	context.allocator = allocator
	ch := stage == .Pixel ? 0x28 : 0x368
	if len(object) < ch + 0x24 {return c, .Bad_Size}
	o := object[ch:]
	magic := rd32(o, 0)
	if magic & 0xffffff00 != 0x102a1100 {return c, .Bad_Magic}
	c.stage = stage
	c.magic_flags = magic & 0xff
	if (magic & 1 != 0) != (stage == .Vertex) {return c, .Bad_Magic}
	vsize := int(rd32(o, 4))
	psize := int(rd32(o, 8))
	if vsize > len(o) || psize != len(phys) {return c, .Bad_Size}
	dto := int(rd32(o, 0x14))
	so := int(rd32(o, 0x18))
	if so < 0x24 || so + 0x20 > vsize {return c, .Bad_Record}
	phys_off := int(rd32(o, so))
	size := int(rd32(o, so + 4))
	if phys_off + size > psize || size % 4 != 0 || size == 0 {return c, .Bad_Record}
	c.ucode = make([]u32, size / 4)
	for i in 0 ..< size / 4 {c.ucode[i] = rd32(phys, phys_off + 4 * i)}
	c.flags8 = rd32(o, so + 8)
	c.flags_c = rd32(o, so + 0xc)
	c.flags10 = rd32(o, so + 0x10)
	c.reg_info = rd32(o, so + 0x14)
	// literal constants
	if dto != 0 {
		if dto + 0x14 > vsize {return c, .Bad_Defs}
		c.def_mask = rd32(o, dto + 4)
		nint := int(rd32(o, dto + 8))
		dsize := int(rd32(o, dto + 0x10))
		at := dto + 0x14
		end := at + dsize
		if end > vsize {return c, .Bad_Defs}
		for at + 4 <= end {
			w := rd32(o, at)
			if w == 0 {at += 4; break}
			reg := w >> 16
			count := w & 0xffff
			off := int(rd32(o, at + 4))
			at += 8
			if count % 4 != 0 || off + int(count) * 4 > phys_off {return c, .Bad_Defs}
			base := reg
			if stage == .Pixel {
				if base < 256 {return c, .Bad_Defs}
				base -= 256
			}
			for k in 0 ..< count / 4 {
				d := Float_Def{reg = base + k}
				for j in 0 ..< 4 {d.value[j] = rd32(phys, off + 16 * int(k) + 4 * j)}
				append(&c.float_defs, d)
			}
		}
		for _ in 0 ..< nint {
			if at + 4 > end {return c, .Bad_Defs}
			w := rd32(o, at)
			if w == 0 {break}
			count := int(w & 0xffff)
			at += 4
			for k in 0 ..< count {
				if at + 4 > end {return c, .Bad_Defs}
				append(&c.int_defs, Int_Def{key = (w >> 16) + u32(k), value = rd32(o, at)})
				at += 4
			}
		}
	}
	// semantics
	if stage == .Vertex {
		nint := int((c.flags8 >> 20) & 0xf) + 1
		nve := int(rd32(o, so + 0x1c))
		npatch := int(rd32(o, so + 0x20))
		at := so + 0x24 + 4 * int(rd32(o, so + 0x18))
		if at + 4 * (nve + nint + npatch) > vsize {
			// tolerate a missing tail (a record without interpolators)
			nint = 0
		}
		for _ in 0 ..< nve {
			if at + 4 > vsize {return c, .Bad_Record}
			w := rd32(o, at)
			append(&c.vertex_elements, Vertex_Element{address = w & 0xfff, usage = Decl_Usage((w >> 12) & 0xf), index = (w >> 16) & 0xf, group = w >> 20})
			at += 4
		}
		for _ in 0 ..< nint {
			if at + 4 > vsize {break}
			w := rd32(o, at)
			append(&c.interpolators, Interpolator{index = w & 0xf, usage = Decl_Usage((w >> 4) & 0xf), reg = (w >> 8) & 0xf, mask = (w >> 12) & 0xf, slot = w >> 16})
			at += 4
		}
		for _ in 0 ..< npatch {
			if at + 4 > vsize {break}
			append(&c.patch_addresses, rd32(o, at))
			at += 4
		}
	} else {
		c.ps_masks = rd32(o, so + 0x18)
		c.ps_flags = rd32(o, so + 0x1c)
		at := so + 0x20
		for at + 4 <= vsize {
			w := rd32(o, at)
			append(&c.interpolators, Interpolator{index = w & 0xf, usage = Decl_Usage((w >> 4) & 0xf), reg = (w >> 8) & 0xf, mask = (w >> 12) & 0xf, slot = w >> 16})
			at += 4
		}
	}
	return c, .None
}

container_destroy :: proc(c: ^Container) {
	delete(c.ucode)
	delete(c.float_defs)
	delete(c.int_defs)
	delete(c.vertex_elements)
	delete(c.interpolators)
	delete(c.patch_addresses)
}
