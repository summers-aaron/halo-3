// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// stddef.h: ISO C common definitions. (port/sdk/include/stddef.h, ahead of this on the search path, re-points offsetof
// at the builtin after every inclusion.)
#pragma once
#ifndef _INC_STDDEF
#define _INC_STDDEF
#include <crtdefs.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifdef __cplusplus
#define offsetof(s, m) (size_t)&reinterpret_cast<const volatile char &>((((s *)0)->m))
#else
#define offsetof(s, m) (size_t)&(((s *)0)->m)
#endif

#ifdef __cplusplus
extern "C" {
#endif
_CRTIMP extern int *__cdecl _errno(void);
#ifdef __cplusplus
}
#endif
#define errno (*_errno())

#endif // _INC_STDDEF
