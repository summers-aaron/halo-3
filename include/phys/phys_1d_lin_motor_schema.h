#ifndef INCLUDE_PHYS_1D_LIN_MOTOR_SCHEMA_H
#define INCLUDE_PHYS_1D_LIN_MOTOR_SCHEMA_H

#include "phys/phys_schema.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_1d_motor_solver_info.h"

struct phys_1d_lin_motor_schema : public phys_schema {
    phys_1lin2ang_jacobian m_contact_jac;
    phys_1d_motor_solver_info m_schema_motor;

    __forceinline phys_1d_motor_solver_info &init_1d_lin_motor(const phys_1lin2ang_jacobian &) {
        set_schema_type(this, SCHEMA_KIND_1D_LIN_MOTOR);
        return m_schema_motor;
    }
};

typedef char phys_1d_lin_motor_schema_size[(sizeof(phys_1d_lin_motor_schema) == 80) ? 1 : -1];
typedef char phys_1d_lin_motor_schema_info_offset[(offsetof(phys_1d_lin_motor_schema, m_schema_motor) == 48) ? 1 : -1];

#endif
