#ifndef RASTERIZER_RASTERIZER_VERTEX_BUFFER_H
#define RASTERIZER_RASTERIZER_VERTEX_BUFFER_H

#include "tag_files/tag_types.h"

struct D3DVertexBuffer;

// March 82ca8be0: rebuild stores the resource pointer, declaration, count and stride.
class c_rasterizer_vertex_buffer
{
public:
    enum e_vertex_buffer_declaration { k_number_of_vertex_buffer_declarations = 30 };

#if !defined(_XBOX) || defined(__clang__)
    constexpr c_rasterizer_vertex_buffer() : m_declaration(0), m_vertex_stride(0), m_vertex_count(0), m_d3d_buffer() {}
#endif

    bool create(e_vertex_buffer_declaration declaration, long count);

    void rebuild(e_vertex_buffer_declaration declaration, long count, long stride, D3DVertexBuffer *buffer)
    {
        m_d3d_buffer = buffer;
        m_declaration = (unsigned char)declaration;
        m_vertex_count = (unsigned short)count;
        m_vertex_stride = (unsigned char)stride;
    }

    __forceinline e_vertex_buffer_declaration get_declaration() const { return (e_vertex_buffer_declaration)m_declaration; }
    __forceinline unsigned char get_vertex_stride() const { return m_vertex_stride; }
    __forceinline unsigned short get_vertex_count() const { return m_vertex_count; }
    __forceinline D3DVertexBuffer *get_d3d_buffer() const { return m_d3d_buffer; }

    unsigned char m_declaration;
    unsigned char m_vertex_stride;
    unsigned short m_vertex_count;
    TAG_POINTER(D3DVertexBuffer) m_d3d_buffer;
};

class c_rasterizer_vertex_buffer_factory_interface
{
public:
    static void *write_lock(c_rasterizer_vertex_buffer *buffer);
    static const void *read_lock(c_rasterizer_vertex_buffer *buffer, bool read_only);
    static bool unlock(c_rasterizer_vertex_buffer *buffer);
};

typedef char rasterizer_vertex_buffer_size[(sizeof(c_rasterizer_vertex_buffer) == 0x08) ? 1 : -1];

#if !defined(_XBOX) || defined(__clang__)
bool rasterizer_set_vertex_stream_with_offset(const c_rasterizer_vertex_buffer *stream, unsigned long offset);
#endif

#endif // RASTERIZER_RASTERIZER_VERTEX_BUFFER_H
