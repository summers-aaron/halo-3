// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// ppcintrinsics.h: the scalar PowerPC intrinsics (cache hints, byte-reversed access, FPU conversions, time base,
// barriers); brings in the vector intrinsics. Implemented on the PC by port/sdk/vmx/ppc_intrinsics.c.
#ifndef __PPCINTRINSICS_H__
#define __PPCINTRINSICS_H__

#include "VectorIntrinsics.h"

#ifdef __cplusplus
extern "C" {
#endif

// Emit one raw instruction word (the port maps the barrier and yield encodings).
void __emit(unsigned int opcode);

// Store a word at base + offset (bytes) and update base (stwu / stwux).
void __storewordupdate(unsigned long val, int offset, void *base);

// Cache-line hints at base + offset.
void __dcbt(int offset, const void *base);
void __dcbst(int offset, const void *base);
void __dcbf(int offset, const void *base);

// Byte-reversed load.
unsigned long __loadwordbytereverse(int offset, const void *base);

unsigned short _byteswap_ushort(unsigned short value);
unsigned long _byteswap_ulong(unsigned long value);

// Floating point: single sqrt, reciprocal estimate, select (fComparand >= 0 ? fValGE : fLT; NaN picks fLT), and
// the integer conversions that keep their result in a floating-point register.
float __fsqrts(float fval);
float __fres(double fval);
double __fsel(double fComparand, double fValGE, double fLT);
double __fctiwz(double fParam);
double __fctidz(double fParam);
double __fcfid(double i64Param);
void __stfiwx(double fParam, int offset, void *base);

unsigned int _CountLeadingZeros(long val);

// The 64-bit time base register.
unsigned __int64 __mftb();

// 1 when val is an integer constant expression.
int __IsIntConst(int val);

// Light-weight memory barrier (lwsync).
#define __lwsync() __emit(0x7C2004AC)

#ifdef __cplusplus
}
#endif

#endif
