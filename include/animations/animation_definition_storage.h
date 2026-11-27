#ifndef ANIMATIONS_ANIMATION_DEFINITION_STORAGE_H
#define ANIMATIONS_ANIMATION_DEFINITION_STORAGE_H

#include "math/matrix_math_types.h"

// Backing constants owned by real_math.cpp and the local generator in random_math.cpp.
extern "C" {
extern const real_orientation k_real_math_identity_orientation_storage;
extern const real_matrix4x3 k_real_math_identity_matrix_storage;
extern const float k_real_zero_storage;
}

#endif
