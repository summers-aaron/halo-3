// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// time.inl: the ISO C time functions over the runtime's 64-bit (_*64) implementations. Included by time.h.
#ifndef _INC_TIME_INL
#define _INC_TIME_INL
static __inline double __CRTDECL difftime(time_t _Time1, time_t _Time2) { return _difftime64(_Time1, _Time2); }
static __inline char *__CRTDECL ctime(const time_t *_Time) { return _ctime64(_Time); }
static __inline errno_t __CRTDECL ctime_s(char *_Buffer, size_t _SizeInBytes, const time_t *_Time)
{
    return _ctime64_s(_Buffer, _SizeInBytes, _Time);
}
static __inline struct tm *__CRTDECL gmtime(const time_t *_Time) { return _gmtime64(_Time); }
static __inline errno_t __CRTDECL gmtime_s(struct tm *_Tm, const time_t *_Time) { return _gmtime64_s(_Tm, _Time); }
static __inline struct tm *__CRTDECL localtime(const time_t *_Time) { return _localtime64(_Time); }
static __inline errno_t __CRTDECL localtime_s(struct tm *_Tm, const time_t *_Time) { return _localtime64_s(_Tm, _Time); }
static __inline time_t __CRTDECL mktime(struct tm *_Tm) { return _mktime64(_Tm); }
static __inline time_t __CRTDECL _mkgmtime(struct tm *_Tm) { return _mkgmtime64(_Tm); }
static __inline time_t __CRTDECL time(time_t *_Time) { return _time64(_Time); }
#endif
