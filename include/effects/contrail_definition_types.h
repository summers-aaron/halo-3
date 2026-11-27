#ifndef EFFECTS_CONTRAIL_DEFINITION_TYPES_H
#define EFFECTS_CONTRAIL_DEFINITION_TYPES_H
#include "effects/editable_property.h"
#include "tag_files/tag_types.h"
#include "render_methods/render_method.h"

struct s_contrails_property_row {
    long name;
    c_render_method shader;
    long prediction_atom_index;
    unsigned short appearance_flags;
    float brightness_ratio;
    c_editable_property<c_light_volume_states> properties[4];
    c_editable_property<c_light_volume_states> profile_thickness;
    c_editable_property<c_light_volume_states> profile_color;
    c_editable_property<c_light_volume_states> profile_alpha;
    c_editable_property<c_light_volume_states> profile_intensity;
    unsigned long constant_per_profile_properties;
    unsigned long used_states;
    unsigned long max_profile_count;
    s_tag_block gpu_properties;
    s_tag_block gpu_functions;
    s_tag_block gpu_colors;
};
struct s_contrails_property_block { long count; TAG_POINTER(s_contrails_property_row) rows; };

class c_contrail_definition {
public:
    long name;
    union { float profile_distance; float origin_fade_range; };
    union { float first_profile_distance; float origin_fade_cutoff; };
    float edge_fade_range;
    float edge_fade_cutoff;
    float lod_in_distance;
    float lod_feather_in_distance;
    float lod_out_distance;
    float lod_feather_out_distance;
    c_editable_property<c_contrail_states> emission_rate;
    c_editable_property<c_contrail_states> lifespan;
    c_editable_property<real_vector3d> profile_self_acceleration;
    c_editable_property<c_contrail_states> profile_size;
    c_editable_property<real_point2d> profile_offset;
    c_editable_property<c_contrail_states> profile_rotation;
    c_editable_property<c_contrail_states> profile_rotation_rate;
    union { unsigned short flags; unsigned short profile_type; };
    unsigned char first_flag;
    unsigned char second_flag;
    // The render block is viewed as a render method when submitted (as in s_effect_entry).
    union {
        c_render_method shader;
        struct {
            s_tag_reference render_source;
            s_tag_block shader_options;
            s_tag_block shader_parameters;
            s_tag_block shader_postprocess;
            unsigned short shader_flags;
            unsigned char sort_layer;
            unsigned char shader_version;
            long shader_atmosphere_index;
        };
    };
    union { long prediction_atom_index; unsigned short appearance_flags; };
    real_vector2d uv_tiling_rate;
    real_vector2d uv_scroll_rate;
    c_editable_property<c_contrail_states> profile_color;
    c_editable_property<c_contrail_states> profile_alpha;
    c_editable_property<c_contrail_states> profile_secondary_alpha;
    c_editable_property<c_contrail_states> profile_black_point;
    c_editable_property<c_contrail_states> profile_palette;
    c_editable_property<c_contrail_states> profile_intensity;
    unsigned long constant_per_profile_properties;
    unsigned long used_states;
    s_tag_block gpu_properties;
    s_tag_block gpu_functions;
    s_tag_block gpu_colors;
};
struct s_contrail_system_definition { long count; TAG_POINTER(c_contrail_definition) contrails; };

typedef char contrail_definition_size[(sizeof(void *) != 4 || sizeof(c_contrail_definition) == 620) ? 1 : -1];
typedef char contrail_definition_states_offset[(sizeof(void *) != 4 || offsetof(c_contrail_definition, used_states) == 0x244) ? 1 : -1];
typedef char effect_property_row_size[(sizeof(void *) != 4 || sizeof(s_contrails_property_row) == 0x17c) ? 1 : -1];

typedef char contrail_definition_shader_offset[(offsetof(c_contrail_definition, render_source) == 0x130) ? 1 : -1];
typedef char contrail_definition_appearance_offset[(offsetof(c_contrail_definition, appearance_flags) == 0x16c) ? 1 : -1];
typedef char contrail_definition_uv_offset[(offsetof(c_contrail_definition, uv_tiling_rate) == 0x170) ? 1 : -1];
typedef char contrail_definition_properties_offset[(offsetof(c_contrail_definition, gpu_properties) == 0x248) ? 1 : -1];
typedef char contrail_definition_acceleration_offset[(offsetof(c_contrail_definition, profile_self_acceleration) == 0x64) ? 1 : -1];
typedef char contrail_definition_profile_offset[(offsetof(c_contrail_definition, profile_offset) == 0xbc) ? 1 : -1];
typedef char contrail_definition_color_offset[(offsetof(c_contrail_definition, profile_color) == 0x180) ? 1 : -1];
typedef char effect_property_row_properties_offset[(offsetof(s_contrails_property_row, properties) == 0x4c) ? 1 : -1];
typedef char effect_property_row_states_offset[(offsetof(s_contrails_property_row, used_states) == 0x150) ? 1 : -1];

#endif
