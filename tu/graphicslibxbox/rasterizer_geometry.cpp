// graphicslibxbox/rasterizer_geometry.obj: from the records 0x14cec0 0x150210 0x15ec70 0x15f828 0x45a200 0x45a268 0x45a2d0
// @flags /TP /O2
// @address 0x74b898 g_geometry_globals
// @address 0x15ec70 render_geometry_get_index_buffer
#include <xtl.h>
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/geometry_storage.h"
#include "multithreading/thread_tls.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "render/dynamic_vertex_buffer_types.h"
#include "geometry/geometry_block_types.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "rasterizer/rasterizer_index_buffer.h"
#include "rasterizer/rasterizer_d3d_allocations.h"
#include "rasterizer/rasterizer.h"



// a base class 0x320 bytes into s_geometry_storage_member: converting a pointer to it checks for null





#include "rasterizer/rasterizer_geometry_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/datum_index.h"
#include "geometry/render_geometry_types.h"
// @address 0x11df630 g_restricted_address



#include "cache/restricted_memory_regions_inline.h"

// Vertex streams indexed by declaration type.
// @rdata 0x82024f14..0x82024fb0
__declspec(align(4)) static const unsigned long k_vertex_stream_from_type[9] = { 0, 4, 4, 2, 1, 2, 2, 3, 0x38d1b717 };
__declspec(align(4)) static const long k_vertex_type_from_declaration[30] =
{
    0, 0, 0, 0, 1, 2, 1, 0, 3, 3, 3, 3, 0, 0, 2,
    0, 4, 5, 4, 4, 4, 5, 5, 5, 0, 0, 6, 7, 0, 4
};
// @rdata-end

static inline unsigned long rasterizer_geometry_get_stream_from_declaration(c_rasterizer_vertex_buffer::e_vertex_buffer_declaration declaration)
{
    return k_vertex_stream_from_type[k_vertex_type_from_declaration[declaration]];
}

bool c_rasterizer::set_vertex_stream(const c_rasterizer_vertex_buffer *stream)
{
    unsigned long stream_index = rasterizer_geometry_get_stream_from_declaration(stream->get_declaration());
    return SUCCEEDED(c_rasterizer::get_device()->SetStreamSource(stream_index, stream->get_d3d_buffer(), 0, stream->get_vertex_stride()));
}

#if !defined(_XBOX) || defined(__clang__)
bool rasterizer_set_vertex_stream_with_offset(const c_rasterizer_vertex_buffer *stream, unsigned long offset)
{
    unsigned long stream_index = rasterizer_geometry_get_stream_from_declaration(stream->get_declaration());
    return SUCCEEDED(c_rasterizer::get_device()->SetStreamSource(
        stream_index, stream->get_d3d_buffer(), offset, stream->get_vertex_stride()));
}
#endif

bool c_rasterizer::clear_vertex_stream(const c_rasterizer_vertex_buffer *stream)
{
    unsigned long stream_index = rasterizer_geometry_get_stream_from_declaration(stream->get_declaration());
    c_rasterizer::get_device()->SetStreamSource(stream_index, 0, 0, 0);
    return true;
}

extern "C" c_rasterizer_index_buffer *render_geometry_get_index_buffer(const s_render_geometry *geometry, long entry_index)
{
    long resource_handle = geometry->api_resource_handle;
    s_geometry_storage_member *member = &g_geometry_globals.manager->m_resource_cache.m_geometry_storage;
    s_geometry_second_base *base = member;
    const c_tag_resource_cache_file_access_cache *cache = base->m_per_thread_access_cache[get_current_thread_index()];
    const s_geometry_block *block = static_cast<const s_geometry_block *>(cache->m_resource_values[datum_index_to_absolute_index(resource_handle)]);
    return block->entries[entry_index].index_buffer;
}

bool c_rasterizer_index_buffer_factory_interface::unlock(c_rasterizer_index_buffer *buffer)
{
    bool result = false;
    if (restricted_region_locked_for_current_thread(2)) {
        if (c_rasterizer::get_device()) {
            HRESULT unlock_result = buffer->get_d3d_buffer()->Unlock();
            result = unlock_result == S_OK;
        }
    } else {
        result = true;
    }
    return result;
}

void *c_rasterizer_vertex_buffer_factory_interface::write_lock(c_rasterizer_vertex_buffer *buffer)
{
    void *result = 0;
    if (c_rasterizer::get_device() && buffer->get_vertex_count()) {
        D3DVertexBuffer *vb = buffer->m_d3d_buffer;
        if (g_restricted_address[2])
            result = D3DVertexBuffer_Lock(vb, 0, buffer->get_vertex_count() * buffer->get_vertex_stride(), 0);
        else
            result = (void *)(uintptr_t)(vb->Format.dword[0] & ~3);
    }
    return result;
}

bool c_rasterizer_vertex_buffer_factory_interface::unlock(c_rasterizer_vertex_buffer *buffer)
{
    bool result = false;
    if (c_rasterizer::get_device() && buffer->get_vertex_count() > 0) {
        if (restricted_region_locked_for_current_thread(2)) {
            HRESULT unlock_result = buffer->get_d3d_buffer()->Unlock();
            result = unlock_result == S_OK;
        } else {
            result = true;
        }
    }
    return result;
}


// @address 0x45a2d0 ?create@c_rasterizer_vertex_buffer@@QAA_NW4e_vertex_buffer_declaration@1@J@Z
bool c_rasterizer_vertex_buffer::create(e_vertex_buffer_declaration declaration, long count)

{
    c_rasterizer_vertex_buffer *buffer = this;
    long stride = rasterizer_vertex_get_stride_from_declaration(
        &g_rasterizer_vertex_buffer_declarations[declaration]);
    buffer->m_declaration = (unsigned char)declaration;
    buffer->m_vertex_count = (unsigned short)count;
    buffer->m_vertex_stride = stride;
    D3DVertexBuffer *created = 0;
    if (c_d3d_resource_allocator::create_d3d_vertex_buffer((unsigned char)stride * (unsigned short)count, 0, 0, 1, &created, 0) >= 0) {
        buffer->m_d3d_buffer = created;
        return true;
    }
    return false;
}
