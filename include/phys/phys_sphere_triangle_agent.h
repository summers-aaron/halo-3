#ifndef INCLUDE_PHYS_SPHERE_TRIANGLE_AGENT_H
#define INCLUDE_PHYS_SPHERE_TRIANGLE_AGENT_H
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_triangle_shape.h"
class phys_sphere_triangle_agent : public phys_iterative_linear_cast_agent {
public:
    virtual __forceinline ~phys_sphere_triangle_agent() {}
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    static void register_agent(phys_dispatcher *);
    enum closest_point_hit { TRI_NEAREST_MISS, TRI_NEAREST_FACE, TRI_NEAREST_EDGE };
    static closest_point_hit get_closest_point(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &,
        phys_collide_triangle_util::closest_point_tri_cache &, phys_contact_point &);
    static __forceinline closest_point_hit get_closest_point_inl(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &,
        phys_collide_triangle_util::closest_point_tri_cache &, phys_contact_point &);
protected:
    static phys_agent *create_sphere_tri_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_tri_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    __forceinline phys_sphere_triangle_agent(const phys_collision_proxy &, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &, phys_contact_manager *pair_contact_manager)
        : phys_iterative_linear_cast_agent(pair_contact_manager), m_process_contact_id(0xffff)
    {
        const phys_triangle_shape *triangle = (const phys_triangle_shape *)agent_body_b.get_shape();
        phys_collide_triangle_util::setup_closest_point_triangle_cache(triangle->get_vertices(), m_closest_point_cache);
    }
public:
    unsigned short m_process_contact_id;
    phys_collide_triangle_util::closest_point_tri_cache m_closest_point_cache;
};

#include "phys/phys_symmetric_agent.h"
template <> void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector);

#endif
