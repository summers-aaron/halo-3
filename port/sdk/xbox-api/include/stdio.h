// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// stdio.h: ISO C input/output and the runtime's bounded / secure / wide formatting functions.
#pragma once
#ifndef _INC_STDIO
#define _INC_STDIO
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define BUFSIZ 512
#define _NSTREAM_ 512
#define _NFILE _NSTREAM_
#define _IOB_ENTRIES 20
#define EOF (-1)
#define FILENAME_MAX 260
#define FOPEN_MAX 20
#define _SYS_OPEN 20
#define TMP_MAX 32767
#define _P_tmpdir "D:\\"
#define L_tmpnam (sizeof(_P_tmpdir) + 12)
#define L_tmpnam_s (sizeof(_P_tmpdir) + 16)
#define TMP_MAX_S 2147483647
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0
#define _IOREAD 0x0001
#define _IOWRT 0x0002
#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004
#define _IOMYBUF 0x0008
#define _IOEOF 0x0010
#define _IOERR 0x0020
#define _IOSTRG 0x0040
#define _IORW 0x0080
#define _TWO_DIGIT_EXPONENT 0x1

#ifdef __cplusplus
extern "C" {
#endif

#include <port_crt_wstdio.h>
#ifndef _FPOS_T_DEFINED
#define _FPOS_T_DEFINED
typedef __int64 fpos_t;
#endif

_CRTIMP int __cdecl _filbuf(FILE *_File);
_CRTIMP int __cdecl _flsbuf(int _Ch, FILE *_File);
_CRTIMP FILE *__cdecl fopen(const char *_Filename, const char *_Mode);
_CRTIMP errno_t __cdecl fopen_s(FILE **_File, const char *_Filename, const char *_Mode);
_CRTIMP FILE *__cdecl freopen(const char *_Filename, const char *_Mode, FILE *_File);
_CRTIMP int __cdecl fclose(FILE *_File);
_CRTIMP int __cdecl fflush(FILE *_File);
_CRTIMP int __cdecl feof(FILE *_File);
_CRTIMP int __cdecl ferror(FILE *_File);
_CRTIMP void __cdecl clearerr(FILE *_File);
_CRTIMP int __cdecl _fileno(FILE *_File);
_CRTIMP size_t __cdecl fread(void *_DstBuf, size_t _ElementSize, size_t _Count, FILE *_File);
_CRTIMP size_t __cdecl fwrite(const void *_Str, size_t _Size, size_t _Count, FILE *_File);
_CRTIMP int __cdecl fseek(FILE *_File, long _Offset, int _Origin);
_CRTIMP long __cdecl ftell(FILE *_File);
_CRTIMP int __cdecl _fseeki64(FILE *_File, __int64 _Offset, int _Origin);
_CRTIMP __int64 __cdecl _ftelli64(FILE *_File);
_CRTIMP int __cdecl fgetpos(FILE *_File, fpos_t *_Pos);
_CRTIMP int __cdecl fsetpos(FILE *_File, const fpos_t *_Pos);
_CRTIMP void __cdecl rewind(FILE *_File);
_CRTIMP void __cdecl setbuf(FILE *_File, char *_Buffer);
_CRTIMP int __cdecl setvbuf(FILE *_File, char *_Buf, int _Mode, size_t _Size);
_CRTIMP int __cdecl fgetc(FILE *_File);
_CRTIMP int __cdecl fputc(int _Ch, FILE *_File);
_CRTIMP int __cdecl getc(FILE *_File);
_CRTIMP int __cdecl putc(int _Ch, FILE *_File);
_CRTIMP int __cdecl getchar(void);
_CRTIMP int __cdecl putchar(int _Ch);
_CRTIMP int __cdecl ungetc(int _Ch, FILE *_File);
_CRTIMP char *__cdecl fgets(char *_Buf, int _MaxCount, FILE *_File);
_CRTIMP int __cdecl fputs(const char *_Str, FILE *_File);
_CRTIMP char *__cdecl gets(char *_Buffer);
_CRTIMP int __cdecl puts(const char *_Str);
_CRTIMP int __cdecl remove(const char *_Filename);
_CRTIMP int __cdecl rename(const char *_OldFilename, const char *_NewFilename);
_CRTIMP int __cdecl _unlink(const char *_Filename);
_CRTIMP int __cdecl unlink(const char *_Filename);
_CRTIMP FILE *__cdecl tmpfile(void);
_CRTIMP char *__cdecl tmpnam(char *_Buffer);
_CRTIMP void __cdecl perror(const char *_ErrMsg);
_CRTIMP void __cdecl _lock_file(FILE *_File);
_CRTIMP void __cdecl _unlock_file(FILE *_File);
_CRTIMP int __cdecl _fclose_nolock(FILE *_File);
_CRTIMP int __cdecl _fflush_nolock(FILE *_File);
_CRTIMP size_t __cdecl _fread_nolock(void *_DstBuf, size_t _ElementSize, size_t _Count, FILE *_File);
_CRTIMP size_t __cdecl _fwrite_nolock(const void *_DstBuf, size_t _Size, size_t _Count, FILE *_File);
_CRTIMP int __cdecl _fseek_nolock(FILE *_File, long _Offset, int _Origin);
_CRTIMP long __cdecl _ftell_nolock(FILE *_File);
_CRTIMP int __cdecl _ungetc_nolock(int _Ch, FILE *_File);

_CRTIMP int __cdecl printf(const char *_Format, ...);
_CRTIMP int __cdecl fprintf(FILE *_File, const char *_Format, ...);
_CRTIMP int __cdecl sprintf(char *_Dest, const char *_Format, ...);
_CRTIMP int __cdecl vprintf(const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl vfprintf(FILE *_File, const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl vsprintf(char *_Dest, const char *_Format, va_list _Args);
_CRTIMP int __cdecl vsnprintf(char *_DstBuf, size_t _MaxCount, const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl _snprintf(char *_Dest, size_t _Count, const char *_Format, ...);
_CRTIMP int __cdecl _vsnprintf(char *_Dest, size_t _Count, const char *_Format, va_list _Args);
_CRTIMP int __cdecl _scprintf(const char *_Format, ...);
_CRTIMP int __cdecl _vscprintf(const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl printf_s(const char *_Format, ...);
_CRTIMP int __cdecl fprintf_s(FILE *_File, const char *_Format, ...);
_CRTIMP int __cdecl sprintf_s(char *_DstBuf, size_t _DstSize, const char *_Format, ...);
_CRTIMP int __cdecl vsprintf_s(char *_DstBuf, size_t _Size, const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl vfprintf_s(FILE *_File, const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl _snprintf_s(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, ...);
_CRTIMP int __cdecl _vsnprintf_s(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, va_list _ArgList);
_CRTIMP int __cdecl _vsprintf_l(char *_DstBuf, const char *_Format, _locale_t, va_list _ArgList);
_CRTIMP int __cdecl _vsprintf_s_l(char *_DstBuf, size_t _DstSize, const char *_Format, _locale_t _Locale, va_list _ArgList);
_CRTIMP int __cdecl _sprintf_s_l(char *_DstBuf, size_t _DstSize, const char *_Format, _locale_t _Locale, ...);
_CRTIMP int __cdecl scanf(const char *_Format, ...);
_CRTIMP int __cdecl fscanf(FILE *_File, const char *_Format, ...);
_CRTIMP int __cdecl sscanf(const char *_Src, const char *_Format, ...);
_CRTIMP int __cdecl sscanf_s(const char *_Src, const char *_Format, ...);

#define _fgetc_nolock(_stream) (--(_stream)->_cnt >= 0 ? 0xff & *(_stream)->_ptr++ : _filbuf(_stream))
#define _fputc_nolock(_c, _stream) \
    (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) : _flsbuf((_c), (_stream)))
#define _getc_nolock(_stream) _fgetc_nolock(_stream)
#define _putc_nolock(_c, _stream) _fputc_nolock(_c, _stream)

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif // _INC_STDIO
