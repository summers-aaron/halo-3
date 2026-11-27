// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// wctype.h: wide-character classification (ctype.h declares it all).
#pragma once
#ifndef _INC_WCTYPE
#define _INC_WCTYPE
#include <ctype.h>
typedef wchar_t wctrans_t;
#ifdef __cplusplus
extern "C" {
#endif
_CRTIMP wint_t __cdecl towctrans(wint_t _C, wctrans_t _Type);
_CRTIMP wctrans_t __cdecl wctrans(const char *_Name);
_CRTIMP wctype_t __cdecl wctype(const char *_Name);
#ifdef __cplusplus
}
#endif
#endif
