#ifndef PORT_SDK_VECTORINTRINSICS_H
#define PORT_SDK_VECTORINTRINSICS_H

/*
 * The XDK's VMX128 intrinsics (<vectorintrinsics.h>, searched after port/sdk/include), with __lvx and __stvx made
 * exact-address loads and stores of 16 bytes.
 *
 * The XDK spells them as a dereference of a __vector4 pointer. The Xbox compiler emits lvx / stvx for that, which
 * ignore the low four address bits, so the code only ever uses them on addresses the Xbox build lays out 16-aligned.
 * The PC build does not always lay the same storage out 16-aligned (a 4-aligned local array), and the dereference
 * tells the optimizer it is, so it stores with movaps, which faults. A byte copy keeps the meaning wherever the Xbox
 * address was aligned and makes no alignment claim (GAME-CHANGES F90).
 */

#include_next <vectorintrinsics.h>

#undef __lvx
#undef __stvx

static inline __vector4 port_vmx_load16(const void *address)
{
    __vector4 value;
    __builtin_memcpy(&value, address, sizeof(value));
    return value;
}

static inline void port_vmx_store16(void *address, __vector4 value)
{
    __builtin_memcpy(address, &value, sizeof(value));
}

#define __lvx(base, offset) port_vmx_load16((const char *)(base) + (offset))
#define __stvx(vSrc, base, offset) port_vmx_store16((char *)(base) + (offset), (vSrc));

#ifdef HALO3_64BIT

typedef float port_v4sf __attribute__((vector_size(16), aligned(1)));
typedef unsigned int port_v4su __attribute__((vector_size(16), aligned(1)));
typedef int port_v4si __attribute__((vector_size(16), aligned(1)));

static inline port_v4sf port_vmx_to_v4sf(__vector4 v)
{
    port_v4sf r;
    __builtin_memcpy(&r, &v, 16);
    return r;
}

static inline __vector4 port_vmx_from_v4sf(port_v4sf r)
{
    __vector4 out;
    __builtin_memcpy(&out, &r, 16);
    return out;
}

static inline port_v4su port_vmx_to_v4su(__vector4 v)
{
    port_v4su r;
    __builtin_memcpy(&r, &v, 16);
    return r;
}

static inline __vector4 port_vmx_from_v4su(port_v4su r)
{
    __vector4 out;
    __builtin_memcpy(&out, &r, 16);
    return out;
}

static inline port_v4si port_vmx_to_v4si(__vector4 v)
{
    port_v4si r;
    __builtin_memcpy(&r, &v, 16);
    return r;
}

static inline __vector4 port_vmx_from_v4si(port_v4si r)
{
    __vector4 out;
    __builtin_memcpy(&out, &r, 16);
    return out;
}

static inline port_v4su port_vmx_f32_to_u32(port_v4sf r)
{
    port_v4su u;
    __builtin_memcpy(&u, &r, 16);
    return u;
}

static inline port_v4sf port_vmx_u32_to_f32(port_v4su u)
{
    port_v4sf r;
    __builtin_memcpy(&r, &u, 16);
    return r;
}

static inline port_v4su port_vmx_i32_to_u32(port_v4si s)
{
    port_v4su u;
    __builtin_memcpy(&u, &s, 16);
    return u;
}

static inline unsigned int port_vmx_cr6(port_v4su mask)
{
    int m = __builtin_ia32_movmskps(port_vmx_u32_to_f32(mask));
    return (m == 0xf) ? 0x80u : ((m == 0) ? 0x20u : 0u);
}

/* Unaligned / element loads & stores.
 * Launder aligned-down pointers through an empty volatile asm with a memory clobber so LLVM
 * keeps preceding stack-alloca stores live and cannot fold aligned-down loads/stores on small
 * stack objects as out-of-bounds UB. */
static inline __vector4 port_vmx_lvewx(const void *base, int offset)
{
    __SIZE_TYPE__ ea = (__SIZE_TYPE__)((const unsigned char *)base + offset) & ~(__SIZE_TYPE__)3u;
    const void *ptr = (const void *)ea;
    __asm__ volatile("" : "+r"(ptr) : : "memory");
    unsigned int word;
    __builtin_memcpy(&word, ptr, 4);
    port_v4su out = { word, word, word, word };
    return port_vmx_from_v4su(out);
}

static inline void port_vmx_stvewx(__vector4 vSrc, void *base, int offset)
{
    __SIZE_TYPE__ ea = (__SIZE_TYPE__)((unsigned char *)base + offset) & ~(__SIZE_TYPE__)3u;
    unsigned int lane = (unsigned int)((ea >> 2) & 3u);
    void *ptr = (void *)ea;
    __asm__ volatile("" : "+r"(ptr) : : "memory");
    __builtin_memcpy(ptr, &vSrc.u[lane], 4);
    __asm__ volatile("" : : : "memory");
}

static inline __vector4 port_vmx_lvlx(const void *base, int offset)
{
    const unsigned char *p = (const unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((__SIZE_TYPE__)p & 15u);
    const unsigned char *aligned = p - sh;
    __asm__ volatile("" : "+r"(aligned) : : "memory");
    unsigned __int128 v;
    __builtin_memcpy(&v, aligned, 16);
    v >>= (sh * 8u);
    __vector4 out;
    __builtin_memcpy(&out, &v, 16);
    return out;
}

static inline __vector4 port_vmx_lvrx(const void *base, int offset)
{
    const unsigned char *p = (const unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((__SIZE_TYPE__)p & 15u);
    __vector4 out;
    if (sh == 0u) {
        port_v4sf z = { 0.0f, 0.0f, 0.0f, 0.0f };
        __builtin_memcpy(&out, &z, 16);
        return out;
    }
    const unsigned char *aligned = p - sh;
    __asm__ volatile("" : "+r"(aligned) : : "memory");
    unsigned __int128 v;
    __builtin_memcpy(&v, aligned, 16);
    v <<= ((16u - sh) * 8u);
    __builtin_memcpy(&out, &v, 16);
    return out;
}

/* Floating-point arithmetic. */
static inline __vector4 port_vmx_vaddfp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4sf(port_vmx_to_v4sf(VRA) + port_vmx_to_v4sf(VRB));
}

static inline __vector4 port_vmx_vsubfp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4sf(port_vmx_to_v4sf(VRA) - port_vmx_to_v4sf(VRB));
}

static inline __vector4 port_vmx_vmulfp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4sf(port_vmx_to_v4sf(VRA) * port_vmx_to_v4sf(VRB));
}

static inline __vector4 port_vmx_vmaddfp(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
    return port_vmx_from_v4sf(port_vmx_to_v4sf(VRA) * port_vmx_to_v4sf(VRB) + port_vmx_to_v4sf(VRC));
}

static inline __vector4 port_vmx_vnmsubfp(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
    return port_vmx_from_v4sf(-(port_vmx_to_v4sf(VRA) * port_vmx_to_v4sf(VRB) - port_vmx_to_v4sf(VRC)));
}

static inline __vector4 port_vmx_vmsum3fp(__vector4 VRA, __vector4 VRB)
{
    port_v4sf p = port_vmx_to_v4sf(VRA) * port_vmx_to_v4sf(VRB);
    port_v4sf p0 = __builtin_shufflevector(p, p, 0, 0, 0, 0);
    port_v4sf p1 = __builtin_shufflevector(p, p, 1, 1, 1, 1);
    port_v4sf p2 = __builtin_shufflevector(p, p, 2, 2, 2, 2);
    return port_vmx_from_v4sf((p0 + p1) + p2);
}

static inline __vector4 port_vmx_vmsum4fp(__vector4 VRA, __vector4 VRB)
{
    port_v4sf p = port_vmx_to_v4sf(VRA) * port_vmx_to_v4sf(VRB);
    port_v4sf s1 = p + __builtin_shufflevector(p, p, 1, 0, 3, 2);
    port_v4sf dot = s1 + __builtin_shufflevector(s1, s1, 2, 3, 0, 1);
    return port_vmx_from_v4sf(dot);
}

static inline __vector4 port_vmx_vmaxfp(__vector4 VRA, __vector4 VRB)
{
    port_v4sf a = port_vmx_to_v4sf(VRA);
    port_v4sf b = port_vmx_to_v4sf(VRB);
    port_v4su m1 = port_vmx_f32_to_u32(__builtin_ia32_maxps(a, b));
    port_v4su m2 = port_vmx_f32_to_u32(__builtin_ia32_maxps(b, a));
    port_v4su res = m1 & m2; /* +0.0 beats -0.0 when a == b */
    port_v4su nan_a = port_vmx_i32_to_u32(a != a);
    port_v4su nan_b = port_vmx_i32_to_u32(b != b);
    port_v4su qnan_bit = { 0x00400000u, 0x00400000u, 0x00400000u, 0x00400000u };
    port_v4su qa = port_vmx_f32_to_u32(a) | qnan_bit;
    port_v4su qb = port_vmx_f32_to_u32(b) | qnan_bit;
    res = (res & ~nan_b) | (qb & nan_b);
    res = (res & ~nan_a) | (qa & nan_a);
    return port_vmx_from_v4sf(port_vmx_u32_to_f32(res));
}

static inline __vector4 port_vmx_vminfp(__vector4 VRA, __vector4 VRB)
{
    port_v4sf a = port_vmx_to_v4sf(VRA);
    port_v4sf b = port_vmx_to_v4sf(VRB);
    port_v4su m1 = port_vmx_f32_to_u32(__builtin_ia32_minps(a, b));
    port_v4su m2 = port_vmx_f32_to_u32(__builtin_ia32_minps(b, a));
    port_v4su res = m1 | m2; /* -0.0 beats +0.0 when a == b */
    port_v4su nan_a = port_vmx_i32_to_u32(a != a);
    port_v4su nan_b = port_vmx_i32_to_u32(b != b);
    port_v4su qnan_bit = { 0x00400000u, 0x00400000u, 0x00400000u, 0x00400000u };
    port_v4su qa = port_vmx_f32_to_u32(a) | qnan_bit;
    port_v4su qb = port_vmx_f32_to_u32(b) | qnan_bit;
    res = (res & ~nan_b) | (qb & nan_b);
    res = (res & ~nan_a) | (qa & nan_a);
    return port_vmx_from_v4sf(port_vmx_u32_to_f32(res));
}

static inline __vector4 port_vmx_vrefp(__vector4 VRB)
{
    port_v4sf one = { 1.0f, 1.0f, 1.0f, 1.0f };
    return port_vmx_from_v4sf(one / port_vmx_to_v4sf(VRB));
}

static inline __vector4 port_vmx_vrsqrtefp(__vector4 VRB)
{
    port_v4sf x = port_vmx_to_v4sf(VRB);
    port_v4sf one = { 1.0f, 1.0f, 1.0f, 1.0f };
    port_v4sf zero = { 0.0f, 0.0f, 0.0f, 0.0f };
    port_v4sf r = one / __builtin_elementwise_sqrt(x);
    port_v4su neg = port_vmx_i32_to_u32(x < zero);
    port_v4su qnan = { 0x7fc00000u, 0x7fc00000u, 0x7fc00000u, 0x7fc00000u };
    port_v4su ru = port_vmx_f32_to_u32(r);
    ru = (ru & ~neg) | (qnan & neg);
    return port_vmx_from_v4sf(port_vmx_u32_to_f32(ru));
}

/* Bitwise & select. */
static inline __vector4 port_vmx_vand(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) & port_vmx_to_v4su(VRB));
}

static inline __vector4 port_vmx_vandc(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) & ~port_vmx_to_v4su(VRB));
}

static inline __vector4 port_vmx_vor(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) | port_vmx_to_v4su(VRB));
}

static inline __vector4 port_vmx_vxor(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) ^ port_vmx_to_v4su(VRB));
}

static inline __vector4 port_vmx_vnor(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(~(port_vmx_to_v4su(VRA) | port_vmx_to_v4su(VRB)));
}

static inline __vector4 port_vmx_vsel(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
    port_v4su a = port_vmx_to_v4su(VRA);
    port_v4su b = port_vmx_to_v4su(VRB);
    port_v4su m = port_vmx_to_v4su(VRC);
    return port_vmx_from_v4su((a & ~m) | (b & m));
}

/* Shifts, splats, merges & permutes. */
static inline __vector4 port_vmx_vslw(__vector4 VRA, __vector4 VRB)
{
    port_v4su mask = { 31u, 31u, 31u, 31u };
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) << (port_vmx_to_v4su(VRB) & mask));
}

static inline __vector4 port_vmx_vsrw(__vector4 VRA, __vector4 VRB)
{
    port_v4su mask = { 31u, 31u, 31u, 31u };
    return port_vmx_from_v4su(port_vmx_to_v4su(VRA) >> (port_vmx_to_v4su(VRB) & mask));
}

static inline __vector4 port_vmx_vsraw(__vector4 VRA, __vector4 VRB)
{
    port_v4si mask = { 31, 31, 31, 31 };
    return port_vmx_from_v4si(port_vmx_to_v4si(VRA) >> (port_vmx_to_v4si(VRB) & mask));
}

static inline __vector4 port_vmx_vspltisw(int SIM)
{
    int sext = (SIM & 0x10) ? (SIM | ~0x1f) : (SIM & 0x1f);
    unsigned int w = (unsigned int)sext;
    port_v4su out = { w, w, w, w };
    return port_vmx_from_v4su(out);
}

static inline __vector4 port_vmx_vspltw(__vector4 VRB, unsigned int UIM)
{
    port_v4su b = port_vmx_to_v4su(VRB);
    switch (UIM & 3u) {
    case 0: return port_vmx_from_v4su(__builtin_shufflevector(b, b, 0, 0, 0, 0));
    case 1: return port_vmx_from_v4su(__builtin_shufflevector(b, b, 1, 1, 1, 1));
    case 2: return port_vmx_from_v4su(__builtin_shufflevector(b, b, 2, 2, 2, 2));
    default: return port_vmx_from_v4su(__builtin_shufflevector(b, b, 3, 3, 3, 3));
    }
}

static inline __vector4 port_vmx_vmrghw(__vector4 VRA, __vector4 VRB)
{
    port_v4su a = port_vmx_to_v4su(VRA);
    port_v4su b = port_vmx_to_v4su(VRB);
    return port_vmx_from_v4su(__builtin_shufflevector(a, b, 0, 4, 1, 5));
}

static inline __vector4 port_vmx_vmrglw(__vector4 VRA, __vector4 VRB)
{
    port_v4su a = port_vmx_to_v4su(VRA);
    port_v4su b = port_vmx_to_v4su(VRB);
    return port_vmx_from_v4su(__builtin_shufflevector(a, b, 2, 6, 3, 7));
}

static inline __vector4 port_vmx_vpermwi(__vector4 VRB, unsigned int imm)
{
    port_v4su b = port_vmx_to_v4su(VRB);
    port_v4su out = {
        b[(imm >> 6) & 3u],
        b[(imm >> 4) & 3u],
        b[(imm >> 2) & 3u],
        b[imm & 3u]
    };
    return port_vmx_from_v4su(out);
}

static inline __vector4 port_vmx_vrlimi(__vector4 VRT, __vector4 VRB, unsigned int WMASK, unsigned int SHW)
{
    port_v4su t = port_vmx_to_v4su(VRT);
    port_v4su b = port_vmx_to_v4su(VRB);
    unsigned int sh = SHW & 3u;
    port_v4su rot = {
        b[sh],
        b[(1u + sh) & 3u],
        b[(2u + sh) & 3u],
        b[(3u + sh) & 3u]
    };
    port_v4su mask = {
        (WMASK & 8u) ? 0xffffffffu : 0u,
        (WMASK & 4u) ? 0xffffffffu : 0u,
        (WMASK & 2u) ? 0xffffffffu : 0u,
        (WMASK & 1u) ? 0xffffffffu : 0u
    };
    return port_vmx_from_v4su((t & ~mask) | (rot & mask));
}

/* Compares (non-R and R variants). */
static inline __vector4 port_vmx_vcmpgtfp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) > port_vmx_to_v4sf(VRB)));
}

static inline __vector4 port_vmx_vcmpgefp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) >= port_vmx_to_v4sf(VRB)));
}

static inline __vector4 port_vmx_vcmpeqfp(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) == port_vmx_to_v4sf(VRB)));
}

static inline __vector4 port_vmx_vcmpequw(__vector4 VRA, __vector4 VRB)
{
    return port_vmx_from_v4su(port_vmx_i32_to_u32(port_vmx_to_v4su(VRA) == port_vmx_to_v4su(VRB)));
}

static inline __vector4 port_vmx_vcmpgtfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
    port_v4su mask = port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) > port_vmx_to_v4sf(VRB));
    if (CR)
        *CR = port_vmx_cr6(mask);
    return port_vmx_from_v4su(mask);
}

static inline __vector4 port_vmx_vcmpgefpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
    port_v4su mask = port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) >= port_vmx_to_v4sf(VRB));
    if (CR)
        *CR = port_vmx_cr6(mask);
    return port_vmx_from_v4su(mask);
}

static inline __vector4 port_vmx_vcmpeqfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
    port_v4su mask = port_vmx_i32_to_u32(port_vmx_to_v4sf(VRA) == port_vmx_to_v4sf(VRB));
    if (CR)
        *CR = port_vmx_cr6(mask);
    return port_vmx_from_v4su(mask);
}

static inline __vector4 port_vmx_vcmpequwR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
    port_v4su mask = port_vmx_i32_to_u32(port_vmx_to_v4su(VRA) == port_vmx_to_v4su(VRB));
    if (CR)
        *CR = port_vmx_cr6(mask);
    return port_vmx_from_v4su(mask);
}

static inline __vector4 port_vmx_vsldoi(__vector4 VRA, __vector4 VRB, unsigned int SHB)
{
    const unsigned int sh = SHB & 15u;
    if ((sh & 3u) == 0u) {
        port_v4su a = port_vmx_to_v4su(VRA);
        port_v4su b = port_vmx_to_v4su(VRB);
        switch (sh) {
        case 0u:  return VRA;
        case 4u:  return port_vmx_from_v4su(__builtin_shufflevector(a, b, 1, 2, 3, 4));
        case 8u:  return port_vmx_from_v4su(__builtin_shufflevector(a, b, 2, 3, 4, 5));
        default:  return port_vmx_from_v4su(__builtin_shufflevector(a, b, 3, 4, 5, 6));
        }
    }
    unsigned char bytes[32];
    for (unsigned int w = 0u; w < 4u; w++) {
        unsigned int x = __builtin_bswap32(VRA.u[w]);
        unsigned int y = __builtin_bswap32(VRB.u[w]);
        __builtin_memcpy(bytes + 4u * w, &x, 4);
        __builtin_memcpy(bytes + 16u + 4u * w, &y, 4);
    }
    __vector4 out;
    for (unsigned int w = 0u; w < 4u; w++) {
        unsigned int word;
        __builtin_memcpy(&word, bytes + sh + 4u * w, 4);
        out.u[w] = __builtin_bswap32(word);
    }
    return out;
}

static inline __vector4 port_vmx_vcfsx(__vector4 VRB, unsigned short scale)
{
    unsigned int fbits = (127u - (unsigned int)(scale & 31u)) << 23;
    float factor;
    __builtin_memcpy(&factor, &fbits, 4);
    port_v4sf fvec = { factor, factor, factor, factor };
    return port_vmx_from_v4sf(__builtin_convertvector(port_vmx_to_v4si(VRB), port_v4sf) * fvec);
}

static inline __vector4 port_vmx_vctsxs(__vector4 VRB, unsigned int scale)
{
    typedef float port_v4sf_a16 __attribute__((vector_size(16)));
    unsigned int fbits = (127u + (scale & 31u)) << 23;
    float factor;
    __builtin_memcpy(&factor, &fbits, 4);
    port_v4sf fvec = { factor, factor, factor, factor };
    port_v4sf s = port_vmx_to_v4sf(VRB) * fvec;
    port_v4sf max_f = { 2147483648.0f, 2147483648.0f, 2147483648.0f, 2147483648.0f };
    port_v4su ge_max = port_vmx_i32_to_u32(s >= max_f);
    port_v4su not_nan = port_vmx_i32_to_u32(s == s);
    port_v4sf_a16 sa;
    __builtin_memcpy(&sa, &s, 16);
    port_v4su res = port_vmx_i32_to_u32(__builtin_ia32_cvttps2dq(sa));
    port_v4su int_max = { 0x7fffffffu, 0x7fffffffu, 0x7fffffffu, 0x7fffffffu };
    res = ((res & ~ge_max) | (int_max & ge_max)) & not_nan;
    return port_vmx_from_v4su(res);
}

#ifndef PORT_VMX128_IMPL

#define __vaddfp(a, b) port_vmx_vaddfp((a), (b))
#define __vsubfp(a, b) port_vmx_vsubfp((a), (b))
#define __vmulfp(a, b) port_vmx_vmulfp((a), (b))
#define __vmaddfp(a, b, c) port_vmx_vmaddfp((a), (b), (c))
#define __vnmsubfp(a, b, c) port_vmx_vnmsubfp((a), (b), (c))
#define __vmsum3fp(a, b) port_vmx_vmsum3fp((a), (b))
#define __vmsum4fp(a, b) port_vmx_vmsum4fp((a), (b))
#define __vmaxfp(a, b) port_vmx_vmaxfp((a), (b))
#define __vminfp(a, b) port_vmx_vminfp((a), (b))
#define __vrefp(b) port_vmx_vrefp((b))
#define __vrsqrtefp(b) port_vmx_vrsqrtefp((b))

#define __vand(a, b) port_vmx_vand((a), (b))
#define __vandc(a, b) port_vmx_vandc((a), (b))
#define __vor(a, b) port_vmx_vor((a), (b))
#define __vxor(a, b) port_vmx_vxor((a), (b))
#define __vnor(a, b) port_vmx_vnor((a), (b))
#define __vsel(a, b, c) port_vmx_vsel((a), (b), (c))

#define __vslw(a, b) port_vmx_vslw((a), (b))
#define __vsrw(a, b) port_vmx_vsrw((a), (b))
#define __vsraw(a, b) port_vmx_vsraw((a), (b))
#define __vspltisw(sim) port_vmx_vspltisw((sim))
#define __vspltw(b, uim) port_vmx_vspltw((b), (uim))
#define __vmrghw(a, b) port_vmx_vmrghw((a), (b))
#define __vmrglw(a, b) port_vmx_vmrglw((a), (b))
#define __vpermwi(b, uim) port_vmx_vpermwi((b), (uim))
#define __vrlimi(t, b, wmask, shw) port_vmx_vrlimi((t), (b), (wmask), (shw))
#define __vsldoi(a, b, shb) port_vmx_vsldoi((a), (b), (shb))
#define __vcfsx(b, scale) port_vmx_vcfsx((b), (scale))
#define __vctsxs(b, scale) port_vmx_vctsxs((b), (scale))

#define __vcmpgtfp(a, b) port_vmx_vcmpgtfp((a), (b))
#define __vcmpgefp(a, b) port_vmx_vcmpgefp((a), (b))
#define __vcmpeqfp(a, b) port_vmx_vcmpeqfp((a), (b))
#define __vcmpequw(a, b) port_vmx_vcmpequw((a), (b))
#define __vcmpgtfpR(a, b, cr) port_vmx_vcmpgtfpR((a), (b), (cr))
#define __vcmpgefpR(a, b, cr) port_vmx_vcmpgefpR((a), (b), (cr))
#define __vcmpeqfpR(a, b, cr) port_vmx_vcmpeqfpR((a), (b), (cr))
#define __vcmpequwR(a, b, cr) port_vmx_vcmpequwR((a), (b), (cr))

#define __lvewx(base, offset) port_vmx_lvewx((base), (offset))
#define __stvewx(v, base, offset) port_vmx_stvewx((v), (base), (offset))
#define __lvlx(base, offset) port_vmx_lvlx((base), (offset))
#define __lvrx(base, offset) port_vmx_lvrx((base), (offset))

#undef __loadunalignedvector
#undef __storeunalignedvector
#define __loadunalignedvector(ptr) port_vmx_load16((ptr))
#define __storeunalignedvector(v, ptr) port_vmx_store16((ptr), (v))

#endif /* !PORT_VMX128_IMPL */
#endif /* HALO3_64BIT */

#endif
