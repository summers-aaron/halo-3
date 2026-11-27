// hkconstraintsolver/hkSimpleConstraintUtil.obj: from the records 0x676c10 0x676f78 0x6770b8 0x677180
// @compiler xdk4314
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_simple_constraint_util.h"
#include "phys/phys_vec4_util.h"















namespace phys_vec4_util {
    __forceinline void set_mul(const phys_mat3 &a_from_b, const phys_mat3 &b_from_c, phys_mat3 &a_to_c_out)
    {
        phys_vec4_util::rotate_points(a_from_b, &b_from_c.get_column(0), 3, &a_to_c_out.get_column(0));
    }
    __forceinline void transpose_in_place(phys_mat3 &m)
    {
        float h1 = m(0, 1); m(0, 1) = m(1, 0), m(1, 0) = h1;
        float swap_02 = m(0, 2); m(0, 2) = m(2, 0), m(2, 0) = swap_02;
        float swap_12 = m(1, 2); m(1, 2) = m(2, 1), m(2, 1) = swap_12;
    }
    __forceinline void add(phys_mat3 &m, const phys_mat3 &a)
    {
        m.get_column(0).add4(a.get_column(0));
        m.get_column(1).add4(a.get_column(1));
        m.get_column(2).add4(a.get_column(2));
    }
    __forceinline void invert_symmetric(const phys_mat3 &in, float float_epsilon, phys_mat3 &out)
    {
        phys_vec4 r0; r0.set_cross(in.get_column(1), in.get_column(2));
        phys_vec4 r1; r1.set_cross(in.get_column(2), in.get_column(0));
        phys_vec4 r2; r2.set_cross(in.get_column(0), in.get_column(1));

        phys_simd_scalar determinant = phys_simd_scalar::create(float_epsilon * float_epsilon * float_epsilon) + in.get_column(0).dot3(r0);

        phys_simd_scalar inv_det = phys_simd_scalar::create(1.0f) / determinant;
        out.get_column(0).set_mul4(inv_det, r0);
        out.get_column(1).set_mul4(inv_det, r1);
        out.get_column(2).set_mul4(inv_det, r2);
    }
}

// @address 0x676c10 phys_simple_constraint_init_info
extern "C" void phys_simple_constraint_init_info(const phys_simple_constraint_init_input &agent_body_a, const phys_simple_constraint_init_input &agent_body_b,
    const phys_mat3 &directions, phys_simple_constraint_info &out_constraint_info)
{
    const phys_simple_constraint_init_input *in = &agent_body_a;
    float inv_mass_sum = agent_body_a.m_inv_mass + agent_body_b.m_inv_mass;
    out_constraint_info.m_inv_mass_mat.set_diagonal(inv_mass_sum, inv_mass_sum, inv_mass_sum);

    const phys_vec4 &axis_dir0 = directions.get_column(0);
    const phys_vec4 &axis_dir1 = directions.get_column(1);
    const phys_vec4 &axis_dir2 = directions.get_column(2);
    out_constraint_info.m_base.set_cols(axis_dir0, axis_dir1, axis_dir2);

    for (int i = 0; i < 2; i++)
    {
        phys_simple_constraint_info::body_info &bi = out_constraint_info.m_body_info[i];
        const phys_vec4 &mass_rel_pos = in->m_mass_rel_pos;
        bi.m_jac_transpose.get_column(0).set_cross(mass_rel_pos, axis_dir0);
        bi.m_jac_transpose.get_column(1).set_cross(mass_rel_pos, axis_dir1);
        bi.m_jac_transpose.get_column(2).set_cross(mass_rel_pos, axis_dir2);

        phys_vec4_util::set_mul(in->m_inv_inertia, bi.m_jac_transpose, bi.m_inv_ijac);
        phys_vec4_util::transpose_in_place(bi.m_jac_transpose);

        phys_mat3 m;
        phys_vec4_util::set_mul(bi.m_jac_transpose, bi.m_inv_ijac, m);
        phys_vec4_util::add(out_constraint_info.m_inv_mass_mat, m);

        bi.m_inv_mass = in->m_inv_mass;
        bi.m_body_mass = 1.0f / (bi.m_inv_mass + 1.1920929e-07f);
        in = &agent_body_b;
    }

    phys_vec4_util::invert_symmetric(out_constraint_info.m_inv_mass_mat, 1.1920929e-07f, out_constraint_info.m_mass_mat);

    out_constraint_info.m_inv_mass_mat.get_column(0)(3) = 1.0f / (1.1920929e-07f + out_constraint_info.m_inv_mass_mat(0, 0));
    float det_12 = out_constraint_info.m_inv_mass_mat(1, 1) * out_constraint_info.m_inv_mass_mat(2,
        2) - out_constraint_info.m_inv_mass_mat(1, 2) * out_constraint_info.m_inv_mass_mat(2, 1);
    out_constraint_info.m_inv_mass_mat.get_column(1)(3) = 1.0f / (1.1920929e-07f + det_12);
}

// @address 0x6770b8 phys_simple_constraint_point_vel
extern "C" void phys_simple_constraint_point_vel(const phys_simple_constraint_info &info, phys_body_vel &body_vel_a, phys_body_vel &body_vel_b, phys_vec4 &out)
{
    phys_vec4 jac_ang_a;
    jac_ang_a.set_mul_mat3_vec3(info.m_body_info[0].m_jac_transpose, body_vel_a.m_ang_jacobian);
    phys_vec4 jac_ang_b;
    jac_ang_b.set_mul_mat3_vec3(info.m_body_info[1].m_jac_transpose, body_vel_b.m_ang_jacobian);
    phys_vec4 linear;
    linear.set_sub4(body_vel_a.m_lin_parts, body_vel_b.m_lin_parts);
    jac_ang_a.sub4(jac_ang_b);
    linear.set_rotated_inverse_dir(info.m_base, linear);
    out.set_add4(jac_ang_a, linear);
}

// @address 0x676f78 phys_simple_constraint_apply_impulse
extern "C" void phys_simple_constraint_apply_impulse(const phys_simple_constraint_info &info, const phys_vec4 &impulse,
    phys_body_vel &body_vel_a, phys_body_vel &body_vel_b)
{
    phys_vec4 linear;
    linear.set_mul_mat3_vec3(info.m_base, impulse);
    body_vel_a.m_lin_parts.add_scaled4(phys_simd_scalar::create(info.m_body_info[0].m_inv_mass), linear);
    body_vel_b.m_lin_parts.sub_mul4(phys_simd_scalar::create(info.m_body_info[1].m_inv_mass), linear);

    phys_vec4 jac_ang_a;
    jac_ang_a.set_mul_mat3_vec3(info.m_body_info[0].m_inv_ijac, impulse);
    body_vel_a.m_ang_jacobian.add4(jac_ang_a);

    phys_vec4 jac_ang_b;
    jac_ang_b.set_mul_mat3_vec3(info.m_body_info[1].m_inv_ijac, impulse);
    body_vel_b.m_ang_jacobian.sub4(jac_ang_b);

    body_vel_a.m_ang_jacobian.zero_element(3);
    body_vel_b.m_ang_jacobian.zero_element(3);
    body_vel_a.m_lin_parts.zero_element(3);
    body_vel_b.m_lin_parts.zero_element(3);
}


// @address 0x677180 phys_simple_constraint_collide
extern "C" float phys_simple_constraint_collide(
    const phys_simple_constraint_info &info, const phys_simple_collide_params &params, phys_body_vel &body_vel_a, phys_body_vel &body_vel_b)
{
    phys_vec4 rel_vel;
    phys_simple_constraint_point_vel(info, body_vel_a, body_vel_b, rel_vel);
    if (rel_vel(0) > params.m_external_sep_vel)
    {
        rel_vel(0) = params.m_external_sep_vel;
        if (params.m_external_sep_vel >= params.m_extra_sep_vel)
        {
            return 0.0f;
        }
    }

    rel_vel.set_neg4(rel_vel);
    rel_vel(0) *= (1.0f + params.m_contact_restitution);
    rel_vel(0) += params.m_extra_sep_vel;

    phys_vec4 body_linear_impulse;
    body_linear_impulse.set_mul_mat3_vec3(info.m_mass_mat, rel_vel);
    phys_vec4 impulse_sq;
    impulse_sq.set_mul4(body_linear_impulse, body_linear_impulse);
    float tangent_impulse_sq = impulse_sq(1) + impulse_sq(2);
    float max_friction_imp_sq = params.m_contact_friction * params.m_contact_friction * impulse_sq(0);

    if (tangent_impulse_sq > max_friction_imp_sq)
    {
        float tangent_vel_sq = rel_vel(1) * rel_vel(1) + rel_vel(2) * rel_vel(2);
        body_linear_impulse.set_zero4();

        float x = info.m_inv_mass_mat.get_column(0)(3) * rel_vel(0);
        body_linear_impulse(0) += x;
        rel_vel.sub_mul4(phys_simd_scalar::create(x), info.m_inv_mass_mat.get_column(0));

        float rel_vel1 = rel_vel(1);
        float rel_vel2 = rel_vel(2);
        const phys_mat3 &m = info.m_inv_mass_mat;
        float i1 = info.m_inv_mass_mat.get_column(1)(3) * (rel_vel1 * m(1, 1) - rel_vel2 * m(1, 2));
        float i2 = info.m_inv_mass_mat.get_column(1)(3) * (rel_vel2 * m(2, 2) - rel_vel2 * m(1, 2));
        float imp_sq_total = i1 * i1 + i2 * i2;
        float max_substep_impulse = body_linear_impulse(0) * params.m_contact_friction;
        max_friction_imp_sq = max_substep_impulse * max_substep_impulse;
        if (imp_sq_total > max_friction_imp_sq)
        {
            float f = phys_math::sqrt(max_friction_imp_sq / imp_sq_total);
            i1 *= f;
            i2 *= f;
        }
        body_linear_impulse(1) = i1;
        body_linear_impulse(2) = i2;
        rel_vel.sub_mul4(phys_simd_scalar::create(i1), info.m_inv_mass_mat.get_column(1));
        rel_vel.sub_mul4(phys_simd_scalar::create(i2), info.m_inv_mass_mat.get_column(2));

        float extra = phys_math::max_of_two(0.0f, info.m_inv_mass_mat.get_column(0)(3) * rel_vel(0));
        body_linear_impulse(0) += extra;
        rel_vel.sub_mul4(phys_simd_scalar(extra), info.m_inv_mass_mat.get_column(0));

        float rest_vel_sq = rel_vel(1) * rel_vel(1) + rel_vel(2) * rel_vel(2);
        rest_vel_sq = phys_math::min_of_two(rest_vel_sq, tangent_vel_sq);
        float rest_vel = phys_math::sqrt(rest_vel_sq);
        float mass = info.m_inv_mass_mat.get_column(0)(3);
        body_linear_impulse(0) += mass * rest_vel * params.m_extra_slope;
    }

    phys_simple_constraint_apply_impulse(info, body_linear_impulse, body_vel_a, body_vel_b);
    return body_linear_impulse(0);
}
