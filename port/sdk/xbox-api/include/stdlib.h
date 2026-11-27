// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// stdlib.h: ISO C general utilities plus the runtime's extensions the game uses (byte swaps, rotates, 64-bit
// conversions, error/purecall handlers, _countof). The allocators are declared noalias/restrict, as the runtime's are.
#pragma once
#ifndef _INC_STDLIB
#define _INC_STDLIB
#include <crtdefs.h>
#include <limits.h>

#pragma pack(push, _CRT_PACKING)

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 0x7fff
#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
#endif
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX_PATH 260
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256
#define _MAX_ENV 32767
#define _OUT_TO_DEFAULT 0
#define _OUT_TO_STDERR 1
#define _OUT_TO_MSGBOX 2
#define _REPORT_ERRMODE 3
#define _WRITE_ABORT_MSG 0x1
#define _CALL_REPORTFAULT 0x2

#ifdef __cplusplus
extern "C" {
#endif

typedef int(__cdecl *_onexit_t)(void);
typedef struct _div_t
{
    int quot;
    int rem;
} div_t;
typedef struct _ldiv_t
{
    long quot;
    long rem;
} ldiv_t;
typedef void(__cdecl *_purecall_handler)(void);
typedef void(__cdecl *_invalid_parameter_handler)(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int,
                                                  uintptr_t);

_CRTIMP extern int __mb_cur_max;
_CRTIMP int __cdecl ___mb_cur_max_func(void);
_CRTIMP _purecall_handler __cdecl _set_purecall_handler(_purecall_handler _Handler);
_CRTIMP _purecall_handler __cdecl _get_purecall_handler(void);
_CRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(_invalid_parameter_handler _Handler);
_CRTIMP _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);
#ifndef _CRT_ERRNO_DEFINED
#define _CRT_ERRNO_DEFINED
_CRTIMP extern int *__cdecl _errno(void);
#define errno (*_errno())
errno_t __cdecl _set_errno(int _Value);
errno_t __cdecl _get_errno(int *_Value);
#endif
_CRTIMP unsigned long *__cdecl __doserrno(void);
#define _doserrno (*__doserrno())

_CRTIMP __declspec(noreturn) void __cdecl exit(int _Code);
_CRTIMP __declspec(noreturn) void __cdecl _exit(int _Code);
_CRTIMP void __cdecl abort(void);
int __cdecl atexit(void(__cdecl *)(void));
_onexit_t __cdecl _onexit(_onexit_t _Func);
_CRTIMP unsigned int __cdecl _set_abort_behavior(unsigned int _Flags, unsigned int _Mask);
_CRTIMP int __cdecl system(const char *_Command);
char *__cdecl getenv(const char *_VarName);

int __cdecl abs(int _X);
long __cdecl labs(long _X);
__int64 __cdecl _abs64(__int64);
_CRTIMP div_t __cdecl div(int _Numerator, int _Denominator);
_CRTIMP ldiv_t __cdecl ldiv(long _Numerator, long _Denominator);
_CRTIMP int __cdecl rand(void);
_CRTIMP void __cdecl srand(unsigned int _Seed);
_CRTIMP errno_t __cdecl rand_s(unsigned int *_RandomValue);

#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
_CRTIMP double __cdecl atof(const char *_String);
_CRTIMP double __cdecl _atof_l(const char *_String, _locale_t _Locale);
#endif
_CRTIMP int __cdecl atoi(const char *_Str);
_CRTIMP long __cdecl atol(const char *_Str);
_CRTIMP __int64 __cdecl _atoi64(const char *_String);
_CRTIMP double __cdecl strtod(const char *_Str, char **_EndPtr);
_CRTIMP long __cdecl strtol(const char *_Str, char **_EndPtr, int _Radix);
_CRTIMP unsigned long __cdecl strtoul(const char *_Str, char **_EndPtr, int _Radix);
_CRTIMP __int64 __cdecl _strtoi64(const char *_String, char **_EndPtr, int _Radix);
_CRTIMP unsigned __int64 __cdecl _strtoui64(const char *_String, char **_EndPtr, int _Radix);
_CRTIMP char *__cdecl _itoa(int _Value, char *_Dest, int _Radix);
_CRTIMP char *__cdecl _ltoa(long _Value, char *_Dest, int _Radix);
_CRTIMP char *__cdecl _ultoa(unsigned long _Value, char *_Dest, int _Radix);
_CRTIMP char *__cdecl _i64toa(__int64 _Val, char *_DstBuf, int _Radix);
_CRTIMP char *__cdecl _ui64toa(unsigned __int64 _Val, char *_DstBuf, int _Radix);
_CRTIMP errno_t __cdecl _itoa_s(int _Value, char *_DstBuf, size_t _Size, int _Radix);
_CRTIMP errno_t __cdecl _ltoa_s(long _Val, char *_DstBuf, size_t _Size, int _Radix);
_CRTIMP errno_t __cdecl _ultoa_s(unsigned long _Val, char *_DstBuf, size_t _Size, int _Radix);
#ifndef _CRT_WCSTO_DEFINED
#define _CRT_WCSTO_DEFINED
_CRTIMP double __cdecl wcstod(const wchar_t *_Str, wchar_t **_EndPtr);
_CRTIMP long __cdecl wcstol(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
_CRTIMP unsigned long __cdecl wcstoul(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
#endif
_CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
_CRTIMP long __cdecl _wtol(const wchar_t *_Str);
_CRTIMP __int64 __cdecl _wtoi64(const wchar_t *_Str);
_CRTIMP wchar_t *__cdecl _itow(int _Value, wchar_t *_Dest, int _Radix);
_CRTIMP int __cdecl mblen(const char *_Ch, size_t _MaxCount);
_CRTIMP int __cdecl mbtowc(wchar_t *_DstCh, const char *_SrcCh, size_t _SrcSizeInBytes);
_CRTIMP int __cdecl wctomb(char *_MbCh, wchar_t _WCh);
_CRTIMP size_t __cdecl mbstowcs(wchar_t *_Dest, const char *_Source, size_t _MaxCount);
_CRTIMP size_t __cdecl wcstombs(char *_Dest, const wchar_t *_Source, size_t _MaxCount);
_CRTIMP errno_t __cdecl mbstowcs_s(size_t *_PtNumOfCharConverted, wchar_t *_DstBuf, size_t _SizeInWords, const char *_SrcBuf,
                           size_t _MaxCount);
_CRTIMP errno_t __cdecl wcstombs_s(size_t *_PtNumOfCharConverted, char *_Dst, size_t _DstSizeInBytes, const wchar_t *_Src,
                           size_t _MaxCountInBytes);

_CRTIMP void *__cdecl bsearch(const void *_Key, const void *_Base, size_t _NumOfElements, size_t _SizeOfElements,
                      int(__cdecl *_PtFuncCompare)(const void *, const void *));
_CRTIMP void __cdecl qsort(void *_Base, size_t _NumOfElements, size_t _SizeOfElements,
                   int(__cdecl *_PtFuncCompare)(const void *, const void *));
_CRTIMP void __cdecl qsort_s(void *_Base, rsize_t _NumOfElements, rsize_t _SizeOfElements,
                     int(__cdecl *_PtFuncCompare)(void *, const void *, const void *), void *_Context);

#ifndef _CRT_ALLOCATION_DEFINED
#define _CRT_ALLOCATION_DEFINED
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl calloc(size_t _NumOfElements, size_t _SizeOfElements);
_CRTIMP _CRTNOALIAS void __cdecl free(void *_Memory);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl malloc(size_t _Size);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl realloc(void *_Memory, size_t _NewSize);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _recalloc(void *_Memory, size_t _Count, size_t _Size);
_CRTIMP _CRTNOALIAS void __cdecl _aligned_free(void *_Memory);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _aligned_malloc(size_t _Size, size_t _Alignment);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _aligned_realloc(void *_Memory, size_t _Size, size_t _Alignment);
#endif

unsigned short (_byteswap_ushort)(unsigned short value);
unsigned long (_byteswap_ulong)(unsigned long value);
unsigned __int64 _byteswap_uint64(unsigned __int64 value);
unsigned int __cdecl _rotl(unsigned int _Val, int _Shift);
unsigned int __cdecl _rotr(unsigned int _Val, int _Shift);
unsigned __int64 __cdecl _rotl64(unsigned __int64 _Val, int _Shift);
unsigned __int64 __cdecl _rotr64(unsigned __int64 _Val, int _Shift);
unsigned long __cdecl _lrotl(unsigned long _Val, int _Shift);
unsigned long __cdecl _lrotr(unsigned long _Val, int _Shift);
_CRTIMP void __cdecl _swab(char *_Buf1, char *_Buf2, int _SizeInBytes);
_CRTIMP void __cdecl _sleep(unsigned long _Duration);

#if !__STDC__ && !defined(__cplusplus)
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef __cplusplus
}

extern "C++" {
inline ldiv_t __CRTDECL div(long _A1, long _A2) { return ldiv(_A1, _A2); }
// _countof: the element count of a true array (a pointer argument does not compile).
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
}
#ifndef _countof
#define _countof(_Array) sizeof(*__countof_helper(_Array))
#endif
#else
#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif
#endif

#pragma pack(pop)
#endif // _INC_STDLIB
