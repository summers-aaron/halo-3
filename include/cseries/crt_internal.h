#ifndef CSERIES_CRT_INTERNAL_H
#define CSERIES_CRT_INTERNAL_H

// The XDK CRT's internal error reporting (declared by the CRT sources' internal.h, defined by invarg.c), for the
// LIBCMT units. The public headers declare them only in an alternative-CRT configuration (safecrt.h), and xutility
// is C++ only. C and C++ units can include this.

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl _invalid_parameter(const wchar_t *expression, const wchar_t *function_name, const wchar_t *file_name,
                                unsigned int line, uintptr_t reserved);
void __cdecl _invoke_watson(const wchar_t *expression, const wchar_t *function_name, const wchar_t *file_name,
                            unsigned int line, uintptr_t reserved);

#ifdef __cplusplus
}
#endif

#endif // CSERIES_CRT_INTERNAL_H
