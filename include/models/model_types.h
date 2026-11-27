#ifndef MODELS_MODEL_TYPES_H
#define MODELS_MODEL_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "animations/animation_codec_flags.h"

struct s_model_region_permutation
{
    long name;
    unsigned char flags;
    signed char collision_permutation_index;
    signed char physics_permutation_index;
};

struct s_model_region
{
    long name;
    signed char collision_region_index;
    signed char physics_region_index;
    s_tag_block permutations;
};

struct s_model_fade_parameters
{
    float disappear_distance;
    float fade_start_distance;
    float render_time_animation_distance;
    float shadow_fade_start_distance;
    float lod_distance;
    unsigned long lod_model_group_tag;
    TAG_POINTER(const char) lod_model_name;
    long lod_model_name_length;
    long render_model_index;
};

// Local-space bounds for each model object type.
struct s_model_object_data
{
    short type;
    real_point3d offset;
    float radius;
};
typedef char model_object_data_size[(sizeof(s_model_object_data) == 0x14) ? 1 : -1];
typedef char model_object_data_radius_offset[(offsetof(s_model_object_data, radius) == 0x10) ? 1 : -1];

// Shadow-capsule element of s_model_definition.shadow_capsules: a marker-to-marker axis plus radius.
struct s_model_shadow_capsule
{
    long start_marker_name, start_marker_index;
    long end_marker_name, end_marker_index;
    float radius;
};
typedef char model_shadow_capsule_size[(sizeof(s_model_shadow_capsule) == 0x14) ? 1 : -1];

struct s_model_definition
{
    unsigned long render_model_group_tag;
    TAG_POINTER(const char) render_model_name;
    long render_model_name_length;
    long render_model_index;
    unsigned long collision_model_group_tag;
    TAG_POINTER(const char) collision_model_name;
    long collision_model_name_length;
    long collision_model_index;
    s_tag_reference animation_graph;
    unsigned long physics_model_group_tag;
    TAG_POINTER(const char) physics_model_name;
    long physics_model_name_length;
    long physics_model_index;
    union {
        s_model_fade_parameters fade;
        struct {
            float disappear_distance;
            float fade_start_distance;
            float render_time_animation_distance;
            float shadow_fade_start_distance;
            float lod_distance;
            unsigned long lod_model_group_tag;
            TAG_POINTER(const char) lod_model_name;
            long lod_model_name_length;
            long skinning_render_model_index;
        };
    };
    s_tag_block variants;
    s_tag_block groups;
    s_tag_block materials;
    s_tag_block damage_info;
    s_tag_block targets;
    s_tag_block regions;
    s_tag_block nodes;
    long runtime_node_list_checksum;
    c_typed_tag_block<s_model_object_data> object_data;
    s_tag_reference default_dialogue;
    s_tag_reference alternate_dialogue;
    unsigned long flags;
    long dialogue_variant;
    flags256 node_mask;
    flags256 render_only_section_flags;
    unsigned long runtime_flags;
    s_tag_block scenario_load_parameters;
    unsigned char shadow_detail;
    unsigned char shadow_bounces;
    s_tag_block shadow_cast_overrides;
    s_tag_block shadow_receive_overrides;
    s_tag_block shadow_capsules;
    unsigned long shield_impact_third_person_group_tag;
    TAG_POINTER(const char) shield_impact_third_person_name;
    long shield_impact_third_person_name_length;
    long tag_index_174;
    unsigned long shield_impact_first_person_group_tag;
    TAG_POINTER(const char) shield_impact_first_person_name;
    long shield_impact_first_person_name_length;
    long tag_index_184;
};

typedef char model_object_data_offset[(offsetof(s_model_definition, object_data) == 0xbc) ? 1 : -1];
typedef char model_definition_size[(sizeof(s_model_definition) == 0x188) ? 1 : -1];
typedef char model_section_flags_offset[(offsetof(s_model_definition, render_only_section_flags) == 0x110) ? 1 : -1];
typedef char model_runtime_flags_offset[(offsetof(s_model_definition, runtime_flags) == 0x130) ? 1 : -1];
typedef char model_scenario_load_parameters_offset[(offsetof(s_model_definition, scenario_load_parameters) == 0x134) ? 1 : -1];
typedef char model_shadow_detail_offset[(offsetof(s_model_definition, shadow_detail) == 0x140) ? 1 : -1];
typedef char model_shadow_cast_overrides_offset[(offsetof(s_model_definition, shadow_cast_overrides) == 0x144) ? 1 : -1];
typedef char model_shadow_receive_overrides_offset[(offsetof(s_model_definition, shadow_receive_overrides) == 0x150) ? 1 : -1];

typedef char model_render_override_offset[(offsetof(s_model_definition, tag_index_174) == 0x174) ? 1 : -1];
typedef char model_render_override_alternate_offset[(offsetof(s_model_definition, tag_index_184) == 0x184) ? 1 : -1];

typedef char model_fade_parameters_size[(sizeof(s_model_fade_parameters) == 0x24) ? 1 : -1];
typedef char model_fade_render_model_offset[(offsetof(s_model_fade_parameters, render_model_index) == 0x20) ? 1 : -1];
typedef char model_fade_offset[(offsetof(s_model_definition, fade) == 0x40) ? 1 : -1];
typedef char model_fade_shadow_offset[(offsetof(s_model_definition, fade.shadow_fade_start_distance) == 0x4c) ? 1 : -1];
typedef char model_fade_lod_offset[(offsetof(s_model_definition, fade.lod_distance) == 0x50) ? 1 : -1];
typedef char model_collision_offset[(offsetof(s_model_definition, collision_model_index) == 0x1c) ? 1 : -1];
typedef char model_collision_permutation_offset[(offsetof(s_model_region_permutation, collision_permutation_index) == 5) ? 1 : -1];
typedef char model_permutation_size[(sizeof(s_model_region_permutation) == 8) ? 1 : -1];
typedef char model_region_size[(sizeof(void *) != 4 || sizeof(s_model_region) == 0x14) ? 1 : -1];
typedef char model_physics_offset[(offsetof(s_model_definition, physics_model_index) == 0x3c) ? 1 : -1];
typedef char model_render_offset[(offsetof(s_model_definition, render_model_index) == 0x0c) ? 1 : -1];
typedef char model_skinning_render_offset[(offsetof(s_model_definition, skinning_render_model_index) == 0x60) ? 1 : -1];
typedef char model_materials_offset[(offsetof(s_model_definition, materials) == 0x7c) ? 1 : -1];

typedef char model_damage_info_offset[(offsetof(s_model_definition, damage_info) == 0x88) ? 1 : -1];
typedef char model_targets_offset[(offsetof(s_model_definition, targets) == 0x94) ? 1 : -1];
typedef char model_regions_offset[(offsetof(s_model_definition, regions) == 0xa0) ? 1 : -1];
typedef char model_flags_offset[(offsetof(s_model_definition, flags) == 0xe8) ? 1 : -1];

typedef char model_shadow_capsules_offset[(offsetof(s_model_definition, shadow_capsules) == 0x15c) ? 1 : -1];

typedef char model_animation_distance_offset[(offsetof(s_model_definition, render_time_animation_distance) == 0x48) ? 1 : -1];
typedef char model_node_mask_offset[(offsetof(s_model_definition, node_mask) == 0xf0) ? 1 : -1];

typedef char model_default_dialogue_offset[(offsetof(s_model_definition, default_dialogue.index) == 0xd4) ? 1 : -1];
typedef char model_alternate_dialogue_offset[(offsetof(s_model_definition, alternate_dialogue.index) == 0xe4) ? 1 : -1];
typedef char model_dialogue_variant_offset[(offsetof(s_model_definition, dialogue_variant) == 0xec) ? 1 : -1];

#endif // MODELS_MODEL_TYPES_H
