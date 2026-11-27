#ifndef INCLUDE_PHYS_POWERED_CHAIN_SOLVER_UTIL_H
#define INCLUDE_PHYS_POWERED_CHAIN_SOLVER_UTIL_H

// Functions defined in tu/hkconstraintsolver/hkPoweredChainSolverUtil.cpp.

#include "phys/phys_cfm_param.h"

struct phys_3d_ang_motor_solver_info;
struct phys_chain_solver_info;
struct phys_chain_mat6_triple;
class phys_vel_accum;
class phys_vel_accum_offset;
class phys_1lin2ang_jacobian;
class phys_2ang_jacobian;
class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_vec8;

// What hkPoweredChainData::buildJacobian hands the chain builder once the per-link jacobians are written.
struct phys_powered_chain_jacobian_params {
    int m_constraint_count;
    float m_chain_tau;
    float m_chain_damping;
    phys_cfm_param m_cfm_param;
    phys_vel_accum_offset *m_vel_accums;
    const phys_vel_accum *m_accums_base;
    phys_3d_ang_motor_solver_info *m_motors_state;
    float m_max_torque_hyst;
    unsigned char *m_child_status_writeback_buf;
    phys_2ang_jacobian *m_jacobians_end;
};

typedef char phys_powered_chain_build_jacobian_params_size[(sizeof(void *) != 4 || sizeof(phys_powered_chain_jacobian_params) == 52) ? 1 : -1];

extern "C" void phys_powered_chain_build_jacobian(const phys_powered_chain_jacobian_params &params, const phys_constraint_query_in &in,
    phys_constraint_query_out &out);
extern "C" void phys_powered_chain_disable_motors(const phys_chain_solver_info &info, int &modified_child_constraint_idx, int &modified_coord_idx,
    float &impulse, phys_3d_ang_motor_solver_info *motor_states);
extern "C" void phys_powered_chain_enable_motors(const phys_chain_solver_info &info, int &modified_child_constraint_idx, int &modified_coord_idx,
    float &impulse, phys_3d_ang_motor_solver_info *motor_states);
extern "C" void phys_powered_chain_update_lu(int first_constraint_idx, int solver_constraint_count, phys_3d_ang_motor_solver_info *motor_states,
    phys_chain_mat6_triple *chain_triples);
extern "C" void phys_powered_chain_calc_velocities(const phys_chain_solver_info &info, phys_vec8 *velocities);
extern "C" void phys_powered_chain_build_lu_matrix(int solver_constraint_count, const phys_cfm_param &constraint_force_mix,
    phys_3d_ang_motor_solver_info *motor_states,
    phys_1lin2ang_jacobian *j, phys_2ang_jacobian *ang_jac, phys_vel_accum_offset *va, const phys_vel_accum *accums_base,
    phys_chain_mat6_triple *chain_triples, int solver_buffer_size);

extern "C" void phys_powered_chain_set_impulse_vel(int modified_constraint_idx, int modified_coord_idx,
    const phys_3d_ang_motor_solver_info *motor_states, phys_vec8 *velocities);
extern "C" void phys_powered_chain_restore_vel(int modified_constraint_idx, int modified_coord_idx, const phys_chain_solver_info &info,
    phys_vec8 *velocities);
extern "C" void phys_powered_chain_solve_vec_g(const phys_chain_solver_info &info, phys_vec8 *temp_buffer_g);

#endif // HAVOK_HK_POWERED_CHAIN_SOLVER_UTIL_H
