#ifndef MATH_REAL_MATH_NORMALIZE_INLINE_H
#define MATH_REAL_MATH_NORMALIZE_INLINE_H
#include "math/real_math.h"
#include <math.h>

inline float normalize3d(real_vector3d *vector)
{
    float length = magnitude3d(vector);
    if (!(fabsf(length - 0.0f) < k_real_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    else
        length = 0.0f;
    return length;
}

#endif
