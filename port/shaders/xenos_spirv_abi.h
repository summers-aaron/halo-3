/*
 * xenos_spirv_abi.h — the binding contract between the AOT Xenos-microcode -> SPIR-V translator
 * (port/tools/xenos_spirv, Odin package `xenos`; shader pack version 3, port/tools/shaderpack --xenos) and
 * port/sdk/d3d/vulkan_device.cpp.
 *
 * Mirrors port/tools/xenos_spirv/abi.odin (XE_ABI_VERSION is its ABI_VERSION; the pack records it). Every translated
 * module (VS, PS, memexport CS, water TES) and the two tessellation helpers use the same universal pipeline layout, so
 * one VkPipelineLayout serves all of them:
 *
 *   set 0  per-draw constants, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER(_DYNAMIC), stage flags VS|TES|FS|CS
 *          binding 0  xe_system            (sizeof 288)
 *          binding 1  float4 c[256]        VS float constants  (D3DDevice m_Constants.VertexShaderF verbatim)
 *          binding 2  float4 c[256]        PS float constants  (m_Constants.PixelShaderF verbatim)
 *          binding 3  xe_bool_loop         (VS bools 0-127 / PS 128-255; VS loops i0-15 / PS i16-31)
 *          binding 4  xe_fetch             32 Xenos texture fetch constants x 6 dwords (GPUTEXTURE_FETCH_CONSTANT)
 *                                          + what the device derives from each (xe_fetch_derive)
 *          binding 5  xe_vertex_fetch      what D3D's bind-time vfetch patch would write (VS / TES only)
 *   set 1  textures + samplers (SAMPLED_IMAGE / SAMPLER), stage flags VS|TES|FS|CS
 *          binding  0 + n   2D image view of fetch constant n (tfetch1D / tfetch2D)
 *          binding 32 + n   3D image view (tfetch3D, fetch constant dimension 3D)
 *          binding 64 + n   cube image view (tfetchCube)
 *          binding 96 + n   2D-array image view (tfetch3D, fetch constant dimension stacked 2D)
 *          binding 128 + 32*v + n  sampler variant v (XE_SAMPLER_*): the fetch constant / D3D sampler state with the
 *                                  instruction filter override; mipLodBias = the fetch constant LOD bias
 *   ABI 5 has no push constants; vertex streams and memexport destinations by VkDeviceAddress
 *   (bufferDeviceAddress, core and mandatory in Vulkan 1.3).
 *   ABI 6 replaces float bindings 1 and 2 with immutable arena chunks. Its push constants hold the arena
 *   base address and stage-local chunk, image and sampler table offsets. Set 1 uses five descriptor arrays.
 *
 * Interface locations (VS out / TES out / PS in) by semantic: TEXCOORDn -> n (0-15), COLORn -> 16+n,
 * POSITIONn -> 20+(n&3), NORMALn -> 24+(n&1), TANGENT 26, BINORMAL 27, BLENDWEIGHT 28, BLENDINDICES 29, FOG 30,
 * other 31. PS colour outputs oC0-3 -> Location 0-3, oDepth -> FragDepth.
 */
#ifndef XENOS_SPIRV_ABI_H
#define XENOS_SPIRV_ABI_H

#include <stddef.h>
#include <stdint.h>

#define XE_ABI_VERSION 5
#define XE_ABI_VERSION_BINDLESS 6

#define XE_V6_CHUNK_BYTES 64u
#define XE_V6_CHUNK_COUNT 65536u
#define XE_V6_ZERO_TABLE 0x400000u
#define XE_V6_TABLE_BYTES 128u
#define XE_V6_TABLE_START 0x400080u
#define XE_V6_IMAGES 6144u
#define XE_V6_SAMPLERS 2048u

#ifdef __cplusplus
struct alignas(8) xe_push_v6 {
    uint64_t arena_bda;
    uint32_t vs_chunk_table_off;
    uint32_t ps_chunk_table_off;
    uint32_t vs_tex_table_off;
    uint32_t ps_tex_table_off;
    uint32_t vs_samp_table_off;
    uint32_t ps_samp_table_off;
};
static_assert(sizeof(xe_push_v6) == 32, "ABI 6 push constants");
static_assert(offsetof(xe_push_v6, vs_chunk_table_off) == 8 && offsetof(xe_push_v6, ps_chunk_table_off) == 12 &&
              offsetof(xe_push_v6, vs_tex_table_off) == 16 && offsetof(xe_push_v6, ps_tex_table_off) == 20 &&
              offsetof(xe_push_v6, vs_samp_table_off) == 24 && offsetof(xe_push_v6, ps_samp_table_off) == 28,
              "ABI 6 push constant offsets");
constexpr uint32_t k_xe_v6_slots[] = {4096, 512, 512, 1024, XE_V6_SAMPLERS};
static_assert(k_xe_v6_slots[0] + k_xe_v6_slots[1] + k_xe_v6_slots[2] + k_xe_v6_slots[3] == XE_V6_IMAGES,
              "ABI 6 sampled image count");
static_assert(XE_V6_CHUNK_COUNT * XE_V6_CHUNK_BYTES == XE_V6_ZERO_TABLE, "ABI 6 arena chunk range");
#endif

#define XE_SET_CONSTANTS 0
#define XE_SET_TEXTURES 1

#define XE_BINDING_SYSTEM 0
#define XE_BINDING_FLOAT_VS 1
#define XE_BINDING_FLOAT_PS 2
#define XE_BINDING_BOOL_LOOP 3
#define XE_BINDING_FETCH 4
#define XE_BINDING_VERTEX_FETCH 5

#define XE_BINDING_TEX_2D 0
#define XE_BINDING_TEX_3D 32
#define XE_BINDING_TEX_CUBE 64
#define XE_BINDING_TEX_STACKED 96
#define XE_BINDING_SAMPLER 128
#define XE_SAMPLER_VARIANT_STRIDE 32
#define XE_SAMPLER_FETCH_CONSTANT 0   /* as the fetch constant says */
#define XE_SAMPLER_POINT 1            /* mag = min = point */
#define XE_SAMPLER_POINT_MIP_POINT 2  /* mag = min = mip = point, anisotropy off */
#define XE_SAMPLER_LINEAR 3           /* mag = min = linear */
#define XE_SAMPLER_LINEAR_MIP_LINEAR 4 /* mag = min = mip (= volume) = linear */
#define XE_SAMPLER_NO_ANISO 5         /* anisotropy off (no computed LOD / AnisoFilter=disabled) */

/* std140, 288 bytes */
#define XE_CLIP_PLANES 6
/* xe_system.alpha_test[2] (abi.odin SYS_FLAG_*): XE_SYS_FLAG_POINT_PRIMITIVE = the draw is a point list (the pixel
 * shader's param gen register gets the point sprite coordinates in zw and a negative y, as the Xenos PsParamGen) */
#define XE_SYS_FLAG_POINT_PRIMITIVE 1u
typedef struct xe_system {
    float ndc_scale[4];       /*   0 oPos.xyz *= ndc_scale.xyz                              (1,1,1,_) */
    float ndc_offset[4];      /*  16 oPos.xyz += ndc_offset.xyz * oPos.w: half-pixel offset in NDC   (0,0,0,_) */
    float pixel_scale[4];     /*  32 xy: guest pixels per host pixel (1/resolution scale; 0.5 at 2x),
                                     zw: guest window / predicated-tile origin added to VPOS (guest pixels) */
    uint32_t alpha_test[4];   /*  48 x: Xenos CompareFunction (0 never .. 7 always), y: reference (float bits),
                                     z: XE_SYS_FLAG_* */
    float color_exp_bias[4];  /*  64 oC[i] *= color_exp_bias[i] (2^RB_COLOR_INFO.exp_bias); 1 when none */
    uint32_t memexport[4][4]; /*  80 exported ranges {guest base (dwords, GPU address >> 2), size (dwords), BDA lo, BDA hi}
                                     (D3D12: z = byte offset in UAV u{entry}, w unused):
                                     a stream whose base (eA.x low 30 bits) is inside one writes at its offset */
    float point[4];           /* 144 xy: point size min/max (guest pixels), zw: guest pixel -> NDC scale (2 / the guest
                                     viewport size: the D3D12 point GS; unused by Vulkan) */
    uint32_t vertex[4];       /* 160 x: vertex index offset (int, added to gl_VertexIndex / gl_PrimitiveID; D3D12 adds it to
                                     SV_VertexID, so it also carries BaseVertexLocation / StartVertexLocation there,
                                     except for tessellated draws: SV_PrimitiveID, the patch offset alone),
                                     y: memexport CS first guest vertex, z: guest vertex count (CS bound),
                                     w: unused */
    float tessellation[4];    /* 176 xy: adaptive edge factor range (VGT_HOS_MIN / MAX_TESS_LEVEL + 1),
                                     z: the factor buffer's Xenos Endian (uint bits), for the tessellation helpers */
    float clip_planes[XE_CLIP_PLANES][4]; /* 192 user clip planes in clip space (D3DRS_CLIPPLANEENABLE); a disabled
                                     plane is all zero (distance 0: never clipped) */
} xe_system;

typedef struct xe_bool_loop {
    uint32_t bools[8];        /* bit i = Xenos b# i (VS 0-127, PS 128-255) */
    uint32_t loops[32];       /* Xenos i# (count | start << 8 | step << 16), VS 0-15, PS 16-31 */
} xe_bool_loop;

/* std140, 1280 bytes (ABI 5). derived[n] is what the shaders would otherwise decode from fetch constant n at every
 * fetch: x = the result components (bit c) that are degammaed (GPUSIGN_GAMMA through the swizzle), y = 2^exp_adjust
 * as float bits; z, w zero. */
typedef struct xe_fetch {
    uint32_t constants[32][6];
    uint32_t derived[32][4];
} xe_fetch;

/* xe_fetch.derived of one fetch constant (dwords 0-5 as the GPU reads them), as the translator's shaders used to
 * compute it: component c reads the channel its swizzle selects (dword 3 bits 1 + 3c; 4-7 are the constants 0 / 1,
 * never degammaed), whose sign is dword 0 bits 2 + 2 * channel (3: GPUSIGN_GAMMA); exp_adjust is dword 3 bits 13-18,
 * signed (2^e is exact: a float exponent). */
static inline void xe_fetch_derive(const uint32_t *constant, uint32_t *out)
{
    uint32_t mask = 0;
    for (uint32_t c = 0; c < 4; c++) {
        const uint32_t swizzle = (constant[3] >> (1u + 3u * c)) & 7u;
        if (swizzle < 4u && ((constant[0] >> (2u + 2u * swizzle)) & 3u) == 3u)
            mask |= 1u << c;
    }
    const int32_t exp_adjust = (int32_t)(constant[3] << 13) >> 26; /* bits 13-18 */
    out[0] = mask;
    out[1] = (uint32_t)(127 + exp_adjust) << 23;
    out[2] = 0;
    out[3] = 0;
}

typedef struct xe_vertex_stream {
    uint32_t address_lo, address_hi; /* VkDeviceAddress of vertex 0 of the stream (+ SetStreamSource offset); D3D12: lo =
                                        the byte offset of vertex 0 in SRV t{stream} space2 (a multiple of 4), hi unused */
    uint32_t size_bytes;             /* readable bytes from address (bounds check); bind >= 16 bytes always */
    uint32_t stride_endian;          /* stride in bytes | Xenos Endian << 16 (0 none, 1 8in16, 2 8in32, 3 16in32) */
} xe_vertex_stream;

#define XE_VF_SIGNED 1u        /* fomat_comp_all: signed */
#define XE_VF_INTEGER 2u       /* num_format_all: integer (not normalized) */
#define XE_VF_NO_ZERO 4u       /* signed_rf_mode_all: "no zero" signed normalization */
#define XE_VF_HAS_SWIZZLE 8u   /* bits 16-27 hold the declaration type's swizzle (else identity) */
#define XE_VF_EXP_SHIFT 8u     /* int8 exp_adjust at bits 8-15 */
#define XE_VF_SWIZZLE_SHIFT 16u /* GPUSWIZZLE x | y << 3 | z << 6 | w << 9 of the D3DDECLTYPE (0-3 a component, 4 zero,
                                   5 one): the Xbox runtime folds it into the vfetch destination swizzle */

typedef struct xe_vertex_element {
    uint32_t stream;          /* index into xe_vertex_fetch.stream */
    uint32_t offset_bytes;    /* element offset in the vertex */
    uint32_t format;          /* Xenos VertexFormat: 6 8_8_8_8, 7 2_10_10_10, 16 10_11_11, 17 11_11_10, 25 16_16,
                                 26 16_16_16_16, 31 16_16_FLOAT, 32 16_16_16_16_FLOAT, 33-35 32.., 36-38 32_FLOAT..,
                                 57 32_32_32_FLOAT; anything else reads zeros */
    uint32_t flags;           /* XE_VF_* */
} xe_vertex_element;

/* XE_VF_MODE_*: how xe_vertex_unpack.mode_flags decodes (bits 4-5). */
#define XE_VF_MODE_ZERO 0u    /* an unknown format (or no element): zeros */
#define XE_VF_MODE_PACKED 1u  /* fixed point: normalized by xe_vertex_fetch.denominator, or integer */
#define XE_VF_MODE_HALF 2u    /* 16-bit floats */
#define XE_VF_MODE_FLOAT 3u   /* 32-bit floats */
#define XE_VF_MODE_SHIFT 4u

/* An element's layout (ABI 5), derived by the device from its format and flags (xe_vertex_unpack_fill), so the
 * shaders decode every format with the same code: component c is bits [offset, offset + width) of word `word`. */
typedef struct xe_vertex_unpack {
    uint32_t layout;          /* component c at bits 8c: bit offset (bits 0-4) | word index << 5 */
    uint32_t widths;          /* component c's width in bits at bits 8c (0: the format lacks it, read as 0) */
    uint32_t mode_flags;      /* XE_VF_MODE_* << XE_VF_MODE_SHIFT | XE_VF_SIGNED / XE_VF_INTEGER / XE_VF_NO_ZERO (only
                                 when signed) */
    uint32_t exp_scale;       /* 2^exp_adjust as float bits (0 for no element: zeros) */
} xe_vertex_unpack;

/* std140, 1792 bytes. element[k] serves the VS's k-th container vertex element (manifest column `elements`);
 * element[31] serves a vfetch outside the patch list (zeros unless filled). unpack[k] / denominator[k] describe
 * element[k]'s format (xe_vertex_unpack_fill). */
typedef struct xe_vertex_fetch {
    xe_vertex_stream stream[16];
    xe_vertex_element element[32];
    xe_vertex_unpack unpack[32];
    float denominator[32][4]; /* per component the normalization divisor: 2^w - 1, signed 2^(w-1) - 1 (in float) */
} xe_vertex_fetch;

/* The layout of a Xenos vertex format with element flags (xe_vertex_element), as the translator's decoder used to
 * switch over them: fixed-point components low bits first, a 64-bit format's second dword its components 2-3. */
static inline void xe_vertex_unpack_fill(uint32_t format, uint32_t flags, xe_vertex_unpack *out, float *denominator)
{
    /* per format: mode, then (word << 5 | offset, width) per component */
    uint8_t layout[4] = { 0, 0, 0, 0 }, width[4] = { 0, 0, 0, 0 };
    uint32_t mode = XE_VF_MODE_PACKED;
#define XE_C(c, w, o, n) (layout[c] = (uint8_t)((w) << 5 | (o)), width[c] = (uint8_t)(n))
    switch (format) {
    case 6: XE_C(0, 0, 0, 8); XE_C(1, 0, 8, 8); XE_C(2, 0, 16, 8); XE_C(3, 0, 24, 8); break;        /* 8_8_8_8 */
    case 7: XE_C(0, 0, 0, 10); XE_C(1, 0, 10, 10); XE_C(2, 0, 20, 10); XE_C(3, 0, 30, 2); break;    /* 2_10_10_10 */
    case 16: XE_C(0, 0, 0, 11); XE_C(1, 0, 11, 11); XE_C(2, 0, 22, 10); break;                       /* 10_11_11 */
    case 17: XE_C(0, 0, 0, 10); XE_C(1, 0, 10, 11); XE_C(2, 0, 21, 11); break;                       /* 11_11_10 */
    case 25: XE_C(0, 0, 0, 16); XE_C(1, 0, 16, 16); break;                                          /* 16_16 */
    case 26: XE_C(0, 0, 0, 16); XE_C(1, 0, 16, 16); XE_C(2, 1, 0, 16); XE_C(3, 1, 16, 16); break;    /* 16_16_16_16 */
    case 33: XE_C(0, 0, 0, 32); break;                                                               /* 32 */
    case 34: XE_C(0, 0, 0, 32); XE_C(1, 1, 0, 32); break;                                           /* 32_32 */
    case 35: XE_C(0, 0, 0, 32); XE_C(1, 1, 0, 32); XE_C(2, 2, 0, 32); XE_C(3, 3, 0, 32); break;     /* 32_32_32_32 */
    case 31: mode = XE_VF_MODE_HALF; XE_C(0, 0, 0, 16); XE_C(1, 0, 16, 16); break;                  /* 16_16_FLOAT */
    case 32: mode = XE_VF_MODE_HALF; XE_C(0, 0, 0, 16); XE_C(1, 0, 16, 16); XE_C(2, 1, 0, 16); XE_C(3, 1, 16, 16); break;
    case 36: mode = XE_VF_MODE_FLOAT; XE_C(0, 0, 0, 32); break;                                     /* 32_FLOAT */
    case 37: mode = XE_VF_MODE_FLOAT; XE_C(0, 0, 0, 32); XE_C(1, 1, 0, 32); break;                  /* 32_32_FLOAT */
    case 38: mode = XE_VF_MODE_FLOAT; XE_C(0, 0, 0, 32); XE_C(1, 1, 0, 32); XE_C(2, 2, 0, 32); XE_C(3, 3, 0, 32); break;
    case 57: mode = XE_VF_MODE_FLOAT; XE_C(0, 0, 0, 32); XE_C(1, 1, 0, 32); XE_C(2, 2, 0, 32); break; /* 32_32_32_FLOAT */
    default: mode = XE_VF_MODE_ZERO; break;
    }
#undef XE_C
    const uint32_t is_signed = (flags & XE_VF_SIGNED) != 0, no_zero = is_signed && (flags & XE_VF_NO_ZERO) != 0;
    out->layout = out->widths = 0;
    for (uint32_t c = 0; c < 4; c++) {
        out->layout |= (uint32_t)layout[c] << (8u * c);
        out->widths |= (uint32_t)width[c] << (8u * c);
        /* 2^w - 1 (signed without "no zero": 2^(w-1) - 1) in float, as the GPU would form it: 2^32 - 1 rounds to 2^32 */
        const uint32_t bits = width[c] == 0 ? 0u : is_signed && !no_zero ? width[c] - 1u : width[c];
        union { uint32_t u; float f; } power;
        power.u = (127u + bits) << 23;
        denominator[c] = width[c] == 0 ? 1.0f : power.f - 1.0f;
    }
    out->mode_flags = mode << XE_VF_MODE_SHIFT | (flags & (XE_VF_SIGNED | XE_VF_INTEGER)) | (no_zero ? XE_VF_NO_ZERO : 0u);
    const int32_t exp_adjust = (int32_t)(flags << 16) >> 24; /* XE_VF_EXP_SHIFT: int8 at bits 8-15 */
    union { uint32_t u; float f; } scale;
    scale.u = exp_adjust < -126 ? 0u : (uint32_t)(127 + exp_adjust) << 23; /* below the normal range: 0 */
    out->exp_scale = mode == XE_VF_MODE_ZERO ? 0u : scale.u;
}

/* An element from a D3DDECLTYPE (MAKED3DDECLTYPE: GPUVERTEXFORMAT bits 0-5, GPUENDIAN 6-7, GPUVERTEXSIGN bit 8,
 * GPUNUMFORMAT bit 9, GPUSWIZZLE 10-21; port/sdk/d3d/xenos_vertex.h), as the Xbox runtime's bind-time patch writes
 * the vfetch instruction: format = bits 0-5; flags = XE_VF_SIGNED if bit 8, XE_VF_INTEGER if bit 9, XE_VF_HAS_SWIZZLE
 * with bits 10-21 at XE_VF_SWIZZLE_SHIFT. The endian goes to the stream (one xe_vertex_stream per D3D stream and
 * endian: the shader swaps by its stream's). */
static inline uint32_t xe_vertex_element_flags(uint32_t d3d_decl_type)
{
    return ((d3d_decl_type >> 8) & 1u ? XE_VF_SIGNED : 0u) | ((d3d_decl_type >> 9) & 1u ? XE_VF_INTEGER : 0u) |
           XE_VF_HAS_SWIZZLE | ((d3d_decl_type >> 10) & 0xfffu) << XE_VF_SWIZZLE_SHIFT;
}

/* Direct3D 12 (shader pack version 4, port/tools/shaderpack --dxil; abi.odin D3D12_*): the same contract with
 * register = binding and space = set, one version 1.1 root signature for every module (each embeds it; the pack also
 * carries it alone, s_shader_pack_file_header_v4.root_signature_module, for CreateRootSignature). Root parameters:
 * 0-5 root CBVs b0-b5 space0 (the set 0 bindings, same layouts), 6 SRV table t0-t127 space1 (the set 1 images),
 * 7 sampler table s128-s319 space1 (the set 1 samplers), 8 SRV table t0-t15 space2 (a ByteAddressBuffer per
 * xe_vertex_stream), 9 UAV table u0-u3 space2 (a RWByteAddressBuffer per xe_system.memexport entry). Every table is
 * DESCRIPTORS_VOLATILE (only the descriptors a shader names are read); 192 samplers need resource binding tier 2.
 * Interface: one fixed signature (abi.odin HLSL_INTERFACE): locations 0-27 at registers 0-27, BLENDWEIGHT .. other
 * (28-31) unsupported. Points draw through the point GS module (xe_system.point.zw). */
#define XE_D3D12_ROOT_SIGNATURE \
    "RootFlags(0), " \
    "CBV(b0, space = 0), CBV(b1, space = 0), CBV(b2, space = 0), CBV(b3, space = 0), CBV(b4, space = 0), CBV(b5, space = 0), " \
    "DescriptorTable(SRV(t0, space = 1, numDescriptors = 128, flags = DESCRIPTORS_VOLATILE)), " \
    "DescriptorTable(Sampler(s128, space = 1, numDescriptors = 192, flags = DESCRIPTORS_VOLATILE)), " \
    "DescriptorTable(SRV(t0, space = 2, numDescriptors = 16, flags = DESCRIPTORS_VOLATILE)), " \
    "DescriptorTable(UAV(u0, space = 2, numDescriptors = 4, flags = DESCRIPTORS_VOLATILE | DATA_VOLATILE))"
#define XE_D3D12_ROOT_SYSTEM 0          /* 0-5: the CBVs of XE_BINDING_SYSTEM .. XE_BINDING_VERTEX_FETCH */
#define XE_D3D12_ROOT_TEXTURES 6
#define XE_D3D12_ROOT_SAMPLERS 7
#define XE_D3D12_ROOT_VERTEX_STREAMS 8
#define XE_D3D12_ROOT_MEMEXPORT 9
#define XE_D3D12_SPACE_BUFFERS 2        /* vertex streams t0-t15, memexport u0-u3 */
#define XE_D3D12_VERTEX_STREAMS 16
#define XE_D3D12_MEMEXPORT_BUFFERS 4

#endif
