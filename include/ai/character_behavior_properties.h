#ifndef AI_CHARACTER_BEHAVIOR_PROPERTIES_H
#define AI_CHARACTER_BEHAVIOR_PROPERTIES_H
#include <stddef.h>
#include "tag_files/tag_types.h"
// Character tag blocks shared by actor behaviors.
struct character_retreat_properties
{
    long flags;
    float shield_threshold;
    float scary_target_threshold;
    float danger_threshold;
    float proximity;
    float cower_min;
    float cower_max;
    float timeout_min;
    float timeout_max;
    float ambush_proximity;
    float ambush_vitality;
    float threshold;
    float peer_chance;
    float recent_peer_chance;
    float flee_timeout;
    float zigzag_angle;
    float zigzag_period;
    float retreat_grenade_chance;
    s_tag_reference backup_weapon;
};

struct character_search_properties
{
    long flags;
    float search_time_lower, search_time_upper;
    float investigate_radius;
    float uncover_distance_lower;
    float uncover_distance_upper;
    float orphan_offset;
    float minimum_offset;
};

struct character_cover_properties
{
    long flags;
    float cover_time_lower, cover_time_upper;
    float shield_threshold;
    float vitality_threshold;
    float danger_time;
    float danger_lower;
    float danger_upper;
    float chance_lower, chance_upper;
    float shield_chance_lower, shield_chance_upper;
    float cover_time;
    float shield_recovery;
    float proximity;
    float melee_distance;
    float unreachable_danger;
    float scary_target_threshold;
    float shield_depleted_chance;
    float shield_equipment_vitality_fraction;
    float recent_damage_shield_equipment;
};

struct character_engage_properties
{
    long flags;
    float maximum_time, minimum_time;
    float crouch_threshold;
    float stand_threshold;
    float fight_danger_move_threshold;
    s_tag_reference override_grenade_projectile;
};
typedef char character_engage_projectile_offset[(offsetof(character_engage_properties, override_grenade_projectile.index) == 0x24) ? 1 : -1];
typedef char character_engage_size[(sizeof(character_engage_properties) == 0x28) ? 1 : -1];

struct character_ready_properties { float minimum; float maximum; };

struct character_vitality_properties
{
    long flags;
    float body_vitality;
    float shield_vitality;
    float heroic_body_vitality;
    float heroic_shield_vitality;
    float body_recharge_fraction;
    float soft_ping_shield_threshold;
    float soft_ping_body_threshold;
    float soft_ping_minimum_interrupt_time;
    float hard_ping_shield_threshold;
    float hard_ping_body_threshold;
    float ping_time;
    float current_damage_decay_delay;
    float current_damage_decay_time;
    float recent_damage_decay_delay;
    float recent_damage_decay_time;
    float stun_time;
    float body_recharge_time;
    float surge_duration;
    float shield_recharge_time;
    float stun_threshold;
    float stun_minimum;
    float stun_maximum;
    float recoverable_damage_threshold;
    float shield_damage_threshold;
    float suicide_radius;
    float body_recharge_rate;
    float shield_recharge_rate;
    s_tag_reference resurrection_weapon;
};

struct character_presearch_properties
{
    long flags;
    float minimum_lower, minimum_upper, maximum_lower, maximum_upper;
    float maximum_distance;
    float deprecated;
    float suppress_min, suppress_max;
    short maximum_searchers;
};

struct character_grenade_properties
{
    long flags;
    union { short grenade_type; short identifier; };
    short trajectory_type;
    short pad8;
    short collateral_damage_limit;
    float throw_velocity;
    float grenade_ideal_velocity;
    float grenade_velocity;
    float minimum_distance;
    float maximum_distance;
    float damage_radius;
    float target_throw_chance;
    float throw_delay;
    float throw_chance;
    float anti_vehicle_throw_chance;
    short minimum_count;
    short maximum_count;
    float drop_chance;
};
typedef char character_retreat_properties_size[(sizeof(character_retreat_properties) == 88) ? 1 : -1];
typedef char character_search_properties_size[(sizeof(character_search_properties) == 32) ? 1 : -1];
typedef char character_cover_properties_size[(sizeof(character_cover_properties) == 84) ? 1 : -1];
typedef char character_engage_properties_size[(sizeof(character_engage_properties) == 40) ? 1 : -1];
typedef char character_ready_properties_size[(sizeof(character_ready_properties) == 8) ? 1 : -1];
typedef char character_vitality_properties_size[(sizeof(character_vitality_properties) == 0x80) ? 1 : -1];
typedef char character_presearch_properties_size[(sizeof(character_presearch_properties) == 40) ? 1 : -1];
typedef char character_grenade_properties_size[(sizeof(character_grenade_properties) == 60) ? 1 : -1];
typedef char character_grenade_identifier_offset[(offsetof(character_grenade_properties, identifier) == 4) ? 1 : -1];
typedef char character_grenade_minimum_count_offset[(offsetof(character_grenade_properties, minimum_count) == 0x34) ? 1 : -1];
typedef char character_grenade_maximum_count_offset[(offsetof(character_grenade_properties, maximum_count) == 0x36) ? 1 : -1];
typedef char character_vitality_damage_offset[(offsetof(character_vitality_properties, recoverable_damage_threshold) == 0x5c) ? 1 : -1];
typedef char character_vitality_recharge_offset[(offsetof(character_vitality_properties, body_recharge_rate) == 0x68) ? 1 : -1];
typedef char character_vitality_surge_offset[(offsetof(character_vitality_properties, surge_duration) == 0x48) ? 1 : -1];
typedef char character_retreat_zigzag_offset[(offsetof(character_retreat_properties, zigzag_angle) == 0x3c) ? 1 : -1];
typedef char character_retreat_backup_weapon_offset[(offsetof(character_retreat_properties, backup_weapon.index) == 0x54) ? 1 : -1];
typedef char character_cover_time_offset[(offsetof(character_cover_properties, cover_time) == 0x30) ? 1 : -1];
typedef char character_vitality_ping_time_offset[(offsetof(character_vitality_properties, ping_time) == 0x2c) ? 1 : -1];
typedef char character_vitality_stun_threshold_offset[(offsetof(character_vitality_properties, stun_threshold) == 0x50) ? 1 : -1];
typedef char character_vitality_suicide_radius_offset[(offsetof(character_vitality_properties, suicide_radius) == 0x64) ? 1 : -1];
typedef char character_grenade_velocity_offset[(offsetof(character_grenade_properties, throw_velocity) == 0xc) ? 1 : -1];
typedef char character_grenade_damage_radius_offset[(offsetof(character_grenade_properties, damage_radius) == 0x20) ? 1 : -1];
typedef char character_grenade_drop_chance_offset[(offsetof(character_grenade_properties, drop_chance) == 0x38) ? 1 : -1];
typedef char character_cover_melee_offset[(offsetof(character_cover_properties, melee_distance) == 0x3c) ? 1 : -1];

#endif
