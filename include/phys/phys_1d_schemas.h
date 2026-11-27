#ifndef INCLUDE_PHYS_1D_SCHEMAS_H
#define INCLUDE_PHYS_1D_SCHEMAS_H

// The simple solver schemas: written by the 1d jacobian builders, read by the solver and the solver export.
// Each starts with its jacobian; the schema type is the byte at offset 3 (hkJacobianSchema::setType).

#include <stddef.h>
#include "phys/phys_math.h"
#include "phys/phys_schema.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_1d_motor_solver_info.h"

struct phys_1d_bilateral_schema : public phys_schema {
    phys_1lin2ang_jacobian m_contact_jac;

    __forceinline void init_1d_bilateral(const phys_1lin2ang_jacobian &) { set_schema_type(this, SCHEMA_KIND_1D_BILATERAL); }
};

struct phys_1d_bilateral_user_tau_schema : public phys_schema {
    phys_1lin2ang_jacobian m_contact_jac;
    float m_query_tau;
    float m_solver_damping;

    __forceinline void init_1d_bilateral_user_tau(const phys_1lin2ang_jacobian &, float tau, float damping) {
        m_query_tau = tau;
        m_solver_damping = damping;
        set_schema_type(this, SCHEMA_KIND_1D_BILATERAL_USER_TAU);
    }
};

struct phys_1d_lin_limits_schema : public phys_schema {
    phys_1lin2ang_jacobian m_contact_jac;
    float m_min_limit;
    float m_max_limit;

    __forceinline void init_1d_lin_limits(const phys_1lin2ang_jacobian &, float min, float max) {
        m_min_limit = min;
        m_max_limit = max;
        set_schema_type(this, SCHEMA_KIND_1D_LIN_LIMITS);
    }
};

struct phys_1d_friction_schema : public phys_schema {
    phys_1lin2ang_jacobian m_contact_jac;
    float m_max_friction_force;

    __forceinline void init_1d_friction(const phys_1lin2ang_jacobian &, float contact_max_friction) {
        m_max_friction_force = contact_max_friction;
        set_schema_type(this, SCHEMA_KIND_1D_FRICTION);
    }
};

struct phys_1d_ang_schema : public phys_schema {
    phys_2ang_jacobian m_contact_jac;
};

struct phys_1d_ang_friction_schema : public phys_schema {
    phys_2ang_jacobian m_contact_jac;
    float m_max_friction_force;

    __forceinline void init_1d_ang_friction(const phys_2ang_jacobian &, float contact_max_friction) {
        m_max_friction_force = contact_max_friction;
        set_schema_type(this, SCHEMA_KIND_1D_ANG_FRICTION);
    }
};

struct phys_1d_pulley_schema : public phys_schema {
    phys_2lin2ang_jacobian m_contact_jac;

    __forceinline void init_1d_pulley(const phys_2lin2ang_jacobian &) {
        set_schema_type(this, SCHEMA_KIND_1D_PULLEY);
    }
};

struct phys_1d_ang_limits_schema : public phys_schema {
    phys_2ang_jacobian m_contact_jac;
    float m_min_limit;
    float m_max_limit;
    float m_query_tau;

    __forceinline void init_1d_ang_limits(const phys_2ang_jacobian &, float min_impulse, float max_impulse, float tau) {
        m_min_limit = min_impulse;
        m_max_limit = max_impulse;
        m_query_tau = tau;
        set_schema_type(this, SCHEMA_KIND_1D_ANG_LIMITS);
    }
};

struct phys_1d_ang_motor_schema : public phys_schema {
    phys_2ang_jacobian m_contact_jac;
    phys_1d_motor_solver_info m_schema_motor;

    __forceinline phys_1d_motor_solver_info &init_1d_ang_motor(const phys_2ang_jacobian &) {
        set_schema_type(this, SCHEMA_KIND_1D_ANG_MOTOR);
        return m_schema_motor;
    }
};

struct phys_set_mass_schema : public phys_schema {
    phys_vec4 m_inv_mass_a;
    phys_vec4 m_inv_mass_b;

    __forceinline void init_set_mass(const phys_vec4 &inv_mass_a, const phys_vec4 &inv_mass_b) {
        m_inv_mass_a = inv_mass_a;
        m_inv_mass_b = inv_mass_b;
        set_schema_type(this, SCHEMA_KIND_SET_MASS);
    }
};

struct phys_add_vel_schema : public phys_schema {
    phys_vec4 m_velocity;
    int m_body_index;
};

typedef char phys_1d_bilateral_schema_size[(sizeof(phys_1d_bilateral_schema) == 48) ? 1 : -1];
typedef char phys_1d_bilateral_user_tau_size[(sizeof(phys_1d_bilateral_user_tau_schema) == 64) ? 1 : -1];
typedef char phys_1d_lin_limits_schema_size[(sizeof(phys_1d_lin_limits_schema) == 64) ? 1 : -1];
typedef char phys_1d_friction_schema_size[(sizeof(phys_1d_friction_schema) == 64) ? 1 : -1];
typedef char phys_1d_ang_schema_size[(sizeof(phys_1d_ang_schema) == 32) ? 1 : -1];
typedef char phys_1d_ang_friction_schema_size[(sizeof(phys_1d_ang_friction_schema) == 48) ? 1 : -1];
typedef char phys_1d_pulley_schema_size[(sizeof(phys_1d_pulley_schema) == 64) ? 1 : -1];
typedef char phys_1d_ang_limits_schema_size[(sizeof(phys_1d_ang_limits_schema) == 48) ? 1 : -1];
typedef char phys_1d_ang_motor_schema_size[(sizeof(phys_1d_ang_motor_schema) == 64) ? 1 : -1];
typedef char phys_1d_ang_motor_schema_offset[(offsetof(phys_1d_ang_motor_schema, m_schema_motor) == 32) ? 1 : -1];
typedef char phys_set_mass_schema_size[(sizeof(phys_set_mass_schema) == 32) ? 1 : -1];
typedef char phys_add_vel_schema_size[(sizeof(phys_add_vel_schema) == 32) ? 1 : -1];
typedef char phys_add_vel_schema_body_offset[(offsetof(phys_add_vel_schema, m_body_index) == 16) ? 1 : -1];

#endif // HAVOK_HK_JACOBIAN_1D_SCHEMAS_H
