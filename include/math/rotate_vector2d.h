#ifndef MATH_ROTATE_VECTOR2D_H
#define MATH_ROTATE_VECTOR2D_H

#include "math/real_math_types.h"

static __forceinline real_vector2d *rotate_vector2d(const real_vector2d *vector, float sine, float cosine, real_vector2d *result)
{
    float j = sine * vector->i + cosine * vector->j;
    result->i = cosine * vector->i - sine * vector->j;
    result->j = j;
    return result;
}

#endif
