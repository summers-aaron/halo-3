#ifndef INCLUDE_PHYS_BOX_BOX_AGENT_H
#define INCLUDE_PHYS_BOX_BOX_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_collide.h"

class phys_dispatcher;
class phys_volume_callback_shape;
#include "phys/phys_box_box_manifold.h"
class phys_box_box_agent : public phys_iterative_linear_cast_agent
{
public:
    virtual __forceinline ~phys_box_box_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector);
    virtual void get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector);
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
protected:
    static phys_agent *create_box_box_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    __forceinline phys_box_box_agent(phys_contact_manager *agent_contact_manager) : phys_iterative_linear_cast_agent(agent_contact_manager) {}
public:
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
protected:
    float m_cached_distance;
    phys_box_box_manifold m_box_manifold;
};

#endif
