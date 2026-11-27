#ifndef ITEMS_EQUIPMENT_DEFINITIONS_H
#define ITEMS_EQUIPMENT_DEFINITIONS_H

#include <stddef.h>
#include "game/materials.h"
#include "items/item_definitions.h"
#include "tag_files/tag_types.h"

struct s_equipment_super_shield_definition
{
    float shield_recharge_delay_scale;
    float shield_recharge_rate_scale;
    float shield_ceiling_scale;
    s_tag_reference shield_effect;
    s_tag_reference overcharge_effect;
    s_tag_reference overcharge_damage_effect;
};

struct s_equipment_spawner_definition
{
    s_tag_reference spawned_object;
    s_tag_reference spawned_effect;
    float spawn_radius;
    float spawn_z_offset;
    float spawn_area_radius;
    float spawn_velocity;
    short type;
};

struct s_equipment_orbit_definition
{
    float arm_time;
    float radius;
    long function;
    float speed;
};

typedef char equipment_shield_ceiling_offset[(offsetof(s_equipment_super_shield_definition, shield_ceiling_scale) == 8) ? 1 : -1];
typedef char equipment_spawner_object_index_offset[
    (offsetof(s_equipment_spawner_definition, spawned_object) + offsetof(s_tag_reference, index) == 0xc) ? 1 : -1];

enum e_equipment_type
{
    _equipment_type_none = -1,
    k_equipment_type_count = 9
};

struct s_equipment_type_invincibility
{
    long material_name;
    c_global_material_type material_type;

    float duration;
    s_tag_reference start_effect;
    s_tag_reference looping_effect;
    s_tag_reference end_effect;
};

struct s_equipment_proximity_mine_definition
{
    s_tag_reference detonation_effect;
    s_tag_reference detonation_damage_effect;
    float arm_time;
    float self_destruct_time;
    float trigger_time;
    float trigger_velocity;
};

typedef char equipment_invincibility_material_offset[(offsetof(s_equipment_type_invincibility, material_type) == 4) ? 1 : -1];
typedef char equipment_invincibility_duration_offset[(offsetof(s_equipment_type_invincibility, duration) == 8) ? 1 : -1];
typedef char equipment_invincibility_size[(sizeof(s_equipment_type_invincibility) == 0x3c) ? 1 : -1];

typedef char equipment_layout_start_effect[(offsetof(s_equipment_type_invincibility, start_effect) == 0xc) ? 1 : -1];
typedef char equipment_layout_looping_effect[(offsetof(s_equipment_type_invincibility, looping_effect) == 0x1c) ? 1 : -1];
typedef char equipment_layout_end_effect[(offsetof(s_equipment_type_invincibility, end_effect) == 0x2c) ? 1 : -1];
typedef char equipment_layout_mine_size[(sizeof(s_equipment_proximity_mine_definition) == 0x30) ? 1 : -1];

struct s_equipment_definition
{
    s_item_definition item;
    float duration;
    float phantom_duration;
    short max_charges;
    union { short flags; short ai_flags; }; // obstacle avoidance bit 0; combat throw targeting bit 2
    float ai_radius;
    float ai_throw_distance;
    float activation_delay;
    s_tag_block equipment_types[k_equipment_type_count];
    unsigned long hud_interface_group_tag;
    TAG_POINTER(const char) hud_interface_name;
    long hud_interface_name_length;
    long hud_interface_index;
    s_tag_reference pickup_sound;
    unsigned long activate_effect_group_tag;
    TAG_POINTER(const char) activate_effect_name;
    long activate_effect_name_length;
    long sound_index;
    unsigned long running_effect_group_tag;
    TAG_POINTER(const char) running_effect_name;
    long running_effect_name_length;
    long effect_index;
    unsigned long deactivate_sound_group_tag;
    TAG_POINTER(const char) deactivate_sound_name;
    long deactivate_sound_name_length;
    long activation_sound;
};

typedef char equipment_activation_delay_offset[(offsetof(s_equipment_definition, activation_delay) == 0x1c0) ? 1 : -1];
typedef char equipment_definition_flags_offset[(offsetof(s_equipment_definition, flags) == 0x1b6) ? 1 : -1];

typedef char equipment_definition_types_offset[(offsetof(s_equipment_definition, equipment_types) == 0x1c4) ? 1 : -1];
typedef char equipment_pickup_sound_offset[(offsetof(s_equipment_definition, pickup_sound.index) == 0x24c) ? 1 : -1];

typedef char equipment_definition_sound_offset[(offsetof(s_equipment_definition, sound_index) == 0x25c) ? 1 : -1];
typedef char equipment_definition_size[(sizeof(s_equipment_definition) == 0x280) ? 1 : -1];
typedef char equipment_definition_hud_offset[(offsetof(s_equipment_definition, hud_interface_index) == 0x23c) ? 1 : -1];

e_equipment_type equipment_definition_get_type(long tag_index, long which);

typedef char equipment_ai_throw_distance_offset[(offsetof(s_equipment_definition, ai_throw_distance) == 0x1bc) ? 1 : -1];

#endif // ITEMS_EQUIPMENT_DEFINITIONS_H
