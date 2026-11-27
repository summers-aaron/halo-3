#ifndef INCLUDE_PHYS_BVH_AGENT_H
#define INCLUDE_PHYS_BVH_AGENT_H


class phys_dispatcher;
#include "phys/phys_agent.h"
#include "phys/phys_collide.h"

class phys_bvh_agent : public phys_agent {
public:
  phys_bvh_agent(phys_contact_manager *agent_contact_manager);
  virtual void cleanup(phys_constraint_owner &constraint_owner_ref);
  virtual void invalidate_sweep_info(phys_collision_input &input);
  virtual void warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input);
  static void set_use_aabb_caching(phys_bool enabled);
  static phys_bool get_use_aabb_caching();
  static void register_agent(phys_dispatcher *dispatcher);
  virtual void sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
      phys_hit_collector &, phys_hit_collector *);
  virtual void get_closest_points(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_hit_collector &);
  virtual void get_penetrations(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_proxy_pair_collector &);
  virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
      phys_process_collision_output &);
  static void static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_collision_input &input, phys_proxy_pair_collector &collector);
  static void static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_collision_input &input, phys_hit_collector &collector);
  static void static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector);
  virtual void update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_collision_input &input, phys_constraint_owner &owner);
  struct phys_bounding_agent_entry_info {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLIDE, phys_bvh_agent::phys_bounding_agent_entry_info);
    unsigned int m_key;
    int m_user_info;
    phys_agent *m_collision_agent;
    __forceinline unsigned int &get_bvh_key() { return m_key; }
    __forceinline void set_cached_key(unsigned int &key) { m_key = key; }
  };
  phys_array<phys_bounding_agent_entry_info> m_collision_partners;
  phys_aabb m_cached_aabb;
protected:
  static phys_bool m_use_aabb_cache;
  static phys_bool m_use_fast_update;
  static void compute_sweep_cast_bounds(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &, phys_aabb &);
  static void static_compute_bounds(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_aabb &);
  void prepare_collision_partners(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_collision_input &input, phys_constraint_owner &constraint_owner_ref);
  void prepare_process_partners(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_process_collision_input &input, phys_constraint_owner &constraint_owner_ref);
  static phys_result compute_bounds_and_query_tree(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_transform &b_from_a,
    const phys_vec4 &lin_sweep_info, const phys_process_collision_input &input, phys_aabb *cached_aabb, phys_array<unsigned int> &out_hit_list);
  virtual ~phys_bvh_agent();
  virtual void gather_memory_stats(phys_statistics_collector *collector) const;
  static phys_agent *create_bvh_shape_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
  static phys_agent *create_shape_bounding_agent(const phys_collision_proxy &, const phys_collision_proxy &,
      const phys_collision_input &, phys_contact_manager *);
  static phys_agent *create_bvh_bvh_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
  static phys_agent *default_agent_create(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_collision_input &input, phys_contact_manager *pair_contact_manager);
};

typedef char phys_bvh_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_bvh_agent) == 64) )) ? 1 : -1];
typedef char phys_bvh_agent_partners_offset[(sizeof(void *) != 4 || ((offsetof(phys_bvh_agent, m_collision_partners) == 12) )) ? 1 : -1];
typedef char phys_bvh_agent_aabb_offset[(sizeof(void *) != 4 || ((offsetof(phys_bvh_agent, m_cached_aabb) == 32) )) ? 1 : -1];

#endif
