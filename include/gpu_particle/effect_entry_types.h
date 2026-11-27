#ifndef GPU_PARTICLE_EFFECT_ENTRY_TYPES_H
#define GPU_PARTICLE_EFFECT_ENTRY_TYPES_H

#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "render_methods/render_method.h"
#include "effects/editable_property.h"
#include <stddef.h>

struct s_effect_entry {
    long name;
    union
    {
        c_render_method shader;
        struct
        {
            s_tag_reference render_method_reference;
            s_tag_block shader_options;
            s_tag_block shader_parameters;
            s_tag_block shader_postprocess;
            unsigned short shader_flags;
            unsigned char shader_sort_layer;
            unsigned char shader_version;
            long shader_atmosphere_index;
        };
    };
    long prediction_atom_index;
    unsigned short count;
    float scale;
    c_editable_property<c_light_volume_states> properties[4];
    c_editable_property<c_light_volume_states> profile_thickness;
    c_editable_property<c_light_volume_states> profile_color;
    c_editable_property<c_light_volume_states> profile_alpha;
    c_editable_property<c_light_volume_states> profile_intensity;
    unsigned long constant_per_profile_properties;
    unsigned long flags;
    unsigned long profile_bytes;
    s_tag_block gpu_properties;
    s_tag_block gpu_functions;
    s_tag_block gpu_colors;
};

typedef char effect_entry_profile_bytes_offset[(offsetof(s_effect_entry, profile_bytes) == 0x154) ? 1 : -1];
typedef char effect_entry_properties_offset[(offsetof(s_effect_entry, properties) == 0x4c) ? 1 : -1];
typedef char effect_entry_flags_offset[(offsetof(s_effect_entry, flags) == 0x150) ? 1 : -1];
typedef char effect_entry_sort_layer_offset[(offsetof(s_effect_entry, shader) + offsetof(c_render_method, sort_layer) == 0x3a) ? 1 : -1];

typedef char effect_entry_size[(sizeof(void *) != 4 || sizeof(s_effect_entry) == 0x17c) ? 1 : -1];
typedef char effect_entry_gpu_properties_offset[(offsetof(s_effect_entry, gpu_properties) == 0x158) ? 1 : -1];
typedef char effect_entry_gpu_functions_offset[(offsetof(s_effect_entry, gpu_functions) == 0x164) ? 1 : -1];
typedef char effect_entry_gpu_colors_offset[(offsetof(s_effect_entry, gpu_colors) == 0x170) ? 1 : -1];

#endif // GPU_PARTICLE_EFFECT_ENTRY_TYPES_H
