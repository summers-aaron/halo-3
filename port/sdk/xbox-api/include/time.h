// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// time.h: ISO C time. time_t is 64-bit; the standard names are inline forwarders to the runtime's _*64 functions
// (time.inl).
#pragma once
#ifndef _INC_TIME
#define _INC_TIME
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
#ifndef _CLOCK_T_DEFINED
#define _CLOCK_T_DEFINED
typedef long clock_t;
#endif
#include <port_crt_wtime.h>
#define CLOCKS_PER_SEC 1000
#define CLK_TCK CLOCKS_PER_SEC

_CRTIMP char *__cdecl asctime(const struct tm *_Tm);
_CRTIMP errno_t __cdecl asctime_s(char *_Buf, size_t _SizeInBytes, const struct tm *_Tm);
_CRTIMP clock_t __cdecl clock(void);
_CRTIMP size_t __cdecl strftime(char *_Buf, size_t _SizeInBytes, const char *_Format, const struct tm *_Tm);
_CRTIMP char *__cdecl _strdate(char *_Buffer);
_CRTIMP char *__cdecl _strtime(char *_Buffer);
_CRTIMP void __cdecl _tzset(void);
_CRTIMP double __cdecl _difftime64(__time64_t _Time1, __time64_t _Time2);
_CRTIMP char *__cdecl _ctime64(const __time64_t *_Time);
_CRTIMP errno_t __cdecl _ctime64_s(char *_Buf, size_t _SizeInBytes, const __time64_t *_Time);
_CRTIMP struct tm *__cdecl _gmtime64(const __time64_t *_Time);
_CRTIMP errno_t __cdecl _gmtime64_s(struct tm *_Tm, const __time64_t *_Time);
_CRTIMP struct tm *__cdecl _localtime64(const __time64_t *_Time);
_CRTIMP errno_t __cdecl _localtime64_s(struct tm *_Tm, const __time64_t *_Time);
_CRTIMP __time64_t __cdecl _mktime64(struct tm *_Tm);
_CRTIMP __time64_t __cdecl _mkgmtime64(struct tm *_Tm);
_CRTIMP __time64_t __cdecl _time64(__time64_t *_Time);
#include <time.inl>

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif // _INC_TIME
