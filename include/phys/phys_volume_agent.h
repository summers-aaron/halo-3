#ifndef INCLUDE_PHYS_VOLUME_AGENT_H
#define INCLUDE_PHYS_VOLUME_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_collide.h"

class phys_dispatcher;
class phys_volume_callback_shape;
class phys_volume_agent : public phys_agent {
public:
    virtual __forceinline ~phys_volume_agent() {}
    virtual void cleanup(phys_constraint_owner &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
protected:
    phys_volume_agent(const phys_collision_proxy &, const phys_collision_proxy &, phys_contact_manager *);
    static phys_agent *create_volume_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_no_volume_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    const phys_collider *m_collider_a;
    const phys_collider *m_collider_b;
    phys_volume_callback_shape *m_volume_shape_a;
    phys_volume_callback_shape *m_volume_shape_b;
    int m_shape_type_a;
    int m_shape_type_b;
};

#endif
