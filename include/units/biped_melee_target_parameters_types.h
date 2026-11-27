#ifndef UNITS_BIPED_MELEE_TARGET_PARAMETERS_TYPES_H
#define UNITS_BIPED_MELEE_TARGET_PARAMETERS_TYPES_H

#include "math/real_math_types.h"

struct s_biped_melee_target_parameters {
    bool airborne;
    float distance;
    real_euler_angles2d ground_angles;
    real_euler_angles2d aiming_angles;
    real_euler_angles2d target_angles;
    real_point3d source_position;
    union { real_point3d target_position; long target_position_words[3]; };
    real_vector3d target_velocity;
    float target_radius;
};

#endif // UNITS_BIPED_MELEE_TARGET_PARAMETERS_TYPES_H
