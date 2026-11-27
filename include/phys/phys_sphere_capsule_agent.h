#ifndef INCLUDE_PHYS_SPHERE_CAPSULE_AGENT_H
#define INCLUDE_PHYS_SPHERE_CAPSULE_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_collide.h"

class phys_dispatcher;
class phys_volume_callback_shape;
class phys_sphere_capsule_agent : public phys_iterative_linear_cast_agent {
public:
    virtual __forceinline ~phys_sphere_capsule_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void register_agent(phys_dispatcher *dispatcher);
    enum closest_point_hit { TRI_NEAREST_MISS, TRI_NEAREST_HIT };
protected:
    static phys_agent *create_sphere_capsule_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_capsule_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static closest_point_hit get_closest_point(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_point &);
    static __forceinline closest_point_hit get_closest_point_inl(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_point &);
    __forceinline phys_sphere_capsule_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &,
        phys_contact_manager *pair_contact_manager)
        : phys_iterative_linear_cast_agent(pair_contact_manager), m_process_contact_id(0xffff) {}
    unsigned short m_process_contact_id;
};

#include "phys/phys_symmetric_agent.h"

template<> void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input, phys_hit_collector &collector,
        phys_hit_collector *second_collector);

#endif
