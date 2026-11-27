#ifndef RASTERIZER_RASTERIZER_SHADER_DEFINITIONS_H
#define RASTERIZER_RASTERIZER_SHADER_DEFINITIONS_H

#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer_compiled_shader_types.h"
#include "tag_files/tag_groups.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

// The run of compiled shaders one entry point (or vertex type) selects: `count` permutations from `start_index`.
struct s_compiled_shader_reference
{
    unsigned char start_index;
    unsigned char count;
};

struct s_rasterizer_vertex_shader_entry_point
{
    c_typed_tag_block<s_compiled_shader_reference> vertex_types;
};

class c_rasterizer_pixel_shader
{
public:
    static const c_rasterizer_pixel_shader *get(long index) { return (const c_rasterizer_pixel_shader *)tag_get('pixl', index); }
private:
    long m_flags;
    c_typed_tag_block<s_compiled_shader_reference> m_entry_point_shaders;
    long m_version;
    c_typed_tag_block<c_rasterizer_compiled_pixel_shader> m_shaders;
public:
    __forceinline const s_tag_block &get_shaders() const { return m_shaders; }
    __forceinline c_rasterizer_compiled_pixel_shader *get_compiled_shader(e_entry_point entry, long permutation) const
    {
        c_rasterizer_compiled_pixel_shader *result = 0;
        if (entry < m_entry_point_shaders.count()) {
            const s_compiled_shader_reference &reference = m_entry_point_shaders[entry];
            if (permutation >= 0 && permutation < reference.count)
                result = &((c_rasterizer_compiled_pixel_shader *)m_shaders.address)[reference.start_index + permutation];
        }
        return result;
    }
    __forceinline D3DPixelShader *get_d3d_shader(e_entry_point entry, long permutation) const
    {
        c_rasterizer_compiled_pixel_shader *compiled = get_compiled_shader(entry, permutation);
        if (compiled) return compiled->get_d3d_shader();
        else return 0;
    }
};

class c_rasterizer_vertex_shader
{
private:
    long m_flags;
    c_typed_tag_block<s_rasterizer_vertex_shader_entry_point> m_entry_point_shaders;
    long m_version;
    c_typed_tag_block<c_rasterizer_compiled_vertex_shader> m_shaders;
public:
    __forceinline long get_shader_count() const { return m_shaders.count(); }
    __forceinline const c_rasterizer_compiled_vertex_shader *get_shaders() const { return m_shaders.begin(); }
    static const c_rasterizer_vertex_shader *get(long index) { return (const c_rasterizer_vertex_shader *)tag_get('vtsh', index); }
    const c_rasterizer_compiled_vertex_shader *get_compiled_shader(e_vertex_type vertex_type, e_entry_point entry, long permutation) const;
    __forceinline D3DVertexShader *get_d3d_shader(e_vertex_type vertex_type, e_entry_point entry, long permutation) const
    {
        const c_rasterizer_compiled_vertex_shader *compiled = get_compiled_shader(vertex_type, entry, permutation);
        if (compiled) return compiled->get_d3d_shader();
        else return 0;
    }
};

typedef char compiled_shader_reference_size[(sizeof(s_compiled_shader_reference) == 2) ? 1 : -1];
typedef char vertex_shader_entry_point_size[(sizeof(void *) != 4 || sizeof(s_rasterizer_vertex_shader_entry_point) == 0xC) ? 1 : -1];
typedef char compiled_pixel_shader_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_compiled_pixel_shader) == 0x50) ? 1 : -1];
typedef char compiled_vertex_shader_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_compiled_vertex_shader) == 0x50) ? 1 : -1];
typedef char pixel_shader_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_pixel_shader) == 0x20) ? 1 : -1];
typedef char vertex_shader_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_vertex_shader) == 0x20) ? 1 : -1];

#endif
