package main

// Read-only access to a retail Halo 3 (cache version 11, Xbox 360, big-endian) .map: the header, the tag index, tag
// names and string ids, and the few tag layouts the shader pack needs. Offsets follow the earlier retail shader dump
// tool (tools/odin/h3shaders, git dd3aea268^) and Assembly's H3 layouts, checked against mainmenu.map and
// 010_jungle.map:
//   rasg (rasterizer_globals, 0xa4): explicit shaders block at +0x38 (elements 0x20: vertex, pixel tag references)
//   rmt2 (render_method_template): vertex shader reference +0x00, pixel shader reference +0x10
//   pixl (pixel_shader): entry point block at +0x04 (elements 2 bytes: first compiled shader, count)
//   rmdf (render_method_definition): categories +0x10 (0x18: name sid, options block +4, vertex/pixel function sids
//        +0x10/+0x14), options (0x1c: name sid, vertex/pixel function sids +0x14/+0x18), entry points +0x1c (0x10:
//        enum at +0), vertex types +0x28 (0x10: enum16 at +0)
// A tag reference is 0x10 bytes: group tag, then the referenced tag's datum index at +0xc.

import "core:fmt"
import "core:os"
import "core:strings"

Map :: struct {
	path:      string,
	b:         []u8,
	vbase:     u32, // tag-space virtual base
	tag_file:  int, // file offset of the tag section
	tag_size:  int,
	names_at:  int,
	nindex_at: int,
	sidx_at:   int,
	sdata_at:  int,
	nstrings:  int,
	ngroups:   int,
	groups:    int,
	ntags:     int,
	tags:      int,
}

be32 :: proc(b: []u8, o: int) -> u32 {return u32(b[o]) << 24 | u32(b[o + 1]) << 16 | u32(b[o + 2]) << 8 | u32(b[o + 3])}
be16 :: proc(b: []u8, o: int) -> u32 {return u32(b[o]) << 8 | u32(b[o + 1])}

fourcc :: proc(v: u32) -> [4]u8 {return {u8(v >> 24), u8(v >> 16), u8(v >> 8), u8(v)}}

map_open :: proc(path: string) -> (m: Map, ok: bool) {
	b, err := os.read_entire_file(path, context.allocator)
	if err != nil {
		fmt.eprintfln("shaderpack: cannot read %s", path)
		return
	}
	if len(b) < 0x3000 || be32(b, 0) != 0x68656164 || be32(b, 4) != 11 {
		fmt.eprintfln("shaderpack: %s is not a Halo 3 (v11) cache file", path)
		return
	}
	masks := [4]u32{be32(b, 0x46c), be32(b, 0x470), be32(b, 0x474), be32(b, 0x478)}
	m.path = path
	m.b = b
	m.vbase = be32(b, 0x2e8)
	m.tag_file = int(be32(b, 0x47c + 8 * 2) + masks[2])
	m.tag_size = int(be32(b, 0x480 + 16))
	debug_mask := masks[0]
	m.names_at = int(be32(b, 0x2b8) + debug_mask)
	m.nindex_at = int(be32(b, 0x2c0) + debug_mask)
	m.nstrings = int(be32(b, 0x158))
	m.sidx_at = int(be32(b, 0x160) + debug_mask)
	m.sdata_at = int(be32(b, 0x164) + debug_mask)
	// shared.map and campaign.map hold resources only: no tag index
	if be32(b, 0x10) == 0 {return m, true}
	ih, ih_ok := map_at(&m, be32(b, 0x10), 0x28)
	if !ih_ok {return}
	m.ngroups = int(be32(b, ih))
	m.ntags = int(be32(b, ih + 8))
	m.groups, ok = map_at(&m, be32(b, ih + 4), 16 * m.ngroups)
	if !ok {return}
	m.tags, ok = map_at(&m, be32(b, ih + 12), 8 * m.ntags)
	if !ok {return}
	if m.ntags != int(be32(b, 0x2b4)) {
		fmt.eprintfln("shaderpack: %s: tag count %d != name count %d", path, m.ntags, be32(b, 0x2b4))
		return m, false
	}
	return m, true
}

// tag-space pointer -> file offset (bounds-checked against the tag section)
map_at :: proc(m: ^Map, ptr: u32, size: int) -> (int, bool) {
	o := int(ptr) - int(m.vbase)
	if ptr < m.vbase || o + size > m.tag_size {
		fmt.eprintfln("shaderpack: %s: pointer %x (+%x) outside the tag section", m.path, ptr, size)
		return 0, false
	}
	return m.tag_file + o, true
}

// string id -> text (a copy: the map buffer is released after reading); "" for 0 or an id outside the table. Index in the low 16 bits, namespace in bits 16-23
// (Assembly H3_StringIDs.xml).
map_sid :: proc(m: ^Map, v: u32) -> string {
	starts := [9]u32{0, 0x4b7, 0xa7d, 0xb0f, 0xbaf, 0xb63, 0xbbf, 0xbf0, 0xc04}
	i, set := v & 0xffff, (v >> 16) & 0xff
	g: u32 = 0xffffffff
	if set == 0 {
		g = i >= 0x4b7 ? i - 0x4b7 + 0xc11 : i
	} else if set < 9 {
		g = starts[set] + i
	}
	if v == 0 || int(g) >= m.nstrings {return ""}
	return strings.clone(string(cstring(raw_data(m.b[m.sdata_at + int(be32(m.b, m.sidx_at + 4 * int(g))):]))))
}

map_tag_group :: proc(m: ^Map, t: int) -> (group: [4]u8, address: u32, ok: bool) {
	if t < 0 || t >= m.ntags {return}
	gi := int(i16(be16(m.b, m.tags + 8 * t)))
	address = be32(m.b, m.tags + 8 * t + 4)
	if gi < 0 || gi >= m.ngroups || address == 0 {return}
	return fourcc(be32(m.b, m.groups + 16 * gi)), address, true
}

map_tag_name :: proc(m: ^Map, t: int) -> string {
	no := int(be32(m.b, m.nindex_at + 4 * t))
	return string(cstring(raw_data(m.b[m.names_at + no:])))
}

// tag reference at file offset o -> tag index (-1 when null)
map_tag_ref :: proc(m: ^Map, o: int) -> int {
	datum := be32(m.b, o + 0xc)
	if datum == 0xffffffff {return -1}
	return int(datum & 0xffff)
}

group_is :: proc(g: [4]u8, s: string) -> bool {g := g; return string(g[:]) == s}

// ---- the shader data the pack needs ----

Rmdf_Option :: struct {
	name:     string,
	vs_fn:    string,
	ps_fn:    string,
}
Rmdf_Category :: struct {
	name:    string,
	vs_fn:   string,
	ps_fn:   string,
	options: [dynamic]Rmdf_Option,
}
Rmdf :: struct {
	name:         string, // shaders\<type>
	categories:   [dynamic]Rmdf_Category,
	entry_points: [dynamic]u32,
	vertex_types: [dynamic]u32,
}

Template :: struct {
	name:         string, // shaders\<type>_templates\_<options>
	entry_points: u32,    // bit per entry point with pixel shaders in the template's pixl
	source_map:   string,
}

Explicit :: struct {
	name:      string, // rasterizer\shaders\<name>; "" for an empty slot
	has_vs:    bool,
	has_ps:    bool,
}

Map_Shaders :: struct {
	rmdfs:     map[string]Rmdf,
	templates: map[string]Template,
	explicit:  [dynamic]Explicit,
	// rasterizer\shaders\<name> vertex / pixel shader tags no rasterizer_globals slot references (the HUD's chud_*,
	// which the chud globals reference): compiled like explicit shaders, numbered after the rasg slots (main.odin)
	named:     map[string]Explicit,
}

map_read_rmdf :: proc(m: ^Map, name: string, addr: u32) -> (r: Rmdf, ok: bool) {
	base := map_at(m, addr, 0x5c) or_return
	b := m.b
	r.name = name
	nc, cp := int(be32(b, base + 0x10)), be32(b, base + 0x14)
	for c in 0 ..< nc {
		ce := map_at(m, cp + u32(0x18 * c), 0x18) or_return
		cat := Rmdf_Category {
			name  = map_sid(m, be32(b, ce)),
			vs_fn = map_sid(m, be32(b, ce + 0x10)),
			ps_fn = map_sid(m, be32(b, ce + 0x14)),
		}
		no, op := int(be32(b, ce + 4)), be32(b, ce + 8)
		for o in 0 ..< no {
			oe := map_at(m, op + u32(0x1c * o), 0x1c) or_return
			append(&cat.options, Rmdf_Option{name = map_sid(m, be32(b, oe)), vs_fn = map_sid(m, be32(b, oe + 0x14)), ps_fn = map_sid(m, be32(b, oe + 0x18))})
		}
		append(&r.categories, cat)
	}
	ne, ep := int(be32(b, base + 0x1c)), be32(b, base + 0x20)
	for e in 0 ..< ne {
		eo := map_at(m, ep + u32(0x10 * e), 0x10) or_return
		append(&r.entry_points, be32(b, eo))
	}
	nv, vp := int(be32(b, base + 0x28)), be32(b, base + 0x2c)
	for v in 0 ..< nv {
		vo := map_at(m, vp + u32(0x10 * v), 0x10) or_return
		append(&r.vertex_types, be16(b, vo))
	}
	return r, true
}

// entry points with compiled pixel shaders in a pixl tag
map_pixl_entry_points :: proc(m: ^Map, t: int) -> (mask: u32, ok: bool) {
	g, addr := map_tag_group(m, t) or_return
	if !group_is(g, "pixl") {return}
	base := map_at(m, addr, 0x20) or_return
	ne, eb := int(be32(m.b, base + 4)), be32(m.b, base + 8)
	for e in 0 ..< ne {
		k := map_at(m, eb + u32(2 * e), 2) or_return
		if m.b[k + 1] != 0 && e < 32 {mask |= 1 << u32(e)}
	}
	return mask, true
}

map_read_shaders :: proc(m: ^Map, out: ^Map_Shaders) -> bool {
	rasg_found := false
	for t in 0 ..< m.ntags {
		g, addr, ok := map_tag_group(m, t)
		if !ok {continue}
		name := map_tag_name(m, t)
		switch string(g[:]) {
		case "vtsh", "pixl":
			// a shader of its own (rasterizer\shaders\<name>, not a render method's): explicit unless rasg has it
			if strings.has_prefix(name, "rasterizer\\shaders\\") {
				x := out.named[name] or_else Explicit{name = strings.clone(name)}
				if string(g[:]) == "vtsh" {x.has_vs = true} else {x.has_ps = true}
				out.named[x.name] = x
			}
		case "rmdf":
			if name in out.rmdfs {continue}
			r := map_read_rmdf(m, strings.clone(name), addr) or_return
			out.rmdfs[r.name] = r
		case "rmt2":
			if name in out.templates {continue}
			base := map_at(m, addr, 0x20) or_return
			pixl := map_tag_ref(m, base + 0x10)
			if pixl < 0 {
				fmt.eprintfln("shaderpack: %s: template %s has no pixel shader", m.path, name)
				return false
			}
			if map_tag_name(m, pixl) != name {
				fmt.eprintfln("shaderpack: %s: template %s references pixl %s", m.path, name, map_tag_name(m, pixl))
				return false
			}
			mask := map_pixl_entry_points(m, pixl) or_return
			out.templates[strings.clone(name)] = Template{name = strings.clone(name), entry_points = mask, source_map = m.path}
		case "rasg":
			if rasg_found {
				fmt.eprintfln("shaderpack: %s: more than one rasterizer_globals tag", m.path)
				return false
			}
			rasg_found = true
			base := map_at(m, addr, 0xa4) or_return
			count, ptr := int(be32(m.b, base + 0x38)), be32(m.b, base + 0x3c)
			list: [dynamic]Explicit
			for i in 0 ..< count {
				e := map_at(m, ptr + u32(0x20 * i), 0x20) or_return
				vs, ps := map_tag_ref(m, e), map_tag_ref(m, e + 0x10)
				x := Explicit{has_vs = vs >= 0, has_ps = ps >= 0}
				if vs >= 0 {x.name = strings.clone(map_tag_name(m, vs))}
				if ps >= 0 {
					pn := map_tag_name(m, ps)
					if vs >= 0 && pn != x.name {
						fmt.eprintfln("shaderpack: %s: explicit shader %d: vertex %s, pixel %s", m.path, i, x.name, pn)
						return false
					}
					x.name = strings.clone(pn)
				}
				append(&list, x)
			}
			if len(out.explicit) == 0 {
				out.explicit = list
			} else {
				// every map carries the same rasterizer_globals; a difference would make explicit indices ambiguous
				same := len(list) == len(out.explicit)
				for i in 0 ..< len(list) {if !same {break}; same = list[i].name == out.explicit[i].name}
				if !same {
					fmt.eprintfln("shaderpack: %s: explicit shader table differs from an earlier map", m.path)
					return false
				}
			}
		}
	}
	return true
}
