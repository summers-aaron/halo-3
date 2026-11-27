#ifndef MATH_RANDOM_MATH_RANGE_H
#define MATH_RANDOM_MATH_RANGE_H

#include "math/random_math.h"
#include "math/real_math_types.h"

extern "C" const float k_random_word_normalization;

static __forceinline unsigned short _random(unsigned long *seed, const char *file, const char *function, unsigned long line)
{
    return (unsigned short)(random_seed_next(seed) >> 16);
}

// match: water's range operation loads the external normalization constant.
static __forceinline float real_random_range_from_seed(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    return lower + (upper - lower) * ((float)(*seed >> 16) * k_random_word_normalization);
}

static __forceinline short _random_range(unsigned long *seed, const char *file, const char *function, unsigned long line, short lower, short upper)
{
    random_seed_next(seed);
    return lower + (short)(((upper - lower) * (*seed >> 16)) >> 16);
}

static __forceinline real_vector3d *_random_direction3d(unsigned long *seed, const char *file, const char *function, unsigned long line,
    real_vector3d *direction)
{
    *direction = global_random_directions[_random_range(seed, file, function, line, 0, 1026)];
    return direction;
}

// Random fraction in [0, 1] from the seed's high half.
static __forceinline float _real_random(unsigned long *seed, const char *file, const char *function, unsigned long line)
{
    random_seed_next(seed);
    return (1.0f / 65535.0f) * (float)(*seed >> 16);
}

static __forceinline float _real_random_range(unsigned long *seed, const char *file, const char *function, unsigned long line, float lower, float upper)
{
    random_seed_next(seed);
    return lower + (upper - lower) * ((1.0f / 65535.0f) * (float)(*seed >> 16));
}

static __forceinline float _real_random_range(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
    return lower + (upper - lower) * fraction;
}

// Random short between minimum and maximum, advanced from an explicit seed.
static __forceinline short random_range(unsigned long *seed, short minimum, short maximum)
{
    random_seed_next(seed);
    return (short)(minimum + ((unsigned long)((maximum - minimum) * (*seed >> 16)) >> 16));
}

#endif
