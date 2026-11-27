#ifndef INCLUDE_PHYS_HEIGHTFIELD_AGENT_H
#define INCLUDE_PHYS_HEIGHTFIELD_AGENT_H


class phys_dispatcher;
#include "phys/phys_agent.h"
#include "phys/phys_base.h"
#include "phys/phys_symmetric_agent.h"

class phys_heightfield_agent : public phys_agent {
public:
    virtual void cleanup(phys_constraint_owner &);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    phys_array<unsigned short> m_contact_point_ids;
    static void register_agent(phys_dispatcher *dispatcher);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
protected:
    phys_heightfield_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    virtual ~phys_heightfield_agent();
    static phys_agent *create_heightfield_a_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_heightfield_b_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
};

template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_proxy_pair_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_proxy_pair_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);
template <> void phys_symmetric_agent_linear_cast<phys_heightfield_agent>::static_sweep_cast(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector);
template <> void phys_symmetric_agent<phys_heightfield_agent>::sweep_cast(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector);

typedef char phys_heightfield_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_heightfield_agent) == 24) )) ? 1 : -1];
typedef char phys_heightfield_agent_ids_offset[(sizeof(void *) != 4 || ((offsetof(phys_heightfield_agent, m_contact_point_ids) == 12) )) ? 1 : -1];

#endif
