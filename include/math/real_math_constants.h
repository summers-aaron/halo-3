#ifndef MATH_REAL_MATH_CONSTANTS_H
#define MATH_REAL_MATH_CONSTANTS_H

#include "math/real_math_types.h"

// Shared three-fortieths constant.
extern "C" const float k_real_three_fortieths_storage;

// Direct storage underlying global_origin3d, owned by real_math.obj.
extern "C" const real_point3d k_real_math_origin3d;

// Zero vector sharing the pooled storage of k_real_math_origin3d.
extern "C" const real_vector3d k_real_math_zero_vector3d_storage;

// Shared one-third approximation used by animation, flocks and terminals.
extern "C" float k_real_third;

// Shared epsilon for floating-point comparisons.
extern "C" const float k_real_math_epsilon_storage;

extern "C" const float k_real_zero_storage;
extern "C" const float k_real_constant_pool[];
// Floating-point constants anchored at 1.0f.
extern "C" const float k_real_constant_pool_one[];
extern "C" float k_pi_global;
// Shared constant for 0.7f.
extern "C" const float k_real_seven_tenths_storage;
// Shared constants for one thousandth and two fifths.
extern "C" const float k_real_one_thousandth_storage;
extern "C" const float k_real_two_fifths_storage[];
#endif
