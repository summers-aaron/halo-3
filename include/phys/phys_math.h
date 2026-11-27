#ifndef INCLUDE_PHYS_MATH_H
#define INCLUDE_PHYS_MATH_H

#include "phys/phys_math_value_types.h"

class phys_transform {
public:
    phys_rotation m_rotation;
    phys_vec4 m_translation;

    __forceinline phys_transform() {}
    // March 2007 (0x82ba71f0): the four columns read into locals, then stored.
    __forceinline phys_transform(const phys_transform &other) {
        const phys_vec4 c0 = other.m_rotation.get_column(0);
        const phys_vec4 c1 = other.m_rotation.get_column(1);
        const phys_vec4 c2 = other.m_rotation.get_column(2);
        const phys_vec4 c3 = other.m_rotation.get_column(3);
        m_rotation.get_column(0) = c0;
        m_rotation.get_column(1) = c1;
        m_rotation.get_column(2) = c2;
        m_rotation.get_column(3) = c3;
    }
    __forceinline phys_transform(const phys_rotation &r, const phys_vec4 &t) : m_rotation(r), m_translation(t) {}
    __forceinline phys_transform(const phys_quat &q, const phys_vec4 &t) : m_translation(t) { m_rotation.set(q); }
    __forceinline phys_rotation &get_rotation() { return m_rotation; }
    __forceinline const phys_rotation &get_rotation() const { return m_rotation; }
    __forceinline phys_vec4 &get_translation() { return m_translation; }
    __forceinline const phys_vec4 &get_translation() const { return m_translation; }
    // March 2007 (0x834e0230, 0x834769a0): through the rotation's columns; column 3 is the translation.
    __forceinline phys_vec4 &get_column(int index) { return m_rotation.get_column(index); }
    __forceinline const phys_vec4 &get_column(int index) const { return m_rotation.get_column(index); }
    __forceinline float &operator()(int row, int column) { return get_column(column)(row); }
    __forceinline const float &operator()(int row, int column) const { return get_column(column)(row); }

    // March 2007 (0x83542960).
    __forceinline void set_rows4(const phys_vec4 &r0, const phys_vec4 &r1, const phys_vec4 &r2, const phys_vec4 &r3) {
        get_column(0).set(r0(0), r1(0), r2(0), r3(0));
        get_column(1).set(r0(1), r1(1), r2(1), r3(1));
        get_column(2).set(r0(2), r1(2), r2(2), r3(2));
        get_column(3).set(r0(3), r1(3), r2(3), r3(3));
    }

    // March 2007 (0x83476928, 0x8354fe38, 0x82bf9a10).
    __forceinline void set_rotation(const phys_quat &q) { m_rotation.set(q); }
    __forceinline void set_rotation(const phys_rotation &r) { static_cast<phys_mat3 &>(m_rotation) = r; }
    __forceinline void set_translation(const phys_vec4 &t) { m_translation = t; }
    __forceinline void set(const phys_quat &q, const phys_vec4 &t) {
        m_rotation.set(q);
        m_translation.m_simd_quad = t.m_simd_quad;
    }
    __forceinline void set_to_identity() {
        m_rotation.set_to_identity();
        m_translation.set_zero4();
    }

    static __forceinline const phys_transform &get_identity();
    phys_bool is_stream_ok() const;
    void get_4x4_column_major(float *p) const;
    void set_4x4_column_major(const float *p);
    void set_to_inverse(const phys_transform &t);
    void set_mul(const phys_transform &a, const phys_transform &b);
    void set_mul_inverse_mul(const phys_transform &a, const phys_transform &b);
    void set_mul_mul_inverse(const phys_transform &a, const phys_transform &b);
    void set_mul_eq(const phys_transform &b);
    phys_bool is_approx_equal(const phys_transform &t, float epsilon = 1e-3f) const;
};

// The identity transform is the global hkTransformIdentity.
extern float phys_transform_identity[];
__forceinline const phys_transform &phys_transform::get_identity() { return reinterpret_cast<const phys_transform &>(phys_transform_identity); }

class phys_qs_transform {
public:
    phys_vec4 m_translation;
    phys_quat m_rotation;
    phys_vec4 m_scale;

    __forceinline phys_qs_transform() {}
    __forceinline void set(const phys_vec4 &translation, const phys_quat &rotation, const phys_vec4 &scale) {
        phys_qs_transform *transform = this;
        transform->m_translation.m_simd_quad = translation.m_simd_quad;
        transform->m_rotation.m_quat_vec.m_simd_quad = rotation.m_quat_vec.m_simd_quad;
        transform->m_scale.m_simd_quad = scale.m_simd_quad;
    }

    phys_bool is_stream_ok() const;
    void set_from_transform_unscaled(const phys_transform &t);
    void to_transform_unscaled(phys_transform &t) const;
    phys_bool is_approx_equal(const phys_qs_transform &other, float epsilon) const;
    void get_4x4_column_major(float *d) const;
    phys_bool set_4x4_column_major(const float *p);
    void set_to_inverse(const phys_qs_transform &t);
    void set_from_transform(const phys_transform &t);
    void to_transform(phys_transform &t) const;
    static void fast_renormalize_batch(phys_qs_transform *pose_out, float *weight, unsigned int transform_count);
};

class phys_swept_transform {
public:
    __forceinline phys_swept_transform() {}
    __forceinline float get_base_time() const { return m_center_of_mass0(3); }
    __forceinline float get_inv_delta_time() const { return m_center_of_mass1(3); }
    // March 2007 (0x835083c0): the fraction of the step at time t; its range asserts are debug only.
    __forceinline float get_interp_value(float t) const {
        float dt = (t - get_base_time()) * get_inv_delta_time();
        return dt;
    }
    // March 2007 (0x8378dcc0): the time shifted by an add-on before scaling; its range asserts are debug only.
    __forceinline float get_interp_value_precise(float t, float delta_time_addon) const {
        float dt = t - get_base_time();
        dt += delta_time_addon;
        float scaled_dt = dt * get_inv_delta_time();
        return scaled_dt;
    }

    // March 2007 (0x8378e1f8): rotation and centre of mass interpolated at the time's fraction, then the
    // centre shifted back to the body origin.
    __forceinline void approx_transform_at(float time, phys_transform &xform_out) const {
        phys_simd_scalar dt = phys_simd_scalar::create((time - get_base_time()) * get_inv_delta_time());
        phys_quat q;
        q.m_quat_vec.set_lerp4(m_rotation_0.m_quat_vec, m_rotation_1.m_quat_vec, dt);
        q.m_quat_vec.normalize_xyzw();
        xform_out.set_rotation(q);
        xform_out.get_translation().set_lerp4(m_center_of_mass0, m_center_of_mass1, dt);
        phys_vec4 center_shift;
        center_shift.set_rotated_dir(xform_out.get_rotation(), m_center_of_mass_local);
        xform_out.get_translation().sub4(center_shift);
    }
    void approx_body_transform_at(float time, phys_transform &xform_out) const;
    void init_swept_transform(const phys_vec4 &position, const phys_quat &rotation);

    phys_vec4 m_center_of_mass0;
    phys_vec4 m_center_of_mass1;
    phys_quat m_rotation_0;
    phys_quat m_rotation_1;
    phys_vec4 m_center_of_mass_local;
};

class phys_motion_state {
public:
    __forceinline phys_motion_state() {}
    void init_motion_state(const phys_vec4 &position, const phys_quat &rotation);
    phys_motion_state &operator=(const phys_motion_state &other);

    __forceinline phys_transform &get_transform() { return m_body_transform; }
    __forceinline const phys_transform &get_transform() const { return m_body_transform; }
    __forceinline phys_swept_transform &get_swept_transform() { return m_swept_transform; }
    __forceinline const phys_swept_transform &get_swept_transform() const { return m_swept_transform; }

    phys_transform m_body_transform;
    phys_swept_transform m_swept_transform;
    phys_vec4 m_delta_angle;
    float m_object_radius;
    float m_max_lin_vel;
    float m_max_ang_vel;
    float m_linear_damping;
    float m_ang_damping;
    unsigned short m_deactivation_class;
    unsigned short m_deactivation_counter;
    unsigned int m_sleep_ref_orientation[2];
};

__forceinline void phys_vec4::set_mul_mat3_vec3(const phys_mat3 &matrix, const phys_vec4 &vector) {
    const __vector4 x = __vspltw(vector.m_simd_quad, 0);
    const __vector4 y = __vspltw(vector.m_simd_quad, 1);
    const __vector4 z = __vspltw(vector.m_simd_quad, 2);
    const __vector4 partial_sum0 = __vmulfp(matrix.get_column(0).m_simd_quad, x);
    const __vector4 partial_sum1 = __vmaddfp(matrix.get_column(1).m_simd_quad, y, partial_sum0);
    m_simd_quad = __vmaddfp(matrix.get_column(2).m_simd_quad, z, partial_sum1);
}

__forceinline void phys_vec4::set_rotated_dir(const phys_rotation &rotation, const phys_vec4 &direction) {
    set_mul_mat3_vec3(rotation, direction);
}

__forceinline void phys_vec4::set_rotated_inverse_dir(const phys_rotation &rotation, const phys_vec4 &direction) {
    const __vector4 &vector = direction.get_quad();
    const __vector4 &rot_column0 = rotation.get_column(0).get_quad();
    const __vector4 &rot_column1 = rotation.get_column(1).get_quad();
    const __vector4 &rot_column2 = rotation.get_column(2).get_quad();
    __vector4 x = __vmsum3fp(rot_column0, vector);
    __vector4 y = __vmsum3fp(rot_column1, vector);
    __vector4 z = __vmsum3fp(rot_column2, vector);
    __vector4 xz = __vmrghw(x, z);
    __vector4 yw = __vmrghw(y, phys_simd_vec_0000);
    m_simd_quad = __vmrghw(xz, yw);
}

__forceinline void phys_vec4::set_transformed_pos(const phys_transform &transform, const phys_vec4 &position) {
    __vector4 x = __vspltw(position.m_simd_quad, 0);
    __vector4 y = __vspltw(position.m_simd_quad, 1);
    __vector4 z = __vspltw(position.m_simd_quad, 2);
    const phys_rotation &r = transform.get_rotation();
    __vector4 r0 = __vmulfp(r.get_column(0).m_simd_quad, x);
    __vector4 r1 = __vmaddfp(r.get_column(1).m_simd_quad, y, r0);
    __vector4 r2 = __vmaddfp(r.get_column(2).m_simd_quad, z, r1);
    m_simd_quad = __vaddfp(r2, transform.get_translation().m_simd_quad);
}

__forceinline void phys_vec4::set_mul_mat3_vec4(const phys_mat3 &matrix, const phys_vec4 &vector) {
    set_mul_mat3_vec3(matrix, vector);
}

__forceinline void phys_vec4::set_mul_xform_xyz1(const phys_transform &transform, const phys_vec4 &position) {
    set_transformed_pos(transform, position);
}

__forceinline void phys_vec4::set_transformed_inverse_pos(const phys_transform &transform, const phys_vec4 &position) {
    phys_vec4 trans;
    trans.set_sub4(position, transform.m_translation);
    set_rotated_inverse_dir(transform.m_rotation, trans);
}

// March 2007 (0x82bd8830, 0x83468d18): the vector rotated by a unit quaternion q = (v, w) as
// 2 * (d * (w * w - 1/2) + v * (v . d) +/- (v x d) * w).
__forceinline void phys_vec4::vec_set_rotated_dir(const phys_quat &quat, const phys_vec4 &direction) {
    phys_vec4 neg_const;
    neg_const.get_quad() = phys_simd_vec_minus_half;
    phys_vec4 q_coef;
    q_coef.set_broadcast(quat.m_quat_vec, 3);
    phys_vec4 w_pow_coef;
    w_pow_coef.set_add_mul4(neg_const, q_coef, q_coef);
    phys_vec4 ret;
    ret.set_mul4(direction, w_pow_coef);
    phys_vec4 v_term;
    v_term.get_quad() = quat.get_imag().dot3(direction).get_quad();
    v_term.broadcast(0);
    ret.add_scaled4(v_term, quat.get_imag());
    phys_vec4 quat_prod_val;
    quat_prod_val.set_cross(quat.get_imag(), direction);
    ret.add_scaled4(quat_prod_val, q_coef);
    this->set_add4(ret, ret);
}

__forceinline void phys_vec4::vec_set_rotated_inverse_dir(const phys_quat &quat, const phys_vec4 &direction) {
    phys_vec4 neg_const;
    neg_const.get_quad() = phys_simd_vec_minus_half;
    phys_vec4 w;
    w.set_broadcast(quat.m_quat_vec, 3);
    phys_vec4 w_squared_minus_half;
    w_squared_minus_half.set_add_mul4(neg_const, w, w);
    phys_vec4 result;
    result.set_mul4(direction, w_squared_minus_half);
    phys_vec4 imag_dot_dir;
    imag_dot_dir.m_simd_quad = quat.get_imag().dot3(direction).get_quad();
    imag_dot_dir.broadcast(0);
    result.add_scaled4(imag_dot_dir, quat.get_imag());
    phys_vec4 dir_cross_imag;
    dir_cross_imag.set_cross(direction, quat.get_imag());
    result.add_scaled4(dir_cross_imag, w);
    this->set_add4(result, result);
}

__forceinline void phys_vec4::set_transformed_pos(const phys_qs_transform &a, const phys_vec4 &b) {
    phys_vec4 temp(b);
    temp.mul4(a.m_scale);
    vec_set_rotated_dir(a.m_rotation, temp);
    add4(a.m_translation);
}

__forceinline void phys_vec4::set_transformed_inverse_pos(const phys_qs_transform &a, const phys_vec4 &b) {
    phys_vec4 temp(b);
    temp.sub4(a.m_translation);
    vec_set_rotated_inverse_dir(a.m_rotation, temp);
    phys_vec4 math_inv_scale;
    math_inv_scale.set_reciprocal4(a.m_scale);
    mul4(math_inv_scale);
}

typedef char havok_size_hkVector4[(sizeof(phys_vec4) == 16) ? 1 : -1];
typedef char havok_size_hkMatrix3[(sizeof(phys_mat3) == 48) ? 1 : -1];
typedef char havok_size_hkRotation[(sizeof(phys_rotation) == 48) ? 1 : -1];
typedef char havok_size_hkTransform[(sizeof(phys_transform) == 64) ? 1 : -1];
typedef char havok_offset_hkMatrix3_m_col0[(offsetof(phys_mat3, m_column0) == 0) ? 1 : -1];
typedef char havok_offset_hkMatrix3_m_col1[(offsetof(phys_mat3, m_column1) == 16) ? 1 : -1];
typedef char havok_offset_hkMatrix3_m_col2[(offsetof(phys_mat3, m_column2) == 32) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_MATH_H
