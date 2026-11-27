#include "geometry_swap.h"

#include "data_crt.h"

namespace {

// MAKED3DDECLTYPE (the XDK's d3d9types.h): GPUVERTEXFORMAT | GPUENDIAN << 6 | GPUVERTEXSIGN << 8 |
// GPUNUMFORMAT << 9 | swizzle << 10, the swizzle being GPUSWIZZLE x | y << 3 | z << 6 | w << 9.
constexpr uint32_t decltype_of(uint32_t format, uint32_t endian, uint32_t sign, uint32_t numformat, uint32_t swizzle)
{
    return format | endian << 6 | sign << 8 | numformat << 9 | swizzle << 10;
}
constexpr uint32_t swizzle_of(uint32_t x, uint32_t y, uint32_t z, uint32_t w) { return x | y << 3 | z << 6 | w << 9; }

constexpr uint32_t k_xyzw = swizzle_of(0, 1, 2, 3);
constexpr uint32_t k_zyxw = swizzle_of(2, 1, 0, 3);
constexpr uint32_t k_xyz1 = swizzle_of(0, 1, 2, 5);
constexpr uint32_t k_xy01 = swizzle_of(0, 1, 4, 5);
constexpr uint32_t k_x001 = swizzle_of(0, 4, 4, 5);

// The D3DDECLTYPE of each e_rasterizer_vertex_format (rasterizer_vertex_types.h). The element sizes are
// rasterizer_vertex_get_stride_from_declaration's; the types are the ones c_vertex_declaration_table
// (tu/graphicslibxbox/rasterizer_resource_definitions.cpp) declares for the same elements: world FLOAT3 position,
// FLOAT2 texcoord, DHEN3N normal/tangent/binormal; rigid USHORT4N / USHORT2N; skinned UBYTE4 indices and UBYTE4N
// weights; per-vertex lighting BYTE4N; PRT UBYTE4N / BYTE4N / DEC3N; D3DCOLOR colors.
const uint32_t k_rasterizer_vertex_format_decltypes[18] = {
    decltype_of(36, 2, 1, 1, k_x001), //  0 FLOAT1
    decltype_of(37, 2, 1, 1, k_xy01), //  1 FLOAT2
    decltype_of(57, 2, 1, 1, k_xyz1), //  2 FLOAT3
    decltype_of(38, 2, 1, 1, k_xyzw), //  3 FLOAT4
    decltype_of(6, 2, 0, 1, k_xyzw),  //  4 UBYTE4
    decltype_of(6, 2, 0, 0, k_xyzw),  //  5 UBYTE4N
    decltype_of(6, 2, 0, 0, k_zyxw),  //  6 D3DCOLOR
    decltype_of(25, 1, 1, 1, k_xy01), //  7 SHORT2
    decltype_of(26, 1, 1, 1, k_xyzw), //  8 SHORT4
    decltype_of(25, 1, 1, 0, k_xy01), //  9 SHORT2N
    decltype_of(26, 1, 1, 0, k_xyzw), // 10 SHORT4N
    decltype_of(25, 1, 0, 0, k_xy01), // 11 USHORT2N
    decltype_of(26, 1, 0, 0, k_xyzw), // 12 USHORT4N
    decltype_of(6, 2, 1, 0, k_xyzw),  // 13 BYTE4N
    decltype_of(17, 2, 1, 0, k_xyz1), // 14 DHEN3N
    decltype_of(31, 1, 1, 1, k_xy01), // 15 FLOAT16_2
    decltype_of(32, 1, 1, 1, k_xyzw), // 16 FLOAT16_4
    decltype_of(7, 2, 1, 0, k_xyz1),  // 17 DEC3N
};

// g_rasterizer_vertex_buffer_declarations (tu/graphicslibxbox/rasterizer_vertex_definitions.cpp, 0x730580): the
// element formats of each declaration in order (-1 ends it). Entries 16..19 and 29 are empty there; 20..28 belong to
// the GPU effect units (particles, contrails, light volumes, beams), whose buffers the game writes itself.
const int8_t k_declaration_formats[PORT_GEOMETRY_VERTEX_BUFFER_DECLARATION_COUNT][9] = {
    { -1 },
    { 2, 1, 14, 14, 14, -1 },               //  1 world
    { 12, 11, 14, 14, 14, -1 },             //  2 rigid
    { 12, 11, 14, 14, 14, 4, 5, -1 },       //  3 skinned
    { 11, -1 },                             //  4 per-pixel lightmap texcoord
    { 6, -1 },                              //  5 color
    { 6, 6, 6, 6, 6, -1 },                  //  6 per-vertex lighting (BYTE4N in the D3D declaration)
    { 0, 3, -1 },                           //  7
    { -1 },                                 //  8
    { 5, -1 },                              //  9 PRT ambient
    { 5, -1 },                              // 10 PRT linear
    { 17, 17, 17, -1 },                     // 11 PRT quadratic
    { 1, 1, 6, -1 },                        // 12
    { 2, 6, -1 },                           // 13
    { 2, -1 },                              // 14
    { 2, 1, 6, -1 },                        // 15
    { -1 }, { -1 }, { -1 }, { -1 },         // 16..19
    { 12, 4, 6, -1 },                       // 20
    { 7, -1 },                              // 21
    { 7, -1 },                              // 22
    { 7, -1 },                              // 23
    { 12, 11, 14, -1 },                     // 24
    { 12, 11, 14, -1 },                     // 25
    { 7, 7, 7, -1 },                        // 26
    { 2, 2, 2, -1 },                        // 27
    { 3, 3, 3, 3, 3, 3, 3, 3, -1 },         // 28
    { -1 },                                 // 29
};

inline uint16_t load_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
inline uint32_t load_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
inline void store_u16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
inline void store_u32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
inline uint16_t bswap16(uint16_t v) { return (uint16_t)((v >> 8) | (v << 8)); }
inline uint32_t bswap32(uint32_t v)
{
    return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24);
}

void swap_units(uint8_t *p, uint32_t size, uint32_t endian)
{
    switch (endian) {
    case 1: // 8in16
        for (uint32_t b = 0; b + 2 <= size; b += 2) store_u16(p + b, bswap16(load_u16(p + b)));
        break;
    case 2: // 8in32
        for (uint32_t b = 0; b + 4 <= size; b += 4) store_u32(p + b, bswap32(load_u32(p + b)));
        break;
    case 3: // 16in32
        for (uint32_t b = 0; b + 4 <= size; b += 4) {
            uint32_t v = load_u32(p + b);
            store_u32(p + b, (v >> 16) | (v << 16));
        }
        break;
    default:
        break;
    }
}

// Bytes of a GPUVERTEXFORMAT (d3d9gpu.h), 0 for one no D3DDECLTYPE uses.
uint32_t vertex_format_size(uint32_t format)
{
    switch (format) {
    case 6: case 7: case 16: case 17: case 25: case 31: case 33: case 36:
        return 4;
    case 26: case 32: case 34: case 37:
        return 8;
    case 57:
        return 12;
    case 35: case 38:
        return 16;
    default:
        return 0;
    }
}

} // namespace

extern "C" uint32_t port_geometry_decltype_size(uint32_t d3d_decltype, uint32_t *out_swap_unit)
{
    const uint32_t size = d3d_decltype == 0xffffffffu ? 0 : vertex_format_size(d3d_decltype & 0x3fu);
    if (out_swap_unit) {
        static const uint32_t k_units[4] = { 1, 2, 4, 4 };
        *out_swap_unit = size ? k_units[(d3d_decltype >> 6) & 3u] : 0;
    }
    return size;
}

extern "C" uint32_t port_geometry_swap_element(uint32_t d3d_decltype, void *element)
{
    const uint32_t size = port_geometry_decltype_size(d3d_decltype, NULL);
    if (size)
        swap_units((uint8_t *)element, size, (d3d_decltype >> 6) & 3u);
    return size;
}

extern "C" int port_geometry_vertex_declaration_element(uint32_t declaration, uint32_t index,
                                                        uint32_t *out_d3d_decltype, uint32_t *out_offset)
{
    if (declaration >= PORT_GEOMETRY_VERTEX_BUFFER_DECLARATION_COUNT || index >= 8)
        return 0;
    const int8_t *formats = k_declaration_formats[declaration];
    uint32_t offset = 0;
    for (uint32_t i = 0; formats[i] >= 0; i++) {
        const uint32_t type = k_rasterizer_vertex_format_decltypes[formats[i]];
        if (i == index) {
            if (out_d3d_decltype) *out_d3d_decltype = type;
            if (out_offset) *out_offset = offset;
            return 1;
        }
        offset += port_geometry_decltype_size(type, NULL);
    }
    return 0;
}

extern "C" uint32_t port_geometry_vertex_declaration_stride(uint32_t declaration)
{
    uint32_t stride = 0, type = 0, offset = 0;
    for (uint32_t i = 0; port_geometry_vertex_declaration_element(declaration, i, &type, &offset); i++)
        stride = offset + port_geometry_decltype_size(type, NULL);
    return stride;
}

extern "C" uint32_t port_geometry_swap_vertices(uint32_t declaration, uint32_t stride, uint32_t vertex_count,
                                                void *data, uint32_t data_size)
{
    const uint32_t declaration_stride = port_geometry_vertex_declaration_stride(declaration);
    if (declaration_stride == 0)
        return k_port_geometry_unknown_declaration;
    if (stride < declaration_stride)
        return k_port_geometry_stride_mismatch;
    if ((uint64_t)vertex_count * stride > data_size)
        return k_port_geometry_size_mismatch;

    uint32_t types[8], offsets[8], count = 0;
    while (port_geometry_vertex_declaration_element(declaration, count, &types[count], &offsets[count]))
        count++;

    uint8_t *vertex = (uint8_t *)data;
    for (uint32_t v = 0; v < vertex_count; v++, vertex += stride)
        for (uint32_t e = 0; e < count; e++)
            port_geometry_swap_element(types[e], vertex + offsets[e]);
    return k_port_geometry_ok;
}

extern "C" void port_geometry_swap_indices(uint32_t index_size, uint32_t index_count, void *data)
{
    swap_units((uint8_t *)data, index_size * index_count, index_size == 4 ? 2u : 1u);
}
