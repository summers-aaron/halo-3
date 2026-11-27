// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// yvals.h: configuration shared by the C++ library headers (VC8-era library, version 405): namespace macros,
// exception mode, checked-iterator mode, secure copy helpers and the import macros of the C++ runtime.
#pragma once
#ifndef _YVALS
#define _YVALS

#include <crtdefs.h>

// A DLL-runtime build still links the C++ library statically (the Xbox runtime has no C++ library DLL).
#ifdef _DLL
#ifndef _STATIC_CPPLIB
#define _STATIC_CPPLIB
#endif
#endif

#pragma pack(push, _CRT_PACKING)

#define _CPPLIB_VER 405

#ifndef __CRTDECL
#define __CRTDECL __cdecl
#endif
#ifndef __CLR_OR_THIS_CALL
#define __CLR_OR_THIS_CALL
#endif
#ifndef __CLRCALL_OR_CDECL
#define __CLRCALL_OR_CDECL __cdecl
#endif
#ifndef __CLRCALL_PURE_OR_CDECL
#define __CLRCALL_PURE_OR_CDECL __cdecl
#endif

#define _WIN32_C_LIB 1
#define _MULTI_THREAD 1
#define _GLOBAL_LOCALE 0
#define _GLOBAL_USING 1

// The library throws only when the compiler has C++ exceptions on (the game builds without them: errors are
// reported through std::_Throw instead).
#ifndef _HAS_EXCEPTIONS
#ifdef _CPPUNWIND
#define _HAS_EXCEPTIONS 1
#else
#define _HAS_EXCEPTIONS 0
#endif
#endif

#ifndef _HAS_ITERATOR_DEBUGGING
#ifdef _DEBUG
#define _HAS_ITERATOR_DEBUGGING 1
#else
#define _HAS_ITERATOR_DEBUGGING 0
#endif
#endif

// Checked iterators are on by default; a failed check traps (no throw, no call).
#ifndef _SECURE_SCL
#define _SECURE_SCL 1
#endif
#ifndef _SECURE_SCL_THROWS
#define _SECURE_SCL_THROWS 0
#endif
#ifndef _SECURE_SCL_DEPRECATE
#define _SECURE_SCL_DEPRECATE 0
#endif
#define _SCL_INSECURE_DEPRECATE
#ifndef _SCL_SECURE_INVALID_PARAMETER
#define _SCL_SECURE_INVALID_PARAMETER(expr) __debugbreak()
#endif

// Raw block copies used by the character traits: the bounded CRT forms when the secure CRT is on.
#if __STDC_WANT_SECURE_LIB__
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy_s((dest), (destsize), (source), (count))
#define _CRT_SECURE_MEMMOVE(dest, destsize, source, count) ::memmove_s((dest), (destsize), (source), (count))
#else
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy((dest), (source), (count))
#define _CRT_SECURE_MEMMOVE(dest, destsize, source, count) ::memmove((dest), (source), (count))
#endif

#include <use_ansi.h>

// Import macros of the C++ runtime (_CRTIMP2 / _MRTIMP2 come from crtdefs.h).
#ifndef _CRTIMP2
#define _CRTIMP2
#endif
#ifndef _CRTIMP2_NCEEPURE
#define _CRTIMP2_NCEEPURE _CRTIMP2
#endif
#ifndef _MRTIMP2_NPURE
#define _MRTIMP2_NPURE
#endif
#ifndef _MRTIMP2_NPURE_NCEEPURE
#define _MRTIMP2_NPURE_NCEEPURE _MRTIMP2_NPURE
#endif
#ifndef _CRTIMP2_PURE
#define _CRTIMP2_PURE _CRTIMP2
#endif
#ifndef _CRTDATA2
#define _CRTDATA2 _CRTIMP2
#endif
#if defined(_DLL) && !defined(_STATIC_CPPLIB) && !defined(_M_CEE_PURE)
#define _DLL_CPPLIB
#endif

#ifdef __cplusplus
#define _STD_BEGIN namespace std {
#define _STD_END }
#define _STD ::std::
#define _STDEXT_BEGIN namespace stdext {
#define _STDEXT_END }
#define _STDEXT ::stdext::
#ifdef _STD_USING
#define _C_STD_BEGIN namespace std {
#define _C_STD_END }
#define _CSTD ::std::
#else
#define _C_STD_BEGIN
#define _C_STD_END
#define _CSTD ::
#endif
#define _C_LIB_DECL extern "C" {
#define _END_C_LIB_DECL }
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#else
#define _STD_BEGIN
#define _STD_END
#define _STD
#define _C_STD_BEGIN
#define _C_STD_END
#define _CSTD
#define _C_LIB_DECL
#define _END_C_LIB_DECL
#define _EXTERN_C
#define _END_EXTERN_C
#endif

#define _LONGLONG __int64
#define _ULONGLONG unsigned __int64
#define _LLONG_MAX 0x7fffffffffffffff
#define _ULLONG_MAX 0xffffffffffffffff
#define _C2 1

#ifdef __cplusplus
_STD_BEGIN
typedef bool _Bool;
_STD_END
#endif
typedef _LONGLONG _Longlong;
typedef _ULONGLONG _ULonglong;
typedef int _Mbstatet;

#define _EXTERN_TEMPLATE template
#define _THROW_BAD_ALLOC _THROW1(...)

#pragma pack(pop)

#endif // _YVALS
