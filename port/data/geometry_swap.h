#ifndef PORT_DATA_GEOMETRY_SWAP_H
#define PORT_DATA_GEOMETRY_SWAP_H

/*
 * Byte order of Xbox 360 vertex and index data.
 *
 * Halo 3's paged render geometry (render_geometry_api_resource_definition: 'mode', 'sbsp', 'sLdT', 'Lbsp', 'pmdf',
 * ...) holds big-endian vertex and index buffers that the Xenos vertex fetch byte-swaps per element with the
 * element's GPUENDIAN (d3d9gpu.h): 8in16 for 16-bit components (SHORT*, USHORT*, FLOAT16_*), 8in32 for everything
 * else (FLOAT*, D3DCOLOR, UBYTE4*, BYTE4N, DEC3N, DHEN3N and the other packed 32-bit formats). Converting a buffer
 * here performs that same swap once in memory, so the result is what the GPU reads after its fetch swap: every
 * element in host order, exactly like the vertices the game writes at run time with the CPU. The Vulkan layer
 * (port/sdk/d3d/xenos_vertex.h) reads such host-order data directly; formats Vulkan lacks are converted there.
 *
 * Every swap is an involution (swapping again restores the original), so the same calls convert back.
 *
 * Retail data agrees with the declarations: positions and texture coordinates are finite, DHEN3N normals unit length
 * and UBYTE4N blend weights sum to 255 only after the per-element swap (tmp/test_resource_swap.cpp).
 *
 * Host C/C++ with C linkage; no C library beyond memcpy.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Xenos D3DDECLTYPE ---- */

/* Bytes one element of a D3DDECLTYPE occupies and the unit its bytes are swapped in (1 = none, 2 = 8in16,
 * 4 = 8in32 / 16in32 by GPUENDIAN). Returns 0 for an unknown type (D3DDECLTYPE_UNUSED, an unknown GPUVERTEXFORMAT). */
uint32_t port_geometry_decltype_size(uint32_t d3d_decltype, uint32_t *out_swap_unit);

/* Swaps one element of the given D3DDECLTYPE in place by its GPUENDIAN (8in16, 8in32 or 16in32). Returns its size,
 * 0 (and leaves it alone) for an unknown type. */
uint32_t port_geometry_swap_element(uint32_t d3d_decltype, void *element);

/* ---- Halo 3 vertex buffer formats ---- */

/* c_rasterizer_vertex_buffer::e_vertex_buffer_declaration, the `declaration` of s_render_vertex_buffer_descriptor
 * (tag_vertex_buffer_types.h) and the index into g_rasterizer_vertex_buffer_declarations
 * (tu/graphicslibxbox/rasterizer_vertex_definitions.cpp). */
#define PORT_GEOMETRY_VERTEX_BUFFER_DECLARATION_COUNT 30

/* Stride of a vertex buffer declaration (rasterizer_vertex_get_stride_from_declaration), 0 for an empty or unknown
 * one. */
uint32_t port_geometry_vertex_declaration_stride(uint32_t declaration);

/* The D3DDECLTYPE and byte offset of element `index` of a vertex buffer declaration. Returns 0 past the last
 * element. */
int port_geometry_vertex_declaration_element(uint32_t declaration, uint32_t index, uint32_t *out_d3d_decltype,
                                             uint32_t *out_offset);

enum e_port_geometry_result {
    k_port_geometry_ok = 0,
    k_port_geometry_unknown_declaration = 1, /* empty or unknown declaration: nothing swapped */
    k_port_geometry_stride_mismatch = 2,     /* stride smaller than the declaration's elements: nothing swapped */
    k_port_geometry_size_mismatch = 3,       /* vertex_count * stride exceeds data_size: nothing swapped */
};

/* Swaps `vertex_count` vertices (`stride` bytes apart) of a vertex buffer of the given declaration in place, each
 * element in its GPUENDIAN unit. A stride larger than the declaration's elements leaves the extra bytes alone.
 * Returns an e_port_geometry_result. */
uint32_t port_geometry_swap_vertices(uint32_t declaration, uint32_t stride, uint32_t vertex_count, void *data,
                                     uint32_t data_size);

/* Swaps `index_count` indices of `index_size` bytes (2: INDEX16, 4: INDEX32) in place. */
void port_geometry_swap_indices(uint32_t index_size, uint32_t index_count, void *data);

#ifdef __cplusplus
}
#endif

#endif /* PORT_DATA_GEOMETRY_SWAP_H */
