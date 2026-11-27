#ifndef INCLUDE_PHYS_DISPATCHER_H
#define INCLUDE_PHYS_DISPATCHER_H

#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_narrow_agent.h"

class phys_contact_manager_factory;
class phys_agent;
class phys_collision_proxy;
class phys_proxy_pair_collector;
class phys_hit_collector;
struct phys_collision_input;
struct phys_linear_cast_collision_input;
struct phys_narrow_agent_input;
struct phys_agent_entry;
struct phys_narrow_agent_process_input;

class phys_dispatcher : public phys_ref_object {
public:
  struct agent_funcs {
    typedef phys_agent *(*agent_create_fn)(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *agent_contact_manager);
    typedef void (*penetrations_fn)(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector);
    typedef void (*closest_points_fn)(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector);
    typedef void (*sweep_cast_fn)(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
      const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector);
    agent_create_fn m_create_fn;
    penetrations_fn m_penetrations_fn;
    closest_points_fn m_closest_points_fn;
    sweep_cast_fn m_linear_cast_func;
    phys_bool m_flipped;
    phys_bool m_predictive;
    __forceinline agent_funcs() : m_flipped(false), m_predictive(false) {}
  };
  typedef agent_funcs::agent_create_fn agent_create_fn;
  typedef agent_funcs::penetrations_fn penetrations_fn;
  typedef agent_funcs::closest_points_fn closest_points_fn;
  typedef agent_funcs::sweep_cast_fn sweep_cast_fn;
  __declspec(align(16)) struct narrow_agent_funcs {
    void *(*create)(const phys_narrow_agent_input &, phys_agent_entry *, void *);
    void (*destroy)(phys_agent_entry *, void *, phys_contact_manager *, phys_constraint_owner &, phys_dispatcher *);
    void *(*cleanup)(phys_agent_entry *, void *, phys_contact_manager *, phys_constraint_owner &);
    void (*bridge_remove_point)(phys_agent_entry *, void *, unsigned short);
    void (*commit_potential)(phys_agent_entry *, void *, unsigned short);
    void (*mark_zombie_entry)(phys_agent_entry *, void *, unsigned short);
    void (*update_agent_filter)(phys_agent_entry *, void *, phys_collision_proxy &, phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *, phys_constraint_owner &);
    void (*invalidate_sweep_info)(phys_agent_entry *, void *, phys_collision_input &);
    void (*warp_time)(phys_agent_entry *, void *, float, float, phys_collision_input &);
    void (*compute_sep_normal)(const phys_narrow_agent_input &, void *, phys_vec4 &);
    void *(*process)(const phys_narrow_agent_process_input &, phys_agent_entry *, void *, phys_vec4 *, phys_process_collision_output &);
    phys_bool agent_is_predictive;
    phys_bool symmetric;
    __forceinline narrow_agent_funcs() : agent_is_predictive(false), symmetric(false)
    {
      update_agent_filter = 0;
      invalidate_sweep_info = 0;
      warp_time = 0;
      compute_sep_normal = 0;
    }
  };
  struct narrow_agent_funcs_internal : public narrow_agent_funcs {
    enum dispatch_symmetry { DISPATCH_SYMMETRIC = 0, AGENT_ORDER_DIRECT = 1, AGENT_ORDER_FLIPPED = 2 };
    dispatch_symmetry m_agent_symmetry;
    __forceinline narrow_agent_funcs_internal() {}
  };
  struct shape_inheritance { phys_shape_type m_primary_type; phys_shape_type m_alternate_type; };
  struct quality_init_info {
    float m_gravity_len;
    float m_collision_tolerance;
    float m_min_dt;
    float m_max_lin_vel;
    unsigned short m_default_constraint_priority;
    unsigned short m_toi_constraint_priority;
    unsigned short m_toi_higher_priority;
    unsigned short m_toi_forced_priority;
    phys_bool m_ccd_enabled;
    phys_bool m_neg_manifold_sweeps;
    phys_bool m_neg_tol_non4d_contacts;
    phys_bool m_toi_weld_reject;
  };
  enum collision_quality {
    COLLIDE_QUALITY_INVALID,
    COLLIDE_QUALITY_DISCRETE,
    COLLIDE_QUALITY_TOI,
    COLLIDE_QUALITY_TOI_HIGH,
    COLLIDE_QUALITY_TOI_FORCED,
    COLLIDE_QUALITY_EXPAND_MANIFOLD_TMP,
    COLLIDE_QUALITY_CHARACTER,
    COLLIDE_QUALITY_CHARACTER_FORCED
  };
  struct dispatch_debug_entry { unsigned char m_data[3]; };
  agent_funcs::agent_create_fn m_default_agent_create;
  phys_contact_manager_factory *m_contact_manager_factory[8][8];
  __forceinline phys_contact_manager_factory *get_contact_manager_factory(int response_a, int response_b) const {
    return m_contact_manager_factory[response_a][response_b];
  }
  // Nonzero when shapes of the given type also answer as alternateType (a bit per alternate type).
  __forceinline int has_alternate_type(phys_shape_type type, phys_shape_type alternate_type) const {
    return m_alternate_types[type] & (1 << alternate_type);
  }
  
  __declspec(align(16)) int m_alternate_types[32];
  int m_agent2_type_count;
  unsigned char m_agent2_types[32][32];
  unsigned char m_agent2_types_pred[32][32];
  agent_funcs m_agent2_funcs[64];
  int m_narrow_agent_type_count;
  
  __declspec(align(16)) unsigned char m_narrow_agent_types[32][32];
  unsigned char m_narrow_agent_types_pred[32][32];
  narrow_agent_funcs_internal m_narrow_agent_funcs[16];
  __declspec(align(16)) char m_quality_table[PHYS_COLLIDER_QUALITY_MAX][PHYS_COLLIDER_QUALITY_MAX];
  __declspec(align(16)) phys_collision_quality_info m_quality_info[8];
  phys_bool m_agent_registered;
  phys_bool m_narrow_agent_registered;
  phys_bool m_checks_enabled;
  phys_bool m_midphase_narrow_registered;
  phys_array<shape_inheritance> m_shape_inheritance;
  dispatch_debug_entry (*m_debug_table)[32][32];
  dispatch_debug_entry (*m_debug_table_pred)[32][32];
  dispatch_debug_entry (*m_debug_table3)[32][32];
  dispatch_debug_entry (*m_debug_table3_pred)[32][32];
  float m_expected_max_lin_vel;
  float m_expected_min_psi_dt;
  phys_dispatcher(agent_funcs::agent_create_fn default_agent_fn, phys_contact_manager_factory *default_contact_factory);
  virtual ~phys_dispatcher();
  PHYS_DECLARE_ALLOCATOR(PHYS_MEM_COLLISION_INFO);
  void set_enable_checks(phys_bool enabled);
  virtual void gather_memory_stats(phys_statistics_collector *collector) const;
  void disable_debugging();
  void print_debug_table();
protected:
  void register_agent_internal(unsigned char (* const table)[32], int agent_type_id,
    phys_shape_type shape_type_a, phys_shape_type shape_type_b, phys_shape_type orig_type_a, phys_shape_type orig_type_b,
    dispatch_debug_entry (*debug_table)[32][32], int depth);
  void update_alternate_type(phys_shape_type primary_shape_type, phys_shape_type alternate_type, int depth);
  void reset_creation_functions();
public:
  __forceinline int get_agent_type(int shape_type_a, int shape_type_b, phys_bool agent_is_predictive) const {
    return (agent_is_predictive ? m_agent2_types_pred : m_agent2_types)[shape_type_a][shape_type_b];
  }
  __forceinline signed char get_collision_quality_idx(phys_collider_quality a, phys_collider_quality b) {
    return m_quality_table[a][b];
  }
  __forceinline phys_collision_quality_info *get_collision_quality_info(signed char index) {
    return &m_quality_info[index];
  }
  enum agent_predictive { AGENT_NON_PREDICTIVE, AGENT_PREDICTIVE };
  __forceinline phys_narrow_agent::agent_create_fn get_narrow_agent_create_fn(int type) { return m_narrow_agent_funcs[type].create; }
  __forceinline phys_narrow_agent::agent_destroy_fn get_narrow_agent_destroy_fn(int type) { return m_narrow_agent_funcs[type].destroy; }
  __forceinline phys_narrow_agent::agent_cleanup_fn get_narrow_agent_cleanup_fn(int type) { return m_narrow_agent_funcs[type].cleanup; }
  __forceinline phys_narrow_agent::agent_remove_point_fn get_narrow_agent_remove_point_fn(int type) { return m_narrow_agent_funcs[type].bridge_remove_point; }
  __forceinline phys_narrow_agent::agent_commit_potential_fn get_narrow_agent_commit_potential_fn(int type) {
      return m_narrow_agent_funcs[type].commit_potential; }
  __forceinline phys_narrow_agent::agent_create_zombie_fn get_narrow_agent_create_zombie_fn(int type) { return m_narrow_agent_funcs[type].mark_zombie_entry; }
  __forceinline phys_narrow_agent::agent_update_filter_fn get_narrow_agent_update_filter_fn(int type) const {
      return m_narrow_agent_funcs[type].update_agent_filter; }
  __forceinline phys_narrow_agent::agent_invalidate_sweep_fn get_narrow_agent_invalidate_sweep_fn(int type) {
      return m_narrow_agent_funcs[type].invalidate_sweep_info; }
  __forceinline phys_narrow_agent::agent_warp_time_fn get_narrow_agent_warp_time_fn(int type) { return m_narrow_agent_funcs[type].warp_time; }
  __forceinline phys_narrow_agent::agent_sep_normal_fn get_narrow_agent_sep_normal_fn(int type) { return m_narrow_agent_funcs[type].compute_sep_normal; }
  __forceinline phys_narrow_agent::agent_process_fn get_narrow_agent_process_fn(int type) { return m_narrow_agent_funcs[type].process; }
  __forceinline phys_narrow_agent::narrow_agent_symmetry get_narrow_agent_symmetric(int type) {
    return static_cast<phys_narrow_agent::narrow_agent_symmetry>(m_narrow_agent_funcs[type].m_agent_symmetry);
  }
  // True selects the predictive agent table, false selects the normal table.
  __forceinline phys_narrow_agent::narrow_agent_kind get_narrow_agent_type(phys_shape_type shape_type_a, phys_shape_type shape_type_b,
      bool agent_is_predictive) const {
    if (agent_is_predictive) {
      return m_narrow_agent_types_pred[shape_type_a][shape_type_b];
    }
    return m_narrow_agent_types[shape_type_a][shape_type_b];
  }
  __forceinline phys_bool is_pair_flipped(phys_shape_type shape_type_a, phys_shape_type shape_type_b) const {
    int idx = m_agent2_types[shape_type_a][shape_type_b];
    return m_agent2_funcs[idx].m_flipped;
  }
  __forceinline agent_funcs::agent_create_fn get_agent_create_fn(phys_shape_type shape_type_a, phys_shape_type shape_type_b,
      agent_predictive agent_is_predictive) const
  {
    int idx = (agent_is_predictive ? m_agent2_types_pred : m_agent2_types)[shape_type_a][shape_type_b];
    return m_agent2_funcs[idx].m_create_fn;
  }
  __forceinline phys_agent *create_collision_agent(const phys_collision_proxy &collider_a, const phys_collision_proxy &collider_b,
    const phys_collision_input &environment, phys_contact_manager *pair_contact_manager) const
  {
    const phys_shape_type shape_type_a = collider_a.get_shape()->get_type_id();
    const phys_shape_type shape_type_b = collider_b.get_shape()->get_type_id();
    // match: 0x638918 et al.: keep the value snapshot formerly returned by hkPadSpu::val().
    const phys_bool want_predictive_agents = environment.m_create_pred_agents;
    agent_predictive agent_is_predictive = static_cast<agent_predictive>((int)want_predictive_agents);
    agent_funcs::agent_create_fn f = get_agent_create_fn(shape_type_a, shape_type_b, agent_is_predictive);
    return f(collider_a, collider_b, environment, pair_contact_manager);
  }
  __forceinline agent_funcs::penetrations_fn get_penetrations_fn(phys_shape_type shape_type_a, phys_shape_type shape_type_b) const
  {
    int idx = m_agent2_types[shape_type_a][shape_type_b];
    return m_agent2_funcs[idx].m_penetrations_fn;
  }
  __forceinline agent_funcs::sweep_cast_fn get_linear_cast_fn(phys_shape_type shape_type_a, phys_shape_type shape_type_b) const
  {
    int idx = m_agent2_types[shape_type_a][shape_type_b];
    return m_agent2_funcs[idx].m_linear_cast_func;
  }
  __forceinline agent_funcs::closest_points_fn get_closest_points_fn(phys_shape_type shape_type_a, phys_shape_type shape_type_b) const
  {
    return m_agent2_funcs[m_agent2_types[shape_type_a][shape_type_b]].m_closest_points_fn;
  }
  __forceinline phys_bool has_alternate_type(phys_shape_type primary, phys_shape_type alternate)
  {
    return phys_bool((m_alternate_types[primary] & (1 << alternate)) != 0);
  }
  void register_contact_factory(phys_contact_manager_factory *factory, int shape_type_a, int shape_type_b);
  void register_contact_factory_all(phys_contact_manager_factory *factory, int response);
  void register_collision_agent(agent_funcs &dispatcher_agent_funcs, phys_shape_type shape_type_a, phys_shape_type shape_type_b);
  int register_narrow_agent(narrow_agent_funcs &dispatcher_agent_funcs, phys_shape_type shape_type_a, phys_shape_type shape_type_b);
  void register_alt_shape_type(phys_shape_type primary_shape_type, phys_shape_type alternate_type);
  void init_collision_quality(quality_init_info &input);
};

#endif
