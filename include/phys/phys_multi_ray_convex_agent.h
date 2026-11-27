#ifndef INCLUDE_PHYS_MULTI_RAY_CONVEX_AGENT_H
#define INCLUDE_PHYS_MULTI_RAY_CONVEX_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_symmetric_agent.h"

class phys_dispatcher;

extern "C" void *phys_multi_ray_convex_agent_delete(class phys_multi_ray_convex_agent *pair_agent_entry, unsigned int dtor_flags);

class phys_multi_ray_convex_agent : public phys_iterative_linear_cast_agent {
public:
    virtual __forceinline ~phys_multi_ray_convex_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);

    friend void *phys_multi_ray_convex_agent_delete(phys_multi_ray_convex_agent *, unsigned int);

protected:
    phys_multi_ray_convex_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_multi_ray_convex_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_convex_multi_ray_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    struct ray_contact_info { unsigned short m_process_contact_id; };
    phys_inline_array<ray_contact_info, 4> m_process_contact_points;
};

template <> void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);

#endif
