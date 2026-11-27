#ifndef RENDER_SHADER_EXTERN_INFO_TYPES_H
#define RENDER_SHADER_EXTERN_INFO_TYPES_H

#include <stddef.h>
#include "geometry/geometry_sampling_types.h"
#include "cubemaps/cubemap_sample.h"
#include "render/render_lighting_types.h"
#include "render/shader_reference_types.h"

// Shared by default shader setup, mesh submission, and cached object lighting.
struct s_shader_extern_info
{
    long object_index;
    float (*function_value_simple_callback)(long, long, long);
    long skinning_offset;
    unsigned long frame_index;
    union
    {
        render_lighting lighting;
        struct
        {
            real_vector3d down;
            s_geometry_sample lightprobe_sample;
            c_dynamic_cubemap_sample cubemap_sample;
            long instance_index;
        };
    };
    s_shader_reference change_colors;
    unsigned char change_color_count;
    bool override_material;
    unsigned char camouflage;
    unsigned char shield;
    unsigned char shield_damage;
    real_vector4d bounding_sphere;
};

typedef char shader_extern_lighting_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, lighting) == 0x10))) ? 1 : -1];

typedef char shader_extern_info_size[(sizeof(void *) != 4 || ((sizeof(s_shader_extern_info) == 0x250))) ? 1 : -1];
typedef char shader_extern_info_down_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, down) == 0x10))) ? 1 : -1];
typedef char shader_extern_info_lightprobe_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, lightprobe_sample) == 0x1c))) ? 1 : -1];
typedef char shader_extern_info_cubemap_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, cubemap_sample) == 0x214))) ? 1 : -1];
typedef char shader_extern_info_change_colors_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, change_colors) == 0x228))) ? 1 : -1];

typedef char shader_extern_info_camouflage_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, camouflage) == 0x23a))) ? 1 : -1];
typedef char shader_extern_info_sphere_offset[(sizeof(void *) != 4 || ((offsetof(s_shader_extern_info, bounding_sphere) == 0x240))) ? 1 : -1];

#endif // RENDER_SHADER_EXTERN_INFO_TYPES_H
