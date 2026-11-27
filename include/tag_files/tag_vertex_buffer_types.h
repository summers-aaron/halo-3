#ifndef TAG_FILES_TAG_VERTEX_BUFFER_TYPES_H
#define TAG_FILES_TAG_VERTEX_BUFFER_TYPES_H

#include <d3d9.h>
#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer_vertex_buffer.h"
#include "rasterizer/rasterizer_index_buffer.h"

// March tag_vertex_buffers.cpp:1727..1783 and the corresponding typed-interop getters.
struct s_render_vertex_buffer_descriptor
{
    long vertex_count;
    short declaration;
    short stride;
    s_tag_data vertices;
};

struct s_render_index_buffer_descriptor
{
    c_rasterizer_index_buffer::e_primitive_type primitive_type;
    s_tag_data indices;
};

struct s_tag_d3d_vertex_buffer : c_rasterizer_vertex_buffer
{
    D3DVertexBuffer xenon_vertex_buffer;
};

struct s_tag_d3d_index_buffer : c_rasterizer_index_buffer
{
    D3DIndexBuffer xenon_index_buffer;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char tag_d3d_vertex_buffer_size[(sizeof(void *) != 4 || sizeof(s_tag_d3d_vertex_buffer) == 40) ? 1 : -1];
typedef char tag_d3d_index_buffer_size[(sizeof(void *) != 4 || sizeof(s_tag_d3d_index_buffer) == 40) ? 1 : -1];
#endif

#endif // TAG_FILES_TAG_VERTEX_BUFFER_TYPES_H
