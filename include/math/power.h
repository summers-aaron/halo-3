#ifndef MATH_POWER_H
#define MATH_POWER_H

#include <math.h>

static __forceinline float power(float base, float exponent)
{
    return (float)pow(base, exponent);
}

#endif // MATH_POWER_H
