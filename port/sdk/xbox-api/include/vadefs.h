// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// vadefs.h: variable-argument lists for the 32-bit MS ABI. va_list is a char pointer walking 4-byte stack slots;
// va_start is the compiler's __va_start builtin.
#pragma once
#ifndef _INC_VADEFS
#define _INC_VADEFS
#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#ifdef HALO3_64BIT
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#endif
#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
typedef char *va_list;
#endif

#ifdef __cplusplus
extern "C" {
#endif
void __cdecl __va_start(va_list *, ...);
#ifdef __cplusplus
}
#define _ADDRESSOF(v) (&reinterpret_cast<const char &>(v))
#else
#define _ADDRESSOF(v) (&(v))
#endif

#ifdef HALO3_64BIT
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(__int64) - 1) & ~(sizeof(__int64) - 1))
#define _crt_va_start(ap, v) __va_start(&ap, &v)
#define _crt_va_arg(ap, t) __builtin_va_arg(ap, t)
#define _crt_va_end(ap) (ap = (va_list)0)
#else
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define _crt_va_start(ap, v) __va_start(&ap, &v)
#define _crt_va_arg(ap, t) (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define _crt_va_end(ap) (ap = (va_list)0)
#endif

#endif // _INC_VADEFS
