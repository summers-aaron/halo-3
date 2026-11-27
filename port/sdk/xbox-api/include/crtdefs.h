// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// crtdefs.h: the C runtime's base types and declaration macros (32-bit MS ABI: ILP32, 64-bit time_t, 16-bit wchar_t).
// Written from the public C/C++ runtime contract and the census of what the game uses; no XDK text.
#pragma once
#ifndef _INC_CRTDEFS
#define _INC_CRTDEFS
#include <sal.h>
#include <vadefs.h>

// Runtime entry points are DLL imports only for a DLL-runtime build that is not an Xbox one (the PC port's Windows
// target: /MD defines _DLL, and _XBOX_VER is set only once xtl.h is read); otherwise ordinary declarations. The
// C++ library (_CRTIMP2) follows the same rule unless it is linked statically.
#ifndef _CRTIMP
#if defined(_DLL) && _XBOX_VER < 200
#define _CRTIMP __declspec(dllimport)
#else
#define _CRTIMP
#endif
#endif
#ifndef _CRTIMP2
#if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
#define _CRTIMP2 __declspec(dllimport)
#else
#define _CRTIMP2
#endif
#endif
#ifndef _MRTIMP2
#if defined(_DLL) && !defined(_STATIC_CPPLIB) && _XBOX_VER < 200
#define _MRTIMP2 __declspec(dllimport)
#else
#define _MRTIMP2
#endif
#endif
#ifndef _CRTIMP_ALTERNATIVE
#ifdef _DLL
#define _CRTIMP_ALTERNATIVE _CRTIMP
#define _CRT_ALTERNATIVE_IMPORTED
#else
#define _CRTIMP_ALTERNATIVE
#endif
#endif
#ifndef _CRTIMP_PURE
#ifdef _STATIC_CPPLIB
#define _CRTIMP_PURE
#else
#define _CRTIMP_PURE _CRTIMP
#endif
#endif
#define _CRTIMP_NOIA64 _CRTIMP
#define _CRT_PACKING 8
#define _W64
#define __CRTDECL __cdecl
#define __CLR_OR_THIS_CALL
#define __CLRCALL_OR_CDECL __cdecl
#define _CRT_ALIGN(x) __declspec(align(x))
#define _CRTNOALIAS __declspec(noalias)
#define _CRTRESTRICT __declspec(restrict)
#define _CRT_JIT_INTRINSIC
#define _CRT_DEPRECATE_TEXT(text)
#define _CRT_INSECURE_DEPRECATE(replacement)
#define _CRT_INSECURE_DEPRECATE_MEMORY(replacement)
#define _CRT_INSECURE_DEPRECATE_GLOBALS(replacement)
#define _CRT_NONSTDC_DEPRECATE(name)
#define _CRT_OBSOLETE(name)
#define _CRT_MANAGED_HEAP_DEPRECATE
#define __STDC_SECURE_LIB__ 200411L
#define __GOT_SECURE_LIB__ __STDC_SECURE_LIB__
#ifndef __STDC_WANT_SECURE_LIB__
#define __STDC_WANT_SECURE_LIB__ 1
#endif
// The secure-CRT template overloads (array-size inference) are off in the Xbox runtime.
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 0
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 0
#define _SECURECRT_FILL_BUFFER_PATTERN 0xFD
#define _CRT_STRINGIZE_(x) #x
#define _CRT_STRINGIZE(x) _CRT_STRINGIZE_(x)
#define _CRT_WIDE_(s) L##s
#define _CRT_WIDE(s) _CRT_WIDE_(s)
#define _CRT_UNUSED(x) (void)x
#define _ARGMAX 100
#define _TRUNCATE ((size_t)-1)
#define UNALIGNED __unaligned
#ifdef __cplusplus
#define _CONST_RETURN const
#define _CRT_CONST_CORRECT_OVERLOADS
#else
#define _CONST_RETURN
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
#ifdef HALO3_64BIT
typedef unsigned __int64 size_t;
#else
typedef unsigned int size_t;
#endif
#endif
#ifndef _RSIZE_T_DEFINED
#define _RSIZE_T_DEFINED
typedef size_t rsize_t;
#endif
#ifndef _INTPTR_T_DEFINED
#define _INTPTR_T_DEFINED
#ifdef HALO3_64BIT
typedef __int64 intptr_t;
#else
typedef int intptr_t;
#endif
#endif
#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#ifdef HALO3_64BIT
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#endif
#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
#ifdef HALO3_64BIT
typedef __int64 ptrdiff_t;
#else
typedef int ptrdiff_t;
#endif
#endif
#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef unsigned short wchar_t; // C units; C++ has the built-in type
#endif
#ifndef _WCTYPE_T_DEFINED
#define _WCTYPE_T_DEFINED
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
#endif
#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
typedef char *va_list;
#endif
#ifndef _ERRCODE_DEFINED
#define _ERRCODE_DEFINED
typedef int errcode;
typedef int errno_t;
#endif
#ifndef _TIME32_T_DEFINED
#define _TIME32_T_DEFINED
typedef long __time32_t;
#endif
#ifndef _TIME64_T_DEFINED
#define _TIME64_T_DEFINED
typedef __int64 __time64_t;
#endif
#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
typedef __time64_t time_t;
#endif

// Locale handles (opaque to the game; _vsprintf_s_l takes one).
struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;
typedef struct localeinfo_struct
{
    pthreadlocinfo locinfo;
    pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;

#endif // _INC_CRTDEFS
