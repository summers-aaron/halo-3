#ifndef INCLUDE_PHYS_CAPSULE_TRIANGLE_AGENT_H
#define INCLUDE_PHYS_CAPSULE_TRIANGLE_AGENT_H
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_triangle_shape.h"
class phys_capsule_triangle_agent : public phys_iterative_linear_cast_agent {
public:
    __forceinline phys_capsule_triangle_agent(const phys_collision_proxy &, const phys_collision_proxy &triangle_proxy, const phys_collision_input &,
        phys_contact_manager *pair_contact_manager) : phys_iterative_linear_cast_agent(pair_contact_manager) {
        m_process_contact_id[0] = 0xffff;
        m_process_contact_id[1] = 0xffff;
        m_process_contact_id[2] = 0xffff;
        const phys_triangle_shape *triangle = (const phys_triangle_shape *)triangle_proxy.get_shape();
        const phys_vec4 *vertices = triangle->get_vertices();
        phys_collide_triangle_util::setup_point_triangle_distance_cache(vertices, m_triangle_cache);
    }
    virtual ~phys_capsule_triangle_agent() {}
    virtual void cleanup(phys_constraint_owner &);
    virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static void register_agent(phys_dispatcher *);
    enum closest_point_hit { TRI_NEAREST_MISS, TRI_NEAREST_HIT };
    static closest_point_hit get_closest_point(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &,
        phys_collide_triangle_util::point_tri_distance_cache &, phys_contact_point &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
    static void static_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    unsigned short m_process_contact_id[3];
    phys_collide_triangle_util::point_tri_distance_cache m_triangle_cache;
protected:
    static phys_agent *create_capsule_tri_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static phys_agent *create_tri_capsule_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
private:
  static void get_closest_points_public(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1, struct phys_collision_input const &p2,
      struct phys_collide_triangle_util::point_tri_distance_cache &p3, int p4, class phys_contact_point *p5);
    static __forceinline void get_closest_points_inl(const phys_collision_proxy &agent_body_a,
        const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_collide_triangle_util::point_tri_distance_cache &cache, int search_manifold, phys_contact_point *points);
};
template<> void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector);
template<> void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector);

#endif
