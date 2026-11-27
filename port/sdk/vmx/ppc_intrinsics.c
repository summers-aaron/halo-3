// port/sdk/vmx: the XDK's PowerPC intrinsics that are not vector operations, plus per-thread storage for the
// /QVMXReserve register file (__VMXGetReg / __VMXSetReg) and LIBCMT _blkmov.
#define PORT_PPC_INTRINSICS_IMPL 1
#include "../runtime/port_runtime.h"

// The barriers map to the x86 ordering that gives what they give (x86 is TSO: loads are not reordered with loads, nor
// stores with stores, nor a store with an earlier load). sync orders everything, a store before a later load included:
// a full fence (mfence). lwsync orders all but a store before a later load, and eieio orders stores: TSO already does,
// so they only keep the compiler from moving accesses across them (the mfence they cost before was 2% of the main
// thread, from Havok's and the game's lock-free code).
void PORT_MS_ABI __emit(unsigned int opcode)
{
    switch (opcode) {
    case 0x7c0004ac: // sync
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        return;
    case 0x7c2004ac: // lwsync
    case 0x7c0006ac: // eieio
        __atomic_signal_fence(__ATOMIC_SEQ_CST);
        return;
    case 0x7ffffb78: // or r31,r31,r31: YieldProcessor(), a low-priority hint
        __builtin_ia32_pause();
        return;
    default:
        port_write_error("port: __emit of an unknown PowerPC instruction\n");
        port_exit(5);
    }
}

// Scalar PowerPC instructions the XDK exposes as functions.
unsigned int PORT_MS_ABI _CountLeadingZeros(long value) { return value == 0 ? 32 : (unsigned int)__builtin_clz((unsigned int)value); }
void PORT_MS_ABI __dcbt(int offset, const void *base) { (void)offset; (void)base; }   // cache-line touch: a hint
void PORT_MS_ABI __dcbst(int offset, const void *base) { (void)offset; (void)base; }  // cache-line store: coherent here
void PORT_MS_ABI __dcbf(int offset, const void *base) { (void)offset; (void)base; }   // cache-line flush: coherent here
int PORT_MS_ABI __IsIntConst(int value) { (void)value; return 0; }                    // a compile-time query; "not constant" is safe

// lwbrx: a little-endian word, which is a plain load on a little-endian host.
unsigned long PORT_MS_ABI __loadwordbytereverse(int offset, const void *base)
{
    unsigned long value;
    __builtin_memcpy(&value, (const char *)base + offset, sizeof(value));
    return value;
}

// stwu: note that ppcintrinsics.h declares void __storewordupdate(unsigned long, int, void*) by value; only
// tu/d3d9/query.cpp (unused XDK GPU ring writer) calls it.
void PORT_MS_ABI __storewordupdate(unsigned long value, int offset, void *base)
{
    if (base != 0)
        __builtin_memcpy((char *)base + offset, &value, sizeof(value));
}

// fcfid / fctidz / fctiwz / stfiwx move 64-bit or 32-bit integers through a floating-point register's bit pattern.
double PORT_MS_ABI __fcfid(double integer_bits)
{
    long long value;
    __builtin_memcpy(&value, &integer_bits, sizeof(value));
    return (double)value;
}

double PORT_MS_ABI __fctidz(double value)
{
    long long truncated = value >= (double)0x7fffffffffffffffLL ? 0x7fffffffffffffffLL
        : (value <= -(double)0x8000000000000000ULL ? (long long)0x8000000000000000ULL : (long long)value);
    double bits;
    __builtin_memcpy(&bits, &truncated, sizeof(bits));
    return bits;
}

double PORT_MS_ABI __fctiwz(double value)
{
    int truncated;
    if (value != value)
        truncated = (int)0x80000000u;
    else if (value >= 2147483647.0)
        truncated = 2147483647;
    else if (value <= -2147483648.0)
        truncated = (int)0x80000000u;
    else
        truncated = (int)value;
    unsigned long long raw = (unsigned int)truncated;
    double bits;
    __builtin_memcpy(&bits, &raw, sizeof(bits));
    return bits;
}

void PORT_MS_ABI __stfiwx(double fparam, int offset, void *base)
{
    unsigned long long raw;
    __builtin_memcpy(&raw, &fparam, sizeof(raw));
    unsigned int low = (unsigned int)raw;
    __builtin_memcpy((char *)base + offset, &low, sizeof(low));
}

float PORT_MS_ABI __fres(double value) { return (float)(1.0 / value); }  // the hardware estimate is within 1/256; exact here
float PORT_MS_ABI __fsqrts(float value) { return __builtin_sqrtf(value); }
double PORT_MS_ABI __fsel(double comparand, double greater_or_equal, double less) { return comparand >= 0.0 ? greater_or_equal : less; }

void *PORT_MS_ABI _blkmov(void *dst, const void *src, unsigned int size)
{
    return __builtin_memmove(dst, src, size);
}

// Per-thread bank of 128 x 16-byte VMX registers for /QVMXReserve (__VMXGetReg / __VMXSetReg in Havok).
#if defined(_WIN32)
static __declspec(thread) unsigned char port_tls_vmx_regs[128 * 16 + 16];
#else
static __thread __attribute__((aligned(16))) unsigned char port_tls_vmx_regs[128 * 16];
#endif

void *PORT_MS_ABI port_vmx_reg_bank(void)
{
    __UINTPTR_TYPE__ addr = (__UINTPTR_TYPE__)port_tls_vmx_regs;
#if defined(_WIN32)
    addr = (addr + (__UINTPTR_TYPE__)15u) & ~(__UINTPTR_TYPE__)15u;
#endif
    return (void *)addr;
}
