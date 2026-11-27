#ifndef OBJECTS_OBJECT_EARLY_MOVERS_TLS_H
#define OBJECTS_OBJECT_EARLY_MOVERS_TLS_H

#include "math/matrix_math_types.h"

struct s_mover
{
    real_vector3d velocity, angular_velocity;
    real_point3d center;
    real_point3d previous_center;
    real_point3d older_center;
    real_vector3d older_velocity, previous_velocity, previous_angular_velocity, angular_velocity2;
    real_matrix4x3 previous_transform, transform, delta;
    bool valid0, valid1, valid2;
    long bsp_count;
    long bsps[8];
};

struct s_owner_table
{
    s_mover movers[32];
    long owners[32];
    long owner_count;
    bool map_initialized;
};

extern "C" __declspec(thread) s_owner_table *g_object_early_movers_globals;

#endif
