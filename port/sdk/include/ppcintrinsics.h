/*
 * Port override of the XDK's ppcintrinsics.h (searched ahead of port/sdk/xbox-api/include).
 *
 * On 64-bit (HALO3_64BIT), inline scalar PowerPC intrinsics (__emit barriers/yield, __lwsync,
 * cache hints, byte-swaps, scalar float sqrt/rcp/fsel, CLZ, and __IsIntConst) directly into
 * game TUs so hot Havok, lock-free, and D3D paths avoid out-of-line MS-ABI calls into
 * port/sdk/vmx/ppc_intrinsics.c.
 *
 * On 32-bit and Xbox 360, this header is a pure #include_next pass-through.
 */
#include_next <ppcintrinsics.h>

#if defined(HALO3_64BIT) && !defined(PORT_SDK_PPCINTRINSICS_H) && !defined(PORT_PPC_INTRINSICS_IMPL)
#define PORT_SDK_PPCINTRINSICS_H

static __attribute__((always_inline)) inline void port_ppc_emit(unsigned int opcode)
{
    if (__builtin_constant_p(opcode)) {
        if (opcode == 0x7c2004acu || opcode == 0x7c0006acu) {
            __atomic_signal_fence(__ATOMIC_SEQ_CST);
            return;
        }
        if (opcode == 0x7c0004acu) {
            __atomic_thread_fence(__ATOMIC_SEQ_CST);
            return;
        }
        if (opcode == 0x7ffffb78u) {
            __builtin_ia32_pause();
            return;
        }
    }
    (__emit)(opcode);
}

static __attribute__((always_inline)) inline void port_ppc_nop_cache(int offset, const void *base)
{
    (void)offset;
    (void)base;
}

static __attribute__((always_inline)) inline unsigned long port_ppc_lwbrx(int offset, const void *base)
{
    unsigned int value;
    __builtin_memcpy(&value, (const char *)base + offset, 4);
    return (unsigned long)value;
}

static __attribute__((always_inline)) inline unsigned short port_ppc_bswap16(unsigned short value)
{
    return __builtin_bswap16(value);
}

static __attribute__((always_inline)) inline unsigned long port_ppc_bswap32(unsigned long value)
{
    return (unsigned long)__builtin_bswap32((unsigned int)value);
}

static __attribute__((always_inline)) inline float port_ppc_fsqrts(float fval)
{
    return __builtin_elementwise_sqrt(fval);
}

static __attribute__((always_inline)) inline float port_ppc_fres(double fval)
{
    return (float)(1.0 / fval);
}

static __attribute__((always_inline)) inline double port_ppc_fsel(double fComparand, double fValGE, double fLT)
{
    return fComparand >= 0.0 ? fValGE : fLT;
}

static __attribute__((always_inline)) inline unsigned int port_ppc_clz(long val)
{
    unsigned int u = (unsigned int)val;
    return u == 0u ? 32u : (unsigned int)__builtin_clz(u);
}

#undef __lwsync
#define __lwsync() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#define __emit(opcode) port_ppc_emit((opcode))
#define __dcbt(offset, base) port_ppc_nop_cache((offset), (base))
#define __dcbst(offset, base) port_ppc_nop_cache((offset), (base))
#define __dcbf(offset, base) port_ppc_nop_cache((offset), (base))
#define __loadwordbytereverse(offset, base) port_ppc_lwbrx((offset), (base))
#define _byteswap_ushort(val) port_ppc_bswap16((val))
#define _byteswap_ulong(val) port_ppc_bswap32((val))
#define __fsqrts(fval) port_ppc_fsqrts((fval))
#define __fres(fval) port_ppc_fres((fval))
#define __fsel(c, ge, lt) port_ppc_fsel((c), (ge), (lt))
#define _CountLeadingZeros(val) port_ppc_clz((val))
#define __IsIntConst(val) __builtin_constant_p((val))

#endif /* HALO3_64BIT && !PORT_SDK_PPCINTRINSICS_H && !PORT_PPC_INTRINSICS_IMPL */
