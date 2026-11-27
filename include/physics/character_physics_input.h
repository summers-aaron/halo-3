#ifndef PHYSICS_CHARACTER_PHYSICS_INPUT_H
#define PHYSICS_CHARACTER_PHYSICS_INPUT_H

#include <stddef.h>

#include "animations/animation_id.h"
#include "physics/havok_group_types.h"
#include "math/matrix_math_types.h"

struct s_character_physics_definition;
class c_animation_manager;
class phys_shape;

struct s_character_physics_update_input_datum {
    long mode;
    bool sub_initialized;
    const s_character_physics_definition *definition;
    unsigned long flags;
    long local_physics;
    long accepted_physics;
    long havok_component_index;
    const c_animation_manager *animation_manager;
    union {
        float scale;
        float speed;
    };
    e_havok_group havok_group;
    float flying_friction;
    real_point3d previous_position;
    real_point3d position;
    real_vector3d velocity;
    // The sentinel animation starts cleared to NONE/NONE.
    c_animation_id animation;
    long support_component;
    long support_body;
    real_matrix4x3 support_matrix;
    real_vector3d ground_plane;
    float ground_friction;
    real_vector3d desired_velocity;
    float ground_adhesion;
    bool ground_contact;
    real_vector3d desired_up;
    bool dead_size_increase;
    union {
        bool dead_size_reset;
        bool force_facing;
    };
    union {
        bool melee_movement;
        struct {
            bool enabled;
            union { float time_step; float radius; };
            union { float duration; float target_radius; };
            union { real_point3d position; real_point3d target_position; };
            union { real_vector3d velocity; real_vector3d target_velocity; };
        } melee;
    };
    float movement_magnitude;
    real_vector3d movement;
    union {
        real_point3d current_position;
        real_point3d origin;
        real_point3d target;
    };
    real_vector3d forward;
    real_vector3d up;
    real_vector3d facing;
    real_vector3d aiming;
    union {
        real_vector3d external_velocity;
        real_vector3d target_velocity;
    };
    union {
        real_vector3d early_velocity;
        real_vector3d reference_velocity;
    };
    float movement_scale;
    float acceleration_scale;
    union {
        struct {
            real_vector3d acceleration;
            float maximum_velocity;
        };
        struct {
            real_vector3d throttle;
            float acceleration;
        } flying;
    };
    float maximum_slope;
    const phys_shape *shape;
};

typedef char char_phys_input_size[
    (sizeof(void *) != 4 || ((sizeof(s_character_physics_update_input_datum) == 0x16c) )) ? 1 : -1];
typedef char char_phys_input_definition_offset[
    (sizeof(void *) != 4 || offsetof(s_character_physics_update_input_datum, definition) == 0x8) ? 1 : -1];
typedef char char_phys_input_desired_up_offset[
    (sizeof(void *) != 4 || offsetof(s_character_physics_update_input_datum, desired_up) == 0xb4) ? 1 : -1];
typedef char char_phys_input_melee_offset[
    (sizeof(void *) != 4 || offsetof(s_character_physics_update_input_datum, melee) == 0xc4) ? 1 : -1];
typedef char char_phys_input_movement_magnitude_offset[
    (sizeof(void *) != 4 || offsetof(s_character_physics_update_input_datum, movement_magnitude) == 0xe8) ? 1 : -1];
typedef char char_phys_input_melee_time_offset[
    (sizeof(void *) != 4 || ((offsetof(s_character_physics_update_input_datum, melee.time_step) == 0xc8) )) ? 1 : -1];
typedef char char_phys_input_melee_position_offset[
    (sizeof(void *) != 4 || ((offsetof(s_character_physics_update_input_datum, melee.position) == 0xd0) )) ? 1 : -1];
typedef char char_phys_input_melee_velocity_offset[
    (sizeof(void *) != 4 || ((offsetof(s_character_physics_update_input_datum, melee.velocity) == 0xdc) )) ? 1 : -1];
typedef char char_phys_input_flying_throttle_offset[
    (sizeof(void *) != 4 || ((offsetof(s_character_physics_update_input_datum, flying.throttle) == 0x154) )) ? 1 : -1];
typedef char char_phys_input_flying_acceleration_offset[
    (sizeof(void *) != 4 || ((offsetof(s_character_physics_update_input_datum, flying.acceleration) == 0x160) )) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_INPUT_H
