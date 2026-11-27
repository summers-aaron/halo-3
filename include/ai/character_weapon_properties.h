#ifndef AI_CHARACTER_WEAPON_PROPERTIES_H
#define AI_CHARACTER_WEAPON_PROPERTIES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct character_firing_pattern;

struct character_weapon_properties
{
    unsigned long flags;
    union
    {
        s_tag_reference weapon;
        struct
        {
            unsigned long weapon_group_tag;
            TAG_POINTER(const char) weapon_name;
            long weapon_name_length;
            long weapon_definition_index;
        };
    };
    float maximum_range;
    float maximum_firing_range;
    float default_minimum;
    float default_maximum;
    float bombardment_range;
    float maximum_targeting_distance;
    float mode1_minimum;
    float mode1_maximum;
    float mode2_minimum;
    float mode2_maximum;
    float super_ballistic_range;
    real_bounds ballistic_firing_bounds;
    real_bounds ballistic_fraction_bounds;
    float first_delay_lower;
    float first_delay_upper;
    float surprise_delay_time;
    float surprise_fire_wildly_time;
    float berserk_chance;
    float berserk_duration;
    real_vector3d estimate_direction;
    real_vector3d custom_crouch_gun_offset;
    short special_fire_mode;
    short fire_mode;
    float special_fire_chance;
    float special_fire_time;
    float special_fire_delay;
    float special_fire_aim_velocity;
    float amount_min, amount_max;
    short rounds_min, rounds_max;
    float burst[3][3];
    c_typed_tag_block<character_firing_pattern> firing_patterns;
    union
    {
        s_tag_reference weapon_melee_damage;
        struct
        {
            unsigned long melee_damage_group_tag;
            TAG_POINTER(const char) melee_damage_name;
            long melee_damage_name_length;
            long melee_damage;
        };
    };
};

typedef char character_weapon_properties_size[(sizeof(character_weapon_properties) == 0xe0) ? 1 : -1];
typedef char character_weapon_melee_damage_offset[(offsetof(character_weapon_properties, melee_damage) == 0xdc) ? 1 : -1];
typedef char character_weapon_maximum_range_offset[(offsetof(character_weapon_properties, maximum_firing_range) == 0x18) ? 1 : -1];
typedef char character_weapon_flags_offset[(offsetof(character_weapon_properties, flags) == 0) ? 1 : -1];
typedef char character_weapon_fire_mode_offset[(offsetof(character_weapon_properties, fire_mode) == 0x82) ? 1 : -1];
typedef char character_weapon_burst_offset[(offsetof(character_weapon_properties, burst) == 0xa0) ? 1 : -1];
typedef char character_weapon_range_offset[(offsetof(character_weapon_properties, maximum_range) == 0x14) ? 1 : -1];
typedef char character_weapon_bombardment_offset[(offsetof(character_weapon_properties, bombardment_range) == 0x24) ? 1 : -1];
typedef char character_weapon_estimate_offset[(offsetof(character_weapon_properties, estimate_direction) == 0x68) ? 1 : -1];
typedef char character_weapon_reference_offset[(offsetof(character_weapon_properties, weapon) == 4) ? 1 : -1];
typedef char character_weapon_surprise_delay_offset[(offsetof(character_weapon_properties, surprise_delay_time) == 0x58) ? 1 : -1];
typedef char character_weapon_surprise_fire_offset[(offsetof(character_weapon_properties, surprise_fire_wildly_time) == 0x5c) ? 1 : -1];
typedef char character_weapon_crouch_offset[(offsetof(character_weapon_properties, custom_crouch_gun_offset) == 0x74) ? 1 : -1];
typedef char character_weapon_patterns_offset[(offsetof(character_weapon_properties, firing_patterns) == 0xc4) ? 1 : -1];
typedef char character_weapon_melee_reference_offset[(offsetof(character_weapon_properties, weapon_melee_damage) == 0xd0) ? 1 : -1];

#endif
