// blamlibxbox/game_engine_spawning.obj: from the records 0x27aea0 0x27b018 0x27b2b8 0x27b5a8 0x27b730 0x27b7c8 0x27b948 0x27b970
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "cseries/long_sort.h"
#include "units/weapon_addition_method_types.h"
#include "physics/havok_group_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_iterator.h"
// @address 0x27b5a8 ?game_engine_should_spawn_player@@YA_NJ@Z
// @address 0x27b730 ?game_engine_limited_lives_active@@YA_NXZ
// @address 0x27b948 game_engine_compare_spawn_scores
// @address 0x27b970 game_engine_get_spawn_object_location
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "game/player_types.h"
#include "game/player_traits.h"
#include "game/game_globals.h"
#include <stdlib.h>
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "physics/collision_types.h"
#include "memory/data_types.h"
#include "game/weapon_set_types.h"
#include "game/dynamic_spawn_influencer_collection_types.h"
#include "game/dynamic_spawn_influencer_types.h"
#include "cache/cache_files.h"
#include "physics/collisions.h"
#include "physics/havok.h"
#include "physics/havok_utilities.h"
#include "game/game_engine_default.h"
#include "game/game_engine_event_data.h"
#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_list.h"

#include "units/biped_definitions.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_starting_profile.h"
#include "game/game_engine_current_entry.h"
#include "memory/datum_index.h"
#include "memory/datum_storage_access.h"
#include "physics/character_physics_shape_types.h"

extern "C" {
}



class c_spawning_object_iterator : public c_object_iterator_base
{
public:
    void begin(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};




static __forceinline bool game_engine_spawning_object_can_be_used_by_team(s_multiplayer_object_properties *multiplayer, long engine_team)
{
  long team_designator = multiplayer->team_designator;
  bool team_match;
  if (team_designator == 8 || engine_team == 8 || team_designator == engine_team)
    team_match = true;
  else
    team_match = false;
  return team_match;
}

static __forceinline long game_engine_spawning_engine_team_for_player(unsigned long player_index)
{
  const c_game_engine *engine = current_game_engine();
  long team;
  if (engine)
    team = engine->get_player_team_designator(player_index);
  else
    team = 8;
  return team;
}

static __forceinline c_game_variant *game_engine_spawning_get_variant(game_options *options)
{
  // match: 0x27b730: retain the original volatile mode read across game_engine_has_teams.
  return static_cast<volatile game_options *>(options)->game_mode == 2 ? &options->game_variant : 0;
}

static __forceinline long game_engine_spawning_seconds_to_ticks(long seconds)
{
    return seconds * tls_game_time_globals()->ticks_per_second;
}

static __forceinline void game_engine_spawning_initialize_event(long type,long index,s_game_engine_event_data *event) {
 event->type=type; event->event=index;
 event->identifier=-1; event->flags=-1; event->cause_player_index=-1; event->cause_team_index=-1;
 event->effect_player_index=-1; event->effect_team_index=-1; event->data=0; event->data_word=-1;
}


static __forceinline const phys_shape *game_engine_spawning_unit_shape(const biped_definition *definition)
{
  if (definition->physics.pills.count)
    return (const phys_shape *)&((const s_character_physics_pill *)definition->physics.pills.address)->shape;
  else if (!(definition->physics.flags & 1))
#ifdef HALO3_64BIT
    return (const phys_shape *)&((const s_character_physics_dead_sphere *)definition->physics.spheres.address)->transform;
#else
    return (const phys_shape *)&((const s_character_physics_spheres *)definition->physics.spheres.address)->crouching_shape;
#endif
  else
#ifdef HALO3_64BIT
    return (const phys_shape *)&((const s_character_physics_dead_sphere *)definition->physics.spheres.address)->shape;
#else
    return (const phys_shape *)&((const s_character_physics_spheres *)definition->physics.spheres.address)->standing_shape;
#endif
}

// @address 0x27aea0 ?game_engine_unit_can_spawn_at_location_unobstructed@@YA_NJPBTreal_point3d@@_N@Z
bool game_engine_unit_can_spawn_at_location_unobstructed(long unit_definition_index, const real_point3d *location, bool)
{
  bool result = false;
  if (unit_definition_index != -1) {
    if (tls_game_globals()->options.game_mode == 1) {
      result = true;
    } else {
      s_location scenario_location;
      real_point3d test_point;
      scenario_location_from_point(&scenario_location, location);
      biped_definition *definition = (biped_definition *)global_tag_instances[datum_index_to_absolute_index(unit_definition_index)].definition;
      const phys_shape *shape = game_engine_spawning_unit_shape(definition);

      real_matrix4x3 matrix;
      matrix4x3_identity(&matrix);
      matrix.position = *location;
      matrix.position.z += 0.2f;
      if (!havok_world_shape_collides(shape, -1, &matrix, _havok_group_biped)) {
        test_point = *location;
        test_point.z += definition->physics.height_standing * 0.5f;
        s_collision_test_flags flags = k_placement_test_collision_flags;
        if (!collision_test_point(flags, &test_point, -1, -1, 0))
          result = true;
      }
    }
  }
  return result;
}


// @address 0x27b018 ?game_engine_find_best_player_respawn_object_index@@YAJJ_N@Z
long game_engine_find_best_player_respawn_object_index(long player_index, bool)
{
  real_point3d origin;
  long indices[256];
  long order[256];
  float scores[256];
  s_dynamic_spawn_influencer_collection collection;

  long best = -1;
  player_datum *player = (player_datum *)tls_player_data()->data + (player_index & 0xffff);
  bool flag = ((player->flags >> 3) & 1) != 0;
  unsigned char first_spawn = (unsigned char)flag;
  long match_flags = first_spawn ? 4 : 2;
  long initial_count;
  {
  long count = 256;
  game_engine_find_available_spawn_location_objects(player_index, 64, match_flags, indices, &count);
  initial_count = count;
  }
  bool do_second_pass = true;
  do_second_pass = do_second_pass && first_spawn != 0;
  long total;
  if (do_second_pass) {
    long first_count = initial_count;
    long *remaining_indices = &indices[first_count];
    long remaining_count = 256 - first_count;
    game_engine_find_available_spawn_location_objects(player_index, 64, 2, remaining_indices, &remaining_count);
    total = remaining_count + first_count;
  } else {
    total = initial_count;
  }
  if (total == 0) {
    long count = 256;
    game_engine_find_available_spawn_location_objects(player_index, 64, 0, indices, &count);
    total = count;
  }
  if (total >= 1u) {
    if (total != 1u) {
      game_engine_spawn_influencers_build_collection_for_player(player_index, &collection);
      for (long i = 0; i < total; i++) {
        long object_index = indices[i];
        s_object_header *header = object_header_get(object_index);
        s_multiplayer_object_properties *multiplayer =
            (s_multiplayer_object_properties *)((char *)header->datum + header->datum->multiplayer_properties.offset);
        const bool &unbounded = multiplayer->object_type == _multiplayer_object_type_spawn_point && (_rotl(multiplayer->flags.get_unsafe(), 31) & 1) != 0;
        object_get_origin(object_index, &origin);
        scores[i] = game_engine_spawn_influencer_compute_score(&origin, flag, unbounded,
          multiplayer->initial_spawn_weight, multiplayer->spawn_weight_adjustment, &collection);
        order[i] = i;
      }
    } else {
      best = indices[0];
    }
  }
  if (best == -1 && total > 0) {
    if (total >= 2u)
      templated_sort<long>::qsort(order, total, game_engine_compare_spawn_scores, scores);
    long count;
    player_find_player_character_unit_and_variant_info(player_index, &count, 0);
    long unit_definition_index = count;
    long i = 0;
    while (true) {
      if (i >= total)
        break;
      long object_index = indices[order[i]];
      bool valid;
      if (unit_definition_index != -1 && object_index != -1) {
        game_engine_get_spawn_object_location(object_index, unit_definition_index, &origin);
        valid = game_engine_unit_can_spawn_at_location_unobstructed(unit_definition_index, &origin, false);
      } else {
        valid = false;
      }
      if (valid)
        best = object_index;
      ++i;
      if (best == -1)
        continue;
      break;
    }
  }
  if (best != -1)
    player->respawn_counter = player->respawn_counter + 1;
  return best;
}


static __forceinline void game_engine_spawning_add_starting_weapon(long unit_index, long definition_index)
{
  if (definition_index != -1)
  {
    object_placement_data placement;
    object_placement_data_new(&placement, definition_index, -1, 0);
    placement.model_variant_index = 0;
    long weapon_index = object_new(&placement);
    if (weapon_index != -1)
    {
      if (!unit_has_weapon_definition_index(unit_index, object_get(weapon_index)->definition_index))
        unit_add_weapon_to_inventory(unit_index, weapon_index, _weapon_addition_method_silent);
      else
        object_delete(weapon_index);
    }
  }
}

// @address 0x27b2b8 ?game_engine_add_starting_equipment@@YAXJ@Z
void game_engine_add_starting_equipment(long unit_index)
{
  unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
  unsigned char maximum_grenade_counts[4];
  unsigned char initial_grenade_counts[4];
  memset(initial_grenade_counts, 0, sizeof(initial_grenade_counts));
  memset(maximum_grenade_counts, 0, sizeof(maximum_grenade_counts));
  long multiplayer_globals_index = global_game_globals->multiplayer_globals_index;
  s_multiplayer_globals_definition *multiplayer_globals = multiplayer_globals_index != -1
    ? (s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(multiplayer_globals_index)].definition : 0;
  if (multiplayer_globals && multiplayer_globals->runtime.count > 0)
  {
    s_multiplayer_runtime_globals_definition *runtime = (s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
    maximum_grenade_counts[0] = (unsigned char)runtime->maximum_fragmentation_grenades;
    maximum_grenade_counts[1] = (unsigned char)runtime->maximum_plasma_grenades;
  }
  unit_datum *player_unit = (unit_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
  long player_index = -1;
  if (player_unit)
    player_index = player_unit->unit.player_index;
  player_datum *player = &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)];
  const c_player_trait_weapons *weapons = player->traits.get_weapons_traits();
  long weapon_definition_indices[2];
  const long &primary_weapon_index = weapons->m_initial_primary_weapon;
  weapon_definition_indices[0] = game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(
    (short)primary_weapon_index, _weapon_set_primary);
  if (weapons->m_initial_primary_weapon != -1 && weapon_definition_indices[0] == -1)
    weapon_definition_indices[0] = game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(-1, _weapon_set_primary);
  const long &secondary_weapon_index = weapons->m_initial_secondary_weapon;
  weapon_definition_indices[1] = game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(
    (short)secondary_weapon_index, _weapon_set_secondary);
  if (weapons->m_initial_secondary_weapon != -1 && weapon_definition_indices[1] == -1)
    weapon_definition_indices[1] = game_engine_weapon_item_definition_index_from_absolute_weapons_selection_block_index(-1, _weapon_set_secondary);
  for (unsigned long index = 0; index < 2; index++)
    game_engine_spawning_add_starting_weapon(unit_index, weapon_definition_indices[index]);
  scenario *map = global_scenario;
  s_scenario_starting_profile *profile = map->starting_profiles.count > 0 ? (s_scenario_starting_profile *)map->starting_profiles.address : 0;
  if (profile && weapons->m_grenade_count != 2)
  {
    for (long index = 0; index < 4; index++)
      initial_grenade_counts[index] = profile->grenade_counts[index];
  }
  for (long index = 0; index < 4; index++)
    unit->unit.grenade_counts[index] = initial_grenade_counts[index] > maximum_grenade_counts[index]
      ? maximum_grenade_counts[index] : initial_grenade_counts[index];
  for (long index = 0; index < 4; index++)
  {
    if (unit->unit.grenade_counts[index] > 0)
    {
      unit->unit.current_grenade_index = (signed char)index;
      unit->unit.desired_grenade_index = (signed char)index;
      break;
    }
  }
  simulation_action_object_update(unit_index, 0x4000000);
}

bool game_engine_should_spawn_player(long player_index) {
 bool should_spawn=true;
 if(current_game_engine_entry()) {
  player_datum *player=datum_get((player_datum *)tls_player_data()->data,player_index);
  if(!game_engine_player_is_playing(player_index)) should_spawn=false;
  else if(!game_engine_in_round()) should_spawn=false;
  else {
   if(player->respawn_timer>0) {
    if (player->respawn_timer == game_engine_spawning_seconds_to_ticks(3) ||
        player->respawn_timer == game_engine_spawning_seconds_to_ticks(2) ||
        player->respawn_timer == game_engine_spawning_seconds_to_ticks(1))
    {
     s_game_engine_event_data event;
     game_engine_spawning_initialize_event(0,20,&event);
     event.cause_player_index=player_index; event.flags=player_index;
     game_engine_send_event(&event);
    } else if(player->respawn_timer==1) {
     s_game_engine_event_data event;
     game_engine_spawning_initialize_event(0,21,&event);
     event.cause_player_index=player_index; event.flags=player_index;
     game_engine_send_event(&event);
    }
    game_engine_player_set_respawn_timer(player_index,player->respawn_timer-1);
   }
   should_spawn=player->respawn_timer<=0;
  }
 }
 return should_spawn;
}

bool game_engine_limited_lives_active(void)
{
  game_options *options = &tls_game_globals()->options;
  bool player_lives_limited = game_engine_spawning_get_variant(options)->lives_per_round > 0;
  bool team_lives_limited = false;
  if (game_engine_has_teams()) {
    team_lives_limited = game_engine_spawning_get_variant(options)->team_lives_per_round > 0;
  }
  return team_lives_limited | player_lives_limited;
}

// @address 0x27b7c8 ?game_engine_find_available_spawn_location_objects@@YAXJKKPAJ0@Z
void game_engine_find_available_spawn_location_objects(
    long player_index, unsigned long team_player_index, unsigned long match_flags, long *object_indices, long *count)
{
  long capacity = *count;
  long player_unit[2];
  long found = 0;
  ::player_find_player_character_unit_and_variant_info(player_index, (long *)(player_unit), (long *)(0));
  c_spawning_object_iterator iterator;
  iterator.begin(64, 0, match_flags);
  long *output = object_indices;
  if (capacity > 0)
  {
    do
    {
      if (!iterator.next())
        break;
      long object_index = iterator.get_index();
      s_data_array *headers = tls_object_header_data();
      if (object_index != -1 && (object_index & 0xffff) < headers->first_unallocated)
      {
        object_header_datum *header = (object_header_datum *)(headers->data + headers->size * (object_index & 0xffff));
        if (header->identifier != 0 && header->identifier == (object_index >> 16) && (1 << header->object_type) != 0)
        {
          object_datum *datum = header->datum;
          if (datum && datum->multiplayer_properties.offset != -1)
          {
            s_multiplayer_object_properties *multiplayer =
                (s_multiplayer_object_properties *)((char *)datum + datum->multiplayer_properties.offset);
            if (multiplayer)
            {
              long engine_team = game_engine_spawning_engine_team_for_player(player_index);
              if (game_engine_spawning_object_can_be_used_by_team(multiplayer, engine_team))
              {
                *output++ = object_index;
                found++;
              }
            }
          }
        }
      }
    } while (found < capacity);
  }
  *count = found;
}

bool game_engine_compare_spawn_scores(long index_a, long index_b, const void *context)
{
  const float *weights = (const float *)context;
  return weights[index_a] < weights[index_b];
}

extern "C" void game_engine_get_spawn_object_location(long object_index, long unit_definition_index, real_point3d *location)
{
  biped_definition *definition = (biped_definition *)global_tag_instances[datum_index_to_absolute_index(unit_definition_index)].definition;
  real_vector3d forward;
  real_vector3d up;
  multiplayer_get_spawn_object_position_and_facing_vectors(object_index, location, &forward, &up);
  if (definition->physics.flags & 1) {
    float offset = definition->physics.radius;
    location->x += up.i * offset;
    location->y += up.j * offset;
    location->z += up.k * offset;
  }
}
