package xenos

import "core:fmt"
import "core:strings"

// HLSL backend: the declarations every module shares (the D3D12 binding of abi.odin) and the fixed host modules the
// D3D12 device pairs with translated shaders, the HLSL forms of helpers.odin (tessellation VS / HS, rectangle-list GS)
// plus the point-sprite GS D3D12 needs (Vulkan rasterizes sprites itself) and the root signature alone.

HLSL_PRELUDE :: "#define XE_ROOT_SIGNATURE \"" + D3D12_ROOT_SIGNATURE + "\"\n" + `
cbuffer XeSystem : register(b0, space0) {
	float4 xe_ndc_scale;
	float4 xe_ndc_offset;
	float4 xe_pixel_scale;
	uint4 xe_alpha_test;
	float4 xe_color_exp_bias;
	uint4 xe_memexport_ranges[4];
	float4 xe_point;
	uint4 xe_vertex;
	float4 xe_tessellation;
	float4 xe_clip_planes[6];
};
cbuffer XeBoolLoop : register(b3, space0) {
	uint4 xe_b[2];
	uint4 xe_i[8];
};
cbuffer XeFetch : register(b4, space0) {
	uint4 xe_f[48];
	uint4 xe_fetch_derived[32];
};

// SPIR-V OpBitFieldUExtract / OpBitFieldSExtract (offset + count <= 32; count 0 reads 0)
uint xe_ubfe(uint x, uint offset, uint count) {
	return count == 0u ? 0u : ((x >> offset) & (0xFFFFFFFFu >> (32u - count)));
}
int xe_sbfe(uint x, uint offset, uint count) {
	return count == 0u ? 0 : (int(x << (32u - offset - count)) >> (32u - count));
}

// Xenos Endian: 0 none, 1 8in16, 2 8in32, 3 16in32
uint4 xe_endian_swap(uint4 w, uint e) {
	uint4 s1 = ((w & 0x00FF00FFu) << 8u) | ((w >> 8u) & 0x00FF00FFu);
	uint4 s2 = (s1 << 16u) | (s1 >> 16u);
	uint4 s3 = (w << 16u) | (w >> 16u);
	uint4 r = (e == 1u) ? s1 : w;
	r = (e == 2u) ? s2 : r;
	r = (e == 3u) ? s3 : r;
	return r;
}

`

HLSL_FLOAT_VS :: `cbuffer XeFloatVS : register(b1, space0) {
	float4 xe_c[256];
};
`

HLSL_FLOAT_PS :: `cbuffer XeFloatPS : register(b2, space0) {
	float4 xe_c[256];
};
`

HLSL_VERTEX_FETCH :: `cbuffer XeVertexFetch : register(b5, space0) {
	uint4 xe_stream[16];
	uint4 xe_element[32];
	uint4 xe_unpack[32];
	float4 xe_denominator[32];
};
ByteAddressBuffer xe_vertex_streams[16] : register(t0, space2);
`

HLSL_MEMEXPORT :: `RWByteAddressBuffer xe_memexport_buffers[4] : register(u0, space2);
`

// The fixed stage interface (abi.odin D3D12): ABI location n is slot l<n>. XeInterface: the VS / DS / GS output and the
// GS input; XePSInput: the pixel shader input, the same elements but the trailing clip distances (consumed by the
// rasterizer), so its 31 rows with SV_IsFrontFace fit the 32 a pixel shader has, and DXC's prefix-stable packing gives
// every shared element the same register in both.
HLSL_INTERFACE :: "struct XeInterface {\n" + HLSL_INTERFACE_COMMON + "\tfloat4 clip_distance_0 : SV_ClipDistance0;\n\tfloat2 clip_distance_1 : SV_ClipDistance1;\n};\n\n"
HLSL_PS_INPUT :: "struct XePSInput {\n" + HLSL_INTERFACE_COMMON + "};\n\n"
HLSL_INTERFACE_COMMON :: `	float4 l0 : TEXCOORD0;
	float4 l1 : TEXCOORD1;
	float4 l2 : TEXCOORD2;
	float4 l3 : TEXCOORD3;
	float4 l4 : TEXCOORD4;
	float4 l5 : TEXCOORD5;
	float4 l6 : TEXCOORD6;
	float4 l7 : TEXCOORD7;
	float4 l8 : TEXCOORD8;
	float4 l9 : TEXCOORD9;
	float4 l10 : TEXCOORD10;
	float4 l11 : TEXCOORD11;
	float4 l12 : TEXCOORD12;
	float4 l13 : TEXCOORD13;
	float4 l14 : TEXCOORD14;
	float4 l15 : TEXCOORD15;
	float4 l16 : COLOR0;
	float4 l17 : COLOR1;
	float4 l18 : COLOR2;
	float4 l19 : COLOR3;
	float4 l20 : POSITION0;
	float4 l21 : POSITION1;
	float4 l22 : POSITION2;
	float4 l23 : POSITION3;
	float4 l24 : NORMAL0;
	float4 l25 : NORMAL1;
	float4 l26 : TANGENT0;
	float4 l27 : BINORMAL0;
	float2 point_coord : XESPRITETEXCOORD;
	float point_size : XEPSIZE;
	float4 position : SV_Position;
`

HLSL_TESS_TYPES :: `struct XeTessControlPoint {
	float edge_factor : XETESSFACTOR;
};
struct XeTessFactors {
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

`

// The root signature alone: dxc -T rootsig_1_1 -E XE_ROOT_SIGNATURE.
hlsl_root_signature :: proc(allocator := context.allocator) -> string {
	return strings.clone("// the D3D12 root signature of every translated module (abi.odin D3D12_ROOT_SIGNATURE)\n#define XE_ROOT_SIGNATURE \"" + D3D12_ROOT_SIGNATURE + "\"\n", allocator)
}

// The adaptive tessellation vertex shader (as helper_tess_vs): the "index" is a factor's bits (SV_VertexID is the
// index buffer's value), byte-swapped by XeSystem.tessellation.z, + 1, clamped to tessellation.xy.
hlsl_helper_tess_vs :: proc(allocator := context.allocator) -> string {
	return strings.clone(HLSL_PRELUDE + HLSL_TESS_TYPES + `[RootSignature(XE_ROOT_SIGNATURE)]
XeTessControlPoint main(uint xe_vertex_id : SV_VertexID) {
	float4 tess = xe_tessellation;
	uint endian = asuint(tess.z);
	uint w = xe_vertex_id;
	uint s8in16 = ((w & 0x00FF00FFu) << 8u) | ((w >> 8u) & 0x00FF00FFu);
	uint s16in32 = (w << 16u) | (w >> 16u);
	uint s8in32 = (s8in16 << 16u) | (s8in16 >> 16u);
	uint r = (endian == 1u) ? s8in16 : w;
	r = (endian == 2u) ? s8in32 : r;
	r = (endian == 3u) ? s16in32 : r;
	float factor = asfloat(r) + 1.0;
	XeTessControlPoint o;
	o.edge_factor = clamp(factor, tess.x, tess.y);
	return o;
}
`, allocator)
}

// The adaptive tessellation hull shader (as helper_tess_tcs): guest edge 0 is v0 -> v1, 1 v1 -> v2, 2 v2 -> v0;
// SV_TessFactor 0 is the U0 edge (v1 -> v2), 1 the V0 edge (v2 -> v0), 2 the W0 edge (v0 -> v1) as in Vulkan; the
// inside factor is the least of the edges.
hlsl_helper_tess_hs :: proc(allocator := context.allocator) -> string {
	return strings.clone(HLSL_PRELUDE + HLSL_TESS_TYPES + `XeTessFactors xe_patch_constants(InputPatch<XeTessControlPoint, 3> xe_ip) {
	XeTessFactors o;
	o.edges[0] = xe_ip[1].edge_factor;
	o.edges[1] = xe_ip[2].edge_factor;
	o.edges[2] = xe_ip[0].edge_factor;
	o.inside = min(min(xe_ip[0].edge_factor, xe_ip[1].edge_factor), xe_ip[2].edge_factor);
	return o;
}

[RootSignature(XE_ROOT_SIGNATURE)]
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(1)]
[patchconstantfunc("xe_patch_constants")]
XeTessControlPoint main(InputPatch<XeTessControlPoint, 3> xe_ip, uint xe_id : SV_OutputControlPointID) {
	return xe_ip[0];
}
`, allocator)
}

// The rectangle-list geometry shader for vertex shaders writing `locations` (as helper_rect_gs): the longest edge
// picks the strip order, the fourth vertex is v1 - v0 + v2 for the position, the clip distances and every
// interpolator of the set (the others stay zero).
hlsl_helper_rect_gs :: proc(locations: u32, allocator := context.allocator) -> (text: string, err: string) {
	sb := strings.builder_make(allocator)
	strings.write_string(&sb, HLSL_PRELUDE)
	strings.write_string(&sb, HLSL_INTERFACE)
	fmt.sbprintf(&sb, "// interpolators: 0x%08X\n", locations)
	strings.write_string(&sb, "[RootSignature(XE_ROOT_SIGNATURE)]\n[maxvertexcount(4)]\nvoid main(triangle XeInterface xe_v[3], inout TriangleStream<XeInterface> xe_stream) {\n")
	// squared edge lengths: [0] 12, [1] 20, [2] 01
	for k in 0 ..< 3 {
		fmt.sbprintf(&sb, "\tfloat4 e%d_p0 = xe_v[%d].position;\n\tfloat4 e%d_p1 = xe_v[%d].position;\n", k, (1 + k) % 3, k, (2 + k) % 3)
		fmt.sbprintf(&sb, "\tfloat e%d_dx = e%d_p1.x - e%d_p0.x;\n\tfloat e%d_dy = e%d_p1.y - e%d_p0.y;\n", k, k, k, k, k, k)
		fmt.sbprintf(&sb, "\tfloat edge%d = (e%d_dx * e%d_dx) + (e%d_dy * e%d_dy);\n", k, k, k, k, k)
	}
	strings.write_string(&sb, "\tint first = ((edge0 > edge1) && (edge0 > edge2)) ? 0 : ((edge1 > edge2) ? 1 : 2);\n")
	strings.write_string(&sb, "\tint vi0 = first;\n")
	for k in 1 ..< 3 {fmt.sbprintf(&sb, "\tint n%d = first + %d;\n\tint vi%d = (n%d < 3) ? n%d : (n%d - 3);\n", k, k, k, k, k, k)}
	strings.write_string(&sb, "\tXeInterface o;\n")
	for k in 0 ..< 3 {
		strings.write_string(&sb, "\to = (XeInterface)0;\n")
		for l in u32(0) ..< 32 {
			if locations & (1 << l) == 0 {continue}
			if l >= HLSL_INTERFACE_SLOTS {return "", fmt.tprintf("D3D12 interface: location %d has no slot", l)}
			fmt.sbprintf(&sb, "\to.l%d = xe_v[vi%d].l%d;\n", l, k, l)
		}
		fmt.sbprintf(&sb, "\to.position = xe_v[vi%d].position;\n", k)
		fmt.sbprintf(&sb, "\to.clip_distance_0 = xe_v[vi%d].clip_distance_0;\n\to.clip_distance_1 = xe_v[vi%d].clip_distance_1;\n", k, k)
		strings.write_string(&sb, "\txe_stream.Append(o);\n")
	}
	// c - a + d (v1 - v0 + v2)
	strings.write_string(&sb, "\to = (XeInterface)0;\n")
	for l in u32(0) ..< 32 {
		if locations & (1 << l) == 0 {continue}
		fmt.sbprintf(&sb, "\to.l%d = (xe_v[vi1].l%d - xe_v[vi0].l%d) + xe_v[vi2].l%d;\n", l, l, l, l)
	}
	strings.write_string(&sb, "\to.position = (xe_v[vi1].position - xe_v[vi0].position) + xe_v[vi2].position;\n")
	strings.write_string(&sb, "\to.clip_distance_0 = (xe_v[vi1].clip_distance_0 - xe_v[vi0].clip_distance_0) + xe_v[vi2].clip_distance_0;\n")
	strings.write_string(&sb, "\to.clip_distance_1 = (xe_v[vi1].clip_distance_1 - xe_v[vi0].clip_distance_1) + xe_v[vi2].clip_distance_1;\n")
	strings.write_string(&sb, "\txe_stream.Append(o);\n\txe_stream.RestartStrip();\n}\n")
	return strings.to_string(sb), ""
}

// The point-sprite geometry shader (D3D12 only; Vulkan rasterizes PointSize itself): a square of XEPSIZE host pixels
// around the point (XeSystem.point.zw: guest pixel -> NDC, 2 / the guest viewport size; pixel_scale: guest pixels per
// host pixel), XESPRITETEXCOORD s left to right and t top to bottom as gl_PointCoord, every interpolator, and the
// clip distances of the point (a point clipped by a plane is dropped whole, as Vulkan clips points).
hlsl_helper_point_gs :: proc(allocator := context.allocator) -> string {
	return strings.clone(HLSL_PRELUDE + HLSL_INTERFACE + `[RootSignature(XE_ROOT_SIGNATURE)]
[maxvertexcount(4)]
void main(point XeInterface xe_v[1], inout TriangleStream<XeInterface> xe_stream) {
	XeInterface c = xe_v[0];
	float2 radius = (((0.5 * c.point_size) * xe_pixel_scale.xy) * xe_point.zw) * c.position.w;
	const float2 corners[4] = { float2(-1.0, 1.0), float2(1.0, 1.0), float2(-1.0, -1.0), float2(1.0, -1.0) };
	[unroll] for (uint i = 0u; i < 4u; ++i) {
		XeInterface o = c;
		o.position.xy = c.position.xy + (corners[i] * radius);
		o.point_coord = float2((corners[i].x * 0.5) + 0.5, 0.5 - (corners[i].y * 0.5));
		xe_stream.Append(o);
	}
	xe_stream.RestartStrip();
}
`, allocator)
}
