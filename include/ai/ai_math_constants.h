#ifndef AI_AI_MATH_CONSTANTS_H
#define AI_AI_MATH_CONSTANTS_H

#include "math/real_constant_pool.h"

// Shared addressable base of the AI floating-point constant pool (0.5f anchor).
#define AI_REAL_CONSTANTS k_real_constant_pool_half

#define AI_SQUAD_HEALTH_THRESHOLD 0.25f

#if defined(_XBOX) && !defined(__clang__)
// The Xbox speaker search loads both zero and the search radius from this base.
extern "C" const float k_ai_dialogue_real_constants[];
#endif

#endif
