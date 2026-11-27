// port/sdk/vmx: Xbox 360 VMX128 vector intrinsics (<vectorintrinsics.h>) for 32-bit MS-ABI game and Havok code.
//
// Compiled as part of halo3_sdk so struct __vector4 layout and 32-bit MS-ABI struct-return conventions match tu/
// identically on both Linux (i686-pc-windows-msvc-elf) and Windows (i686-pc-windows-msvc).
#define PORT_VMX128_IMPL 1
#include <xtl.h>
#include <vectorintrinsics.h>

extern "C" void *port_vmx_reg_bank(void);

namespace {

static inline unsigned int float_to_bits(float f)
{
    unsigned int u;
    __builtin_memcpy(&u, &f, 4);
    return u;
}

static inline float bits_to_float(unsigned int u)
{
    float f;
    __builtin_memcpy(&f, &u, 4);
    return f;
}

// Xbox 360 extended-range float16 (s1e5m10, exponents 1..31 all normal up to 131008.0f, no NaN/Inf).
static inline unsigned short float_to_xbox_half(float value)
{
    unsigned int bits = float_to_bits(value);
    unsigned int abs_bits = bits & 0x7fffffffu;
    unsigned int sign = (bits & 0x80000000u) >> 16;

    if (abs_bits > 0x47ffefffu)
        return (unsigned short)(sign | 0x7fffu);

    if (abs_bits < 0x38800000u) {
        unsigned int shift = 113u - (abs_bits >> 23);
        if (shift >= 25u)
            abs_bits = 0u;
        else
            abs_bits = (0x800000u | (abs_bits & 0x7fffffu)) >> shift;
    } else {
        abs_bits += 0xc8000000u;
    }

    return (unsigned short)(sign | ((abs_bits + 0x0fffu + ((abs_bits >> 13) & 1u)) >> 13));
}

static inline float xbox_half_to_float(unsigned short value)
{
    unsigned int mantissa = (unsigned int)(value & 0x03ffu);
    unsigned int exponent;
    if ((value & 0x7c00u) != 0) {
        exponent = (unsigned int)((value >> 10) & 0x1fu);
    } else if (mantissa != 0) {
        exponent = 1u;
        do {
            exponent--;
            mantissa <<= 1;
        } while ((mantissa & 0x0400u) == 0);
        mantissa &= 0x03ffu;
    } else {
        exponent = (unsigned int)-112;
    }
    unsigned int out = ((unsigned int)(value & 0x8000u) << 16) | ((exponent + 112u) << 23) | (mantissa << 13);
    return bits_to_float(out);
}

// Extracts signed integer encoded as (3.0f + k * 2^-22), saturating outside [min_val, max_val].
static inline int unpack_biased_signed(float f, int min_val, int max_val)
{
    if (f != f)
        return 0;
    if (f <= 2.0f)
        return min_val;
    if (f >= 4.0f)
        return max_val;
    int delta = (int)(float_to_bits(f) - 0x40400000u);
    if (delta < min_val) return min_val;
    if (delta > max_val) return max_val;
    return delta;
}

// Extracts unsigned integer encoded as (3.0f + k * 2^-22), saturating outside [0, max_val].
static inline unsigned int unpack_biased_unsigned(float f, unsigned int max_val)
{
    if (f != f || f <= 3.0f)
        return 0u;
    if (f >= 4.0f)
        return max_val;
    unsigned int delta = float_to_bits(f) - 0x40400000u;
    return delta > max_val ? max_val : delta;
}

#ifndef HALO3_64BIT
static inline unsigned int compute_cr6(const __vector4 &mask)
{
    bool all_true = (mask.u[0] & mask.u[1] & mask.u[2] & mask.u[3]) == 0xffffffffu;
    bool all_false = (mask.u[0] | mask.u[1] | mask.u[2] | mask.u[3]) == 0u;
    return all_true ? 0x80u : (all_false ? 0x20u : 0u);
}

// VMX128 runs in non-Java mode (VSCR[NJ] = 1): denormal inputs and results are flushed to zero, keeping the sign.
// The Xenon scalar FPU does not flush, so only the vector intrinsics apply this (Xenia: ppc_emit_altivec.cc, NJ).
static inline float vmx_flush(float x)
{
    unsigned int u = float_to_bits(x);
    return ((u & 0x7f800000u) == 0u) ? bits_to_float(u & 0x80000000u) : x;
}

// vmaddfp / vnmsubfp are fused (one rounding). The product of two floats is exact in a double; the double sum is
// rounded to odd (TwoSum gives its exact error), so the final round to float is the correctly rounded fused result
// with no double-rounding error. This avoids a libm fmaf call on i686 without FMA hardware.
static inline float vmx_fma(float a, float b, float c)
{
    const double p = (double)vmx_flush(a) * (double)vmx_flush(b);
    const double q = (double)vmx_flush(c);
    double s = p + q;
    const double bv = s - p;
    const double err = (p - (s - bv)) + (q - bv);
    if (err != 0.0 && s == s) {
        unsigned long long bits;
        __builtin_memcpy(&bits, &s, 8);
        if ((bits & 1u) == 0u) {
            bits += ((err > 0.0) == (s > 0.0)) ? 1u : (unsigned long long)-1;
            __builtin_memcpy(&s, &bits, 8);
        }
    }
    return (float)s;
}

// vmaxfp / vminfp: a NaN operand gives that NaN made quiet; +0 orders above -0 (AltiVec PEM; Xenia vmaxfp).
static inline float vmx_max(float a, float b)
{
    if (a != a) return bits_to_float(float_to_bits(a) | 0x00400000u);
    if (b != b) return bits_to_float(float_to_bits(b) | 0x00400000u);
    if (a == b) return (float_to_bits(a) & 0x80000000u) ? b : a;
    return a > b ? a : b;
}

static inline float vmx_min(float a, float b)
{
    if (a != a) return bits_to_float(float_to_bits(a) | 0x00400000u);
    if (b != b) return bits_to_float(float_to_bits(b) | 0x00400000u);
    if (a == b) return (float_to_bits(a) & 0x80000000u) ? a : b;
    return a < b ? a : b;
}
#endif

static inline float round_nearest_even(float x)
{
    if (x != x)
        return x;
    float ax = x < 0.0f ? -x : x;
    if (ax >= 8388608.0f)
        return x;
    float magic = x < 0.0f ? -8388608.0f : 8388608.0f;
    float r = (x + magic) - magic;
    return (r == 0.0f && (float_to_bits(x) & 0x80000000u) != 0) ? -0.0f : r;
}

} // namespace

extern "C" {

// ---------------------------------------------------------------------------------------------------------------
// Vector loads & stores.

__vector4 __lvewx(const void *base, int offset)
{
#ifdef HALO3_64BIT
    return port_vmx_lvewx(base, offset);
#else
    size_t ea = ((size_t)base + (size_t)offset) & ~(size_t)3u;
    unsigned int word;
    __builtin_memcpy(&word, (const void *)ea, 4);
    __vector4 out;
    out.u[0] = word;
    out.u[1] = word;
    out.u[2] = word;
    out.u[3] = word;
    return out;
#endif
}

void __stvewx(__vector4 vSrc, void *base, int offset)
{
#ifdef HALO3_64BIT
    port_vmx_stvewx(vSrc, base, offset);
#else
    size_t ea = ((size_t)base + (size_t)offset) & ~(size_t)3u;
    unsigned int lane = (unsigned int)((ea >> 2) & 3u);
    __builtin_memcpy((void *)ea, &vSrc.u[lane], 4);
#endif
}

// lvlx / lvrx / stvlx / stvrx move the part of a 16-byte vector on one side of a 16-byte boundary. Their sizes vary
// with the address, and a variable-size __builtin_memcpy is a C library call (with -fno-builtin, so is the memset of
// the rest), two calls per load in Havok's and the renderer's unaligned vector loads. The loads read the whole aligned
// block the bytes are in (as the hardware does; it never crosses a page, so it is as readable as the bytes asked for)
// and shift it through a 32-byte buffer; the stores write the bytes in fixed-size pieces. Results are the same bytes.

// n bytes (0..16) from s to d, in fixed-size moves.
static inline void store_bytes(unsigned char *d, const unsigned char *s, unsigned int n)
{
    if (n & 16u) {
        __builtin_memcpy(d, s, 16);
        return;
    }
    if (n & 8u) {
        __builtin_memcpy(d, s, 8);
        d += 8;
        s += 8;
    }
    if (n & 4u) {
        __builtin_memcpy(d, s, 4);
        d += 4;
        s += 4;
    }
    if (n & 2u) {
        __builtin_memcpy(d, s, 2);
        d += 2;
        s += 2;
    }
    if (n & 1u)
        *d = *s;
}

__vector4 __lvlx(const void *base, int offset)
{
#ifdef HALO3_64BIT
    return port_vmx_lvlx(base, offset);
#else
    const unsigned char *p = (const unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((size_t)p & 15u);
    __vector4 out;
    if (sh == 0) {
        __builtin_memcpy(&out, p, 16);
        return out;
    }
    unsigned char buffer[32]; // the block, then zeros: out = its bytes from p on
    __builtin_memcpy(buffer, p - sh, 16);
    __builtin_memset(buffer + 16, 0, 16);
    __builtin_memcpy(&out, buffer + sh, 16);
    return out;
#endif
}

__vector4 __lvrx(const void *base, int offset)
{
#ifdef HALO3_64BIT
    return port_vmx_lvrx(base, offset);
#else
    const unsigned char *p = (const unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((size_t)p & 15u);
    __vector4 out = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (sh == 0)
        return out;
    unsigned char buffer[32]; // zeros, then the block: out ends with its bytes before p
    __builtin_memset(buffer, 0, 16);
    __builtin_memcpy(buffer + 16, p - sh, 16);
    __builtin_memcpy(&out, buffer + sh, 16);
    return out;
#endif
}

void __stvlx(__vector4 vSrc, void *base, int offset)
{
    unsigned char *p = (unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((size_t)p & 15u);
    store_bytes(p, (const unsigned char *)&vSrc, 16u - sh);
}

void __stvrx(__vector4 vSrc, void *base, int offset)
{
    unsigned char *p = (unsigned char *)base + offset;
    unsigned int sh = (unsigned int)((size_t)p & 15u);
    store_bytes(p - sh, (const unsigned char *)&vSrc + (16u - sh), sh);
}

__vector4 __vsplth(__vector4 VRB, unsigned int UIM)
{
    unsigned short h[8];
    __builtin_memcpy(h, &VRB, 16);
    unsigned short val = h[UIM & 7u];
    for (int i = 0; i < 8; i++)
        h[i] = val;
    __vector4 out;
    __builtin_memcpy(&out, h, 16);
    return out;
}

void __stvehx(__vector4 vSrc, void *base, int offset)
{
    size_t ea = ((size_t)base + (size_t)offset) & ~(size_t)1u;
    unsigned short h[8];
    __builtin_memcpy(h, &vSrc, 16);
    unsigned int lane = (unsigned int)((ea >> 1) & 7u);
    __builtin_memcpy((void *)ea, &h[lane], 2);
}

// ---------------------------------------------------------------------------------------------------------------
// Floating-point arithmetic, FMA, dot products, estimates & rounding.

__vector4 __vaddfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vaddfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_flush(vmx_flush(VRA.v[i]) + vmx_flush(VRB.v[i]));
    return out;
#endif
}

__vector4 __vsubfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vsubfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_flush(vmx_flush(VRA.v[i]) - vmx_flush(VRB.v[i]));
    return out;
#endif
}

__vector4 __vmulfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmulfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_flush(vmx_flush(VRA.v[i]) * vmx_flush(VRB.v[i]));
    return out;
#endif
}

__vector4 __vmaddfp(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
#ifdef HALO3_64BIT
    return port_vmx_vmaddfp(VRA, VRB, VRC);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_flush(vmx_fma(VRA.v[i], VRB.v[i], VRC.v[i]));
    return out;
#endif
}

__vector4 __vnmsubfp(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
#ifdef HALO3_64BIT
    return port_vmx_vnmsubfp(VRA, VRB, VRC);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_flush(-vmx_fma(VRA.v[i], VRB.v[i], -VRC.v[i]));
    return out;
#endif
}

__vector4 __vmaxfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmaxfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_max(VRA.v[i], VRB.v[i]);
    return out;
#endif
}

__vector4 __vminfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vminfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = vmx_min(VRA.v[i], VRB.v[i]);
    return out;
#endif
}

__vector4 __vmsum3fp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmsum3fp(VRA, VRB);
#else
    float dot = VRA.v[0] * VRB.v[0] + VRA.v[1] * VRB.v[1] + VRA.v[2] * VRB.v[2];
    __vector4 out = { dot, dot, dot, dot };
    return out;
#endif
}

__vector4 __vmsum4fp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmsum4fp(VRA, VRB);
#else
    float dot = VRA.v[0] * VRB.v[0] + VRA.v[1] * VRB.v[1] + VRA.v[2] * VRB.v[2] + VRA.v[3] * VRB.v[3];
    __vector4 out = { dot, dot, dot, dot };
    return out;
#endif
}

__vector4 __vrefp(__vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vrefp(VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = 1.0f / VRB.v[i];
    return out;
#endif
}

__vector4 __vrsqrtefp(__vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vrsqrtefp(VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++) {
        float x = VRB.v[i];
        if (x == 0.0f) {
            unsigned int sign = float_to_bits(x) & 0x80000000u;
            out.u[i] = sign | 0x7f800000u;
        } else if (x < 0.0f) {
            out.u[i] = 0x7fc00000u;
        } else {
            // sqrtss. __builtin_sqrtf is a library call here (the double sqrt: math.h maps sqrtf to it). Same value: the
            // correctly rounded double root rounded to float is the correctly rounded float root (53 >= 2 * 24 + 2 bits).
            out.v[i] = 1.0f / __builtin_elementwise_sqrt(x);
        }
    }
    return out;
#endif
}

__vector4 __vrfin(__vector4 VRB)
{
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = round_nearest_even(VRB.v[i]);
    return out;
}

__vector4 __vrfiz(__vector4 VRB)
{
    __vector4 out;
    for (int i = 0; i < 4; i++) {
        float x = VRB.v[i];
        float ax = x < 0.0f ? -x : x;
        if (x != x || ax >= 8388608.0f) {
            out.v[i] = x;
        } else {
            int t = (int)x;
            float r = (float)t;
            out.v[i] = (r == 0.0f && (float_to_bits(x) & 0x80000000u) != 0) ? -0.0f : r;
        }
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------
// Fixed-point <-> float conversions.

__vector4 __vcfsx(__vector4 VRB, unsigned short scale)
{
#ifdef HALO3_64BIT
    return port_vmx_vcfsx(VRB, scale);
#else
    float factor = bits_to_float((127u - (unsigned int)(scale & 31u)) << 23);
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = (float)((int)VRB.u[i]) * factor;
    return out;
#endif
}

__vector4 __vcfux(__vector4 VRB, unsigned short scale)
{
    float factor = bits_to_float((127u - (unsigned int)(scale & 31u)) << 23);
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.v[i] = (float)VRB.u[i] * factor;
    return out;
}

__vector4 __vctsxs(__vector4 VRB, unsigned int scale)
{
#ifdef HALO3_64BIT
    return port_vmx_vctsxs(VRB, scale);
#else
    float factor = bits_to_float((127u + (scale & 31u)) << 23);
    __vector4 out;
    for (int i = 0; i < 4; i++) {
        double scaled = (double)VRB.v[i] * (double)factor;
        int res;
        if (scaled != scaled)
            res = 0;
        else if (scaled >= 2147483647.0)
            res = 2147483647;
        else if (scaled <= -2147483648.0)
            res = (int)0x80000000u;
        else
            res = (int)scaled;
        out.u[i] = (unsigned int)res;
    }
    return out;
#endif
}

__vector4 __vctuxs(__vector4 VRB, unsigned int scale)
{
    float factor = bits_to_float((127u + (scale & 31u)) << 23);
    __vector4 out;
    for (int i = 0; i < 4; i++) {
        double scaled = (double)VRB.v[i] * (double)factor;
        unsigned int res;
        if (scaled != scaled || scaled <= 0.0)
            res = 0u;
        else if (scaled >= 4294967295.0)
            res = 0xffffffffu;
        else
            res = (unsigned int)scaled;
        out.u[i] = res;
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------
// Vector comparisons & integer operations.

__vector4 __vcmpeqfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpeqfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.v[i] == VRB.v[i] ? 0xffffffffu : 0u;
    return out;
#endif
}

__vector4 __vcmpgefp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpgefp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.v[i] >= VRB.v[i] ? 0xffffffffu : 0u;
    return out;
#endif
}

__vector4 __vcmpgtfp(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpgtfp(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.v[i] > VRB.v[i] ? 0xffffffffu : 0u;
    return out;
#endif
}

__vector4 __vcmpgefpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpgefpR(VRA, VRB, CR);
#else
    __vector4 out = __vcmpgefp(VRA, VRB);
    if (CR)
        *CR = compute_cr6(out);
    return out;
#endif
}

__vector4 __vcmpgtfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpgtfpR(VRA, VRB, CR);
#else
    __vector4 out = __vcmpgtfp(VRA, VRB);
    if (CR)
        *CR = compute_cr6(out);
    return out;
#endif
}

__vector4 __vcmpeqfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpeqfpR(VRA, VRB, CR);
#else
    __vector4 out = __vcmpeqfp(VRA, VRB);
    if (CR)
        *CR = compute_cr6(out);
    return out;
#endif
}

__vector4 __vcmpequw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpequw(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] == VRB.u[i] ? 0xffffffffu : 0u;
    return out;
#endif
}

__vector4 __vcmpequwR(__vector4 VRA, __vector4 VRB, unsigned int *CR)
{
#ifdef HALO3_64BIT
    return port_vmx_vcmpequwR(VRA, VRB, CR);
#else
    __vector4 out = __vcmpequw(VRA, VRB);
    if (CR)
        *CR = compute_cr6(out);
    return out;
#endif
}

__vector4 __vcmpgtuh(__vector4 VRA, __vector4 VRB)
{
    unsigned short a[8], b[8], r[8];
    __builtin_memcpy(a, &VRA, 16);
    __builtin_memcpy(b, &VRB, 16);
    for (int i = 0; i < 8; i++)
        r[i] = a[i] > b[i] ? 0xffffu : 0u;
    __vector4 out;
    __builtin_memcpy(&out, r, 16);
    return out;
}

__vector4 __vadduhm(__vector4 VRA, __vector4 VRB)
{
    unsigned short a[8], b[8], r[8];
    __builtin_memcpy(a, &VRA, 16);
    __builtin_memcpy(b, &VRB, 16);
    for (int i = 0; i < 8; i++)
        r[i] = (unsigned short)(a[i] + b[i]);
    __vector4 out;
    __builtin_memcpy(&out, r, 16);
    return out;
}

__vector4 __vsubuhm(__vector4 VRA, __vector4 VRB)
{
    unsigned short a[8], b[8], r[8];
    __builtin_memcpy(a, &VRA, 16);
    __builtin_memcpy(b, &VRB, 16);
    for (int i = 0; i < 8; i++)
        r[i] = (unsigned short)(a[i] - b[i]);
    __vector4 out;
    __builtin_memcpy(&out, r, 16);
    return out;
}

__vector4 __vand(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vand(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] & VRB.u[i];
    return out;
#endif
}

__vector4 __vandc(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vandc(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] & ~VRB.u[i];
    return out;
#endif
}

__vector4 __vor(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vor(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] | VRB.u[i];
    return out;
#endif
}

__vector4 __vxor(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vxor(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] ^ VRB.u[i];
    return out;
#endif
}

// vnor (hkVector4Comparison::setNot uses __vnor(a, a)).
__vector4 __vnor(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vnor(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = ~(VRA.u[i] | VRB.u[i]);
    return out;
#endif
}

__vector4 __vsel(__vector4 VRA, __vector4 VRB, __vector4 Mask)
{
#ifdef HALO3_64BIT
    return port_vmx_vsel(VRA, VRB, Mask);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = (VRA.u[i] & ~Mask.u[i]) | (VRB.u[i] & Mask.u[i]);
    return out;
#endif
}

__vector4 __vslw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vslw(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] << (VRB.u[i] & 31u);
    return out;
#endif
}

__vector4 __vsrw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vsrw(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = VRA.u[i] >> (VRB.u[i] & 31u);
    return out;
#endif
}

__vector4 __vsraw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vsraw(VRA, VRB);
#else
    __vector4 out;
    for (int i = 0; i < 4; i++)
        out.u[i] = (unsigned int)((int)VRA.u[i] >> (VRB.u[i] & 31u));
    return out;
#endif
}

// ---------------------------------------------------------------------------------------------------------------
// Permute, splat, merge, shift-double & rotate-insert.

__vector4 __vspltisw(int SIM)
{
#ifdef HALO3_64BIT
    return port_vmx_vspltisw(SIM);
#else
    int sext = (SIM & 0x10) ? (SIM | ~0x1f) : (SIM & 0x1f);
    unsigned int w = (unsigned int)sext;
    __vector4 out;
    out.u[0] = w;
    out.u[1] = w;
    out.u[2] = w;
    out.u[3] = w;
    return out;
#endif
}

__vector4 __vspltw(__vector4 VRB, unsigned int UIM)
{
#ifdef HALO3_64BIT
    return port_vmx_vspltw(VRB, UIM);
#else
    unsigned int w = VRB.u[UIM & 3u];
    __vector4 out;
    out.u[0] = w;
    out.u[1] = w;
    out.u[2] = w;
    out.u[3] = w;
    return out;
#endif
}

__vector4 __vmrghw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmrghw(VRA, VRB);
#else
    __vector4 out;
    out.u[0] = VRA.u[0];
    out.u[1] = VRB.u[0];
    out.u[2] = VRA.u[1];
    out.u[3] = VRB.u[1];
    return out;
#endif
}

__vector4 __vmrglw(__vector4 VRA, __vector4 VRB)
{
#ifdef HALO3_64BIT
    return port_vmx_vmrglw(VRA, VRB);
#else
    __vector4 out;
    out.u[0] = VRA.u[2];
    out.u[1] = VRB.u[2];
    out.u[2] = VRA.u[3];
    out.u[3] = VRB.u[3];
    return out;
#endif
}

// vmrghh: halfwords 0..3 of VRA and VRB interleaved, in the memory order the other halfword intrinsics use (h[k] =
// the k-th halfword once stored with stvx): out = A0 B0 A1 B1 A2 B2 A3 B3 (render_structure.cpp decorators).
__vector4 __vmrghh(__vector4 VRA, __vector4 VRB)
{
    unsigned short a[8], b[8], r[8];
    __builtin_memcpy(a, &VRA, 16);
    __builtin_memcpy(b, &VRB, 16);
    for (int i = 0; i < 4; i++) {
        r[2 * i] = a[i];
        r[2 * i + 1] = b[i];
    }
    __vector4 out;
    __builtin_memcpy(&out, r, 16);
    return out;
}

__vector4 __vpermwi(__vector4 VRB, unsigned int imm)
{
#ifdef HALO3_64BIT
    return port_vmx_vpermwi(VRB, imm);
#else
    __vector4 out;
    out.u[0] = VRB.u[(imm >> 6) & 3u];
    out.u[1] = VRB.u[(imm >> 4) & 3u];
    out.u[2] = VRB.u[(imm >> 2) & 3u];
    out.u[3] = VRB.u[imm & 3u];
    return out;
#endif
}

// VRA:VRB as 32 bytes in register (big-endian) byte order, the table vperm and vsldoi pick from: byte k is register
// byte k, the (k % 4)-th byte of word k / 4 counted from its most significant.
static inline void be_bytes(const __vector4 &a, const __vector4 &b, unsigned char bytes[32])
{
    for (unsigned int w = 0; w < 4u; w++) {
        const unsigned int x = __builtin_bswap32(a.u[w]), y = __builtin_bswap32(b.u[w]);
        __builtin_memcpy(bytes + 4u * w, &x, 4);
        __builtin_memcpy(bytes + 16u + 4u * w, &y, 4);
    }
}

__vector4 __vperm(__vector4 VRA, __vector4 VRB, __vector4 VRC)
{
    unsigned char bytes[32];
    be_bytes(VRA, VRB, bytes);
    __vector4 out;
    for (unsigned int w = 0; w < 4u; w++) {
        const unsigned int c = VRC.u[w]; // the selectors of register bytes 4w..4w+3, most significant first
        out.u[w] = (unsigned int)bytes[(c >> 24) & 0x1fu] << 24 | (unsigned int)bytes[(c >> 16) & 0x1fu] << 16 |
                   (unsigned int)bytes[(c >> 8) & 0x1fu] << 8 | (unsigned int)bytes[c & 0x1fu];
    }
    return out;
}

// vsldoi: bytes SHB..SHB+15 of VRA:VRB in register (big-endian) byte order, like __vperm. A memcpy over the host words
// is the same only when SHB is a multiple of 4.
__vector4 __vsldoi(__vector4 VRA, __vector4 VRB, unsigned int SHB)
{
#ifdef HALO3_64BIT
    return port_vmx_vsldoi(VRA, VRB, SHB);
#else
    const unsigned int sh = SHB & 15u;
    unsigned char bytes[32];
    be_bytes(VRA, VRB, bytes);
    __vector4 out;
    for (unsigned int w = 0; w < 4u; w++) {
        unsigned int word;
        __builtin_memcpy(&word, bytes + sh + 4u * w, 4);
        out.u[w] = __builtin_bswap32(word);
    }
    return out;
#endif
}

__vector4 __vrlimi(__vector4 VRT, __vector4 VRB, unsigned int WMASK, unsigned int SHW)
{
#ifdef HALO3_64BIT
    return port_vmx_vrlimi(VRT, VRB, WMASK, SHW);
#else
    unsigned int sh = SHW & 3u;
    __vector4 out = VRT;
    for (unsigned int i = 0; i < 4u; i++) {
        if (WMASK & (0x8u >> i))
            out.u[i] = VRB.u[(i + sh) & 3u];
    }
    return out;
#endif
}

// ---------------------------------------------------------------------------------------------------------------
// D3D / GPU vector pack & unpack (__vpkuwus, __vpkd3d, __vupkd3d).

__vector4 __vpkuwus(__vector4 VRA, __vector4 VRB)
{
    unsigned short h[8];
    for (int i = 0; i < 4; i++) {
        h[i]     = (unsigned short)(VRA.u[i] > 0xffffu ? 0xffffu : VRA.u[i]);
        h[4 + i] = (unsigned short)(VRB.u[i] > 0xffffu ? 0xffffu : VRB.u[i]);
    }
    __vector4 out;
    __builtin_memcpy(&out, h, 16);
    return out;
}

__vector4 __vpkd3d(__vector4 VRT, __vector4 VRB, __VECTOR_PACK_TYPES DT, __VECTOR_PACK_MASK MS, unsigned int SHW)
{
    __vector4 tmp = { 0.0f, 0.0f, 0.0f, 0.0f };
    switch (DT) {
    case VPACK_D3DCOLOR: {
        unsigned int r = unpack_biased_unsigned(VRB.v[0], 255u);
        unsigned int g = unpack_biased_unsigned(VRB.v[1], 255u);
        unsigned int b = unpack_biased_unsigned(VRB.v[2], 255u);
        unsigned int a = unpack_biased_unsigned(VRB.v[3], 255u);
        tmp.u[3] = (a << 24) | (r << 16) | (g << 8) | b;
        break;
    }
    case VPACK_NORMSHORT2: {
        short s[2];
        s[0] = (short)unpack_biased_signed(VRB.v[0], -32767, 32767);
        s[1] = (short)unpack_biased_signed(VRB.v[1], -32767, 32767);
        __builtin_memcpy(&tmp.u[3], s, 4);
        break;
    }
    case VPACK_NORMPACKED32: {
        int x = unpack_biased_signed(VRB.v[0], -511, 511);
        int y = unpack_biased_signed(VRB.v[1], -511, 511);
        int z = unpack_biased_signed(VRB.v[2], -511, 511);
        unsigned int w = unpack_biased_unsigned(VRB.v[3], 3u);
        tmp.u[3] = (w << 30) | (((unsigned int)z & 0x3ffu) << 20) | (((unsigned int)y & 0x3ffu) << 10) | ((unsigned int)x & 0x3ffu);
        break;
    }
    case VPACK_FLOAT16_2: {
        unsigned short h[2];
        h[0] = float_to_xbox_half(VRB.v[0]);
        h[1] = float_to_xbox_half(VRB.v[1]);
        __builtin_memcpy(&tmp.u[3], h, 4);
        break;
    }
    case VPACK_NORMSHORT4: {
        short s[4];
        for (int i = 0; i < 4; i++)
            s[i] = (short)unpack_biased_signed(VRB.v[i], -32767, 32767);
        __builtin_memcpy(&tmp.u[2], s, 8);
        break;
    }
    case VPACK_FLOAT16_4: {
        unsigned short h[4];
        for (int i = 0; i < 4; i++)
            h[i] = float_to_xbox_half(VRB.v[i]);
        __builtin_memcpy(&tmp.u[2], h, 8);
        break;
    }
    case VPACK_NORMPACKED64: {
        int x = unpack_biased_signed(VRB.v[0], -524287, 524287);
        int y = unpack_biased_signed(VRB.v[1], -524287, 524287);
        int z = unpack_biased_signed(VRB.v[2], -524287, 524287);
        unsigned int w = unpack_biased_unsigned(VRB.v[3], 15u);
        unsigned long long v64 = ((unsigned long long)w << 60)
            | (((unsigned long long)(unsigned int)z & 0xfffffull) << 40)
            | (((unsigned long long)(unsigned int)y & 0xfffffull) << 20)
            | ((unsigned long long)(unsigned int)x & 0xfffffull);
        __builtin_memcpy(&tmp.u[2], &v64, 8);
        break;
    }
    }

    unsigned int sh = SHW & 3u;
    unsigned int wmask = 0u;
    if (MS == VPACK_32) {
        static const unsigned int k_mask32[4] = { 0x1u, 0x2u, 0x4u, 0x8u };
        wmask = k_mask32[sh];
    } else if (MS == VPACK_64LO) {
        static const unsigned int k_mask64lo[4] = { 0x3u, 0x6u, 0xcu, 0x8u };
        wmask = k_mask64lo[sh];
    } else if (MS == VPACK_64HI) {
        static const unsigned int k_mask64hi[4] = { 0x3u, 0x6u, 0xcu, 0x1u };
        wmask = k_mask64hi[sh];
    }

    return __vrlimi(VRT, tmp, wmask, sh);
}

__vector4 __vupkd3d(__vector4 VRB, __VECTOR_PACK_TYPES DT)
{
    __vector4 out = { 0.0f, 0.0f, 0.0f, 1.0f };
    switch (DT) {
    case VPACK_D3DCOLOR: {
        unsigned int c = VRB.u[3];
        out.u[0] = 0x3f800000u | ((c >> 16) & 0xffu); // R (1.0f + R * 2^-23)
        out.u[1] = 0x3f800000u | ((c >> 8)  & 0xffu); // G
        out.u[2] = 0x3f800000u | (c         & 0xffu); // B
        out.u[3] = 0x3f800000u | ((c >> 24) & 0xffu); // A
        break;
    }
    case VPACK_NORMSHORT2: {
        short s[2];
        __builtin_memcpy(s, &VRB.u[3], 4);
        out.u[0] = 0x40400000u + (int)s[0];
        out.u[1] = 0x40400000u + (int)s[1];
        out.v[2] = 0.0f;
        out.v[3] = 1.0f;
        break;
    }
    case VPACK_NORMPACKED32: {
        unsigned int p = VRB.u[3];
        int x = ((int)(p << 22)) >> 22;
        int y = ((int)(p << 12)) >> 22;
        int z = ((int)(p << 2))  >> 22;
        unsigned int w = p >> 30;
        out.u[0] = 0x40400000u + x;
        out.u[1] = 0x40400000u + y;
        out.u[2] = 0x40400000u + z;
        out.u[3] = 0x3f800000u | w;
        break;
    }
    case VPACK_FLOAT16_2: {
        unsigned short h[2];
        __builtin_memcpy(h, &VRB.u[3], 4);
        out.v[0] = xbox_half_to_float(h[0]);
        out.v[1] = xbox_half_to_float(h[1]);
        out.v[2] = 0.0f;
        out.v[3] = 1.0f;
        break;
    }
    case VPACK_NORMSHORT4: {
        short s[4];
        __builtin_memcpy(s, &VRB.u[2], 8);
        for (int i = 0; i < 4; i++)
            out.u[i] = 0x40400000u + (int)s[i];
        break;
    }
    case VPACK_FLOAT16_4: {
        unsigned short h[4];
        __builtin_memcpy(h, &VRB.u[2], 8);
        for (int i = 0; i < 4; i++)
            out.v[i] = xbox_half_to_float(h[i]);
        break;
    }
    case VPACK_NORMPACKED64: {
        unsigned long long p;
        __builtin_memcpy(&p, &VRB.u[2], 8);
        int x = ((int)((unsigned int)p << 12)) >> 12;
        int y = ((int)((unsigned int)(p >> 20) << 12)) >> 12;
        int z = ((int)((unsigned int)(p >> 40) << 12)) >> 12;
        unsigned int w = (unsigned int)(p >> 60);
        out.u[0] = 0x40400000u + x;
        out.u[1] = 0x40400000u + y;
        out.u[2] = 0x40400000u + z;
        out.u[3] = 0x3f800000u | w;
        break;
    }
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------
// /QVMXReserve per-thread register file (__VMXGetReg / __VMXSetReg).

__vector4 __VMXGetReg(const int reg)
{
    const __vector4 *bank = (const __vector4 *)port_vmx_reg_bank();
    return bank[reg & 127];
}

void __VMXSetReg(const int reg, __vector4 val)
{
    __vector4 *bank = (__vector4 *)port_vmx_reg_bank();
    bank[reg & 127] = val;
}

} // extern "C"
