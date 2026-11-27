#ifndef RASTERIZER_RASTERIZER_INDEX_BUFFER_H
#define RASTERIZER_RASTERIZER_INDEX_BUFFER_H

// c_rasterizer_index_buffer: a D3D index buffer with its primitive type. The March 2007 build lists the methods
// (create, rebuild, release, read_lock, write_lock, unlock, setup, get_index_count, delete_proc) but no assert names
// a file, so this header's location is a guess. The field getters are written here; write_lock is out of line in
// March (rasterizer_geometry.cpp) and has no body in the tree yet except decals.cpp's.

#include "tag_files/tag_types.h"

struct D3DIndexBuffer;

class c_rasterizer_index_buffer
{
public:
    enum e_primitive_type
    {
        _primitive_type_point_list,
        _primitive_type_line_list,
        _primitive_type_line_strip,
        _primitive_type_triangle_list,
        _primitive_type_triangle_fan,
        _primitive_type_triangle_strip,
        _primitive_type_quad_list,
        _primitive_type_count
    };

    e_primitive_type get_primitive_type() const { return static_cast<e_primitive_type>(m_primitive_type); }
    D3DIndexBuffer *get_d3d_buffer() const { return m_d3d_buffer; }
    unsigned short *write_lock();
    bool create(e_primitive_type primitive_type, long index_count);
    void setup(unsigned char primitive_type, long index_count)
    {
        m_primitive_type = primitive_type;
        m_index_count = (unsigned short)index_count;
    }

    // March 82ca8f70: the index count occupies the halfword at +2.
    void rebuild(e_primitive_type primitive_type, long index_count, D3DIndexBuffer *buffer)
    {
        m_d3d_buffer = buffer;
        m_primitive_type = (unsigned char)primitive_type;
        m_index_count = (unsigned short)index_count;
    }

private:
    friend class c_rasterizer_index_buffer_factory_interface;
    unsigned char m_primitive_type;
    unsigned short m_index_count;
    TAG_POINTER(D3DIndexBuffer) m_d3d_buffer;
};

typedef char rasterizer_index_buffer_size[(sizeof(c_rasterizer_index_buffer) == 0x08) ? 1 : -1];

#endif // RASTERIZER_RASTERIZER_INDEX_BUFFER_H
