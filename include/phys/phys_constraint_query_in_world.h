#ifndef INCLUDE_PHYS_CONSTRAINT_QUERY_IN_WORLD_H
#define INCLUDE_PHYS_CONSTRAINT_QUERY_IN_WORLD_H

#include "phys/phys_dynamics.h"
#include "phys/phys_constraint_query_in.h"

    __forceinline void phys_constraint_query_in::set(const phys_world &world)
    {
        m_substep_dt = world.m_dynamics_step_info.m_solver_info.m_delta_time;
        m_micro_step_dt = world.m_dynamics_step_info.m_solver_info.m_delta_time * (float)world.m_dynamics_step_info.m_solver_info.m_micro_step_count;
        m_substep_inv_dt = world.m_dynamics_step_info.m_solver_info.m_inv_dt;
        m_inv_steps = world.m_dynamics_step_info.m_solver_info.m_inv_steps;
        m_inv_total_micro_steps = world.m_dynamics_step_info.m_solver_info.m_inv_steps * world.m_dynamics_step_info.m_solver_info.m_inv_micro_steps;
        m_query_tau = world.m_dynamics_step_info.m_solver_info.m_query_tau;
        m_solver_damping = world.m_dynamics_step_info.m_solver_info.m_solver_damping;
        m_rhs_factor = world.m_dynamics_step_info.m_solver_info.m_tau_div_damp * world.m_dynamics_step_info.m_solver_info.m_inv_dt;
        m_virt_mass_factor = world.m_dynamics_step_info.m_solver_info.m_solver_damping;
        m_friction_rhs_factor = world.m_dynamics_step_info.m_solver_info.m_friction_tau_div_damp * world.m_dynamics_step_info.m_solver_info.m_inv_dt;
        m_frame_dt = world.m_dynamics_step_info.m_step_info.m_delta_time;
        m_frame_inv_dt = world.m_dynamics_step_info.m_step_info.m_inv_dt;
    }

#endif
