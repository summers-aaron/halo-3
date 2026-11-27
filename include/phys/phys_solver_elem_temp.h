#ifndef INCLUDE_PHYS_SOLVER_ELEM_TEMP_H
#define INCLUDE_PHYS_SOLVER_ELEM_TEMP_H

struct phys_solver_elem_temp {
    union {
        float m_applied_impulse;
        float m_used_impulse_factor;
        float m_delta_rhs;
    };
};

typedef char phys_solver_elem_temp_size[(sizeof(phys_solver_elem_temp) == 4) ? 1 : -1];

#endif
