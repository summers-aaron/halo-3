#ifndef MATH_PROJECTION_MATH_H
#define MATH_PROJECTION_MATH_H

#include "math/real_math.h"

static __forceinline real_point3d *project_point3d(const real_point3d *point, short projection, bool sign, real_point3d *result)
{
    set_real_point3d(result,
        point->coordinates[global_projection3d_mappings[projection][sign][0]],
        point->coordinates[global_projection3d_mappings[projection][sign][1]],
        point->coordinates[global_projection3d_mappings[projection][sign][2]]);
    return result;
}

static __forceinline short projection_from_vector3d(const real_vector3d *vector)
{
    float abs_i = fabsf(vector->i);
    float abs_j = fabsf(vector->j);
    float abs_k = fabsf(vector->k);
    if (abs_k >= abs_j && abs_k >= abs_i)
        return 2;
    else if (abs_j >= abs_i)
        return 1;
    else
        return 0;
}

static __forceinline bool projection_sign_from_vector3d(const real_vector3d *vector, short projection)
{
    return vector->n[projection] > 0.0f;
}

static __forceinline real_point3d *inverse_project_point3d(const real_point3d *point, short projection, bool sign, real_point3d *result)
{
    set_real_point3d(result,
        point->n[global_projection3d_inverse_mappings[projection][sign][0]],
        point->n[global_projection3d_inverse_mappings[projection][sign][1]],
        point->n[global_projection3d_inverse_mappings[projection][sign][2]]);
    return result;
}

static __forceinline real_point2d *project_point3d(const real_point3d *point, short projection, bool sign, real_point2d *out)
{
    return set_real_point2d(out,
        point->n[global_projection3d_mappings[projection][sign][0]],
        point->n[global_projection3d_mappings[projection][sign][1]]);
}

static __forceinline real_vector2d *project_vector3d(const real_vector3d *vector, short projection, bool sign, real_vector2d *result)
{
    return (real_vector2d *)project_point3d((const real_point3d *)vector, projection, sign, (real_point2d *)result);
}

#endif
