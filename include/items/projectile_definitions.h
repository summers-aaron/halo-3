#ifndef ITEMS_PROJECTILE_DEFINITIONS_H
#define ITEMS_PROJECTILE_DEFINITIONS_H

#include <stddef.h>
#include "objects/object_definitions.h"
#include "game/material_constants.h"

struct projectile_definition {
  s_object_definition object;
  union {
    unsigned long targeting_flags;
    // Xbox big-endian halfword view used by AI projectile attachment.
#if defined(_XBOX) && !defined(__clang__)
    struct { unsigned short targeting_flags_upper; unsigned short targeting_flags_lower; };
#else
    struct { unsigned short targeting_flags_lower; unsigned short targeting_flags_upper; };
#endif
  };
  union {
    s_tag_block campaign_metagame;
    struct {
      short timer_start;
      short impact_ai_volume;
      float collision_radius;
      float timer_duration;
    };
  };
  float ai_radius;
  float lifetime_minimum;
  float detonation_timer;
  float minimum_velocity;
  float lifetime;
  float maximum_travel_distance;
  short ai_volume;
  short supercombine_count;
  float super_detonation_time;
  s_tag_reference detonation_started;
  s_tag_reference detonation_effect_airborne;
  s_tag_reference detonation_effect_ground;
  s_tag_reference detonation_damage;
  s_tag_reference impact_damage;
  s_tag_reference super_detonation;
  s_tag_reference super_detonation_damage;
  s_tag_reference detonation_sound;
  union { unsigned char damage_reporting_flags; unsigned char reporting_type; };
  s_tag_reference super_impact_damage;
  float detonation_proximity;
  s_tag_reference flyby_sound;
  s_tag_reference impact_effect;
  s_tag_reference object_impact_effect;
  s_tag_reference collision_damage;
  float attached_detonation_timescale;
  s_tag_reference attached_detonation_damage;
  s_tag_reference attached_impact_damage;
  float air_gravity_scale;
  float minimum_damage_distance_air;
  float maximum_damage_distance_default;
  float water_gravity_scale;
  float minimum_damage_distance_water;
  float maximum_damage_distance_flagged;
  union { float guided1; float initial_velocity; };
  float guided2;
  float single_player_velocity;
  float multiplayer_velocity;
  float angular_velocity_minimum;
  float angular_velocity_maximum;
  float minimum_angular_velocity;
  float start;
  float acceleration_range_upper;
  float scale;
  float target_leading;
  float target_error;
  float autoaim_leading_max_lead_time;
  s_tag_block responses;
  s_tag_block brute_grenade;
  s_tag_block detonation_offset;
  s_tag_block conical_distribution;
};


typedef char projectile_definition_metagame_offset[(offsetof(projectile_definition, campaign_metagame) == 0xfc) ? 1 : -1];
typedef char projectile_definition_initial_velocity_offset[(offsetof(projectile_definition, initial_velocity) == 0x23c) ? 1 : -1];
typedef char projectile_definition_reporting_type_offset[(offsetof(projectile_definition, reporting_type) == 0x1a8) ? 1 : -1];
#if defined(_XBOX) && !defined(__clang__)
typedef char projectile_definition_targeting_upper_offset[(offsetof(projectile_definition, targeting_flags_upper) == 0xf8) ? 1 : -1];
#else
typedef char projectile_definition_targeting_upper_offset[(offsetof(projectile_definition, targeting_flags_upper) == 0xfa) ? 1 : -1];
#endif
typedef char projectile_definition_ai_radius_offset[(offsetof(projectile_definition, ai_radius) == 0x108) ? 1 : -1];
typedef char projectile_definition_targeting_offset[(offsetof(projectile_definition, targeting_flags) == 0xf8) ? 1 : -1];
typedef char projectile_definition_guided_offset[(offsetof(projectile_definition, guided1) == 0x23c) ? 1 : -1];
typedef char projectile_definition_size[(sizeof(projectile_definition) == 0x2a0) ? 1 : -1];
typedef char projectile_collision_radius_offset[(offsetof(projectile_definition, collision_radius) == 0x100) ? 1 : -1];
typedef char projectile_super_detonation_time_offset[(offsetof(projectile_definition, super_detonation_time) == 0x124) ? 1 : -1];
typedef char projectile_detonation_started_offset[(offsetof(projectile_definition, detonation_started) == 0x128) ? 1 : -1];
typedef char projectile_impact_effect_offset[(offsetof(projectile_definition, impact_effect) == 0x1d0) ? 1 : -1];
typedef char projectile_air_damage_lower_offset[(offsetof(projectile_definition, minimum_damage_distance_air) == 0x228) ? 1 : -1];
typedef char projectile_water_damage_lower_offset[(offsetof(projectile_definition, minimum_damage_distance_water) == 0x234) ? 1 : -1];
typedef char projectile_acceleration_upper_offset[(offsetof(projectile_definition, acceleration_range_upper) == 0x25c) ? 1 : -1];
typedef char projectile_autoaim_lead_offset[(offsetof(projectile_definition, autoaim_leading_max_lead_time) == 0x26c) ? 1 : -1];

#endif
