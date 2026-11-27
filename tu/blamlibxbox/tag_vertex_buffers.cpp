// blamlibxbox/tag_vertex_buffers.obj: from the records 0x287210 0x287278
// @flags /TP /O1
#include <xtl.h>
#include <xgraphics.h>
#include "tag_files/tag_groups.h"
#include "tag_files/tag_api_interop.h"
#include "tag_files/tag_vertex_buffer_types.h"
#include "tag_files/tag_vertex_buffers.h"

// @address 0x287210 ?tag_d3d_vertex_buffer_fixup@@YA_NPAUs_tag_interop@@PAVc_tag_interop_verifier@@@Z
bool tag_d3d_vertex_buffer_fixup(s_tag_interop *value, c_tag_interop_verifier *verifier)
{
    s_render_vertex_buffer_descriptor *vertex_buffer = (s_render_vertex_buffer_descriptor *)(void *)value->descriptor;
    s_tag_d3d_vertex_buffer *d3d_buffer = (s_tag_d3d_vertex_buffer *)value->address;
    long vertex_data_size = vertex_buffer->vertices.size;
    void *vertex_data = tag_data_get_pointer(&vertex_buffer->vertices, 0, vertex_data_size);
    XGSetVertexBufferHeader(vertex_data_size, 4, D3DPOOL_DEFAULT, 0, &d3d_buffer->xenon_vertex_buffer);
    XGOffsetResourceAddress(&d3d_buffer->xenon_vertex_buffer, vertex_data);
    d3d_buffer->rebuild((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)vertex_buffer->declaration,
        vertex_buffer->vertex_count, vertex_buffer->stride, &d3d_buffer->xenon_vertex_buffer);
    return true;
}

// @address 0x287278 ?tag_d3d_index_buffer_fixup@@YA_NPAUs_tag_interop@@PAVc_tag_interop_verifier@@@Z
bool tag_d3d_index_buffer_fixup(s_tag_interop *value, c_tag_interop_verifier *verifier)
{
    s_render_index_buffer_descriptor *index_buffer = (s_render_index_buffer_descriptor *)(void *)value->descriptor;
    s_tag_d3d_index_buffer *d3d_buffer = (s_tag_d3d_index_buffer *)value->address;
    unsigned long index_data_size = index_buffer->indices.size;
    void *index_data = index_buffer->indices.address;
    XGSetIndexBufferHeader(index_data_size, 4, (D3DFORMAT)1, D3DPOOL_DEFAULT, 0, &d3d_buffer->xenon_index_buffer);
    XGOffsetResourceAddress(&d3d_buffer->xenon_index_buffer, index_data);
    d3d_buffer->rebuild(index_buffer->primitive_type, index_data_size >> 1, &d3d_buffer->xenon_index_buffer);
    return true;
}

// @address 0x42d380 ?tag_d3d_vertex_buffer_teardown@@YA_NPAUs_tag_interop@@@Z
bool tag_d3d_vertex_buffer_teardown(s_tag_interop *)
{
    return true;
}

// @address 0x42d380 ?tag_d3d_index_buffer_teardown@@YA_NPAUs_tag_interop@@@Z
bool tag_d3d_index_buffer_teardown(s_tag_interop *)
{
    return true;
}
