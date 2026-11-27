// hkcollide/hkCollisionDispatcher.obj: from the records 0x61d998 0x61d9a8 0x61d9b0 0x61d9b8 0x61db28 0x61db30 0x61dc70 0x61ddb0 0x61deb0 0x61e0d0 0x61e160 0x61e530 0x61e9e0 0x61ea68 0x61eb80 0x61ed28 0x61ee40 0x61ef90 0x61f090
// @flags /TP /O2
// @address 0x61d998 ?create@phys_null_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x61d9a8 ?destroy@phys_null_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x61d9b0 ?process@phys_null_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
// @address 0x61db28 ?set_enable_checks@phys_dispatcher@@QAAXVphys_bool@@@Z
// @address 0x61dc70 ?register_contact_factory_all@phys_dispatcher@@QAAXPAVphys_contact_manager_factory@@H@Z
// @address 0x61ddb0 ?gather_memory_stats@phys_dispatcher@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x61deb0 ?register_agent_internal@phys_dispatcher@@IAAXQAY0CA@EHW4phys_shape_type@@111PAY1CA@CA@Udispatch_debug_entry@1@H@Z
// @address 0x61e0d0 ?update_alternate_type@phys_dispatcher@@IAAXW4phys_shape_type@@0H@Z
// @address 0x61e160 ?print_debug_table@phys_dispatcher@@QAAXXZ
// @address 0x61e530 ?init_collision_quality@phys_dispatcher@@QAAXAAUquality_init_info@1@@Z
// @address 0x61e9e0 ?disable_debugging@phys_dispatcher@@QAAXXZ
// @address 0x61ea68 ?register_collision_agent@phys_dispatcher@@QAAXAAUagent_funcs@1@W4phys_shape_type@@1@Z
// @address 0x61eb80 ?register_narrow_agent@phys_dispatcher@@QAAHAAUnarrow_agent_funcs@1@W4phys_shape_type@@1@Z
// @address 0x61ed28 ?register_alt_shape_type@phys_dispatcher@@QAAXW4phys_shape_type@@0@Z
// @address 0x61ef90 ??1phys_dispatcher@@UAA@XZ
// @address 0x61f090 ??_Gphys_dispatcher@@UAAPAXI@Z
// @address 0x61db30 ?register_contact_factory@phys_dispatcher@@QAAXPAVphys_contact_manager_factory@@HH@Z
#include "physics/havok_halo_external.h"
#include "phys/phys_collide.h"

#include "phys/phys_thread_memory.h"
#include "phys/phys_base_types.h"
#include "phys/phys_null_agent.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_error.h"
#include "phys/phys_thread_memory_allocator.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_narrow_agent_bridge.h"
#include "phys/phys_object.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_agent.h"
#include "phys/phys_string.h"
#include "phys/phys_writer.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_math.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collision_input.h"

namespace {
}
#include "phys/phys_agent_entry.h"
#include "phys/phys_shape_type.h"
#include "phys/phys_err_stream.h"
namespace phys_null_narrow_agent {
  void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data)
  {
    entry->m_stream_command = 0;
    return agent_data;
  }
  void destroy(phys_agent_entry *entry, void *agent_data, phys_contact_manager *agent_contact_manager,
               phys_constraint_owner &constraint_owner_ref, phys_dispatcher *dispatcher)
  {
  }
  void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data,
                phys_vec4 *simplex_separating_normal, phys_process_collision_output &output)
  {
    return agent_data;
  }
}
// @address 0x61ee40 ??0phys_dispatcher@@QAA@P6APAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@ZPAVphys_contact_manager_factory@@@Z
phys_dispatcher::phys_dispatcher(agent_funcs::agent_create_fn default_agent_fn, phys_contact_manager_factory *default_contact_factory)
  : m_default_agent_create(default_agent_fn)
{
  m_debug_table = 0;
  m_debug_table_pred = 0;
  m_debug_table3 = 0;
  m_debug_table3_pred = 0;
  m_agent_registered = false;
  m_checks_enabled = true;
  m_narrow_agent_type_count = 0;
  {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        m_contact_manager_factory[i][j] = default_contact_factory;
        if (default_contact_factory)
          default_contact_factory->add_object_reference();
      }
    }
  }
  {
    for (int i = 0; i < 32; i++)
      m_alternate_types[i] = 1 << i;
  }
  reset_creation_functions();
  m_narrow_agent_registered = false;
}

// @address 0x61d9b8 ?reset_creation_functions@phys_dispatcher@@IAAXXZ
void phys_dispatcher::reset_creation_functions()
{
  m_agent2_type_count = 1;
  m_narrow_agent_type_count = 1;
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 32; ++j) {
      if (m_debug_table) {
        m_debug_table[0][i][j].m_data[2] = 100;
        m_debug_table_pred[0][i][j].m_data[2] = 100;
        m_debug_table3[0][i][j].m_data[2] = 100;
        m_debug_table3_pred[0][i][j].m_data[2] = 100;
      }
      m_agent2_types[i][j] = 0;
      m_narrow_agent_types[i][j] = 0;
      m_agent2_types_pred[i][j] = 0;
      m_narrow_agent_types_pred[i][j] = 0;
    }
  }
  agent_funcs &f2 = m_agent2_funcs[0];
  f2.m_create_fn = phys_null_agent::create_null_agent;
  f2.m_penetrations_fn = phys_null_agent::static_penetrations;
  f2.m_closest_points_fn = phys_null_agent::static_closest_points;
  f2.m_linear_cast_func = phys_null_agent::static_sweep_cast;
  f2.m_flipped = false;
  f2.m_predictive = true;
  narrow_agent_funcs_internal &f3 = m_narrow_agent_funcs[0];
  f3.cleanup = 0;
  f3.create = phys_null_narrow_agent::create;
  f3.destroy = phys_null_narrow_agent::destroy;
  f3.bridge_remove_point = 0;
  f3.commit_potential = 0;
  f3.mark_zombie_entry = 0;
  f3.update_agent_filter = 0;
  f3.compute_sep_normal = 0;
  f3.invalidate_sweep_info = 0;
  f3.warp_time = 0;
  f3.process = phys_null_narrow_agent::process;
  f3.m_agent_symmetry = narrow_agent_funcs_internal::DISPATCH_SYMMETRIC;
  phys_narrow_agent_bridge::register_narrow_agent(this);
  m_agent_registered = false;
  if (m_debug_table3) {
    for (int i = 0; i < 32; ++i) {
      for (int j = 0; j < 32; ++j) {
        m_debug_table3[0][i][j].m_data[2] = 100;
        m_debug_table3_pred[0][i][j].m_data[2] = 100;
      }
    }
  }
}
void phys_dispatcher::set_enable_checks(phys_bool enabled)
{
  m_checks_enabled = enabled;
}

void phys_dispatcher::register_contact_factory(phys_contact_manager_factory *factory, int shape_type_a, int shape_type_b)
{
  factory->add_object_reference();
  m_contact_manager_factory[shape_type_b][shape_type_a]->object_remove_ref();
  m_contact_manager_factory[shape_type_b][shape_type_a] = factory;
  factory->add_object_reference();
  m_contact_manager_factory[shape_type_a][shape_type_b]->object_remove_ref();
  m_contact_manager_factory[shape_type_a][shape_type_b] = factory;
}

void phys_dispatcher::register_contact_factory_all(phys_contact_manager_factory *factory, int response)
{
  for (int index = 0; index < 8; ++index) {
    factory->add_object_reference();
    m_contact_manager_factory[index][response]->object_remove_ref();
    m_contact_manager_factory[index][response] = factory;
    factory->add_object_reference();
    m_contact_manager_factory[response][index]->object_remove_ref();
    m_contact_manager_factory[response][index] = factory;
  }
}


void phys_dispatcher::gather_memory_stats(phys_statistics_collector *collector) const
{
  collector->begin_stats_object(0, phys_statistics_collector::stat_category_instance, this);
  if (m_debug_table) {
    collector->add_chunk_stats("DebugTable", phys_statistics_collector::stat_category_engine, m_debug_table, 3072, 0);
    collector->add_chunk_stats("DebugTable", phys_statistics_collector::stat_category_engine, m_debug_table_pred, 3072, 0);
    collector->add_chunk_stats("DebugTable", phys_statistics_collector::stat_category_engine, m_debug_table3, 3072, 0);
    collector->add_chunk_stats("DebugTable", phys_statistics_collector::stat_category_engine, m_debug_table3_pred, 3072, 0);
  }
  collector->end_object();
}

void phys_dispatcher::register_agent_internal(unsigned char (* const table)[32], int agent_type_id,
  phys_shape_type shape_type_a, phys_shape_type shape_type_b, phys_shape_type orig_type_a, phys_shape_type orig_type_b,
  dispatch_debug_entry (*debug_table)[32][32], int depth)
{
  m_agent_registered = true;
  for (int index = 0; index < m_shape_inheritance.getSize(); ++index) {
    shape_inheritance &inheritance = m_shape_inheritance[index];
    if (inheritance.m_alternate_type == shape_type_a)
      register_agent_internal(table, agent_type_id, inheritance.m_primary_type, shape_type_b,
        orig_type_a, orig_type_b, debug_table, depth + 1);
    if (inheritance.m_alternate_type == shape_type_b)
      register_agent_internal(table, agent_type_id, shape_type_a, inheritance.m_primary_type,
        orig_type_a, orig_type_b, debug_table, depth + 1);
  }
  int first_edge_a = shape_type_a;
  int first_edge_b = shape_type_b;
  int end_type_a = shape_type_a + 1;
  int end_type_b = shape_type_b + 1;
  int priority = depth;
  if (shape_type_a == -1) {
    first_edge_a = 1;
    end_type_a = 32;
    ++priority;
  }
  if (shape_type_b == -1) {
    first_edge_b = 1;
    end_type_b = 32;
    ++priority;
  }
  for (int penetration_shape_a = first_edge_a; penetration_shape_a < end_type_a; ++penetration_shape_a) {
    for (int penetration_shape_b = first_edge_b; penetration_shape_b < end_type_b; ++penetration_shape_b) {
      table[penetration_shape_a][penetration_shape_b] = (unsigned char)agent_type_id;
      if (debug_table != 0) {
        dispatch_debug_entry &entry = (*debug_table)[penetration_shape_a][penetration_shape_b];
        if (m_checks_enabled && priority > (signed char)entry.m_data[2]) {
          const char *old_a_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[0]);
          const char *old_b_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[1]);
          const char *new_proxy_a = phys_get_shape_type_name(shape_type_a);
          const char *new_b_name = phys_get_shape_type_name(shape_type_b);
          char message[1024];
          phys_string::format_string_n(message, 1000,
            "Agent handling types <%s-%s> would override more specialized agent <%s-%s>\n"
            "Maybe the order of registering your collision agent is wrong, "
            "make sure you register your alternate type agents first",
            new_proxy_a, new_b_name, old_a_name, old_b_name);
        }
        entry.m_data[2] = (signed char)priority;
        entry.m_data[0] = (signed char)orig_type_a;
        entry.m_data[1] = (signed char)orig_type_b;
      }
    }
  }
}


void phys_dispatcher::update_alternate_type(phys_shape_type primary_shape_type, phys_shape_type alternate_type, int depth)
{
  m_alternate_types[primary_shape_type] |= m_alternate_types[alternate_type];
  for (int index = 0; index < m_shape_inheritance.getSize(); ++index) {
    shape_inheritance &inheritance = m_shape_inheritance[index];
    if (inheritance.m_alternate_type == primary_shape_type) {
      update_alternate_type(inheritance.m_primary_type, inheritance.m_alternate_type, depth + 1);
    }
  }
}

void phys_dispatcher::disable_debugging()
{
  if (m_debug_table) {
    phys_free_memory(m_debug_table);
    phys_free_memory(m_debug_table_pred);
    phys_free_memory(m_debug_table3);
    phys_free_memory(m_debug_table3_pred);
    m_debug_table = 0;
    m_debug_table_pred = 0;
    m_debug_table3 = 0;
    m_debug_table3_pred = 0;
  }
}

void phys_dispatcher::register_collision_agent(agent_funcs &dispatcher_agent_funcs, phys_shape_type shape_type_a, phys_shape_type shape_type_b)
{
  m_agent2_funcs[m_agent2_type_count] = dispatcher_agent_funcs;
  register_agent_internal(m_narrow_agent_types, 1, shape_type_a, shape_type_b, shape_type_a, shape_type_b, m_debug_table3, 0);
  register_agent_internal(m_agent2_types, m_agent2_type_count, shape_type_a, shape_type_b, shape_type_a, shape_type_b, m_debug_table, 0);
  if (dispatcher_agent_funcs.m_predictive) {
    register_agent_internal(m_narrow_agent_types_pred, 1, shape_type_a, shape_type_b, shape_type_a, shape_type_b, m_debug_table3_pred, 0);
    register_agent_internal(m_agent2_types_pred, m_agent2_type_count, shape_type_a, shape_type_b, shape_type_a, shape_type_b, m_debug_table_pred, 0);
  }
  ++m_agent2_type_count;
}

int phys_dispatcher::register_narrow_agent(narrow_agent_funcs &dispatcher_agent_funcs, phys_shape_type shape_type_a, phys_shape_type shape_type_b)
{
  m_narrow_agent_registered = true;
  narrow_agent_funcs_internal internal_funcs;
  static_cast<narrow_agent_funcs &>(internal_funcs) = dispatcher_agent_funcs;
  internal_funcs.m_agent_symmetry = narrow_agent_funcs_internal::DISPATCH_SYMMETRIC;
  if (shape_type_a != shape_type_b && !dispatcher_agent_funcs.symmetric) {
    internal_funcs.m_agent_symmetry = narrow_agent_funcs_internal::AGENT_ORDER_FLIPPED;
    m_narrow_agent_funcs[m_narrow_agent_type_count] = internal_funcs;
    register_agent_internal(m_narrow_agent_types, m_narrow_agent_type_count, shape_type_b, shape_type_a,
      shape_type_b, shape_type_a, m_debug_table3, 0);
    if (internal_funcs.agent_is_predictive)
      register_agent_internal(m_narrow_agent_types_pred, m_narrow_agent_type_count, shape_type_b, shape_type_a,
        shape_type_b, shape_type_a, m_debug_table3_pred, 0);
    ++m_narrow_agent_type_count;
    internal_funcs.m_agent_symmetry = narrow_agent_funcs_internal::AGENT_ORDER_DIRECT;
  }
  m_narrow_agent_funcs[m_narrow_agent_type_count] = internal_funcs;
  register_agent_internal(m_narrow_agent_types, m_narrow_agent_type_count, shape_type_a, shape_type_b,
    shape_type_a, shape_type_b, m_debug_table3, 0);
  if (internal_funcs.agent_is_predictive)
    register_agent_internal(m_narrow_agent_types_pred, m_narrow_agent_type_count, shape_type_a, shape_type_b,
      shape_type_a, shape_type_b, m_debug_table3_pred, 0);
  int agent_type_id = m_narrow_agent_type_count;
  ++m_narrow_agent_type_count;
  return agent_type_id;
}

void phys_dispatcher::register_alt_shape_type(phys_shape_type primary_shape_type, phys_shape_type alternate_type)
{
  for (int index = 0; index < m_shape_inheritance.getSize(); ++index) {
    shape_inheritance &inheritance = m_shape_inheritance[index];
    if (inheritance.m_primary_type == primary_shape_type && inheritance.m_alternate_type == alternate_type) {
      m_shape_inheritance.remove_at_and_copy(index);
      --index;
    }
  }
  update_alternate_type(primary_shape_type, alternate_type, 0);
  shape_inheritance &inheritance = m_shape_inheritance.expand_one();
  inheritance.m_primary_type = primary_shape_type;
  inheritance.m_alternate_type = alternate_type;
}

static __forceinline float phys_dispatcher_fabs(float r) { return static_cast<float>(::fabs(static_cast<double>(r))); }

void phys_dispatcher::init_collision_quality(quality_init_info &input)
{
  m_expected_min_psi_dt = input.m_min_dt;
  m_expected_max_lin_vel = input.m_max_lin_vel;

  float dist_per_step = 0.5f * input.m_gravity_len * input.m_min_dt * input.m_min_dt;

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_DISCRETE];

    sq.m_toi_weld_reject = input.m_toi_weld_reject;

    sq.m_keep_contact = input.m_collision_tolerance;
    sq.m_create_4d_contact = input.m_collision_tolerance;

    sq.m_manifold_sweep_dist = input.m_collision_tolerance;
    if (input.m_neg_manifold_sweeps) {
      sq.m_manifold_sweep_dist = -2.0f * dist_per_step;
    }
    sq.m_create_contact = input.m_collision_tolerance;
    if (input.m_neg_tol_non4d_contacts) {
      sq.m_create_contact = -1.0f * dist_per_step;
    }

    sq.m_max_constraint_violation = 3.40282e+38f;
    sq.m_use_ccd = false;
    sq.m_constraint_priority = input.m_default_constraint_priority;

    float d = -1e20f;
    sq.m_min_separation = d * 0.5f;
    sq.m_min_extra_separation = d * 0.5f;
    sq.m_toi_separation = d * 0.1f;
    sq.m_toi_extra_separation = d * 0.1f;
    sq.m_toi_accuracy = phys_dispatcher_fabs(d * 0.05f);
    sq.m_min_safe_dt = 1.0f;
    sq.m_min_abs_safe_dt = 1.0f;
    sq.m_min_toi_dt = 1.0f;
  }

  {
    m_quality_info[COLLIDE_QUALITY_EXPAND_MANIFOLD_TMP] = m_quality_info[COLLIDE_QUALITY_DISCRETE];
    m_quality_info[COLLIDE_QUALITY_TOI] = m_quality_info[COLLIDE_QUALITY_DISCRETE];
    m_quality_info[COLLIDE_QUALITY_TOI_HIGH] = m_quality_info[COLLIDE_QUALITY_DISCRETE];
    m_quality_info[COLLIDE_QUALITY_TOI_FORCED] = m_quality_info[COLLIDE_QUALITY_DISCRETE];
  }

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_EXPAND_MANIFOLD_TMP];
    sq.m_manifold_sweep_dist = input.m_collision_tolerance;
    sq.m_create_4d_contact = input.m_collision_tolerance;
    sq.m_create_contact = input.m_collision_tolerance;
  }

  const float radius_to_pen_ratio = 5.0f;

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_TOI];

    if (input.m_ccd_enabled) {
      const int tois_till_allowed_pen = 3;
      const float max_initial_rel_pen = 0.6f;

      sq.m_min_separation = -1.0f * max_initial_rel_pen;
      sq.m_min_extra_separation = -1.0f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_separation = -0.5f * max_initial_rel_pen;
      sq.m_toi_extra_separation = -0.7f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_accuracy = -0.3f * sq.m_toi_extra_separation;
      sq.m_max_constraint_violation = -1.0f * sq.m_min_extra_separation;

      sq.m_min_safe_dt = radius_to_pen_ratio / input.m_max_lin_vel;
      sq.m_min_abs_safe_dt = 0.005f / input.m_max_lin_vel;
      sq.m_min_toi_dt = -2.0f * sq.m_min_extra_separation / input.m_max_lin_vel;

      sq.m_constraint_priority = input.m_toi_constraint_priority;
      sq.m_use_ccd = true;
    }
  }

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_TOI_HIGH];

    if (input.m_ccd_enabled) {
      const int tois_till_allowed_pen = 4;
      const float max_initial_rel_pen = 0.5f;

      sq.m_min_separation = -1.0f * max_initial_rel_pen;
      sq.m_min_extra_separation = -1.0f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_separation = -0.5f * max_initial_rel_pen;
      sq.m_toi_extra_separation = -0.7f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_accuracy = -0.3f * sq.m_toi_extra_separation;
      sq.m_max_constraint_violation = -1.0f * sq.m_min_extra_separation;

      sq.m_min_safe_dt = radius_to_pen_ratio / input.m_max_lin_vel;
      sq.m_min_abs_safe_dt = 0.005f / input.m_max_lin_vel;
      sq.m_min_toi_dt = -1.0f * sq.m_min_extra_separation / input.m_max_lin_vel;

      sq.m_constraint_priority = input.m_toi_higher_priority;
      sq.m_use_ccd = true;
    }
  }

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_TOI_FORCED];

    sq.m_keep_contact = input.m_collision_tolerance;

    if (input.m_ccd_enabled) {
      const int tois_till_allowed_pen = 20;
      const float max_initial_rel_pen = 0.4f;

      sq.m_min_separation = -1.0f * max_initial_rel_pen;
      sq.m_min_extra_separation = -1.0f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_separation = -0.5f * max_initial_rel_pen;
      sq.m_toi_extra_separation = -0.7f * (1.0f - max_initial_rel_pen) / (tois_till_allowed_pen - 1);
      sq.m_toi_accuracy = -0.3f * sq.m_toi_extra_separation;
      sq.m_max_constraint_violation = -1.0f * sq.m_min_extra_separation;

      sq.m_min_safe_dt = radius_to_pen_ratio / input.m_max_lin_vel;
      sq.m_min_abs_safe_dt = 0.005f / input.m_max_lin_vel;
      sq.m_min_toi_dt = -1.0f * sq.m_min_extra_separation / input.m_max_lin_vel;

      sq.m_constraint_priority = input.m_toi_forced_priority;
      sq.m_use_ccd = true;
    }
  }

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_CHARACTER];
    sq = m_quality_info[COLLIDE_QUALITY_TOI_HIGH];
    sq.m_manifold_sweep_dist = input.m_collision_tolerance;
    sq.m_create_contact = input.m_collision_tolerance;
  }

  {
    phys_collision_quality_info &sq = m_quality_info[COLLIDE_QUALITY_CHARACTER_FORCED];
    sq = m_quality_info[COLLIDE_QUALITY_TOI_FORCED];
    sq.m_manifold_sweep_dist = input.m_collision_tolerance;
    sq.m_create_contact = input.m_collision_tolerance;
  }

  {
    for (int i = 0; i < PHYS_COLLIDER_QUALITY_MAX; i++) {
      for (int j = 0; j < PHYS_COLLIDER_QUALITY_MAX; j++) {
        m_quality_table[i][j] = COLLIDE_QUALITY_DISCRETE;
      }
    }
  }

#define T m_quality_table
#define L PHYS_COLLIDER_QUALITY_FIXED
#define K PHYS_COLLIDER_QUALITY_KEYFRAMED
#define R PHYS_COLLIDER_QUALITY_KEYFRAMED_REPORTING
#define D PHYS_COLLIDER_QUALITY_DEBRIS
#define M PHYS_COLLIDER_QUALITY_MOVING
#define C PHYS_COLLIDER_QUALITY_CRITICAL
#define B PHYS_COLLIDER_QUALITY_BULLET
#define P PHYS_COLLIDER_QUALITY_CHARACTER
#define QI char(COLLIDE_QUALITY_INVALID)
#define QP char(COLLIDE_QUALITY_DISCRETE)
#define QT char(COLLIDE_QUALITY_TOI)
#define QH char(COLLIDE_QUALITY_TOI_HIGH)
#define QF char(COLLIDE_QUALITY_TOI_FORCED)
#define QC char(COLLIDE_QUALITY_CHARACTER)
#define QG char(COLLIDE_QUALITY_CHARACTER_FORCED)

  T[L][L] = QI; T[L][K] = QI; T[L][R] = QT; T[L][D] = QP; T[L][M] = QH; T[L][C] = QF; T[L][B] = QH; T[L][P] = QG;
  T[K][L] = QI; T[K][K] = QI; T[K][R] = QT; T[K][D] = QP; T[K][M] = QT; T[K][C] = QT; T[K][B] = QT; T[K][P] = QC;
  T[R][L] = QT; T[R][K] = QT; T[R][R] = QT; T[R][D] = QP; T[R][M] = QT; T[R][C] = QT; T[R][B] = QT; T[R][P] = QC;
  T[D][L] = QP; T[D][K] = QP; T[D][R] = QP; T[D][D] = QP; T[D][M] = QP; T[D][C] = QP; T[D][B] = QT; T[D][P] = QP;
  T[M][L] = QH; T[M][K] = QT; T[M][R] = QT; T[M][D] = QP; T[M][M] = QP; T[M][C] = QT; T[M][B] = QT; T[M][P] = QT;
  T[C][L] = QF; T[C][K] = QT; T[C][R] = QT; T[C][D] = QP; T[C][M] = QT; T[C][C] = QT; T[C][B] = QT; T[C][P] = QC;
  T[B][L] = QH; T[B][K] = QT; T[B][R] = QT; T[B][D] = QT; T[B][M] = QT; T[B][C] = QT; T[B][B] = QT; T[B][P] = QT;
  T[P][L] = QG; T[P][K] = QC; T[P][R] = QC; T[P][D] = QP; T[P][M] = QT; T[P][C] = QC; T[P][B] = QT; T[P][P] = QT;

#undef T
#undef L
#undef K
#undef R
#undef D
#undef M
#undef C
#undef B
#undef P
#undef QI
#undef QP
#undef QT
#undef QH
#undef QF
#undef QC
#undef QG
}

phys_dispatcher::~phys_dispatcher()
{
  disable_debugging();
  for (int response_a = 0; response_a < 8; ++response_a) {
    for (int response_b = 0; response_b < 8; ++response_b) {
      if (m_contact_manager_factory[response_a][response_b] != 0)
        m_contact_manager_factory[response_a][response_b]->object_remove_ref();
    }
  }
}

void phys_dispatcher::print_debug_table()
{
  char buf[255];
  phys_error *error = &phys_error::get_singleton_instance();
  error->begin_section(0x5e4345e4, "hkCollisionDispatcher::debugPrintTable");
  if (m_debug_table != 0 && m_debug_table_pred != 0) {
    for (int a = 0; a < 32; ++a) {
      phys_string label = phys_string("\nEntries for (continuous)") + phys_get_shape_type_name(static_cast<phys_shape_type>(a));
      {
        char report_buf[512];
        phys_err_stream report_stream(report_buf, 0x200);
        report_stream << label;
        phys_error::get_singleton_instance().message(phys_error::MESSAGE_REPORT, -1, report_buf, ".\\dispatch\\hkCollisionDispatcher.cpp", 509);
      }
      for (int b = 1; b < 32; ++b) {
        dispatch_debug_entry &entry = m_debug_table_pred[0][a][b];
        if ((signed char)entry.m_data[2] < 100) {
          const char *sB = phys_get_shape_type_name(static_cast<phys_shape_type>(b));
          const char *old_a_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[0]);
          const char *old_b_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[1]);
          phys_string::format_string_n(buf, 255, "vs %30s <%i:%s-%s>", sB, (int)(signed char)entry.m_data[2], old_a_name, old_b_name);
          char report_buf[512];
          phys_err_stream report_stream(report_buf, 0x200);
          report_stream << buf;
          phys_error::get_singleton_instance().message(phys_error::MESSAGE_REPORT, -1, report_buf, ".\\dispatch\\hkCollisionDispatcher.cpp", 524);
        }
      }
    }
    for (int a = 0; a < 32; ++a) {
      phys_string label = phys_string("\nEntries for (discrete)") + phys_get_shape_type_name(static_cast<phys_shape_type>(a));
      {
        char report_buf[512];
        phys_err_stream report_stream(report_buf, 0x200);
        report_stream << label;
        phys_error::get_singleton_instance().message(phys_error::MESSAGE_REPORT, -1, report_buf, ".\\dispatch\\hkCollisionDispatcher.cpp", 532);
      }
      for (int b = 1; b < 32; ++b) {
        dispatch_debug_entry &entry = m_debug_table[0][a][b];
        if ((signed char)entry.m_data[2] < 100) {
          const char *sB = phys_get_shape_type_name(static_cast<phys_shape_type>(b));
          const char *old_a_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[0]);
          const char *old_b_name = phys_get_shape_type_name((phys_shape_type)(signed char)entry.m_data[1]);
          phys_string::format_string_n(buf, 255, "vs %30s <%i:%s-%s>", sB, (int)(signed char)entry.m_data[2], old_a_name, old_b_name);
          char report_buf[512];
          phys_err_stream report_stream(report_buf, 0x200);
          report_stream << buf;
          phys_error::get_singleton_instance().message(phys_error::MESSAGE_REPORT, -1, report_buf, ".\\dispatch\\hkCollisionDispatcher.cpp", 547);
        }
      }
    }
    phys_error::get_singleton_instance().end_section();
  }
}

