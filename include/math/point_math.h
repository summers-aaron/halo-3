#ifndef MATH_POINT_MATH_H
#define MATH_POINT_MATH_H
#include "math/real_math_types.h"
static __forceinline real_point3d *scale_point3d(const real_point3d *point, float scale, real_point3d *result)
{
    result->x = scale * point->x;
    result->y = scale * point->y;
    result->z = scale * point->z;
    return result;
}
static __forceinline real_point3d *add_points3d(const real_point3d *a, const real_point3d *b, real_point3d *result)
{
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
    return result;
}
#endif
