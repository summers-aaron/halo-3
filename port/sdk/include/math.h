/*
 * Port override of the XDK's math.h (searched ahead of port/sdk/xbox-api/include).
 *
 * On 64-bit (HALO3_64BIT), inline fabs, fabsf, sqrt, and sqrtf directly to hardware SSE2
 * instructions (andps/andpd/sqrtss/sqrtsd) via Clang builtins so game TUs do not go through
 * the MS-ABI __ms_fabs/__ms_sqrt trampolines into libm.so.6.
 *
 * IEEE-754 parity:
 * - __builtin_fabs / __builtin_fabsf clear the sign bit (andpd/andps): fabs(-0.0) == +0.0, NaN preserved.
 * - __builtin_elementwise_sqrt lowers to sqrtsd/sqrtss: sqrt(-0.0) == -0.0, sqrt(NaN) == NaN, sqrt(-1.0) == NaN.
 *
 * On 32-bit (build-pc/lin) and Xbox 360, this header is a pure #include_next pass-through.
 */
#include_next <math.h>

#if defined(HALO3_64BIT) && !defined(PORT_SDK_MATH_H)
#define PORT_SDK_MATH_H

static __attribute__((always_inline)) inline double port_math_floor_impl(double _X)
{
    unsigned long long bits;
    __builtin_memcpy(&bits, &_X, 8);
    if ((bits & 0x7fffffffffffffffULL) >= 0x4330000000000000ULL)
        return _X;
    double fi = (double)(long long)_X;
    if (fi > _X)
        fi -= 1.0;
    if (fi == 0.0 && (bits >> 63))
        return -0.0;
    return fi;
}

static __attribute__((always_inline)) inline double port_math_ceil_impl(double _X)
{
    unsigned long long bits;
    __builtin_memcpy(&bits, &_X, 8);
    if ((bits & 0x7fffffffffffffffULL) >= 0x4330000000000000ULL)
        return _X;
    double fi = (double)(long long)_X;
    if (fi < _X)
        fi += 1.0;
    if (fi == 0.0 && (bits >> 63))
        return -0.0;
    return fi;
}

static __attribute__((always_inline)) inline float port_math_floorf(float _X)
{
    unsigned int bits;
    __builtin_memcpy(&bits, &_X, 4);
    if ((bits & 0x7fffffffu) >= 0x4b000000u)
        return _X;
    float fi = (float)(int)_X;
    if (fi > _X)
        fi -= 1.0f;
    if (fi == 0.0f && (bits >> 31))
        return -0.0f;
    return fi;
}

static __attribute__((always_inline)) inline float port_math_ceilf(float _X)
{
    unsigned int bits;
    __builtin_memcpy(&bits, &_X, 4);
    if ((bits & 0x7fffffffu) >= 0x4b000000u)
        return _X;
    float fi = (float)(int)_X;
    if (fi < _X)
        fi += 1.0f;
    if (fi == 0.0f && (bits >> 31))
        return -0.0f;
    return fi;
}

#ifdef __cplusplus
extern "C" {
__attribute__((always_inline)) inline double __cdecl fabs(double _X)
{
    return __builtin_fabs(_X);
}
__attribute__((always_inline)) inline double __cdecl sqrt(double _X)
{
    return __builtin_elementwise_sqrt(_X);
}
__attribute__((always_inline)) inline double __cdecl floor(double _X)
{
    return port_math_floor_impl(_X);
}
__attribute__((always_inline)) inline double __cdecl ceil(double _X)
{
    return port_math_ceil_impl(_X);
}
}

static __attribute__((always_inline)) inline float port_math_fabsf(float _X)
{
    return __builtin_fabsf(_X);
}
static __attribute__((always_inline)) inline float port_math_sqrtf(float _X)
{
    return __builtin_elementwise_sqrt(_X);
}
#define fabsf(x) port_math_fabsf((x))
#define sqrtf(x) port_math_sqrtf((x))
#define floorf(x) port_math_floorf((x))
#define ceilf(x) port_math_ceilf((x))
#else
static __attribute__((always_inline)) inline double port_math_fabs(double _X)
{
    return __builtin_fabs(_X);
}
static __attribute__((always_inline)) inline double port_math_sqrt(double _X)
{
    return __builtin_elementwise_sqrt(_X);
}
static __attribute__((always_inline)) inline float port_math_fabsf(float _X)
{
    return __builtin_fabsf(_X);
}
static __attribute__((always_inline)) inline float port_math_sqrtf(float _X)
{
    return __builtin_elementwise_sqrt(_X);
}
#undef fabsf
#undef sqrtf
#undef floorf
#undef ceilf
#define fabs(x) port_math_fabs((x))
#define sqrt(x) port_math_sqrt((x))
#define floor(x) port_math_floor_impl((x))
#define ceil(x) port_math_ceil_impl((x))
#define fabsf(x) port_math_fabsf((x))
#define sqrtf(x) port_math_sqrtf((x))
#define floorf(x) port_math_floorf((x))
#define ceilf(x) port_math_ceilf((x))
#endif

#endif /* HALO3_64BIT && !PORT_SDK_MATH_H */
