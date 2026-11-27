#ifndef RENDER_RENDER_VISIBILITY_CACHED_STATE_TYPES_H
#define RENDER_RENDER_VISIBILITY_CACHED_STATE_TYPES_H
#include <stddef.h>
#include "render/shader_extern_info_types.h"

// Cached-state layout shared by lighting, shader and LOD lookups.
// Sample and target offsets are corroborated by render_objects_static_lighting.
struct s_visibility_cached_object_render_state
{
    short identifier;
    bool sample_valid;
    unsigned char valid;
    bool converged;
    long object_index;
    long sample_frame;
    union { unsigned long frame_index; long update_frame; };
    real_point3d sample_position;
    union
    {
        render_lighting target_lighting;
        struct
        {
            real_vector3d target_down;
            s_geometry_sample target_sample;
            c_dynamic_cubemap_sample cubemap_sample;
            long instance_index;
        };
    };
    s_shader_extern_info shader_info;
    char lod;
    char region_data[4][16];
};

typedef char visibility_cached_target_lighting_offset[(offsetof(s_visibility_cached_object_render_state, target_lighting) == 0x20) ? 1 : -1];

typedef char visibility_cached_state_size[(sizeof(void *) != 4 || ((sizeof(s_visibility_cached_object_render_state) == 0x4cc))) ? 1 : -1];
typedef char visibility_cached_shader_offset[(offsetof(s_visibility_cached_object_render_state, shader_info) == 0x238) ? 1 : -1];
typedef char visibility_cached_lod_offset[(sizeof(void *) != 4 || ((offsetof(s_visibility_cached_object_render_state, lod) == 0x488))) ? 1 : -1];
typedef char visibility_cached_object_index_offset[(offsetof(s_visibility_cached_object_render_state, object_index) == 0x8) ? 1 : -1];
typedef char visibility_cached_sample_frame_offset[(offsetof(s_visibility_cached_object_render_state, sample_frame) == 0xc) ? 1 : -1];
typedef char visibility_cached_frame_offset[(offsetof(s_visibility_cached_object_render_state, frame_index) == 0x10) ? 1 : -1];
typedef char visibility_cached_sample_position_offset[(offsetof(s_visibility_cached_object_render_state, sample_position) == 0x14) ? 1 : -1];
typedef char visibility_cached_target_offset[(offsetof(s_visibility_cached_object_render_state, target_down) == 0x20) ? 1 : -1];
typedef char visibility_cached_target_sample_offset[(offsetof(s_visibility_cached_object_render_state, target_sample) == 0x2c) ? 1 : -1];
#endif
