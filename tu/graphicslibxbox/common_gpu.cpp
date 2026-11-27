// graphicslibxbox/common_gpu.obj: from the records 0x45f1a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x45f1a8 common_gpu_vertex_buffer_set_address

#include <xtl.h>
#include <xgraphics.h>
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "gpu_particle/common_gpu.h"

extern "C" void common_gpu_vertex_buffer_set_address(common_gpu_vertex_buffer *object, void *address)
{
    int format = object->format;
    object->m_d3d_buffer = 0;
    object->address = address;
    long stride = rasterizer_vertex_get_stride_from_declaration(&g_rasterizer_vertex_buffer_declarations[format]);
    D3DVertexBuffer *buffer = &object->xenon_buffer;
    object->rebuild((c_rasterizer_vertex_buffer::e_vertex_buffer_declaration)format, object->element_size * object->element_count, stride, buffer);
    XGSetVertexBufferHeader(object->get_vertex_stride() * object->get_vertex_count(), 4, D3DPOOL_DEFAULT, 0, buffer);
    XGOffsetResourceAddress(object->get_d3d_buffer(), address);
}
