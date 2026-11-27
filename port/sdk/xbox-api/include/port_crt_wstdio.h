// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// port_crt_wstdio.h (internal): FILE and the wide-character stdio functions, shared by stdio.h and wchar.h; include
// only inside their extern "C" blocks.
#ifndef _FILE_DEFINED
#define _FILE_DEFINED
struct _iobuf
{
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
};
typedef struct _iobuf FILE;
_CRTIMP FILE *__cdecl __iob_func(void);
#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#endif

#ifndef _WSTDIO_DEFINED
#define _WSTDIO_DEFINED
#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif
_CRTIMP wint_t __cdecl fgetwc(FILE *_File);
_CRTIMP wint_t __cdecl fputwc(wchar_t _Ch, FILE *_File);
_CRTIMP wint_t __cdecl getwc(FILE *_File);
_CRTIMP wint_t __cdecl putwc(wchar_t _Ch, FILE *_File);
_CRTIMP wint_t __cdecl ungetwc(wint_t _Ch, FILE *_File);
_CRTIMP wchar_t *__cdecl fgetws(wchar_t *_Dst, int _SizeInWords, FILE *_File);
_CRTIMP int __cdecl fputws(const wchar_t *_Str, FILE *_File);
_CRTIMP FILE *__cdecl _wfopen(const wchar_t *_Filename, const wchar_t *_Mode);
_CRTIMP int __cdecl wprintf(const wchar_t *_Format, ...);
_CRTIMP int __cdecl fwprintf(FILE *_File, const wchar_t *_Format, ...);
_CRTIMP int __cdecl vwprintf(const wchar_t *_Format, va_list _ArgList);
_CRTIMP int __cdecl vfwprintf(FILE *_File, const wchar_t *_Format, va_list _ArgList);
_CRTIMP int __cdecl wscanf(const wchar_t *_Format, ...);
_CRTIMP int __cdecl fwscanf(FILE *_File, const wchar_t *_Format, ...);
_CRTIMP int __cdecl swscanf(const wchar_t *_Src, const wchar_t *_Format, ...);
_CRTIMP int __cdecl _swprintf(wchar_t *_Dest, const wchar_t *_Format, ...);
_CRTIMP int __cdecl _vswprintf(wchar_t *_Dest, const wchar_t *_Format, va_list _Args);
_CRTIMP int __cdecl swprintf_s(wchar_t *_Dst, size_t _SizeInWords, const wchar_t *_Format, ...);
_CRTIMP int __cdecl vswprintf_s(wchar_t *_Dst, size_t _SizeInWords, const wchar_t *_Format, va_list _ArgList);
_CRTIMP int __cdecl _vswprintf_c_l(wchar_t *_DstBuf, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale,
                           va_list _ArgList);
_CRTIMP int __cdecl _snwprintf(wchar_t *_Dest, size_t _Count, const wchar_t *_Format, ...);
_CRTIMP int __cdecl _vsnwprintf(wchar_t *_Dest, size_t _Count, const wchar_t *_Format, va_list _Args);
_CRTIMP int __cdecl _snwprintf_s(wchar_t *_DstBuf, size_t _DstSizeInWords, size_t _MaxCount, const wchar_t *_Format, ...);
_CRTIMP int __cdecl _vsnwprintf_s(wchar_t *_DstBuf, size_t _DstSizeInWords, size_t _MaxCount, const wchar_t *_Format,
                          va_list _ArgList);
_CRTIMP int __cdecl _scwprintf(const wchar_t *_Format, ...);
_CRTIMP int __cdecl _vscwprintf(const wchar_t *_Format, va_list _ArgList);
#include <swprintf.inl>
#ifndef __cplusplus
#define swprintf _swprintf
#define vswprintf _vswprintf
#endif
#ifdef __cplusplus
inline wint_t __CRTDECL getwchar() { return fgetwc(stdin); }
inline wint_t __CRTDECL putwchar(wchar_t _C) { return fputwc(_C, stdout); }
#else
#define getwchar() fgetwc(stdin)
#define putwchar(_c) fputwc((_c), stdout)
#endif
#endif

