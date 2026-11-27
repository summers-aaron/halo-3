#ifndef MATH_REAL_MATH_TRIGONOMETRY_H
#define MATH_REAL_MATH_TRIGONOMETRY_H

#include <math.h>
#include "cseries/cseries.h"

inline float arccosine(float value) { return (float)acos(PIN(value, -1.0f, 1.0f)); }

static __forceinline float arctangent(float y, float x) { return (float)atan2((double)y, (double)x); }
static __forceinline float cosine(float angle) { return cosf(angle); }
static __forceinline float cosine_double(float angle) { return (float)cos((double)angle); }
static __forceinline float sine(float angle) { return sinf(angle); }

#endif // MATH_REAL_MATH_TRIGONOMETRY_H
