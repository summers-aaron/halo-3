#ifndef MATH_RANDOM_MATH_H
#define MATH_RANDOM_MATH_H

// Functions defined in tu/blamlibxbox/random_math.cpp.

union real_vector3d;

extern const real_vector3d *const global_random_directions;

extern "C" { extern const real_vector3d k_random_direction_table[]; }

void random_math_initialize_internal();
unsigned long generate_random_seed(void);
unsigned long *get_local_random_seed_address();
union real_vector3d * _random_vector_in_cone3d(unsigned long * seed, char const * file, char const * function, unsigned long line,
    union real_vector3d const * direction, float minimum, float maximum, union real_vector3d * result);
real_vector3d *_distriubuite_vector_in_cone3d(unsigned long *seed, const char *file, const char *function, unsigned long line,
    const real_vector3d *direction, long radial_index, long radial_count, long angular_index,
    long angular_count, float minimum, float maximum, float exponent, real_vector3d *result);
extern "C" void random_math_register_cleanup(void);

static __forceinline unsigned long random_seed_next(unsigned long *seed)
{
    *seed = *seed * 1664525 + 1013904223;
    return *seed;
}

// The seed's high halfword. The console reads it through an unsigned short pointer at +0 (big-endian); on a
// little-endian host that pointer reads the low halfword, whose low bits cycle with period 2, 4, 8, ...
#if defined(_XBOX) && !defined(__clang__)
#define random_seed_high_half(seed) (*(unsigned short *)(seed))
#else
static inline unsigned short random_seed_high_half(const unsigned long *seed)
{
    return (unsigned short)(*seed >> 16);
}
#endif

static __forceinline float _real_random(unsigned long *seed)
{
    random_seed_next(seed);
    return (float)random_seed_high_half(seed) * (1.0f / 65535.0f);
}

static __forceinline short random_range(short lower, short upper) {
    random_seed_next(get_local_random_seed_address());
    unsigned short random = random_seed_high_half(get_local_random_seed_address());
    short result = (short)(((unsigned int)(upper - lower) * random) >> 16);
    return lower + result;
}

void random_math_initialize(void);
void random_math_dispose(void);

#endif // MATH_RANDOM_MATH_H
