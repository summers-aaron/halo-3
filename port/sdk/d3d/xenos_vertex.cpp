// port/sdk/d3d: Halo 3's vertex declarations and the Xbox 360 vertex element types as host vertex input. See
// xenos_vertex.h for the sources and the byte-order and conversion rules.
#include "xenos_vertex.h"
#include "gpu_backend.h"
#include "gpu_crt.h"
#include "../../shaders/shader_pack.h"
#include "../../data/geometry_swap.h"

#define XENOS_D3DDECL_END { 0xff, 0, (uint32_t)k_xenos_decltype_unused, 0, 0, 0 }

static_assert(sizeof(s_xenos_d3d_vertex_element) == 12, "D3DVERTEXELEMENT9 is 12 bytes");

// The tables below are tu/graphicslibxbox/rasterizer_resource_definitions.cpp's, token for token (D3DDECLTYPE_X ->
// k_xenos_decltype_x, D3DDECLUSAGE_X -> k_xenos_declusage_x, D3DDECLMETHOD_DEFAULT -> 0); only the inner array
// bound is smaller than the game's 64.

// c_vertex_declaration_table::base_vertex_decls, by e_vertex_type (= e_shader_pack_vertex_type).
static const s_xenos_d3d_vertex_element k_base_vertex_decls[k_shader_pack_vertex_type_count][16] = {
    /* 0: k_shader_pack_vertex_world */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 24, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 28, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        XENOS_D3DDECL_END
    },
    /* 1: k_shader_pack_vertex_rigid */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 16, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        XENOS_D3DDECL_END
    },
    /* 2: k_shader_pack_vertex_skinned */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 16, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        { 0, 24, k_xenos_decltype_ubyte4, 0, k_xenos_declusage_blendindices, 0 },
        { 0, 28, k_xenos_decltype_ubyte4n, 0, k_xenos_declusage_blendweight, 0 },
        XENOS_D3DDECL_END
    },
    /* 3: k_shader_pack_vertex_particle_model */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        XENOS_D3DDECL_END
    },
    /* 4: k_shader_pack_vertex_flat_world */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 24, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 28, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        XENOS_D3DDECL_END
    },
    /* 5: k_shader_pack_vertex_flat_rigid */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 16, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        XENOS_D3DDECL_END
    },
    /* 6: k_shader_pack_vertex_flat_skinned */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 16, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        { 0, 24, k_xenos_decltype_ubyte4, 0, k_xenos_declusage_blendindices, 0 },
        { 0, 28, k_xenos_decltype_ubyte4n, 0, k_xenos_declusage_blendweight, 0 },
        XENOS_D3DDECL_END
    },
    /* 7: k_shader_pack_vertex_screen */
    {
        { 0, 0, k_xenos_decltype_float2, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 16, k_xenos_decltype_d3dcolor, 0, k_xenos_declusage_color, 0 },
        XENOS_D3DDECL_END
    },
    /* 8: k_shader_pack_vertex_debug */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_d3dcolor, 0, k_xenos_declusage_color, 0 },
        XENOS_D3DDECL_END
    },
    /* 9: k_shader_pack_vertex_transparent */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 20, k_xenos_decltype_d3dcolor, 0, k_xenos_declusage_color, 0 },
        XENOS_D3DDECL_END
    },
    /* 10: k_shader_pack_vertex_particle */
    {
        XENOS_D3DDECL_END
    },
    /* 11: k_shader_pack_vertex_contrail */
    {
        XENOS_D3DDECL_END
    },
    /* 12: k_shader_pack_vertex_light_volume */
    {
        XENOS_D3DDECL_END
    },
    /* 13: k_shader_pack_vertex_chud_simple */
    {
        { 0, 0, k_xenos_decltype_float2, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        XENOS_D3DDECL_END
    },
    /* 14: k_shader_pack_vertex_chud_fancy */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_d3dcolor, 0, k_xenos_declusage_color, 0 },
        { 0, 16, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        XENOS_D3DDECL_END
    },
    /* 15: k_shader_pack_vertex_decorator */
    {
        { 0, 0, k_xenos_decltype_ushort4n, 0, k_xenos_declusage_position, 0 },
        { 0, 8, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 12, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 1, 0, k_xenos_decltype_ushort4, 0, k_xenos_declusage_position, 1 },
        { 1, 8, k_xenos_decltype_byte4n, 0, k_xenos_declusage_normal, 1 },
        { 1, 12, k_xenos_decltype_ubyte4n, 0, k_xenos_declusage_color, 1 },
        { 2, 0, k_xenos_decltype_ushort2, 0, k_xenos_declusage_position, 2 },
        XENOS_D3DDECL_END
    },
    /* 16: k_shader_pack_vertex_tiny_position */
    {
        { 0, 0, k_xenos_decltype_short4n, 0, k_xenos_declusage_position, 0 },
        XENOS_D3DDECL_END
    },
    /* 17: k_shader_pack_vertex_patchy_fog */
    {
        { 0, 0, k_xenos_decltype_float4, 0, k_xenos_declusage_position, 0 },
        XENOS_D3DDECL_END
    },
    /* 18: k_shader_pack_vertex_water */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 20, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_normal, 0 },
        { 0, 24, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_tangent, 0 },
        { 0, 28, k_xenos_decltype_dhen3n, 0, k_xenos_declusage_binormal, 0 },
        { 4, 0, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 1 },
        { 2, 0, k_xenos_decltype_ushort2, 0, k_xenos_declusage_color, 0 },
        { 2, 4, k_xenos_decltype_ushort2, 0, k_xenos_declusage_color, 1 },
        { 2, 8, k_xenos_decltype_ushort2, 0, k_xenos_declusage_color, 2 },
        { 3, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 1 },
        { 3, 12, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 2 },
        { 3, 24, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 3 },
        XENOS_D3DDECL_END
    },
    /* 19: k_shader_pack_vertex_ripple */
    {
        { 0, 0, k_xenos_decltype_float4, 0, k_xenos_declusage_position, 0 },
        { 0, 16, k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 0 },
        { 0, 32, k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 1 },
        { 0, 48, k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 2 },
        { 0, 64, k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 3 },
        { 0, 80, k_xenos_decltype_float4, 0, k_xenos_declusage_texcoord, 4 },
        { 0, 96, k_xenos_decltype_float4, 0, k_xenos_declusage_color, 0 },
        { 0, 112, k_xenos_decltype_float4, 0, k_xenos_declusage_color, 1 },
        XENOS_D3DDECL_END
    },
    /* 20: k_shader_pack_vertex_implicit */
    {
        { 0, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_position, 0 },
        { 0, 12, k_xenos_decltype_float2, 0, k_xenos_declusage_texcoord, 0 },
        XENOS_D3DDECL_END
    },
    /* 21: k_shader_pack_vertex_beam */
    {
        XENOS_D3DDECL_END
    },
};

// c_vertex_declaration_table::lighting_vertex_decls (stream 4: lightmap texcoords and per-vertex lighting).
static const s_xenos_d3d_vertex_element k_lighting_vertex_decls[4][8] = {
    /* 0: none */
    {
        XENOS_D3DDECL_END
    },
    /* 1: lightmap_debug */
    {
        { 4, 0, k_xenos_decltype_float3, 0, k_xenos_declusage_texcoord, 3 },
        XENOS_D3DDECL_END
    },
    /* 2: per_pixel_lightmap */
    {
        { 4, 0, k_xenos_decltype_ushort2n, 0, k_xenos_declusage_texcoord, 1 },
        XENOS_D3DDECL_END
    },
    /* 3: per_vertex_lighting */
    {
        { 4, 0, k_xenos_decltype_byte4n, 0, k_xenos_declusage_texcoord, 3 },
        { 4, 4, k_xenos_decltype_byte4n, 0, k_xenos_declusage_texcoord, 4 },
        { 4, 8, k_xenos_decltype_byte4n, 0, k_xenos_declusage_texcoord, 5 },
        { 4, 12, k_xenos_decltype_byte4n, 0, k_xenos_declusage_texcoord, 6 },
        { 4, 16, k_xenos_decltype_byte4n, 0, k_xenos_declusage_texcoord, 7 },
        XENOS_D3DDECL_END
    },
};

// c_vertex_declaration_table::transfer_vertex_decls (stream 2: PRT transfer coefficients).
static const s_xenos_d3d_vertex_element k_transfer_vertex_decls[4][4] = {
    /* 0: none */
    {
        XENOS_D3DDECL_END
    },
    /* 1: prt_ambient */
    {
        { 2, 0, k_xenos_decltype_ubyte4n, 0, k_xenos_declusage_blendweight, 1 },
        XENOS_D3DDECL_END
    },
    /* 2: prt_linear */
    {
        { 2, 0, k_xenos_decltype_byte4n, 0, k_xenos_declusage_blendweight, 1 },
        XENOS_D3DDECL_END
    },
    /* 3: prt_quadratic */
    {
        { 2, 0, k_xenos_decltype_dec3n, 0, k_xenos_declusage_blendweight, 1 },
        { 2, 4, k_xenos_decltype_dec3n, 0, k_xenos_declusage_blendweight, 2 },
        { 2, 8, k_xenos_decltype_dec3n, 0, k_xenos_declusage_blendweight, 3 },
        XENOS_D3DDECL_END
    },
};

const uint32_t k_xenos_vertex_null_buffer_data[XENOS_VERTEX_NULL_BUFFER_SIZE / 4] = {
    0, 0, 0, 0x3f800000u, // float (0, 0, 0, 1)
    0, 0, 0, 0,           // int / uint (0, 0, 0, 0)
};

// GPUSWIZZLE: x, y, z, w sources of a D3DDECLTYPE.
enum {
    k_swizzle_x = 0, k_swizzle_y = 1, k_swizzle_z = 2, k_swizzle_w = 3, k_swizzle_0 = 4, k_swizzle_1 = 5,
};

static uint32_t decltype_swizzle(uint32_t d3d_type, uint32_t component)
{
    return (d3d_type >> (10u + 3u * component)) & 7u;
}

// The swizzle reads the format's components in order and fills the missing ones as a host vertex fetch does (0 for
// y and z, 1 for w).
static bool swizzle_is_identity(uint32_t d3d_type, uint32_t component_count)
{
    for (uint32_t c = 0; c < 4; c++) {
        uint32_t expected = c < component_count ? c : (c == 3 ? (uint32_t)k_swizzle_1 : (uint32_t)k_swizzle_0);
        if (decltype_swizzle(d3d_type, c) != expected)
            return false;
    }
    return true;
}

static bool swizzle_is_argb(uint32_t d3d_type)
{
    return decltype_swizzle(d3d_type, 0) == k_swizzle_z && decltype_swizzle(d3d_type, 1) == k_swizzle_y &&
           decltype_swizzle(d3d_type, 2) == k_swizzle_x && decltype_swizzle(d3d_type, 3) == k_swizzle_w;
}

static bool swizzle_is_xyz1(uint32_t d3d_type)
{
    return decltype_swizzle(d3d_type, 0) == k_swizzle_x && decltype_swizzle(d3d_type, 1) == k_swizzle_y &&
           decltype_swizzle(d3d_type, 2) == k_swizzle_z && decltype_swizzle(d3d_type, 3) == k_swizzle_1;
}

static_assert(k_gpu_format_r8g8b8a8_sint == k_gpu_format_r8g8b8a8_unorm + 5 &&
                  k_gpu_format_b8g8r8a8_sint == k_gpu_format_b8g8r8a8_unorm + 5 &&
                  k_gpu_format_a2b10g10r10_sint == k_gpu_format_a2b10g10r10_unorm + 5 &&
                  k_gpu_format_r16g16_sint == k_gpu_format_r16g16_unorm + 5 &&
                  k_gpu_format_r16g16b16a16_sint == k_gpu_format_r16g16b16a16_unorm + 5,
              "e_gpu_format keeps each family's members in order");
static_assert(k_gpu_format_r32_sint == k_gpu_format_r32_uint + 1 && k_gpu_format_r32g32_sint == k_gpu_format_r32g32_uint + 1 &&
                  k_gpu_format_r32g32b32a32_sint == k_gpu_format_r32g32b32a32_uint + 1,
              "e_gpu_format: each 32-bit SINT format follows its UINT format");

// The e_gpu_format families of 8-, 10- and 16-bit formats come in the order UNORM, SNORM, USCALED, SSCALED, UINT,
// SINT (gpu_backend.h). Picks the member for an element's sign and number format read into a shader base type; -1 if
// that read is not expressible (normalized data into an integer input, or an integer input of the other signedness).
static int family_member(bool is_signed, bool is_integer, uint32_t base_type)
{
    if (!is_integer)
        return base_type == k_xenos_vertex_base_float ? (is_signed ? 1 : 0) : -1;
    if (base_type == k_xenos_vertex_base_float)
        return is_signed ? 3 : 2;
    if (base_type == (is_signed ? (uint32_t)k_xenos_vertex_base_int : (uint32_t)k_xenos_vertex_base_uint))
        return is_signed ? 5 : 4;
    return -1;
}

extern "C" int xenos_vertex_format_from_d3d_type(uint32_t d3d_type, uint32_t base_type, s_xenos_vertex_format *out)
{
    memset(out, 0, sizeof(*out));
    if (d3d_type == (uint32_t)k_xenos_decltype_unused)
        return 0;
    uint32_t vertex_format = d3d_type & 0x3fu;
    bool is_signed = ((d3d_type >> 8) & 1u) != 0;
    bool is_integer = ((d3d_type >> 9) & 1u) != 0;  // GPUNUMFORMAT_INTEGER; FRACTION is normalized
    bool float_input = base_type == k_xenos_vertex_base_float;
    uint32_t family = k_gpu_format_undefined;
    uint32_t format = k_gpu_format_undefined;
    out->endian = (d3d_type >> 6) & 3u;
    out->conversion = k_xenos_vertex_convert_none;

    switch (vertex_format) {
    case k_xenos_vertex_format_8_8_8_8:
        out->size = 4, out->component_count = 4;
        if (swizzle_is_argb(d3d_type))
            family = k_gpu_format_b8g8r8a8_unorm;  // D3DCOLOR: 0xAARRGGBB, little-endian bytes B, G, R, A
        else if (swizzle_is_identity(d3d_type, 4))
            family = k_gpu_format_r8g8b8a8_unorm;
        break;
    case k_xenos_vertex_format_2_10_10_10:
        out->size = 4, out->component_count = 4;
        if (swizzle_is_identity(d3d_type, 4)) {
            family = k_gpu_format_a2b10g10r10_unorm;
        } else if (swizzle_is_xyz1(d3d_type)) {
            family = k_gpu_format_a2b10g10r10_unorm;
            out->conversion = k_xenos_vertex_convert_2_10_10_10_w_one;
        }
        break;
    case k_xenos_vertex_format_11_11_10:
    case k_xenos_vertex_format_10_11_11:
        out->size = 4, out->component_count = 3;
        if (swizzle_is_identity(d3d_type, 3)) {
            family = k_gpu_format_a2b10g10r10_unorm;
            out->conversion = vertex_format == k_xenos_vertex_format_11_11_10
                                  ? k_xenos_vertex_convert_11_11_10_to_10_10_10_2
                                  : k_xenos_vertex_convert_10_11_11_to_10_10_10_2;
        }
        break;
    case k_xenos_vertex_format_16_16:
        out->size = 4, out->component_count = 2;
        if (swizzle_is_identity(d3d_type, 2))
            family = k_gpu_format_r16g16_unorm;
        break;
    case k_xenos_vertex_format_16_16_16_16:
        out->size = 8, out->component_count = 4;
        if (swizzle_is_identity(d3d_type, 4))
            family = k_gpu_format_r16g16b16a16_unorm;
        break;
    case k_xenos_vertex_format_16_16_float:
        out->size = 4, out->component_count = 2;
        if (float_input && swizzle_is_identity(d3d_type, 2))
            format = k_gpu_format_r16g16_sfloat;
        break;
    case k_xenos_vertex_format_16_16_16_16_float:
        out->size = 8, out->component_count = 4;
        if (float_input && swizzle_is_identity(d3d_type, 4))
            format = k_gpu_format_r16g16b16a16_sfloat;
        break;
    case k_xenos_vertex_format_32_float:
    case k_xenos_vertex_format_32_32_float:
    case k_xenos_vertex_format_32_32_32_float:
    case k_xenos_vertex_format_32_32_32_32_float: {
        static const uint32_t formats[4] = {
            k_gpu_format_r32_sfloat,
            k_gpu_format_r32g32_sfloat,
            k_gpu_format_r32g32b32_sfloat,
            k_gpu_format_r32g32b32a32_sfloat,
        };
        uint32_t count = vertex_format == k_xenos_vertex_format_32_float          ? 1
                         : vertex_format == k_xenos_vertex_format_32_32_float     ? 2
                         : vertex_format == k_xenos_vertex_format_32_32_32_float  ? 3
                                                                                  : 4;
        out->size = 4 * count, out->component_count = count;
        if (float_input && swizzle_is_identity(d3d_type, count))
            format = formats[count - 1];
        break;
    }
    case k_xenos_vertex_format_32:
    case k_xenos_vertex_format_32_32:
    case k_xenos_vertex_format_32_32_32_32: {
        static const uint32_t uint_formats[4] = {
            k_gpu_format_r32_uint, k_gpu_format_r32g32_uint, k_gpu_format_undefined, k_gpu_format_r32g32b32a32_uint,
        };
        static const uint32_t float_formats[4] = {
            k_gpu_format_r32_sfloat, k_gpu_format_r32g32_sfloat, k_gpu_format_undefined, k_gpu_format_r32g32b32a32_sfloat,
        };
        uint32_t count = vertex_format == k_xenos_vertex_format_32 ? 1 : vertex_format == k_xenos_vertex_format_32_32 ? 2 : 4;
        out->size = 4 * count, out->component_count = count;
        if (!swizzle_is_identity(d3d_type, count))
            break;
        if (float_input) {
            // No 32-bit SCALED or NORM formats: the conversion pass turns the integers into the floats.
            format = float_formats[count - 1];
            out->conversion = k_xenos_vertex_convert_int32_to_float;
        } else if (is_integer && base_type == (is_signed ? (uint32_t)k_xenos_vertex_base_int : (uint32_t)k_xenos_vertex_base_uint)) {
            // R32*_SINT is R32*_UINT + 1.
            format = uint_formats[count - 1] + (is_signed ? 1u : 0u);
        }
        break;
    }
    default:
        break;
    }

    if (family != k_gpu_format_undefined) {
        int member = family_member(is_signed, is_integer, base_type);
        if (member >= 0)
            format = family + (uint32_t)member;
    }
    if (format == k_gpu_format_undefined) {
        memset(out, 0, sizeof(*out));
        return 0;
    }
    out->format = format;
    return 1;
}

static char upper_case(char c)
{
    return c >= 'a' && c <= 'z' ? (char)(c - 'a' + 'A') : c;
}

extern "C" int xenos_vertex_parse_semantic(const char *semantic, uint32_t *out_usage, uint32_t *out_usage_index)
{
    static const struct {
        const char *name;
        uint32_t usage;
    } k_usages[] = {
        { "POSITION", k_xenos_declusage_position },     { "BLENDWEIGHT", k_xenos_declusage_blendweight },
        { "BLENDINDICES", k_xenos_declusage_blendindices }, { "NORMAL", k_xenos_declusage_normal },
        { "PSIZE", k_xenos_declusage_psize },           { "TEXCOORD", k_xenos_declusage_texcoord },
        { "TANGENT", k_xenos_declusage_tangent },       { "BINORMAL", k_xenos_declusage_binormal },
        { "TESSFACTOR", k_xenos_declusage_tessfactor }, { "COLOR", k_xenos_declusage_color },
        { "FOG", k_xenos_declusage_fog },               { "DEPTH", k_xenos_declusage_depth },
        { "SAMPLE", k_xenos_declusage_sample },
    };
    if (semantic == 0)
        return 0;
    // Name: the semantic up to its trailing digits.
    size_t length = strlen(semantic);
    size_t name_length = length;
    while (name_length > 0 && semantic[name_length - 1] >= '0' && semantic[name_length - 1] <= '9')
        name_length--;
    if (name_length == 0 || length - name_length > 3)
        return 0;
    uint32_t index = 0;
    for (size_t i = name_length; i < length; i++)
        index = index * 10 + (uint32_t)(semantic[i] - '0');
    for (size_t u = 0; u < sizeof(k_usages) / sizeof(k_usages[0]); u++) {
        const char *name = k_usages[u].name;
        size_t i = 0;
        while (i < name_length && name[i] != 0 && upper_case(semantic[i]) == name[i])
            i++;
        if (i == name_length && name[i] == 0) {
            *out_usage = k_usages[u].usage;
            *out_usage_index = index;
            return 1;
        }
    }
    return 0;
}

// Appends a D3DDECL_END-terminated list (concat_decl). Returns false when the result would not fit.
static bool append_elements(s_xenos_vertex_declaration *out, const s_xenos_d3d_vertex_element *elements)
{
    for (uint32_t i = 0; elements[i].stream != 0xff; i++) {
        const s_xenos_d3d_vertex_element &element = elements[i];
        s_xenos_vertex_format format;
        if (out->element_count == XENOS_VERTEX_MAX_ELEMENTS || element.stream >= XENOS_VERTEX_MAX_STREAMS)
            return false;
        out->elements[out->element_count++] = element;
        out->stream_mask |= 1u << element.stream;
        // Element sizes do not depend on the shader input type; ask for a float read only to get the size.
        if (xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_float, &format) ||
            xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_uint, &format) ||
            xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_int, &format)) {
            uint32_t end = element.offset + format.size;
            if (end > out->stream_size[element.stream])
                out->stream_size[element.stream] = end;
        }
    }
    return true;
}

extern "C" int xenos_vertex_declaration_build(uint32_t vertex_type, uint32_t lighting, uint32_t transfer,
                                               s_xenos_vertex_declaration *out)
{
    memset(out, 0, sizeof(*out));
    if (vertex_type >= k_shader_pack_vertex_type_count || lighting >= 4 || transfer >= 4 ||
        (lighting != 0 && transfer == 3))
        return 0;
    if (!append_elements(out, k_base_vertex_decls[vertex_type]) ||
        !append_elements(out, k_lighting_vertex_decls[lighting]) ||
        !append_elements(out, k_transfer_vertex_decls[transfer])) {
        memset(out, 0, sizeof(*out));
        return 0;
    }
    // s_decorator_vertex (hlsl_vertex_types.fx): "instance data (stream 1)". The Xbox fetches it by index in the
    // shader; the DX11 shaders read it as instanced vertex input.
    if (vertex_type == k_shader_pack_vertex_decorator)
        out->instance_stream_mask = 1u << 1;
    return 1;
}

extern "C" int xenos_vertex_declaration_for_entry_point(uint32_t vertex_type, uint32_t entry_point,
                                                         uint32_t transfer_type, s_xenos_vertex_declaration *out)
{
    // rasterizer_get_vertex_declaration: g_rasterizer_vertex_declarations[vertex_type][lighting][transfer], by entry
    // point number: 13 (lightmap_debug_mode, the per-pixel lightmap texcoord) takes [2], 14 (static_per_vertex_color,
    // float3 TEXCOORD3) takes [1]. include/render_methods/render_method_definitions.h names 13 and 14 the other way round.
    uint32_t lighting = 0, transfer = 0;
    switch (entry_point) {
    case k_shader_pack_entry_static_per_pixel:
    case k_shader_pack_entry_lightmap_debug_mode:
        lighting = 2;
        break;
    case k_shader_pack_entry_static_per_vertex:
        lighting = 3;
        break;
    case k_shader_pack_entry_static_per_vertex_color:
        lighting = 1;
        break;
    case k_shader_pack_entry_static_prt_ambient:
    case k_shader_pack_entry_static_prt_linear:
    case k_shader_pack_entry_static_prt_quadratic:
        transfer = transfer_type;
        break;
    default:
        break;
    }
    return xenos_vertex_declaration_build(vertex_type, lighting, transfer, out);
}

extern "C" int xenos_vertex_declaration_from_d3d(const s_xenos_d3d_vertex_element *elements,
                                                  s_xenos_vertex_declaration *out)
{
    memset(out, 0, sizeof(*out));
    if (!append_elements(out, elements)) {
        memset(out, 0, sizeof(*out));
        return 0;
    }
    return 1;
}

extern "C" uint32_t xenos_vertex_input_build(const s_xenos_vertex_declaration *declaration,
                                              const s_shader_pack_input *inputs, uint32_t input_count,
                                              s_xenos_vertex_input *out, uint32_t *out_failed_input)
{
    const uint32_t k_unassigned = 0xffffffffu;
    memset(out, 0, sizeof(*out));
    out->null_binding = k_unassigned;
    if (out_failed_input != 0)
        *out_failed_input = 0;
    if (input_count > XENOS_VERTEX_MAX_ATTRIBUTES)
        return k_xenos_vertex_input_too_many_inputs;

    uint32_t used_streams = 0;
    bool needs_null = false;
    for (uint32_t i = 0; i < input_count; i++) {
        const s_shader_pack_input &input = inputs[i];
        s_xenos_vertex_attribute &attribute = out->attributes[out->attribute_count++];
        attribute.location = input.location;
        uint32_t usage, usage_index;
        const s_xenos_d3d_vertex_element *element = 0;
        if (xenos_vertex_parse_semantic(input.semantic, &usage, &usage_index)) {
            for (uint32_t e = 0; e < declaration->element_count && element == 0; e++) {
                const s_xenos_d3d_vertex_element &candidate = declaration->elements[e];
                if (candidate.usage == usage && candidate.usage_index == usage_index)
                    element = &candidate;
            }
            // s_decorator_vertex's instance data (stream 1, advancing per instance): the DX11 decorators.hlsl reads it as
            // TEXCOORD1 / 2 / 3; the D3D declaration (and the Xbox shader's vfetch) names it POSITION1 / NORMAL1 / COLOR1
            if (element == 0 && (declaration->instance_stream_mask & (1u << 1)) != 0 &&
                usage == k_xenos_declusage_texcoord && usage_index >= 1 && usage_index <= 3) {
                static const uint32_t k_instance_usage[4] = { 0, k_xenos_declusage_position, k_xenos_declusage_normal,
                                                              k_xenos_declusage_color };
                for (uint32_t e = 0; e < declaration->element_count && element == 0; e++) {
                    const s_xenos_d3d_vertex_element &candidate = declaration->elements[e];
                    if (candidate.stream == 1 && candidate.usage == k_instance_usage[usage_index] &&
                        candidate.usage_index == 1)
                        element = &candidate;
                }
            }
        }
        if (element != 0) {
            s_xenos_vertex_format format;
            if (!xenos_vertex_format_from_d3d_type(element->type, input.base_type, &format)) {
                if (out_failed_input != 0)
                    *out_failed_input = i;
                return k_xenos_vertex_input_type_mismatch;
            }
            attribute.binding = element->stream;
            attribute.format = format.format;
            attribute.offset = element->offset;
            used_streams |= 1u << element->stream;
            if (format.conversion != k_xenos_vertex_convert_none)
                out->conversion_mask |= 1u << format.conversion;
        } else {
            // No element: the null buffer (binding assigned below).
            needs_null = true;
            attribute.binding = k_unassigned;
            if (input.base_type == k_xenos_vertex_base_float) {
                attribute.format = k_gpu_format_r32g32b32a32_sfloat;
                attribute.offset = XENOS_VERTEX_NULL_FLOAT_OFFSET;
            } else {
                attribute.format = input.base_type == k_xenos_vertex_base_int ? k_gpu_format_r32g32b32a32_sint
                                                                               : k_gpu_format_r32g32b32a32_uint;
                attribute.offset = XENOS_VERTEX_NULL_INTEGER_OFFSET;
            }
        }
    }

    for (uint32_t stream = 0; stream < XENOS_VERTEX_MAX_STREAMS; stream++) {
        if ((used_streams & (1u << stream)) == 0)
            continue;
        s_xenos_vertex_binding &binding = out->bindings[out->binding_count++];
        binding.binding = stream;
        binding.stride = declaration->stream_size[stream];
        binding.input_rate = (declaration->instance_stream_mask & (1u << stream)) != 0 ? k_xenos_vertex_rate_instance
                                                                                       : k_xenos_vertex_rate_vertex;
    }
    if (needs_null) {
        // The highest stream the declaration does not use, clear of the game's low stream numbers.
        for (uint32_t stream = XENOS_VERTEX_MAX_STREAMS; stream-- > 0 && out->null_binding == k_unassigned;)
            if ((declaration->stream_mask & (1u << stream)) == 0)
                out->null_binding = stream;
        if (out->null_binding == k_unassigned)
            return k_xenos_vertex_input_no_free_binding;
        for (uint32_t a = 0; a < out->attribute_count; a++)
            if (out->attributes[a].binding == k_unassigned)
                out->attributes[a].binding = out->null_binding;
        s_xenos_vertex_binding &binding = out->bindings[out->binding_count++];
        binding.binding = out->null_binding;
        binding.stride = 0;
        binding.input_rate = k_xenos_vertex_rate_vertex;
    }
    return k_xenos_vertex_input_ok;
}

// ---- data conversion ----


// Only the 11-bit fields need requantizing. For normalized inputs the scale is just below 1/2,
// so nearest rounding truncates the half-integer toward zero. The most negative SNORM clamps.
static uint32_t requantize_11_to_10(uint32_t value, bool is_signed, bool is_integer)
{
    value &= 0x7ffu;
    if (!is_signed || is_integer)
        return value >> 1; // logical and arithmetic shifts have the same low ten bits
    const int32_t signed_value = (int32_t)(value << 21) >> 21;
    const int32_t rounded = signed_value == -1024 ? -511 : signed_value / 2;
    return (uint32_t)rounded & 0x3ffu;
}

static uint32_t convert_packed_word(uint32_t word, uint32_t conversion, bool is_signed, bool is_integer)
{
    const uint32_t w_one = (is_signed || is_integer ? 1u : 3u) << 30;
    switch (conversion) {
    case k_xenos_vertex_convert_2_10_10_10_w_one:
        return (word & 0x3fffffffu) | w_one;
    case k_xenos_vertex_convert_11_11_10_to_10_10_10_2:
        return (word & 0x3ffu) |
            requantize_11_to_10(word >> 10, is_signed, is_integer) << 10 |
            requantize_11_to_10(word >> 21, is_signed, is_integer) << 20 | w_one;
    case k_xenos_vertex_convert_10_11_11_to_10_10_10_2:
        return requantize_11_to_10(word, is_signed, is_integer) |
            requantize_11_to_10(word >> 11, is_signed, is_integer) << 10 |
            (word >> 22) << 20 | w_one;
    default:
        return word;
    }
}

template <unsigned endian>
static void swap_word_range(uint8_t *out, const uint8_t *in, size_t count)
{
    typedef uint32_t words4 __attribute__((vector_size(16)));
    for (; count >= 4; count -= 4, in += 16, out += 16) {
        words4 w;
        __builtin_memcpy(&w, in, sizeof(w));
        if (endian == 1 || endian == 2)
            w = ((w & 0x00ff00ffu) << 8) | ((w >> 8) & 0x00ff00ffu);
        if (endian == 2 || endian == 3)
            w = (w >> 16) | (w << 16);
        __builtin_memcpy(out, &w, sizeof(w));
    }
    for (; count != 0; count--, in += 4, out += 4) {
        uint32_t w;
        __builtin_memcpy(&w, in, sizeof(w));
        if (endian == 1)
            w = ((w & 0x00ff00ffu) << 8) | ((w >> 8) & 0x00ff00ffu);
        else if (endian == 2)
            w = __builtin_bswap32(w);
        else
            w = (w >> 16) | (w << 16);
        __builtin_memcpy(out, &w, sizeof(w));
    }
}

// `count` 32-bit words from `in` to `out`, each given the Xenos fetch swap of its element's GPUENDIAN `endian` (1 8in16,
// 2 8in32, 3 16in32; 0 none). The mode is chosen once, outside the loop, so each loop is a plain copy or a straight-line
// swap the compiler can unroll and vectorize.
static void swap_words(uint8_t *out, const uint8_t *in, size_t count, uint32_t endian)
{
    switch (endian) {
    case 1:
        swap_word_range<1>(out, in, count);
        break;
    case 2:
        swap_word_range<2>(out, in, count);
        break;
    case 3:
        swap_word_range<3>(out, in, count);
        break;
    default:
        if (out != in)
            memcpy(out, in, count * 4u);
        break;
    }
}

// A stream converted word by word: every element of a 4-aligned layout swaps in units of at most 4 bytes, so a
// vertex is a run of words, each swapped by its element's GPUENDIAN (or copied), then the few packed formats are
// requantized. Returns false for a layout it does not cover (the element-by-element loop below converts those).
static bool convert_stream_by_words(const s_xenos_vertex_declaration *declaration, uint32_t stream, const uint8_t *in,
                                    uint8_t *out, uint32_t vertex_count, uint32_t stride, uint32_t big_endian)
{
    enum { k_max_words = 64 };
    if (stride == 0 || (stride & 3u) != 0 || stride > 4u * k_max_words)
        return false;
    const uint32_t words = stride / 4u;
    uint8_t endian[k_max_words] = {};
    struct s_requantize {
        uint32_t offset, conversion;
        bool is_signed, is_integer;
    } requantize[XENOS_VERTEX_MAX_ELEMENTS];
    uint32_t requantize_count = 0;
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &element = declaration->elements[e];
        if (element.stream != stream)
            continue;
        s_xenos_vertex_format format;
        if (!xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_float, &format) &&
            !xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_uint, &format) &&
            !xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_int, &format))
            continue;
        if (element.offset + format.size > stride)
            continue;
        const uint32_t size = format.size;
        if ((element.offset & 3u) != 0 || (size & 3u) != 0)
            return false;
        if (big_endian && size != 0)
            for (uint32_t w = element.offset / 4u; w < (element.offset + size) / 4u; w++)
                endian[w] = (uint8_t)((element.type >> 6) & 3u);
        if (format.conversion != k_xenos_vertex_convert_none && format.conversion != k_xenos_vertex_convert_int32_to_float) {
            if (requantize_count == XENOS_VERTEX_MAX_ELEMENTS)
                return false;
            requantize[requantize_count++] = { element.offset, format.conversion, ((element.type >> 8) & 1u) != 0,
                                               ((element.type >> 9) & 1u) != 0 };
        }
    }
    // the vertex as runs of words that swap alike; a single run (all words alike, as in an all-float layout, or a
    // host-order stream with nothing to swap) converts the whole range as one array
    uint32_t run_start[k_max_words], run_length[k_max_words], run_mode[k_max_words], runs = 0;
    for (uint32_t w = 0; w < words; w++) {
        if (runs != 0 && run_mode[runs - 1] == endian[w]) {
            run_length[runs - 1]++;
            continue;
        }
        run_start[runs] = w;
        run_length[runs] = 1;
        run_mode[runs] = endian[w];
        runs++;
    }
    // `out` is usually the frame ring, mapped write-combined: it is written once and never read back (a load from it
    // is uncached, and the requantize pass's loads were most of the conversion's time). Layouts with packed formats
    // are built a vertex at a time on the stack, requantized there, then written.
    if (requantize_count == 0 && runs == 1) {
        swap_words(out, in, (size_t)vertex_count * words, run_mode[0]);
        return true;
    }
    if (requantize_count == 0) {
        for (uint32_t v = 0; v < vertex_count; v++)
            for (uint32_t r = 0; r < runs; r++)
                swap_words(out + (size_t)v * stride + 4u * run_start[r], in + (size_t)v * stride + 4u * run_start[r],
                           run_length[r], run_mode[r]);
        return true;
    }
    uint8_t vertex[4u * k_max_words];
    for (uint32_t v = 0; v < vertex_count; v++) {
        for (uint32_t r = 0; r < runs; r++)
            swap_words(vertex + 4u * run_start[r], in + (size_t)v * stride + 4u * run_start[r], run_length[r], run_mode[r]);
        for (uint32_t r = 0; r < requantize_count; r++) {
            const s_requantize &q = requantize[r];
            uint32_t w;
            __builtin_memcpy(&w, vertex + q.offset, 4);
            w = convert_packed_word(w, q.conversion, q.is_signed, q.is_integer);
            __builtin_memcpy(vertex + q.offset, &w, 4);
        }
        memcpy(out + (size_t)v * stride, vertex, stride);
    }
    return true;
}

extern "C" void xenos_vertex_convert_stream(const s_xenos_vertex_declaration *declaration, uint32_t stream,
                                             const void *src, void *dst, uint32_t vertex_count, uint32_t stride,
                                             uint32_t big_endian)
{
    if (vertex_count == 0 || stride == 0)
        return;
    const uint8_t *in = (const uint8_t *)src;
    uint8_t *const destination = (uint8_t *)dst;
    if (convert_stream_by_words(declaration, stream, in, destination, vertex_count, stride, big_endian))
        return;
    // the layouts the word plan does not cover are converted in place, in scratch memory: `dst` is usually the
    // write-combined frame ring, which is written once and not read back
    const size_t bytes = (size_t)vertex_count * stride;
    uint8_t *scratch = (uint8_t *)malloc(bytes ? bytes : 1u);
    uint8_t *out = scratch != 0 ? scratch : destination;
    if (out != in)
        memcpy(out, in, bytes);
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &element = declaration->elements[e];
        if (element.stream != stream)
            continue;
        s_xenos_vertex_format format;
        if (!xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_float, &format) &&
            !xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_uint, &format) &&
            !xenos_vertex_format_from_d3d_type(element.type, k_xenos_vertex_base_int, &format))
            continue;
        if (element.offset + format.size > stride)
            continue;
        uint8_t *first = out + element.offset;
        if (big_endian && format.endian != 0)
            for (uint32_t v = 0; v < vertex_count; v++)
                port_geometry_swap_element(element.type, first + (size_t)v * stride);
        if (format.conversion != k_xenos_vertex_convert_none && format.conversion != k_xenos_vertex_convert_int32_to_float) {
            const bool is_signed = ((element.type >> 8) & 1u) != 0;
            const bool is_integer = ((element.type >> 9) & 1u) != 0;
            for (uint32_t v = 0; v < vertex_count; v++) {
                uint8_t *p = first + (size_t)v * stride;
                uint32_t w;
                __builtin_memcpy(&w, p, 4);
                w = convert_packed_word(w, format.conversion, is_signed, is_integer);
                __builtin_memcpy(p, &w, 4);
            }
        }
    }
    if (scratch != 0) {
        memcpy(destination, scratch, bytes);
        free(scratch);
    }
}

extern "C" int xenos_vertex_stream_is_packed_prt(const s_xenos_vertex_declaration *declaration, uint32_t stream,
                                                  uint32_t stride)
{
    if (stream != 2 || stride != 4)
        return 0;
    uint32_t elements = 0, packed = 0;
    for (uint32_t e = 0; e < declaration->element_count; e++) {
        const s_xenos_d3d_vertex_element &element = declaration->elements[e];
        if (element.stream != stream)
            continue;
        elements++;
        if (element.type == k_xenos_decltype_ubyte4n && element.offset == 0 &&
            element.usage == k_xenos_declusage_blendweight && element.usage_index == 1)
            packed++;
    }
    return elements == 1 && packed == 1;
}

extern "C" void xenos_vertex_expand_packed_prt(const void *src, void *dst, uint32_t value_count, uint32_t big_endian)
{
    const uint8_t *in = (const uint8_t *)src;
    uint8_t *out = (uint8_t *)dst;
    // The big-endian fetch swap cancels the reversal of the Xenos wzyx packing.
    uint32_t v = 0;
    for (; value_count - v >= 4u; v += 4, in += 4, out += 16) {
        uint32_t packed;
        __builtin_memcpy(&packed, in, sizeof(packed));
        if (!big_endian)
            packed = __builtin_bswap32(packed);
        const uint32_t expanded[4] = { packed & 255u, (packed >> 8) & 255u, (packed >> 16) & 255u, packed >> 24 };
        __builtin_memcpy(out, expanded, sizeof(expanded));
    }
    if (v == value_count)
        return;
    uint8_t word[4] = { 0, 0, 0, 0 };
    const uint32_t left = value_count - v;
    memcpy(word, in, left);
    for (uint32_t i = 0; i < left; i++) {
        const uint32_t expanded = word[big_endian ? i : 3u - i];
        __builtin_memcpy(out + 4u * i, &expanded, sizeof(expanded));
    }
}
