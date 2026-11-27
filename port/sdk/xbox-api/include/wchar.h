// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// wchar.h: wide-character utilities: FILE and the wide stdio / string / ctype / time declarations (the pieces stdio.h,
// string.h, ctype.h and time.h share), the wide conversions of stdlib.h, multibyte state functions and the inline wmem* functions.
#pragma once
#ifndef _INC_WCHAR
#define _INC_WCHAR
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif
#include <port_crt_wstdio.h>
#include <port_crt_wctype.h>
#include <port_crt_wstring.h>
#include <port_crt_wtime.h>

#define WCHAR_MIN 0
#define WCHAR_MAX 0xffff
typedef int mbstate_t;
typedef wchar_t _Wint_t;

#ifndef _CRT_WCSTO_DEFINED
#define _CRT_WCSTO_DEFINED
_CRTIMP double __cdecl wcstod(const wchar_t *_Str, wchar_t **_EndPtr);
_CRTIMP long __cdecl wcstol(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
_CRTIMP unsigned long __cdecl wcstoul(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
#endif
_CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
_CRTIMP long __cdecl _wtol(const wchar_t *_Str);

_CRTIMP wint_t __cdecl btowc(int);
_CRTIMP size_t __cdecl mbrlen(const char *_Ch, size_t _SizeInBytes, mbstate_t *_State);
_CRTIMP size_t __cdecl mbrtowc(wchar_t *_DstCh, const char *_SrcCh, size_t _SizeInBytes, mbstate_t *_State);
_CRTIMP size_t __cdecl mbsrtowcs(wchar_t *_Dest, const char **_PSrc, size_t _Count, mbstate_t *_State);
_CRTIMP errno_t __cdecl mbsrtowcs_s(size_t *_Retval, wchar_t *_Dst, size_t _SizeInWords, const char **_PSrc, size_t _N,
                            mbstate_t *_State);
_CRTIMP size_t __cdecl wcrtomb(char *_Dest, wchar_t _Source, mbstate_t *_State);
_CRTIMP errno_t __cdecl wcrtomb_s(size_t *_Retval, char *_Dst, size_t _SizeInBytes, wchar_t _Ch, mbstate_t *_State);
_CRTIMP size_t __cdecl wcsrtombs(char *_Dest, const wchar_t **_PSource, size_t _Count, mbstate_t *_State);
_CRTIMP errno_t __cdecl wcsrtombs_s(size_t *_Retval, char *_Dst, size_t _SizeInBytes, const wchar_t **_Src, size_t _Size,
                            mbstate_t *_State);
_CRTIMP int __cdecl wctob(wint_t _WCh);
void *__cdecl (memcpy)(void *_Dst, const void *_Src, size_t _Size);
_CRTIMP void *__cdecl (memmove)(void *_Dst, const void *_Src, size_t _Size);
_CRTIMP errno_t __cdecl memcpy_s(void *_Dst, rsize_t _DstSize, const void *_Src, rsize_t _MaxCount);
_CRTIMP errno_t __cdecl memmove_s(void *_Dst, rsize_t _DstSize, const void *_Src, rsize_t _MaxCount);

__inline int __CRTDECL fwide(FILE *_F, int _M)
{
    (void)_F;
    return _M;
}
__inline int __CRTDECL mbsinit(const mbstate_t *_P) { return _P == NULL || *_P == 0; }
__inline _CONST_RETURN wchar_t *__CRTDECL wmemchr(const wchar_t *_S, wchar_t _C, size_t _N)
{
    for (; 0 < _N; ++_S, --_N)
        if (*_S == _C)
            return (_CONST_RETURN wchar_t *)_S;
    return 0;
}
__inline int __CRTDECL wmemcmp(const wchar_t *_S1, const wchar_t *_S2, size_t _N)
{
    for (; 0 < _N; ++_S1, ++_S2, --_N)
        if (*_S1 != *_S2)
            return *_S1 < *_S2 ? -1 : +1;
    return 0;
}
__inline wchar_t *__CRTDECL wmemcpy(wchar_t *_S1, const wchar_t *_S2, size_t _N)
{
    return (wchar_t *)memcpy(_S1, _S2, _N * sizeof(wchar_t));
}
__inline errno_t __CRTDECL wmemcpy_s(wchar_t *_S1, rsize_t _N1, const wchar_t *_S2, rsize_t _N)
{
    return memcpy_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
}
__inline wchar_t *__CRTDECL wmemmove(wchar_t *_S1, const wchar_t *_S2, size_t _N)
{
    return (wchar_t *)memmove(_S1, _S2, _N * sizeof(wchar_t));
}
__inline errno_t __CRTDECL wmemmove_s(wchar_t *_S1, rsize_t _N1, const wchar_t *_S2, rsize_t _N)
{
    return memmove_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
}
__inline wchar_t *__CRTDECL wmemset(wchar_t *_S, wchar_t _C, size_t _N)
{
    wchar_t *_Su = _S;
    for (; 0 < _N; ++_Su, --_N)
        *_Su = _C;
    return _S;
}
#ifdef __cplusplus
extern "C++" {
inline wchar_t *__CRTDECL wmemchr(wchar_t *_S, wchar_t _C, size_t _N)
{
    return (wchar_t *)wmemchr((const wchar_t *)_S, _C, _N);
}
}
#endif

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif // _INC_WCHAR
