#ifndef AI_SWARM_SPAWNER_TYPES_H
#define AI_SWARM_SPAWNER_TYPES_H

#include <stddef.h>
#include "math/matrix_math_types.h"

struct s_swarm_spawner {
    long swarm_index;
    short creature_count;
    short spawn_ticks;
    short flags;
    short pad0a;
    long definition_index;
    long starting_location_index;
    long marker_name;
    union {
        real_matrix4x3 matrix;
        struct { float matrix_scale; real_vector3d facing, left, up; real_point3d position; };
    };
    short node_index;
    short pad4e;
    union {
        struct {
            float yaw_minimum, yaw_maximum, pitch_minimum, pitch_maximum;
            float random_velocity_minimum, random_velocity_maximum;
            float form_lifetime_minimum, form_lifetime_maximum;
        };
        float spawn_parameters[8];
    };
    float minimum_spawn_delay;
    float maximum_spawn_delay;
};

typedef char swarm_spawner_size_check[(sizeof(s_swarm_spawner) == 120) ? 1 : -1];

struct s_swarm_spawner_reference {
    long definition_index;
    long starting_location_index;
    long marker_name;
};


typedef char swarm_spawner_matrix_offset[(offsetof(s_swarm_spawner, matrix) == 0x18) ? 1 : -1];
typedef char swarm_spawner_parameters_offset[(offsetof(s_swarm_spawner, spawn_parameters) == 0x50) ? 1 : -1];

#endif
