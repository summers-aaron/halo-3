#ifndef INCLUDE_PHYS_VEC4_UTIL_H
#define INCLUDE_PHYS_VEC4_UTIL_H

#include "phys/phys_base.h"

#include "phys/phys_int_union64.h"

namespace phys_vec4_util {
    __forceinline void PHYS_CALL mul_signs4(phys_vec4& in_out_vec, const phys_vec4& sign_vec);
    float PHYS_CALL float_to_i16_floor_bias();
    float PHYS_CALL float_to_i32_floor_bias();
    unsigned int PHYS_CALL pack_quat_to_u32(phys_vec4_param quat_in);
    __forceinline void PHYS_CALL unpack_i32_to_quat(unsigned int packed_int, phys_vec4& unpacked_quat);

    __forceinline void PHYS_CALL convert_to_u16(const phys_vec4& in, const phys_vec4& offset, const phys_vec4& scale, phys_int_union64& out);
    __forceinline void PHYS_CALL convert_to_u32(const phys_vec4& in, const phys_vec4& offset, const phys_vec4& scale, unsigned int* out);
    __forceinline void PHYS_CALL compute_perpendicular_vec(const phys_vec4& vec_in, phys_vec4& bi_vec_out);
    __forceinline void PHYS_CALL rotate_points(const phys_mat3& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out);
    __forceinline void PHYS_CALL rotate_inverse_points(const phys_rotation& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out);
    __forceinline void PHYS_CALL transform_points(const phys_transform& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out);
    __forceinline void PHYS_CALL transform_points4(const phys_transform& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out);
    __forceinline void PHYS_CALL dot4_4x4(const phys_vec4& a0, const phys_vec4& b0, const phys_vec4& a1, const phys_vec4& b1,
        const phys_vec4& a2, const phys_vec4& b2, const phys_vec4& a3, const phys_vec4& b3, phys_vec4& dot_products_out);
    __forceinline void PHYS_CALL reserve_registers(unsigned int register_mask) {}
    __forceinline void PHYS_CALL release_registers(unsigned int register_mask) {}
    __forceinline void PHYS_CALL check_registers(unsigned int register_mask) {}
}

// March 0x83508188 (emitted in hkRigidMotionUtil): the four bytes of a packed quaternion, re-centred and scaled.
inline void PHYS_CALL phys_vec4_util::unpack_i32_to_quat(unsigned int packed_int, phys_vec4& unpacked_quat) {
    int a = packed_int;
    int b = packed_int >> 8;
    int c = packed_int >> 16;
    int d = packed_int >> 24;
    a &= 0xff;
    b &= 0xff;
    c &= 0xff;
    a -= 0x80;
    b -= 0x80;
    c -= 0x80;
    d -= 0x80;
    const float f = 1.1f / 128.0f;
    unpacked_quat(0) = f * a;
    unpacked_quat(1) = f * b;
    unpacked_quat(2) = f * c;
    unpacked_quat(3) = f * d;
}

// Flip sign bits with the shared four-component sign mask.
inline void PHYS_CALL phys_vec4_util::mul_signs4(phys_vec4& in_out_vec, const phys_vec4& sign_vec) {
    static const __declspec(align(16)) unsigned int simd_sign_mask[4] = {0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u};
    __vector4 temp = __vand(sign_vec.m_simd_quad, *(const __vector4*)simd_sign_mask);
    in_out_vec.m_simd_quad = __vxor(in_out_vec.m_simd_quad, temp);
}

inline void PHYS_CALL phys_vec4_util::convert_to_u16(const phys_vec4& in, const phys_vec4& offset, const phys_vec4& scale, phys_int_union64& out) {
    // Bias each component by 196608.0f before the bit-shift extraction below.
    static const float magic[4] = { 196608.0f, 196608.0f, 196608.0f, 196608.0f };
    phys_vec4 x;
    x.set_add4(in, offset);
    x.mul4(scale);
    x.add4(reinterpret_cast<const phys_vec4&>(magic));
    union { phys_simd_vec q; unsigned int i[4]; } u;
    u.q = x.get_quad();
    out.i16_lanes[0] = static_cast<short>(u.i[0] >> 6);
    out.i16_lanes[1] = static_cast<short>(u.i[1] >> 6);
    out.i16_lanes[2] = static_cast<short>(u.i[2] >> 6);
    out.i16_lanes[3] = static_cast<short>(u.i[3] >> 6);
}

inline void PHYS_CALL phys_vec4_util::convert_to_u32(const phys_vec4& in, const phys_vec4& offset, const phys_vec4& scale, unsigned int* out) {
    phys_vec4 x;
    x.set_add4(in, offset);
    x.mul4(scale);
    out[0] = static_cast<unsigned int>(phys_math::phys_to_int_fast(x(0)));
    out[1] = static_cast<unsigned int>(phys_math::phys_to_int_fast(x(1)));
    out[2] = static_cast<unsigned int>(phys_math::phys_to_int_fast(x(2)));
    out[3] = static_cast<unsigned int>(phys_math::phys_to_int_fast(x(3)));
}

inline void PHYS_CALL phys_vec4_util::compute_perpendicular_vec(const phys_vec4& vec_in, phys_vec4& bi_vec_out) {
    int min = 0;
    int ok_flag1 = 1;
    int ok_flag2 = 2;
    float a0 = phys_math::fabs(vec_in(0));
    float a1 = phys_math::fabs(vec_in(1));
    float a2 = phys_math::fabs(vec_in(2));
    if (a1 < a0) { ok_flag1 = 0; min = 1; a0 = a1; }
    if (a2 < a0) { ok_flag2 = min; min = 2; }
    bi_vec_out.set_zero4();
    bi_vec_out(ok_flag1) = vec_in(ok_flag2);
    bi_vec_out(ok_flag2) = -vec_in(ok_flag1);
}

inline void PHYS_CALL phys_vec4_util::rotate_points(const phys_mat3& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out) {
    check_registers(0x70000000);
    phys_mat3 unaliased_copy = t;
    int i = rotate_vec_count - 1;
    do { vecs_out[i].set_mul_mat3_vec3(unaliased_copy, vecs_in[i]); } while (--i >= 0);
}

inline void PHYS_CALL phys_vec4_util::rotate_inverse_points(const phys_rotation& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out) {
    phys_rotation unaliased_copy = t;
    int i = rotate_vec_count - 1;
    do { vecs_out[i].set_rotated_inverse_dir(unaliased_copy, vecs_in[i]); } while (--i >= 0);
}

// March 0x8347c2b0: copy the transform before traversing the vectors backwards.
inline void PHYS_CALL phys_vec4_util::transform_points(const phys_transform& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out) {
    check_registers(0xf0000000);
    phys_transform unaliased_copy = t;
    int i = rotate_vec_count - 1;
    do {
        vecs_out[i].set_transformed_pos(unaliased_copy, vecs_in[i]);
    } while (--i >= 0);
}

inline void PHYS_CALL phys_vec4_util::dot4_4x4(const phys_vec4& a0, const phys_vec4& b0, const phys_vec4& a1, const phys_vec4& b1,
    const phys_vec4& a2, const phys_vec4& b2, const phys_vec4& a3, const phys_vec4& b3, phys_vec4& dot_products_out) {
    dot_products_out.get_quad() = __vrlimi(__vmsum4fp(a0, b0), __vmsum4fp(a1, b1), 4, 0);
    dot_products_out.get_quad() = __vrlimi(dot_products_out.get_quad(), __vmsum4fp(a2, b2), 2, 0);
    dot_products_out.get_quad() = __vrlimi(dot_products_out.get_quad(), __vmsum4fp(a3, b3), 1, 0);
}

// March 0x83560a40: each point times the transform, w taken as one (the translation added last).
inline void PHYS_CALL phys_vec4_util::transform_points4(const phys_transform& t, const phys_vec4* vecs_in, int rotate_vec_count, phys_vec4* vecs_out) {
    phys_transform unaliased_copy = t;
    int i = rotate_vec_count - 1;
    do {
        const __vector4 b = vecs_in[i].m_simd_quad;
        __vector4 x = __vspltw(b, 0);
        __vector4 y = __vspltw(b, 1);
        __vector4 z = __vspltw(b, 2);
        __vector4 r = __vmulfp(unaliased_copy.get_column(0).m_simd_quad, x);
        r = __vmaddfp(unaliased_copy.get_column(1).m_simd_quad, y, r);
        r = __vmaddfp(unaliased_copy.get_column(2).m_simd_quad, z, r);
        vecs_out[i].m_simd_quad = __vaddfp(r, unaliased_copy.get_column(3).m_simd_quad);
    } while (--i >= 0);
}

#endif // INCLUDE_HAVOK_HK_VECTOR4_UTIL_H
