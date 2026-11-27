#ifndef INCLUDE_PHYS_LIST_AGENT_H
#define INCLUDE_PHYS_LIST_AGENT_H


class phys_dispatcher;
#include "phys/phys_agent.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_symmetric_agent.h"

class phys_list_agent : public phys_agent {
public:
    phys_list_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void invalidate_sweep_info(phys_collision_input &input);
    virtual void warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input);
    virtual void update_compound_filter(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    static phys_agent *create_list_a_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_list_b_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static void register_agent(phys_dispatcher *dispatcher);
    phys_dispatcher *m_agent_dispatcher;
    phys_batch_agent_track m_agent_track;
};

typedef char phys_list_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_list_agent) == 28) )) ? 1 : -1];
typedef char phys_list_agent_track_offset[(sizeof(void *) != 4 || ((offsetof(phys_list_agent, m_agent_track) == 16) )) ? 1 : -1];

template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::update_compound_filter(
    const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
template<> void phys_symmetric_agent_linear_cast<phys_list_agent>::get_closest_points(const phys_collision_proxy &,
    const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
template<> void phys_symmetric_agent_linear_cast<phys_list_agent>::get_penetrations(const phys_collision_proxy &,
    const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::process_collision(
    const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &, phys_process_collision_output &);
template<>
void phys_symmetric_agent<phys_list_agent>::sweep_cast(
    const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &, phys_hit_collector &, phys_hit_collector *);

#endif
