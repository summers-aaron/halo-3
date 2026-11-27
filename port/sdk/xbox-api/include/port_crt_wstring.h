// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// port_crt_wstring.h (internal): the wide-character string functions, shared by string.h and wchar.h; include only
// inside their extern "C" blocks.
#ifndef _WSTRING_DEFINED
#define _WSTRING_DEFINED
_CRTIMP size_t __cdecl wcslen(const wchar_t *_Str);
_CRTIMP size_t __cdecl wcsnlen(const wchar_t *_Src, size_t _MaxCount);
_CRTIMP wchar_t *__cdecl wcscpy(wchar_t *_Dest, const wchar_t *_Source);
_CRTIMP wchar_t *__cdecl wcsncpy(wchar_t *_Dest, const wchar_t *_Source, size_t _Count);
_CRTIMP wchar_t *__cdecl wcscat(wchar_t *_Dest, const wchar_t *_Source);
_CRTIMP wchar_t *__cdecl wcsncat(wchar_t *_Dest, const wchar_t *_Source, size_t _Count);
_CRTIMP int __cdecl wcscmp(const wchar_t *_Str1, const wchar_t *_Str2);
_CRTIMP int __cdecl wcsncmp(const wchar_t *_Str1, const wchar_t *_Str2, size_t _MaxCount);
_CRTIMP int __cdecl wcscoll(const wchar_t *_Str1, const wchar_t *_Str2);
_CRTIMP size_t __cdecl wcsxfrm(wchar_t *_Dst, const wchar_t *_Src, size_t _MaxCount);
_CRTIMP _CONST_RETURN wchar_t *__cdecl wcschr(const wchar_t *_Str, wchar_t _Ch);
_CRTIMP _CONST_RETURN wchar_t *__cdecl wcsrchr(const wchar_t *_Str, wchar_t _Ch);
_CRTIMP _CONST_RETURN wchar_t *__cdecl wcspbrk(const wchar_t *_Str, const wchar_t *_Control);
_CRTIMP _CONST_RETURN wchar_t *__cdecl wcsstr(const wchar_t *_Str, const wchar_t *_SubStr);
_CRTIMP size_t __cdecl wcsspn(const wchar_t *_Str, const wchar_t *_Control);
_CRTIMP size_t __cdecl wcscspn(const wchar_t *_Str, const wchar_t *_Control);
_CRTIMP wchar_t *__cdecl wcstok(wchar_t *_Str, const wchar_t *_Delim);
_CRTIMP wchar_t *__cdecl wcstok_s(wchar_t *_Str, const wchar_t *_Delim, wchar_t **_Context);
_CRTIMP errno_t __cdecl wcscpy_s(wchar_t *_Dst, rsize_t _SizeInWords, const wchar_t *_Src);
_CRTIMP errno_t __cdecl wcsncpy_s(wchar_t *_Dst, rsize_t _SizeInWords, const wchar_t *_Src, rsize_t _MaxCount);
_CRTIMP errno_t __cdecl wcscat_s(wchar_t *_Dst, rsize_t _SizeInWords, const wchar_t *_Src);
_CRTIMP errno_t __cdecl wcsncat_s(wchar_t *_Dst, rsize_t _SizeInWords, const wchar_t *_Src, rsize_t _MaxCount);
_CRTIMP wchar_t *__cdecl _wcsdup(const wchar_t *_Str);
_CRTIMP int __cdecl _wcsicmp(const wchar_t *_Str1, const wchar_t *_Str2);
_CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1, const wchar_t *_Str2, size_t _MaxCount);
_CRTIMP int __cdecl _wcsicoll(const wchar_t *_Str1, const wchar_t *_Str2);
_CRTIMP wchar_t *__cdecl _wcslwr(wchar_t *_String);
_CRTIMP wchar_t *__cdecl _wcsupr(wchar_t *_String);
_CRTIMP errno_t __cdecl _wcslwr_s(wchar_t *_Str, size_t _SizeInWords);
_CRTIMP errno_t __cdecl _wcsupr_s(wchar_t *_Str, size_t _Size);
_CRTIMP wchar_t *__cdecl _wcsrev(wchar_t *_Str);
_CRTIMP wchar_t *__cdecl _wcsset(wchar_t *_Str, wchar_t _Val);
_CRTIMP wchar_t *__cdecl _wcsnset(wchar_t *_Str, wchar_t _Val, size_t _MaxCount);

static __inline size_t __CRTDECL wcsnlen_s(const wchar_t *_Src, size_t _MaxCount)
{
    return wcsnlen(_Src, _MaxCount);
}
#endif
#ifdef __cplusplus
extern "C++" {
#ifndef _CPP_WIDE_INLINES_DEFINED
#define _CPP_WIDE_INLINES_DEFINED
inline wchar_t *__CRTDECL wcschr(wchar_t *_Str, wchar_t _Ch) { return (wchar_t *)wcschr((const wchar_t *)_Str, _Ch); }
inline wchar_t *__CRTDECL wcspbrk(wchar_t *_Str, const wchar_t *_Control) { return (wchar_t *)wcspbrk((const wchar_t *)_Str, _Control); }
inline wchar_t *__CRTDECL wcsrchr(wchar_t *_Str, wchar_t _Ch) { return (wchar_t *)wcsrchr((const wchar_t *)_Str, _Ch); }
inline wchar_t *__CRTDECL wcsstr(wchar_t *_Str, const wchar_t *_SubStr) { return (wchar_t *)wcsstr((const wchar_t *)_Str, _SubStr); }
#endif
}
#endif
