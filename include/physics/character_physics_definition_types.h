#ifndef PHYSICS_CHARACTER_PHYSICS_DEFINITION_TYPES_H
#define PHYSICS_CHARACTER_PHYSICS_DEFINITION_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_character_physics_ground_definition
{
    float maximum_slope_angle;
    float downhill_falloff_angle;
    float downhill_cutoff_angle;
    float uphill_falloff_angle;
    float uphill_cutoff_angle;
    float downhill_velocity_scale;
    float uphill_velocity_scale;
    float minimum_normal_k;
    float downhill_k0;
    float downhill_k1;
    float uphill_k0;
    float uphill_k1;
    float climb_inflection_angle;
    float airborne_reaction_time_scale;
    float ground_adhesion_velocity_scale;
    float gravity_scale;
};

// Flying parameters: bank angle and timing, pitch ratio, and angular limits.
struct s_character_physics_flying_definition
{
    float bank_angle;
    float bank_apply_time;
    float bank_decay_time;
    float pitch_ratio;
    float maximum_velocity;
    float maximum_sidestep_velocity;
    float acceleration;
    float deceleration;
    float angular_velocity_maximum;
    float angular_acceleration_maximum;
    float crouch_velocity_modifier;
    long flags;
};

struct s_character_physics_definition
{
    enum e_flag { _use_spheres = 0, _no_rigid_body = 5, _force_group = 6, k_flag_count = 8 };
    enum e_shape_type { _shape_standing, _shape_crouching };

    unsigned long flags;
    float height_standing;
    float height_crouching;
    float radius;
    float mass;
    long living_material_name;
    long dead_material_name;
    short living_global_material_index;
    short dead_global_material_index;
    s_tag_block dead_spheres;
    s_tag_block pills;
    s_tag_block spheres;
    s_character_physics_ground_definition ground;
    s_character_physics_flying_definition flying;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char character_physics_ground_definition_size[(sizeof(s_character_physics_ground_definition) == 0x40) ? 1 : -1];
typedef char character_physics_flying_definition_size[(sizeof(s_character_physics_flying_definition) == 0x30) ? 1 : -1];
typedef char character_physics_definition_size[(sizeof(s_character_physics_definition) == 0xb4) ? 1 : -1];
typedef char character_physics_radius_offset[(offsetof(s_character_physics_definition, radius) == 0xc) ? 1 : -1];
typedef char character_physics_ground_offset[(offsetof(s_character_physics_definition, ground) == 0x44) ? 1 : -1];
typedef char character_physics_downhill_scale_offset[(offsetof(s_character_physics_definition, ground.downhill_velocity_scale) == 0x58) ? 1 : -1];
typedef char character_physics_minimum_normal_offset[(offsetof(s_character_physics_definition, ground.minimum_normal_k) == 0x60) ? 1 : -1];
typedef char character_physics_climb_inflection_offset[(offsetof(s_character_physics_definition, ground.climb_inflection_angle) == 0x74) ? 1 : -1];
typedef char character_physics_flying_offset[(offsetof(s_character_physics_definition, flying) == 0x84) ? 1 : -1];
#endif

#endif // PHYSICS_CHARACTER_PHYSICS_DEFINITION_TYPES_H
