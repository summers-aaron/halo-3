#ifndef INCLUDE_PHYS_BVH_STREAM_AGENT_H
#define INCLUDE_PHYS_BVH_STREAM_AGENT_H


class phys_dispatcher;
#include "phys/phys_agent.h"
#include "phys/phys_collide.h"
#include "phys/phys_batch_agent_track.h"

class phys_bvh_stream_agent : public phys_agent {
protected:
    virtual ~phys_bvh_stream_agent();
    phys_bvh_stream_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
public:
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
        phys_hit_collector &, phys_hit_collector *);
    virtual void cleanup(phys_constraint_owner &);
    virtual void invalidate_sweep_info(phys_collision_input &);
    virtual void warp_time(float, float, phys_collision_input &);
    virtual void update_compound_filter(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_constraint_owner &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
protected:
    virtual void gather_memory_stats(phys_statistics_collector *) const;
public:
    phys_dispatcher *m_agent_dispatcher;
    phys_vec4 m_aabb_min;
    phys_vec4 m_aabb_max;
    phys_batch_agent_track m_agent_track;
    static void register_agent(phys_dispatcher *dispatcher);
    static void register_multi_ray_agent(phys_dispatcher *dispatcher);
    static void register_convex_list_agent(phys_dispatcher *dispatcher);
    static phys_agent *create_bvh_shape_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
    static phys_agent *create_shape_bounding_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
    static phys_agent *dispatch_bvh_convex_list(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
    static phys_agent *dispatch_convex_list_bvh(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
};

typedef char phys_bvh_stream_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_bvh_stream_agent) == 64) )) ? 1 : -1];
typedef char phys_bvh_stream_agent_track_offset[(sizeof(void *) != 4 || ((offsetof(phys_bvh_stream_agent, m_agent_track) == 48) )) ? 1 : -1];

#endif
