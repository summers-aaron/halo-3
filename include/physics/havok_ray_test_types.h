#ifndef PHYSICS_HAVOK_RAY_TEST_TYPES_H
#define PHYSICS_HAVOK_RAY_TEST_TYPES_H

#include "math/real_math_types.h"

struct s_havok_ray_test
{
    real_point3d start;
    real_vector3d vector;
    float fraction;
    real_vector3d normal;
    long flags;
    long object_index;
};

typedef char s_havok_ray_test_size[(sizeof(s_havok_ray_test) == 0x30) ? 1 : -1];

#endif // PHYSICS_HAVOK_RAY_TEST_TYPES_H
