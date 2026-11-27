#ifndef INCLUDE_PHYS_CONSTRAINT_QUERY_IN_SOLVER_H
#define INCLUDE_PHYS_CONSTRAINT_QUERY_IN_SOLVER_H

#include "phys/phys_dynamics.h"
#include "phys/phys_constraint_query_in.h"

// The step-info half of hkConstraintQueryIn: the per-step solver constants copied from the solver info.
__forceinline void phys_constraint_query_in::set(const phys_solver_info &si, const phys_step_info &step_info)
{
    m_substep_dt = si.m_delta_time;
    m_micro_step_dt = si.m_delta_time * si.m_micro_step_count;
    m_substep_inv_dt = si.m_inv_dt;
    m_inv_steps = si.m_inv_steps;
    m_inv_total_micro_steps = si.m_inv_steps * si.m_inv_micro_steps;
    m_query_tau = si.m_query_tau;
    m_solver_damping = si.m_solver_damping;
    m_rhs_factor = si.m_tau_div_damp * si.m_inv_dt;
    m_virt_mass_factor = si.m_solver_damping;
    m_friction_rhs_factor = si.m_friction_tau_div_damp * si.m_inv_dt;
    m_frame_dt = step_info.m_delta_time;
    m_frame_inv_dt = step_info.m_inv_dt;
}

#endif
