// ailibxbox/actor_type_guardian.obj: from the records 0x571390 0x571448 0x5714c0 0x5715d0 0x571638 0x571740 0x571890 0x5718d8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "memory/data.h"
#include "ai/firing_position_ref_inline.h"
#include "ai/firing_position_ref.h"
#include "math/real_math_types.h"
#include "units/motor_enums.h"
#include "units/action_request.h"
#include "ai/behavior.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "motor/action_system.h"
#include "ai/ai.h"
#include "ai/props.h"
#include "tag_files/tag_groups.h"
#include "ai/actor_type_guardian.h"
#include "ai/actor_datum.h"
#include "ai/actor_firing_position.h"
#include "ai/firing_position_definition.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_state.h"
#include "ai/character_definitions.h"
#include "memory/tls_globals.h"
#include "ai/actor_moving.h"
#include "ai/path_state.h"
// @address 0x571448 ?guardian_is_surging@@YA_NJ@Z
// @address 0x5715d0 ?guardian_isolation_surge_get_impulse@@YAFJPAVc_behavior_state@@@Z
// @address 0x571638 ?guardian_danger_surge_get_impulse@@YAFJPAVc_behavior_state@@@Z


static const float actor_type_guardian_default_proximity(2.0f);

struct s_guardian_surge_behavior_state : public c_behavior_state {
  bool performed;
};

// @address 0x571390 ?guardian_check_surge_timer@@YA_NJ@Z
bool guardian_check_surge_timer(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  int property_handle = actor->character_index;
  character_guardian_properties *property = 0;

  while (property_handle != -1) {
    character_definition *definition = TAG_GET('char', character_definition, property_handle);
    if (definition->guardian_properties.count > 0) {
      property = (character_guardian_properties *)definition->guardian_properties.address;
      break;
    }
    property_handle = definition->parent_character.index;
  }

  bool result = false;
  if (property == 0 || actor->child_test_times[12] == -1) {
    result = true;
  } else {
    float elapsed = (float)(long long)(tls_game_time_globals()->time - actor->child_test_times[12]) * tls_game_time_globals()->tick_length;
    if (elapsed > property->surge_delay_time)
      result = true;
  }
  return result;
}

bool guardian_is_surging(long actor_index)
{
  int unit_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->unit_index;
  return action_executing(unit_index, _action_surge) || action_executing(unit_index, _action_phase);
}

// @address 0x5714c0 ?guardian_proximity_surge_get_impulse@@YAFJPAVc_behavior_state@@@Z
inline character_guardian_properties *actor_type_guardian_proximity_properties_get(int handle)
{
  while (handle != -1) {
    character_definition *definition = TAG_GET('char', character_definition, handle);
    if (definition->guardian_properties.count > 0)
      return (character_guardian_properties *)definition->guardian_properties.address;
    handle = definition->parent_character.index;
  }
  return 0;
}
short guardian_proximity_surge_get_impulse(long actor_index, c_behavior_state *state)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  short result = -1;
  int prop_index = actor->target_prop_index;
  if (prop_index != -1 && guardian_check_surge_timer(actor_index) && !guardian_is_surging(actor_index)) {
    prop_ref_datum *prop = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
    character_guardian_properties *properties = actor_type_guardian_proximity_properties_get(actor->character_index);
    float proximity = properties && properties->proximity_surge_distance > 0.0f ? properties->proximity_surge_distance : actor_type_guardian_default_proximity;
    if (prop->distance < proximity && prop->awareness >= 2)
      result = 154;
  }
  return result;
}

short guardian_isolation_surge_get_impulse(long guardian_actor_index, c_behavior_state *)
{
    short result = -1;
    actor_datum *p = datum_get<actor_datum>(tls_actor_data(), guardian_actor_index);
    if (p->isolation_level > 2 && !guardian_is_surging(guardian_actor_index))
        result = 154;
    return result;
}

short guardian_danger_surge_get_impulse(long actor_index, c_behavior_state *)
{
    bool danger;
    bool vehicle_danger;
    bool projectile_danger;
    short result;
    result = -1;
    if (guardian_check_surge_timer(actor_index) && !guardian_is_surging(actor_index) &&
        actor_assess_danger(actor_index, 0.3f, &danger, &vehicle_danger, &projectile_danger, 0, 0)) {
        if (vehicle_danger) {
            result = 154;
        }
        else if (danger || projectile_danger) {
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            actor->path.valid = false;
            actor->path.obstacle_object_index = NONE;
            actor->path.result_value = 0;
            actor->path_result_time = 0;
            actor->path_result_count = 0;
            actor->path_state = 0;
            actor->destination_orders.clear();
            if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE))
                actor_discard_firing_position(actor_index, actor->firing_position_reference, 0);
        }
    }
    return result;
}

// @address 0x571740 ?behavior_surge_activate@@YA_NJPAVc_behavior_state@@@Z
static __forceinline character_guardian_properties *character_definition_guardian_properties_get(long index)
{
  character_guardian_properties *result = 0;
  while (index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, index);
    if (definition->guardian_properties.count > 0) {
      result = (character_guardian_properties *)definition->guardian_properties.address;
      break;
    }
    index = definition->parent_character.index;
  }
  return result;
}
static __forceinline void actor_type_guardian_path_clear(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  actor->path.valid = false;
  actor->path.obstacle_object_index = NONE;
  actor->path.result_value = 0;
  actor->path_result_time = 0;
  actor->path_result_count = 0;
  actor->path_state = 0;
  actor->destination_orders.clear();
}
bool behavior_surge_activate(long actor_index, c_behavior_state *state)
{
  bool result = false;
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  character_guardian_properties *properties = character_definition_guardian_properties_get(actor->character_index);
  action_request request;
  memset(&request, 0, sizeof(request));
  request.surge.has_vector = 0;
  request.action = _action_surge;
  request.surge.value = properties ? properties->surge_time : 1.0f;
  if (action_submit(actor->unit_index, &request)) {
    union {
      firing_position_ref reference;
      int word;
    } no_firing_position;
    actor_type_guardian_path_clear(actor_index);
    no_firing_position.reference = FP_REF_NONE;
    actor_set_current_firing_position(actor_index, no_firing_position.word);
    actor->flags &= ~0x2000000ul;
    static_cast<s_guardian_surge_behavior_state *>(state)->performed = false;
    actor->child_test_times[12] = tls_game_time_globals()->time;
    result = true;
  }
  return result;
}

// @address 0x571890 ?behavior_surge_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_surge_next(long surge_actor, c_behavior_state *surge_state, c_behavior_state *)
{
  short result;
  result = -2;
  if ((static_cast<s_guardian_surge_behavior_state *>(surge_state)->performed != '\0') && (guardian_is_surging(surge_actor) == 0)) {
    result = -1;
  }
  return result;
}

// @address 0x5718d8 ?behavior_surge_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_surge_perform(long actor_index, c_behavior_state *behavior_state)
{
  s_guardian_surge_behavior_state *state = (s_guardian_surge_behavior_state *)behavior_state;
  actor_datum *a = datum_get<actor_datum>(tls_actor_data(), actor_index);
  bool result = true;
  if (a->uncovering && !state->performed) {
    float self_radius = 5.0f;
    float target_radius = 3.0f;
    character_guardian_properties *properties = character_definition_guardian_properties_get(a->character_index);
    if (properties) {
      if (properties->current_position_distance > 0.0f)
        self_radius = properties->current_position_distance;
      if (properties->target_position_distance > 0.0f)
        target_radius = properties->target_position_distance;
    }
    firing_position_evaluation_context c;
    memset(&c, 0, sizeof(c));
    c.type = 0;
    c.ignore_area_path = true;
    c.influences[c.influence_count].reject = true;
    c.influences[c.influence_count].position = a->position.body;
    c.influences[c.influence_count].radius = self_radius;
    c.influence_count++;
    if (a->target_prop_index != -1) {
      prop_state *p = (prop_state *)prop_state_get_by_index(a->target_prop_index);
      c.influences[c.influence_count].reject = true;
      c.influences[c.influence_count].position = p->position;
      c.influences[c.influence_count].radius = target_radius;
      c.influence_count++;
    }
    firing_position position;
    long index;
    bool found;
    firing_position_ref ref;
    {
      c_path_state path;
      ref = actor_select_firing_position(actor_index, &c, &position, &index,
                         (path_state *)path.m_data, &found);
    }
    if (!fp_ref_equals(ref, FP_REF_NONE)) {
      actor_change_firing_position(actor_index, ref, &position, index, 0, false);
      a->surge_destination = position.definition->point;
      a->flags |= 0x2000000;
    }
    state->performed = true;
  }
  return result;
}

// @address 0x42d298 ?behavior_surge_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_surge_get_desire(long, const s_behavior_input *, const s_behavior_arguments *)
{
    return 0;
}

// @address 0x5bec98 ?behavior_surge_control@@YAXJPAVc_behavior_state@@@Z
void behavior_surge_control(long, c_behavior_state *)
{
}

// @address 0x7c5760 ?surge_behavior@@3VPrimitive_behavior@@A
Primitive_behavior surge_behavior = Primitive_behavior(154, -1, behavior_surge_get_desire, behavior_surge_next,
    behavior_surge_activate, 0, -1, behavior_surge_perform, behavior_surge_control);

// @address 0x7c5724 ?guardian_proximity_surge_impulse@@3VBehavior_impulse@@A
Behavior_impulse guardian_proximity_surge_impulse = Behavior_impulse(155, -1,
    guardian_proximity_surge_get_impulse);

// @address 0x7c574c ?guardian_danger_surge_impulse@@3VBehavior_impulse@@A
Behavior_impulse guardian_danger_surge_impulse = Behavior_impulse(156, -1, guardian_danger_surge_get_impulse);

// @address 0x7c5738 ?guardian_isolation_surge_impulse@@3VBehavior_impulse@@A
Behavior_impulse guardian_isolation_surge_impulse = Behavior_impulse(157, -1,
    guardian_isolation_surge_get_impulse);
