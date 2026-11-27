#ifndef UNITS_BIPED_DEFINITIONS_H
#define UNITS_BIPED_DEFINITIONS_H

#include <stddef.h>

#include "tag_files/tag_types.h"
#include "physics/character_physics_definition_types.h"
#include "units/unit_definitions.h"

// Biped movement, camera, collision, and leap settings.
struct biped_definition
{
    unit_definition unit;
    float moving_turning_speed;
    unsigned long flags;                         // biped_falling_danger tests bit 2
    float stationary_turning_threshold;
    float jump_velocity;
    float soft_landing_recovery_time;
    float hard_landing_recovery_time;
    float minimum_soft_landing_velocity;
    float minimum_hard_landing_velocity;
    float maximum_hard_landing_velocity;
    float death_hard_landing_velocity;
    float stun_duration;
    float standing_camera_height;
    float crouching_camera_height;
    float crouch_transition_time;
    float camera_angle_min;
    float camera_angle_max;
    float camera_scale_x;
    float camera_scale_y;
    float camera_scale_z;
    float camera_min_value;
    float autoaim_radius;
    long autoaim_flags;
    float autoaim_width;
    short sentinel_animation_node_index;
    float turning_velocity_maximum;               // biped_update_turning: flying turn rate when not overridden
    float crouch_velocity;
    union { short autoaim_node_a; short perch_node_index; };
    short autoaim_node_b;
    float head_shot_acceleration_scale;
    s_tag_reference death_effect;
    s_character_physics_definition physics;       // biped_update_turning passes &physics.flying (+0x410)
    s_tag_block contact_points;                   // bipeds.cpp s_biped_definition
    s_tag_reference reanimation_character;        // a dead biped is infectable only with one (swarm_behavior prop_ref_infectable)
    s_tag_reference reanimation_morph_muffins;    // creature_ground_motor_program infect request
    s_tag_reference death_spawn_character;
    unsigned short death_spawn_marker;
    unsigned long leap_flags;
    float leap_dampening_scale;
    float leap_roll_delay;
    float cannonball_offaxis_scale;
    float cannonball_offtrack_scale;
    float cannonball_roll_lower;
    float cannonball_roll_upper;
    float anticipation_lower;
    float anticipation_upper;
    float leap_recovery_lower;
    float leap_recovery_upper;
    float maximum_time_scale;                    // leap ballistic aim time bound
    float ground_normal_dampening;
    float root_offset_max_scale;
    float root_offset_dampening;
    float following_camera_scale;
    float root_leaning_scale;
    float foot_roll_maximum;
    float foot_pitch_maximum;
    float moving_turn_minimum;
    float moving_turn_height_threshold;
    float moving_turn_width_scale;
    float moving_turn_unknown;
    float moving_turn_smoothing;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char biped_definition_offset_turning_velocity_maximum[(offsetof(biped_definition, turning_velocity_maximum) == 0x36c) ? 1 : -1];
typedef char biped_definition_offset_moving_turning_speed[(offsetof(biped_definition, moving_turning_speed) == 0x30c) ? 1 : -1];
typedef char biped_definition_offset_flags[(offsetof(biped_definition, flags) == 0x310) ? 1 : -1];
typedef char biped_definition_offset_camera_height[(offsetof(biped_definition, standing_camera_height) == 0x338) ? 1 : -1];
typedef char biped_definition_offset_autoaim_nodes[(offsetof(biped_definition, autoaim_node_a) == 0x374) ? 1 : -1];
typedef char biped_definition_offset_ground_dampening[(offsetof(biped_definition, ground_normal_dampening) == 0x4b0) ? 1 : -1];
typedef char biped_definition_offset_reanimation_morph_muffins[(offsetof(biped_definition, reanimation_morph_muffins) == 0x45c) ? 1 : -1];
typedef char biped_definition_offset_physics_flying[(offsetof(biped_definition, physics.flying) == 0x410) ? 1 : -1];
#endif
typedef char biped_definition_offset_jump_velocity[(offsetof(biped_definition, jump_velocity) == 0x318) ? 1 : -1];
typedef char biped_definition_offset_landing_recovery[(offsetof(biped_definition, soft_landing_recovery_time) == 0x31c) ? 1 : -1];
typedef char biped_definition_offset_maximum_landing[(offsetof(biped_definition, maximum_hard_landing_velocity) == 0x32c) ? 1 : -1];
typedef char biped_definition_offset_contact_points[(offsetof(biped_definition, contact_points) == 0x440) ? 1 : -1];
typedef char biped_definition_offset_reanimation_character[(offsetof(biped_definition, reanimation_character) == 0x44c) ? 1 : -1];

typedef char biped_anticipation_lower_offset[(offsetof(biped_definition, anticipation_lower) == 0x49c) ? 1 : -1];
typedef char biped_leap_recovery_lower_offset[(offsetof(biped_definition, leap_recovery_lower) == 0x4a4) ? 1 : -1];
typedef char biped_anticipation_upper_offset[(offsetof(biped_definition, anticipation_upper) == 0x4a0) ? 1 : -1];
typedef char biped_leap_recovery_upper_offset[(offsetof(biped_definition, leap_recovery_upper) == 0x4a8) ? 1 : -1];
typedef char biped_leap_maximum_time_scale_offset[(offsetof(biped_definition, maximum_time_scale) == 0x4ac) ? 1 : -1];

typedef char biped_autoaim_flags_offset[(offsetof(biped_definition, autoaim_flags) == 0x360) ? 1 : -1];
typedef char biped_autoaim_width_offset[(offsetof(biped_definition, autoaim_width) == 0x364) ? 1 : -1];
typedef char biped_crouch_velocity_offset[(offsetof(biped_definition, crouch_velocity) == 0x370) ? 1 : -1];
typedef char biped_head_shot_acceleration_offset[(offsetof(biped_definition, head_shot_acceleration_scale) == 0x378) ? 1 : -1];
typedef char biped_death_spawn_character_offset[(offsetof(biped_definition, death_spawn_character.index) == 0x478) ? 1 : -1];
typedef char biped_death_spawn_marker_offset[(offsetof(biped_definition, death_spawn_marker) == 0x47c) ? 1 : -1];
typedef char biped_stationary_turning_offset[(offsetof(biped_definition, stationary_turning_threshold) == 0x314) ? 1 : -1];
typedef char biped_death_landing_offset[(offsetof(biped_definition, death_hard_landing_velocity) == 0x330) ? 1 : -1];
typedef char biped_crouch_transition_offset[(offsetof(biped_definition, crouch_transition_time) == 0x340) ? 1 : -1];
typedef char biped_death_effect_offset[(offsetof(biped_definition, death_effect.index) == 0x388) ? 1 : -1];
typedef char biped_leap_flags_offset[(offsetof(biped_definition, leap_flags) == 0x480) ? 1 : -1];
typedef char biped_cannonball_roll_offset[(offsetof(biped_definition, cannonball_roll_lower) == 0x494) ? 1 : -1];
typedef char biped_definition_size[(sizeof(biped_definition) == 0x4e0) ? 1 : -1];
#endif // UNITS_BIPED_DEFINITIONS_H
