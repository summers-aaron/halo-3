#ifndef RASTERIZER_RASTERIZER_GLOBAL_DEFINITIONS_H
#define RASTERIZER_RASTERIZER_GLOBAL_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer_textures_cache.h"

struct s_rasterizer_default_texture
{
    unsigned long flags;
    s_tag_reference bitmap;
};

// March's accessor asserts name rasterizer/rasterizer_global_definitions.h.
// The leading blocks hold the texture and shader tables, followed by the GPR limits.
// The two references after active camo are the performance template and
// shield-impact definition; March used those slots for noise-texture references.
class c_rasterizer_globals
{
public:
    struct s_explicit_shader
    {
        s_tag_reference vertex;
        s_tag_reference pixel;
    };

    const c_rasterizer_texture_ref get_default_texture_rasterizer_texture_ref(long index) const;

    const s_tag_reference *get_default_texture_ref(long index) const
    {
        const s_rasterizer_default_texture *default_texture =
            &m_default_textures_refs[index];
        return &default_texture->bitmap;
    }

    const s_explicit_shader *get_explicit_shaders(long index)
    {
        return &m_explicit_shaders[index];
    }

    c_typed_tag_block<s_rasterizer_default_texture> m_default_textures_refs;
    c_typed_tag_block<s_tag_reference> m_material_textures_refs;
    s_tag_reference m_default_vertex_shader;
    s_tag_reference m_default_pixel_shader;
    c_typed_tag_block<s_explicit_shader> m_explicit_shaders;
    s_tag_block m_atmosphere_lookup_tables;
    unsigned long max_vs_gprs;
    unsigned long max_ps_gprs;
    s_tag_reference m_active_camo_distortion_texture;
    s_tag_reference m_default_performance_template;
    s_tag_reference m_default_shield_impact;
    long m_motion_blur_taps;
    float m_motion_blur_max_x;
    float m_motion_blur_max_y;
    float m_motion_blur_scale_x;
    float m_motion_blur_scale_y;
    float m_motion_blur_center_falloff;
    float m_motion_blur_expected_dt;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char rasterizer_default_texture_size[(sizeof(s_rasterizer_default_texture) == 0x14) ? 1 : -1];
typedef char rasterizer_explicit_shader_size[(sizeof(c_rasterizer_globals::s_explicit_shader) == 0x20) ? 1 : -1];
typedef char rasterizer_globals_tag_size[(sizeof(c_rasterizer_globals) == 0xa4) ? 1 : -1];
typedef char rasterizer_globals_explicit_shaders_offset[(offsetof(c_rasterizer_globals, m_explicit_shaders) == 0x38) ? 1 : -1];
typedef char rasterizer_globals_max_vs_gprs_offset[(offsetof(c_rasterizer_globals, max_vs_gprs) == 0x50) ? 1 : -1];
typedef char rasterizer_globals_max_ps_gprs_offset[(offsetof(c_rasterizer_globals, max_ps_gprs) == 0x54) ? 1 : -1];
typedef char rasterizer_globals_motion_blur_offset[(offsetof(c_rasterizer_globals, m_motion_blur_taps) == 0x88) ? 1 : -1];
#endif

#endif // RASTERIZER_RASTERIZER_GLOBAL_DEFINITIONS_H
