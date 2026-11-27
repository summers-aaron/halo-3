// hkconstraintsolver/hk1dBilateralConstraintBuildJacobian.obj: from the records 0x66bad8 0x66bb08 0x66bb30 0x66bb38 0x66bcc0 0x66be70 0x66c038 0x66c248 0x66c3d8 0x66c588 0x66c718 0x66c910 0x66cb10
// @flags /TP /O2
// @compiler xdk4314
// @address 0x66bad8 phys_inv_mass_build_jacobian
// @address 0x66bb08 phys_add_vel_build_jacobian
// @address 0x66bb30 phys_load_vel_accums
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"
#include "phys/phys_schema.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_1d_schemas.h"
#include "phys/phys_1d_bilateral_info.h"


extern "C" void phys_inv_mass_build_jacobian(const phys_vec4 &inv_mass_a, const phys_vec4 &inv_mass_b, phys_constraint_query_out &out)
{
    phys_set_mass_schema *jacobian_schema = reinterpret_cast<phys_set_mass_schema *>(out.m_jacobian_schemas.val());
    jacobian_schema->init_set_mass(inv_mass_a, inv_mass_b);
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
}

extern "C" void phys_add_vel_build_jacobian(const phys_vec4 &delta_vel, int jacobian_body_index, phys_constraint_query_out &out)
{
    phys_add_vel_schema *jacobian_schema = reinterpret_cast<phys_add_vel_schema *>(out.m_jacobian_schemas.val());
    jacobian_schema->m_velocity = delta_vel;
    jacobian_schema->m_body_index = jacobian_body_index;
    phys_schema::set_schema_type(jacobian_schema, phys_schema::SCHEMA_KIND_ADD_VEL);
    out.m_jacobian_schemas = jacobian_schema + 1;
}

extern "C" void phys_load_vel_accums(const phys_constraint_query_in *in)
{
}

static __forceinline void phys_build_ang_jacobian_begin(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &dof, float virt_mass_factor, phys_2ang_jacobian &lin_ang_jac)
{
    if (agent_body_a->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[0] = dof;
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(agent_body_a->get_core_from_world(), dof);
    }
    if (agent_body_b->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[1].set_neg4(dof);
    }
    else
    {
        phys_vec4 neg_dof;
        neg_dof.set_neg4(dof);
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(agent_body_b->get_core_from_world(), neg_dof);
    }
    phys_vec4 ang_sq0;
    ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sq1;
    ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
    ang_sq0.mul4(agent_body_a->m_inv_masses);
    ang_sq0.add_scaled4(ang_sq1, agent_body_b->m_inv_masses);
    float diag_term;
    __stvewx(__vmsum3fp(ang_sq0.m_simd_quad, phys_simd_vec_1111), &diag_term, 0);
    lin_ang_jac.m_ang_jacobian[0](3) = virt_mass_factor / (diag_term + 1.1920929e-7f);
}

// @address 0x66bb38 phys_1d_ang_bilateral_build_jacobian
extern "C" void phys_1d_ang_bilateral_build_jacobian(
    const phys_1d_ang_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    phys_2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_2ang_jacobian *>(out.m_jacobian_schemas.val());
    phys_build_ang_jacobian_begin(in.m_body_a, in.m_body_b, info.m_constrained_dof_w, in.m_virt_mass_factor, lin_ang_jac);
    float rhs = -info.m_perp_zero_err_axis_b.dot3_fpu(info.m_zero_err_axis_a_in_w);
    lin_ang_jac.set_ang_rhs(rhs * in.m_rhs_factor);
    phys_1d_ang_schema *jacobian_schema = reinterpret_cast<phys_1d_ang_schema *>(out.m_jacobian_schemas.val());
    out.m_jacobian_schemas = jacobian_schema + 1;
    phys_schema::set_schema_type(jacobian_schema, phys_schema::SCHEMA_KIND_1D_ANG);
}

// The friction loop sums the two weighted squares as separate products.
static __forceinline void phys_build_ang_friction_jacobian_begin(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &dof, float virt_mass_factor, phys_2ang_jacobian &lin_ang_jac)
{
    if (agent_body_a->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[0] = dof;
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(agent_body_a->get_core_from_world(), dof);
    }
    if (agent_body_b->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[1].set_neg4(dof);
    }
    else
    {
        phys_vec4 neg_dof;
        neg_dof.set_neg4(dof);
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(agent_body_b->get_core_from_world(), neg_dof);
    }
    phys_vec4 ang_sq0;
    ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sq1;
    ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
    ang_sq0.mul4(agent_body_a->m_inv_masses);
    ang_sq1.mul4(agent_body_b->m_inv_masses);
    ang_sq0.add4(ang_sq1);
    float diag_term = ang_sq0.horizontal_add3();
    float d = diag_term + 1.1920929e-7f;
    lin_ang_jac.m_ang_jacobian[0](3) = virt_mass_factor / d;
}

// @address 0x66bcc0 phys_1d_ang_friction_build_jacobian
extern "C" void phys_1d_ang_friction_build_jacobian(const phys_1d_ang_friction_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    const phys_vec4 *dof = info.m_constrained_dof_w;
    phys_solver_results *sr = info.m_last_solver_results.val();
    phys_1d_ang_friction_schema *jacobian_schema = reinterpret_cast<phys_1d_ang_friction_schema *>(out.m_jacobian_schemas.val());
    for (int i = info.m_friction_count - 1; i >= 0; i--)
    {
        phys_2ang_jacobian &lin_ang_jac = jacobian_schema->m_contact_jac;
        phys_build_ang_friction_jacobian_begin(in.m_body_a, in.m_body_b, *dof, 1.0f, lin_ang_jac);
        float rhs = sr->m_internal_solver_data;
        lin_ang_jac.set_ang_rhs(rhs * in.m_substep_inv_dt);
        jacobian_schema->init_1d_ang_friction(lin_ang_jac, info.m_max_friction_torque * in.m_micro_step_dt);
        jacobian_schema++;
        dof++;
        sr++;
    }
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema);
}

static __forceinline void phys_build_lin_jacobian_begin(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &dir, float virt_mass_factor, phys_1lin2ang_jacobian &lin_ang_jac)
{
    lin_ang_jac.m_lin_part_0 = dir;

    phys_vec4 lin_a;
    lin_a.set_sub4(joint_pivot_a, agent_body_a->get_sum_lin_vel());
    phys_vec4 lin_b;
    lin_b.set_sub4(joint_pivot_b, agent_body_b->get_sum_lin_vel());

    phys_vec4 ang_a;
    ang_a.set_cross(lin_a, dir);
    if (!agent_body_a->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(agent_body_a->get_core_from_world(), ang_a);
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[0] = ang_a;
    }

    phys_vec4 ang_b;
    ang_b.set_cross(dir, lin_b);
    if (!agent_body_b->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(agent_body_b->get_core_from_world(), ang_b);
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[1] = ang_b;
    }

    phys_vec4 ang_sq0;
    ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sq1;
    ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
    ang_sq0.mul4(agent_body_a->m_inv_masses);
    ang_sq0.add_scaled4(ang_sq1, agent_body_b->m_inv_masses);
    phys_vec4 sm;
    sm.set_add4(agent_body_a->m_inv_masses, agent_body_b->m_inv_masses);
    ang_sq0.set_w(sm);
    float virtual_mass;
    __stvewx(__vmsum4fp(ang_sq0.m_simd_quad, phys_simd_vec_1111), &virtual_mass, 0);
    float diag_mass = virtual_mass + 1.1920929e-7f;
    lin_ang_jac.m_ang_jacobian[1](3) = diag_mass;
    lin_ang_jac.m_ang_jacobian[0](3) = virt_mass_factor / diag_mass;
}

// @address 0x66c248 ?phys_1d_lin_bilateral_build_jacobian_rhs_no_schema_inline@@YAXABVphys_1d_lin_bilateral_info@@ABVphys_constraint_query_in@@AAVphys_1lin2ang_jacobian@@M@Z
__forceinline void phys_1d_lin_bilateral_build_jacobian_rhs_no_schema_inline(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_1lin2ang_jacobian &lin_ang_jac, float custom_rhs)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    float virt_mass_factor = in.m_virt_mass_factor;
    phys_build_lin_jacobian_begin(agent_body_a, agent_body_b, info.m_pivot_a, info.m_pivot_a, info.m_constrained_dof_w, virt_mass_factor, lin_ang_jac);
    lin_ang_jac.set_rhs(custom_rhs * in.m_rhs_factor);
}

// @address 0x66c3d8 phys_1d_lin_bilateral_build_jacobian_rhs
extern "C" void phys_1d_lin_bilateral_build_jacobian_rhs(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out, float custom_rhs)
{
    phys_1lin2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
    phys_1d_lin_bilateral_build_jacobian_rhs_no_schema_inline(info, in, lin_ang_jac, custom_rhs);
    phys_1d_bilateral_schema *jacobian_schema = reinterpret_cast<phys_1d_bilateral_schema *>(out.m_jacobian_schemas.val());
    phys_schema::set_schema_type(jacobian_schema, phys_schema::SCHEMA_KIND_1D_BILATERAL);
    out.m_jacobian_schemas = jacobian_schema + 1;
}

// @address 0x66c588 phys_1d_lin_bilateral_build_jacobian_rhs_no_schema
extern "C" void phys_1d_lin_bilateral_build_jacobian_rhs_no_schema(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_1lin2ang_jacobian *lin_ang_jac, float custom_rhs)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    float virt_mass_factor = in.m_virt_mass_factor;

    lin_ang_jac->m_lin_part_0 = info.m_constrained_dof_w;

    phys_vec4 lin_a;
    lin_a = agent_body_a->get_sum_lin_vel();
    lin_a.set_sub4(info.m_pivot_a, lin_a);
    phys_vec4 lin_b;
    lin_b = agent_body_b->get_sum_lin_vel();
    lin_b.set_sub4(info.m_pivot_a, lin_b);

    phys_vec4 ang_a;
    ang_a.set_cross(lin_a, info.m_constrained_dof_w);
    if (!agent_body_a->m_mat_is_identity)
    {
        lin_ang_jac->m_ang_jacobian[0].set_rotated_dir(agent_body_a->get_core_from_world(), ang_a);
    }
    else
    {
        lin_ang_jac->m_ang_jacobian[0] = ang_a;
    }

    phys_vec4 ang_b;
    ang_b.set_cross(info.m_constrained_dof_w, lin_b);
    if (!agent_body_b->m_mat_is_identity)
    {
        lin_ang_jac->m_ang_jacobian[1].set_rotated_dir(agent_body_b->get_core_from_world(), ang_b);
    }
    else
    {
        lin_ang_jac->m_ang_jacobian[1] = ang_b;
    }

    phys_vec4 ang_sq0;
    ang_sq0.set_mul4(lin_ang_jac->m_ang_jacobian[0], lin_ang_jac->m_ang_jacobian[0]);
    phys_vec4 ang_sq1;
    ang_sq1.set_mul4(lin_ang_jac->m_ang_jacobian[1], lin_ang_jac->m_ang_jacobian[1]);
    ang_sq0.mul4(agent_body_a->m_inv_masses);
    ang_sq0.add_scaled4(ang_sq1, agent_body_b->m_inv_masses);
    phys_vec4 sm;
    sm.set_add4(agent_body_a->m_inv_masses, agent_body_b->m_inv_masses);
    ang_sq0.set_w(sm);
    float virtual_mass;
    __stvewx(__vmsum4fp(ang_sq0.m_simd_quad, phys_simd_vec_1111), &virtual_mass, 0);
    float diag_mass = virtual_mass + 1.1920929e-7f;
    lin_ang_jac->m_ang_jacobian[1](3) = diag_mass;
    lin_ang_jac->m_ang_jacobian[0](3) = virt_mass_factor / diag_mass;
    lin_ang_jac->set_rhs(in.m_rhs_factor * custom_rhs);
}

// @address 0x66be70 phys_1d_lin_friction_build_jacobian
extern "C" void phys_1d_lin_friction_build_jacobian(const phys_1d_lin_friction_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_1lin2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
    phys_build_lin_jacobian_begin(agent_body_a, agent_body_b, info.m_bilateral_pivot, info.m_bilateral_pivot, info.m_constrained_dof_w,
        in.m_virt_mass_factor, lin_ang_jac);
    float rhs = info.m_last_solver_results->m_internal_solver_data;
    lin_ang_jac.set_rhs(rhs * in.m_rhs_factor);
    phys_1d_friction_schema *jacobian_schema = reinterpret_cast<phys_1d_friction_schema *>(out.m_jacobian_schemas.val());
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
    float max_friction_impulse = info.m_max_friction_force * in.m_micro_step_dt;
    jacobian_schema->init_1d_friction(lin_ang_jac, max_friction_impulse);
}

static __forceinline void phys_build_lin_jacobian_and_rhs(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &dir, float rhs_factor, float virt_mass_factor,
        phys_1lin2ang_jacobian &lin_ang_jac)
{
    phys_build_lin_jacobian_begin(agent_body_a, agent_body_b, joint_pivot_a, joint_pivot_b, dir, virt_mass_factor, lin_ang_jac);

    phys_vec4 delta;
    delta.set_sub4(joint_pivot_b, joint_pivot_a);
    float rhs = delta.dot3(dir);
    lin_ang_jac.set_rhs(rhs * rhs_factor);
}

// The ball-socket loop sums the two weighted squares as separate products.
static __forceinline void phys_build_lin_jacobian_and_rhs_loop(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &dir, float rhs_factor, float virt_mass_factor,
        phys_1lin2ang_jacobian &lin_ang_jac)
{
    lin_ang_jac.m_lin_part_0 = dir;

    phys_vec4 lin_a;
    lin_a.set_sub4(joint_pivot_a, agent_body_a->get_sum_lin_vel());
    phys_vec4 lin_b;
    lin_b.set_sub4(joint_pivot_b, agent_body_b->get_sum_lin_vel());

    phys_vec4 ang_a;
    ang_a.set_cross(lin_a, dir);
    if (!agent_body_a->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(agent_body_a->get_core_from_world(), ang_a);
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[0] = ang_a;
    }

    phys_vec4 ang_b;
    ang_b.set_cross(dir, lin_b);
    if (!agent_body_b->m_mat_is_identity)
    {
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(agent_body_b->get_core_from_world(), ang_b);
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[1] = ang_b;
    }

    phys_vec4 ang_sq0;
    ang_sq0.set_mul4(lin_ang_jac.m_ang_jacobian[0], lin_ang_jac.m_ang_jacobian[0]);
    phys_vec4 ang_sq1;
    ang_sq1.set_mul4(lin_ang_jac.m_ang_jacobian[1], lin_ang_jac.m_ang_jacobian[1]);
    ang_sq0.mul4(agent_body_a->m_inv_masses);
    ang_sq1.mul4(agent_body_b->m_inv_masses);
    ang_sq0.add4(ang_sq1);
    phys_vec4 sm;
    sm.set_add4(agent_body_a->m_inv_masses, agent_body_b->m_inv_masses);
    ang_sq0.set_w(sm);
    float virtual_mass;
    __stvewx(__vmsum4fp(ang_sq0.m_simd_quad, phys_simd_vec_1111), &virtual_mass, 0);
    float diag_mass = virtual_mass + 1.1920929e-7f;
    lin_ang_jac.m_ang_jacobian[1](3) = diag_mass;
    lin_ang_jac.m_ang_jacobian[0](3) = virt_mass_factor / diag_mass;

    phys_vec4 delta;
    delta.set_sub4(joint_pivot_b, joint_pivot_a);
    float rhs = delta.dot3(dir);
    lin_ang_jac.set_rhs(rhs * rhs_factor);
}

// @address 0x66c718 phys_1d_lin_bilateral_user_tau_build_jacobian
extern "C" void phys_1d_lin_bilateral_user_tau_build_jacobian(
    const phys_1d_lin_bilateral_tau_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_1lin2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
    float virt_mass_factor = 1.0f;
    phys_build_lin_jacobian_and_rhs(agent_body_a, agent_body_b, info.m_pivot_a, info.m_pivot_b, info.m_constrained_dof_w,
        in.m_rhs_factor, virt_mass_factor, lin_ang_jac);
    phys_1d_bilateral_user_tau_schema *jacobian_schema = reinterpret_cast<phys_1d_bilateral_user_tau_schema *>(out.m_jacobian_schemas.val());
    jacobian_schema->init_1d_bilateral_user_tau(lin_ang_jac, info.m_query_tau, info.m_solver_damping);
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
}

// @address 0x66c038 phys_1d_lin_bilateral_build_jacobian
extern "C" void phys_1d_lin_bilateral_build_jacobian(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    phys_1lin2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_build_lin_jacobian_and_rhs(agent_body_a, agent_body_b, info.m_pivot_a, info.m_pivot_a, info.m_constrained_dof_w,
        in.m_rhs_factor, in.m_virt_mass_factor, lin_ang_jac);
    phys_vec4 separation_vec;
    separation_vec.set_sub4(info.m_pivot_b, info.m_pivot_a);
    float rhs = separation_vec.dot3(info.m_constrained_dof_w);
    lin_ang_jac.set_rhs(rhs * in.m_rhs_factor);
    phys_1d_bilateral_schema *jacobian_schema = reinterpret_cast<phys_1d_bilateral_schema *>(out.m_jacobian_schemas.val());
    jacobian_schema->init_1d_bilateral(lin_ang_jac);
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
}

// @address 0x66c910 phys_1d_lin_limit_build_jacobian
extern "C" void phys_1d_lin_limit_build_jacobian(const phys_1d_lin_limit_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_1lin2ang_jacobian &lin_ang_jac = *reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
    phys_build_lin_jacobian_and_rhs(agent_body_a, agent_body_b, info.m_pivot_a, info.m_pivot_b, info.m_constrained_dof_w,
        in.m_rhs_factor, in.m_virt_mass_factor, lin_ang_jac);
    float neg_rhs_factor = -in.m_rhs_factor;
    float bilateral_min_limit = info.m_min_limit * neg_rhs_factor;
    float bilateral_max_limit = info.m_max_limit * neg_rhs_factor;
    phys_1d_lin_limits_schema *jacobian_schema = reinterpret_cast<phys_1d_lin_limits_schema *>(out.m_jacobian_schemas.val());
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
    jacobian_schema->init_1d_lin_limits(lin_ang_jac, bilateral_min_limit, bilateral_max_limit);
}

// @address 0x66cb10 phys_stable_ball_socket_jacobian
extern "C" void phys_stable_ball_socket_jacobian(const phys_vec4 &pos_a, const phys_vec4 &pos_b,
    float max_allowed_error_dist, const phys_constraint_query_in &in, phys_1lin2ang_jacobian *lin_ang_jac)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_ball_socket_build_jacobian_no_schema(pos_a, pos_b, in, lin_ang_jac);

    phys_vec4 rhs;
    rhs(0) = lin_ang_jac[0].get_rhs();
    rhs(1) = lin_ang_jac[1].get_rhs();
    rhs(2) = lin_ang_jac[2].get_rhs();

    float cur_error_len = rhs.length_3d();
    float max_error_dist = in.m_rhs_factor * max_allowed_error_dist;
    if (cur_error_len > max_error_dist)
    {
        float factor = phys_math::sqrt(max_error_dist / cur_error_len);
        factor = phys_math::sqrt(factor);

        phys_vec4 new_pos_a;
        // match: 0x66cb10 interpolates towards copies of the pivots (assigned, so they are vector copies).
        phys_vec4 joint_pivot_a;
        joint_pivot_a = pos_a;
        new_pos_a.set_lerp4(agent_body_a->get_sum_lin_vel(), joint_pivot_a, phys_simd_scalar(factor));
        phys_vec4 new_pos_b;
        phys_vec4 joint_pivot_b;
        joint_pivot_b = pos_b;
        new_pos_b.set_lerp4(agent_body_b->get_sum_lin_vel(), joint_pivot_b, phys_simd_scalar(factor));

        const phys_vec4 *dir = &phys_transform::get_identity().get_rotation().get_column(0);
        for (int i = 0; i < 3; i++)
        {
            phys_build_lin_jacobian_and_rhs_loop(agent_body_a, agent_body_b, new_pos_a, new_pos_b, *dir, in.m_rhs_factor,
                in.m_virt_mass_factor, lin_ang_jac[i]);
            dir++;
        }

        float rhs_scale = factor;
        lin_ang_jac[0].set_rhs(rhs(0) * rhs_scale);
        lin_ang_jac[1].set_rhs(rhs(1) * rhs_scale);
        lin_ang_jac[2].set_rhs(rhs(2) * rhs_scale);
    }
}

