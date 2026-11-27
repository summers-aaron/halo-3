// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// swprintf.inl: the ISO C (count-taking) swprintf / vswprintf, over the runtime's _vswprintf_c_l. Included by
// stdio.h and wchar.h inside their extern "C" blocks.
#ifndef _INC_SWPRINTF_INL
#define _INC_SWPRINTF_INL

static __inline int swprintf(wchar_t *_String, size_t _Count, const wchar_t *_Format, ...)
{
    va_list _Arglist;
    int _Ret;
    _crt_va_start(_Arglist, _Format);
    _Ret = _vswprintf_c_l(_String, _Count, _Format, NULL, _Arglist);
    _crt_va_end(_Arglist);
    return _Ret;
}

static __inline int __CRTDECL vswprintf(wchar_t *_String, size_t _Count, const wchar_t *_Format, va_list _Ap)
{
    return _vswprintf_c_l(_String, _Count, _Format, NULL, _Ap);
}

#endif
