// graphicslibxbox/rasterizer_primitives.obj: from the records 0x14e9b8 0x174d38 0x17ed18
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <xtl.h>
D3DPRIMITIVETYPE g_primitive_types[] = {
    D3DPT_POINTLIST, D3DPT_LINELIST, D3DPT_LINESTRIP,
    D3DPT_TRIANGLELIST, D3DPT_TRIANGLEFAN, D3DPT_TRIANGLESTRIP,
    D3DPT_QUADLIST, D3DPT_FORCE_DWORD
};
#include "rasterizer/rasterizer.h"
void c_rasterizer::draw_primitive(c_rasterizer_index_buffer::e_primitive_type type, long start_vertex, long primitive_count)
{
    g_device->SetIndices(0);
    g_device->DrawPrimitive(g_primitive_types[type], start_vertex, primitive_count);
}
void c_rasterizer::draw_indexed_primitive(const c_rasterizer_index_buffer *indices, long base_vertex_index, long num_vertices, long min_index, long index_count)
{
    long triangle_count = indices->get_primitive_type() == c_rasterizer_index_buffer::_primitive_type_triangle_strip ? index_count - 2 : index_count / 3;
    g_device->SetIndices(indices->get_d3d_buffer());
    g_device->DrawIndexedPrimitive(g_primitive_types[indices->get_primitive_type()], base_vertex_index, 0, num_vertices, min_index, triangle_count);
}
void c_rasterizer::draw_primitive_up(
    c_rasterizer_index_buffer::e_primitive_type type, unsigned long primitive_count, const void *vertex_data, unsigned long vertex_stride)
{
    g_device->DrawPrimitiveUP(g_primitive_types[type], primitive_count, vertex_data, vertex_stride);
}
