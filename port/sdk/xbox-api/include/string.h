// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// string.h: ISO C string and memory functions, the runtime's secure (_s) and case-insensitive variants, and the C++
// const-correct overloads (search functions taking a non-const pointer return one).
#pragma once
#ifndef _INC_STRING
#define _INC_STRING
#include <crtdefs.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *__cdecl memcpy(void *_Dst, const void *_Src, size_t _Size);
_CRTIMP void *__cdecl memmove(void *_Dst, const void *_Src, size_t _Size);
void *__cdecl memset(void *_Dst, int _Val, size_t _Size);
int __cdecl memcmp(const void *_Buf1, const void *_Buf2, size_t _Size);
_CRTIMP _CONST_RETURN void *__cdecl memchr(const void *_Buf, int _Val, size_t _MaxCount);
_CRTIMP void *__cdecl _memccpy(void *_Dst, const void *_Src, int _Val, size_t _MaxCount);
_CRTIMP int __cdecl _memicmp(const void *_Buf1, const void *_Buf2, size_t _Size);
_CRTIMP errno_t __cdecl memcpy_s(void *_Dst, rsize_t _DstSize, const void *_Src, rsize_t _MaxCount);
_CRTIMP errno_t __cdecl memmove_s(void *_Dst, rsize_t _DstSize, const void *_Src, rsize_t _MaxCount);

size_t __cdecl strlen(const char *_Str);
_CRTIMP size_t __cdecl strnlen(const char *_Str, size_t _MaxCount);
char *__cdecl strcpy(char *_Dest, const char *_Source);
_CRTIMP char *__cdecl strncpy(char *_Dest, const char *_Source, size_t _Count);
char *__cdecl strcat(char *_Dest, const char *_Source);
_CRTIMP char *__cdecl strncat(char *_Dest, const char *_Source, size_t _Count);
int __cdecl strcmp(const char *_Str1, const char *_Str2);
_CRTIMP int __cdecl strncmp(const char *_Str1, const char *_Str2, size_t _MaxCount);
_CRTIMP int __cdecl strcoll(const char *_Str1, const char *_Str2);
_CRTIMP size_t __cdecl strxfrm(char *_Dst, const char *_Src, size_t _MaxCount);
_CRTIMP _CONST_RETURN char *__cdecl strchr(const char *_Str, int _Val);
_CRTIMP _CONST_RETURN char *__cdecl strrchr(const char *_Str, int _Ch);
_CRTIMP _CONST_RETURN char *__cdecl strpbrk(const char *_Str, const char *_Control);
_CRTIMP _CONST_RETURN char *__cdecl strstr(const char *_Str, const char *_SubStr);
_CRTIMP size_t __cdecl strspn(const char *_Str, const char *_Control);
_CRTIMP size_t __cdecl strcspn(const char *_Str, const char *_Control);
_CRTIMP char *__cdecl strtok(char *_Str, const char *_Delim);
_CRTIMP char *__cdecl strtok_s(char *_Str, const char *_Delim, char **_Context);
_CRTIMP char *__cdecl strerror(int _ErrNum);
_CRTIMP errno_t __cdecl strerror_s(char *_Buf, size_t _SizeInBytes, int _ErrNum);
_CRTIMP errno_t __cdecl strcpy_s(char *_Dst, rsize_t _SizeInBytes, const char *_Src);
_CRTIMP errno_t __cdecl strncpy_s(char *_Dst, rsize_t _SizeInBytes, const char *_Src, rsize_t _MaxCount);
_CRTIMP errno_t __cdecl strcat_s(char *_Dst, rsize_t _SizeInBytes, const char *_Src);
_CRTIMP errno_t __cdecl strncat_s(char *_Dst, rsize_t _SizeInBytes, const char *_Src, rsize_t _MaxCount);
_CRTIMP char *__cdecl _strdup(const char *_Src);
_CRTIMP int __cdecl _stricmp(const char *_Str1, const char *_Str2);
_CRTIMP int __cdecl _strnicmp(const char *_Str1, const char *_Str2, size_t _MaxCount);
_CRTIMP int __cdecl _strcmpi(const char *_Str1, const char *_Str2);
_CRTIMP int __cdecl _stricoll(const char *_Str1, const char *_Str2);
_CRTIMP char *__cdecl _strlwr(char *_String);
_CRTIMP char *__cdecl _strupr(char *_String);
_CRTIMP errno_t __cdecl _strlwr_s(char *_Str, size_t _Size);
_CRTIMP errno_t __cdecl _strupr_s(char *_Str, size_t _Size);
_CRTIMP char *__cdecl _strrev(char *_Str);
char *__cdecl _strset(char *_Str, int _Val);
_CRTIMP char *__cdecl _strnset(char *_Str, int _Val, size_t _MaxCount);
_CRTIMP char *__cdecl _strerror(const char *_ErrMsg);

// Bounded length (strnlen under its secure-library name).
static __inline size_t __CRTDECL strnlen_s(const char *_Str, size_t _MaxCount)
{
    return strnlen(_Str, _MaxCount);
}

#include <port_crt_wstring.h>

#ifdef __cplusplus
}

// Non-const overloads (C++ linkage even when the header is included inside extern "C"): a search through a
// modifiable string yields a modifiable pointer.
extern "C++" {
#ifndef _CPP_NARROW_INLINES_DEFINED
#define _CPP_NARROW_INLINES_DEFINED
inline char *__CRTDECL strchr(char *_Str, int _Ch) { return (char *)strchr((const char *)_Str, _Ch); }
inline char *__CRTDECL strpbrk(char *_Str, const char *_Control) { return (char *)strpbrk((const char *)_Str, _Control); }
inline char *__CRTDECL strrchr(char *_Str, int _Ch) { return (char *)strrchr((const char *)_Str, _Ch); }
inline char *__CRTDECL strstr(char *_Str, const char *_SubStr) { return (char *)strstr((const char *)_Str, _SubStr); }
#endif
#ifndef _CPP_MEMCHR_DEFINED
#define _CPP_MEMCHR_DEFINED
inline void *__CRTDECL memchr(void *_Pv, int _C, size_t _N) { return (void *)memchr((const void *)_Pv, _C, _N); }
#endif
}
#endif

#endif // _INC_STRING
