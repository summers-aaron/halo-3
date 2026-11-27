#ifndef PHYSICS_CHARACTER_PHYSICS_OUTPUT_H
#define PHYSICS_CHARACTER_PHYSICS_OUTPUT_H

#include <stddef.h>

#include "math/matrix_math_types.h"

// What one character physics update reports back to its motor program (0x70 bytes).
struct s_character_physics_update_output_datum {
    enum e_animation_state {
        _animation_state_none = 0,
        // the climbing (sentinel) mode sets these from the vertical movement input
        _animation_state_climb_up = 10,
        _animation_state_climb_down = 11
    };

    unsigned long flags;
    e_animation_state animation_state;
    union {
        bool animation_state_valid;
        bool moving;
        bool value;
    };
    real_vector3d velocity;
    union {
        real_vector3d facing;
        real_vector3d forward;
    };
    real_vector3d up;
    real_matrix3x3 localized_physics_space_rotation; // (ground update copies its support rotation)
    bool origin_valid;
    real_point3d origin;
    real_vector3d previous_external_velocity;
};

typedef char char_phys_output_size[(sizeof(s_character_physics_update_output_datum) == 0x70) ? 1 : -1];
typedef char char_phys_output_off_rotation[(offsetof(s_character_physics_update_output_datum, localized_physics_space_rotation) == 0x30) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_OUTPUT_H
