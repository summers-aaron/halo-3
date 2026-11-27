// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// wtime.inl: the wide ctime forms over the runtime's 64-bit implementations. Included by time.h and wchar.h.
#ifndef _INC_WTIME_INL
#define _INC_WTIME_INL
static __inline wchar_t *__CRTDECL _wctime(const time_t *_Time) { return _wctime64(_Time); }
static __inline errno_t __CRTDECL _wctime_s(wchar_t *_Buffer, size_t _SizeInWords, const time_t *_Time)
{
    return _wctime64_s(_Buffer, _SizeInWords, _Time);
}
#endif
