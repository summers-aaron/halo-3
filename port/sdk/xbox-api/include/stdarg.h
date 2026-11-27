// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// stdarg.h: ISO C variable arguments (vadefs.h).
#pragma once
#ifndef _INC_STDARG
#define _INC_STDARG
#include <vadefs.h>
#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end
#define va_copy(dst, src) ((dst) = (src))
#endif
