// blamlibxbox/havok_collision_damage.obj: from the records 0x108628 0x260bc8 0x260e70 0x260f70 0x2618f0 0x261998 0x261d50 0x261e38 0x261fa0 0x262000 0x262298
// @flags /TP /O2 /Oc
#include "physics/havok_collision_damage_queries.h"
#include "math/real_math_constants.h"
#include "math/real_constant_pool_access.h"
#include "physics/havok_collision_damage_externs.h"
#include "objects/object_globals.h"
#include "objects/damage_data.h"
#include "game/material_constants.h"
#include "game/game_globals_storage.h"
#include "objects/object_header.h"
#include "objects/object_datum.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_types.h"
#include "units/unit_types.h"
#include "units/vehicle_types.h"
#include "units/motor_request.h"
#include "phys/phys_base.h"
#include "phys/phys_dynamics.h"
#include "objects/objects.h"
#include "units/units.h"
#include "objects/damage.h"
#include "objects/damage_effect_definitions.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "effects/effects.h"
#include "physics/breakable_surfaces.h"
#include "sound/game_sound.h"
#include "physics/havok_collision_damage.h"
#include "physics/havok_contact_point.h"
#include "objects/object_marker_globals_types.h"
#include "scenario/scenario.h"
#include "game/game_allegiance.h"
#include "motor/motor_system.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_bsp_cache_types.h"
#include "scenario/scenario_tag.h"
#include "physics/havok_component.h"
#include "structures/structure_instanced_geometry.h"
#include "structures/structure_bsp_tag.h"
#include "objects/object_definitions.h"
#include "units/vehicle_definitions.h"
#include "game/game_globals.h"
#include "physics/havok_contact_point_reference.h"
#include "effects/material_effect_types.h"
#include "effects/material_effects.h"
#include "models/model_damage_info.h"
#include "models/model_types.h"
#include "physics/havok_monitor_storage.h"
#include "units/biped_types.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/havok_component_class.h"
#include "scenario/scenario_access.h"
#include "physics/havok_contact_point_iterator.h"
#include "game/game_time_access.h"
#include "memory/datum_index.h"
#include "motor/mover_inline.h"
#include "units/unit_seat_flags.h"
#include "game/game_simulation_predicates.h"
#include "game/game_globals_inline.h"
#include "physics/havok_shape_key.h"

struct s_havok_collision_damage_tag_instance {
  long group;
  void *definition;
};

struct s_collision_damage_definition {
  float v[10];
};

struct s_breakable_surface_damage_cache {
  s_damage_data damage_data;
  long structure_bsp_index;
  long instance_index;
  short breakable_surface_index;
  short breakable_surface_set_index;
  long surface_index;
};

struct s_havok_collision_damage_game_cache {
  real_point3d position;
  long object_index;
  float speed;
  real_vector3d velocity;
  long other_object_index;
  float impulse;
  c_global_material_type material_type;
};
// @address 0x74b910 g_havok_monitor_globals
// @address 0x7f4310 g_object_marker_globals
// @address 0x11df680 game_globals
// @address 0x8c425b havok_collision_damage_disabled
bool havok_collision_damage_disabled;

// @address 0x261d50 havok_collision_damage_collect_objects
extern "C" void havok_collision_damage_collect_objects(int object_index, int *indices)
{
  s_object_header *header = object_header_get(object_index);
  unit_datum *datum = (unit_datum *)header->datum;
  long parent_index = datum->object.parent_object_index;
  long child_index = datum->object.first_child_object_index;
  if (parent_index == -1 || (((1 << header->object_type) & 0x1003) != 0 && (unsigned short)datum->unit.parent_seat_index != 0xffff &&
                             !::unit_seat_test_flag(parent_index, (unsigned short)datum->unit.parent_seat_index, _unit_seat_boarding_seat_bit))) {
    int &count = indices[0];
    if (count != 512) {
      int slot = count++;
      indices[slot + 1] = object_index;
    }
  }
  if (child_index != -1) {
    do {
      header = object_header_get(child_index);
      datum = (unit_datum *)header->datum;
      if (((1 << header->object_type) & 0x1003) != 0)
        havok_collision_damage_collect_objects(child_index, indices);
      child_index = datum->object.next_object_index;
    } while (child_index != -1);
  }
}

// @address 0x2618f0 havok_collision_damage_apply_absolute_to_hierarchy
extern "C" void havok_collision_damage_apply_absolute_to_hierarchy(const s_collision_damage_definition *collision_damage,
    const real_point3d *point, long object_index, float impulse, c_global_material_type material_type, float speed, float relative_speed,
    long damage_owner_object_index)
{
  int indices[513];
  indices[0] = 0;
  havok_collision_damage_collect_objects(object_index, indices);
  for (int i = 0; i < indices[0]; i++)
    havok_collision_damage_apply_absolute_damage(
        collision_damage, point, indices[i + 1], impulse, material_type, speed, relative_speed, false, damage_owner_object_index);
}

// @address 0x261e38 havok_collision_damage_can_damage_objects
extern "C" bool havok_collision_damage_can_damage_objects(long object_index1, long object_index2)
{
  bool result = true;
  if (game_simulation::game_is_distributed()) {
    s_object_header *header1 = object_header_get(object_index1);
    s_object_header *header2 = object_header_get(object_index2);
    if (((1 << header1->object_type) & 1) != 0 && ((1 << header2->object_type) & 1) != 0)
      result = false;
  }
  if (result) {
    s_object_header *headers = (s_object_header *)object_header_data->data;
    s_object_header *header1 = &headers[datum_index_to_absolute_index(object_index1)];
    bool biped1_state6 = false;
    if ((1 << header1->object_type) & 1) {
      biped_datum *biped = (biped_datum *)header1->datum;
      if (biped->biped.physics.get_mode() == 6)
        biped1_state6 = true;
    }
    if (biped1_state6) {
      result = false;
    } else {
      s_object_header *header2 = &headers[datum_index_to_absolute_index(object_index2)];
      bool biped2_state6 = false;
      if ((1 << header2->object_type) & 1) {
        biped_datum *biped = (biped_datum *)header2->datum;
        if (biped->biped.physics.get_mode() == 6)
          biped2_state6 = true;
      }
      if (biped2_state6)
        result = false;
    }
  }
  if (result && havok_collision_damage_test_biped_state(object_index2))
    result = false;
  return result;
}

// @address 0x261fa0 havok_collision_damage_test_biped_state
extern "C" bool havok_collision_damage_test_biped_state(long object_index)
{
  s_object_header *header = object_header_get(object_index);
  bool result = false;
  if ((1 << header->object_type) & 1) {
    biped_datum *biped = (biped_datum *)header->datum;
    int state = biped->biped.physics.get_mode();
    result = state == 4 || state == 5;
  }
  return result;
}

static __forceinline object_datum *havok_collision_damage_get_object_datum(long object_index)
{
  // match: 0x260f70: the header table read directly; object_get_and_verify_type() here swaps one fmuls operand pair
  return (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}

static __forceinline unsigned char havok_collision_damage_get_object_type(long object_index)
{
  return object_header_get(object_index)->object_type;
}

static __forceinline s_game_globals *havok_collision_damage_get_game_globals()
{
  return ((s_game_globals *)global_game_globals);
}

// @rdata 0x82022838
static const float k_havok_collision_damage_scale = 0.05f;

// 0x262000 (dev name ?havok_collision_damage_collision_damage_definition_get@@YA_NJPAUs_collision_damage_definition@@@Z)
// @address 0x262000 havok_collision_damage_collision_damage_definition_get
// match: 0x262000: pointer __restrict qualifier prevents alias reload
extern "C" bool havok_collision_damage_collision_damage_definition_get(long object_index, s_collision_damage_definition *__restrict out)
{
  object_datum *datum = havok_collision_damage_get_object_datum(object_index);
  s_object_definition *definition = (s_object_definition *)tag_get(0,
      datum->definition_index);
  long collision = definition->collision_damage.index;
  bool result = false;
  if ((1 << havok_collision_damage_get_object_type(object_index)) & 0x1003)
  {
    vehicle_definition *unit = (vehicle_definition *)tag_get(0, datum->definition_index);
    if (unit->boost_collision_damage.index != -1 && ::unit_compute_boost_fraction(object_index) != 0.0f)
      collision = unit->boost_collision_damage.index;
  }
  if (collision != -1) {
    *out = *(s_collision_damage_definition *)tag_get(0, collision);
    if (havok_collision_damage_get_game_globals()->collision_damage.index != -1) {
      const s_collision_damage_definition *defaults = (const s_collision_damage_definition *)tag_get(0,
          havok_collision_damage_get_game_globals()->collision_damage.index);
      if (!(out->v[0] > 0.0f)) out->v[0] = defaults->v[0];
      if (!(out->v[1] > 0.0f)) out->v[1] = defaults->v[1];
      if (!(out->v[2] > 0.0f)) out->v[2] = defaults->v[2];
      if (!(out->v[3] > 0.0f)) out->v[3] = defaults->v[3];
      if (!(out->v[4] > 0.0f)) out->v[4] = defaults->v[4];
      if (!(out->v[5] > 0.0f)) out->v[5] = defaults->v[5];
      if (!(out->v[6] > 0.0f)) out->v[6] = defaults->v[6];
      if (!(out->v[7] > 0.0f)) out->v[7] = defaults->v[7];
      if (!(out->v[8] > 0.0f)) out->v[8] = defaults->v[8];
      if (!(out->v[9] > 0.0f)) out->v[9] = defaults->v[9];
    }
    result = true;
  } else if (havok_collision_damage_get_game_globals()->collision_damage.index != -1) {
    *out = *(s_collision_damage_definition *)tag_get(0, havok_collision_damage_get_game_globals()->collision_damage.index);
    result = true;
  }
  if (result) {
    out->v[2] = MIN(out->v[2], out->v[3] - k_real_one_thousandth_storage);
    out->v[6] = MIN(out->v[6], out->v[7] - k_real_one_thousandth_storage);
    if (datum->collision_damage_scale) {
      float scale = datum->collision_damage_scale * k_havok_collision_damage_scale;
      out->v[2] *= scale;
      out->v[3] *= scale;
      out->v[6] *= scale;
      out->v[7] *= scale;
    }
  }
  return result;
}

// TU-owned .rdata scalars.
// @rdata 0x82022dc0
static const float k_havok_collision_damage_rounding = 0.5f;
// @rdata 0x82022dbc
static const float k_havok_collision_damage_contact_duration = 0.1f;
// @rdata 0x82029c6c
static const float k_havok_collision_damage_keyframe_duration = 0.125f;
// @rdata 0x820242d8
static const float k_havok_collision_damage_reconnect_duration = 2.0f;
// @rdata 0x82033840
static const float k_havok_collision_damage_keyframe_penetration = -0.125f;
// @rdata 0x8203384c
static const float k_havok_collision_damage_environment_penetration = -0.075f;

// @address 0x262298 havok_collision_damage_compute_multiplier
extern "C" float havok_collision_damage_compute_multiplier(int tag_index, int object_index, float fraction, bool *full_out)
{
  s_damage_effect_definition *definition = (s_damage_effect_definition *)tag_get(0, tag_index);
  float scale = definition->damage.damage_upper_bound.upper;
  object_datum *datum = havok_collision_damage_get_object_datum(object_index);
  float maximum_shield_vitality = datum->maximum_shield_vitality;
  float maximum_body_vitality = datum->maximum_body_vitality;
  float total = maximum_body_vitality + maximum_shield_vitality;
  float limit = datum->body_vitality * maximum_body_vitality + datum->shield_vitality * maximum_shield_vitality;
  // The four loads below share one float-table base in the image; naming them
  // separately adds base materializations (see REPORT "Needs a rule exemption").
  float result = real_constant_pool_zero();
  float value = total - total * fraction;
  bool full = fraction >= real_constant_pool_one();
  *full_out = full;
  if (!full) {
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x262298: 0.05f and 0.0001f load off one 0x820221fc GPR base (literals: separate lis per load)
    float minimum = (maximum_body_vitality + maximum_shield_vitality) * k_real_constant_pool_one[399];
#else
    float minimum = (maximum_body_vitality + maximum_shield_vitality) * 0.05f;
#endif
    if (!(value > minimum))
      value = minimum;
  }
  if (value >= limit)
    return result;
  float divisor = scale * fraction;
#if defined(_XBOX) && !defined(__clang__)
  // match: 0x262298: 0.0001f loads off the 0x820221fc GPR base shared with 0.05f
  if (divisor <= k_real_constant_pool_one[2500])
#else
  if (divisor <= 0.0001f)
#endif
    return result;
  result = (limit - value) / divisor;
  if (!full)
    return result;
  return result + 0.2f;
}

typedef char collision_damage_rigid_body_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component::c_rigid_body) == 0x48))) ? 1 : -1];
typedef char collision_damage_component_size[(sizeof(void *) != 4 || ((sizeof(c_havok_component) == 0x90))) ? 1 : -1];
typedef char collision_damage_component_bodies_offset[(offsetof(c_havok_component, m_rigid_bodies) == 0x20) ? 1 : -1];

// match: 0x260f70: preserve the inline validated-index boundary; the shared getter has a strong owner in objects.cpp.
static __forceinline short havok_collision_damage_controlling_rigid_body_index(const c_havok_component *component)
{
  long result;
  if (component->m_object_postion_controlling_rigid_body_index >= 0 && component->m_object_postion_controlling_rigid_body_index <
      component->get_rigid_body_count())
    result = component->m_object_postion_controlling_rigid_body_index;
  else
    result = NONE;
  return (short)result;
}

static __forceinline c_havok_component *havok_collision_damage_get_havok_component(long havok_component_index)
{
  return &((c_havok_component *)g_havok_component_data->data)[datum_index_to_absolute_index(havok_component_index)];
}

static __forceinline collision_bsp *global_collision_bsp_get(long structure_bsp_index)
{
#ifdef HALO3_64BIT
  return const_cast<collision_bsp *>(static_cast<const collision_bsp *>(g_collision_bsp_cache.thread_users[g_registered_thread_index].collision_bsps[structure_bsp_index]));
#else
  return ((collision_bsp **)&g_collision_bsp_cache)[g_registered_thread_index * 64 + structure_bsp_index];
#endif
}

static __forceinline structure_instanced_geometry_definition *global_instanced_geometry_definition_get(
    long structure_bsp_index, long instance_definition_index)
{
  return &g_collision_bsp_cache.users[g_registered_thread_index].entries[structure_bsp_index].data[instance_definition_index];
}

static __forceinline unsigned char damage_reporting_info_build(unsigned char type, unsigned char modifier)
{
  return (unsigned char)((type << 2) | modifier);
}

// @address 0x260bc8
bool havok_collision_damage_apply_breakable_surface_damage(long object_index, s_havok_contact_point_reference havok_contact_point_reference,
    s_breakable_surface_damage_cache *breakable_surface_damage_cache)
{
  object_datum *object = object_get(object_index);
  c_havok_component *havok_component = havok_collision_damage_get_havok_component(object->havok_component_index);
  c_havok_contact_point *contact_point = havok_component->get_contact_point(&havok_contact_point_reference);
  bool result = false;
  real_vector3d normal;
  float impulse;
  real_point3d position;
  havok_component->calculate_impulse(contact_point->get_rigid_body_index_a(), contact_point->get_contact_point_object_index(),
      contact_point->get_rigid_body_index_b(), &impulse, &position, &normal);
  if (impulse > 0.4f)
  {
    long structure_bsp_index = contact_point->get_structure_bsp_index();
    long instance_index = NONE;
    long breakable_surface_index = NONE;
    long breakable_surface_set_index = NONE;
    long surface_index = NONE;
    long index;
    long sub_index;
    long type;
    values_from_shape_key(&index, &sub_index, &type, contact_point->get_environment_shape_key());
    switch (type)
    {
    case 1:
    {
      collision_surface *surface = TAG_BLOCK_GET_ELEMENT(&global_collision_bsp_get(structure_bsp_index)->surfaces, index, collision_surface);
      breakable_surface_index = surface->breakable_surface;
      breakable_surface_set_index = surface->breakable_surface_bit;
      surface_index = index;
      break;
    }
    case 5:
    {
      structure_instanced_geometry_instance *instance = TAG_BLOCK_GET_ELEMENT(
          &global_structure_bsp_get(structure_bsp_index)->instanced_geometry_instances, index, structure_instanced_geometry_instance);
      structure_instanced_geometry_definition *definition = global_instanced_geometry_definition_get(structure_bsp_index, instance->definition_index);
      collision_surface *surface = TAG_BLOCK_GET_ELEMENT(&definition->bsp.surfaces, sub_index, collision_surface);
      instance_index = index;
      breakable_surface_index = surface->breakable_surface;
      breakable_surface_set_index = surface->breakable_surface_bit;
      surface_index = sub_index;
      break;
    }
    default:
      UNREACHABLE();
    }
    long damage_effect_index = havok_collision_damage_get_game_globals()->breakable_surface_damage.index;
    if (damage_effect_index != NONE)
    {
      damage_data_new(&breakable_surface_damage_cache->damage_data, damage_effect_index);
      breakable_surface_damage_cache->structure_bsp_index = contact_point->get_structure_bsp_index();
      breakable_surface_damage_cache->damage_data.position = *contact_point->get_position();
      breakable_surface_damage_cache->damage_data.origin = breakable_surface_damage_cache->damage_data.position;
      breakable_surface_damage_cache->damage_data.direction = *contact_point->get_normal();
      normalize3d_with_default(&breakable_surface_damage_cache->damage_data.direction, global_forward3d);
      breakable_surface_damage_cache->damage_data.reporting_info = damage_reporting_info_build(2, 0);
      breakable_surface_damage_cache->damage_data.material_type = contact_point->get_global_material_type_b();
      breakable_surface_damage_cache->damage_data.location = object->location;
      breakable_surface_damage_cache->damage_data.flags |= FLAG(8);
      breakable_surface_damage_cache->instance_index = instance_index;
      breakable_surface_damage_cache->breakable_surface_set_index = (short)breakable_surface_set_index;
      breakable_surface_damage_cache->breakable_surface_index = (short)breakable_surface_index;
      breakable_surface_damage_cache->surface_index = surface_index;
      result = true;
    }
  }
  return result;
}

static __forceinline bool havok_collision_damage_biped_is_sentinel_mode(const biped_datum *biped)
{
  long mode = biped->biped.physics.get_mode();
  return mode == 4 || mode == 5;
}

static __forceinline bool havok_collision_damage_biped_is_melee(long object_index)
{
  bool result = false;
  if (TEST_BIT(1 << havok_collision_damage_get_object_type(object_index), 0)) {
    biped_datum *biped = (biped_datum *)havok_collision_damage_get_object_datum(object_index);
    if (biped->biped.physics.get_mode() == 6)
      result = true;
  }
  return result;
}

static __forceinline bool havok_collision_damage_object_can_take_damage(long object_index, bool absolute_damage)
{
  bool result = true;
  if (TEST_BIT(1 << havok_collision_damage_get_object_type(object_index), 0)) {
    biped_datum *biped = (biped_datum *)havok_collision_damage_get_object_datum(object_index);
    if (biped->object.parent_object_index == NONE) {
      // game_mode 2 is multiplayer (chud_motion_sensor.cpp owns the only spelling; share it when that lease frees).
      if (absolute_damage && game_globals->options.game_mode == 2)
        result = false;
      else if (biped->biped.physics.is_immune_to_collision_damage())
        result = false;
      else if (absolute_damage && havok_collision_damage_biped_is_sentinel_mode(biped))
        result = false;
      else if (((1 << havok_collision_damage_get_object_type(object_index)) & 0x1803) != 0 &&
          mover_get_motor_program(object_index) == 11)
        result = false;
      else if (havok_collision_damage_biped_is_melee(object_index))
        result = false;
    }
  }
  return result;
}

static __forceinline void havok_collision_damage_material_effect_new(long material_effects_index, long object_index,
    e_effect_type effect_type, e_sweetener_size sweetener_size, c_global_material_type material_type, bool underwater,
    const real_point3d *position, const real_vector3d *normal, const real_vector3d *direction, const s_location *location, float scale)
{
  long effects[7];
  material_effect_find_effects(false, position, material_effects_index, k_global_material_type_invalid, material_type, underwater,
      effect_type, sweetener_size, effects);
  effect_spawn_impact_effects(effects, position, normal, direction, location, NULL);
  game_sound_new_material_effect_sounds(effects, position, normal, direction, (const long *)location, object_index, scale);
}

static __forceinline bool havok_collision_damage_test_physics_flag(unsigned long flags, long bit)
{
  unsigned char result = (flags >> bit) & 1;
  return result;
}

// @address 0x261998
bool havok_collision_damage_apply_absolute_damage(const s_collision_damage_definition *collision_damage, const real_point3d *point,
    long object_index, float impulse, c_global_material_type material_type, float speed, float relative_speed, bool damage_owner_valid,
    long damage_owner_object_index)
{
  object_datum *object = object_get(object_index);
  s_object_definition *definition = (s_object_definition *)tag_get(0,
      object->definition_index);
  long damage_effect_index = havok_collision_damage_get_game_globals()->breakable_surface_damage.index;
  float maximum_speed = MAX(relative_speed, speed);
  bool result = false;
  if (object->havok_component_index != NONE && damage_effect_index != NONE && impulse > 4.0f &&
      maximum_speed > collision_damage->v[6] && collision_damage->v[7] - collision_damage->v[6] >= 0.0001f &&
      havok_collision_damage_object_can_take_damage(object_index, true)) {
    float minimum_damage = collision_damage->v[8];
    float maximum_damage = collision_damage->v[9];
    float damage_range = maximum_damage - minimum_damage;
    s_damage_data damage;
    damage_data_new(&damage, damage_effect_index);
    damage.reporting_info = damage_reporting_info_build(2, 0);
    damage.amount = (maximum_speed - collision_damage->v[6]) * damage_range / (collision_damage->v[7] - collision_damage->v[6]);
    damage.amount = PIN(damage.amount + minimum_damage, minimum_damage, maximum_damage);
    damage.position = *point;
    damage.origin = damage.position;
    if (damage_owner_valid) {
      damage_compute_damage_owner_from_object_index(damage_owner_object_index, &damage.owner);
      damage.damage_owner_object_index = damage_owner_object_index;
    } else {
      damage_compute_damage_owner_from_object_index(object_index, &damage.owner);
    }
    damage.flags |= FLAG(14);
    damage.flags |= FLAG(8);
    damage.flags |= FLAG(16);
    bool full;
    damage.multiplier = havok_collision_damage_compute_multiplier(damage.effect, object_index, damage.amount, &full);
    if (!full)
      damage.flags |= FLAG(15);
    else
      damage.flags &= ~FLAG(15);
    if (!game_simulation::game_is_predicted())
      object_cause_damage(&damage, object_index, NONE, NONE, NONE, NULL);
    if (definition->material_effects.index != NONE && havok_collision_damage_test_physics_flag(object->physics_flags, 7)) {
      real_vector3d direction = object->linear_velocity;
      normalize3d_with_default(&direction, global_up3d);
      havok_collision_damage_material_effect_new(definition->material_effects.index, object_index, _effect_type_collision_damage, _sweetener_size_large,
          material_type, true, point, &direction, &direction, &object->location, damage.amount);
    }
    result = true;
  }
  return result;
}

// match: 0x260f70: keep the unit-private squared-magnitude expression; the shared helper loses a schedule tie here.
static __forceinline float havok_collision_damage_magnitude_squared3d(const real_vector3d *vector)
{
  return ((vector->i * vector->i) + vector->j * vector->j) + vector->k * vector->k;
}

static __forceinline bool havok_collision_damage_index_is_valid(long index)
{
  bool result = index != NONE;
  return result;
}

static __forceinline bool havok_collision_damage_object_is_type(const s_object_definition *definition, e_object_type type)
{
  bool result = definition->object_type == type;
  return result;
}

static __forceinline bool havok_collision_damage_objects_are_allies(long object_index, long other_object_index)
{
  bool result = false;
  if (((1 << havok_collision_damage_get_object_type(object_index)) & 0x1003) != 0 &&
      ((1 << havok_collision_damage_get_object_type(other_object_index)) & 0x1003) != 0) {
    unit_datum *unit = (unit_datum *)havok_collision_damage_get_object_datum(object_index);
    unit_datum *other_unit = (unit_datum *)havok_collision_damage_get_object_datum(other_object_index);
    result = game_team_is_ally(unit->unit.team, other_unit->unit.team);
  }
  return result;
}

static const float k_havok_collision_damage_full_damage_bonus = 0.2f;

static __forceinline float havok_collision_damage_calculate_multiplier(long damage_definition_index, long object_index, float damage, bool *full_out)
{
  s_damage_effect_definition *definition = (s_damage_effect_definition *)tag_get(0, damage_definition_index);
  float scale = definition->damage.damage_upper_bound.upper;
  object_datum *datum = havok_collision_damage_get_object_datum(object_index);
  float vitality = datum->body_vitality * datum->maximum_body_vitality + datum->shield_vitality * datum->maximum_shield_vitality;
  float result = 1.0f;
  bool full = damage >= 1.0f;
  *full_out = full;
  if (scale > 0.001f) {
    result = vitality / scale + k_real_epsilon;
    if (!(result > 0.0f))
      result = 0.0f;
  }
  if (full)
    result += k_havok_collision_damage_full_damage_bonus;
  return result;
}

static const float k_havok_collision_damage_slow_speed = 1.5f;
static const float k_havok_collision_damage_death_acceleration = 1.6f;

// @address 0x260f70 havok_collision_damage_apply_impact_damage
extern "C" bool havok_collision_damage_apply_impact_damage(const s_collision_damage_definition *collision_damage, const
    s_collision_damage_definition *other_collision_damage,
    const real_point3d *point, long object_index, float speed, const real_vector3d *velocity_direction, const real_vector3d *normal,
    long other_object_index, c_global_material_type material_type, float relative_speed)
{
  object_datum *object = object_get(object_index);
  object_datum *other_object = object_get(other_object_index);
  s_object_definition *definition = (s_object_definition *)tag_get(0,
      object->definition_index);
  long damage_effect_index = havok_collision_damage_get_game_globals()->breakable_surface_damage.index;
  bool result = false;
  if (havok_collision_damage_index_is_valid(damage_effect_index) &&
      havok_collision_damage_index_is_valid(other_object->havok_component_index) &&
      havok_collision_damage_object_can_take_damage(object_index, false))
  {
    c_havok_component *other_havok_component = havok_collision_damage_get_havok_component(other_object->havok_component_index);
    if (havok_collision_damage_can_damage_objects(object_index, other_havok_component->get_object_index()) &&
        other_havok_component->get_rigid_body_count() > 0)
    {
      s_object_definition *other_definition =
          (s_object_definition *)tag_get(0, other_object->definition_index);
      real_vector3d velocity;
      velocity.i = 0.0f;
      velocity.k = 0.0f;
      velocity = *other_havok_component->get_rigid_body_pre_simulation_linear_velocity(0);
      float velocity_magnitude = square_root(havok_collision_damage_magnitude_squared3d(&velocity));
      if (speed >= 0.001f) {
        // match: 0x260f70: first-reference order preserves the projection multiply-add operands.
        float projection_scale = 0.0f;
        const float normal_j = normal->j;
        float dot = normal->i * velocity.i + (normal_j * velocity.j + normal->k * velocity.k);
        if (dot > 0.0f) {
          projection_scale = -dot / (speed * speed);
          velocity.i = velocity.i + projection_scale * normal->i;
          velocity.j = velocity.j + projection_scale * normal_j;
          velocity.k = velocity.k + projection_scale * normal->k;
        }
      }
      float maximum_velocity_magnitude = MAX(speed, velocity_magnitude);
      bool slow_speed_collision = speed < k_havok_collision_damage_slow_speed && velocity_magnitude < k_havok_collision_damage_slow_speed;
      if (!slow_speed_collision) {
        float scale = velocity_magnitude / maximum_velocity_magnitude;
        if (havok_collision_damage_objects_are_allies(object_index, other_object_index))
          scale *= other_collision_damage->v[1];
        else
          scale *= other_collision_damage->v[0];
        float impact_speed = relative_speed * scale;
        if (maximum_velocity_magnitude >= 0.001f && impact_speed > collision_damage->v[2] &&
            collision_damage->v[3] - collision_damage->v[2] >= k_real_epsilon) {
          bool was_dead = havok_collision_damage_test_physics_flag(object->damage_flags, 2);
          float minimum_damage = collision_damage->v[4];
          float maximum_damage = collision_damage->v[5];
          float damage_range = maximum_damage - minimum_damage;
          bool fixed_turret = false;
          s_damage_data damage;
          if (game_is_campaign() && !was_dead && havok_collision_damage_test_physics_flag(object->physics_flags, 7) &&
              TEST_BIT(1 << havok_collision_damage_get_object_type(object_index), _object_type_vehicle) &&
              vehicle_get_type(object) == _vehicle_type_turret && havok_collision_damage_index_is_valid(object->havok_component_index)) {
            c_havok_component *havok_component = havok_collision_damage_get_havok_component(object->havok_component_index);
            if (havok_collision_damage_controlling_rigid_body_index(havok_component) != NONE)
              fixed_turret = havok_component->get_rigid_body_is_fixed(havok_collision_damage_controlling_rigid_body_index(havok_component));
          }
          damage_data_new(&damage, damage_effect_index);
          real_point3d origin;
          object_get_origin(object_index, &origin);
          real_point3d other_origin;
          object_get_origin(other_object_index, &other_origin);
          vector_from_points3d(&other_origin, &origin, &damage.direction);
          normalize3d(&damage.direction);
          damage.reporting_info = damage_reporting_info_build(2, 0);
          if (havok_collision_damage_index_is_valid(other_definition->model.index)) {
            s_model_definition *model = (s_model_definition *)tag_get(0,
                other_definition->model.index);
            bool has_damage_info = model->damage_info.count != 0;
            if (has_damage_info) {
              s_model_damage_info *damage_info = TAG_BLOCK_GET_ELEMENT(
                  &model->damage_info, 0, s_model_damage_info);
              if (damage_info->collision_damage_reporting_type)
                damage.reporting_info = damage_reporting_info_build(damage_info->collision_damage_reporting_type, 3);
            }
          }
          damage.amount = (impact_speed - collision_damage->v[2]) * damage_range / (collision_damage->v[3] - collision_damage->v[2]);
          damage.amount = PIN(damage.amount + minimum_damage, minimum_damage, maximum_damage);
          damage_compute_damage_owner_from_object_index(other_havok_component->get_object_index(), &damage.owner);
          damage.damage_owner_object_index = other_havok_component->get_object_index();
          damage.flags |= FLAG(8);
          damage.flags |= FLAG(0);
          damage.flags |= FLAG(16);
          bool full;
          damage.multiplier = havok_collision_damage_calculate_multiplier(damage.effect, object_index, damage.amount, &full);
          if (!full)
            damage.flags |= FLAG(15);
          else
            damage.flags &= ~FLAG(15);
          if (havok_collision_damage_index_is_valid(definition->material_effects.index) && havok_collision_damage_test_physics_flag(object->physics_flags, 7)) {
            real_vector3d direction = velocity;
            normalize3d_with_default(&direction, global_up3d);
            havok_collision_damage_material_effect_new(definition->material_effects.index, object_index, _effect_type_collision_damage,
                (e_sweetener_size)PIN((unsigned char)other_definition->sweetener_size, 0, 2), material_type, true, point, &direction,
                    &direction, &object->location, damage.amount);
          }
          if (!game_simulation::game_is_predicted()) {
            object_cause_damage(&damage, object_index, NONE, NONE, NONE, NULL);
            if (havok_collision_damage_object_is_type(definition, _object_type_biped) &&
                havok_collision_damage_object_is_type(other_definition, _object_type_vehicle))
            {
              c_motor_request request;
              request.setup_stun(_action_none);
              motor_system_submit(object_index, &request);
            }
            if (havok_collision_damage_test_physics_flag(object->damage_flags, 2) && fixed_turret) {
              for (long rigid_body_index = 0; rigid_body_index < other_havok_component->get_rigid_body_count(); rigid_body_index++) {
                c_havok_component::c_rigid_body *const &rigid_body = &other_havok_component->m_rigid_bodies[rigid_body_index];
                other_havok_component->set_rigid_body_linear_velocity(rigid_body_index, &rigid_body->m_pre_simulation_linear_velocity);
                other_havok_component->set_rigid_body_angular_velocity(rigid_body_index, &rigid_body->m_pre_simulation_angular_velocity);
              }
            }
            if (velocity_direction && !was_dead && havok_collision_damage_test_physics_flag(object->damage_flags, 2) &&
                TEST_BIT(1 << definition->object_type, _object_type_biped)) {
              real_vector3d direction = velocity;
              float magnitude = normalize3d(&direction);
              if (magnitude != 0.0f) {
                biped_datum *biped = (biped_datum *)havok_collision_damage_get_object_datum(object_index);
                real_vector3d impulse;
                scale_vector3d(velocity_direction, relative_speed, &impulse);
                float dot = dot_product3d(&direction, &impulse);
                if (dot < 0.0f)
                  point_from_line3d(&impulse, &direction, -dot, &impulse);
                limit3d(&impulse, magnitude);
                bool dead_mode = biped->biped.physics.get_mode() == 3;
                if (dead_mode) {
                  real_vector3d impulse_direction = impulse;
                  float impulse_magnitude = normalize3d(&impulse_direction);
                  if (impulse_magnitude != 0.0) {
                    point_from_line3d(global_zero_vector3d, &impulse_direction, impulse_magnitude + k_havok_collision_damage_death_acceleration, &impulse);
                    biped->unit.death_state = 4;
                    biped->biped.physics.get_mode_dead()->set_death_node_acceleration(0, &impulse);
                  }
                }
              }
            }
          }
          result = true;
        }
      }
    }
  }
  return result;
}
// @address 0x260e70 havok_collision_damage_apply_impact_to_hierarchy
extern "C" void havok_collision_damage_apply_impact_to_hierarchy(const s_collision_damage_definition *collision_damage, const
    s_collision_damage_definition *other_collision_damage,
    const real_point3d *position, long object_index, float speed, const real_vector3d *normal, const real_vector3d *velocity,
    long other_object_index, c_global_material_type material_type, float relative_impulse, bool maximum_impulse_valid, float maximum_impulse)
{
  int indices[513];
  indices[0] = 0;
  havok_collision_damage_collect_objects(object_index, indices);
  for (int i = 0; i < indices[0]; i++)
  {
    bool from_collision = havok_collision_damage_apply_impact_damage(collision_damage, other_collision_damage, position, indices[i + 1],
        speed, i == 0 ? normal : NULL, velocity, other_object_index, material_type, relative_impulse);
    if (maximum_impulse_valid)
      havok_collision_damage_apply_absolute_damage(collision_damage, position, indices[i + 1], speed, material_type, maximum_impulse,
          relative_impulse, from_collision, other_object_index);
  }
}

struct s_breakable_surfaces_event_material;

static __forceinline bool havok_collision_damage_is_disabled()
{
  bool result = false;
  if (g_havok_monitor_globals.initialize_time != NONE)
    result = g_havok_monitor_globals.initialize_time >= game_time_get() - 3 &&
        g_havok_monitor_globals.initialize_time <= game_time_get();
  return result;
}
static __forceinline bool havok_collision_damage_has_contact_points(long object_index)
{
  c_havok_contact_point_iterator iterator(object_index, false);
  return iterator.valid();
}
static __forceinline s_object_marker_globals *havok_collision_damage_get_object_marker_globals()
{
  return &g_object_marker_globals[g_registered_thread_index - 1];
}
static __forceinline void havok_collision_damage_object_marker_begin()
{
  s_object_marker_globals *globals = havok_collision_damage_get_object_marker_globals();
  globals->mark++;
  globals->active = true;
}
static __forceinline void havok_collision_damage_object_marker_end()
{
  havok_collision_damage_get_object_marker_globals()->active = false;
}
static __forceinline bool havok_collision_damage_object_unmarked(long object_index)
{
  s_object_marker_globals *globals = havok_collision_damage_get_object_marker_globals();
  long object_absolute_index = datum_index_to_absolute_index(object_index);
  return globals->marks[object_absolute_index] != globals->mark;
}
static __forceinline void havok_collision_damage_object_mark(long object_index)
{
  s_object_marker_globals *globals = havok_collision_damage_get_object_marker_globals();
  globals->marks[(unsigned short)object_index] = globals->mark;
}

static __forceinline bool havok_collision_damage_test_definition_flag(unsigned short flags)
{
  bool result = (flags >> 5) & 1;
  return result;
}

static __forceinline float havok_collision_damage_seconds_to_ticks(float seconds)
{
  return seconds * (float)game_time_globals->ticks_per_second;
}
static __forceinline long havok_collision_damage_round_ticks(float ticks)
{
  return (long)(ticks + k_havok_collision_damage_rounding);
}
static __forceinline bool havok_collision_damage_object_is_crate(long object_index)
{
  bool result;
  if (object_index != NONE && ((1 << havok_collision_damage_get_object_type(object_index)) & 0x1000))
    result = true;
  else
    result = false;
  return result;
}

static __forceinline void havok_collision_damage_set_physics_flag(unsigned long *flags, bool set)
{
  if (set)
    *flags |= FLAG(23);
  else
    *flags &= ~FLAG(23);
}

// @address 0x108628 havok_collision_damage_update_object
extern "C" void havok_collision_damage_update_object(long object_index, long havok_component_index)
{
  c_havok_component *havok_component_a = havok_collision_damage_get_havok_component(havok_component_index);
  object_datum *object = object_get(object_index);
  if (havok_component_has_active_entity(havok_component_a) && !havok_collision_damage_is_disabled() &&
      (!((1 << object->identifier_type) & 1) || object_accepted_local_physics_object_get(object_index) == NONE)) {
    bool environment_penetration = false;
    if (havok_collision_damage_has_contact_points(object_index)) {
      bool damage_keyframed = false;
      bool keyframed_penetration = false;
      bool delete_keyframed = false;
      s_breakable_surface_damage_cache breakable_surface_damage_cache_entries[16];
      long damage_data_breakable_surface_count = 0;
      s_havok_collision_damage_game_cache collision_damage_cache_entries[16];
      long game_collision_damage_cache_count = 0;
      s_object_definition *object_definition =
          (s_object_definition *)tag_get(0, object->definition_index);
      const real_vector3d *velocity_a = havok_component_a->get_rigid_body_pre_simulation_linear_velocity(0);
      float speed_a = magnitude3d(velocity_a);
      float mass_sum_a = 0.0f;
      for (long body_index = 0; body_index < havok_component_a->get_rigid_body_count(); body_index++)
        mass_sum_a += havok_component_a->get_rigid_body_mass(body_index);
      float maximum_environment_impulse = 0.0f;
      float maximum_object_impulse = 0.0f;
      float maximum_impulse = 0.0f;
      real_vector3d best_normal;
      real_point3d best_position;
      long best_object_index = NONE;
      c_global_material_type best_material_type = k_global_material_type_invalid;
      real_point3d maximum_impulse_position;
      bool maximum_impulse_valid = false;
      bool environment_marked = false;
      havok_collision_damage_object_marker_begin();
      for (c_havok_contact_point_iterator iterator(havok_component_a->get_object_index(), false); iterator.valid(); iterator.next()) {
        c_havok_contact_point *contact_point = iterator.get_contact_point();
        long other_object_index = contact_point->get_contact_point_object_index();
        if (((other_object_index == NONE && !environment_marked) ||
             (other_object_index != NONE && havok_collision_damage_object_unmarked(other_object_index)) ||
             TEST_BIT(contact_point->m_flags, 3)) &&
            (other_object_index == NONE || !havok_collision_damage_test_biped_state(other_object_index))) {
          long other_component_index = contact_point->m_other_havok_component_index;
          c_havok_component *havok_component_b = other_component_index != NONE ? havok_collision_damage_get_havok_component(other_component_index) : NULL;
          bool other_is_crate = havok_collision_damage_object_is_crate(other_object_index);
          bool fixed = havok_component_b == NULL;
          real_vector3d normal;
          float impulse;
          real_point3d position;
          bool apply_damage = true;
          if (other_object_index != NONE)
            havok_collision_damage_object_mark(other_object_index);
          else
            environment_marked = true;
          havok_component_a->calculate_impulse(contact_point->get_rigid_body_index_a(), contact_point->get_contact_point_object_index(),
              contact_point->get_rigid_body_index_b(), &impulse, &position, &normal);
          float relative_impulse = impulse * (havok_component_a->get_rigid_body_mass(contact_point->get_rigid_body_index_a()) / mass_sum_a);
          if (!havok_component_b || havok_component_b->get_rigid_body_is_fixed(contact_point->get_rigid_body_index_b()) ||
              (!other_is_crate && havok_component_b->get_rigid_body_is_keyframed(contact_point->get_rigid_body_index_b())))
            fixed = true;
          if (havok_component_b) {
            if (havok_component_b->get_rigid_body_is_keyframed(contact_point->get_rigid_body_index_b()) &&
                contact_point->m_distance < k_havok_collision_damage_keyframe_penetration)
              keyframed_penetration = true;
          } else if (contact_point->m_distance < k_havok_collision_damage_environment_penetration) {
            environment_penetration = true;
          }
          if (fixed)
            maximum_environment_impulse = MAX(maximum_environment_impulse, relative_impulse);
          if (other_object_index != NONE) {
            object_datum *other_object = object_get(other_object_index);
            s_object_definition *other_definition =
                (s_object_definition *)tag_get(0, other_object->definition_index);
            apply_damage = !TEST_BIT(other_definition->flags, 5);
          }
          if (!fixed && apply_damage && relative_impulse > maximum_object_impulse) {
            maximum_object_impulse = relative_impulse;
            best_object_index = other_object_index;
            best_position = position;
            best_material_type = contact_point->get_global_material_type_b();
            best_normal = normal;
          }
          if (apply_damage && relative_impulse > maximum_impulse) {
            maximum_impulse = relative_impulse;
            maximum_impulse_position = position;
            maximum_impulse_valid = true;
          }
          if (TEST_BIT(contact_point->m_flags, 3) && (unsigned long)damage_data_breakable_surface_count < 16 &&
              havok_collision_damage_apply_breakable_surface_damage(havok_component_a->get_object_index(), *iterator.get_reference(),
                  &breakable_surface_damage_cache_entries[damage_data_breakable_surface_count]))
            damage_data_breakable_surface_count++;
          if (fixed && !havok_collision_damage_test_definition_flag(object_definition->flags) && other_object_index != NONE &&
              relative_impulse >= 0.001f && (unsigned long)game_collision_damage_cache_count < 16)
          {
            collision_damage_cache_entries[game_collision_damage_cache_count].position = position;
            collision_damage_cache_entries[game_collision_damage_cache_count].object_index = other_object_index;
            collision_damage_cache_entries[game_collision_damage_cache_count].speed = 0.0f;
            collision_damage_cache_entries[game_collision_damage_cache_count].velocity = *global_zero_vector3d;
            collision_damage_cache_entries[game_collision_damage_cache_count].other_object_index = object_index;
            collision_damage_cache_entries[game_collision_damage_cache_count].impulse = relative_impulse;
            collision_damage_cache_entries[game_collision_damage_cache_count].material_type = contact_point->m_global_material_type_a;
            game_collision_damage_cache_count++;
          }
        }
      }
      havok_collision_damage_object_marker_end();
      object_datum *collision_object = (object_datum *)object;
      if (keyframed_penetration) {
        collision_object->keyframed_contact_ticks = MIN(collision_object->keyframed_contact_ticks + 1, 254);
        if (collision_object->keyframed_contact_ticks ==
            havok_collision_damage_round_ticks(havok_collision_damage_seconds_to_ticks(k_havok_collision_damage_keyframe_duration)))
          damage_keyframed = true;
        if (collision_object->keyframed_contact_ticks ==
            havok_collision_damage_round_ticks(havok_collision_damage_seconds_to_ticks(k_havok_collision_damage_rounding)))
          delete_keyframed = true;
      } else {
        collision_object->keyframed_contact_ticks = 0;
      }
      if (best_object_index != NONE) {
        object_datum *other_object = object_get(best_object_index);
        if (other_object->havok_component_index != NONE) {
          c_havok_component *other_component = havok_collision_damage_get_havok_component(other_object->havok_component_index);
          if (magnitude3d(other_component->get_rigid_body_pre_simulation_linear_velocity(0)) > speed_a) {
            s_damage_owner owner;
            damage_compute_damage_owner_from_object_index(other_component->get_object_index(), &owner);
            object_set_damage_owner(object_index, &owner, false);
          }
        }
      }
      s_collision_damage_definition collision_damage_a;
      s_collision_damage_definition collision_damage_b;
      if ((maximum_environment_impulse >= 0.001f || maximum_object_impulse >= 0.0f) && !havok_collision_damage_test_physics_flag(object->damage_flags, 2) &&
          object_definition->model.index != NONE &&
              ((s_model_definition *)tag_get(0, object_definition->model.index))
                      ->damage_info.count > 0 &&
          (best_object_index != NONE || maximum_impulse_valid)) {
        if (best_object_index != NONE) {
          long index = havok_component_a->get_object_index();
          if (havok_collision_damage_collision_damage_definition_get(index, &collision_damage_a) &&
              havok_collision_damage_collision_damage_definition_get(best_object_index, &collision_damage_b))
            havok_collision_damage_apply_impact_to_hierarchy(&collision_damage_a, &collision_damage_b, &best_position, index, speed_a,
                &best_normal, velocity_a, best_object_index, best_material_type, maximum_object_impulse, maximum_impulse_valid,
                maximum_impulse);
        } else {
          if (maximum_impulse_valid) {
            long index = havok_component_a->get_object_index();
            if (havok_collision_damage_collision_damage_definition_get(index, &collision_damage_a))
              havok_collision_damage_apply_absolute_to_hierarchy(&collision_damage_a, &maximum_impulse_position, index, speed_a,
                  best_material_type, maximum_impulse, maximum_object_impulse, NONE);
          }
        }
      }
      if (!game_simulation::game_is_predicted()) {
        for (long i = 0; i < damage_data_breakable_surface_count; i++)
          breakable_surface_damage_area_of_effect(
              (const s_breakable_surfaces_event_material *)&breakable_surface_damage_cache_entries[i].damage_data.location,
              &breakable_surface_damage_cache_entries[i].damage_data.position, 0.2f);
      }
      for (long i = 0; i < game_collision_damage_cache_count; i++) {
        s_havok_collision_damage_game_cache *entry = &collision_damage_cache_entries[i];
        if (havok_collision_damage_collision_damage_definition_get(entry->object_index, &collision_damage_b) &&
            havok_collision_damage_collision_damage_definition_get(entry->other_object_index, &collision_damage_a))
          havok_collision_damage_apply_impact_damage(&collision_damage_b, &collision_damage_a, &entry->position, entry->object_index,
              entry->speed, NULL, &entry->velocity, entry->other_object_index, entry->material_type, entry->impulse);
      }
      if (damage_keyframed && (collision_object->collision_damage_scale == 0 || (float)collision_object->collision_damage_scale *
          k_havok_collision_damage_scale < 1.0f)) {
        real_point3d center_of_mass;
        object_get_center_of_mass(object_index, &center_of_mass);
        havok_collision_damage_collision_damage_definition_get(object_index, &collision_damage_b);
        float speed = collision_damage_b.v[3] * k_havok_collision_damage_reconnect_duration;
        havok_collision_damage_apply_absolute_to_hierarchy(
            &collision_damage_b, &center_of_mass, object_index, speed, k_global_material_type_default, speed, 0.0f, NONE);
      }
      if (delete_keyframed && !game_simulation::game_is_predicted() &&
          (collision_object->collision_damage_scale == 0 || (float)collision_object->collision_damage_scale * k_havok_collision_damage_scale < 1.0f))
        object_delete(object_index);
    }
    if (!havok_collision_damage_disabled && !((1 << havok_collision_damage_get_object_type(object_index)) & 1)) {
      object_datum *collision_object = (object_datum *)object;
      if (environment_penetration)
        collision_object->environment_contact_ticks = PIN(collision_object->environment_contact_ticks + 1, 0, 32766);
      else
        collision_object->environment_contact_ticks = MAX(collision_object->environment_contact_ticks - 1, -32767);
      if (!havok_collision_damage_test_physics_flag(object->physics_flags, 23)) {
        if (collision_object->environment_contact_ticks >=
            (long)((float)game_time_globals->ticks_per_second * k_havok_collision_damage_contact_duration + k_havok_collision_damage_rounding))
        {
          havok_collision_damage_set_physics_flag(&object->physics_flags, true);
          havok_object_rebuild(object_index, 0);
        }
      } else {
        if (-collision_object->environment_contact_ticks >=
            (long)((float)game_time_globals->ticks_per_second * k_havok_collision_damage_reconnect_duration + k_havok_collision_damage_rounding))
        {
          havok_collision_damage_set_physics_flag(&object->physics_flags, false);
          havok_object_rebuild(object_index, 0);
        }
      }
    }
  }
}
