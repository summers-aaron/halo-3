#ifndef MATH_REAL_CONSTANT_POOL_H
#define MATH_REAL_CONSTANT_POOL_H

#include "math/real_math_constants.h"

struct real_matrix4x3;

// Six zero components.
extern "C" const float k_real_math_zero_storage[6];

// Identity matrix storage.
extern "C" {
extern const real_matrix4x3 k_real_math_identity_matrix_storage;
}

// Floating-point constants anchored at 3.0f.
extern "C" { extern const float k_real_constant_pool_three[]; }

extern "C" const float k_real_constant_pool_zero[];
extern "C" const float k_real_constant_pool_half[];

extern "C" const float literal_0_95f[];
extern "C" const float literal_0_75f[];

// Floating-point constants anchored at 0.1f.
extern "C" const float literal_0_1f[];

// Floating-point constants anchored at 2.0f.
extern "C" const float k_real_constant_pool_two[];

#endif
