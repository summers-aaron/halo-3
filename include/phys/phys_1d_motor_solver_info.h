#ifndef INCLUDE_PHYS_1D_MOTOR_SOLVER_INFO_H
#define INCLUDE_PHYS_1D_MOTOR_SOLVER_INFO_H

struct phys_1d_motor_solver_info {
    float m_min_impulse;
    float m_max_impulse;
    float m_query_tau;
    float m_solver_damping;
    float m_rhs_factor;
};

typedef char phys_1d_motor_solver_info_size[(sizeof(phys_1d_motor_solver_info) == 20) ? 1 : -1];

#endif
