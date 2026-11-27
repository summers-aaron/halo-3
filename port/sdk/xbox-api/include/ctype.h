// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// ctype.h: ISO C character classification (narrow and wide). C units get the runtime's table-lookup macros for the
// narrow classifiers and iswctype calls for the wide ones; C++ calls the functions.
#pragma once
#ifndef _INC_CTYPE
#define _INC_CTYPE
#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <port_crt_wctype.h>
#define PCTYPE __pctype_func()
#define PWCTYPE __pwctype_func()

#ifndef _CTYPE_DEFINED
#define _CTYPE_DEFINED
_CRTIMP int __cdecl _isctype(int _C, int _Type);
_CRTIMP int __cdecl _isctype_l(int _C, int _Type, _locale_t _Locale);
_CRTIMP int __cdecl isalpha(int _C);
_CRTIMP int __cdecl isupper(int _C);
_CRTIMP int __cdecl islower(int _C);
_CRTIMP int __cdecl isdigit(int _C);
_CRTIMP int __cdecl isxdigit(int _C);
_CRTIMP int __cdecl isspace(int _C);
_CRTIMP int __cdecl ispunct(int _C);
_CRTIMP int __cdecl isalnum(int _C);
_CRTIMP int __cdecl isprint(int _C);
_CRTIMP int __cdecl isgraph(int _C);
_CRTIMP int __cdecl iscntrl(int _C);
_CRTIMP int __cdecl toupper(int _C);
_CRTIMP int __cdecl tolower(int _C);
_CRTIMP int __cdecl _tolower(int _C);
_CRTIMP int __cdecl _toupper(int _C);
_CRTIMP int __cdecl __isascii(int _C);
_CRTIMP int __cdecl __toascii(int _C);
_CRTIMP int __cdecl __iscsymf(int _C);
_CRTIMP int __cdecl __iscsym(int _C);
#endif

#ifndef _CTYPE_DISABLE_MACROS
#ifndef MB_CUR_MAX
#define MB_CUR_MAX (1)
_CRTIMP extern int __mb_cur_max;
_CRTIMP int __cdecl ___mb_cur_max_func(void);
#endif
#define __chvalidchk(a, b) (__PCTYPE_FUNC[(a)] & (b))
#ifndef __cplusplus
#define isalpha(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _ALPHA) : __chvalidchk(_c, _ALPHA))
#define isupper(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _UPPER) : __chvalidchk(_c, _UPPER))
#define islower(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _LOWER) : __chvalidchk(_c, _LOWER))
#define isdigit(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _DIGIT) : __chvalidchk(_c, _DIGIT))
#define isxdigit(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _HEX) : __chvalidchk(_c, _HEX))
#define isspace(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _SPACE) : __chvalidchk(_c, _SPACE))
#define ispunct(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _PUNCT) : __chvalidchk(_c, _PUNCT))
#define isalnum(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _ALPHA | _DIGIT) : __chvalidchk(_c, (_ALPHA | _DIGIT)))
#define isprint(_c) \
    (MB_CUR_MAX > 1 ? _isctype(_c, _BLANK | _PUNCT | _ALPHA | _DIGIT) : __chvalidchk(_c, (_BLANK | _PUNCT | _ALPHA | _DIGIT)))
#define isgraph(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _PUNCT | _ALPHA | _DIGIT) : __chvalidchk(_c, (_PUNCT | _ALPHA | _DIGIT)))
#define iscntrl(_c) (MB_CUR_MAX > 1 ? _isctype(_c, _CONTROL) : __chvalidchk(_c, _CONTROL))
#endif
#define _tolower(_Char) ((_Char) - 'A' + 'a')
#define _toupper(_Char) ((_Char) - 'a' + 'A')
#define __isascii(_Char) ((unsigned)(_Char) < 0x80)
#define __toascii(_Char) ((_Char) & 0x7f)
#define __iscsymf(_c) (isalpha(_c) || ((_c) == '_'))
#define __iscsym(_c) (isalnum(_c) || ((_c) == '_'))
#if !__STDC__
#define isascii __isascii
#define toascii __toascii
#define iscsymf __iscsymf
#define iscsym __iscsym
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif // _INC_CTYPE
