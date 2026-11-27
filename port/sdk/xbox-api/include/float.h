// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// float.h: IEEE-754 single/double characteristics (long double is double) and the runtime's FPU-control interface.
#pragma once
#ifndef _INC_FLOAT
#define _INC_FLOAT
#include <crtdefs.h>

#define FLT_RADIX 2
#define FLT_ROUNDS 1
#define FLT_GUARD 0
#define FLT_NORMALIZE 0
#define FLT_DIG 6
#define FLT_MANT_DIG 24
#define FLT_EPSILON 1.192092896e-07F
#define FLT_MAX 3.402823466e+38F
#define FLT_MIN 1.175494351e-38F
#define FLT_MAX_EXP 128
#define FLT_MIN_EXP (-125)
#define FLT_MAX_10_EXP 38
#define FLT_MIN_10_EXP (-37)
#define _FLT_RADIX 2
#define DBL_DIG 15
#define DBL_MANT_DIG 53
#define DBL_EPSILON 2.2204460492503131e-016
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308
#define DBL_MAX_EXP 1024
#define DBL_MIN_EXP (-1021)
#define DBL_MAX_10_EXP 308
#define DBL_MIN_10_EXP (-307)
#define _DBL_RADIX 2
#define _DBL_ROUNDS 1
#define LDBL_DIG DBL_DIG
#define LDBL_MANT_DIG DBL_MANT_DIG
#define LDBL_EPSILON DBL_EPSILON
#define LDBL_MAX DBL_MAX
#define LDBL_MIN DBL_MIN
#define LDBL_MAX_EXP DBL_MAX_EXP
#define LDBL_MIN_EXP DBL_MIN_EXP
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP

// Status word bits (_clearfp / _statusfp)
#define _SW_INEXACT 0x00000001
#define _SW_UNDERFLOW 0x00000002
#define _SW_OVERFLOW 0x00000004
#define _SW_ZERODIVIDE 0x00000008
#define _SW_INVALID 0x00000010
#define _SW_DENORMAL 0x00080000
// Control word fields (_controlfp / _control87)
#define _MCW_EM 0x0008001f
#define _EM_INEXACT 0x00000001
#define _EM_UNDERFLOW 0x00000002
#define _EM_OVERFLOW 0x00000004
#define _EM_ZERODIVIDE 0x00000008
#define _EM_INVALID 0x00000010
#define _EM_DENORMAL 0x00080000
#define _EM_AMBIGUOUS 0x80000000
#define _EM_AMBIGUIOUS _EM_AMBIGUOUS
#define _MCW_RC 0x00000300
#define _RC_NEAR 0x00000000
#define _RC_DOWN 0x00000100
#define _RC_UP 0x00000200
#define _RC_CHOP 0x00000300
#define _MCW_PC 0x00030000
#define _PC_64 0x00000000
#define _PC_53 0x00010000
#define _PC_24 0x00020000
#define _MCW_IC 0x00040000
#define _IC_AFFINE 0x00040000
#define _IC_PROJECTIVE 0x00000000
#define _MCW_DN 0x03000000
#define _DN_SAVE 0x00000000
#define _DN_FLUSH 0x01000000
#define _DN_FLUSH_OPERANDS_SAVE_RESULTS 0x02000000
#define _DN_SAVE_OPERANDS_FLUSH_RESULTS 0x03000000
#define _CW_DEFAULT (_RC_NEAR + _DN_FLUSH + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW + _EM_INEXACT)
// Floating-point exception subcodes
#define _FPE_INVALID 0x81
#define _FPE_DENORMAL 0x82
#define _FPE_ZERODIVIDE 0x83
#define _FPE_OVERFLOW 0x84
#define _FPE_UNDERFLOW 0x85
#define _FPE_INEXACT 0x86
#define _FPE_UNEMULATED 0x87
#define _FPE_SQRTNEG 0x88
#define _FPE_STACKOVERFLOW 0x8a
#define _FPE_STACKUNDERFLOW 0x8b
#define _FPE_EXPLICITGEN 0x8c
#define _clear87 _clearfp
#define _status87 _statusfp

#ifdef __cplusplus
extern "C" {
#endif
_CRTIMP unsigned int __cdecl _clearfp(void);
_CRTIMP unsigned int __cdecl _statusfp(void);
_CRTIMP unsigned int __cdecl _controlfp(unsigned int _NewValue, unsigned int _Mask);
_CRTIMP errno_t __cdecl _controlfp_s(unsigned int *_CurrentState, unsigned int _NewValue, unsigned int _Mask);
_CRTIMP void __cdecl _set_controlfp(unsigned int _NewValue, unsigned int _Mask);
_CRTIMP unsigned int __cdecl _control87(unsigned int _NewValue, unsigned int _Mask);
_CRTIMP void __cdecl _fpreset(void);
_CRTIMP int *__cdecl __fpecode(void);
#define _fpecode (*__fpecode())
#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
_CRTIMP double __cdecl _copysign(double _Number, double _Sign);
_CRTIMP double __cdecl _chgsign(double _X);
#endif
_CRTIMP double __cdecl _scalb(double _X, long _Y);
_CRTIMP double __cdecl _logb(double _X);
_CRTIMP double __cdecl _nextafter(double _X, double _Y);
_CRTIMP int __cdecl _finite(double _X);
_CRTIMP int __cdecl _isnan(double _X);
_CRTIMP int __cdecl _fpclass(double _X);
#ifdef __cplusplus
}
#endif

#endif // _INC_FLOAT
