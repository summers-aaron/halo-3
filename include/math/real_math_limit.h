#ifndef MATH_REAL_MATH_LIMIT_H
#define MATH_REAL_MATH_LIMIT_H

#include "math/real_math_types.h"

// match: 0xe7a38: limit3d writes only through vector; without noalias update_internal spills across the call (frame 0x120->0x130, limited 104->152).
__declspec(noalias) bool limit3d(real_vector3d *vector, float maximum);

#endif
