#ifndef INCLUDE_PHYS_CHAIN_SOLVER_INFO_H
#define INCLUDE_PHYS_CHAIN_SOLVER_INFO_H
#include "phys/phys_dynamics.h"
#include "phys/phys_chain_matrix_types.h"
#include "phys/phys_chain_motor_solver_info.h"
#include "phys/phys_vel_accum_offset.h"
struct phys_chain_solver_info {
    __forceinline phys_chain_solver_info(const phys_solver_info &solver_info) : m_solver_info(solver_info) {}
    const phys_solver_info &m_solver_info;
    phys_padded<int> m_constraint_count;
    phys_padded<float> m_schema_tau;
    phys_padded<float> m_schema_damping;
    phys_padded<phys_1lin2ang_jacobian *> m_lin_ang_jacs;
    phys_padded<phys_2ang_jacobian *> m_ang_jacs;
    phys_padded<phys_vel_accum_offset *> m_vel_accum_offsets;
    phys_padded<phys_vel_accum *> m_accums_base;
    phys_padded<phys_chain_mat6_triple *> m_chain_triples;
    phys_padded<phys_3d_ang_motor_solver_info *> m_motors_state;
    phys_padded<phys_vec8 *> m_g_temp_buffer;
    phys_padded<phys_vec8 *> m_vel_buffer;
};
#endif
