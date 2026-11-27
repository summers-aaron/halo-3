// hkconstraintsolver/hkMotorBuildJacobian.obj: from the records 0x666598 0x666790 0x666918
// @compiler xdk4314
// @flags /TP /O2 /Oc
// @address 0x666918 phys_1d_lin_vel_motor_commit_jacobian
#include <math.h>
#include "phys/phys_1d_motor_solver_info.h"
#include "phys/phys_1d_lin_motor_schema.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_dynamics.h"
#include "phys/phys_motor_constraint_info.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"
#include "phys/phys_math.h"
namespace {
struct phys_1d_lin_vel_motor_info {
    float m_target_vel;
    float m_query_tau;
    float m_min_force;
    float m_max_force;
    float m_solver_damping;
    float m_rhs_factor;
};
}
class phys_velocity_motor : public phys_limited_force_motor
{
public:
    float m_query_tau;
    float m_vel_target;
    phys_bool m_use_constraint_vel_target;
};
class phys_callback_motor : public phys_limited_force_motor
{
public:
    typedef void (*motor_data_callback_fn)(const phys_callback_motor *motor, const phys_motor_input *input, phys_motor_output *output);
    motor_data_callback_fn m_callback_fn;
};
static __forceinline float phys_motor_clamp(float x, float mi, float ma)
{
    if (x < mi) return mi;
    if (x > ma) return ma;
    return x;
}
// @address 0x666598 phys_compute_motor_data
extern "C" void phys_compute_motor_data(const phys_motor *generic_motor, const phys_motor_input *input, phys_motor_output *output)
{
    float target_vel;
    float damping;
    float motor_target_pos;
    float min_force;
    float tau;
    float motor_max_force;
    switch (generic_motor->get_type_id())
    {
    case phys_motor::motor_kind_position:
    {
        const phys_position_motor *motor = static_cast<const phys_position_motor *>(generic_motor);
        float delta_target = input->m_delta_target;
        float position_error = input->m_pos_error;
        float velocity = position_error * motor->m_proportional_recovery_vel * input->m_step_info->m_frame_dt;
        float remaining = position_error - velocity;
        float max_recovery_vel = motor->m_const_recovery_vel * input->m_step_info->m_frame_dt;
        if (fabsf(remaining) > max_recovery_vel)
        {
            velocity += (remaining > 0.0f) ? max_recovery_vel : -max_recovery_vel;
        }
        else
        {
            velocity = position_error;
        }
        float abs_pos_error = fabsf(position_error);
        velocity = phys_motor_clamp(velocity, -abs_pos_error - input->m_last_results.m_internal_solver_data,
            abs_pos_error - input->m_last_results.m_internal_solver_data);
        target_vel = (delta_target + velocity) * input->m_step_info->m_frame_inv_dt;
        motor_target_pos = input->m_last_results.m_internal_solver_data;
        motor_max_force = motor->m_motor_max_force;
        min_force = -motor_max_force;
        tau = motor->m_tau_scale;
        damping = motor->m_motor_damping;
        break;
    }
    case phys_motor::motor_kind_velocity:
    {
        const phys_velocity_motor *motor = static_cast<const phys_velocity_motor *>(generic_motor);
        if (motor->m_use_constraint_vel_target)
        {
            target_vel = (input->m_delta_target + input->m_pos_error) * input->m_step_info->m_frame_inv_dt;
        }
        else
        {
            target_vel = motor->m_vel_target;
        }
        motor_target_pos = input->m_last_results.m_internal_solver_data;
        tau = motor->m_query_tau;
        damping = motor->m_query_tau;
        motor_max_force = motor->m_motor_max_force;
        min_force = motor->m_motor_min_force;
        break;
    }
    case phys_motor::motor_kind_spring_damper:
    {
        const phys_spring_damper_motor *motor = static_cast<const phys_spring_damper_motor *>(generic_motor);
        target_vel = 0.0f;
        motor_target_pos = input->m_delta_target + input->m_pos_error;
        float inv_virtual_mass = 1.0f / input->m_virtual_mass;
        float dt = input->m_step_info->m_substep_dt;
        float spring_tau = motor->m_spring_stiffness * dt * dt * inv_virtual_mass;
        tau = phys_motor_clamp(spring_tau, 0.0f, 1.0f);
        float motor_spring_damping = motor->m_spring_damp * dt * inv_virtual_mass;
        damping = phys_motor_clamp(motor_spring_damping, 0.0f, 1.0f);
        motor_max_force = motor->m_motor_max_force;
        min_force = motor->m_motor_min_force;
        break;
    }
    case phys_motor::motor_kind_callback:
    {
        const phys_callback_motor *motor = static_cast<const phys_callback_motor *>(generic_motor);
        motor->m_callback_fn(motor, input, output);
        return;
    }
    default:
        return;
    }
    output->m_target_vel = target_vel;
    output->m_target_pos = motor_target_pos;
    output->m_max_force = motor_max_force;
    output->m_min_force = min_force;
    output->m_query_tau = tau;
    output->m_solver_damping = damping;
}
static __forceinline void phys_motor_rotate_direction(phys_vec4 &out, const phys_rotation &rotation, const phys_vec4 &direction)
{
    out.m_simd_quad = __vmaddfp(rotation.get_column(2).m_simd_quad, __vspltw(direction.m_simd_quad, 2),
        __vmaddfp(rotation.get_column(1).m_simd_quad, __vspltw(direction.m_simd_quad, 1),
            __vmulfp(rotation.get_column(0).m_simd_quad, __vspltw(direction.m_simd_quad, 0))));
}
static __forceinline void phys_motor_build_lin_begin(const phys_vel_accum *agent_body_a, const phys_vel_accum *agent_body_b,
    const phys_vec4 &pivot, const phys_vec4 &dir, float inv_jac_factor, phys_1lin2ang_jacobian &lin_ang_jac)
{
    lin_ang_jac.m_lin_part_0 = dir;

    phys_vec4 lin_a;
    lin_a.set_sub4(pivot, agent_body_a->get_center_of_mass_in_world());
    phys_vec4 lin_b;
    lin_b.set_sub4(pivot, agent_body_b->get_center_of_mass_in_world());

    phys_vec4 ang_a;
    ang_a.set_cross(lin_a, dir);
    if (!agent_body_a->m_mat_is_identity)
    {
        phys_motor_rotate_direction(lin_ang_jac.m_ang_jacobian[0], agent_body_a->get_core_from_world(), ang_a);
    }
    else
    {
        lin_ang_jac.m_ang_jacobian[0] = ang_a;
    }

    phys_vec4 ang_b;
    ang_b.set_cross(dir, lin_b);
    if (!agent_body_b->m_mat_is_identity)
    {
        phys_motor_rotate_direction(lin_ang_jac.m_ang_jacobian[1], agent_body_b->get_core_from_world(), ang_b);
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
    ang_sq0.m_simd_quad = __vrlimi(ang_sq0.m_simd_quad, sm.m_simd_quad, 1, 0);
    float virtual_mass;
    __stvewx(__vmsum4fp(ang_sq0.m_simd_quad, phys_simd_vec_1111), &virtual_mass, 0);
    float diag_mass = virtual_mass + 1.1920929e-7f;
    lin_ang_jac.m_ang_jacobian[1](3) = diag_mass;
    lin_ang_jac.m_ang_jacobian[0](3) = inv_jac_factor / diag_mass;
}
// @address 0x666790 phys_1d_lin_vel_motor_begin_jacobian
extern "C" void phys_1d_lin_vel_motor_begin_jacobian(const phys_vec4 &constraint_dir, const phys_vec4 &pivot,
    const phys_constraint_query_in &in, phys_1lin2ang_jacobian &lin_ang_jac, phys_motor_input &status_out)
{
    phys_motor_build_lin_begin(in.m_body_a, in.m_body_b, pivot, constraint_dir, 1.0f, lin_ang_jac);
    status_out.m_virtual_mass = lin_ang_jac.get_inv_jac_diag();
}
extern "C" void phys_1d_lin_vel_motor_commit_jacobian(
    const phys_1d_lin_vel_motor_info &info, const phys_constraint_query_in &in, phys_1lin2ang_jacobian &jacobian_elem, phys_constraint_query_out &out)
{
    float rhs = info.m_target_vel * in.m_substep_inv_dt;
    jacobian_elem.set_rhs(rhs);
    phys_1d_lin_motor_schema *jacobian_schema = reinterpret_cast<phys_1d_lin_motor_schema  *>(out.m_jacobian_schemas.val());
    phys_1d_motor_solver_info &motor = jacobian_schema->init_1d_lin_motor(jacobian_elem);
    motor.m_min_impulse = info.m_min_force * in.m_micro_step_dt;
    motor.m_max_impulse = info.m_max_force * in.m_micro_step_dt;
    motor.m_query_tau = info.m_query_tau;
    motor.m_solver_damping = info.m_solver_damping;
    motor.m_rhs_factor = info.m_rhs_factor;
    out.m_jacobian_schemas = reinterpret_cast<phys_schema *>(jacobian_schema + 1);
}

typedef char phys_vel_accum_size[(sizeof(phys_vel_accum) == 128) ? 1 : -1];
typedef char phys_vel_accum_lin_offset[((unsigned int)&((phys_vel_accum *)0)->m_lin_vel == 16) ? 1 : -1];

