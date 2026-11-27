// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// setjmp.h: non-local jumps. The jump buffer keeps the Xbox runtime's size and alignment (84 16-byte-aligned slots:
// the PowerPC FPRs, GPRs, CR/IAR and 64 VMX128 registers), since game structures embed it (libpng's png_struct).
#pragma once
#ifndef _INC_SETJMP
#define _INC_SETJMP
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)
#ifdef __cplusplus
extern "C" {
#endif

#undef _setjmp
#define setjmp _setjmp

typedef struct __declspec(align(16)) SETJMP_VECTOR4
{
    float floats[4];
} SETJMP_VECTOR4;
#define _JBLEN 84
#define _JBTYPE SETJMP_VECTOR4
#ifndef _JMP_BUF_DEFINED
#define _JMP_BUF_DEFINED
typedef _JBTYPE jmp_buf[_JBLEN];
#endif

int __cdecl setjmp(jmp_buf _Buf);
#ifdef __cplusplus
}
extern "C" {
_CRTIMP __declspec(noreturn) void __cdecl longjmp(jmp_buf _Buf, int _Value) throw(...);
}
#else
_CRTIMP __declspec(noreturn) void __cdecl longjmp(jmp_buf _Buf, int _Value);
#endif

#pragma pack(pop)
#endif // _INC_SETJMP
