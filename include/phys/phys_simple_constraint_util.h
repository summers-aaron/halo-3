#ifndef INCLUDE_PHYS_SIMPLE_CONSTRAINT_UTIL_H
#define INCLUDE_PHYS_SIMPLE_CONSTRAINT_UTIL_H

#include "phys/phys_math.h"

struct phys_body_vel {
    phys_vec4 m_lin_parts;
    phys_vec4 m_ang_jacobian;
};

struct phys_simple_constraint_init_input {
    phys_vec4 m_mass_rel_pos;
    phys_mat3 m_inv_inertia;
    float m_inv_mass;
};

struct phys_simple_constraint_info {
    struct body_info {
        phys_rotation m_jac_transpose;
        phys_mat3 m_inv_ijac;
        float m_inv_mass;
        float m_body_mass;
    };
    body_info m_body_info[2];
    phys_rotation m_base;
    phys_mat3 m_inv_mass_mat;
    phys_mat3 m_mass_mat;
};

struct phys_simple_collide_params {
    phys_vec4 m_direction;
    float m_contact_friction;
    float m_contact_restitution;
    float m_external_sep_vel;
    float m_extra_sep_vel;
    float m_extra_slope;
    float m_max_impulse;
    phys_bool m_impulse_limit_breached;
};

extern "C" {
void phys_simple_constraint_init_info(const phys_simple_constraint_init_input &agent_body_a,
    const phys_simple_constraint_init_input &agent_body_b, const phys_mat3 &directions, phys_simple_constraint_info &info);
void phys_simple_constraint_point_vel(const phys_simple_constraint_info &info,
    phys_body_vel &contact_velocity_a, phys_body_vel &velocity_b, phys_vec4 &out);
void phys_simple_constraint_apply_impulse(const phys_simple_constraint_info &info, const phys_vec4 &impulse,
    phys_body_vel &contact_velocity_a, phys_body_vel &velocity_b);
float phys_simple_constraint_collide(const phys_simple_constraint_info &info, const phys_simple_collide_params &params,
    phys_body_vel &contact_velocity_a, phys_body_vel &velocity_b);
}

typedef char phys_body_vel_size[(sizeof(phys_body_vel) == 32) ? 1 : -1];
typedef char phys_simple_constraint_input_size[(sizeof(phys_simple_constraint_init_input) == 80) ? 1 : -1];
typedef char phys_simple_constraint_info_size[(sizeof(phys_simple_constraint_info) == 368) ? 1 : -1];
typedef char phys_simple_constraint_external_vel_offset[(offsetof(phys_simple_collide_params, m_external_sep_vel) == 24) ? 1 : -1];
typedef char phys_simple_constraint_extra_vel_offset[(offsetof(phys_simple_collide_params, m_extra_sep_vel) == 28) ? 1 : -1];
typedef char phys_simple_constraint_extra_slope_offset[(offsetof(phys_simple_collide_params, m_extra_slope) == 32) ? 1 : -1];

#endif
