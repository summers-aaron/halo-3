#ifndef INCLUDE_PHYS_1D_BILATERAL_INFO_H
#define INCLUDE_PHYS_1D_BILATERAL_INFO_H

// The 1d constraint descriptions the atom builders fill in, and the jacobian builders that read them.
// The builders are defined in tu/hkconstraintsolver/hk1dBilateralConstraintBuildJacobian.cpp.

#include "phys/phys_math.h"

class phys_solver_results;
class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_1lin2ang_jacobian;

class phys_1d_lin_bilateral_info {
public:
    inline phys_1d_lin_bilateral_info() {}

    phys_vec4 m_pivot_a;
    phys_vec4 m_pivot_b;
    phys_vec4 m_constrained_dof_w;
};

class phys_1d_lin_bilateral_tau_info : public phys_1d_lin_bilateral_info {
public:
    phys_padded<float> m_query_tau;
    phys_padded<float> m_solver_damping;
};

class phys_1d_ang_bilateral_info {
public:
    inline phys_1d_ang_bilateral_info() {}

    phys_vec4 m_zero_err_axis_a_in_w;
    phys_vec4 m_constrained_dof_w;
    phys_vec4 m_perp_zero_err_axis_b;
};

class phys_1d_lin_limit_info : public phys_1d_lin_bilateral_info {
public:
    phys_padded<float> m_min_limit;
    phys_padded<float> m_max_limit;
};

class phys_1d_ang_friction_info {
public:
    phys_padded<const phys_vec4 *> m_constrained_dof_w;
    phys_padded<phys_solver_results *> m_last_solver_results;
    phys_padded<float> m_max_friction_torque;
    phys_padded<int> m_friction_count;
};

class phys_1d_lin_friction_info {
public:
    phys_vec4 m_bilateral_pivot;
    phys_vec4 m_constrained_dof_w;
    phys_padded<float> m_max_friction_force;
    phys_padded<phys_solver_results *> m_last_solver_results;
};

class phys_1d_ang_limit_info {
public:
    phys_vec4 m_constrained_dof_w;
    phys_padded<float> m_min_limit;
    phys_padded<float> m_max_limit;
    phys_padded<float> m_computed_angle;
    phys_padded<float> m_query_tau;
};

struct phys_pulley_info {
    phys_vec4 m_pos_a;
    phys_vec4 m_pos_b;
    phys_vec4 m_pulley_pivot_a;
    phys_vec4 m_pulley_pivot_b;
    phys_padded<float> m_rope_length;
    phys_padded<float> m_leverage_on_b;
};

extern "C" {
void phys_1d_lin_bilateral_build_jacobian(const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_lin_bilateral_build_jacobian_rhs(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out, float custom_rhs);
void phys_1d_lin_bilateral_build_jacobian_rhs_no_schema(
    const phys_1d_lin_bilateral_info &info, const phys_constraint_query_in &in, phys_1lin2ang_jacobian *lin_ang_jac, float custom_rhs);
void phys_1d_lin_bilateral_user_tau_build_jacobian(
    const phys_1d_lin_bilateral_tau_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_ang_bilateral_build_jacobian(const phys_1d_ang_bilateral_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_lin_limit_build_jacobian(const phys_1d_lin_limit_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_ang_limit_build_jacobian(const phys_1d_ang_limit_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_ang_friction_build_jacobian(const phys_1d_ang_friction_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_1d_lin_friction_build_jacobian(const phys_1d_lin_friction_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out);
void phys_inv_mass_build_jacobian(const phys_vec4 &inv_mass_a, const phys_vec4 &inv_mass_b, phys_constraint_query_out &out);
void phys_add_vel_build_jacobian(const phys_vec4 &delta_vel, int jacobian_body_index, phys_constraint_query_out &out);
void phys_load_vel_accums(const phys_constraint_query_in *in);
void phys_ball_socket_build_jacobian_no_schema(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b,
    const phys_constraint_query_in &in, phys_1lin2ang_jacobian *lin_ang_jac);
void phys_stable_ball_socket_jacobian(
    const phys_vec4 &pos_a, const phys_vec4 &pos_b, float max_allowed_error_dist, const phys_constraint_query_in &in, phys_1lin2ang_jacobian *lin_ang_jac);
}

void phys_pulley_build_jacobian(const phys_pulley_info &pulley_info, const phys_constraint_query_in &in, phys_constraint_query_out &out);

typedef char phys_1d_lin_bilateral_info_size[(sizeof(phys_1d_lin_bilateral_info) == 48) ? 1 : -1];
typedef char phys_1d_lin_user_tau_info_size[(sizeof(phys_1d_lin_bilateral_tau_info) == 64) ? 1 : -1];
typedef char phys_1d_lin_limit_info_size[(sizeof(phys_1d_lin_limit_info) == 64) ? 1 : -1];
typedef char phys_1d_ang_friction_info_size[(sizeof(void *) != 4 || sizeof(phys_1d_ang_friction_info) == 16) ? 1 : -1];
typedef char phys_1d_lin_friction_info_size[(sizeof(phys_1d_lin_friction_info) == 48) ? 1 : -1];
typedef char phys_1d_ang_limit_info_size[(sizeof(phys_1d_ang_limit_info) == 32) ? 1 : -1];
typedef char phys_pulley_info_size[(sizeof(phys_pulley_info) == 80) ? 1 : -1];

#endif // HAVOK_HK_1D_BILATERAL_CONSTRAINT_INFO_H
