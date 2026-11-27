#ifndef PORT_D3D_XENOS_VERTEX_H
#define PORT_D3D_XENOS_VERTEX_H

/*
 * Halo 3's vertex declarations, and the mapping from Xbox 360 vertex element types (D3DDECLTYPE) to host vertex
 * formats (e_gpu_format, gpu_backend.h): the version 2 (DX11) shader pack's fixed-function vertex input, which a
 * backend turns into its own (vulkan_pack.cpp: a VkPipelineVertexInputStateCreateInfo), and the declaration lookups the
 * version 3 / 4 vertex fetch shares. Backend-neutral: no API types.
 *
 * Sources:
 *   - The declarations are the game's own: c_vertex_declaration_table::base_vertex_decls[22], lighting_vertex_decls[4]
 *     and transfer_vertex_decls[4] in tu/graphicslibxbox/rasterizer_resource_definitions.cpp, combined as
 *     initialize_vertex_declarations / concat_decl do (base, then lighting, then transfer) and selected per entry
 *     point as rasterizer_get_vertex_declaration does. Streams, offsets, types, usages and usage indices are copied
 *     verbatim; they are authoritative for formats and offsets.
 *   - The HLSL vertex structs (tmp/shaders-work/hlsl/hlsl_vertex_types.fx, the MCC DX11 shader source) agree on the
 *     semantics; their DX11 types (float3 NORMAL, uint4 BLENDINDICES) are what the compiled vertex shaders consume.
 *     Types 10, 11, 12 and 21 (particle, contrail, light volume, beam) have no declaration: their shaders fetch
 *     from buffers by vertex index (SV_VertexID), so they take no vertex input.
 *   - D3DDECLTYPE bit layout: the XDK's d3d9types.h (D3DDECLTYPE_*_SHIFT, MAKED3DDECLTYPE2) and d3d9gpu.h
 *     (GPUVERTEXFORMAT, GPUENDIAN, GPUSWIZZLE). Packed format bit positions: xenia
 *     (src/xenia/gpu/spirv_shader_translator_fetch.cc), confirmed by Bungie's UnpackDHEN3N (packed_vector.fx):
 *     11_11_10 is X in bits 0..9 (10 bits), Y in 10..20, Z in 21..31; 10_11_11 is X 0..10, Y 11..21, Z 22..31.
 *
 * Byte order. Xbox vertex data is big-endian and the Xenos fetch swaps it per element with the element's GPUENDIAN:
 * 8in16 for 16-bit components (SHORT*, USHORT*, FLOAT16_*), 8in32 for everything else (FLOAT*, 8_8_8_8, the packed
 * 32-bit formats). The renderer takes host-order data. Paged render geometry is converted per element when its
 * page is loaded (port/data/resource_swap.h, GAME-CHANGES E1); until that hook is in, buffers still in Xenos order
 * (d3d_port_buffer.h) are swapped per element on upload with port_geometry_swap_element (port/data/geometry_swap.h):
 * 16-bit pairs swapped as one 32-bit word would come out in YXWZ order. Vertices the game writes at run time are
 * already in host order. The swap unit of an element is s_xenos_vertex_format::endian.
 *
 * Formats with no exact Vulkan equivalent are mapped to the closest 32-bit-in-place format, so strides and offsets
 * stay the game's, and flagged in s_xenos_vertex_format::conversion for the load-time conversion pass (which runs
 * after the byte swap). In Halo 3's tables that is only DHEN3N (normals, tangents, binormals) and DEC3N (PRT
 * quadratic coefficients); see e_xenos_vertex_conversion.
 *
 * Signed normalized components: Vulkan's SNORM maps -2^(n-1) and -(2^(n-1)-1) both to -1.0 and otherwise
 * x / (2^(n-1)-1), which is the Xenos fetch's "clamp to -1" signed mode and Bungie's x/511, x/1023 unpacking.
 *
 * Host C/C++ with C linkage; every structure here has only 1-, 2- and 4-byte members, so its layout is the same for
 * the game's MS-ABI code and for host code.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct s_shader_pack_input;

/* ---- Xbox 360 D3DDECLTYPE ---- */

/* MAKED3DDECLTYPE (d3d9types.h): GPUVERTEXFORMAT | GPUENDIAN << 6 | GPUVERTEXSIGN << 8 | GPUNUMFORMAT << 9 |
 * swizzle << 10, the swizzle being GPUSWIZZLE x | y << 3 | z << 6 | w << 9. */
#define XENOS_VERTEX_DECLTYPE(format, endian, sign, numformat, swizzle)                                         \
    ((uint32_t)(format) | (uint32_t)(endian) << 6 | (uint32_t)(sign) << 8 | (uint32_t)(numformat) << 9 |        \
     (uint32_t)(swizzle) << 10)
#define XENOS_VERTEX_SWIZZLE(x, y, z, w) ((x) | (y) << 3 | (z) << 6 | (w) << 9)

/* GPUVERTEXFORMAT (d3d9gpu.h). */
enum e_xenos_vertex_format {
    k_xenos_vertex_format_8_8_8_8           = 6,
    k_xenos_vertex_format_2_10_10_10        = 7,
    k_xenos_vertex_format_10_11_11          = 16,
    k_xenos_vertex_format_11_11_10          = 17,
    k_xenos_vertex_format_16_16             = 25,
    k_xenos_vertex_format_16_16_16_16       = 26,
    k_xenos_vertex_format_16_16_float       = 31,
    k_xenos_vertex_format_16_16_16_16_float = 32,
    k_xenos_vertex_format_32                = 33,
    k_xenos_vertex_format_32_32             = 34,
    k_xenos_vertex_format_32_32_32_32       = 35,
    k_xenos_vertex_format_32_float          = 36,
    k_xenos_vertex_format_32_32_float       = 37,
    k_xenos_vertex_format_32_32_32_32_float = 38,
    k_xenos_vertex_format_32_32_32_float    = 57,
};

/* GPUENDIAN of a vertex element: the unit its bytes are swapped in. */
enum e_xenos_vertex_endian {
    k_xenos_vertex_endian_none  = 0,
    k_xenos_vertex_endian_8in16 = 1,
    k_xenos_vertex_endian_8in32 = 2,
    k_xenos_vertex_endian_16in32 = 3,
};

/* GPUSWIZZLE values, and the two swizzle groups of the D3DDECLTYPEs. */
#define XENOS_VERTEX_SWIZZLE_XYZW XENOS_VERTEX_SWIZZLE(0, 1, 2, 3) /* GPUSWIZZLE_ABGR: identity */
#define XENOS_VERTEX_SWIZZLE_ZYXW XENOS_VERTEX_SWIZZLE(2, 1, 0, 3) /* GPUSWIZZLE_ARGB: D3DCOLOR */
#define XENOS_VERTEX_SWIZZLE_XYZ1 XENOS_VERTEX_SWIZZLE(0, 1, 2, 5)
#define XENOS_VERTEX_SWIZZLE_XY01 XENOS_VERTEX_SWIZZLE(0, 1, 4, 5)
#define XENOS_VERTEX_SWIZZLE_X001 XENOS_VERTEX_SWIZZLE(0, 4, 4, 5)

/* Every D3DDECLTYPE of d3d9types.h. */
enum e_xenos_decl_type {
    k_xenos_decltype_float1    = XENOS_VERTEX_DECLTYPE(36, 2, 1, 1, XENOS_VERTEX_SWIZZLE_X001),
    k_xenos_decltype_float2    = XENOS_VERTEX_DECLTYPE(37, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_float3    = XENOS_VERTEX_DECLTYPE(57, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_float4    = XENOS_VERTEX_DECLTYPE(38, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_int1      = XENOS_VERTEX_DECLTYPE(33, 2, 1, 1, XENOS_VERTEX_SWIZZLE_X001),
    k_xenos_decltype_int2      = XENOS_VERTEX_DECLTYPE(34, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_int4      = XENOS_VERTEX_DECLTYPE(35, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_uint1     = XENOS_VERTEX_DECLTYPE(33, 2, 0, 1, XENOS_VERTEX_SWIZZLE_X001),
    k_xenos_decltype_uint2     = XENOS_VERTEX_DECLTYPE(34, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_uint4     = XENOS_VERTEX_DECLTYPE(35, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_int1n     = XENOS_VERTEX_DECLTYPE(33, 2, 1, 0, XENOS_VERTEX_SWIZZLE_X001),
    k_xenos_decltype_int2n     = XENOS_VERTEX_DECLTYPE(34, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_int4n     = XENOS_VERTEX_DECLTYPE(35, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_uint1n    = XENOS_VERTEX_DECLTYPE(33, 2, 0, 0, XENOS_VERTEX_SWIZZLE_X001),
    k_xenos_decltype_uint2n    = XENOS_VERTEX_DECLTYPE(34, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_uint4n    = XENOS_VERTEX_DECLTYPE(35, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_d3dcolor  = XENOS_VERTEX_DECLTYPE(6, 2, 0, 0, XENOS_VERTEX_SWIZZLE_ZYXW),
    k_xenos_decltype_ubyte4    = XENOS_VERTEX_DECLTYPE(6, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_byte4     = XENOS_VERTEX_DECLTYPE(6, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_ubyte4n   = XENOS_VERTEX_DECLTYPE(6, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_byte4n    = XENOS_VERTEX_DECLTYPE(6, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_short2    = XENOS_VERTEX_DECLTYPE(25, 1, 1, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_short4    = XENOS_VERTEX_DECLTYPE(26, 1, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_ushort2   = XENOS_VERTEX_DECLTYPE(25, 1, 0, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_ushort4   = XENOS_VERTEX_DECLTYPE(26, 1, 0, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_short2n   = XENOS_VERTEX_DECLTYPE(25, 1, 1, 0, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_short4n   = XENOS_VERTEX_DECLTYPE(26, 1, 1, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_ushort2n  = XENOS_VERTEX_DECLTYPE(25, 1, 0, 0, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_ushort4n  = XENOS_VERTEX_DECLTYPE(26, 1, 0, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_udec3     = XENOS_VERTEX_DECLTYPE(7, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_dec3      = XENOS_VERTEX_DECLTYPE(7, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_udec3n    = XENOS_VERTEX_DECLTYPE(7, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_dec3n     = XENOS_VERTEX_DECLTYPE(7, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_udec4     = XENOS_VERTEX_DECLTYPE(7, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_dec4      = XENOS_VERTEX_DECLTYPE(7, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_udec4n    = XENOS_VERTEX_DECLTYPE(7, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_dec4n     = XENOS_VERTEX_DECLTYPE(7, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_uhend3    = XENOS_VERTEX_DECLTYPE(16, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_hend3     = XENOS_VERTEX_DECLTYPE(16, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_uhend3n   = XENOS_VERTEX_DECLTYPE(16, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_hend3n    = XENOS_VERTEX_DECLTYPE(16, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_udhen3    = XENOS_VERTEX_DECLTYPE(17, 2, 0, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_dhen3     = XENOS_VERTEX_DECLTYPE(17, 2, 1, 1, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_udhen3n   = XENOS_VERTEX_DECLTYPE(17, 2, 0, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_dhen3n    = XENOS_VERTEX_DECLTYPE(17, 2, 1, 0, XENOS_VERTEX_SWIZZLE_XYZ1),
    k_xenos_decltype_float16_2 = XENOS_VERTEX_DECLTYPE(31, 1, 1, 1, XENOS_VERTEX_SWIZZLE_XY01),
    k_xenos_decltype_float16_4 = XENOS_VERTEX_DECLTYPE(32, 1, 1, 1, XENOS_VERTEX_SWIZZLE_XYZW),
    k_xenos_decltype_unused    = (int)0xffffffff,
};

/* D3DDECLUSAGE (d3d9types.h). */
enum e_xenos_decl_usage {
    k_xenos_declusage_position     = 0,
    k_xenos_declusage_blendweight  = 1,
    k_xenos_declusage_blendindices = 2,
    k_xenos_declusage_normal       = 3,
    k_xenos_declusage_psize        = 4,
    k_xenos_declusage_texcoord     = 5,
    k_xenos_declusage_tangent      = 6,
    k_xenos_declusage_binormal     = 7,
    k_xenos_declusage_tessfactor   = 8,
    k_xenos_declusage_color        = 10,
    k_xenos_declusage_fog          = 11,
    k_xenos_declusage_depth        = 12,
    k_xenos_declusage_sample       = 13,
};

/* One element: the layout of D3DVERTEXELEMENT9 (12 bytes), so the game's tables and the elements it passes to
 * D3DDevice_CreateVertexDeclaration read as these directly (11 bytes of members, padded to 12 as there). A
 * declaration ends at stream 0xff. */
struct s_xenos_d3d_vertex_element {
    uint16_t stream;
    uint16_t offset;
    uint32_t type;        /* e_xenos_decl_type (D3DDECLTYPE) */
    uint8_t  method;      /* D3DDECLMETHOD_DEFAULT (0) */
    uint8_t  usage;       /* e_xenos_decl_usage */
    uint8_t  usage_index;
};

#define XENOS_VERTEX_MAX_ELEMENTS 32
#define XENOS_VERTEX_MAX_STREAMS  16 /* D3D streams 0..15 = vertex input bindings 0..15 (Vulkan's maxVertexInputBindings,
                                        D3D12's input slots: at least 16) */

/* A whole declaration, as the backend uses it. */
struct s_xenos_vertex_declaration {
    uint32_t element_count;
    uint32_t stream_mask;           /* bit s: some element reads stream s */
    uint32_t instance_stream_mask;  /* bit s: stream s advances per instance (decorators: stream 1) */
    uint32_t stream_size[XENOS_VERTEX_MAX_STREAMS]; /* bytes the elements of each stream span (the minimum stride) */
    struct s_xenos_d3d_vertex_element elements[XENOS_VERTEX_MAX_ELEMENTS];
};

/* What a data conversion pass must do to an element (after the byte swap) for its host format to read the
 * values the Xenos fetch would. */
enum e_xenos_vertex_conversion {
    k_xenos_vertex_convert_none = 0,
    /* 2_10_10_10 read as 3 components (DEC3N, UDEC3, ...): the Vulkan A2B10G10R10 formats expose the 2-bit W the
     * Xenos swizzle replaces with 1. Set bits 30..31 to 1's encoding (01 signed or scaled, 11 unsigned normalized);
     * X, Y, Z are bit-exact. */
    k_xenos_vertex_convert_2_10_10_10_w_one = 1,
    /* 11_11_10 (DHEN3N, ...: X 10 bits, Y 11, Z 11) to 10_10_10_2 (A2B10G10R10_*): X kept, Y and Z requantized from
     * 11 to 10 bits (snorm: round(v * 511 / 1023); unorm: round(v * 1023 / 2047); scaled: v >> 1 with the sign kept
     * -- integer DHEN3 values above 10 bits cannot be kept), W = 1 as above. In place: strides and offsets are
     * unchanged. The cost is one bit of Y and Z precision (2^-10 for a unit normal). A bit-exact alternative is a
     * vertex shader that takes the element as R32_UINT and unpacks it (UnpackDHEN3N). */
    k_xenos_vertex_convert_11_11_10_to_10_10_10_2 = 2,
    /* 10_11_11 (HEND3N, ...: X 11, Y 11, Z 10) to 10_10_10_2: X and Y requantized as above, Z kept, W = 1. */
    k_xenos_vertex_convert_10_11_11_to_10_10_10_2 = 3,
    /* 32-bit integer elements read as floats (INT*, UINT*, INT*N, UINT*N with a float shader input): Vulkan has no
     * 32-bit scaled or normalized formats. Convert each component to the float the fetch would produce
     * (normalized: x / (2^31-1) clamped to -1, or x / (2^32-1)), read as R32*_SFLOAT. */
    k_xenos_vertex_convert_int32_to_float = 4,
};

/* The host side of one element type. */
struct s_xenos_vertex_format {
    uint32_t format;          /* e_gpu_format */
    uint32_t size;            /* bytes the element occupies */
    uint32_t component_count; /* components the Xenos format has (before the swizzle fills the rest with 0 / 1) */
    uint32_t endian;          /* e_xenos_vertex_endian: the byte-swap unit for big-endian source data */
    uint32_t conversion;      /* e_xenos_vertex_conversion */
};

/* Component type a vertex shader input is declared with (e_shader_pack_input_type). */
enum e_xenos_vertex_base_type {
    k_xenos_vertex_base_float = 0,
    k_xenos_vertex_base_int   = 1,
    k_xenos_vertex_base_uint  = 2,
};

/* The host format (e_gpu_format) reading a D3DDECLTYPE into a shader input of the given e_xenos_vertex_base_type. Integer
 * element types (UBYTE4, USHORT2, ...) read into float inputs use the SCALED formats (the Xenos integer fetch gives
 * the integer's value as a float); into int/uint inputs, the SINT/UINT format of the element's signedness. Returns 0
 * (and leaves *out zeroed) for an unknown type, a normalized or float type read into an integer input, or an
 * integer type read into an integer input of the other signedness. */
int xenos_vertex_format_from_d3d_type(uint32_t d3d_type, uint32_t base_type, struct s_xenos_vertex_format *out);

/* Splits an HLSL semantic ("POSITION", "TEXCOORD3", "color1"; case-insensitive) into D3DDECLUSAGE and usage index
 * (a missing index is 0). Returns 0 for a semantic with no D3DDECLUSAGE (SV_*, INDEX, ...). */
int xenos_vertex_parse_semantic(const char *semantic, uint32_t *out_usage, uint32_t *out_usage_index);

/* Halo 3's tables (c_vertex_declaration_table): lighting 0 none, 1 lightmap debug (TEXCOORD3 float3), 2 per-pixel
 * lightmap (TEXCOORD1), 3 per-vertex lighting (TEXCOORD3..7); transfer 0 none, 1..3 PRT ambient, linear, quadratic
 * (BLENDWEIGHT1..3). Returns 0 for indices out of range. As initialize_vertex_declarations, lighting != 0 with
 * transfer 3 does not exist. */
int xenos_vertex_declaration_build(uint32_t vertex_type, uint32_t lighting, uint32_t transfer,
                                    struct s_xenos_vertex_declaration *out);

/* rasterizer_get_vertex_declaration: the declaration a render method entry point (e_shader_pack_entry_point) draws
 * a vertex type with; transfer_type (e_transfer_vector_vertex_types, 0..3) only matters for the PRT entry points. */
int xenos_vertex_declaration_for_entry_point(uint32_t vertex_type, uint32_t entry_point, uint32_t transfer_type,
                                              struct s_xenos_vertex_declaration *out);

/* A declaration from D3DVERTEXELEMENT9s ending at stream 0xff (what D3DDevice_CreateVertexDeclaration receives).
 * No stream is per-instance. Returns 0 if there are more than XENOS_VERTEX_MAX_ELEMENTS elements or a stream is
 * out of range. */
int xenos_vertex_declaration_from_d3d(const struct s_xenos_d3d_vertex_element *elements,
                                       struct s_xenos_vertex_declaration *out);

/* ---- pipeline vertex input ---- */

#define XENOS_VERTEX_MAX_ATTRIBUTES 32
/* The null vertex buffer every unmatched input reads (stride 0): the floats (0, 0, 0, 1) at offset 0 for float
 * inputs, zeros at offset 16 for int/uint inputs. */
#define XENOS_VERTEX_NULL_FLOAT_OFFSET 0
#define XENOS_VERTEX_NULL_INTEGER_OFFSET 16
#define XENOS_VERTEX_NULL_BUFFER_SIZE 32

/* A vertex input binding (a D3D stream) and attribute (a shader input), as the APIs describe them (all 4-byte
 * members). */
enum e_xenos_vertex_input_rate {
    k_xenos_vertex_rate_vertex = 0,
    k_xenos_vertex_rate_instance = 1,
};
struct s_xenos_vertex_binding {
    uint32_t binding;
    uint32_t stride;
    uint32_t input_rate;  /* e_xenos_vertex_input_rate */
};
struct s_xenos_vertex_attribute {
    uint32_t location;
    uint32_t binding;
    uint32_t format;      /* e_gpu_format */
    uint32_t offset;
};

/* A graphics pipeline's vertex input state: one attribute per vertex shader input. */
struct s_xenos_vertex_input {
    uint32_t binding_count;
    uint32_t attribute_count;
    uint32_t null_binding;     /* binding of the null buffer, or 0xffffffff when every input has an element */
    uint32_t conversion_mask;  /* bit c: some matched element needs e_xenos_vertex_conversion c */
    struct s_xenos_vertex_binding bindings[XENOS_VERTEX_MAX_STREAMS];
    struct s_xenos_vertex_attribute attributes[XENOS_VERTEX_MAX_ATTRIBUTES];
};

enum e_xenos_vertex_input_result {
    k_xenos_vertex_input_ok = 0,
    k_xenos_vertex_input_too_many_inputs = 1,      /* more than XENOS_VERTEX_MAX_ATTRIBUTES */
    k_xenos_vertex_input_type_mismatch = 2,         /* element type cannot be read into the input's base type */
    k_xenos_vertex_input_no_free_binding = 3,       /* every stream is used, none is left for the null buffer */
};

/*
 * Matches each vertex shader input (by semantic) with the declaration element of the same usage and usage index.
 * Matched inputs read their element at its stream (binding), offset and host format. An input with no element, or
 * with a semantic that is not a D3DDECLUSAGE, reads the null buffer: the backend binds a small buffer of
 * (0, 0, 0, 1) / zeros with stride 0 at null_binding (the highest stream the declaration leaves unused), so every
 * vertex sees the D3D default value instead of the pipeline failing. Binding strides are the declaration's
 * stream_size; the backend's pipelines take the real stride at bind time (dynamic vertex input binding stride).
 * On failure, *out_failed_input (if not NULL) is the index of the offending input.
 */
uint32_t xenos_vertex_input_build(const struct s_xenos_vertex_declaration *declaration,
                                   const struct s_shader_pack_input *inputs, uint32_t input_count,
                                   struct s_xenos_vertex_input *out, uint32_t *out_failed_input);

/* ---- data conversion ---- */

/* Writes `vertex_count` vertices of one stream (`stride` bytes apart) from src to dst as the pipeline's host formats
 * read them. big_endian nonzero (a buffer in Xenos byte order, d3d_port_buffer.h): each element of the stream is
 * swapped in its GPUENDIAN unit first (port_geometry_swap_element); then every element gets its
 * e_xenos_vertex_conversion (2_10_10_10 W, 11_11_10 and 10_11_11 requantization; int32_to_float is left to the
 * caller, as it depends on the shader input). Bytes no element covers are copied unchanged. src and dst must not overlap. */
void xenos_vertex_convert_stream(const struct s_xenos_vertex_declaration *declaration, uint32_t stream,
                                  const void *src, void *dst, uint32_t vertex_count, uint32_t stride,
                                  uint32_t big_endian);

/* PRT ambient (transfer declaration 1: stream 2 is one UBYTE4N BLENDWEIGHT1): the Xbox packs four vertices into each
 * dword, and its static_prt_ambient vertex shaders fetch dword v / 4 and take component 3 - (v & 3) (entry_points.fx,
 * foliage.fx, cortana.fx: `vfetch prt_values, vertex_index * 0.25, blendweight1`). The DX11 shaders read one
 * BLENDWEIGHT1 per vertex. xenos_vertex_stream_is_packed_prt says whether `stream` of the declaration is such a
 * stream (its only element: a UBYTE4N BLENDWEIGHT1 at offset 0 of stream 2, `stride` 4); then
 * xenos_vertex_expand_packed_prt writes one 4-byte vertex per packed value (the value in x, as R8G8B8A8_UNORM reads
 * it; y, z, w zero) for the `value_count` values (bytes) of src: dst holds value_count * 4 bytes. */
int xenos_vertex_stream_is_packed_prt(const struct s_xenos_vertex_declaration *declaration, uint32_t stream,
                                       uint32_t stride);
void xenos_vertex_expand_packed_prt(const void *src, void *dst, uint32_t value_count, uint32_t big_endian);

/* The 32 bytes of the null vertex buffer (see XENOS_VERTEX_NULL_FLOAT_OFFSET). */
extern const uint32_t k_xenos_vertex_null_buffer_data[XENOS_VERTEX_NULL_BUFFER_SIZE / 4];

#ifdef __cplusplus
}
#endif

#endif /* PORT_D3D_XENOS_VERTEX_H */
