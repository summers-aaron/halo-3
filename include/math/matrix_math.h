#ifndef MATH_MATRIX_MATH_H
#define MATH_MATRIX_MATH_H

#include "math/real_math.h"

#include "math/matrix_math_functions.h"

static __forceinline void matrix4x3_rotation_from_quaternion(real_matrix4x3 *matrix, const real_quaternion *quaternion)
{
    matrix3x3_rotation_from_quaternion(&matrix->rotation, quaternion);
    set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
    matrix->scale = 1.0f;
}

static __forceinline real_vector3d *matrix4x3_inverse_transform_normal(const real_matrix4x3 *matrix, const real_vector3d *normal, real_vector3d *result)
{
    float i = normal->i;
    float j = normal->j;
    float k = normal->k;
    result->i = i * matrix->forward.i + j * matrix->forward.j + k * matrix->forward.k;
    result->j = i * matrix->left.i + j * matrix->left.j + k * matrix->left.k;
    result->k = i * matrix->up.i + j * matrix->up.j + k * matrix->up.k;
    return result;
}

#endif // MATH_MATRIX_MATH_H
