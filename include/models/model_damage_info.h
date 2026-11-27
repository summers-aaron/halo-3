#ifndef MODELS_MODEL_DAMAGE_INFO_H
#define MODELS_MODEL_DAMAGE_INFO_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "game/materials.h"

struct damage_region_state {
    long name;
    short state;
    short region;
};
struct s_instantaneous_damage_response {
    short response_type;
    unsigned short constraint_damage_type;
    long constraint_name;
    long flags;
    float threshold;
    s_tag_reference effect;
    s_tag_reference secondary_effect;
    s_tag_reference damage_effect;
    union {
        damage_region_state region_states[2];
        struct {
            long region_name_a;
            short state_a;
            short region_a;
            long region_name_b;
            short state_b;
            short region_b;
        };
    };
    short destroy_instance_group;
    short condition;
    long condition_argument;
    long effect_marker;
    long damage_effect_marker;
    float delay;
    s_tag_reference delay_effect;
    long delay_effect_marker;
    long eject_seat;
    float skip_fraction;
    long destroyed_child_object_marker_name;
    float vitality;
};
struct s_damage_section {
    long name;
    long flags;
    float vitality;
    s_tag_block responses;
    char pad18[48 - 24];
    float damage_stun_time;
    float recharge_time;
    float recharge_rate;
    long resurrection_restored_region_name;
    short model_state;
};
struct s_damage_node {
    short runtime_damage_part;
    char pad2[16 - 2];
};
struct s_model_damage_info {
    long flags;
    long indirect_material_name;
    short indirect_damage_section;
    char padA[16 - 10];
    char collision_damage_reporting_type;
    unsigned char reporting_type;
    char pad12[40 - 18];
    float maximum_body_vitality;
    float minimum_stun_damage;
    float stun_time;
    float body_recharge_time;
    float body_recharge_fraction;
    char pad3c[124 - 60];
    float maximum_shield_vitality;
    long shield_material_name;
    float shield_damage_threshold;
    float shield_stun_time;
    float shield_recharge_time;
    float shield_warning_threshold;
    s_tag_reference shield_warning_effect;
    s_tag_reference shield_depletion_effect;
    s_tag_reference shield_recharge_effect;
    s_tag_block sections;
    s_tag_block nodes;
    c_global_material_type shield_material_type;
    c_global_material_type indirect_material_type;
    float shield_recharge_rate;
    float body_recharge_rate;
    s_tag_block seats;
    s_tag_block constraints;
};

struct s_damage_object_section_state {
    unsigned short flags;
    unsigned char damage;
    unsigned char response_index;
    union {
        unsigned short delayed_response;
#if defined(_XBOX) && !defined(__clang__)
        struct { unsigned short delayed_response_flags : 4; unsigned short delayed_response_index : 12; };
#else
        struct { unsigned short delayed_response_index : 12; unsigned short delayed_response_flags : 4; };
#endif
    };
    short recharge_delay_ticks;
};

typedef char instantaneous_damage_response_size[(sizeof(s_instantaneous_damage_response) == 0x88) ? 1 : -1];
typedef char instantaneous_damage_response_instance_group_offset[
    (offsetof(s_instantaneous_damage_response, destroy_instance_group) == 0x50) ? 1 : -1];
typedef char instantaneous_damage_response_destroyed_marker_offset[
    (offsetof(s_instantaneous_damage_response, destroyed_child_object_marker_name) == 0x80) ? 1 : -1];
typedef char instantaneous_damage_response_delay_effect_offset[(offsetof(s_instantaneous_damage_response, delay_effect) == 0x64) ? 1 : -1];
typedef char damage_section_size[(sizeof(s_damage_section) == 0x44) ? 1 : -1];
typedef char damage_section_recharge_time_offset[(offsetof(s_damage_section, recharge_time) == 0x34) ? 1 : -1];
typedef char damage_section_resurrection_region_offset[(offsetof(s_damage_section, resurrection_restored_region_name) == 0x3c) ? 1 : -1];
typedef char damage_node_size[(sizeof(s_damage_node) == 0x10) ? 1 : -1];
typedef char model_damage_info_size[(sizeof(s_model_damage_info) == 0x100) ? 1 : -1];
typedef char model_damage_info_indirect_material_offset[(offsetof(s_model_damage_info, indirect_material_name) == 4) ? 1 : -1];
typedef char model_damage_info_recharge_time_offset[(offsetof(s_model_damage_info, body_recharge_time) == 0x34) ? 1 : -1];
typedef char model_damage_info_shield_material_offset[(offsetof(s_model_damage_info, shield_material_name) == 0x80) ? 1 : -1];
typedef char model_damage_info_shield_recharge_time_offset[(offsetof(s_model_damage_info, shield_recharge_time) == 0x8c) ? 1 : -1];
typedef char model_damage_info_collision_reporting_offset[(offsetof(s_model_damage_info, collision_damage_reporting_type) == 0x10) ? 1 : -1];
typedef char model_damage_info_sections_offset[(offsetof(s_model_damage_info, sections) == 0xc4) ? 1 : -1];
typedef char damage_object_section_state_size[(sizeof(s_damage_object_section_state) == 8) ? 1 : -1];
typedef char damage_object_section_damage_offset[(offsetof(s_damage_object_section_state, damage) == 2) ? 1 : -1];
typedef char damage_object_section_recharge_delay_offset[
    (offsetof(s_damage_object_section_state, recharge_delay_ticks) == 6) ? 1 : -1];

#endif // MODELS_MODEL_DAMAGE_INFO_H
