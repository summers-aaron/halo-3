#ifndef INCLUDE_PHYS_MOTOR_CONSTRAINT_INFO_H
#define INCLUDE_PHYS_MOTOR_CONSTRAINT_INFO_H

#include "phys/phys_constraint_data.h"
#include "phys/phys_constraint_query_in.h"

class phys_motor;

class phys_1d_bilateral_status {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT_SOLVER, phys_1d_bilateral_status);
    phys_padded<float> m_virtual_mass;
};

class phys_1d_motor_info {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT_SOLVER, phys_1d_motor_info);
    phys_padded<float> m_target_pos;
    phys_padded<float> m_target_vel;
    phys_padded<float> m_max_force;
    phys_padded<float> m_min_force;
    phys_padded<float> m_query_tau;
    phys_padded<float> m_solver_damping;
};

class phys_motor_input : public phys_1d_bilateral_status {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT_SOLVER, phys_motor_input);
    phys_padded<const phys_constraint_query_step_info *> m_step_info;
    phys_solver_results m_last_results;
    phys_padded<float> m_delta_target;
    phys_padded<float> m_pos_error;
};

class phys_motor_output : public phys_1d_motor_info {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT_SOLVER, phys_motor_output);
};

extern "C" void phys_compute_motor_data(const phys_motor *motor, const phys_motor_input *input, phys_motor_output *output);

typedef char phys_motor_status_size[(sizeof(phys_1d_bilateral_status) == 4) ? 1 : -1];
typedef char phys_1d_motor_info_size[(sizeof(phys_1d_motor_info) == 24) ? 1 : -1];
typedef char phys_motor_input_size[(sizeof(void *) != 4 || ((sizeof(phys_motor_input) == 24) )) ? 1 : -1];
typedef char phys_motor_input_step_offset[(sizeof(void *) != 4 || ((offsetof(phys_motor_input, m_step_info) == 4) )) ? 1 : -1];
typedef char phys_motor_input_results_offset[(sizeof(void *) != 4 || ((offsetof(phys_motor_input, m_last_results) == 8) )) ? 1 : -1];
typedef char phys_motor_input_delta_offset[(sizeof(void *) != 4 || ((offsetof(phys_motor_input, m_delta_target) == 16) )) ? 1 : -1];
typedef char phys_motor_output_size[(sizeof(phys_motor_output) == 24) ? 1 : -1];

#endif
