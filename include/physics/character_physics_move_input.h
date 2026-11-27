#ifndef PHYSICS_CHARACTER_PHYSICS_MOVE_INPUT_H
#define PHYSICS_CHARACTER_PHYSICS_MOVE_INPUT_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_character_physics_definition;

// Component and dead move inputs share the same layout. Ground's
// contact search names the support component, body and enabled byte in the tail.
struct s_character_physics_move_input_datum
{
    bool flags_test(long bit) const { return (flags & (1UL << bit)) != 0; }
    const s_character_physics_definition *definition;
    long local_physics;
    long accepted_physics;
    long mode;
    bool sub_initialized;
    unsigned long flags;
    real_point3d position;
    real_vector3d velocity;
    real_vector3d forward;
    real_vector3d up;
    real_vector3d early_velocity;
    long object_index;
    long havok_component_index;
    long ground_havok_component_index;
    long ground_rigid_body_index;
    bool ground_enabled;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    bool dead_enabled;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    real_point3d sentinel_physics_position;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char character_physics_move_input_size[(sizeof(s_character_physics_move_input_datum) == 0x78) ? 1 : -1];
typedef char character_physics_move_input_component_offset[(offsetof(s_character_physics_move_input_datum, havok_component_index) == 0x58) ? 1 : -1];
typedef char character_physics_move_input_ground_component_offset[(offsetof(s_character_physics_move_input_datum,
    ground_havok_component_index) == 0x5c) ? 1 : -1];
typedef char character_physics_move_input_ground_body_offset[(offsetof(s_character_physics_move_input_datum, ground_rigid_body_index) == 0x60) ? 1 : -1];
typedef char character_physics_move_input_ground_enabled_offset[(offsetof(s_character_physics_move_input_datum, ground_enabled) == 0x64) ? 1 : -1];
typedef char character_physics_move_input_dead_enabled_offset[(offsetof(s_character_physics_move_input_datum, dead_enabled) == 0x68) ? 1 : -1];
typedef char character_physics_move_input_sentinel_position_offset[(offsetof(s_character_physics_move_input_datum,
    sentinel_physics_position) == 0x6c) ? 1 : -1];
#endif

#endif // PHYSICS_CHARACTER_PHYSICS_MOVE_INPUT_H
