// objectslibxbox/creatures.obj: from the records 0xd3650 0xd3ce0 0xe0a50 0xe4f78 0x35d4f8 0x35d6a8 0x35d800 0x35dd08 0x35de30
// @flags /TP /O2 /Oc
#include "creatures/creature_datum.h"
#include "creatures/creature_definitions.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math_globals.h"
#include "cseries/cseries_wrapped.h"
#include "objects/object_types.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include "game/game_types.h"
#include <math.h>
#include "cseries/cseries.h"
#include <string.h>
#include <stdlib.h>
#include "objects/objects.h"
#include "objects/object_animation_manager_inline.h"
#include "sound/game_sound.h"
#include "ai/swarms.h"
#include "creatures/creatures.h"
#include "objects/damage_owner.h"
#include "objects/object_early_movers.h"
#include "physics/havok.h"
#include "scenario/scenario_kill_trigger_volumes.h"
#include "ai/actors.h"
#include "ai/ai_flocks.h"
#include "physics/character_physics_component.h"
#include "game/game_allegiance.h"
#include "physics/collisions.h"
#include "game/materials.h"
#include "objects/damage_data.h"
#include "objects/damage_effect_definitions.h"
#include "objects/damage_aftermath_types.h"
#include "physics/collision_test_flags.h"
#include "physics/collision_types.h"
#include "objects/damage.h"
#include "math/real_math.h"
#include "math/random_math_range.h"
#include "game/game_globals_inline.h"
#include "game/game_time_inline.h"

// @address 0xd3650 ?creature_control_clear@@YAXPAUs_creature_control_data@@@Z
// @address 0xd3ce0 ?creature_set_actively_controlled@@YAXJ_N@Z
// @address 0x35d4f8 ?creature_new@@YA_NJPAUobject_placement_data@@PA_N@Z

// match: 0x35d4f8: bool return preserves the shifted physics flag test.
static __forceinline bool creatures_character_physics_flag_test(unsigned long flags, long bit)
{
  return (flags >> bit) & 1;
}

void creature_control_clear(s_creature_control_data *data) {
 memset(data,0,sizeof(*data));
 data->action=138;
 data->animation=12;
 data->forward=*global_forward3d;
 data->up=*global_up3d;
}

void creature_set_actively_controlled(long object_index, bool enable)
{
  creature_datum *object = (creature_datum *)object_get_and_verify_type(object_index, NONE);
  bool hidden = TEST_BIT(object->object.damage_flags, 2);
  if (hidden) {
    enable = false;
  }
  if (enable) {
    object->creature.flags |= 1;
  }
  else {
    object->creature.flags &= ~1;
  }
}

bool creature_new(long object_index, object_placement_data *placement, bool *out_of_memory)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(object_index, NONE);
  creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
  bool success = true;
  creature->creature.team = -1;
  creature->creature.turning_amount = 0.0f;
  creature->creature.physics.initialize(object_index);
  creature->creature.m_flags.clear();
  creature->creature.form_lifetime = 0;
  creature->creature.death_delay_ticks = 0;
  creature_control_clear(&creature->creature.control);
  success = success && object_has_animation_manager(object_index);
  if (success) {
    if (placement->ai_state_type != -1) {
      creature->creature.link_type = placement->ai_state_type;
      success = success && ((unsigned char)::object_header_block_allocate(object_index, 0x174, placement->ai_state_size, placement->ai_state_alignment_bits));
      creature = (creature_datum *)object_get_and_verify_type(object_index, NONE);
    } else {
      creature->creature.link_type = -1;
    }
    if (success) {
      creature->creature.team = (short)placement->team;
      if (game_is_campaign() && (creature->creature.team == 0 || creature->creature.team == -1))
        creature->creature.team = definition->default_team;
      c_motor_program_state *motor = (c_motor_program_state *)((char *)creature + creature->creature.motor_program.offset);
      if (creatures_character_physics_flag_test(definition->physics.flags, 4))
        motor->set_program(object_index, _motor_program_creature_flying);
      else
        motor->set_program(object_index, _motor_program_creature_ground);
    } else {
      *out_of_memory = true;
    }
  }
  return success;
}

// @address 0xe4f78 ?creature_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool creature_compute_function_value(long creature_index, long function_name, long function_index, float *value, bool *active, bool *deterministic)
{
  bool result = true;
  float zero = k_real_zero;
  float one = k_real_one;
  float function_value = zero;
  if (function_name != _string_id_shooting) {
    result = false;
  }
  else if ((((creature_datum *)object_get_and_verify_type(creature_index, NONE))->creature.control.control_flags & 32) != 0) {
    function_value = one;
  } else {
    function_value = zero;
  }
  if (!result) return result;
  function_value = ((function_value > zero ? function_value : zero) < one)
    ? (function_value > zero ? function_value : zero) : one;
  *value = function_value;
  *active = (function_value > zero);
  return result;
}

// @address 0x35d6a8 creature_update_attached_damage
extern "C" void creature_update_attached_damage(long creature_index)
{
    bool infectable = false;
    creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
    creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
    creature_datum *parent = (creature_datum *)object_get_and_verify_type(creature->object.parent_object_index, NONE);
    long damage_effect_index = definition->attached_damage_effect.index;
    if (parent->object.shield_vitality > 0.0f && definition->attached_shield_damage_effect.index != -1)
        damage_effect_index = definition->attached_shield_damage_effect.index;
    swarm_can_attach_to_unit(creature->object.parent_object_index, &infectable);
    if (damage_effect_index != -1 && !infectable)
    {
        s_damage_data damage_data;
        damage_data_new(&damage_data, damage_effect_index);
        damage_compute_damage_owner_from_object_index(creature_index, &damage_data.owner);
        damage_data.position = creature->object.bounding_center;
        damage_data.origin = damage_data.position;
        object_cause_damage(&damage_data, creature->object.parent_object_index, -1, -1, -1, 0);
        const s_damage_effect_definition *damage_effect = TAG_GET('jpt!', const s_damage_effect_definition, damage_effect_index);
        if (damage_effect->sound.index != -1)
        {
            real_vector3d up;
            object_get_orientation(creature_index, 0, &up);
            object_impulse_sound_new(creature_index, damage_effect->sound.index, -1, &creature->object.bounding_center, &up, 1.0f);
        }
    }
}

// @address 0x35dd08 ?creature_accelerate@@YAXJPBTreal_vector3d@@@Z
void creature_accelerate(long object_index, union real_vector3d const * acceleration)
{
  creature_datum *object = (creature_datum *)object_get_and_verify_type(object_index, NONE);
  if ((bool)(_rotl(object->object.physics_flags, 28) & 1))
    object_in_early_mover_detach(object_index);
  ::object_set_at_rest(object_index, false);
  real_vector3d linear;
  object_get_velocities(object_index, &linear, 0);
  real_vector3d updated;
  add_vectors3d(&linear, acceleration, &updated);
  float x = updated.i;
  float y = updated.j;
  float z = updated.k;
  ::object_set_velocities_internal(object_index, &updated, 0, false);
  havok_object_set_velocities(object_index, &updated, 0, true);
  if (!(x * x + (z * z + y * y) <= 9.999999e-09f)) {
    ::object_set_at_rest(object_index, false);
    object_wake(object_index);
    object_set_requires_motion(object_index);
  }
}


// @address 0x35de30 ?creature_damage_aftermath_compute@@YAXJJPBUs_damage_data@@PAUs_damage_aftermath_result_data@@@Z
void creature_damage_aftermath_compute(long creature_index, long damage_flags, const s_damage_data *damage_data, s_damage_aftermath_result_data *result)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
  if (TEST_BIT(damage_flags, 0))
  {
    const creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
    float lower = definition->destroy_after_death_time_lower;
    float upper = definition->destroy_after_death_time_upper;
    if ((lower > upper ? lower : upper) > 0.0f)
      creature->creature.death_delay_ticks = (short)game_seconds_to_ticks_round(_real_random_range(tls_random_seed(), lower, upper));
    if (creature->creature.link_type != -1)
    {
      const s_damage_effect_definition *damage_effect = TAG_GET('jpt!', const s_damage_effect_definition, result->damage_definition_index);
      long damage_owner = result->damage_owner_object_index;
      short damage_type = damage_effect->damage.category;
      switch (((creature_datum *)object_get_and_verify_type(creature_index, NONE))->creature.link_type)
      {
      case 0:
        swarm_handle_creature_death(creature_index, damage_owner, damage_type);
        break;
      case 1:
        flock_handle_boid_death(creature_index);
        break;
      }
    }
  }
}



// @address 0xe0a50 ?creature_update@@YA_NJ@Z
bool creature_update(long creature_index)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
  bool result = false;
  if (creature->object.havok_component_index != -1)
  {
    if (creature->creature.m_flags.test(_creature_flag_actively_controlled))
    {
      result = true;
    }
    else
    {
      creature_control_clear(&creature->creature.control);
      creature->creature.control.forward = creature->object.forward;
      creature->creature.control.up = creature->object.up;
    }
    long parent_index = creature_index;
    long root_index = NONE;
    while (parent_index != NONE)
    {
      root_index = parent_index;
      parent_index = object_get(parent_index)->parent_object_index;
    }
    if (!(bool)TEST_BIT(((creature_datum *)object_get_and_verify_type(root_index, NONE))->object.physics_flags, 9) &&
        scenario_kill_trigger_volumes_test_object(creature_index, 0))
    {
      creature->object.damage_flags |= 64;
    }
    if (!(bool)TEST_BIT(creature->object.damage_flags, 2) && creature->creature.m_flags.test(_creature_flag_airborne) &&
        creature->creature.impact_delay_ticks <= 0 && creature->object.parent_object_index == -1)
    {
      creature_try_to_impact(creature_index);
    }
    if (creature->object.parent_object_index != -1)
    {
      if (--creature->creature.attached_damage_delay_ticks == 0)
      {
        creature_update_attached_damage(creature_index);
        creature->creature.attached_damage_delay_ticks = (signed char)game_seconds_to_ticks_round(0.2f);
      }
      result = true;
    }
    else
    {
      creature->creature.attached_damage_delay_ticks = -1;
    }
    if (creature->creature.impact_delay_ticks > 0)
      creature->creature.impact_delay_ticks--;
    if (creature->creature.death_delay_ticks != 0)
    {
      if (--creature->creature.death_delay_ticks <= 0)
        ::object_destroy_internal(creature_index, -1, -1, 0, 0);
      result = true;
    }
    else if (creature->creature.form_lifetime > 0)
    {
      if (--creature->creature.form_lifetime <= 0)
        ::object_destroy_internal(creature_index, -1, -1, 0, 0);
      result = true;
    }
    creature->creature.m_flags.m_storage &= ~0x40;
  }
  return result;
}






// @rdata 0x82022db8
static const float k_creatures_minimum_impulse = 0.4f;

// @address 0x35d800
bool creature_try_to_impact(long creature_index)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
  long impact_object_index = -1;
  real_point3d impact_point;
  long impact_node_index;
  long impact_region_index;
  long impact_material_index;
  s_location impact_location;
  real_vector3d impact_normal;
  real_vector3d velocity;
  float impulse = 0.0f;
  object_get_velocities(creature_index, &velocity, 0);
  real_vector3d test_vector;
  bool moving = true;
  float velocity_i = velocity.i;
  float scale = 0.0f;
  float velocity_j = velocity.j;
  float velocity_k = velocity.k;
  float speed = sqrtf(velocity_i * velocity_i + (velocity_k * velocity_k + velocity_j * velocity_j));
  if (speed > 0.0001f)
  {
    float radius = creature->object.bounding_radius + 0.1f;
    scale = radius / speed;
    test_vector.i = velocity.i * scale;
    test_vector.j = velocity.j * scale;
    test_vector.k = velocity.k * scale;
  }
  else
    moving = false;
  if (moving)
  {
    float closest_fraction = 3.402823466e+38f;
    long object_indices[16];
    long object_count = 0;
    if (creature->creature.link_type == 0)
    {
      swarm_creature_state *state = (swarm_creature_state *)object_header_block_get(creature, &creature->creature.creature_reference);
      if (state && state->swarm_index != -1)
        object_count = actor_find_props_in_sphere(
            state->swarm_index, &creature->object.bounding_center, creature->object.bounding_radius + 0.1f, object_indices, 16);
    }
    for (long object = 0; object < object_count; ++object)
    {
      long object_index = object_indices[object];
      s_object_header *header = object_header_get(object_index);
      if ((1 << header->object_type) & 0x1003)
      {
        unit_datum *unit = (unit_datum *)header->datum;
        if (!(bool)TEST_BIT(unit->object.damage_flags, 2) && game_team_is_enemy((e_game_team)creature->creature.team, unit->unit.team))
        {
          collision_result collision;
          s_collision_test_flags flags = { 0x1800, 7 };
          if (object_test_vector(object_index, flags, &creature->object.bounding_center, &test_vector, -1, -1, &collision) && collision.type == 4)
          {
            if (object_header_get(collision.object_index)->object_type == 0 && collision.fraction < closest_fraction)
            {
              impact_object_index = collision.object_index;
              impact_node_index = collision.node_index;
              impact_region_index = collision.region_index;
              impact_material_index = (short)collision.material_index;
              impact_point = collision.point;
              impact_normal = collision.plane.n;
              impact_location = collision.location;
              closest_fraction = collision.fraction;
            }
          }
        }
      }
    }
  }
  if (impact_object_index != -1)
  {
    creature_definition *definition = TAG_GET('crea', creature_definition, creature->object.definition_index);
    long damage_effect = definition->attached_damage_effect.index;
    creature_datum *unit = (creature_datum *)object_get_and_verify_type(impact_object_index, NONE);
    bool destroy = false;
    bool infectable = false;
    if (unit->object.shield_vitality > 0.0f)
    {
      destroy = TEST_BIT(definition->flags, 4);
      if (definition->attached_shield_damage_effect.index != -1)
        damage_effect = definition->attached_shield_damage_effect.index;
    }
    bool attach = swarm_can_attach_to_unit(impact_object_index, &infectable);
    if (damage_effect != -1 && attach && !infectable)
    {
      s_damage_data damage;
      damage_data_new(&damage, damage_effect);
      damage_compute_damage_owner_from_object_index(creature_index, &damage.owner);
      damage.location = impact_location;
      damage.origin = impact_point;
      damage.position = creature->object.bounding_center;
      object_cause_damage(&damage, impact_object_index, impact_node_index, impact_region_index, impact_material_index, &impact_normal);
    }
    if (destroy)
      object_destroy_internal(creature_index, -1, -1, 0, 0);
    else if (attach && (bool)TEST_BIT(definition->flags, 5))
    {
      if (!ai_swarm_creature_handle_attach(creature_index, impact_object_index))
      {
        real_vector3d forward;
        real_vector3d left;
        real_vector3d up;
        forward.i = -impact_normal.i;
        forward.j = -impact_normal.j;
        forward.k = -impact_normal.k;
        real_vector3d_build_axes_from_vectors(&forward, &creature->object.up, &forward, &left, &up);
        object_set_position_direct(creature_index, &impact_point, &forward, &up, &impact_location, false);
        object_attach_to_node_with_type(impact_object_index, creature_index, impact_node_index);
        creature->creature.attached_damage_delay_ticks = (signed char)(long)(0.2f * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
      }
    }
    else
    {
      float normal_speed = -dot_product3d(&velocity, &impact_normal);
      impulse = k_creatures_minimum_impulse > (1.0f + 0.7f) * normal_speed
        ? k_creatures_minimum_impulse : (1.0f + 0.7f) * normal_speed;
      real_vector3d reflected_velocity;
      point_from_line3d(&creature->object.linear_velocity, &impact_normal, impulse, &reflected_velocity);
      object_set_velocities(creature_index, &reflected_velocity, 0);
    }
  }
  return impact_object_index != -1;
}
