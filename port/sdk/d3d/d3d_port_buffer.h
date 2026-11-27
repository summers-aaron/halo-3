#ifndef PORT_SDK_D3D_D3D_PORT_BUFFER_H
#define PORT_SDK_D3D_D3D_PORT_BUFFER_H

/*
 * Byte order of vertex and index buffers, between the Xbox D3D/XGraphics layer (port/sdk/d3d/xgraphics.cpp,
 * null_device.cpp) and the PC renderer (port/sdk/d3d/vulkan_*).
 *
 * Buffers the game creates with D3DDevice_CreateVertexBuffer / CreateIndexBuffer and fills through Lock are written by
 * the CPU: host order. Headers the game builds itself with XGSetVertexBufferHeader / XGSetIndexBufferHeader point at
 * memory D3D never wrote; in Halo 3 that is paged render geometry (tag_d3d_vertex_buffer_fixup /
 * tag_d3d_index_buffer_fixup), in Xenos byte order as read from the map, plus a few GPU effect buffers. Those headers
 * carry PORT_D3D_BUFFER_ID_XENOS_ORDER in D3DResource::Identifier (byte offset 0x10; the game never sets it on buffers).
 *
 * Once the paged-resource hook runs (port/GAME-CHANGES.md E1: port_resource_swap_page converts every page as it is
 * loaded, port/data/resource_swap.h), that memory is host order too and the marker is ignored
 * (port_resource_paged_data_is_host_order). Until then the renderer swaps marked buffers per element on upload
 * (port/data/geometry_swap.h), as the Xenos vertex fetch would.
 */

#define PORT_D3D_BUFFER_ID_KIND_MASK   0xf0000000u
#define PORT_D3D_BUFFER_ID_XENOS_ORDER 0x60000000u /* header over memory D3D did not write: Xenos byte order */

#ifdef __cplusplus
extern "C" {
#endif

/* port/data/resource_swap.h (declared here too: that header needs <stdint.h>, which the XDK-built units lack). */
int port_resource_paged_data_is_host_order(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_SDK_D3D_D3D_PORT_BUFFER_H */
