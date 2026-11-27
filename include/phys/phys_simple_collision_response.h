#ifndef INCLUDE_PHYS_SIMPLE_COLLISION_RESPONSE_H
#define INCLUDE_PHYS_SIMPLE_COLLISION_RESPONSE_H

class phys_contact_point;
class phys_dynamics_contact_manager;
class phys_motion;
class phys_rigid_body;
class phys_simple_contact_data;
class phys_vel_accum;
struct phys_simple_collide_params;

class phys_simple_collision_response {
public:
    struct solve_single_output {
        float m_response_impulse;
        float m_keyframed_vel_a;
        float m_keyframed_vel_b;
    };
    struct solve_single_output_ext {
        float m_response_impulse;
    };
    static void solve_single_contact(const phys_contact_point &cp, float time,
        const phys_simple_collide_params &params, phys_motion *agent_body_a, phys_motion *agent_body_b,
        phys_dynamics_contact_manager *agent_contact_manager, solve_single_output &output);
    static void solve_single_contact2(phys_simple_contact_data *data, const phys_contact_point &cp,
        const phys_simple_collide_params &params, phys_rigid_body *agent_body_a, phys_rigid_body *agent_body_b,
        phys_vel_accum *accum_a, phys_vel_accum *accum_b, solve_single_output_ext &output);
};

#endif
