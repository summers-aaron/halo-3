#ifndef AI_CHARACTER_PROPERTIES_TYPES_H
#define AI_CHARACTER_PROPERTIES_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// Character movement and melee charge settings.
struct character_movement_properties
{
    unsigned long flags;
    float pathfinding_radius;
    float destination_tolerance;
    float dive_evasion_chance;
    short obstacle_leap_minimum_size;
    short obstacle_leap_maximum_size;
    short obstacle_ignore_size;
    short obstacle_smashable_size;
    short pad18;
    short jump_height;
    long hoist_flags;
    float maximum_throttle;
    float animation_throttle_scale;
    float max_path_distance;
};

struct s_character_charge_entry
{
    short max_kamikazes;
    short maximum_flagged;
    short minimum_flagged;
};

typedef char character_charge_entry_size[(sizeof(s_character_charge_entry) == 6) ? 1 : -1];
typedef char character_charge_entry_maximum_offset[(offsetof(s_character_charge_entry, maximum_flagged) == 2) ? 1 : -1];
typedef char character_charge_entry_minimum_offset[(offsetof(s_character_charge_entry, minimum_flagged) == 4) ? 1 : -1];

typedef char character_movement_maximum_throttle_offset[(offsetof(character_movement_properties, maximum_throttle) == 0x20) ? 1 : -1];

typedef char character_movement_max_path_distance_offset[(offsetof(character_movement_properties, max_path_distance) == 0x28) ? 1 : -1];

struct character_charge_properties
{
    long flags;
    float melee_range;
    float melee_chance;
    float surprise_distance;
    float leap_range;
    float duration;
    float melee_delay;
    float min_range;
    float max_range;
    float leap_chance;
    float jump_speed;
    float projectile_min_speed;
    float melee_leap_ballistic;
    float min_leap_delay;
    float abandoned_chance;
    float berserk_chance_far;
    float berserk_chance_near;
    float berserk_range_near;
    float berserk_range_far;
    s_tag_reference berserk_weapon;
    float berserk_duration;
    float kamikaze_chance;
    float perimeter_range;
    float perimeter_range_close;
    float perimeter_damage_timeout;
    long entry_count;
    TAG_POINTER(s_character_charge_entry) entries;
    TAG_POINTER(void) entries_definition;
};

typedef char character_charge_properties_size[(sizeof(void *) != 4 || sizeof(character_charge_properties) == 0x7c) ? 1 : -1];
typedef char character_charge_ballistic_offset[(offsetof(character_charge_properties, melee_leap_ballistic) == 0x30) ? 1 : -1];
typedef char character_charge_weapon_offset[(offsetof(character_charge_properties, berserk_weapon) == 0x4c) ? 1 : -1];
typedef char character_charge_entries_definition_offset[(offsetof(character_charge_properties, entries_definition) == 0x78) ? 1 : -1];
typedef char character_charge_entry_size[(sizeof(s_character_charge_entry) == 6) ? 1 : -1];
typedef char character_movement_properties_size[(sizeof(character_movement_properties) == 0x2c) ? 1 : -1];
typedef char character_movement_dive_evasion_offset[(offsetof(character_movement_properties, dive_evasion_chance) == 0xc) ? 1 : -1];
typedef char character_movement_animation_throttle_offset[(offsetof(character_movement_properties, animation_throttle_scale) == 0x24) ? 1 : -1];
typedef char character_movement_radius_offset[(offsetof(character_movement_properties, pathfinding_radius) == 4) ? 1 : -1];
typedef char character_movement_tolerance_offset[(offsetof(character_movement_properties, destination_tolerance) == 8) ? 1 : -1];
typedef char character_movement_obstacle_leap_offset[(offsetof(character_movement_properties, obstacle_leap_minimum_size) == 0x10) ? 1 : -1];
typedef char character_movement_jump_height_offset[(offsetof(character_movement_properties, jump_height) == 0x1a) ? 1 : -1];
typedef char character_movement_hoist_offset[(offsetof(character_movement_properties, hoist_flags) == 0x1c) ? 1 : -1];
typedef char character_charge_jump_speed_offset[(offsetof(character_charge_properties, jump_speed) == 0x28) ? 1 : -1];
typedef char character_charge_melee_range_offset[(offsetof(character_charge_properties, melee_range) == 4) ? 1 : -1];
typedef char character_charge_perimeter_range_offset[(offsetof(character_charge_properties, perimeter_range) == 0x64) ? 1 : -1];
typedef char character_charge_max_kamikazes_offset[(offsetof(s_character_charge_entry, max_kamikazes) == 0) ? 1 : -1];

typedef char character_charge_duration_offset[(offsetof(character_charge_properties, duration) == 0x14) ? 1 : -1];

typedef char character_charge_kamikaze_offset[(offsetof(character_charge_properties, kamikaze_chance) == 0x60) ? 1 : -1];

#endif // AI_CHARACTER_PROPERTIES_TYPES_H
