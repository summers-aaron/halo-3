#ifndef MATH_REAL_CONSTANT_POOL_ACCESS_H
#define MATH_REAL_CONSTANT_POOL_ACCESS_H

#include "math/real_math_constants.h"

// Shared float constants for one and zero.
// 0.0f sits 200 words past the 1.0f anchor in the shared pool; both read through the same anchor so the pool base loads once.
#if !defined(_XBOX) || defined(__clang__)
static __forceinline float real_constant_pool_one() { return 1.0f; }
static __forceinline float real_constant_pool_zero() { return 0.0f; }
#else
static __forceinline float real_constant_pool_one() { return k_real_constant_pool_one[0]; }
static __forceinline float real_constant_pool_zero() { return k_real_constant_pool_one[200]; }
#endif

#endif
