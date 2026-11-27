#ifndef INCLUDE_PHYS_MATH_VALUE_TYPES_H
#define INCLUDE_PHYS_MATH_VALUE_TYPES_H

// Havok scalar, vector, matrix and quaternion values. Transform-dependent inline
// methods are defined by hk_math.h after its transform classes.

#include "cseries/crt_xdk.h"
#include <math.h>
#include <stddef.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>

#include "phys/phys_base_types.h"

#include "phys/phys_scalar_types.h"

extern const __vector4 phys_simd_vec_0000;
extern const __vector4 phys_simd_vec_1111;
extern const __vector4 phys_simd_vec_half;
extern const __vector4 phys_simd_vec_1000;
extern const __vector4 phys_simd_vec_0100;
extern const __vector4 phys_simd_vec_0010;
extern const __vector4 phys_simd_vec_3333;
extern const __vector4 phys_simd_vec_0001;
extern const __vector4 phys_simd_vec_minus_half;
class phys_vec4;
typedef const phys_vec4 &phys_vec4_param;

class phys_simd_scalar {
public:
    __vector4 m_simd_scalar;

    inline phys_simd_scalar() {}
    inline phys_simd_scalar(const __vector4 &x) : m_simd_scalar(x) {}
    inline phys_simd_scalar(const float &x) {
        m_simd_scalar = __lvlx(&x, 0);
        m_simd_scalar = __vspltw(m_simd_scalar, 0);
    }
    inline void set(const __vector4 &x) { m_simd_scalar = x; }
    inline void set(const float &x) {
        m_simd_scalar = __lvlx(&x, 0);
        m_simd_scalar = __vspltw(m_simd_scalar, 0);
    }
    __forceinline void set_zero() { m_simd_scalar = phys_simd_vec_0000; }
    inline operator float() const {
        float x;
        __stvewx(m_simd_scalar, &x, 0);
        return x;
    }
    inline __vector4 &get_quad() { return m_simd_scalar; }
    inline const __vector4 &get_quad() const { return m_simd_scalar; }
    static inline phys_simd_scalar create(float a) {
        phys_simd_scalar sr;
        sr.set(a);
        return sr;
    }
};

typedef const phys_simd_scalar &phys_simd_scalar_param;

namespace phys_math {
    float rand_unit();
    float phys_floor(float r);
    int phys_floor_to_int(float r);
    int phys_float_to_int(float r);
    __forceinline __vector4 simd_reciprocal(__vector4 q) {
        __vector4 e = __vrefp(q);
        return __vmaddfp(__vnmsubfp(e, q, phys_simd_vec_1111), e, e);
    }

    __forceinline __vector4 simd_reciprocal_sqrt(__vector4 q) {
        __vector4 estimate = __vrsqrtefp(q);
        __vector4 OneHalfV = __vmulfp(q, phys_simd_vec_half);
        __vector4 Reciprocal = __vmulfp(estimate, estimate);
        __vector4 Scale = __vnmsubfp(OneHalfV, Reciprocal, phys_simd_vec_half);
        return __vmaddfp(estimate, Scale, estimate);
    }

    // External inline matches Havok 4.5 SDK; wpl folds unoptimized COMDAT copies into havok_component (0x259ca8).
    inline float sqrt(float r) { return __fsqrts(r); }
    __forceinline phys_simd_scalar sqrt(phys_simd_scalar_param r) {
        float x = r;
        phys_simd_scalar res;
        res.set(__fsqrts(x));
        return res;
    }

    // match: retain the checked ungrouped reciprocal while hkSimpleContactConstraintDataBuildJacobian (0x6751f8) and hkPredGskCylinderAgent3 need rematching.
    inline float inv_sqrt(float r) { return 1.0f / __fsqrts(r); }
    __forceinline phys_simd_scalar inv_sqrt(phys_simd_scalar r) {
        float x = r;
        phys_simd_scalar res;
        res.set(1.0f / __fsqrts(x));
        return res;
    }

    inline float fabs(float r) { return static_cast<float>(::fabs(static_cast<double>(r))); }
    inline float sin(float r) { return sinf(r); }
    inline float cos(float r) { return cosf(r); }
    // Out of line in havok_proxies (0x242978): 0 or pi outside [-1, 1], else acosf.
    float acos(float r);
    // Clamped acos: 0 or pi outside [-1, 1], else acosf.
    static __forceinline float acos_inline(float r) {
        if (phys_math::fabs(r) >= 1.0f) {
            r = (r > 0) ? 0.0f : 3.14159265358979f;
            return r;
        }
        return acosf(r);
    }
    // Out of line in hkSimulation (0x650dd8): plus or minus pi/2 outside [-1, 1], else asin.
    float asin(float r);
    __forceinline float min_of_two(float x, float y) { return static_cast<float>(__fsel(x - y, y, x)); }
    __forceinline float max_of_two(float x, float y) { return static_cast<float>(__fsel(x - y, x, y)); }
    inline phys_bool equal(float x, float y, float tolerance_sq = 1e-5f) { return phys_math::fabs(x - y) < tolerance_sq; }

    template <typename T>
    __forceinline T max_of_two(T x, T y) { return x > y ? x : y; }

    template <typename T>
    __forceinline T min_of_two(T x, T y) { return x < y ? x : y; }

    __forceinline int phys_to_int_fast(float r) { return static_cast<int>(r); }

    // March 2007 (0x834685f8): false for an infinity or NaN (exponent bits all set).
    __forceinline phys_bool is_finite(float r) {
        union {
            float f;
            unsigned int bits;
        } value;
        value.f = r;
        return phys_bool((value.bits & 0x7f800000) != 0x7f800000);
    }

    __forceinline void prefetch_128(const void *p) { __dcbt(0, p); }
    __forceinline float fsel_greater_equal_zero(float test_var, float if_true, float if_false) {
        return static_cast<float>(__fsel(test_var, if_true, if_false));
    }
    __forceinline float fsel_greater_zero(float test_var, float if_true, float if_false) {
        return static_cast<float>(__fsel(-test_var, if_true, if_false));
    }
    __forceinline float fsel_equal_zero(float test_var, float if_true, float if_false) {
        return static_cast<float>(__fsel(-test_var, __fsel(test_var, if_true, if_false), if_false));
    }
}

__forceinline const phys_simd_scalar operator*(phys_simd_scalar v0, phys_simd_scalar v1) {
    phys_simd_scalar res;
    res.get_quad() = __vmulfp(v0.get_quad(), v1.get_quad());
    return res;
}
__forceinline const phys_simd_scalar operator-(phys_simd_scalar v0, phys_simd_scalar v1) {
    phys_simd_scalar res;
    res.get_quad() = __vsubfp(v0.get_quad(), v1.get_quad());
    return res;
}
__forceinline const phys_simd_scalar operator+(phys_simd_scalar v0, phys_simd_scalar v1) {
    phys_simd_scalar res;
    res.get_quad() = __vaddfp(v0.get_quad(), v1.get_quad());
    return res;
}
__forceinline const phys_simd_scalar operator/(phys_simd_scalar v0, phys_simd_scalar v1) {
    __vector4 v1_recip = phys_math::simd_reciprocal(v1.get_quad());
    phys_simd_scalar res;
    res.get_quad() = __vmulfp(v0.get_quad(), v1_recip);
    return res;
}
__forceinline const phys_simd_scalar operator-(phys_simd_scalar v0) {
    phys_simd_scalar res;
    res.get_quad() = __vsubfp(phys_simd_vec_0000, v0.get_quad());
    return res;
}

class phys_vec4_cmp {
public:
    enum Mask {
        CMP_MASK_NONE = 0,
        CMP_MASK_W = 1,
        CMP_MASK_Z = 2,
        CMP_MASK_ZW = 3,
        CMP_MASK_Y = 4,
        CMP_MASK_YW = 5,
        CMP_MASK_YZ = 6,
        CMP_MASK_YZW = 7,
        CMP_MASK_X = 8,
        CMP_MASK_XW = 9,
        CMP_MASK_XZ = 10,
        CMP_MASK_XZW = 11,
        CMP_MASK_XY = 12,
        CMP_MASK_XYW = 13,
        CMP_MASK_XYZ = 14,
        CMP_MASK_XYZW = 15
    };

    static const __vector4 s_maskFromBits[CMP_MASK_XYZW + 1];
    static const __vector4 s_invMaskFromBits[CMP_MASK_XYZW + 1];

    __vector4 m_mask;

    __forceinline void set_and(phys_vec4_cmp a, phys_vec4_cmp b) {
        m_mask = __vand(a.m_mask, b.m_mask);
    }
    __forceinline void set_and_not(phys_vec4_cmp a, phys_vec4_cmp b) {
        m_mask = __vandc(a.m_mask, b.m_mask);
    }
    __forceinline void set_not_mask(phys_vec4_cmp a) {
        m_mask = __vnor(a.m_mask, a.m_mask);
    }
    __forceinline void set_or_mask(phys_vec4_cmp a, phys_vec4_cmp b) {
        m_mask = __vor(a.m_mask, b.m_mask);
    }
    __forceinline void set_select(phys_vec4_cmp comp, phys_vec4_cmp true_value, phys_vec4_cmp false_value) {
        m_mask = __vsel(false_value.m_mask, true_value.m_mask, comp.m_mask);
    }
    __forceinline void set(Mask m) { m_mask = s_maskFromBits[m]; }
    template <Mask M>
    __forceinline void set() { m_mask = s_maskFromBits[M]; }

    __forceinline int all_are_set(Mask m) const {
        unsigned int cr6_bits;
        __vector4 a = __vor(m_mask, s_invMaskFromBits[m]);
        __vcmpequwR(a, __vspltisw(-1), &cr6_bits);
        return cr6_bits & (1 << 7);
    }
    __forceinline int any_is_set(Mask m) const {
        unsigned int cr6_bits;
        __vector4 a = __vand(m_mask, s_maskFromBits[m]);
        __vcmpequwR(a, __vspltisw(0), &cr6_bits);
        return ~cr6_bits & (1 << 7);
    }
    __forceinline int all_are_set() const {
        unsigned int cr6_bits;
        __vcmpequwR(m_mask, __vspltisw(-1), &cr6_bits);
        return cr6_bits & (1 << 7);
    }
    __forceinline int any_is_set() const {
        unsigned int cr6_bits;
        __vcmpequwR(m_mask, __vspltisw(0), &cr6_bits);
        return ~cr6_bits & (1 << 7);
    }
    __forceinline int get_mask() const {
        __vector4 one_vec4 = {1.0f, 1.0f, 1.0f, 1.0f};
        __vector4 bit_select = {8.0f, 4.0f, 2.0f, 1.0f};
        __vector4 result = m_mask;
        result = __vand(result, one_vec4);
        result = __vmsum4fp(result, bit_select);
        result = __vctsxs(result, 0);
        return static_cast<int>(result.u[0]);
    }
    __forceinline int get_mask(Mask m) const { return get_mask() & m; }
};

class phys_mat3;
class phys_rotation;
class phys_quat;
class phys_transform;
class phys_qs_transform;

// hkVector4: four floats in one VMX register. The methods are the ones the March 2007 debug build emits for the
// class. The type also provides an array
// allocator (operator new[] / delete[] through hkThreadMemory); its bodies are in hk_math_allocator.h.
class phys_vec4 {
public:
    static __forceinline void *operator new[](size_t nbytes);
    static __forceinline void operator delete[](void *p);
    __vector4 m_simd_quad;

#ifndef PHYS_DISABLE_VEC4_CONSTRUCTORS
    __forceinline phys_vec4() {}
    // No explicit copy constructor; a copy is a plain 16-byte value copy (as in
    // hkCvxCvxDistByHeuristicSampling, hkMoppAabbCastVirtualMachine, hkAabb copies), so the copy stays the
    // compiler's.
    __forceinline phys_vec4(float x, float y, float z, float w = 0.0f) {
        union {
            float r[4];
            __vector4 q;
        } u;
        u.r[0] = x;
        u.r[1] = y;
        u.r[2] = z;
        u.r[3] = w;
        m_simd_quad = u.q;
    }
#endif
    __forceinline void operator=(const phys_vec4 &v) { m_simd_quad = v.m_simd_quad; }
    __forceinline void operator=(const __vector4 &v) { get_quad() = v; }
    __forceinline operator __vector4() const { return m_simd_quad; }

    __forceinline float &operator()(int i) { return ((float *)&m_simd_quad)[i]; }
    __forceinline const float &operator()(int i) const { return ((const float *)&m_simd_quad)[i]; }

    __forceinline const __vector4 &get_quad() const { return m_simd_quad; }
    __forceinline __vector4 &get_quad() { return m_simd_quad; }

    static __forceinline const phys_vec4 &get_zero() { return *reinterpret_cast<const phys_vec4 *>(&phys_simd_vec_0000); }

    __forceinline void set(float x, float y, float z, float w = 0.0f) {
        union {
            float r[4];
            __vector4 q;
        } u;
        u.r[0] = x;
        u.r[1] = y;
        u.r[2] = z;
        u.r[3] = w;
        m_simd_quad = u.q;
    }
    __forceinline void load_xyz(const float *p) {
        (*this)(0) = p[0];
        (*this)(1) = p[1];
        (*this)(2) = p[2];
    }

#if defined(_XBOX) && !defined(__clang__)
    __forceinline void set_zero4() { m_simd_quad = __vxor(m_simd_quad, m_simd_quad); }
#else
    __forceinline void set_zero4() { m_simd_quad = __vzero(); }
#endif
    __forceinline void flags_set_all(float value) {
        m_simd_quad.v[0] = value;
        m_simd_quad = __vspltw(m_simd_quad, 0);
    }
    __forceinline void flags_set_all(phys_simd_scalar_param value) { m_simd_quad = __vspltw(value.get_quad(), 0); }
    __forceinline void set_all3(float value) {
        m_simd_quad.v[0] = value;
        m_simd_quad = __vspltw(m_simd_quad, 0);
    }

    __forceinline void set_int24_w(int value) {
        // match: 0x623ec0: storing through a float-element pointer keeps the w stw after the z stfs; a u[3] store is hoisted above it (also 0x624080)
        unsigned int *w_component = reinterpret_cast<unsigned int *>(&m_simd_quad.v[3]);
        *w_component = static_cast<unsigned int>(value) | 0x3f000000u;
    }
    __forceinline int get_int24_w() const {
        return static_cast<int>(m_simd_quad.u[3] & ~0x3f000000u);
    }

    __forceinline void zero_element(int i) {
        __vector4 zero = __vspltisw(0);
        if (i == 0) m_simd_quad = __vrlimi(m_simd_quad, zero, 8, 0);
        else if (i == 1) m_simd_quad = __vrlimi(m_simd_quad, zero, 4, 0);
        else if (i == 2) m_simd_quad = __vrlimi(m_simd_quad, zero, 2, 0);
        else m_simd_quad = __vrlimi(m_simd_quad, zero, 1, 0);
    }

    __forceinline void add4(const phys_vec4 &v) { m_simd_quad = __vaddfp(m_simd_quad, v.m_simd_quad); }
    __forceinline void add3_clobber_w(const phys_vec4 &v) { m_simd_quad = __vaddfp(m_simd_quad, v.m_simd_quad); }
    __forceinline void sub4(const phys_vec4 &v) { m_simd_quad = __vsubfp(m_simd_quad, v.m_simd_quad); }
    __forceinline void sub3_clobber_w(const phys_vec4 &v) { m_simd_quad = __vsubfp(m_simd_quad, v.m_simd_quad); }
    __forceinline void mul4(const phys_vec4 &v) { m_simd_quad = __vmulfp(m_simd_quad, v.m_simd_quad); }
    __forceinline void mul4(const phys_simd_scalar &s) { m_simd_quad = __vmulfp(m_simd_quad, s.get_quad()); }

    __forceinline void set_add4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vaddfp(a.m_simd_quad, b.m_simd_quad); }
    __forceinline void set_sub4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vsubfp(a.m_simd_quad, b.m_simd_quad); }
    __forceinline void set_mul4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vmulfp(a.m_simd_quad, b.m_simd_quad); }
    __forceinline void set_mul4(const phys_simd_scalar &s, const phys_vec4 &v) { m_simd_quad = __vmulfp(v.m_simd_quad, s.get_quad()); }

    __forceinline void add_scaled4(const phys_simd_scalar &s, const phys_vec4 &v) { m_simd_quad = __vmaddfp(v.m_simd_quad, s.get_quad(), m_simd_quad); }
    __forceinline void add_scaled4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vmaddfp(a.m_simd_quad, b.m_simd_quad, m_simd_quad); }
    __forceinline void sub_mul4(const phys_simd_scalar &s, const phys_vec4 &v) { m_simd_quad = __vnmsubfp(v.m_simd_quad, s.get_quad(), m_simd_quad); }
    __forceinline void set_add_mul4(const phys_vec4 &a, const phys_vec4 &b, const phys_simd_scalar &s) {
        m_simd_quad = __vmaddfp(b.m_simd_quad, s.get_quad(), a.m_simd_quad);
    }
    __forceinline void set_add_mul4(const phys_vec4 &a, const phys_vec4 &x, const phys_vec4 &y) {
        m_simd_quad = __vmaddfp(x.m_simd_quad, y.m_simd_quad, a.m_simd_quad);
    }

    // March 2007 (0x82ba5dc8): two named permutation constants and two reused permute temporaries.
    __forceinline void set_cross(const phys_vec4 &x, const phys_vec4 &y) {
        const unsigned int mask_lo_yzx = 0x63;
        const unsigned int swiz_two_rhs = 0x87;
        __vector4 lhs_one_tmp = __vpermwi(x.m_simd_quad, mask_lo_yzx);
        __vector4 rhs_reg_op = __vpermwi(y.m_simd_quad, swiz_two_rhs);
        __vector4 swiz_prod_lo = __vmulfp(lhs_one_tmp, rhs_reg_op);
        lhs_one_tmp = __vpermwi(x.m_simd_quad, swiz_two_rhs);
        rhs_reg_op = __vpermwi(y.m_simd_quad, mask_lo_yzx);
        __vector4 vec_mul_out = __vmulfp(lhs_one_tmp, rhs_reg_op);
        m_simd_quad = __vsubfp(swiz_prod_lo, vec_mul_out);
    }

    __forceinline void set_lerp4(const phys_vec4 &a, const phys_vec4 &b, const phys_simd_scalar &t) {
        // match: 0x69ddd0: March (0x83476ce8) reads t.getQuad(); that extra inline call moves registers in
        // hkMoppAabbCastVirtualMachine, so this one reads the member.
        __vector4 x = __vspltw(t.m_simd_scalar, 0);
        __vector4 s = __vsubfp(phys_simd_vec_1111, x);
        __vector4 sa = __vmulfp(s, a.m_simd_quad);
        m_simd_quad = __vmaddfp(x, b.m_simd_quad, sa);
    }

    __forceinline void set_neg4(const phys_vec4 &x) {
        static const __declspec(align(16)) unsigned int negate_mask[4] = {0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u};
        m_simd_quad = __vxor(x.m_simd_quad, *(const __vector4 *)negate_mask);
    }
    __forceinline void set_neg3(const phys_vec4 &x) {
        static const __declspec(align(16)) unsigned int negate_mask[4] = {0x80000000u, 0x80000000u, 0x80000000u, 0x00000000u};
        m_simd_quad = __vxor(x.m_simd_quad, *(const __vector4 *)negate_mask);
    }
    __forceinline void set_abs4(const phys_vec4 &x) {
        static const __declspec(align(16)) unsigned int positive_mask[4] = {0x7fffffffu, 0x7fffffffu, 0x7fffffffu, 0x7fffffffu};
        m_simd_quad = __vand(x.m_simd_quad, *(const __vector4 *)positive_mask);
    }

    __forceinline void set_min4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vminfp(a.m_simd_quad, b.m_simd_quad); }
    __forceinline void set_max4(const phys_vec4 &a, const phys_vec4 &b) { m_simd_quad = __vmaxfp(a.m_simd_quad, b.m_simd_quad); }

    __forceinline phys_vec4_cmp compare_less4(const phys_vec4 &a) const {
        phys_vec4_cmp ret;
        ret.m_mask = __vcmpgtfp(a.m_simd_quad, m_simd_quad);
        return ret;
    }
    __forceinline phys_vec4_cmp compare_less_equal4(const phys_vec4 &a) const {
        phys_vec4_cmp ret;
        ret.m_mask = __vcmpgefp(a.m_simd_quad, m_simd_quad);
        return ret;
    }
    __forceinline phys_vec4_cmp compare_greater4(const phys_vec4 &a) const { return a.compare_less4(*this); }
    __forceinline phys_vec4_cmp compare_equal4(const phys_vec4 &a) const {
        phys_vec4_cmp ret;
        ret.m_mask = __vcmpeqfp(a.m_simd_quad, m_simd_quad);
        return ret;
    }
    __forceinline phys_vec4_cmp compare_less_zero4() const {
        phys_vec4_cmp ret;
        ret.m_mask = __vcmpgtfp(__vspltisw(0), m_simd_quad);
        return ret;
    }
    __forceinline void select_by_mask(const phys_vec4 &a, const phys_vec4 &b, const phys_vec4_cmp &comp) {
        m_simd_quad = __vsel(a.m_simd_quad, b.m_simd_quad, comp.m_mask);
    }

    __forceinline int approx_equal3(const phys_vec4 &v, float epsilon = 1e-3f) const {
        phys_vec4 t;
        t.set_sub4(*this, v);
        t.set_abs4(t);
        phys_vec4 epsilon_vec;
        epsilon_vec.set_all3(epsilon);
        return t.compare_less_equal4(epsilon_vec).all_are_set(phys_vec4_cmp::CMP_MASK_XYZ);
    }
    __forceinline int approx_equal4(const phys_vec4 &v, float epsilon = 1e-3f) const {
        phys_vec4 t;
        t.set_sub4(*this, v);
        t.set_abs4(t);
        phys_vec4 epsilon_vec;
        epsilon_vec.flags_set_all(epsilon);
        return t.compare_less_equal4(epsilon_vec).all_are_set(phys_vec4_cmp::CMP_MASK_XYZW);
    }

    __forceinline void set_w(const phys_vec4 &w) { m_simd_quad = __vrlimi(m_simd_quad, w.m_simd_quad, 1, 0); }
    __forceinline void set_xyz(const phys_vec4 &xyz) { m_simd_quad = __vrlimi(xyz.m_simd_quad, m_simd_quad, 1, 0); }
    __forceinline void set_xyzw(const phys_vec4 &xyz, const phys_vec4 &w) { m_simd_quad = __vrlimi(xyz.m_simd_quad, w.m_simd_quad, 1, 0); }
    __forceinline void set_xyz0(const phys_vec4 &v) {
        __vector4 zero = __vspltisw(0);
        m_simd_quad = __vrlimi(v.m_simd_quad, zero, 1, 0);
    }

    __forceinline phys_simd_scalar get_simd_at(int i) const {
        phys_simd_scalar res;
        switch (i) {
        case 0: res.get_quad() = __vspltw(m_simd_quad, 0); return res;
        case 1: res.get_quad() = __vspltw(m_simd_quad, 1); return res;
        case 2: res.get_quad() = __vspltw(m_simd_quad, 2); return res;
        case 3:
        default: res.get_quad() = __vspltw(m_simd_quad, 3); return res;
        }
    }

    __forceinline void set_broadcast(const phys_vec4 &v, int i) {
        if (i == 0) m_simd_quad = __vspltw(v.m_simd_quad, 0);
        else if (i == 1) m_simd_quad = __vspltw(v.m_simd_quad, 1);
        else if (i == 2) m_simd_quad = __vspltw(v.m_simd_quad, 2);
        else m_simd_quad = __vspltw(v.m_simd_quad, 3);
    }
    __forceinline void broadcast(int i) { set_broadcast(*this, i); }
    __forceinline void set_broadcast3_clobber_w(const phys_vec4 &v, int i) { set_broadcast(v, i); }
    __forceinline void store_uncached(void *dest) const { *static_cast<phys_vec4 *>(dest) = *this; }

    __forceinline int all_less_than3(const phys_vec4 &a) const {
        return compare_less4(a).all_are_set(phys_vec4_cmp::CMP_MASK_XYZ);
    }

    __forceinline void set_reciprocal4(const phys_vec4 &vec) {
        m_simd_quad = phys_math::simd_reciprocal(vec.m_simd_quad);
    }
    __forceinline void set_reciprocal3(const phys_vec4 &vec) {
        phys_vec4 ones;
        ones.get_quad() = phys_simd_vec_1111;
        phys_vec4 xyz;
        xyz.set_xyzw(vec, ones);
        set_reciprocal4(xyz);
    }
    __forceinline void set_sqrt_inverse4(const phys_vec4 &vec) {
        m_simd_quad = phys_math::simd_reciprocal_sqrt(vec.m_simd_quad);
    }

    __forceinline phys_simd_scalar dot3(const phys_vec4 &x) const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum3fp(m_simd_quad, x.m_simd_quad);
        return res;
    }
    __forceinline float dot3_fpu(const phys_vec4 &a) const { return (*this)(0) * a(0) + (*this)(1) * a(1) + (*this)(2) * a(2); }
    __forceinline phys_simd_scalar dot4_simd(const phys_vec4 &x) const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum4fp(m_simd_quad, x.m_simd_quad);
        return res;
    }
    __forceinline phys_simd_scalar dot4_xyz1(const phys_vec4 &x) const {
        phys_simd_scalar res;
        res.get_quad() = __vaddfp(__vspltw(m_simd_quad, 3), __vmsum3fp(m_simd_quad, x.m_simd_quad));
        return res;
    }
    __forceinline phys_simd_scalar horizontal_add3() const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum3fp(m_simd_quad, phys_simd_vec_1111);
        return res;
    }
    __forceinline phys_simd_scalar horizontal_add4() const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum4fp(m_simd_quad, phys_simd_vec_1111);
        return res;
    }

    __forceinline phys_simd_scalar length_sq_3d() const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum3fp(m_simd_quad, m_simd_quad);
        return res;
    }
    __forceinline phys_simd_scalar length_sq_4d() const {
        phys_simd_scalar res;
        res.get_quad() = __vmsum4fp(m_simd_quad, m_simd_quad);
        return res;
    }
    __forceinline phys_simd_scalar length_3d() const {
        __vector4 temp = __vmsum3fp(m_simd_quad, m_simd_quad);
        phys_simd_scalar res;
        res.set(phys_math::sqrt(temp.v[0]));
        return res;
    }
    __forceinline phys_simd_scalar inv_length_3d() const {
        __vector4 temp = __vmsum3fp(m_simd_quad, m_simd_quad);
        phys_simd_scalar res;
        res.set(phys_math::inv_sqrt(temp.v[0]));
        return res;
    }
    __forceinline void normalize_xyz() {
        __vector4 temp = __vmsum3fp(m_simd_quad, m_simd_quad);
        __vector4 scale;
        scale.v[0] = phys_math::inv_sqrt(temp.v[0]);
        scale = __vspltw(scale, 0);
        m_simd_quad = __vmulfp(m_simd_quad, scale);
    }
    __forceinline phys_simd_scalar normalize_xyz_with_length() {
        __vector4 length_sq_quad = __vmsum3fp(m_simd_quad, m_simd_quad);
        __vector4 scale;
        scale.v[0] = phys_math::inv_sqrt(length_sq_quad.v[0]);
        scale = __vspltw(scale, 0);
        m_simd_quad = __vmulfp(m_simd_quad, scale);
        phys_simd_scalar res;
        res.get_quad() = __vmulfp(scale, length_sq_quad);
        return res;
    }
    __forceinline void normalize_xyzw() {
        __vector4 temp = __vmsum4fp(m_simd_quad, m_simd_quad);
        __vector4 scale;
        scale.v[0] = phys_math::inv_sqrt(temp.v[0]);
        scale = __vspltw(scale, 0);
        m_simd_quad = __vmulfp(m_simd_quad, scale);
    }
    __forceinline void fast_normalize3() { normalize_xyz(); }

    __forceinline void set_mul_mat3_vec3(const phys_mat3 &matrix, const phys_vec4 &vector);
    __forceinline void set_mul_mat3_vec4(const phys_mat3 &matrix, const phys_vec4 &vector);
    __forceinline void set_rotated_dir(const phys_rotation &rotation, const phys_vec4 &direction);
    __forceinline void set_rotated_inverse_dir(const phys_rotation &rotation, const phys_vec4 &direction);
    __forceinline void set_transformed_pos(const phys_transform &transform, const phys_vec4 &position);
    __forceinline void set_mul_xform_xyz1(const phys_transform &transform, const phys_vec4 &position);
    __forceinline void set_transformed_inverse_pos(const phys_transform &transform, const phys_vec4 &position);
    __forceinline void set_transformed_pos(const phys_qs_transform &a, const phys_vec4 &b);
    __forceinline void set_transformed_inverse_pos(const phys_qs_transform &a, const phys_vec4 &b);

    void set_mul3(const phys_mat3 &a, const phys_vec4 &b);
    void vec_set_rotated_dir(const phys_rotation &a, const phys_vec4 &b);
    void vec_set_rotated_inverse_dir(const phys_rotation &a, const phys_vec4 &b);
    __forceinline void vec_set_rotated_dir(const phys_quat &q, const phys_vec4 &b);
    __forceinline void vec_set_rotated_inverse_dir(const phys_quat &q, const phys_vec4 &b);
    void vec_set_transformed_pos(const phys_transform &a, const phys_vec4 &b);
    void vec_set_transformed_inverse_pos(const phys_transform &a, const phys_vec4 &b);
    void vec_set_transformed_pos(const phys_qs_transform &a, const phys_vec4 &b);
    void vec_set_transformed_inverse_pos(const phys_qs_transform &a, const phys_vec4 &b);
    phys_bool is_ok3() const;
    phys_bool is_ok4() const;
    phys_bool is_normalized3(float float_epsilon = 1e-4f) const;
    phys_bool is_normalized4(float float_epsilon = 1e-4f) const;
    __forceinline int get_major_axis() const {
        phys_vec4 tmp;
        tmp.set_abs4(*this);
        return tmp(0) < tmp(1) ? (tmp(1) < tmp(2) ? 2 : 1) : (tmp(0) < tmp(2) ? 2 : 0);
    }
};

class phys_mat3 {
public:
    phys_vec4 m_column0;
    phys_vec4 m_column1;
    phys_vec4 m_column2;

    __forceinline phys_mat3() {}
    __forceinline phys_mat3(const phys_mat3 &other) {
        m_column0 = other.m_column0;
        m_column1 = other.m_column1;
        m_column2 = other.m_column2;
    }
    // March 2007 (0x82ba6720): column by column, returns nothing.
    inline void operator=(const phys_mat3 &other) {
        m_column0 = other.get_column(0);
        m_column1 = other.get_column(1);
        m_column2 = other.get_column(2);
    }

    __forceinline phys_vec4 &get_column(int index) { return (&m_column0)[index]; }
    __forceinline const phys_vec4 &get_column(int index) const { return (&m_column0)[index]; }
    __forceinline void get_row(int row, phys_vec4 &r) const {
        r.set(get_column(0)(row), get_column(1)(row), get_column(2)(row));
    }
    __forceinline void set_rows(const phys_vec4 &r0, const phys_vec4 &r1, const phys_vec4 &r2) {
        get_column(0).set(r0(0), r1(0), r2(0));
        get_column(1).set(r0(1), r1(1), r2(1));
        get_column(2).set(r0(2), r1(2), r2(2));
    }
    __forceinline float &operator()(int row, int column) { return get_column(column)(row); }
    __forceinline const float &operator()(int row, int column) const { return get_column(column)(row); }

    __forceinline void set_cols(const phys_vec4 &c0, const phys_vec4 &c1, const phys_vec4 &c2) {
        m_column0 = c0;
        m_column1 = c1;
        m_column2 = c2;
    }
    __forceinline void get_columns(phys_vec4 &c0, phys_vec4 &c1, phys_vec4 &c2) const {
        c0 = get_column(0);
        c1 = get_column(1);
        c2 = get_column(2);
    }
    __forceinline void set_transpose_of(const phys_mat3 &s) {
        phys_mat3 &d = *this;
        d(0, 0) = s(0, 0);
        d(1, 1) = s(1, 1);
        d(2, 2) = s(2, 2);
        d(3, 0) = 0.0f;
        d(3, 1) = 0.0f;
        d(3, 2) = 0.0f;
        d(1, 0) = s(0, 1);
        d(0, 1) = s(1, 0);
        d(2, 0) = s(0, 2);
        d(0, 2) = s(2, 0);
        d(2, 1) = s(1, 2);
        d(1, 2) = s(2, 1);
    }
    // March 2007 (0x82ba67a0, 0x82ba7408): one zero vector stored to each column; one named 1.0f.
    __forceinline void set_zero() {
        phys_vec4 zero;
        zero.set_zero4();
        get_column(0) = zero;
        get_column(1) = zero;
        get_column(2) = zero;
    }
    __forceinline void set_to_identity() {
        set_zero();
        float one = 1.0f;
        (*this)(0, 0) = one;
        (*this)(1, 1) = one;
        (*this)(2, 2) = one;
    }

    phys_bool is_stream_ok() const;
    static const phys_mat3 &get_identity();
    void set_transpose(const phys_mat3 &s);
    void transpose_in_place();
    phys_result try_invert(float epsilon);
    void set_mul(const phys_mat3 &a, const phys_mat3 &b);
    void set_mul_inverse(const phys_mat3 &a, const phys_rotation &b);
    // Zero the matrix, then set the three diagonal elements.
    __forceinline void set_diagonal(float m00, float m11, float m22) {
        set_zero();
        (*this)(0, 0) = m00;
        (*this)(1, 1) = m11;
        (*this)(2, 2) = m22;
    }
    void set_cross_skew_symmetric(const phys_vec4 &r);
    void change_basis(const phys_rotation &r);

    // Out of line in hkMatrix3.obj (0x611d48..0x612388).
    phys_bool is_approx_equal(const phys_mat3 &m, float epsilon = 1e-3f) const;
    void invert_symmetric();
    void set_mul(const phys_simd_scalar &scale, const phys_mat3 &a);
    void add_scaled(const phys_simd_scalar &scale, const phys_mat3 &a);
    void add(const phys_mat3 &a);
    void sub(const phys_mat3 &a);
    void mul(const phys_mat3 &a);
    void mul(const phys_simd_scalar &scale);

    // March 2007 (0x8378ef98): the product through the C helper hkMatrix3.obj exports (0x612218).
    __forceinline void set_mat3_product(const phys_mat3 &a, const phys_mat3 &b);
    // Not in March: the inline form of setMul(scale, a) (March 0x8346ad20), column by column.
    __forceinline void set_mat3_product(const phys_simd_scalar &scale, const phys_mat3 &a) {
        get_column(0).set_mul4(scale, a.get_column(0));
        get_column(1).set_mul4(scale, a.get_column(1));
        get_column(2).set_mul4(scale, a.get_column(2));
    }
    // March 2007 (0x8378efd0, 0x8378f068): column by column.
    __forceinline void accumulate_mat3(const phys_mat3 &a) {
        get_column(0).add4(a.get_column(0));
        get_column(1).add4(a.get_column(1));
        get_column(2).add4(a.get_column(2));
    }
    __forceinline void subtract_mat3(const phys_mat3 &a) {
        get_column(0).sub4(a.get_column(0));
        get_column(1).sub4(a.get_column(1));
        get_column(2).sub4(a.get_column(2));
    }
};

extern "C" {
void phys_mat3_set_mul_mat3(phys_mat3 *product, const phys_mat3 &a, const phys_mat3 &b);
void phys_mat3_invert_symmetric(phys_mat3 &matrix);
}

__forceinline void phys_mat3::set_mat3_product(const phys_mat3 &a, const phys_mat3 &b) { phys_mat3_set_mul_mat3(this, a, b); }

// A 4x4 matrix as four columns. March 2007 emits only the constructor and setCols (0x837901c0).
class phys_mat4 {
public:
    __forceinline phys_mat4() {}
    __forceinline void set_cols(const phys_vec4 &c0, const phys_vec4 &c1, const phys_vec4 &c2, const phys_vec4 &c3) {
        m_cols[0] = c0;
        m_cols[1] = c1;
        m_cols[2] = c2;
        m_cols[3] = c3;
    }

    phys_vec4 m_cols[4];
};

class phys_rotation : public phys_mat3 {
public:

    void set(const phys_quat &q);
    void set_axis_angle(const phys_vec4 &axis, float angle);
    phys_bool is_orthonormal(float epsilon = 1e-3f) const;
    void renormalize_axes();
};

class phys_quat {
public:
    phys_vec4 m_quat_vec;

    __forceinline phys_quat() {}
    __forceinline phys_quat(float x, float y, float z, float w) { m_quat_vec.set(x, y, z, w); }
    __forceinline phys_quat(const phys_rotation &r) { set(r); }
    inline phys_quat(const phys_vec4 &axis, float angle) { set_axis_angle(axis, angle); }
    // March 2007 (0x82ba68c0): returns nothing.
    inline void operator=(const phys_quat &q) { m_quat_vec = q.m_quat_vec; }

    __forceinline const phys_vec4 &get_imag() const { return m_quat_vec; }
    __forceinline void set_imag_part(const phys_vec4 &i) { m_quat_vec.set_xyz(i); }
    __forceinline float get_real() const { return m_quat_vec(3); }
    __forceinline void set_real_part(float r) { m_quat_vec(3) = r; }
    __forceinline const float &operator()(int i) const { return m_quat_vec(i); }

    __forceinline void set(float x, float y, float z, float w) { m_quat_vec.set(x, y, z, w); }
    // The SIMD build loads the (0, 0, 0, 1) constant.
    __forceinline void set_to_identity() { m_quat_vec.get_quad() = phys_simd_vec_0001; }
    __forceinline void normalize() { m_quat_vec.normalize_xyzw(); }

    __forceinline void set_to_inverse(const phys_quat &q) {
        float real = q.get_real();
        m_quat_vec.set_neg4(q.get_imag());
        set_real_part(real);
    }
    __forceinline void set_mul(const phys_simd_scalar &r, const phys_quat &q) { m_quat_vec.set_mul4(r, q.m_quat_vec); }
    __forceinline void add_scaled(const phys_simd_scalar &r, const phys_quat &q) { m_quat_vec.add_scaled4(r, q.m_quat_vec); }
    __forceinline void mul(const phys_quat &q) {
        phys_vec4 vec;
        vec.set_cross(get_imag(), q.get_imag());
        vec.add_scaled4(m_quat_vec.get_simd_at(3), q.get_imag());
        vec.add_scaled4(q.m_quat_vec.get_simd_at(3), get_imag());
        float w = get_real() * q.get_real() - static_cast<float>(get_imag().dot3(q.get_imag()));
        m_quat_vec = vec;
        m_quat_vec(3) = w;
    }
    __forceinline void set_mul(const phys_quat &q0, const phys_quat &q1) {
        phys_vec4 vec;
        vec.set_cross(q0.get_imag(), q1.get_imag());
        vec.add_scaled4(q0.m_quat_vec.get_simd_at(3), q1.get_imag());
        vec.add_scaled4(q1.m_quat_vec.get_simd_at(3), q0.get_imag());
        float real = q0.get_real() * q1.get_real() - static_cast<float>(q0.m_quat_vec.dot3(q1.m_quat_vec));
        m_quat_vec = vec;
        m_quat_vec(3) = real;
    }
    __forceinline void set_mul_inverse(const phys_quat &q0, const phys_quat &q1) {
        phys_vec4 h;
        h.set_cross(q1.get_imag(), q0.get_imag());
        h.sub_mul4(q0.m_quat_vec.get_simd_at(3), q1.get_imag());
        h.add_scaled4(q1.m_quat_vec.get_simd_at(3), q0.get_imag());
        m_quat_vec = h;
        m_quat_vec(3) = q0.m_quat_vec.dot4_simd(q1.m_quat_vec);
    }
    __forceinline void set_inverse_mul(const phys_quat &q0, const phys_quat &q1) {
        m_quat_vec.set_cross(q1.get_imag(), q0.get_imag());
        m_quat_vec.add_scaled4(q0.m_quat_vec.get_simd_at(3), q1.get_imag());
        m_quat_vec.sub_mul4(q1.m_quat_vec.get_simd_at(3), q0.get_imag());
        m_quat_vec(3) = q0.m_quat_vec.dot4_simd(q1.m_quat_vec);
    }
    // Out of line in havok_proxies (0x242938).
    float get_rotation_angle() const;
    // The rotation angle as twice acos of |w| (see also
    // hkSweptTransformUtil and hkKeyFrameUtility; the out-of-line getAngle stays in havok_proxies 0x242938).
    __forceinline float get_rotation_angle_inline() const {
        float angle = phys_math::fabs(m_quat_vec(3));
        angle = phys_math::acos_inline(angle);
        angle *= 2.0f;
        return angle;
    }
    // Normalized imaginary axis, flipped when w is negative.
    inline void get_rotation_axis(phys_vec4 &axis) const {
        axis = m_quat_vec;
        axis.normalize_xyz();
        if (m_quat_vec(3) < 0.0f) {
            axis.set_neg4(axis);
        }
    }

    void set(const phys_rotation &r);
    void set_axis_angle(const phys_vec4 &axis, float angle);
    void set_and_normalize(const phys_rotation &r);
    void remove_axis_component(const phys_vec4 &axis);
    void decompose_rest_axis(const phys_vec4 &axis, phys_quat &rest_out, float &angle_out) const;
    void set_slerp(const phys_quat &q0, const phys_quat &q1, float t);
    phys_bool is_stream_ok() const;
    // March 2007 (0x834e50d8): the global identity quaternion.
    static __forceinline const phys_quat &get_identity() {
        extern const phys_quat phys_quat_identity;
        return phys_quat_identity;
    }

protected:
    void set_flipped_rotation(const phys_vec4 &from);
};


#endif // INCLUDE_HAVOK_HK_MATH_VALUE_TYPES_H
