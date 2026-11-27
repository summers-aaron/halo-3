// ailibxbox/event_handling.obj: from the records 0xfaf30 0xfb278 0xfb6e8 0x51b9d8 0x51bb48 0x51bc58 0x51bd70 0x51bf28
// @flags /TP /O2 /Oc
#include "ai/ai_information_data.h"
#include "memory/tls_data_arrays.h"
#include "ai/clump_importance_types.h"
#include "ai/ai_player_types.h"
#include "math/real_math_types.h"
#include <string.h>
#include "math/real_math.h"
#include "game/game_time_types.h"
#include "game/game_time_conversion.h"
#include "memory/data_types.h"
#include "objects/object_header_access.h"
#include "objects/object_header.h"
#include "objects/object_reference_types.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "units/motor_enums.h"
#include "ai/clump_datum.h"
#include "ai/clump_actor_cursor_types.h"
#include "structures/cluster_bit_vectors.h"
#include "ai/prop_datum.h"
#include "cseries/cseries.h"
// @address 0x51bc58 ?actor_handle_speech@@YA_NJJPAUai_information_data@@@Z
// @address 0xfb6e8 ?clump_handle_sound@@YAXJJJ_NPAUs_game_cluster_bit_vectors@@PAUai_sound_data@@@Z
#include <math.h>
#include "ai/actor_datum.h"

#include "ai/actors.h"
#include "objects/objects.h"
#include "ai/actor_perception.h"
#include "ai/actor_stimulus.h"
#include "ai/ai_flocks.h"
#include "ai/ai_player.h"
#include "ai/clump.h"
#include "ai/event_handling.h"
#include "ai/ai_dialogue.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "structures/structure_audibility.h"
#include "game/game_allegiance.h"
#include "motor/action_system.h"
#include "ai/prop_stimulus_types.h"
#include "tag_files/tag_types.h"
#include "scenario/scenario_tag.h"
#include "memory/data_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "ai/props.h"


#include "ai/prop_ref_datum.h"
#include "ai/actor_queries.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "ai/event_handling_sound_types.h"
#include "memory/tls_globals.h"
#include "ai/actor_globals.h"


#include "ai/prop_view.h"
#include "ai/prop_view_inline.h"
#include "ai/clump_iterator.h"
#include "scenario/cluster_reference_helpers.h"

static __forceinline void event_handling_stimulus_environmental_noise(long index, long object) {
 if (datum_get<actor_datum>(actor_data, index)->attention_level <= 0) {
  datum_get<actor_datum>(actor_data, index)->attention_ticks = (short)game_seconds_to_ticks_round(4.0f);
  actor_datum *actor = datum_get<actor_datum>(actor_data, index);
  actor->attention_direction.type = 7;
  actor->attention_direction.object_index = object;
  datum_get<actor_datum>(actor_data, index)->attention_level = 0;
 }
}

// @address 0xfb278 actor_handle_sound
extern "C" void actor_handle_sound(long index, long speaker, long object, long prop, ai_sound_data *sound) {
 if (!actor_is_deaf(index) && datum_get<actor_datum>(actor_data,
     index)->position.location.cluster_reference.bsp_index != NONE) {
  switch (sound->type) {
  case 0: actor_handle_speech(index, speaker, (ai_information_data *)sound->information); break;
  case 1: actor_stimulus_heard_shooting(index, prop, speaker); break;
  case 2: break;
  case 3: ::prop_perceive(index, object, 3, _prop_stimulus_none, 0, -1, -1, -1); ::actor_initiate_combat(index); break;
  case 4: event_handling_stimulus_environmental_noise(index, object); break;
  }
 }
}

// @address 0x51bb48 ?test_actor_audibility@@YA_NJJPAUs_game_cluster_bit_vectors@@PATreal_point3d@@PAUs_location@@PAUai_sound_data@@@Z
bool test_actor_audibility(long actor_index, long source_actor_index,
    s_game_cluster_bit_vectors *cluster_bits, real_point3d *point, s_location *location, ai_sound_data *sound)
{
    actor_datum *listener_actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    do
    {
        if (actor_index == source_actor_index || !listener_actor->active
            || listener_actor->position.location.cluster_reference.bsp_index == NONE)
            break;
        if (cluster_bits)
        {
            s_cluster_reference listener_cluster_reference = listener_actor->position.location.cluster_reference;
            long listener_cluster_index = listener_cluster_reference.cluster_index;
            if (!cluster_bits->bit_vectors[listener_cluster_reference.bsp_index].test(listener_cluster_index))
                break;
        }
        actor_position_data listener_sense_position;
        if (!actor_perception_find_sense_position(actor_index, point, &listener_sense_position))
            break;
        unsigned short sound_volume = sound->volume;
        result = actor_audibility_at_point(actor_index, &listener_sense_position, point, location,
            sound_volume, 1.0f, 0) >= 1;
    } while (false);
    return result;
}

bool actor_handle_speech(long actor_index, long other_actor_index, ai_information_data *information) {
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool handled = false;
 if (other_actor_index != NONE) {
  actor_datum *other = datum_get<actor_datum>(actor_data, other_actor_index);
  if (!game_team_is_enemy((e_game_team)actor->team, (e_game_team)other->team)) {
   switch (information->type) {
    case 1:
     handled = false;
     break;
    case 2:
     handled = true;
     break;
    case 3:
     if (information->object_index != NONE) prop_perceive(actor_index, information->object_index, NONE,
         _prop_stimulus_none, 0, NONE, NONE, NONE);
     handled = true;
     break;
    case 4:
     handled = true;
     break;
    case 5:
     actor_stimulus_prop_just_killed(actor_index, information->prop_index, other_actor_index);
     handled = true;
     break;
   }
  }
 }
 return handled;
}

void clump_handle_sound(long clump_index, long sound_index, long object_index, bool test_audibility,
    s_game_cluster_bit_vectors *bits, ai_sound_data *sound) {
 clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
 unit_datum *object = (unit_datum *)object_get(object_index);
 ai_information_data *information = &sound->information_data;
 bool handle = true;
 if (sound->type == 0) {
  switch (information->type) {
  case 1:
   game_allegiance_incident((e_game_team)information->allegiance_incident.team,
       (e_game_team)information->allegiance_incident.other_team, information->allegiance_incident.incident);
   handle = false;
   break;
  }
 }
 if (handle) {
  real_point3d origin;
  object_get_origin(object_index, &origin);
  s_location *where = &object->object.location;
  if (object->object.parent_object_index != -1)
   where = &((unit_datum *)object_get(inl_object_get_ultimate_parent(object_index)))->object.location;
  long prop_index = clump_get_clump_prop(clump_index, object_index);
  if (prop_index != -1) {
   prop_datum *prop = datum_get<prop_datum>(prop_data, prop_index);
   if (!prop->ignore) {
    long ref_index = prop->first;
    while (ref_index != -1) {
     prop_ref_datum *ref = datum_get<prop_ref_datum>(prop_ref_data, ref_index);
     if (datum_get<actor_datum>(actor_data, ref->actor_index)->active) {
      if (!test_audibility || test_actor_audibility(ref->actor_index, sound_index, bits, &origin, where, sound))
       actor_handle_sound(ref->actor_index, sound_index, object_index, ref_index, sound);
     }
     ref_index = ref->prop_next_ref_index;
    }
   }
  } else {
   clump_actor_iterator iterator;
   clump_actor_iterator_new(&iterator, clump);
   actor_datum *actor;
   while ((actor = clump_actor_iterator_next(&iterator)) != 0) {
    if (actor->active) {
     if (!test_audibility || test_actor_audibility(iterator.actor_index, sound_index, bits, &origin, where, sound))
      actor_handle_sound(iterator.actor_index, sound_index, object_index, -1, sound);
    }
   }
  }
 }
}


// @address 0x51b9d8 ?calculate_pas@@YAPAUs_game_cluster_bit_vectors@@Us_cluster_reference@@@Z
s_game_cluster_bit_vectors *calculate_pas(s_cluster_reference cluster_reference)
{
 ai_globals_type *ai_globals = ::ai_globals;
 if (cluster_reference.value() != ai_globals->pas_cluster_reference.value()) {
  audibility_table table;
  actor_perception_initialize_audibility_table(&table, global_scenario_index, global_zone_set_index);
  unsigned long active_mask = g_active_structure_bsp_mask;
  const ::scenario *scenario = global_scenario;
  memset(&ai_globals->pas, 0, sizeof(ai_globals->pas));
  for (long bsp_index = 0; bsp_index < scenario->structure_bsps.count; bsp_index++) {
   if ((1 << bsp_index) & active_mask) {
    s_cluster_reference other_cluster;
    cluster_reference_set(&other_cluster, bsp_index, 0);
    structure_bsp *bsp =
     (structure_bsp *)global_tag_instances[
         datum_index_to_absolute_index(
             ((scenario_structure_bsp_reference *)scenario->structure_bsps.address)[bsp_index].structure_bsp.index)].definition;
    for (; other_cluster.cluster_index < bsp->clusters.count; other_cluster.cluster_index++) {
     if (!structure_get_cluster_ai_deafening((s_structure_cluster_mapping *)&table, cluster_reference, other_cluster) &&
         !(structure_audibility_get_distance((s_structure_cluster_mapping *)&table,
             (unsigned short)cluster_reference.value(), (unsigned short)other_cluster.value()) >= 40.0f))
         {
      s_cluster_reference hit_cluster;
      cluster_reference_set(&hit_cluster, bsp_index, other_cluster.cluster_index);
      ai_globals->pas.bit_vectors[hit_cluster.bsp_index].set(hit_cluster.cluster_index, true);
     }
    }
   }
  }
  ai_globals->pas_cluster_reference = cluster_reference;
 }
 return &ai_globals->pas;
}


static __forceinline bool event_handling_location_valid(const s_location *location)
{
 return location->cluster_reference.bsp_index != -1;
}
static __forceinline void event_handling_data_iterator_new(s_data_iterator *iterator, void *data)
{
 iterator->absolute_index = -1;
 iterator->index = -1;
 iterator->data = (s_data_array *)data;
}
static __forceinline void event_handling_clump_iterator_new(clump_actor_cursor *iterator)
{
 if (ai_globals->enabled)
  event_handling_data_iterator_new(&iterator->iterator, clump_data);
}
static __forceinline clump_datum *event_handling_clump_iterator_next(clump_actor_cursor *iterator)
{
 clump_datum *result = 0;
 if (ai_globals->enabled) {
  iterator->datum = (clump_datum *)::data_iterator_next(&iterator->iterator);
  if (iterator->datum)
   result = (clump_datum *)iterator->datum;
  iterator->index = iterator->iterator.index;
 }
 return result;
}
// @address 0xfaf30 ?handle_sound@@YAXJJF_NPAUai_sound_data@@@Z
void handle_sound(long actor_index, long object_index, short clump_count, bool test_audibility, ai_sound_data *sound)
{
 unit_datum *object = (unit_datum *)object_get(object_index);
 long team = -1;
 actor_datum *actor = 0;
 float range;
 s_game_cluster_bit_vectors *pas = 0;
 real_point3d origin;
 object_get_origin(object_index, &origin);
 s_location *location = &object->object.location;
 if (object->object.parent_object_index != -1)
  location = &((unit_datum *)object_get(inl_object_get_ultimate_parent(object_index)))->object.location;
 if (actor_index != -1) {
  actor = datum_get<actor_datum>(actor_data, actor_index);
  team = actor->team;
 } else if (object->object.object_type == 0) {
  unit_datum *unit = object;
  if (unit->unit.player_index != -1)
   team = datum_get<player_datum>(player_data, unit->unit.player_index)->configuration.host.team;
 }
 if (sound->type == 1 && object->object.object_type == 0) {
  unit_datum *unit = object;
  if (unit->unit.player_index != -1) {
   ai_player_state *state = ai_player_state_get(unit->unit.player_index);
   if (state)
    state->reset_a = 0;
  }
 }
 if (test_audibility && event_handling_location_valid(location))
  pas = calculate_pas(location->cluster_reference);
 if (sound->type == 0)
  range = 30.0f;
 else
  range = 40.0f;
 if (clump_count == 0 && actor && actor->clump_index != -1) {
  clump_handle_sound(actor->clump_index, actor_index, object_index, test_audibility, pas, sound);
 } else if (clump_count >= 1) {
  clump_actor_cursor iterator;
  event_handling_clump_iterator_new(&iterator);
  clump_datum *clump;
  while ((clump = event_handling_clump_iterator_next(&iterator)) != 0) {
   if (actor_index != -1 && clump_count < 2 && team != clump->team && game_team_is_enemy((e_game_team)team,
       (e_game_team)clump->team))
    continue;
   if (distance3d(&clump->position, &origin) < clump->radius + range)
    clump_handle_sound(iterator.index, actor_index, object_index, test_audibility, pas, sound);
  }
 }
 if (sound->type == 3) {
  flocks_handle_weapon_detonation(&origin);
 } else if (sound->type == 1) {
  if (((1 << object->object.object_type) & 0x1003) && object->unit.player_index != -1)
   ai_globals->last_player_sound_time = game_time_get();
 }
}


void handle_intuition(long actor_index, long object_index, long prop_index, short clump_count)
{
 prop_perceive(actor_index, object_index, 1, _prop_stimulus_none, 0, prop_index, NONE, NONE);
 ai_sound_data sound;
 sound.type = 0;
 sound.volume = 1;
 ai_information_data *information = (ai_information_data *)sound.information;
 information->type = 3;
 information->object_index = object_index;
 handle_sound(actor_index, object_index, 1, false, &sound);
}


static __forceinline void event_handling_stimulus_bumped(prop_ref_datum *prop_ref)
{
 prop_ref->maximum_awareness = 3;
 prop_ref->awareness = 3;
}

// @address 0x51bd70 ?actor_handle_bump@@YAXJJ@Z
void actor_handle_bump(long actor_index, long object_index)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 unit_datum *object = (unit_datum *)object_get(object_index);
 bool enemy = true;
 if (action_executing(object_index, _action_melee_player))
  enemy = false;
 else if (object->unit.actor_index != NONE) {
  actor_datum *other = datum_get<actor_datum>(actor_data, object->unit.actor_index);
  enemy = game_team_is_enemy((e_game_team)actor->team, (e_game_team)other->team);
 }
 if (enemy) {
  prop_perceive(actor_index, object_index, 5, _prop_stimulus_bump, (short)game_seconds_to_ticks_round(1.0f), NONE,
      NONE, NONE);
  long prop_ref_index = prop_ref_get_by_object_index(actor_index, object_index, false, _clump_importance_none);
  if (prop_ref_index != NONE) {
   prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
   prop_view *view = prop_view_get(prop_ref);
   if (view)
    view->ineffability = 3;
   event_handling_stimulus_bumped(prop_ref);
  }
 }
 if (actor->root_vehicle_index == NONE && object->unit.player_index != NONE)
  ai_dialogue_trigger_actor_scream(actor_index, 20);
}
