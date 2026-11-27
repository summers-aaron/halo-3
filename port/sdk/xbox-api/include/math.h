// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// math.h: ISO C mathematics. The Xbox runtime implements only the double functions; the float (f) and long double (l)
// forms are the double function on the converted argument, rounded back: macros in C, C-linkage inline functions in
// C++, followed by the C++ float / long double overloads of the standard names.
#pragma once
#ifndef _INC_MATH
#define _INC_MATH
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _EXCEPTION_DEFINED
#define _EXCEPTION_DEFINED
struct _exception
{
    int type;
    char *name;
    double arg1;
    double arg2;
    double retval;
};
#endif
#ifndef _COMPLEX_DEFINED
#define _COMPLEX_DEFINED
struct _complex
{
    double x, y;
};
#endif

#define _DOMAIN 1
#define _SING 2
#define _OVERFLOW 3
#define _UNDERFLOW 4
#define _TLOSS 5
#define _PLOSS 6
#define EDOM 33
#define ERANGE 34
_CRTIMP extern double _HUGE;
#define HUGE_VAL _HUGE

#ifndef _CRT_ABS_DEFINED
#define _CRT_ABS_DEFINED
int __cdecl abs(int _X);
long __cdecl labs(long _X);
#endif
double __cdecl acos(double _X);
double __cdecl asin(double _X);
double __cdecl atan(double _X);
double __cdecl atan2(double _Y, double _X);
#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
_CRTIMP double __cdecl _copysign(double _Number, double _Sign);
_CRTIMP double __cdecl _chgsign(double _X);
#endif
double __cdecl cos(double _X);
double __cdecl cosh(double _X);
double __cdecl exp(double _X);
double __cdecl fabs(double _X);
double __cdecl fmod(double _X, double _Y);
double __cdecl log(double _X);
double __cdecl log10(double _X);
double __cdecl pow(double _X, double _Y);
double __cdecl sin(double _X);
double __cdecl sinh(double _X);
double __cdecl tan(double _X);
double __cdecl tanh(double _X);
double __cdecl sqrt(double _X);
#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
_CRTIMP double __cdecl atof(const char *_String);
_CRTIMP double __cdecl _atof_l(const char *_String, _locale_t _Locale);
#endif
_CRTIMP double __cdecl _cabs(struct _complex _Complex_value);
_CRTIMP double __cdecl ceil(double _X);
_CRTIMP double __cdecl floor(double _X);
_CRTIMP double __cdecl frexp(double _X, int *_Y);
_CRTIMP double __cdecl _hypot(double _X, double _Y);
_CRTIMP double __cdecl _j0(double _X);
_CRTIMP double __cdecl _j1(double _X);
_CRTIMP double __cdecl _jn(int _X, double _Y);
_CRTIMP double __cdecl ldexp(double _X, int _Y);
#ifndef _CRT_MATHERR_DEFINED
#define _CRT_MATHERR_DEFINED
int __cdecl _matherr(struct _exception *_Except);
#endif
_CRTIMP double __cdecl modf(double _X, double *_Y);
_CRTIMP double __cdecl _y0(double _X);
_CRTIMP double __cdecl _y1(double _X);
_CRTIMP double __cdecl _yn(int _X, double _Y);

#ifndef __cplusplus
#define acosl(x) ((long double)acos((double)(x)))
#define asinl(x) ((long double)asin((double)(x)))
#define atanl(x) ((long double)atan((double)(x)))
#define atan2l(x, y) ((long double)atan2((double)(x), (double)(y)))
#define ceill(x) ((long double)ceil((double)(x)))
#define cosl(x) ((long double)cos((double)(x)))
#define coshl(x) ((long double)cosh((double)(x)))
#define expl(x) ((long double)exp((double)(x)))
#define fabsl(x) ((long double)fabs((double)(x)))
#define floorl(x) ((long double)floor((double)(x)))
#define fmodl(x, y) ((long double)fmod((double)(x), (double)(y)))
#define frexpl(x, y) ((long double)frexp((double)(x), (y)))
#define _hypotl(x, y) ((long double)_hypot((double)(x), (double)(y)))
#define ldexpl(x, y) ((long double)ldexp((double)(x), (y)))
#define logl(x) ((long double)log((double)(x)))
#define log10l(x) ((long double)log10((double)(x)))
#define _matherrl _matherr
#define modfl(x, y) ((long double)modf((double)(x), (double *)(y)))
#define powl(x, y) ((long double)pow((double)(x), (double)(y)))
#define sinl(x) ((long double)sin((double)(x)))
#define sinhl(x) ((long double)sinh((double)(x)))
#define sqrtl(x) ((long double)sqrt((double)(x)))
#define tanl(x) ((long double)tan((double)(x)))
#define tanhl(x) ((long double)tanh((double)(x)))
#define _chgsignl(x) ((long double)_chgsign((double)(x)))
#define _copysignl(x, y) ((long double)_copysign((double)(x), (double)(y)))
#define frexpf(x, y) ((float)frexp((double)(x), (y)))
#define fabsf(x) ((float)fabs((double)(x)))
#define ldexpf(x, y) ((float)ldexp((double)(x), (y)))
#define acosf(x) ((float)acos((double)(x)))
#define asinf(x) ((float)asin((double)(x)))
#define atanf(x) ((float)atan((double)(x)))
#define atan2f(x, y) ((float)atan2((double)(x), (double)(y)))
#define ceilf(x) ((float)ceil((double)(x)))
#define cosf(x) ((float)cos((double)(x)))
#define coshf(x) ((float)cosh((double)(x)))
#define expf(x) ((float)exp((double)(x)))
#define floorf(x) ((float)floor((double)(x)))
#define fmodf(x, y) ((float)fmod((double)(x), (double)(y)))
#define logf(x) ((float)log((double)(x)))
#define log10f(x) ((float)log10((double)(x)))
#define modff(x, y) ((float)modf((double)(x), (double *)(y)))
#define powf(x, y) ((float)pow((double)(x), (double)(y)))
#define sinf(x) ((float)sin((double)(x)))
#define sinhf(x) ((float)sinh((double)(x)))
#define sqrtf(x) ((float)sqrt((double)(x)))
#define tanf(x) ((float)tan((double)(x)))
#define tanhf(x) ((float)tanh((double)(x)))
#else
// One-argument and two-argument forwarders: T name(T x) { return (T)base((double)x); }
#define _PORT_MATH_FWD1(T, name, base) \
    inline T name(T _X) { return (T)base((double)_X); }
#define _PORT_MATH_FWD2(T, name, base) \
    inline T name(T _X, T _Y) { return (T)base((double)_X, (double)_Y); }
_PORT_MATH_FWD1(long double, acosl, acos)
_PORT_MATH_FWD1(long double, asinl, asin)
_PORT_MATH_FWD1(long double, atanl, atan)
_PORT_MATH_FWD2(long double, atan2l, atan2)
_PORT_MATH_FWD1(long double, ceill, ceil)
_PORT_MATH_FWD1(long double, cosl, cos)
_PORT_MATH_FWD1(long double, coshl, cosh)
_PORT_MATH_FWD1(long double, expl, exp)
_PORT_MATH_FWD1(long double, fabsl, fabs)
_PORT_MATH_FWD1(long double, floorl, floor)
_PORT_MATH_FWD2(long double, fmodl, fmod)
inline long double frexpl(long double _X, int *_Y) { return frexp((double)_X, _Y); }
inline long double ldexpl(long double _X, int _Y) { return ldexp((double)_X, _Y); }
_PORT_MATH_FWD1(long double, logl, log)
_PORT_MATH_FWD1(long double, log10l, log10)
// The long double modf returns the fraction only; the integral part is not stored (as the runtime's own form).
inline long double modfl(long double _X, long double *_Y)
{
    double _Di, _Df = modf((double)_X, &_Di);
    return _Df;
}
_PORT_MATH_FWD2(long double, powl, pow)
_PORT_MATH_FWD1(long double, sinl, sin)
_PORT_MATH_FWD1(long double, sinhl, sinh)
_PORT_MATH_FWD1(long double, sqrtl, sqrt)
_PORT_MATH_FWD1(long double, tanl, tan)
_PORT_MATH_FWD1(long double, tanhl, tanh)
inline long double _chgsignl(long double _Number) { return _chgsign(static_cast<double>(_Number)); }
inline long double _copysignl(long double _Number, long double _Sign)
{
    return _copysign(static_cast<double>(_Number), static_cast<double>(_Sign));
}
inline float frexpf(float _X, int *_Y) { return (float)frexp((double)_X, _Y); }
_PORT_MATH_FWD1(float, fabsf, fabs)
inline float ldexpf(float _X, int _Y) { return (float)ldexp((double)_X, _Y); }
_PORT_MATH_FWD1(float, acosf, acos)
_PORT_MATH_FWD1(float, asinf, asin)
_PORT_MATH_FWD1(float, atanf, atan)
_PORT_MATH_FWD2(float, atan2f, atan2)
_PORT_MATH_FWD1(float, ceilf, ceil)
_PORT_MATH_FWD1(float, cosf, cos)
_PORT_MATH_FWD1(float, coshf, cosh)
_PORT_MATH_FWD1(float, expf, exp)
_PORT_MATH_FWD1(float, floorf, floor)
_PORT_MATH_FWD2(float, fmodf, fmod)
_PORT_MATH_FWD1(float, logf, log)
_PORT_MATH_FWD1(float, log10f, log10)
inline float modff(float _X, float *_Y)
{
    double _Di, _Df = modf((double)_X, &_Di);
    return (float)_Df;
}
_PORT_MATH_FWD2(float, powf, pow)
_PORT_MATH_FWD1(float, sinf, sin)
_PORT_MATH_FWD1(float, sinhf, sinh)
_PORT_MATH_FWD1(float, sqrtf, sqrt)
_PORT_MATH_FWD1(float, tanf, tan)
_PORT_MATH_FWD1(float, tanhf, tanh)
#undef _PORT_MATH_FWD1
#undef _PORT_MATH_FWD2
#endif

#if !__STDC__
#define DOMAIN _DOMAIN
#define SING _SING
#define OVERFLOW _OVERFLOW
#define UNDERFLOW _UNDERFLOW
#define TLOSS _TLOSS
#define PLOSS _PLOSS
#define matherr _matherr
_CRTIMP extern double HUGE;
_CRTIMP double __cdecl cabs(struct _complex _X);
_CRTIMP double __cdecl hypot(double _X, double _Y);
_CRTIMP double __cdecl j0(double _X);
_CRTIMP double __cdecl j1(double _X);
_CRTIMP double __cdecl jn(int _X, double _Y);
_CRTIMP double __cdecl y0(double _X);
_CRTIMP double __cdecl y1(double _X);
_CRTIMP double __cdecl yn(int _X, double _Y);
#endif

#ifdef __cplusplus
}

extern "C++" {
// x^n by binary powering (square-and-multiply over the bits of |n|; 1/x^|n| for negative n).
template <class _Ty> inline _Ty _Pow_int(_Ty _X, int _Y)
{
    unsigned int _N = _Y >= 0 ? (unsigned int)_Y : (unsigned int)(-_Y);
    for (_Ty _Z = _Ty(1);; _X *= _X)
    {
        if ((_N & 1) != 0)
            _Z *= _X;
        if ((_N >>= 1) == 0)
            return _Y < 0 ? _Ty(1) / _Z : _Z;
    }
}
extern "C" __int64 __cdecl _abs64(__int64);
inline long __CRTDECL abs(long _X) { return labs(_X); }
inline __int64 __CRTDECL abs(__int64 _X) { return _abs64(_X); }
inline double __CRTDECL abs(double _X) { return fabs(_X); }
inline double __CRTDECL pow(double _X, int _Y) { return _Pow_int(_X, _Y); }

// The float and long double overloads of the standard names, each forwarding to its suffixed function.
#define _PORT_MATH_OVL1(T, name, impl) \
    inline T __CRTDECL name(T _X) { return impl(_X); }
#define _PORT_MATH_OVL2(T, name, impl) \
    inline T __CRTDECL name(T _X, T _Y) { return impl(_X, _Y); }
#define _PORT_MATH_OVERLOADS(T, s) \
    _PORT_MATH_OVL1(T, abs, fabs##s) \
    _PORT_MATH_OVL1(T, acos, acos##s) \
    _PORT_MATH_OVL1(T, asin, asin##s) \
    _PORT_MATH_OVL1(T, atan, atan##s) \
    _PORT_MATH_OVL2(T, atan2, atan2##s) \
    _PORT_MATH_OVL1(T, ceil, ceil##s) \
    _PORT_MATH_OVL1(T, cos, cos##s) \
    _PORT_MATH_OVL1(T, cosh, cosh##s) \
    _PORT_MATH_OVL1(T, exp, exp##s) \
    _PORT_MATH_OVL1(T, fabs, fabs##s) \
    _PORT_MATH_OVL1(T, floor, floor##s) \
    _PORT_MATH_OVL2(T, fmod, fmod##s) \
    inline T __CRTDECL frexp(T _X, int *_Y) { return frexp##s(_X, _Y); } \
    inline T __CRTDECL ldexp(T _X, int _Y) { return ldexp##s(_X, _Y); } \
    _PORT_MATH_OVL1(T, log, log##s) \
    _PORT_MATH_OVL1(T, log10, log10##s) \
    inline T __CRTDECL modf(T _X, T *_Y) { return modf##s(_X, _Y); } \
    _PORT_MATH_OVL2(T, pow, pow##s) \
    inline T __CRTDECL pow(T _X, int _Y) { return _Pow_int(_X, _Y); } \
    _PORT_MATH_OVL1(T, sin, sin##s) \
    _PORT_MATH_OVL1(T, sinh, sinh##s) \
    _PORT_MATH_OVL1(T, sqrt, sqrt##s) \
    _PORT_MATH_OVL1(T, tan, tan##s) \
    _PORT_MATH_OVL1(T, tanh, tanh##s)
_PORT_MATH_OVERLOADS(float, f)
_PORT_MATH_OVERLOADS(long double, l)
#undef _PORT_MATH_OVERLOADS
#undef _PORT_MATH_OVL1
#undef _PORT_MATH_OVL2
}
#endif

#pragma pack(pop)
#endif // _INC_MATH

#if defined(_USE_MATH_DEFINES) && !defined(_MATH_DEFINES_DEFINED)
#define _MATH_DEFINES_DEFINED
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT1_2 0.707106781186547524401
#endif
