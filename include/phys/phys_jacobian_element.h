#ifndef INCLUDE_PHYS_JACOBIAN_ELEMENT_H
#define INCLUDE_PHYS_JACOBIAN_ELEMENT_H

#include "phys/phys_math.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_vel_accum.h"

class phys_2ang_jacobian {
public:
    phys_vec4 m_ang_jacobian[2];
    __forceinline float get_ang_rhs() const { return m_ang_jacobian[1](3); }
    __forceinline void set_ang_rhs(float rhs) { m_ang_jacobian[1](3) = rhs; }
    __forceinline float get_inv_jac_diag() const { return m_ang_jacobian[0](3); }
    __forceinline void set_inv_jac_diag(float value) { m_ang_jacobian[0](3) = value; }
    __forceinline phys_simd_scalar get_inv_jac_diag_simd() const { return m_ang_jacobian[0].get_simd_at(3); }
    __forceinline float get_ang_diag(const phys_vel_accum &mA, const phys_vel_accum &mB) const {
        const phys_2ang_jacobian &lin_ang_jac = *this;
        phys_vec4 ang_sq0; ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
        ang_sq0.mul4(mA.m_inv_masses);
        ang_sq1.mul4(mB.m_inv_masses);
        ang_sq0.add4(ang_sq1);
        float float_epsilon = 1.1920929e-7f;
        float x = ang_sq0.horizontal_add3();
        return x + float_epsilon;
    }

    // The off-diagonal element of the constraint matrix for two angular jacobians on the same body pair.
    __forceinline float get_off_diag_fast(const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_2ang_jacobian &jac_b) const {
        const phys_2ang_jacobian &jac_a = *this;
        phys_vec4 ang_sq0;
        ang_sq0.set_mul4(jac_a.m_ang_jacobian[0], jac_b.m_ang_jacobian[0]);
        phys_vec4 ang_sq1;
        ang_sq1.set_mul4(jac_a.m_ang_jacobian[1], jac_b.m_ang_jacobian[1]);
        ang_sq0.mul4(mA.m_inv_masses);
        ang_sq1.mul4(mB.m_inv_masses);
        ang_sq0.add4(ang_sq1);
        return ang_sq0.horizontal_add3();
    }
    __forceinline float get_off_diag_same_bodies(const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_2ang_jacobian &jac_b) const {
        return get_off_diag_fast(mA, mB, jac_b);
    }
    // jacB acts on this jacobian's second body as its first body.
    __forceinline float get_off_diag_shared_body(const phys_vel_accum &mB, const phys_2ang_jacobian &jac_b) const {
        const phys_2ang_jacobian &jac_a = *this;
        phys_vec4 ang_sq1;
        ang_sq1.set_mul4(jac_a.m_ang_jacobian[1], jac_b.m_ang_jacobian[0]);
        ang_sq1.mul4(mB.m_inv_masses);
        return ang_sq1.horizontal_add3();
    }
};

class phys_1lin2ang_jacobian {
public:
    phys_vec4 m_lin_part_0;
    phys_vec4 m_ang_jacobian[2];
    __forceinline const float &get_rhs() const { return m_lin_part_0(3); }
    __forceinline void set_rhs(float rhs) { m_lin_part_0(3) = rhs; }
    __forceinline float get_inv_jac_diag() const { return m_ang_jacobian[0](3); }
    __forceinline void set_inv_jac_diag(float value) { m_ang_jacobian[0](3) = value; }
    __forceinline void set_user_value(float value) { m_ang_jacobian[1](3) = value; }
    __forceinline float get_user_value() const { return m_ang_jacobian[1](3); }
    __forceinline phys_simd_scalar get_inv_jac_diag_simd() const { return m_ang_jacobian[0].get_simd_at(3); }
    __forceinline float get_jacobian_diag(const phys_vel_accum &mA, const phys_vel_accum &mB) const {
        const phys_1lin2ang_jacobian &lin_ang_jac = *this;
        phys_vec4 ang_sq0; ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
        phys_vec4 ang_sq1; ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
        ang_sq0.mul4(mA.m_inv_masses);
        ang_sq1.mul4(mB.m_inv_masses);
        ang_sq0.add4(ang_sq1);
        phys_vec4 inv_masses_sum; inv_masses_sum.set_add4(mA.m_inv_masses, mB.m_inv_masses);
        ang_sq0.set_w(inv_masses_sum);
        float ret = ang_sq0.horizontal_add4();
        return ret + 1.1920929e-7f;
    }
    __forceinline phys_2ang_jacobian &as_2ang_jacobian() { return *reinterpret_cast<phys_2ang_jacobian *>(&m_ang_jacobian[0]); }

    // The off-diagonal element of the constraint matrix for two jacobians on the same body pair.
    __forceinline float get_off_diag_fast(const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_1lin2ang_jacobian &jac_b) const {
        const phys_1lin2ang_jacobian &jac_a = *this;
        phys_vec4 lin_part;
        lin_part.set_mul4(jac_a.m_lin_part_0, jac_b.m_lin_part_0);
        phys_vec4 ang_sq0;
        ang_sq0.set_mul4(jac_a.m_ang_jacobian[0], jac_b.m_ang_jacobian[0]);
        phys_vec4 ang_sq1;
        ang_sq1.set_mul4(jac_a.m_ang_jacobian[1], jac_b.m_ang_jacobian[1]);
        phys_vec4 mass_a;
        mass_a.set_broadcast3_clobber_w(mA.m_inv_masses, 3);
        phys_vec4 mass_b;
        mass_b.set_broadcast3_clobber_w(mB.m_inv_masses, 3);
        phys_vec4 lin_a;
        lin_a.set_mul4(mass_a, lin_part);
        phys_vec4 lin_b;
        lin_b.set_mul4(mass_b, lin_part);
        ang_sq0.mul4(mA.m_inv_masses);
        ang_sq1.mul4(mB.m_inv_masses);
        lin_a.add4(lin_b);
        ang_sq0.add4(ang_sq1);
        ang_sq0.add4(lin_a);
        return ang_sq0.horizontal_add3();
    }
    __forceinline float get_off_diag_same_bodies(const phys_vel_accum &mA, const phys_vel_accum &mB, const phys_1lin2ang_jacobian &jac_b) const {
        return get_off_diag_fast(mA, mB, jac_b);
    }
    // jacB acts on this jacobian's second body as its first body.
    __forceinline float get_off_diag_shared_body(const phys_vel_accum &mB, const phys_1lin2ang_jacobian &jac_b) const {
        const phys_1lin2ang_jacobian &jac_a = *this;
        phys_vec4 lin_part;
        lin_part.set_mul4(jac_a.m_lin_part_0, jac_b.m_lin_part_0);
        phys_vec4 ang_sq1;
        ang_sq1.set_mul4(jac_a.m_ang_jacobian[1], jac_b.m_ang_jacobian[0]);
        phys_vec4 mass;
        mass.set_broadcast(mB.m_inv_masses, 3);
        phys_vec4 lin_mid;
        lin_mid.set_mul4(mass, lin_part);
        ang_sq1.mul4(mB.m_inv_masses);
        ang_sq1.sub4(lin_mid);
        return ang_sq1.horizontal_add3();
    }
};

class phys_2lin2ang_jacobian {
public:
    phys_vec4 m_lin_parts[2];
    phys_vec4 m_ang_jacobian[2];
    __forceinline float get_rhs() const { return m_lin_parts[0](3); }
    __forceinline void set_rhs(float v) { m_lin_parts[0](3) = v; }
    __forceinline void set_inv_jac_diag(float v) { m_ang_jacobian[0](3) = v; }
    __forceinline float get_jacobian_diag(const phys_vel_accum &mA, const phys_vel_accum &mB, float leverage_ratio) const {
        phys_vec4 ang_sq0; ang_sq0.m_simd_quad = __vmulfp(m_ang_jacobian[0].m_simd_quad, m_ang_jacobian[0].m_simd_quad);
        phys_vec4 ang_sq1; ang_sq1.m_simd_quad = __vmulfp(m_ang_jacobian[1].m_simd_quad, m_ang_jacobian[1].m_simd_quad);
        ang_sq0.m_simd_quad = __vmulfp(ang_sq0.m_simd_quad, mA.m_inv_masses.m_simd_quad);
        ang_sq1.m_simd_quad = __vmulfp(ang_sq1.m_simd_quad, mB.m_inv_masses.m_simd_quad);
        leverage_ratio = leverage_ratio * leverage_ratio;
        ang_sq0.add4(ang_sq1);
        float x = mA.m_inv_masses.m_simd_quad.w + mB.m_inv_masses.m_simd_quad.w * leverage_ratio + 1.1920929e-07f;
        return float(phys_simd_scalar(__vmsum3fp(ang_sq0.m_simd_quad, phys_simd_vec_1111))) + x;
    }
};

typedef char phys_1lin2ang_jacobian_size[(sizeof(phys_1lin2ang_jacobian) == 48) ? 1 : -1];
typedef char phys_2ang_jacobian_size[(sizeof(phys_2ang_jacobian) == 32) ? 1 : -1];
typedef char phys_2lin2ang_jacobian_size[(sizeof(phys_2lin2ang_jacobian) == 64) ? 1 : -1];

#endif
