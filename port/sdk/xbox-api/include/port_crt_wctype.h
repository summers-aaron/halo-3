// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// port_crt_wctype.h (internal): the classification table bits and data, and wide-character classification, shared
// by ctype.h and wchar.h; include only inside their extern "C" blocks.
#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif
#ifndef _UPPER
#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8
#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80
#define _LEADBYTE 0x8000
#define _ALPHA (0x0100 | _UPPER | _LOWER)
#endif
#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#endif
_CRTIMP const unsigned short *__cdecl __pctype_func(void);
_CRTIMP extern const unsigned short *_pctype;
_CRTIMP extern const unsigned short _wctype[];
_CRTIMP const wctype_t *__cdecl __pwctype_func(void);
_CRTIMP extern const wctype_t *_pwctype;
#endif
#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED
_CRTIMP int __cdecl iswalpha(wint_t _C);
_CRTIMP int __cdecl iswupper(wint_t _C);
_CRTIMP int __cdecl iswlower(wint_t _C);
_CRTIMP int __cdecl iswdigit(wint_t _C);
_CRTIMP int __cdecl iswxdigit(wint_t _C);
_CRTIMP int __cdecl iswspace(wint_t _C);
_CRTIMP int __cdecl iswpunct(wint_t _C);
_CRTIMP int __cdecl iswalnum(wint_t _C);
_CRTIMP int __cdecl iswprint(wint_t _C);
_CRTIMP int __cdecl iswgraph(wint_t _C);
_CRTIMP int __cdecl iswcntrl(wint_t _C);
_CRTIMP int __cdecl iswascii(wint_t _C);
_CRTIMP int __cdecl isleadbyte(int _C);
_CRTIMP wint_t __cdecl towupper(wint_t _C);
_CRTIMP wint_t __cdecl towlower(wint_t _C);
_CRTIMP int __cdecl iswctype(wint_t _C, wctype_t _Type);
_CRTIMP int __cdecl _iswctype_l(wint_t _C, wctype_t _Type, _locale_t _Locale);
_CRTIMP int __cdecl is_wctype(wint_t _C, wctype_t _Type);
#endif
#if !defined(_WCTYPE_INLINE_DEFINED) && !defined(_CTYPE_DISABLE_MACROS)
#define _WCTYPE_INLINE_DEFINED
#ifndef __cplusplus
#define iswalpha(_c) (iswctype(_c, _ALPHA))
#define iswupper(_c) (iswctype(_c, _UPPER))
#define iswlower(_c) (iswctype(_c, _LOWER))
#define iswdigit(_c) (iswctype(_c, _DIGIT))
#define iswxdigit(_c) (iswctype(_c, _HEX))
#define iswspace(_c) (iswctype(_c, _SPACE))
#define iswpunct(_c) (iswctype(_c, _PUNCT))
#define iswalnum(_c) (iswctype(_c, _ALPHA | _DIGIT))
#define iswprint(_c) (iswctype(_c, _BLANK | _PUNCT | _ALPHA | _DIGIT))
#define iswgraph(_c) (iswctype(_c, _PUNCT | _ALPHA | _DIGIT))
#define iswcntrl(_c) (iswctype(_c, _CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)
#endif
#define isleadbyte(_c) (__PCTYPE_FUNC[(unsigned char)(_c)] & _LEADBYTE)
#endif
