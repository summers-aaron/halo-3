#ifndef INCLUDE_PHYS_SPHERE_SPHERE_AGENT_H
#define INCLUDE_PHYS_SPHERE_SPHERE_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_collide.h"

class phys_dispatcher;
class phys_volume_callback_shape;
class phys_sphere_sphere_agent : public phys_iterative_linear_cast_agent
{
public:
    virtual __forceinline ~phys_sphere_sphere_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);

    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
protected:
    static __forceinline phys_bool get_closest_point(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_point &);
    static phys_agent *create_sphere_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
public:
    unsigned short sphere_contact_id;
    __forceinline phys_sphere_sphere_agent(phys_contact_manager *pair_contact_manager)
        : phys_iterative_linear_cast_agent(pair_contact_manager), sphere_contact_id(0xffff) {}
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
};

#endif
