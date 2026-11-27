// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// intrin.h: the compiler-intrinsic declarations of the Xbox CRT sources (Source/crt/intrin.h): interlocked
// operations, barriers, rotates, byte swaps, and the CRT routines the compiler also expands inline. Reached through
// port/sdk/xdk-compat/Source/crt/intrin.h, which renames the bit-test pair while this file is read (clang's own
// declarations of those two take a volatile pointer).
#pragma once
#define __INTRIN_H_

#include <crtdefs.h>
#include <setjmp.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

void *__cdecl _alloca(size_t);
int __cdecl abs(int);
long __cdecl labs(long);
__int64 __cdecl _abs64(__int64);

unsigned short __cdecl (_byteswap_ushort)(unsigned short value);
unsigned long __cdecl (_byteswap_ulong)(unsigned long value);
unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64 value);

void __cdecl __debugbreak(void);

// Interlocked read-modify-write on a 32-bit (or 64-bit) location; each returns the value the operation defines
// (the new value for increment / decrement, the old value for the others).
long __cdecl _InterlockedIncrement(long volatile *);
long __cdecl _InterlockedDecrement(long volatile *);
long _InterlockedExchange(long volatile *, long);
long _InterlockedExchangeAdd(long volatile *, long);
long _InterlockedCompareExchange(long volatile *, long, long);
__int64 _InterlockedCompareExchange64(__int64 volatile *, __int64, __int64);
unsigned char _interlockedbittestandset(long *a, long b);
unsigned char _interlockedbittestandreset(long *a, long b);

// Compiler-only barriers (no instruction).
void _WriteBarrier(void);
void _ReadWriteBarrier(void);

void *_ReturnAddress(void);
void *_AddressOfReturnAddress(void);

unsigned int __cdecl _rotl(unsigned int, int);
unsigned int __cdecl _rotr(unsigned int, int);
unsigned long __cdecl _lrotl(unsigned long, int);
unsigned long __cdecl _lrotr(unsigned long, int);
unsigned __int64 __cdecl _rotl64(unsigned __int64, int);
unsigned __int64 __cdecl _rotr64(unsigned __int64, int);

int __cdecl (memcmp)(const void *, const void *, size_t);
void *__cdecl (memcpy)(void *, const void *, size_t);
void *__cdecl (memset)(void *, int, size_t);
char *__cdecl strcat(char *, const char *);
int __cdecl strcmp(const char *, const char *);
char *__cdecl strcpy(char *, const char *);
size_t __cdecl strlen(const char *);

int __cdecl _setjmp(jmp_buf);

#if defined(__cplusplus)
}
#endif
