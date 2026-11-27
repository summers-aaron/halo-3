#ifndef RASTERIZER_GEOMETRY_TYPES_H
#define RASTERIZER_GEOMETRY_TYPES_H
#include <xtl.h>
#include "rasterizer/rasterizer_geometry.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_d3d_allocations.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "tag_files/tag_types.h"
class c_rasterizer_vertex_buffer;
struct s_geometry_globals;

// One element of a render geometry API resource's vertex or index buffer block. March reads it as
// c_typed_tag_interop<s_render_*_buffer_descriptor, s_tag_d3d_*_buffer> (tag_vertex_buffers.cpp): a tag interop
// whose address is the D3D buffer. March reads nothing past the address.
struct s_geometry_entry {
    TAG_POINTER(void) descriptor;
    union { TAG_POINTER(c_rasterizer_index_buffer) index_buffer; TAG_POINTER(c_rasterizer_vertex_buffer) vertex_buffer; };
    unsigned long : 32;
};

class c_rasterizer_index_buffer_factory_interface {
public:
    static bool create(c_rasterizer_index_buffer *buffer, c_rasterizer_index_buffer::e_primitive_type primitive_type,
        long index_count);
    static bool unlock(c_rasterizer_index_buffer *buffer);
};

__forceinline long c_d3d_resource_allocator::create_d3d_index_buffer(unsigned int length, unsigned long usage,
    _D3DFORMAT format, unsigned long pool, D3DIndexBuffer **buffer, void **allocation)
{
    HRESULT result = S_OK;
    if (g_d3d_resource_allocator_device)
        result = c_rasterizer::get_device()->CreateIndexBuffer(length, usage, format, (D3DPOOL)pool, buffer, NULL);
    return result;
}

// March rasterizer_geometry.cpp:233: the failure path asserts "d3d index buffer creation failed".
__forceinline bool c_rasterizer_index_buffer_factory_interface::create(c_rasterizer_index_buffer *buffer,
    c_rasterizer_index_buffer::e_primitive_type primitive_type, long index_count)
{
    buffer->setup((unsigned char)primitive_type, index_count);
    D3DIndexBuffer *d3d_buffer = NULL;
    HRESULT result = c_d3d_resource_allocator::create_d3d_index_buffer(index_count * 2, 0, D3DFMT_INDEX16,
        D3DPOOL_MANAGED, &d3d_buffer, NULL);
    if (SUCCEEDED(result))
        buffer->m_d3d_buffer = d3d_buffer;
    else
        buffer->m_d3d_buffer = NULL;
    return true;
}

__forceinline bool c_rasterizer_index_buffer::create(e_primitive_type primitive_type, long index_count)
{
    return c_rasterizer_index_buffer_factory_interface::create(this, primitive_type, index_count);
}
#endif
