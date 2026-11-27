#ifndef MATH_MATRIX_MATH_TYPES_H
#define MATH_MATRIX_MATH_TYPES_H

#include "math/real_math_types.h"

struct real_matrix3x3 {
    union {
        struct { real_vector3d forward, left, up; };
        // Point views for operations that transform the basis in place.
        struct { real_point3d forward_as_point, left_as_point, up_as_point; };
        real_vector3d basis[3];
        float m[3][3];
        float n[3][3];
    };
};

struct real_matrix4x3 {
    float scale;
    union {
        struct { real_vector3d forward, left, up; };
        real_matrix3x3 rotation;
        float matrix[3][3];
        float m[3][3];
    };
    real_point3d position;
};


typedef char matrix_math_size_real_matrix3x3[(sizeof(real_matrix3x3) == 36) ? 1 : -1];
typedef char matrix_math_size_real_matrix4x3[(sizeof(real_matrix4x3) == 52) ? 1 : -1];

#endif // MATH_MATRIX_MATH_TYPES_H
