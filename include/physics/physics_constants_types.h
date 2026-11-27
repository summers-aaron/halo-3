#ifndef PHYSICS_PHYSICS_CONSTANTS_TYPES_H
#define PHYSICS_PHYSICS_CONSTANTS_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_physics_constants
{
    float gravity;
    float water_density;
    float air_density;
    real_vector3d velocity_frame;
    long character_ground_adhesion_forces_disable_time;
    float slip_surface_maximum_k;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char physics_constants_size[(sizeof(s_physics_constants) == 0x20) ? 1 : -1];
typedef char physics_constants_velocity_offset[(offsetof(s_physics_constants, velocity_frame) == 0xc) ? 1 : -1];
typedef char physics_constants_adhesion_time_offset[(offsetof(s_physics_constants, character_ground_adhesion_forces_disable_time) == 0x18) ? 1 : -1];
typedef char physics_constants_slip_surface_offset[(offsetof(s_physics_constants, slip_surface_maximum_k) == 0x1c) ? 1 : -1];
#endif

#endif // PHYSICS_PHYSICS_CONSTANTS_TYPES_H
