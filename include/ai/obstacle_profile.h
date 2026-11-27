#ifndef AI_OBSTACLE_PROFILE_H
#define AI_OBSTACLE_PROFILE_H

#include "math/real_math.h"
struct special_movement;
struct biped_datum;

struct s_obstacle_profile
{
    real_point3d position;
    float radius;
    special_movement *movement;
    long flags;
    biped_datum *biped;
    short ignore_size;
    short step_size;
    short minimum_push_size;
    short maximum_push_size;
    bool vault_allowed;
    bool mount_allowed;
    real_vector3d forward;
    real_vector3d direction;
    bool in_vehicle;
    long ignore_object_index;
    long ignore_object_index2;
    float lower;
    float upper;
    __forceinline float forward_projection(const real_vector3d *vector) const
    {
        return dot_product3d(vector, &forward);
    }
};

typedef char obstacle_profile_size[(sizeof(void *) != 4 || ((sizeof(s_obstacle_profile) == 0x54))) ? 1 : -1];
#endif
