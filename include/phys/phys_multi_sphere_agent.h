#ifndef INCLUDE_PHYS_MULTI_SPHERE_AGENT_H
#define INCLUDE_PHYS_MULTI_SPHERE_AGENT_H
#include "phys/phys_agent.h"
#include "phys/phys_collision_input.h"
class phys_multi_sphere_agent : public phys_agent {
public:
    virtual __forceinline ~phys_multi_sphere_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    struct KeyAgentPair { unsigned int m_key; phys_agent *m_key_agent; };
    phys_inline_array<KeyAgentPair, 4> m_key_agents;
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
protected:
    phys_multi_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_list_a_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_list_b_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
};

#include "phys/phys_symmetric_agent.h"

template <> void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector,
        phys_hit_collector *start_collector);

#endif
