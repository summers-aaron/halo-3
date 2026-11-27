#ifndef RASTERIZER_DYNAMIC_RENDER_TARGET_TYPES_H
#define RASTERIZER_DYNAMIC_RENDER_TARGET_TYPES_H
#include "rasterizer/rasterizer_textures_cache.h"
struct s_dynamic_render_target {
    unsigned short identifier;
    unsigned short padding;
    long reference;
    long type;
    c_rasterizer_texture_ref accumulation_texture, albedo_texture, normal_texture;
    D3DSurface *accumulation_surface, *albedo_surface, *normal_surface, *depth_surface;
};


#include "memory/data.h"
#include "rasterizer/render_target_types.h"
#include "rasterizer/rasterizer_dynamic_render_targets.h"
class c_dynamic_render_targets {
    static c_static_array<long, 6> m_used_targets;
    static s_data_array *m_targets;
    friend void dynamic_render_targets_set_accumulation_texture();
    static __forceinline s_dynamic_render_target *get_target(long target_index) {
        return datum_get<s_dynamic_render_target>(m_targets, target_index);
    }
public:
#if !defined(_XBOX) || defined(__clang__)
    static __forceinline const c_static_array<long, 6> &target_indices() { return m_used_targets; }
    static __forceinline s_data_array *&target_data() { return m_targets; }
#endif

    static __forceinline void dispose()
    {
        if (m_targets) {
            data_dispose(m_targets);
            m_targets = 0;
        }
    }
    static __forceinline s_dynamic_render_target *chud_get_target(e_render_target_type target_type)
    {
        long target_index = m_used_targets[target_type];
        if (target_index != NONE)
            return datum_get<s_dynamic_render_target>(m_targets, target_index);
        return 0;
    }
    static void initialize();
    static long allocate_target(e_render_target_type, long, long, long, long, char *);
    static void allocate_global_temporary_buffers();
    static s_dynamic_render_target *get_target_from_target_type(e_render_target_type target_type);
};

#if !defined(_XBOX) || defined(__clang__)
#define g_dynamic_render_target_indices (c_dynamic_render_targets::target_indices())
#define g_dynamic_render_targets (c_dynamic_render_targets::target_data())
#endif

#endif
