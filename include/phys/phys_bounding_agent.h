#ifndef INCLUDE_PHYS_BOUNDING_AGENT_H
#define INCLUDE_PHYS_BOUNDING_AGENT_H

#include "phys/phys_agent.h"

class phys_bounding_agent : public phys_agent {
protected:
    phys_bounding_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    virtual __forceinline ~phys_bounding_agent() {}
public:
    virtual void cleanup(phys_constraint_owner &);
    virtual void update_compound_filter(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
    virtual void invalidate_sweep_info(phys_collision_input &);
    virtual void warp_time(float, float, phys_collision_input &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    virtual void bridge_remove_point(unsigned short bounding_contact_point_id);
    virtual void commit_potential(unsigned short bounding_contact_point_id);
    virtual void mark_zombie_entry(unsigned short bounding_contact_point_id);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    __forceinline void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    phys_agent *m_bounding_agent;
    phys_agent *m_child_agent;
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector);
    static void static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector);
    static void static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector);
protected:
    static phys_agent *create_shape_bounding_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
    static phys_agent *create_bounding_shape_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
};


#include "phys/phys_symmetric_agent.h"
template <> void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector);

#endif
