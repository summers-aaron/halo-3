#ifndef AI_CHARACTER_MORPH_PROPERTIES_H
#define AI_CHARACTER_MORPH_PROPERTIES_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Character forms, morph triggers, infection spewing and stalking behavior.
struct character_morph_properties
{
    s_tag_reference forms[3];
    union
    {
        s_tag_reference morph_muffins;
        struct
        {
            unsigned long morph_muffins_group_tag;
            TAG_POINTER(const char) morph_muffins_name;
            long morph_muffins_name_length;
            long morph_time;
        };
    };
    s_tag_reference targets[3];
    float distance_damage_threshold;
    float distance_damage_threshold_sustain;
    float distance_damage_morph_seconds;
    float distance_damage_reset_seconds;
    float tank_far_distance;
    float tank_near_distance;
    float throttle_hold_speed;
    float throttle_hold_seconds;
    union
    {
        s_tag_reference spew_infection_character;
        struct
        {
            unsigned long spew_infection_character_group_tag;
            TAG_POINTER(const char) spew_infection_character_name;
            long spew_infection_character_name_length;
            long infection_definition_index;
        };
    };
    float spew_infection_chance;
    long infection_marker;
    real_bounds spew_frequency;
    float stalker_morph_distance;
    float stalker_morph_damage;
    float stalk_min_distance;
    float stalk_max_distance;
    float stalk_far_distance;
    float stalk_charge_chance;
    float proximity_threshold;
    float damage_threshold;
    float turtle_time_min;
    float turtle_time_max;
    float turtle_propagation_radius;
    float ranged_morph_radius;
    float group_morph_radius;
};

typedef char character_morph_properties_size[(sizeof(character_morph_properties) == 0xe4) ? 1 : -1];
typedef char character_morph_muffins_offset[(offsetof(character_morph_properties, morph_muffins) == 0x30) ? 1 : -1];
typedef char character_morph_infection_offset[(offsetof(character_morph_properties, spew_infection_character) == 0x90) ? 1 : -1];
typedef char character_morph_spew_frequency_offset[(offsetof(character_morph_properties, spew_frequency) == 0xa8) ? 1 : -1];
typedef char character_morph_stalk_charge_offset[(offsetof(character_morph_properties, stalk_charge_chance) == 0xc4) ? 1 : -1];
typedef char character_morph_time_offset[(offsetof(character_morph_properties, morph_time) == 0x3c) ? 1 : -1];
typedef char character_morph_targets_offset[(offsetof(character_morph_properties, targets) == 0x40) ? 1 : -1];
typedef char character_morph_damage_offset[(offsetof(character_morph_properties, distance_damage_threshold) == 0x70) ? 1 : -1];
typedef char character_morph_stalker_offset[(offsetof(character_morph_properties, stalker_morph_distance) == 0xb0) ? 1 : -1];
typedef char character_morph_proximity_offset[(offsetof(character_morph_properties, proximity_threshold) == 0xc8) ? 1 : -1];
typedef char character_morph_ranged_radius_offset[(offsetof(character_morph_properties, ranged_morph_radius) == 0xdc) ? 1 : -1];
typedef char character_morph_group_radius_offset[(offsetof(character_morph_properties, group_morph_radius) == 0xe0) ? 1 : -1];

#endif // AI_CHARACTER_MORPH_PROPERTIES_H
