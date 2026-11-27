package xenos

// The ABI 5 Vulkan layout between translated Xenos shaders and port/sdk/d3d/vulkan_device.cpp.
//
// set 0 — per-draw constants (UNIFORM_BUFFER, all stages; ring slices, ideally UNIFORM_BUFFER_DYNAMIC):
//   binding 0  XeSystem       system constants (struct below, SYS_SIZE bytes)
//   binding 1  XeFloatVS      vec4 c[256]   (D3DDevice m_Constants.VertexShaderF, verbatim)
//   binding 2  XeFloatPS      vec4 c[256]   (m_Constants.PixelShaderF, verbatim)
//   binding 3  XeBoolLoop     uvec4 b[2] (256 bool bits: VS 0-127, PS 128-255), uvec4 i[8] (32 loop constants:
//                             VS i0-15, PS i16-31; count | start << 8 | step << 16)
//   binding 4  XeFetch        uvec4 f[48]  the 32 Xenos texture fetch constants (6 dwords each, GPUTEXTURE_FETCH_
//                             CONSTANT as the D3D runtime keeps them; the shader reads sizes and exp_adjust)
//   binding 5  XeVertexFetch  the vertex declaration the D3D runtime would patch into the vfetch instructions:
//                             uvec4 stream[16] {address lo, address hi (VkDeviceAddress of vertex 0 of the stream),
//                             size in bytes, stride in bytes | endian << 16 (Xenos Endian: 0 none, 1 8in16, 2 8in32,
//                             3 16in32)}, uvec4 element[32] {stream, offset in bytes, Xenos VertexFormat,
//                             flags: bit 0 signed, bit 1 integer (unnormalized), bit 2 signed RF "no zero", bit 3
//                             declaration swizzle valid, bits 8-15 exp_adjust (int8), bits 16-27 the declaration
//                             type swizzle (VF_*)} — element k = the VS's k-th vertex element (container order)
// set 1 — textures and samplers (fragment + vertex + compute):
//   binding  0 + n   texture2D       tfetch1D/2D on fetch constant n (0..31)
//   binding 32 + n   texture3D       tfetch3D when the fetch constant is 3D
//   binding 64 + n   textureCube     tfetchCube
//   binding 96 + n   texture2DArray  tfetch3D when the fetch constant is stacked 2D
//   binding 128 + 32 v + n  sampler  variant v (Sampler_Variant: fetch constant, point, point+mip point, linear,
//                            linear+mip linear, no aniso); mipLodBias = the fetch constant LOD bias
// No set 2: vertex streams and memexport destinations are reached by buffer device address (Vulkan 1.3 core).
// No push constants. Specialization constants: none required.
//
// Interface locations (VS outputs / PS inputs) come from the container semantics, not from o# / r# numbers, so any
// VS links with any PS without the D3D runtime's export patching:
//   TEXCOORD0-15 -> 0-15, COLOR0-3 -> 16-19, POSITION n -> 20 + n%4, NORMAL n -> 24 + n%2, TANGENT -> 26,
//   BINORMAL -> 27, BLENDWEIGHT -> 28, BLENDINDICES -> 29, FOG -> 30, other -> 31.
// PS colour outputs: oC0-3 -> Location 0-3; oDepth -> FragDepth.

SET_CONSTANTS :: 0
SET_TEXTURES :: 1

BINDING_SYSTEM :: 0
BINDING_FLOAT_VS :: 1
BINDING_FLOAT_PS :: 2
BINDING_BOOL_LOOP :: 3
BINDING_FETCH :: 4
BINDING_VERTEX_FETCH :: 5

BINDING_TEX_2D :: 0
BINDING_TEX_3D :: 32
BINDING_TEX_CUBE :: 64
BINDING_TEX_STACKED :: 96
BINDING_SAMPLER :: 128 // + SAMPLER_VARIANT_STRIDE * variant + fetch constant
SAMPLER_VARIANT_STRIDE :: 32

// Samplers per fetch constant: the fetch constant / D3D sampler state, with the tfetch instruction's filter overrides
// applied (the combinations retail Halo 3 uses; others fall back to the nearest)
Sampler_Variant :: enum u32 {
	Fetch_Constant    = 0, // as the fetch constant says
	Point             = 1, // mag = min = point (mip as the fetch constant)
	Point_Mip_Point   = 2, // mag = min = mip = point, anisotropy off
	Linear            = 3, // mag = min = linear
	Linear_Mip_Linear = 4, // mag = min = mip = linear (and volume filters)
	No_Aniso          = 5, // anisotropy off (fetch without a computed LOD, or AnisoFilter=disabled)
}

// XeSystem member indices (std140 offsets in the comment)
Sys :: enum u32 {
	Ndc_Scale = 0, //  0 vec4: oPos.xyz *= scale        (identity 1,1,1)
	Ndc_Offset, //    16 vec4: oPos.xyz += offset * oPos.w  (half-pixel offset in NDC; independent of host size)
	Pixel_Scale, //   32 vec4: xy = guest pixels per host pixel (1/resolution scale), zw = guest window offset
	Alpha_Test, //    48 uvec4: x = Xenos CompareFunction (7 always), y = alpha reference (float bits), z = flags (SYS_FLAG_*)
	Color_Exp_Bias, // 64 vec4: per render target colour multiplier (2^exp_bias); 1 for none
	Memexport, //     80 uvec4[4]: {guest base dwords, size dwords, address lo, address hi}: an exported range; a stream
	//                   whose base (eA.x low 30 bits) falls inside writes at address + (base - entry base) * 4
	Point, //        144 vec4: xy point size min/max (guest pixels), zw guest pixel -> NDC scale
	Vertex, //       160 uvec4: x vertex index offset (int), y compute vertex base, z compute vertex count
	Tessellation, // 176 vec4: xy adaptive edge factor range (VGT_HOS_MIN / MAX_TESS_LEVEL + 1), z factor endian (uint)
	Clip_Planes, //  192 vec4[6]: user clip planes in clip space (PA_CL_UCP); zero when disabled (never clipped)
}
SYS_SIZE :: 288
SYS_CLIP_PLANES :: 6
SYS_MEMBER_OFFSETS :: [10]u32{0, 16, 32, 48, 64, 80, 144, 160, 176, 192}

@(private)
sys_row :: proc(member: Sys) -> u32 {
	offsets := SYS_MEMBER_OFFSETS
	return offsets[u32(member)] / 16
}

SYS_FLAG_POINT_PRIMITIVE :: 1 // a point list: param gen y negative, zw = the sprite coordinates (io.odin)
SYS_FLAG_ALPHA_TO_COVERAGE :: 2

interface_location :: proc(usage: Decl_Usage, index: u32) -> u32 {
	#partial switch usage {
	case .Texcoord: return index & 15
	case .Color: return 16 + (index & 3)
	case .Position: return 20 + (index & 3)
	case .Normal: return 24 + (index & 1)
	case .Tangent: return 26
	case .Binormal: return 27
	case .Blend_Weight: return 28
	case .Blend_Indices: return 29
	case .Fog: return 30
	}
	return 31
}

// XeVertexFetch element flags (element[k].w): what the D3DDECLTYPE of the declaration element says, as the Xbox D3D
// runtime's bind-time patch writes it into the vfetch instruction (signedness, number format, signed RF mode, exp
// adjust), plus the declaration type's own component swizzle (GPUSWIZZLE x | y << 3 | z << 6 | w << 9: 0-3 a fetched
// component, 4 zero, 5 one), which the runtime folds into the instruction's destination swizzle (D3DCOLOR's ZYXW,
// FLOAT3's XYZ1). VF_HAS_SWIZZLE off: identity.
VF_SIGNED :: 1
VF_INTEGER :: 2
VF_NO_ZERO :: 4
VF_HAS_SWIZZLE :: 8
VF_EXP_SHIFT :: 8
VF_SWIZZLE_SHIFT :: 16

// XeVertexFetch.unpack[k] (version 5): element k's layout, which the device derives from its format and flags
// (xe_vertex_unpack_fill in xenos_spirv_abi.h), so the shader decodes every format with the same straight-line code
// instead of a switch over the formats at each vfetch.
//   x: component c's bit offset in its word (bits 0-4) and word index (bits 5-6), at bits 8c
//   y: component c's width in bits (0: the format lacks it, read as 0), at bits 8c
//   z: VF_MODE_* | VF_SIGNED / VF_INTEGER / VF_NO_ZERO as in the element flags (no zero only when signed)
//   w: the exp_adjust scale 2^exp as float bits (0 for an element nothing fills: reads zeros)
// XeVertexFetch.denominator[k]: per component the normalization divisor (2^w - 1, signed 2^(w-1) - 1), in float.
VF_MODE_ZERO :: 0 // an unknown format: zeros
VF_MODE_PACKED :: 1 // fixed point, normalized or integer
VF_MODE_HALF :: 2 // 16-bit floats
VF_MODE_FLOAT :: 3 // 32-bit floats
VF_MODE_SHIFT :: 4 // the mode at bits 4-5 of unpack.z

// XeFetch.derived[n] (version 5), from fetch constant n: x = the result components (bit c) a GPUSIGN_GAMMA channel
// feeds through the fetch constant's swizzle (degammaed), y = 2^exp_adjust as float bits.

// The binding contract's version: the shader pack (version 3) records it, and the device refuses a pack of another.
// 1: tmp/ucode_spirv's first contract (XeSystem 176 bytes); 2: XeSystem.tessellation and .clip_planes (288 bytes),
// declaration swizzles in XeVertexFetch element flags, adaptive tessellation helpers; 3: memexport ranges; 4: GPUSIGN_GAMMA
// fetches degammaed in the shader (the device keeps the raw codes in the image); 5: the device derives the degamma
// component mask and exp_adjust scale of each fetch constant (XeFetch.derived) and the layout of each vertex element
// (XeVertexFetch.unpack / .denominator).
ABI_VERSION :: 5
ABI_VERSION_BINDLESS :: 6

// ABI 6 keeps the XeSystem, XeBoolLoop, XeFetch and XeVertexFetch UBOs. Each float bank
// is a table of 64 packed u16 chunk IDs, and each descriptor table is a list of u32 slots.
// The offsets in XePush are bytes from the same 64-byte-aligned arena base.
Push_Member :: enum u32 {
	Arena_BDA,
	VS_Chunk_Table, PS_Chunk_Table,
	VS_Tex_Table, PS_Tex_Table,
	VS_Samp_Table, PS_Samp_Table,
}
PUSH_MEMBER_OFFSETS :: [7]u32{0, 8, 12, 16, 20, 24, 28}
BINDLESS_SAMPLER :: 4 // image bindings are the TEX_* dimension indices (0..3)
IMAGE_KEY_LIMIT :: 4 * 32
SAMPLER_KEY_LIMIT :: (u32(Sampler_Variant.No_Aniso) + 1) * 32

// Wire records for container v3 / ABI 6. The packer fills the key slices in module order.
Module_Meta :: struct {
	direct_float_mask:    [8]u32,
	relative_lo:         u16,
	relative_hi_exclusive: u16,
	bool_mask:           [8]u32,
	loop_mask:           u32,
	system_rows:         u32,
	fetch_rows:          [3]u32,
	vertex_fetch_rows:   [4]u32,
	first_image_key:     u32,
	first_sampler_key:   u32,
	image_count:         u16,
	sampler_count:       u16,
	reserved_zero:       u32,
}

Entry_Overlay :: struct {
	literal_row_mask: [8]u32,
}

#assert(size_of(Module_Meta) == 120)
#assert(size_of(Entry_Overlay) == 32)

// ---- Direct3D 12 (the HLSL backend, hlsl.odin; shader pack version 4, shaderpack --dxil) ----
//
// The same contract, register = Vulkan binding and space = Vulkan set, so the D3D12 device fills the same blocks and
// tables as vulkan_device.cpp. One root signature (D3D12_ROOT_SIGNATURE, version 1.1, every stage, compute included)
// serves every module; each DXIL module embeds it (RTS0, which DXC checks the module's bindings against), and the pack
// carries it alone (rootsig_1_1) for CreateRootSignature:
//   root parameter 0-5  root CBV b0-b5 space0: XeSystem, XeFloatVS, XeFloatPS, XeBoolLoop, XeFetch, XeVertexFetch
//                       (the Vulkan set 0 bindings 0-5, same std140 = HLSL cbuffer layouts; ring slices by GPU VA)
//   root parameter 6    table SRV t0-t127 space1: Texture2D t(0 + n), Texture3D t(32 + n), TextureCube t(64 + n),
//                       Texture2DArray t(96 + n) of fetch constant n (Vulkan set 1 bindings 0-127)
//   root parameter 7    table Sampler s128-s319 space1: s(128 + 32 v + n), variant v of fetch constant n (set 1
//                       bindings 128-319; MipLODBias = the fetch constant LOD bias as Vulkan's mipLodBias)
//   root parameter 8    table SRV t0-t15 space2: ByteAddressBuffer per vertex stream (XeVertexFetch.stream[s]: x =
//                       the byte offset of vertex 0 in that view, y unused; z / w as Vulkan) — Vulkan's buffer device
//                       addresses, which D3D12 lacks
//   root parameter 9    table UAV u0-u3 space2: RWByteAddressBuffer per XeSystem.memexport entry (entry e writes u{e}
//                       at byte offset entry.z + dwords * 4; w unused)
// 16 DWORDs of the 64. Tables are DESCRIPTORS_VOLATILE (a shader reads only the descriptors it names: the device
// may leave the others unwritten); resource binding tier 2 (192 samplers in a table). No input layout: every vertex
// is fetched by the shader (SV_VertexID, which excludes BaseVertexLocation / StartVertexLocation where Vulkan's
// gl_VertexIndex includes them: the device adds them to XeSystem.vertex.x).
//
// Stage interface: D3D12 links stages by signature register, so every VS / DS output and PS input is one fixed
// struct (XeInterface, HLSL_INTERFACE, 32 registers): ABI locations 0-27 as float4 slots l0-l27 with their semantics
// (TEXCOORD0-15, COLOR0-3, POSITION0-3, NORMAL0-1, TANGENT0, BINORMAL0; unwritten slots are zero), the point sprite
// coordinates XESPRITETEXCOORD and the host point diameter XEPSIZE (one register), SV_Position, SV_ClipDistance0-1
// (the six user clip planes). Locations 28-31 (BLENDWEIGHT, BLENDINDICES, FOG, other) have no slot: no retail shader
// uses them, and the translator refuses one that would. D3D12 has no point sprites: a point list draws through the
// point GS helper (XeSystem.point.zw = the guest pixel -> NDC scale, 2 / the guest viewport size, which the D3D12
// device fills), which expands XEPSIZE into a quad with XESPRITETEXCOORD; points are front-facing as in Vulkan.
// Execution models: vertex vs_6_0, fragment ps_6_0, memexport-only vertex shaders cs_6_0 [numthreads(64, 1, 1)],
// the tessellated water ds_6_0 (domain tri; helpers: tess VS vs_6_0, tess HS hs_6_0 fractional_even / triangle_cw).
// Memexport UAVs u0-u3 are used by cs_6_0 modules and pixel shaders only: no vs_6_0 module declares one.
//
// For the D3D12 device (not checked by the pack build):
//   tessellated draws: the DS adds XeSystem.vertex.x to SV_PrimitiveID (the patch index, as the Vulkan TES adds it to
//     gl_PrimitiveID): it is the draw's patch offset there, never BaseVertexLocation;
//   front face: vulkan_backend.cpp's negative-height viewport gives Vulkan D3D's framebuffer orientation, so
//     FrontCounterClockwise = (front_face == VK_FRONT_FACE_COUNTER_CLOCKWISE) keeps SV_IsFrontFace = gl_FrontFacing;
//   sampler LOD bias: Vulkan's mipLodBias also biases explicit LODs (SampleLevel / OpImageSampleExplicitLod: vertex
//     fetches, fetches without computed LOD); D3D12's MipLODBias on SampleLevel is unverified (Xenia's D3D12 backend
//     sets MipLODBias = 0 and biases in the shader). If it differs, the fetch constant bias moves into the shader
//     (from XeFetch, both backends, an ABI bump);
//   derivatives: HLSL ddx / ddy (getGradients, the 3D layer gradients) compile to coarse derivatives in DXIL, where
//     SPIR-V OpDPdx / OpDPdy leave coarse or fine to the implementation;
//   fp32 denormals: shader model 6.0 leaves them to the driver (-denorm needs 6.2), as the SPIR-V does without
//     float-controls execution modes.
// HLSL min / max / clamp / saturate are IEEE minNum / maxNum (GLSL NMin / NMax / NClamp); where the SPIR-V uses
// FMin / FMax / FClamp (NaN undefined), the DXIL result is one of the results Vulkan allows.
D3D12_ROOT_SIGNATURE ::
	"RootFlags(0), " +
	"CBV(b0, space = 0), CBV(b1, space = 0), CBV(b2, space = 0), CBV(b3, space = 0), CBV(b4, space = 0), CBV(b5, space = 0), " +
	"DescriptorTable(SRV(t0, space = 1, numDescriptors = 128, flags = DESCRIPTORS_VOLATILE)), " +
	"DescriptorTable(Sampler(s128, space = 1, numDescriptors = 192, flags = DESCRIPTORS_VOLATILE)), " +
	"DescriptorTable(SRV(t0, space = 2, numDescriptors = 16, flags = DESCRIPTORS_VOLATILE)), " +
	"DescriptorTable(UAV(u0, space = 2, numDescriptors = 4, flags = DESCRIPTORS_VOLATILE | DATA_VOLATILE))"

D3D12_ROOT_SYSTEM :: 0 // root parameters 0-5: the CBVs of BINDING_SYSTEM .. BINDING_VERTEX_FETCH
D3D12_ROOT_TEXTURES :: 6
D3D12_ROOT_SAMPLERS :: 7
D3D12_ROOT_VERTEX_STREAMS :: 8
D3D12_ROOT_MEMEXPORT :: 9
D3D12_SPACE_BUFFERS :: 2 // vertex streams t0-t15, memexport u0-u3
D3D12_VERTEX_STREAMS :: 16
D3D12_MEMEXPORT_BUFFERS :: 4
