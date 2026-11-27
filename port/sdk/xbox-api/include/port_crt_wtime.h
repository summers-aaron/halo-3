// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// port_crt_wtime.h (internal): struct tm and the wide-character time functions, shared by time.h and wchar.h; include
// only inside their extern "C" blocks.
#ifndef _TM_DEFINED
#define _TM_DEFINED
struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
#endif
#ifndef _WTIME_DEFINED
#define _WTIME_DEFINED
_CRTIMP wchar_t *__cdecl _wasctime(const struct tm *_Tm);
_CRTIMP size_t __cdecl wcsftime(wchar_t *_Buf, size_t _SizeInWords, const wchar_t *_Format, const struct tm *_Tm);
_CRTIMP wchar_t *__cdecl _wctime64(const __time64_t *_Time);
_CRTIMP errno_t __cdecl _wctime64_s(wchar_t *_Buf, size_t _SizeInWords, const __time64_t *_Time);
#include <wtime.inl>
#endif
