#ifndef PORT_DATA_DATA_CRT_H
#define PORT_DATA_DATA_CRT_H

/*
 * The C library functions port/data uses. The Windows build compiles against no C library headers (msvcrt.dll is
 * linked through MinGW's import library, as for port/sdk/runtime), so the declarations are spelled out there; hosted
 * builds (Linux, the tmp/ tests) take them from the system headers.
 */
#include <stddef.h>

#if defined(__has_include)
#if __has_include(<string.h>)
#define PORT_DATA_HAVE_LIBC_HEADERS 1
#endif
#endif

#if defined(PORT_DATA_HAVE_LIBC_HEADERS)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#ifdef __cplusplus
extern "C" {
#endif
void *memcpy(void *dst, const void *src, size_t size);
void *memset(void *dst, int value, size_t size);
void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void *realloc(void *block, size_t size);
int memcmp(const void *a, const void *b, size_t size);
void free(void *block);
char *getenv(const char *name);
/* Whole-file reads (msvcrt's stdio; the FILE structure is never looked into). */
typedef struct _iobuf FILE;
FILE *fopen(const char *path, const char *mode);
size_t fread(void *buffer, size_t size, size_t count, FILE *file);
int fseek(FILE *file, long offset, int origin);
long ftell(FILE *file);
int fclose(FILE *file);
#define SEEK_SET 0
#define SEEK_END 2
#ifdef __cplusplus
}
#endif
#endif

#endif /* PORT_DATA_DATA_CRT_H */
