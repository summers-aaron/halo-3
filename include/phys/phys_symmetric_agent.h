#ifndef INCLUDE_PHYS_SYMMETRIC_AGENT_H
#define INCLUDE_PHYS_SYMMETRIC_AGENT_H
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_collision_input.h"
template <class T> class phys_symmetric_agent_linear_cast : public T {
public:
    __forceinline phys_symmetric_agent_linear_cast(phys_contact_manager *pair_contact_manager) : T(pair_contact_manager) {}
    __forceinline phys_symmetric_agent_linear_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *pair_contact_manager) : T(agent_body_b, agent_body_a, input, pair_contact_manager) {}
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    virtual void update_compound_filter(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
};
template <class T> class phys_symmetric_agent : public phys_symmetric_agent_linear_cast<T> {
public:
    __forceinline phys_symmetric_agent(phys_contact_manager *pair_contact_manager) : phys_symmetric_agent_linear_cast<T>(pair_contact_manager) {}
    __forceinline phys_symmetric_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input,
            phys_contact_manager *pair_contact_manager) : phys_symmetric_agent_linear_cast<T>(agent_body_a, agent_body_b, input,
            pair_contact_manager) {}
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
};
#endif
