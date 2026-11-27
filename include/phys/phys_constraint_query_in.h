#ifndef INCLUDE_PHYS_CONSTRAINT_QUERY_IN_H
#define INCLUDE_PHYS_CONSTRAINT_QUERY_IN_H

#include <stddef.h>

#include "phys/phys_base_types.h"

class phys_vel_accum;
class phys_transform;
class phys_constraint;
class phys_step_info;
class phys_world;
struct phys_solver_info;

class phys_constraint_query_step_info {
public:
    __declspec(align(16)) phys_padded<float> m_substep_dt;
    phys_padded<float> m_micro_step_dt;
    phys_padded<float> m_substep_inv_dt;
    phys_padded<float> m_frame_dt;
    phys_padded<float> m_frame_inv_dt;
    phys_padded<float> m_inv_steps;
    phys_padded<float> m_inv_total_micro_steps;
    phys_padded<float> m_rhs_factor;
    phys_padded<float> m_virt_mass_factor;
    phys_padded<float> m_friction_rhs_factor;
};

class phys_constraint_query_in : public phys_constraint_query_step_info {
public:
    __forceinline phys_constraint_query_in() {}
    __declspec(align(16)) phys_padded<const phys_vel_accum *> m_body_a;
    phys_padded<const phys_vel_accum *> m_body_b;
    phys_padded<const phys_transform *> m_transform_a;
    phys_padded<const phys_transform *> m_transform_b;
    phys_padded<float> m_query_tau;
    phys_padded<float> m_solver_damping;
    phys_padded<phys_constraint *> m_query_constraint;
    phys_padded<void *> m_constraint_runtime;
    phys_padded<unsigned int> m_accum_a_index;
    phys_padded<unsigned int> m_accum_b_index;
    phys_padded<void *> m_runtime_in_main_memory;

    __forceinline void set(const phys_solver_info &solver_info, const phys_step_info &step_info);
    __forceinline void set(const phys_world &world);
};

typedef char phys_constraint_query_step_info_size[(sizeof(phys_constraint_query_step_info) == 48) ? 1 : -1];
typedef char phys_constraint_query_in_size[(sizeof(void *) != 4 || ((sizeof(phys_constraint_query_in) == 96) )) ? 1 : -1];
typedef char phys_constraint_query_body_a_offset[(offsetof(phys_constraint_query_in, m_body_a) == 48) ? 1 : -1];
typedef char phys_constraint_query_tau_offset[(sizeof(void *) != 4 || ((offsetof(phys_constraint_query_in, m_query_tau) == 64) )) ? 1 : -1];
typedef char phys_constraint_query_constraint_offset[(sizeof(void *) != 4 || ((offsetof(phys_constraint_query_in, m_query_constraint) == 72) )) ? 1 : -1];
typedef char phys_constraint_query_accum_offset[(sizeof(void *) != 4 || ((offsetof(phys_constraint_query_in, m_accum_a_index) == 80) )) ? 1 : -1];

#endif
