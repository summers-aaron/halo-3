#ifndef PORT_D3D_GPU_CRT_H
#define PORT_D3D_GPU_CRT_H

/*
 * The C library functions the renderer (its Xenos frontend and backends) uses: port/data/data_crt.h's set (memory,
 * getenv, stdio file reads) plus the few it lacks. As there, the Windows build sees no C library headers (msvcrt.dll
 * through MinGW's import library), so the declarations are spelled out; hosted builds take them from the system headers.
 */
#include "../../data/data_crt.h"

#if !defined(PORT_DATA_HAVE_LIBC_HEADERS)
#ifdef __cplusplus
extern "C" {
#endif
int strcmp(const char *a, const char *b);
size_t strlen(const char *text);
size_t fwrite(const void *buffer, size_t size, size_t count, FILE *file);
int _vsnprintf(char *buffer, size_t size, const char *format, __builtin_va_list arguments);
#ifdef __cplusplus
}
#endif

/* C99 snprintf over msvcrt's _vsnprintf, which neither terminates a truncated string nor returns its length: the
 * text is always terminated, and the result is the characters written (the diagnostics append with it). */
static inline int snprintf(char *buffer, size_t size, const char *format, ...)
{
    if (size == 0)
        return 0;
    __builtin_va_list arguments;
    __builtin_va_start(arguments, format);
    int written = _vsnprintf(buffer, size, format, arguments);
    __builtin_va_end(arguments);
    buffer[size - 1] = '\0';
    if (written < 0 || (size_t)written >= size)
        written = (int)strlen(buffer);
    return written;
}
#endif

#endif /* PORT_D3D_GPU_CRT_H */
