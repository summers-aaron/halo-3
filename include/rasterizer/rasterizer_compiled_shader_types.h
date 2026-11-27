#ifndef RASTERIZER_COMPILED_SHADER_TYPES_H
#define RASTERIZER_COMPILED_SHADER_TYPES_H

#include "gpu_particle/shader_slot_types.h"

struct D3DPixelShader;
struct D3DVertexShader;

#include "tag_files/tag_types.h"

class c_rasterizer_compiled_pixel_shader
{
    char m_shader_data[40];
    s_entry_table m_constants;
    char m_constant_data[20];
    TAG_POINTER(D3DPixelShader) m_d3d_shader;
public:
    __forceinline s_entry_table *get_constant_table() { return &m_constants; }
    __forceinline D3DPixelShader *get_d3d_shader() const { return m_d3d_shader; }
};

class c_rasterizer_compiled_vertex_shader
{
    char m_shader_data[40];
    s_entry_table m_constants;
    char m_constant_data[20];
    TAG_POINTER(D3DVertexShader) m_d3d_shader;
public:
    __forceinline s_entry_table *get_constant_table() { return &m_constants; }
    __forceinline D3DVertexShader *get_d3d_shader() const { return m_d3d_shader; }
};

typedef char compiled_pixel_shader_layout_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_compiled_pixel_shader) == 0x50) ? 1 : -1];
typedef char compiled_vertex_shader_layout_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_compiled_vertex_shader) == 0x50) ? 1 : -1];

#endif
