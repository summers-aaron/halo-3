#ifndef PHYSICS_COLLISION_SPHERE_QUERIES_H
#define PHYSICS_COLLISION_SPHERE_QUERIES_H

#include "math/real_math_types.h"
struct s_collision_test_flags;

extern "C" bool collision_test_sphere(s_collision_test_flags flags, const real_point3d *point, float radius, long first_ignore, long second_ignore);

#endif
