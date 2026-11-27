#ifndef EFFECTS_PARTICLE_RANDOM_H
#define EFFECTS_PARTICLE_RANDOM_H

#include "math/random_math_range.h"

static __forceinline float particle_random_range(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    return lower + (upper - lower) * ((float)random_seed_high_half(seed) * k_random_word_normalization);
}

#endif // EFFECTS_PARTICLE_RANDOM_H
