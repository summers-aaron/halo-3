#ifndef INCLUDE_PHYS_MULTI_SPHERE_TRIANGLE_AGENT_H
#define INCLUDE_PHYS_MULTI_SPHERE_TRIANGLE_AGENT_H


class phys_dispatcher;
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_collide_triangle_util.h"

class phys_multi_sphere_triangle_agent : public phys_iterative_linear_cast_agent {
protected:
    __forceinline phys_multi_sphere_triangle_agent(const phys_collision_proxy &, const phys_collision_proxy &triangle_proxy,
        const phys_collision_input &, phys_contact_manager *pair_contact_manager) :
        phys_iterative_linear_cast_agent(pair_contact_manager)
    {
        for (int i = 0; i < 8; ++i) m_process_contact_id[i] = 0xffff;
        const phys_triangle_shape *triangle = (const phys_triangle_shape *)triangle_proxy.get_shape();
        const phys_vec4 *vertices = triangle->get_vertices();
        phys_collide_triangle_util::setup_closest_point_triangle_cache(vertices, m_triangle_cache);
    }
public:
    virtual ~phys_multi_sphere_triangle_agent() {}
    virtual void cleanup(phys_constraint_owner &);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void register_agent(phys_dispatcher *);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    phys_collide_triangle_util::closest_point_tri_cache m_triangle_cache;
    unsigned short m_process_contact_id[8];
protected:
    static phys_agent *create_multi_sphere_tri_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_tri_multi_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
};

typedef char phys_multi_sphere_triangle_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_multi_sphere_triangle_agent) == 44) )) ? 1 : -1];
typedef char phys_multi_sphere_triangle_agent_cache_offset[(sizeof(void *) != 4 || ((offsetof(phys_multi_sphere_triangle_agent,
    m_triangle_cache) == 12) )) ? 1 : -1];
typedef char phys_multi_sphere_triangle_agent_ids_offset[(sizeof(void *) != 4 || ((offsetof(phys_multi_sphere_triangle_agent,
    m_process_contact_id) == 28) )) ? 1 : -1];


#include "phys/phys_symmetric_agent.h"
template<> void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);

#endif
