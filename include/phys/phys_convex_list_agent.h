#ifndef INCLUDE_PHYS_CONVEX_LIST_AGENT_H
#define INCLUDE_PHYS_CONVEX_LIST_AGENT_H


class phys_dispatcher;
#include "phys/phys_simplex_agent.h"
#include "phys/phys_batch_agent_track.h"

class phys_convex_list_agent : public phys_simplex_agent {
protected:
    phys_convex_list_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    virtual __forceinline ~phys_convex_list_agent() {}
public:
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void update_compound_filter(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
    virtual void invalidate_sweep_info(phys_collision_input &input);
    virtual void warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input);
    virtual void bridge_remove_point(unsigned short bounding_contact_point_id);
    virtual void commit_potential(unsigned short bounding_contact_point_id);
    virtual void mark_zombie_entry(unsigned short bounding_contact_point_id);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    static phys_agent *create_convex_vs_list_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_list_vs_convex_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_list_vs_list_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static void register_agent(phys_dispatcher *);
protected:
    void switch_to_simplex_mode(phys_constraint_owner &owner);
    void switch_to_stream_mode(phys_constraint_owner &owner);
    struct convex_list_stream_data {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLIDE, phys_convex_list_agent::convex_list_stream_data);
        phys_batch_agent_track m_agent_track;
        float m_stream_mode_sweep_dist;
    };
    // March 0x834a2c08 asserts !m_inGskMode, then returns the manifold storage at +48.
    __forceinline convex_list_stream_data &get_stream() { return *reinterpret_cast<convex_list_stream_data *>(&m_box_manifold); }
public:
    phys_dispatcher *m_agent_dispatcher;
    phys_bool m_in_simplex_mode;
    phys_bool m_process_called;
    short m_stream_mode_counter;
};

typedef char phys_convex_list_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_convex_list_agent) == 144) )) ? 1 : -1];

#include "phys/phys_symmetric_agent.h"

template<> void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input, phys_hit_collector &collector,
        phys_hit_collector *start_collector);

#endif
