#ifndef INCLUDE_PHYS_SIMPLEX_AGENT_BASE_H
#define INCLUDE_PHYS_SIMPLEX_AGENT_BASE_H
#include "phys/phys_iterative_linear_cast_agent.h"
class phys_simplex;
struct phys_extended_simplex_out;
class phys_simplex_agent_base : public phys_iterative_linear_cast_agent {
public:
    phys_simplex_agent_base(const phys_collision_proxy &, const phys_collision_proxy &, phys_contact_manager *);
    virtual ~phys_simplex_agent_base() {}
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
    virtual void cleanup(phys_constraint_owner &);
    static void static_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
protected:
    virtual void invalidate_sweep_info(phys_collision_input &);
    virtual void warp_time(float, float, phys_collision_input &);
    phys_simplex_cache m_cache;
    float m_separating_normal_time;
    float m_allowed_penetration;
    phys_vec4 m_separating_normal;
    static void compute_separating_normal(const phys_collision_proxy &, const phys_collision_proxy &, float, phys_simplex &, phys_vec4 &);
public:
  virtual void get_penetrations(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1,
      struct phys_collision_input const &p2, class phys_proxy_pair_collector &p3);
protected:
  phys_bool get_closest_point(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1,
      struct phys_collision_input const &p2, struct phys_extended_simplex_out &p3);
public:
  static void static_closest_points(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1,
      struct phys_collision_input const &p2, class phys_hit_collector &p3);
  static __forceinline phys_bool static_closest_point(const phys_collision_proxy &agent_body_a,
      const phys_collision_proxy &agent_body_b, const phys_transform &a_from_b,
      const phys_collision_input &input, phys_simplex_cache &cache, phys_extended_simplex_out &output);
};
#endif
