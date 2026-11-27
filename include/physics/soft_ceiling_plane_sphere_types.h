#ifndef PHYSICS_SOFT_CEILING_PLANE_SPHERE_TYPES_H
#define PHYSICS_SOFT_CEILING_PLANE_SPHERE_TYPES_H

#include "math/real_math_types.h"

struct s_soft_ceiling_plane_sphere {
    float normal_i, normal_j, normal_k;
    float plane_d;
    real_point3d center;
    float radius;
};

#endif // PHYSICS_SOFT_CEILING_PLANE_SPHERE_TYPES_H
