// ailibxbox/actors.obj: from the records 0xa8bf0 0xad7f0 0xadb80 0xadd38 0xae510 0xb00a8 0xb16f8 0xb3780 0xb4560 0xb4838 0xb7eb8 0xbbee0 0xbf208 0xcb738 0xcb8a8 0x11ba70 0x11e510 0x1d7550 0x4fd620 0x4fd728 0x4fd748 0x4fdf70 0x4fe198 0x4fe3f8 0x4fe5d8 0x4fe958 0x4febd0 0x4ff018 0x4ff158 0x4ff238 0x5001c8 0x5002b8 0x500580 0x5006f0 0x500860 0x5008b8 0x500ab8 0x500c58 0x500d78 0x501020 0x501138 0x5015f0 0x5016d0 0x501738 0x5018a0 0x501c08 0x501e30 0x502128 0x5025c8 0x502650 0x502798 0x502d80 0x502e70 0x502f20 0x503010 0x503060 0x503138 0x503288 0x5035b8 0x503700 0x503800 0x503918 0x503a60 0x503b98 0x503d08 0x504330 0x5044d8 0x504c30 0x504cd0 0x504ee8 0x505078 0x505120 0x505198 0x5051f0 0x505260 0x5052d0 0x5054d0 0x505670 0x505778 0x505818 0x505838 0x5058d8 0x5059b8 0x54eb88 0x5695a8
#include "ai/firing_position_ref_inline.h"
#include "game/game_time_inline.h"
#include "game/game_time_inlines.h"
#include "memory/tls_globals.h"
#include "ai/ai_player_tls.h"
#include "game/players_globals_types.h"
#include "ai/ai_tls_data.h"
#include "game/players_global_data.h"
#include "cache/restricted_memory_region_members.h"
#include "ai/squad_data.h"
#include "ai/ai_information_data.h"
#include "ai/character_behavior_properties.h"
#include "ai/character_weapon_properties.h"
#include "ai/actor_nearby_iterator_types.h"
#include "ai/actor_type_flood_types.h"
#include "objects/object_lifecycle.h"
#include "units/weapon_addition_method_types.h"
#include "ai/clump_importance_types.h"
#include "ai/ai_player_types.h"
#include "units/unit_seat.h"
#include "ai/firing_position_ref.h"
#include "ai/actors.h"
#include "ai/actor_types.h"
#include "ai/squads.h"
#include "scenario/scenario.h"
#include "ai/actor_firing_position.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_groups.h"
#include "ai/sector_ref.h"
#include "game/game_types.h"
#include "units/unit_passenger_iterator.h"
#include "units/vehicle_types.h"
#include "units/motor_enums.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"
#include "ai/squad_types.h"
#include "ai/actor_globals.h"
#include "ai/clump_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/recent_obstacle.h"
#include "game/game_time_types.h"
#include "game/game_globals.h"
#include "cache/restricted_memory_regions.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "units/unit_control.h"
#include "scenario/scenario_objects.h"
#include "objects/object_types.h"
#include "objects/object_reference_types.h"
#include "ai/pureform_types.h"
#include "ai/behavior_stimulus_types.h"
#include "ai/prop_stimulus_types.h"
#include "ai/behavior_info.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/actor_looking.h"
#include "ai/actor_moving.h"
#include "ai/actor_perception.h"
#include "ai/actor_stimulus.h"
#include "ai/ai.h"
#include "ai/ai_actions.h"
#include "ai/ai_vehicles.h"
#include "ai/ai_vehicle_queries.h"
#include "ai/behavior.h"
#include "ai/cl_engine.h"
#include "ai/event_handling.h"
#include "ai/joint_behavior.h"
#include "ai/objectives.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "ai/swarms.h"
#include "creatures/creatures.h"
#include "items/weapons.h"
#include "motor/motor_system.h"
#include "objects/damage.h"
#include "objects/object_early_movers.h"
#include "physics/havok_shapes.h"
#include "physics/water_physics.h"
#include "units/vehicles.h"
#include "ai/actor_type_pureform.h"
#include "ai/clump.h"
#include "ai/actor_moving_objects.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/styles.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "units/biped_types.h"
#include "units/vehicle_datum.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "ai/ai_player.h"
#include "memory/hashtable.h"
#include "units/bipeds.h"
#include "objects/object_placement.h"
#include "game/game_allegiance.h"
#include "ai/task_actor_iterator_types.h"
#include "ai/actor_array_types.h"
#include "units/unit_dialogue_sound_permutation_types.h"
#include "ai/unit_dialogue_types.h"
#include "ai/objective_storage.h"
#include "ai/objective_definitions.h"
#include "ai/swarm_creature_iterator_types.h"
#include "ai/squad_array_types.h"
#include "ai/prop_ref_array_types.h"
#include "ai/ai_direction_mode_types.h"
#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"
#include "game/cheats.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cseries/cseries.h"
#include "math/real_constant_pool.h"
#include "ai/joint_behavior_types.h"
#include "ai/interaction_target.h"
#include "items/weapon_definitions.h"
#include "ai/actor_senses.h"
#include "units/unit_estimate_position.h"
#include "ai/prop_view.h"
#include "structures/cluster_bit_vectors.h"
#include "ai/ai_globals_definition.h"
#include "ai/actor_iterator.h"
#include "creatures/creature_datum.h"
#include "ai/character_voice_properties.h"
#include "ai/actor_firing_position_hash.h"
#include "ai/squad_leader.h"
#include "ai/ai_reference_frame.h"
#include "ai/actor_prop_ref_iterator.h"
#include "cache/restricted_memory.h"
#include "ai/actor_allocators.h"
#include "scenario/scenario_tag.h"
#include "ai/squad_definitions.h"
#include "ai/actor_squad.h"
#include "ai/squad_area_iterator.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"
#include "game/player_types.h"
#include "physics/water_physics_types.h"
#include "ai/firing_position_ref_hash.h"
#include "memory/hashtable_reset.h"
#include "simulation/simulation_globals.h"
#include "simulation/simulation_world_types.h"
#include "ai/command_script_data.h"
#include "game/game_globals_storage.h"
#include "items/equipment_definitions.h"
#include "math/random_math_range.h"
#include "units/unit_dialogue.h"
#include "ai/event_handling_sound_types.h"
#include "ai/dialogue_event.h"
#include "ai/ai_dialogue.h"
#include "items/projectile_definitions.h"
#include "ai/actor_emotion.h"
#include "ai/actor_behavior_state.h"
#include "units/action_request.h"
#include "units/motor_program_state.h"
#include "scenario/scenario_light_types.h"
#include "scenario/scenario_map_variant_types.h"
#include "scenario/scenario_object_reference_frame.h"
#include "ai/activities.h"
#include "ai/clump_iterator.h"
#include "ai/objective_access.h"
#include "objects/object_type_access.h"
// @address 0x11df840 g_actor_firing_position_hash
__declspec(thread) s_hash_table *g_actor_firing_position_hash;
// @address 0x11df80c g_ai_reference_frame_data
__declspec(thread) s_attachment_state *g_ai_reference_frame_data;
// @address 0x11df65c actor_data
__declspec(thread) s_data_array *actor_data;
// @flags /TP /O2 /J /Oc
// @address 0xa8bf0 ?actor_iterator_next@@YAPAUactor_datum@@PAUactor_iterator@@@Z
// @address 0x5052d0 ?actor_nearby_actors_iterator_next@@YAPAUactor_datum@@PAUactor_nearby_actors_iterator@@@Z
// @address 0xadb80 ?actor_runtime_within_structure_bsp@@YA_NJ@Z
// @address 0xae510 ?actor_get_weapon@@YAJJ@Z
// @address 0xb00a8 actor_is_blind
// @address 0xb16f8 actor_is_deaf
// @address 0xb4560 ?actor_has_ranged_weapon@@YA_NJ@Z
// @address 0xb7eb8 ?actor_destination_tolerance@@YAMJ@Z
// @address 0xbf208 ?actor_unit_control_lower_weapon@@YAXJ_N@Z
// @address 0xcb738 ?actor_get_standing@@YAFJ@Z
// @address 0xcb8a8 ?actor_get_rank@@YAFJ@Z
// @address 0x1d7550 ?__tls_set_g_actor_data_allocator@@YAXPAX@Z
// @address 0x504c30 ?actor_unit_control_boost@@YAXJ_N@Z
// @address 0x505198 ?actor_set_covered@@YAXJ_N@Z
// @address 0x505818 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_actor_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x500c58 ?actor_handle_deleted_actor@@YAXJJ@Z
// @address 0x500d78 ?actor_handle_deleted_object@@YAXJJ@Z
// @address 0x503700 ?actor_broken_update@@YAXJ@Z
// @address 0x503a60 ?actor_cover_update@@YAXJ@Z
// @address 0xb4838 ?actor_is_giant@@YA_NPAUactor_datum@@@Z
// @address 0x4febd0 ?actor_select_character_voice@@YAXJJ@Z
// @address 0x4fd728 ?__tls_set_g_actor_firing_position_hash_allocator@@YAXPAX@Z
// @address 0x5016d0 ?actor_get_gravity_scale@@YAMJ@Z
// @address 0x5018a0 ?actor_flush_prop@@YAXJJ@Z
// @address 0x501c08 ?actor_flush_position_indices@@YAXJ_N@Z
// @address 0x501e30 ?actor_flush_structure_indices@@YAXJ@Z
// @address 0x54eb88 ?flush_object@c_ai_direction@@QAAXJ@Z
// @address 0x505778 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_actor_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x505838 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_reference_frame_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x5006f0 ?actor_set_active@@YA_NJ_N@Z
// @address 0x5059b8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_actor_firing_position_hash_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x500580 ?actor_set_team@@YAXJF@Z
// @address 0x502f20 ?actor_set_active_camo@@YA_NJ_N@Z




/* c_ai_direction (0x14): a direction toward an object (type 7) or a point; flushing a deleted object clears it */



struct s_character_variant { long tag; short variant; s_tag_block voices; };
typedef char s_character_variant_size[(sizeof(s_character_variant) == 0x14) ? 1 : -1];
struct actors3_hash_table {
  char name[32];
  unsigned long count;
  long entry_count;
  long entry_size;
  unsigned long (*hash)(void *);
  unsigned char (*equal)(void *, void *);
  c_allocation_base *volatile allocator;
};

static __forceinline void actors_direction_flush_prop(c_ai_direction *dir, long prop_index)
{
  if (dir->m_type == 1 && dir->m_object_index == prop_index)
    dir->clear();
}


/* objects: object_header_data (tls), 16-byte headers */

/* tags */


/* squads: squad_data (tls), 176 bytes each */
struct s_variant_reference;

/* character definitions read by the actor functions */


static __forceinline long unit_get_current_weapon(long unit_index)
{
  biped_datum *unit = (biped_datum *)object_get(unit_index);
  long result = -1;
  if (unit->unit.current_weapon_index != -1)
    result = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
  return result;
}

// the two pathfinding timeslices of one actor
static __forceinline float timeslice_game_ticks_to_seconds(float ticks)
{
  return ticks * TLS_GAME_TIME_GLOBALS->tick_length;
}


static __forceinline bool actors_actor_is_active(actor_datum *actor) { return actor->active; }


static __forceinline clump_datum *actors_clump_iterator_next(actor_nearby_actors_iterator::clump_iterator *iterator)
{
  clump_datum *clump = 0;
  if (ai_globals->enabled) {
    iterator->datum = (clump_datum *)::data_iterator_next(&iterator->iterator);
    if (iterator->datum != 0)
      clump = iterator->datum;
    iterator->index = iterator->iterator.index;
  }
  return clump;
}
static __forceinline bool actors_location_valid(const s_location *location)
{
  return (unsigned char)location->cluster_reference.bsp_index != 255;
}
static __forceinline void actors_data_iterator_new(s_data_iterator *iterator, s_data_array *data)
{
  iterator->absolute_index = -1;
  iterator->index = -1;
  iterator->data = data;
}
static __forceinline void actors_clump_iterator_new(actor_nearby_actors_iterator::clump_iterator *iterator)
{
  if (ai_globals->enabled)
    actors_data_iterator_new(&iterator->iterator, clump_data);
}
static __forceinline float actors_ai_hint_get_max_jump_down_height(short height)
{
  float result = 0.0f;
  s_game_globals *globals = global_game_globals;
  if (height >= 0 && height < 6 && globals && globals->ai_globals.count() > 0)
    result = globals->ai_globals[0].max_jump_down_heights[height];
  return result;
}


actor_datum *actor_iterator_next(actor_iterator *iterator)
{
  actor_datum *actor = 0;
  if (ai_globals->enabled) {
    while (iterator->next()) {
      if (!iterator->active_only || actors_actor_is_active(iterator->m_datum)) {
        actor = iterator->m_datum;
        break;
      }
    }
    iterator->index = iterator->get_index();
  }
  return actor;
}

long actor_get_weapon(long player_index)
{
  actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
  long weapon_index = -1;
  if (player->controlling_other_unit && player->controlled_unit_index != -1) {
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player->controlled_unit_index)].datum;
    weapon_index = unit_get_current_weapon(player->controlled_unit_index);
  }
  if (weapon_index == -1 && player->unit_index != -1) {
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player->unit_index)].datum;
    weapon_index = unit_get_current_weapon(player->unit_index);
  }
  return weapon_index;
}

bool actor_is_blind(unsigned short unit_index)
{
  actor_datum *actor = &((actor_datum *)actor_data->data)[unit_index];
  bool result = actor->blind || !actor->active || actor->behavior_mode == 1;
  if (!result && actor->squad_index != -1) {
    squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
    bool squad_result = squad->flags & 1;
    if (!squad_result && ::squad_valid_task((squad_datum *)(const s_variant_reference *)squad))
      squad_result = _rotl(*(unsigned short *)::squad_get_task(squad), 26) & 1;
    result = squad_result;
  }
  return result;
}

unsigned char actor_is_deaf(unsigned short player_index)
{
  actor_datum *player = &((actor_datum *)actor_data->data)[player_index];
  bool skip = player->deaf || !player->active;
  unsigned char result = skip;
  if (!skip) {
    long index = player->squad_index;
    if (index != -1) {
      squad_datum *squad = datum_get<squad_datum>(squad_data, index);
      unsigned char bit = _rotl(squad->flags, 31) & 1;
      if (!bit && ::squad_valid_task((squad_datum *)(const s_variant_reference *)squad)) {
        bit = _rotl(*(unsigned short *)::squad_get_task(squad), 25) & 1;
      }
      result = bit;
    }
  }
  return result;
}

bool actor_in_combat(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = true;
  if (!actor->active)
    result = false;
  else if (actor->situation.best_prop_class <= 3)
    result = false;
  else if (actor->ticks_since_combat > game_seconds_integer_to_ticks(20))
    result = false;
  else if (actor_executing(actor_index, 82))
    result = false;
  return result;
}

bool actor_has_ranged_weapon(long player_index)
{
  actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
  long weapon_index = actor_get_weapon(player_index);
  bool result = false;
  if (weapon_index != -1)
    result = !(TAG_GET('weap', const weapon_definition, object_get(weapon_index)->definition)->flags & 0x01000000);
  if (result && player->unit_index != -1 && (object_get(player->unit_index)->flags15 & 2))
    result = false;
  return result;
}

static __forceinline bool actors_object_attached(object_datum *object) { return (object->flags & FLAG(26)) != 0; }

bool actor_is_giant(actor_datum *actor)
{
  bool result = false;
  if (actor->unit_index != -1) {
    if (object_get_type(actor->unit_index) == 12)
      return true;
    long object_index = actor->unit_index;
    do {
      object_datum *object = object_get(object_index);
      if (!actors_object_attached(object))
        break;
      object_index = object->parent_object_index;
    } while (object_index != -1);
    if (object_index != actor->unit_index)
      result = object_get_type(object_index) == 12;
  }
  return result;
}

/* An actor's effective value: its own when set, otherwise its character entry's (for the held object's
   definition, or its own) or its fallback definition's; unset values give 0.5 or 0.3, set ones at least 0.2. */
const float k_default_value = 0.3f;
const float k_held_default_value = 0.5f;

float actor_destination_tolerance(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  float value = actor->destination_orders.f24;
  if (value == k_real_zero) {
    object_datum *object = (object_datum *)object_get(actor->unit_index);
    if (actor->root_vehicle_index != -1) {
      object_datum *held = (object_datum *)object_get(actor->root_vehicle_index);
      character_vehicle_properties *entry = character_vehicle_properties_get(actor_index, held->definition);
      if (entry)
        value = entry->steering_distance;
      if (value <= k_real_zero)
        return k_held_default_value;
    } else {
      character_vehicle_properties *entry;
      if (actor->use_object && (entry = character_vehicle_properties_get(actor_index, object->definition)) != 0) {
        value = entry->steering_distance;
        if (value <= k_real_zero)
          return k_default_value;
      } else {
        character_movement_properties *fallback = character_definition_movement_properties_get(actor->character_index);
        if (fallback)
          value = fallback->destination_tolerance;
        if (value <= k_real_zero)
          return k_default_value;
      }
    }
  }
  float result = 0.2f;
  if (value > result)
    result = value;
  return result;
}

void actor_unit_control_crouch(long player_index, bool value)
{
  actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
  if (value)
    player->flags |= FLAG(2);
  else
    player->flags &= ~FLAG(2);
  if (value)
    player->control_flags |= FLAG(0);
  else
    player->control_flags &= ~FLAG(0);
}

void actor_unit_control_lower_weapon(long player_index, bool value)
{
  actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
  if (value)
    player->flags |= FLAG(8);
  else
    player->flags &= ~FLAG(8);
  if (value)
    player->control_flags |= FLAG(14);
  else
    player->control_flags &= ~FLAG(14);
}

short actor_get_rank(long player_index)
{
  short result = 0;
  character_general_properties *definition
    = character_definition_general_properties_get(datum_get<actor_datum>(actor_data, player_index)->character_index);
  if (definition)
    result = definition->rank;
  return result;
}

// match: 0x11ba70: without __restrict the attack_vector j/k loads sink below the stack-copy stw/lfs/fmuls of i
bool actor_attacking_target(long actor_index, real_vector3d *__restrict attack_vector)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = false;
  if (inl_target_get_prop_class(actor) > 7 && actor_executing(actor_index, 25) && actor->current_fire_target_type > 0) {
    *attack_vector = actor->attack_vector;
    result = magnitude_squared3d(attack_vector) > 0.0f;
  }
  return result;
}

void actor_estimate_position(long object_index, short ticks, const real_point3d *origin, const real_vector3d *vector1,
    const real_vector3d *vector2, const real_vector3d *vector3, real_point3d *result)
{
  object_datum *object = object_get(object_index);
  if ((1 << object->object_type) & 4099)
    unit_estimate_position(object_index, ticks, origin, vector1, vector2, vector3, result);
  else
    *result = object->bounding_center;
}

void __tls_set_g_actor_data_allocator(void *object)
{
  data_set_new_base_address(&player_data, (s_data_array *)object);
}

bool actor_set_bring_forward(long unit_index, bool bring_forward, float value, float seconds)
{
  bool result = false;
  for (short index = 0; index < 8; index++) {
    if (ai_globals->mission_critical[index].unit_index == unit_index) {
      if (bring_forward) {
        short ticks = (short)(seconds > 0.0f ? game_seconds_to_ticks_round(seconds) : -1);
        if (ticks == -1 ||
            (ai_globals->mission_critical[index].bring_forward_ticks != -1 &&
                ai_globals->mission_critical[index].bring_forward_ticks < ticks))
          ai_globals->mission_critical[index].bring_forward_ticks = ticks;
        ai_globals->mission_critical[index].bring_forward_value = value;
      }
      if (bring_forward)
        ai_globals->mission_critical[index].flags |= FLAG(1);
      else
        ai_globals->mission_critical[index].flags &= ~FLAG(1);
      result = true;
      break;
    }
  }
  return result;
}

void actor_attach_unit(long actor_index, long unit_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  object_datum *object = object_get(unit_index);
  biped_datum *unit = (biped_datum *)object;
  long attached_actor = unit->unit.actor_index;
  if (attached_actor != actor_index)
  {
    if (attached_actor != -1)
      ::actor_delete(attached_actor, false);
    if (actor->unit_index != -1)
      actor_detach_from_unit(actor_index);
    actor->unit_index = unit_index;
    unit->unit.actor_index = actor_index;
    squad_datum *squad = 0;
    if (actor->squad_index != -1)
    {
      squad = datum_get<squad_datum>(squad_data, actor->squad_index);
      squad->strength_weight -= actor->weight;
    }
    actor->weight = object->maximum_body_vitality;
    ::unit_set_actively_controlled(unit_index, true);
    if (squad)
      squad->strength_weight = actor->weight + squad->strength_weight;
  }
}

/* swarms: g_swarm_data (tls); each creature links to the next */

static __forceinline void actors_swarm_set_team(long swarm_index, short team)
{
  long object_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
  while (object_index != -1) {
    creature_datum *creature = (creature_datum *)object_get(object_index);
    swarm_creature_state *link = 0;
    if (creature->creature.link_type == 0)
      link = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
    if (link == 0)
      break;
    creature->creature.team = team;
    object_index = link->next_swarm_creature_index;
  }
}

void actor_set_team(long actor_index, short team)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  unsigned char is_swarm = actor->swarm;
  short old_team = actor->team;
  actor->team = team;
  if (is_swarm) {
    actors_swarm_set_team(actor->swarm_index, team);
  } else if (actor->unit_index != -1) {
    biped_datum *unit = (biped_datum *)object_get(actor->unit_index);
    unit->unit.team = (e_game_team)team;
  }
  if (actor->attached_vehicle_index != -1) {
    if (old_team != team) {
      bool new_team_ally = (team == _game_team_player || game_team_is_ally((e_game_team)team, _game_team_player));
      bool old_team_ally = (old_team == _game_team_player || game_team_is_ally((e_game_team)old_team, _game_team_player));
      if (new_team_ally != old_team_ally)
        ai_vehicle_handle_allegiance_change(actor_index);
    }
  }
}


static __forceinline void actors_clump_activate(long clump_index)
{
  clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
  clump->active = true;
  clump->activation_value = 0.05f;
}

// 0x5006f0
bool actor_set_active(long actor_index, bool active)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = true;
  if (actor->active != active)
  {
    if (active)
    {
      if (actor_runtime_within_structure_bsp(actor_index))
      {
        actor->active = true;
        if (actor->activation_ticks == -1)
          actor->activation_ticks = game_time_get();
        ai_globals->active_actor_count++;
        if (actor->clump_index != -1)
        {
          clump_datum *clump = datum_get<clump_datum>(clump_data, actor->clump_index);
          if (!clump->active)
            actors_clump_activate(actor->clump_index);
        }
        if (actor->squad_index != -1)
        {
          squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
          if (!((short)squad->flags & 0x80))
            squad_activate(actor->squad_index);
        }
      }
      else
      {
        result = false;
      }
    }
    else
    {
      actor->active = false;
      actor->deactivation_ticks = game_time_get();
      if (actor->dynamic_firing_set_index != -1)
        dynamic_firing_set_discard(actor_index);
      ai_globals->active_actor_count--;
    }
  }
  return result;
}

void actor_detach_from_unit(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  long unit_index = actor->unit_index;
  if (unit_index == -1)
    return;
  biped_datum *unit = (biped_datum *)object_get(unit_index);
  unit->unit.actor_index = -1;
  actor->unit_index = -1;
  ::unit_set_actively_controlled(unit_index, false);
}

void actor_erase(long actor_index, bool immediately)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  long unit_index = actor->unit_index;
  long object_index = actor->swarm_index;
  ::actor_delete(actor_index, false);
  if (unit_index != -1)
  {
    if (immediately)
    {
      objects_purge_deleted_object(unit_index);
      return;
    }
    ::object_delete(unit_index);
    return;
  }
  if (object_index != -1)
    ::swarm_delete(object_index);
}

/* character definitions read by actor_select_character_variant: the variant block at 0x54, parents at 0x10 */
struct s_table;
// 0x4fd728
void __tls_set_g_actor_firing_position_hash_allocator(void *allocator)
{
  g_actor_firing_position_hash = (s_hash_table *)allocator;
  if (allocator)
    hash_table_add((s_hash_table *)allocator);
}

static __forceinline void actors_actor_iterator_new(actor_iterator *iterator, bool active_only)
{
  if (ai_globals->enabled) {
    actors_data_iterator_new(&iterator->m_iterator, (s_data_array *)actor_data);
    iterator->active_only = active_only;
  }
}
static __forceinline s_tag_block *actors_character_get_variants(long character_index)
{
  s_tag_block *result = 0;
  long index = character_index;
  while (index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, index);
    if (definition->variants.count > 0) {
      result = &definition->variants;
      break;
    }
    index = definition->parent_character.index;
  }
  return result;
}

long actor_select_character_variant(long character_index)
{
  s_tag_block *variants = actors_character_get_variants(character_index);
  long variant_tag = 0;
  if (variants) {
    if (variants->count == 1) {
      variant_tag = TAG_BLOCK_GET_ELEMENT(variants, 0, s_character_variant)->tag;
    } else if (variants->count > 1) {
      unsigned char candidates[64];
      signed char counts[64] = {};
      actor_iterator iterator;
      actors_actor_iterator_new(&iterator, false);
      actor_datum *actor;
      while ((actor = actor_iterator_next(&iterator)) != 0) {
        if (actor->character_index == character_index && actor->unit_index != -1) {
          object_datum *unit = object_get(actor->unit_index);
          if (unit->variant_index >= 0 && unit->variant_index < 64)
            counts[unit->variant_index]++;
        }
      }
      short candidate_count = 0;
      short best_count = 0x7fff;
      for (short index = 0; index < variants->count; index++) {
        s_character_variant *variant = TAG_BLOCK_GET_ELEMENT(variants, index, s_character_variant);
        if (variant->tag != 0 && variant->variant >= 0 && variant->variant < 64) {
          short count = counts[variant->variant];
          if (count < best_count) {
            candidates[0] = (unsigned char)index;
            candidate_count = 1;
            best_count = count;
          } else if (count == best_count) {
            candidates[candidate_count] = (unsigned char)index;
            candidate_count++;
          }
        }
      }
      if (candidate_count > 0) {
        short pick = random_range(TLS_RANDOM_SEED, 0, candidate_count);
        variant_tag = TAG_BLOCK_GET_ELEMENT(variants, (signed char)candidates[pick], s_character_variant)->tag;
      }
    }
  }
  return variant_tag;
}

static __forceinline character_voice_properties *actors_character_voice_properties_get(long character_index)
{
  character_voice_properties *result = 0;
  while (character_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, character_index);
    if (definition->voice_properties.count > 0) {
      result = TAG_BLOCK_GET_ELEMENT(&definition->voice_properties, 0, character_voice_properties);
      break;
    }
    character_index = definition->parent_character.index;
  }
  return result;
}
static __forceinline void actors_unit_dialogue_set(long unit_index, long dialogue_index, long designator)
{
  s_unit_dialogue *dialogue = (s_unit_dialogue *)object_header_block_get(unit_index,
      &((unit_datum *)object_get(unit_index))->unit.dialogue_reference);
  dialogue->dialogue_index = dialogue_index;
  dialogue->designator = designator;
}

// 0x4febd0
void actor_select_character_voice(long character_index, long unit_index)
{
  s_tag_block *voices = 0;
  character_voice_properties *voice_properties = actors_character_voice_properties_get(character_index);
  if (voice_properties && voice_properties->voices.count() > 0)
    voices = &voice_properties->voices;
  s_tag_block *variants = actors_character_get_variants(character_index);
  if (variants) {
    object_datum *unit = object_get(unit_index);
    for (short index = 0; index < variants->count; index++) {
      s_character_variant *variant = TAG_BLOCK_GET_ELEMENT(variants, index, s_character_variant);
      if (variant->variant == unit->variant_index && variant->voices.count > 0) {
        voices = &variant->voices;
        break;
      }
    }
  }
  if (voices && voices->count > 0) {
    long dialogue_index = -1;
    long designator = 0;
    if (voices->count == 1) {
      s_character_voice *voice = TAG_BLOCK_GET_ELEMENT(voices, 0, s_character_voice);
      dialogue_index = voice->dialogue.index;
      designator = voice->designator;
    } else {
      character_general_properties *general_properties = character_definition_general_properties_get(character_index);
      short actor_type = -1;
      unsigned char candidates[64];
      signed char counts[64] = {};
      actor_iterator iterator;
      if (general_properties)
        actor_type = (short)general_properties->type;
      actors_actor_iterator_new(&iterator, false);
      actor_datum *actor;
      while ((actor = actor_iterator_next(&iterator)) != 0) {
        if (actor->unit_index != -1 && actor->type == actor_type) {
          s_unit_dialogue *dialogue = (s_unit_dialogue *)object_header_block_get(actor->unit_index,
              &((unit_datum *)object_get(actor->unit_index))->unit.dialogue_reference);
          if (dialogue->dialogue_index != -1) {
            for (short index = 0; index < voices->count; index++) {
              if (dialogue->designator == TAG_BLOCK_GET_ELEMENT(voices, index, s_character_voice)->designator) {
                counts[index]++;
                break;
              }
            }
          }
        }
      }
      for (short index = 0; index < voices->count; index++) {
        float weight = TAG_BLOCK_GET_ELEMENT(voices, index, s_character_voice)->weight;
        if (weight > 0.01f)
          counts[index] = (signed char)(counts[index] / weight);
      }
      short candidate_count = 0;
      short best_count = 0x7fff;
      for (short index = 0; index < voices->count; index++) {
        s_character_voice *voice = TAG_BLOCK_GET_ELEMENT(voices, index, s_character_voice);
        if (voice->designator != 0 && voice->dialogue.index != -1) {
          short count = counts[index];
          if (count < best_count) {
            candidates[0] = (unsigned char)index;
            candidate_count = 1;
            best_count = count;
          } else if (count == best_count) {
            candidates[candidate_count] = (unsigned char)index;
            candidate_count++;
          }
        }
      }
      if (candidate_count > 0) {
        short pick = random_range(TLS_RANDOM_SEED, 0, candidate_count);
        s_character_voice *voice = TAG_BLOCK_GET_ELEMENT(voices, (signed char)candidates[pick], s_character_voice);
        dialogue_index = voice->dialogue.index;
        designator = voice->designator;
      }
    }
    if (dialogue_index != -1)
      actors_unit_dialogue_set(unit_index, dialogue_index, designator);
  }
}


static __forceinline void inl_object_set_invalid_for_recycling(long object_index, bool invalid)
{
  if (object_index != -1) {
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
    if (object)
      if (invalid)
        object->recycling_flags |= FLAG(0);
      else
        object->recycling_flags &= ~FLAG(0);
  }
}
void actor_set_mission_critical(long actor_index, long source_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  for (short index = 0; index < 8; index++) {
    if (ai_globals->mission_critical[index].unit_index == -1) {
      ai_globals->mission_critical[index].flags = 0;
      ai_globals->mission_critical[index].unit_index = actor->unit_index;
      ai_globals->mission_critical[index].source_index = source_index;
      ai_globals->mission_critical[index].bring_forward_value = 0.0f;
      ai_globals->mission_critical[index].bring_forward_ticks = 0;
      inl_object_set_invalid_for_recycling(actor->unit_index, true);
      break;
    }
  }
}

// @address 0x504330 ?actor_get_timeslices@@YAXJ@Z
void actor_get_timeslices(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  actor->pathfinding_ticks_a++;
  actor->pathfinding_ticks_b++;

  bool timeslice_a = false;
  if (actor_pathfinding_available(actor_index))
  {
    if (!ai_globals->timeslice_given_a && actor->pathfinding_ticks_a > ai_globals->maximum_ticks_a
        && timeslice_game_ticks_to_seconds((float)actor->pathfinding_ticks_a) > 0.5f)
    {
      actor->pathfinding_ticks_a = 0;
      ai_globals->timeslice_given_a = true;
      timeslice_a = true;
    }
    else if (actor->pathfinding_ticks_a > ai_globals->worst_ticks_a)
    {
      ai_globals->worst_ticks_a = actor->pathfinding_ticks_a;
    }
  }
  actor->uncovering = timeslice_a;

  bool timeslice_b = false;
  if (!ai_globals->timeslice_given_b && actor->pathfinding_ticks_b > ai_globals->maximum_ticks_b
      && timeslice_game_ticks_to_seconds((float)actor->pathfinding_ticks_b) > 0.5f)
  {
    actor->pathfinding_ticks_b = 0;
    ai_globals->timeslice_given_b = true;
    timeslice_b = true;
  }
  else if (actor->pathfinding_ticks_b > ai_globals->worst_ticks_b)
  {
    ai_globals->worst_ticks_b = actor->pathfinding_ticks_b;
  }
  actor->tracking_timeslice = timeslice_b;
}

void actor_unit_control_boost(long player_index, bool value)
{
  actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
  bool allowed = true;

  if (value && !player->use_object && (unsigned int)player->root_vehicle_index != 0xffffffff) {
    allowed = ((vehicle_datum *)object_get(player->root_vehicle_index))->vehicle.airborne_ticks <= 0;
  }
  if (!allowed) {
    return;
  }
  if (value) {
    player->control_flags |= 0x800;
    return;
  }
  player->control_flags &= 0xfffff7ff;
}

void actor_set_covered(long actor_index, bool covered)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  actor->cover_index = -1;
  if (covered)
    actor->cover_flags |= FLAG(0);
  else
    actor->cover_flags &= ~FLAG(0);
  actor->cover_flags &= ~FLAG(1);
}

void actor_break(long actor_index, bool value)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (value)
    ::stimulus_register(actor_index, _behavior_stimulus_broken, 1, 3);
  actor->broken = value;
}

void actor_nearby_actors_iterator_new(actor_nearby_actors_iterator *iterator, const real_point3d *point, float radius, short team, short filter)
{
  iterator->radius = radius;
  iterator->radius_squared = radius * radius;
  iterator->team = team;
  iterator->filter = filter;
  iterator->point = *point;
  iterator->clump_index = -1;
  iterator->actor_index = -1;
  actors_clump_iterator_new(&iterator->clump_iter);
  iterator->clump_index = -1;
}

actor_datum *actor_nearby_actors_iterator_next(actor_nearby_actors_iterator *iterator)
{
  actor_datum *actor = 0;
  if (iterator->clump_index == -1) {
    bool valid = false;
    clump_datum *clump;
    while ((clump = actors_clump_iterator_next(&iterator->clump_iter)) != 0) {
      switch (iterator->filter) {
      case 0: valid = !::game_team_is_enemy((e_game_team)clump->team, (e_game_team)iterator->team); break;
      case 1: valid = ::game_team_is_enemy((e_game_team)clump->team, (e_game_team)iterator->team); break;
      case 2: valid = true; break;
      }
      if (valid && distance3d(&iterator->point, &clump->position) < clump->radius + iterator->radius) {
        iterator->clump_index = iterator->clump_iter.index;
        clump_actor_iterator_new(&iterator->actor_iter, datum_get<clump_datum>(clump_data, iterator->clump_index));
        break;
      }
    }
  }
  if (iterator->clump_index != -1) {
    while ((actor = clump_actor_iterator_next(&iterator->actor_iter)) != 0) {
      float distance_squared = distance_squared3d(&actor->position.body, &iterator->point);
      if (distance_squared < iterator->radius_squared) {
        iterator->actor_index = iterator->actor_iter.actor_index;
        iterator->distance_squared = distance_squared;
        break;
      }
    }
    if (!actor) {
      iterator->clump_index = -1;
      return actor_nearby_actors_iterator_next(iterator);
    }
  }
  return actor;
}

struct creature_datum;

bool actor_test_activation_cluster_bit_vector(long actor_index, const s_game_cluster_bit_vectors *bits)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = false;
  if (actor_runtime_within_structure_bsp(actor_index)) {
    if (actor->swarm) {
      if (actor->swarm_index != -1) {
        swarm_creature_iterator iterator;
        swarm_creature_iterator_new(actor->swarm_index, &iterator);
        while (swarm_creature_iterator_next(&iterator) != 0) {
          long parent_index = inl_object_get_ultimate_parent(iterator.current_object_index);
          object_datum *object = object_get(parent_index);
          if (actors_location_valid(&object->location)) {
            result = game_clusters_test(bits, object->location.cluster_reference);
            if (result)
              break;
          }
        }
      }
    } else {
      long parent_index = inl_object_get_ultimate_parent(actor->unit_index);
      object_datum *object = object_get(parent_index);
      if (actors_location_valid(&object->location))
        result = game_clusters_test(bits, object->location.cluster_reference);
    }
  }
  return result;
}

void actor_handle_deleted_actor(long actor_index, long object_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->cover_friend_actor_index == object_index)
    actor->cover_friend_actor_index = -1;
  if ((unsigned short)actor->layer_count < 0x8000)
  {
    short layer = 0;
    do
    {
      short behavior_index = -1;
      if (layer >= 0 && layer < 8)
        behavior_index = *(unsigned short *)(actor->behavior_state_data + actor->state_offsets[layer]);
      if (behavior_index >= 0 && behavior_index < 197)
      {
        Behavior *header = static_cast<Behavior *>(global_behavior_info[behavior_index].header);
        if (header != 0 && header->on_object_deleted != 0)
        {
          c_behavior_state *state = 0;
          if (layer >= 0 && layer < 8)
            state = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
          header->on_object_deleted(actor_index, state, object_index);
        }
      }
      layer = (short)(layer + 1);
    } while (layer <= (short)actor->layer_count);
  }
}

float actor_get_maximum_damage_velocity(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
  if (movement->jump_height == 7)
    return 100000.0f;
  float height = 2.0f * actors_ai_hint_get_max_jump_down_height((short)((movement->jump_height < 6 ? movement->jump_height : 6) - 1));
  return sqrtf(6.4173226f * height);
}

// 0x5016d0
float actor_get_gravity_scale(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  float gravity_scale = 1.0f;
  if (actor->unit_index != -1 && (FLAG(object_get_type(actor->unit_index)) & FLAG(0)))
    gravity_scale = biped_get_scale(actor->unit_index);
  return gravity_scale;
}

void actor_initiate_combat(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->suppress_combat) {
    bool engage = true;
    if (actor->squad_index != -1) {
      squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
      if (::squad_valid_task((squad_datum *)(const s_variant_reference *)squad)) {
        objective_task_request_engage(squad->objective_index, squad->task_index, true);
        engage = false;
      }
    }
    if (engage)
      actor->suppress_combat = false;
  }
}


bool actor_set_active_camo(long actor_index, bool camo)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = false;
  if (actor->unit_index != -1) {
    if (camo) {
      biped_datum *unit = (biped_datum *)object_get(actor->unit_index);
      if ((_rotl(unit->unit.unit_flags, 29) & 1) == 0)
        actor_dialogue_event(actor_index, 98, -1, 0);
      unit_active_camouflage_enable(actor->unit_index, 4.0f, false);
    } else {
      unit_active_camouflage_disable(actor->unit_index, 4.0f);
    }
    result = true;
  }
  return result;
}

bool actor_get_active_camo_state(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = false;
  if (actor->unit_index != -1) {
    biped_datum *unit = (biped_datum *)object_get(actor->unit_index);
    result = (unit->unit.unit_flags & FLAG(3)) != 0;
  }
  return result;
}

static __forceinline unsigned long object_outermost_flags(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = ((object_datum *)object_get(object_index))->parent_object_index;
    }
    return ((object_datum *)object_get(result))->flags;
}

bool actor_runtime_within_structure_bsp(long player_index)
{
    bool result = false;
    actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
    if (player->unit_index != -1) {
        unsigned long flags = object_outermost_flags(player->unit_index);
        if ((unsigned char)((flags >> 18) & 1) == 0 && (unsigned char)((flags >> 7) & 1) == 0) result = 1;
    } else if (player->swarm_index != -1) {
        swarm_creature_iterator iterator;
        iterator.next_object_index = datum_get<swarm_datum>(g_swarm_data, player->swarm_index)->first_object_index;
        iterator.current_object_index = -1;
        while (swarm_creature_iterator_next(&iterator)) {
            unsigned long flags = object_outermost_flags(iterator.current_object_index);
            if ((unsigned char)((flags >> 18) & 1) == 0 && (unsigned char)((flags >> 7) & 1) == 0) {
                result = 1;
                break;
            }
        }
    }
    return result;
}

short actor_get_standing(long player_index)
{
  short result = 0;
  long reference = datum_get<actor_datum>(actor_data, player_index)->squad_index;
  if (reference != -1) {
    long other = squad_get_leader((short)reference);
    if (other != -1) {
      if (player_index == other) {
        result = 3;
      } else {
        short a = ::actor_get_rank(other);
        short b = ::actor_get_rank(player_index);
        result = (b == a) + 1;
      }
    }
  }
  return result;
}


// @address 0x501738 ?actor_has_followers@@YA_NJ@Z
bool actor_has_followers(long actor_index)
{
    bool result = false;
    if (::actor_get_standing(actor_index) == 3) {
        squad_datum *squad = datum_get<squad_datum>(squad_data, datum_get<actor_datum>(actor_data, actor_index)->squad_index);
        unsigned short objective_index = (unsigned short)squad->objective_index;
        unsigned short task_index = (unsigned short)squad->task_index;
        unsigned short highest_rank = (unsigned short)objective_get_task_record(objective_index, task_index)->highest_rank;
        task_actor_iterator iterator;
        task_actor_iterator_new(objective_index, task_index, &iterator);
        while (task_actor_iterator_next(&iterator)) {
            if (iterator.actor_index != actor_index) {
                actor_datum *follower = datum_get<actor_datum>(actor_data, iterator.actor_index);
                short rank = 0;
                character_general_properties *properties = character_definition_general_properties_get(follower->character_index);
                if (properties) rank = properties->rank;
                long unit_index = follower->unit_index;
                if (unit_index != -1) {
                    object_datum *unit = (object_datum *)object_get(unit_index);
                    if (unit->body_vitality == k_real_zero_storage && unit->shield_vitality == k_real_zero_storage) continue;
                }
                if (rank < (short)highest_rank) {
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}

static __forceinline void actors_path_clear(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  actor->path.valid = 0;
  actor->path.obstacle_object_index = -1;
  actor->path.result_value = 0;
  actor->path_result_time = 0;
  actor->path_result_count = 0;
  actor->path_state = 0;
  actor->destination_orders.clear();
}


static __forceinline void actors_swarm_flush_prop(long swarm_index, long prop_index)
{
  long creature_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
  while (creature_index != -1)
  {
    creature_datum *object = (creature_datum *)object_get(creature_index);
    if (object->creature.link_type != 0)
      break;
    swarm_creature_state *link = (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset);
    if (!link)
      break;
    if (link->prop_index == prop_index)
      link->prop_index = -1;
    if (link->prop_ref_index == prop_index)
      link->prop_ref_index = -1;
    creature_index = link->next_swarm_creature_index;
  }
}

static __forceinline void actors_swarm_flush_structure_indices(long swarm_index)
{
  long creature_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
  while (creature_index != -1)
  {
    creature_datum *object = (creature_datum *)object_get(creature_index);
    if (object->creature.link_type != 0)
      break;
    swarm_creature_state *link = (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset);
    if (!link)
      break;
    link->target_object_index = -1;
    creature_index = link->next_swarm_creature_index;
  }
}

static __forceinline void *actors_behavior_state_get(actor_datum *actor, short layer)
{
  c_behavior_state *state = 0;
  if (layer >= 0 && layer < 8)
    state = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
  return state;
}

static __forceinline Behavior_header *actors_behavior_get(actor_datum *actor, short layer)
{
  short behavior_index = -1;
  if (layer >= 0 && layer < 8)
    behavior_index = *(unsigned short *)(actor->behavior_state_data + actor->state_offsets[layer]);
  if (behavior_index >= 0 && behavior_index < 197)
    return global_behavior_info[behavior_index].header;
  return 0;
}




// 0x5018a0
void actor_flush_prop(long actor_index, long prop_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool b7 = false, b4 = false;

  if (actor->target_prop_index == prop_index)
  {
    b4 = true;
    actor->target_prop_index = -1;
  }
  if (actor->situation.best_prop_index == prop_index)
    actor->situation.best_prop_index = -1;
  if (actor->target_actor_index == prop_index)
    actor->target_actor_index = -1;
  if (actor->danger_zone.pref_index == prop_index)
  {
    b7 = true;
    actor->danger_zone.pref_index = -1;
    actor->danger_zone.danger_type = 0;
  }
  if (actor->current_fire_target_type == 1 && actor->current_fire_target_prop_index == prop_index)
  {
    actor->current_fire_target_prop_index = -1;
    actor->current_fire_target_type = 0;
  }
  if (actor->target_unit == prop_index)
    actor->target_unit = -1;
  if (actor->surprise_prop_index == prop_index)
    actor->surprise_prop_index = -1;

  if (actor->destination_orders.type == 4 && actor->destination_orders.prop_ref_index == prop_index)
  {
    actors_path_clear(actor_index);
  }

  actors_direction_flush_prop(&actor->attention_direction, prop_index);
  actors_direction_flush_prop(&actor->secondary_aim_direction, prop_index);
  actors_direction_flush_prop(&actor->previous_aim_direction, prop_index);

  if (actor->swarm)
  {
    long u = actor->swarm_index;
    if (u != -1)
    {
      long v = datum_get<swarm_datum>(g_swarm_data, u)->first_object_index;
      while (v != -1)
      {
        creature_datum *o = (creature_datum *)object_get(v);
        if (o->creature.link_type != 0)
          break;
        swarm_creature_state *p = (swarm_creature_state *)((char *)o + o->creature.creature_reference.offset);
        if (!p)
          break;
        if (p->prop_index == prop_index)
          p->prop_index = -1;
        if (p->prop_ref_index == prop_index)
          p->prop_ref_index = -1;
        v = p->next_swarm_creature_index;
      }
    }
  }

  if ((unsigned short)actor->layer_count < 0x8000)
  {
    short i = 0;
    do
    {
      c_behavior_state *s = 0;
      if (i >= 0 && i < 8)
        s = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[i]);
      short t = s->behavior_index;
      signed char k = s->impulse.mode;
      Behavior *header = static_cast<Behavior *>(global_behavior_info[t].header);

      if ((k == 1 && s->impulse.object_index == prop_index) || (k == 2 && b4) || (k == 7 && b7))
      {
        s->impulse.type = 0;
        s->impulse.object_index = -1;
      }

      if (header->flush_prop != 0)
      {
        header->flush_prop(actor_index, s, prop_index);
      }
      i = (short)(i + 1);
    } while (i <= actor->layer_count);
  }
}

// 0x501c08
void actor_flush_position_indices(long actor_index, bool flush)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  firing_position_ref current_firing_position = actor->firing_position_reference;

  if (!fp_ref_equals(current_firing_position, FP_REF_NONE))
  {
    if (!(unsigned char)flush || ((actor->firing_position_reference.block & 0x8000) != 0 && !fp_ref_equals(current_firing_position, FP_REF_CURRENT)))
    {
      actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
    }
  }

  if (actor->destination_orders.type == 3)
  {
    if (!(unsigned char)flush
      || ((actor->destination_orders.firing_position.block & 0x8000) != 0
      && !fp_ref_equals(actor->destination_orders.firing_position, FP_REF_CURRENT)))
    {
      actors_path_clear(actor_index);
    }
  }

  unsigned int flush_dynamic_positions = (unsigned char)flush;
  for (short firing_position_index = 0; firing_position_index < 4; firing_position_index = (short)(firing_position_index + 1))
  {
    if (flush_dynamic_positions != 0)
    {
      firing_position_ref *firing_position = &actor->firing_positions[firing_position_index].reference;
      if ((firing_position->block & 0x8000) == 0 || fp_ref_equals(*firing_position, FP_REF_CURRENT))
        continue;
    }
    actor->firing_positions[firing_position_index].reference = FP_REF_NONE;
  }

  if ((unsigned short)actor->layer_count < 0x8000)
  {
    short behavior_layer = 0;
    do
    {
      short behavior_index = -1;
      if (behavior_layer >= 0 && behavior_layer < 8)
        behavior_index = *(unsigned short *)(actor->behavior_state_data + actor->state_offsets[behavior_layer]);
      if (behavior_index >= 0 && behavior_index < 197)
      {
        Behavior *header = static_cast<Behavior *>(global_behavior_info[behavior_index].header);
        if (header != 0 && header->flush_firing_position != 0)
        {
          c_behavior_state *behavior_state = 0;
          if (behavior_layer >= 0 && behavior_layer < 8)
            behavior_state = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[behavior_layer]);
          header->flush_firing_position(actor_index, behavior_state, flush);
        }
      }
      behavior_layer = (short)(behavior_layer + 1);
    } while (behavior_layer <= actor->layer_count);
  }
}


static __forceinline void s_flush_swarm_indices(long index) {
  long unit = datum_get<swarm_datum>(g_swarm_data, index)->first_object_index;
  while (unit != -1) {
    creature_datum *object = (creature_datum *)object_get(unit);
    if (object->creature.link_type != 0) break;
    swarm_creature_state *data = (swarm_creature_state *)((char *)object + object->creature.creature_reference.offset);
    if (!data) break;
    unit = data->next_swarm_creature_index;
    data->target_object_index = -1;
  }
}

static __forceinline actor_datum *s_flush_actor_get(long index) {
  return datum_get<actor_datum>(actor_data, index);
}

static __forceinline short s_flush_behavior_index_get(actor_datum *actor, short layer)
{
    short result = -1;
    if (layer >= 0 && layer < 8)
        result = ((c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]))->behavior_index;
    return result;
}
static __forceinline c_behavior_state *s_flush_behavior_state_get(actor_datum *actor, short layer)
{
    c_behavior_state *result = 0;
    if (layer >= 0 && layer < 8)
        result = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
    return result;
}

static __forceinline prop_state *s_flush_prop_state_get(const prop_ref_datum *ref)
{
    if (ref->view_index != -1)
        return &((tracking_datum *)((char *)TLS_PROP_DATA->data + (ref->view_index & 0xffff) * 256))->state;
    return &((prop_datum *)((char *)prop_data->data + (ref->prop_index & 0xffff) * 196))->state;
}
static __forceinline prop_view *s_flush_prop_view_get(const prop_ref_datum *ref)
{
    prop_view *result = 0;
    long index = ref->view_index;
    if (index != -1)
    {
        tracking_datum *v = (tracking_datum *)((char *)TLS_PROP_DATA->data + (index & 0xffff) * 256);
        if (v)
            result = &v->tracking_view;
    }
    return result;
}
static __forceinline void s_flush_prop_view_reset_search(prop_view *view)
{
    view->pursuit_type = 0;
    view->sighting_count = 0;
    view->sighting_awareness = 0;
    view->flags &= ~8;
    view->flags &= ~16;
    view->flags &= ~32;
    view->flags |= 2;
    view->flags |= 4;
    view->flags &= ~64;
    view->flags &= ~1;
}

static __forceinline Behavior_header *s_flush_behavior_get(actor_datum *actor, short layer)
{
    Behavior_header *result = 0;
    short index = s_flush_behavior_index_get(actor, layer);
    if (index >= 0 && index < 197)
        result = global_behavior_info[index].header;
    return result;
}

// 0x501e30
void actor_flush_structure_indices(long actor_index)
{
 actor_datum *actor=s_flush_actor_get(actor_index);
 location_invalidate(&actor->position.location);
 actor->sector_valid=false;
 actor->pathfinding_sector.clear();
 actors_path_clear(actor_index);
 if(actor->destination_orders.type==1)actors_path_clear(actor_index);
 if(actor->swarm&&actor->swarm_index!=-1)s_flush_swarm_indices(actor->swarm_index);
 for(short layer=0;layer<=actor->layer_count;layer++){
  Behavior *behavior=static_cast<Behavior *>(s_flush_behavior_get(actor,layer));
  if(behavior&&behavior->flush_structure){c_behavior_state *state=s_flush_behavior_state_get(actor,
      layer);behavior->flush_structure(actor_index,state);}
 }
 actor_prop_ref_iterator it;actor_prop_ref_iterator_new(&it,actor_index);
 prop_ref_datum *ref;
 while((ref=actor_prop_ref_iterator_next(&it))!=0){
  prop_state *state=s_flush_prop_state_get(ref);prop_view *view=s_flush_prop_view_get(ref);
  location_invalidate(&state->location);state->pathfinding_sector.clear();
  if(view&&view->pursuit_type==1)s_flush_prop_view_reset_search(view);
 }
}


// @address 0x503060 ?actor_get_running_blind_vector@@YA_NJPATreal_vector3d@@@Z
bool actor_get_running_blind_vector(long actor_index, real_vector3d *run_vector)
{
    bool ok = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (!actor->swarm) {
        if (((long)actor->flags & 1) != 0) {
            *run_vector = actor->throttle;
            ok = true;
        } else if (actor->path.valid) {
            ai_vector_from_points3d(&actor->position.body, &actor->path.destination, run_vector);
            ok = true;
        }
    }
    if (ok) {
        if (normalize3d(run_vector) == k_real_zero_storage)
            return false;
    }
    return ok;
}

void actor_broken_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->broken)
  {
    bool task_broken = false;
    if (actor->squad_index != -1)
    {
      squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
      if (::squad_valid_task((squad_datum *)(const s_variant_reference *)squad))
      {
        unsigned short objective_index = squad->objective_index;
        s_objective_record *entry = &((s_objective_record *)g_objectives_data->data)[objective_index];
        task_broken = _rotl((unsigned short)g_tasks_records[(short)(entry->first_task + squad->task_index)].flags.get_unsafe(), 30) & 1;
      }
    }
    if (task_broken == 0 && actor->situation.best_prop_class < 9)
      actor_break(actor_index, false);
  }
}

void actor_cover_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  signed char cover_flags = actor->cover_flags;
  if (!TEST_BIT(cover_flags, 0))
    return;
  if (TEST_BIT(cover_flags, 1))
  {
    short cover_index = (short)(actor->cover_index - 1);
    actor->cover_index = cover_index;
    if (cover_index > 0)
      return;
    actor_set_covered(actor_index, false);
    return;
  }
  if (actor->target_prop_index != -1 && target_get_prop_class(actor) < 7)
    return;
  bool path_busy = false;
  if (actor->path.valid != 0 && actor->path_state == 1)
    path_busy = true;
  if (path_busy != 0)
    return;
  character_cover_properties *cover = character_definition_cover_multi_properties_get(actor_index);
  float cover_time = 0.0f;
  if (cover != 0)
    cover_time = cover->cover_time;
  if (cover_time <= 0.0f)
    cover_time = 2.0f;
  short ticks = TLS_GAME_TIME_GLOBALS->ticks_per_second;
  actor->cover_flags = cover_flags | 2;
  actor->cover_index = (short)(int)((float)ticks * cover_time + 0.5f);
}

/* 0x503d08: per-tick vehicle bookkeeping: ignore/exit timers, vehicle desirability, then the
   entering (2), riding (3) and lockdown (1) states */
// match: preserve the long-argument caller ABI; the owner uses byte arguments.

const float k_vehicle_enter_timeout_seconds = 10.0f;
const float k_vehicle_seat_timeout_seconds = 15.0f;
const float k_vehicle_seat_attach_delay_seconds = 5.0f;
static __forceinline long actors_vehicle_seconds_to_ticks(float seconds)
{
  float ticks = (float)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds;
  return (long)(ticks + 0.5f);
}
static __forceinline bool actors_vehicle_ready_for_lockdown(long vehicle_index, e_game_team team)
{
  bool ready = true;
  if (::vehicle_is_flipped(vehicle_index)) {
    short reservations = ai_vehicle_get_num_reservations(vehicle_index, team, false);
    if (reservations < ai_vehicle_get_num_flip_actors(vehicle_index))
      ready = false;
  }
  return ready;
}
// @address 0x503d08 ?actor_vehicle_update@@YAXJ@Z
void actor_vehicle_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->type == 15)
    return;
  if (actor->exited_vehicle_index != -1) {
    short ticks = (short)(actor->detach_timer - 1);
    actor->detach_timer = ticks;
    if (ticks <= 0) {
      actor->exited_vehicle_index = -1;
      actor->exited_seat_index = -1;
      actor->detach_timer = -1;
    }
  }
  if (actor->seat_reentry_timer > 0)
    actor->seat_reentry_timer--;
  if (actor->seat_attach_timer > 0)
    actor->seat_attach_timer--;
  long squad_index = actor->squad_index;
  if (squad_index != -1) {
    bool exiting = false;
    if ((actor->attached_vehicle_index == -1 && ((short)datum_get<squad_datum>(squad_data, squad_index)->flags & FLAG(11)) != 0) ||
        (actor->vehicle_entry_flags & FLAG(1)) != 0)
      exiting = true;
    if (!exiting)
      actor->exit_timer = 0;
    else if (actor->exit_timer > actors_vehicle_seconds_to_ticks(5.0f))
      actor_attach_to_vehicle(actor_index, -1, false);
    else if (actor->root_vehicle_index == -1 && actor->pickup_player == 0xffff && actor->combat_status <= 4)
      actor->exit_timer++;
    else
      actor->exit_timer = 0;
  }
  if (actor->attached_vehicle_index == -1)
    return;
  bool desirable = ai_vehicle_desirable(actor_index, actor->attached_vehicle_index, false, true);
  if (!desirable) {
    if (actor->vehicle_desire > 2) {
      actor->vehicle_desire = 2;
      actor->reserved_ticks = 0;
      actor->vehicle_entry_flags &= ~FLAG(2);
    } else if (actor->vehicle_desire < 2) {
      actor_attach_to_vehicle(actor_index, -1, false);
    }
  } else if (ai_vehicle_enterable(actor_index, actor->attached_vehicle_index, 0.0f, false)) {
    if (actor->vehicle_desire == 4) {
      actor->vehicle_desire = 3;
      actor->reserved_timer = 0;
      actor->vehicle_entry_flags &= ~FLAG(1);
    }
  } else if (actor->vehicle_desire == 3) {
    actor->vehicle_desire = 4;
  }
  switch (actor->vehicle_desire) {
  case 1:
    ai_vehicle_choose_seat(actor_index);
    if (actor->attach_timer > 0) {
      short ticks = (short)(actor->attach_timer - 1);
      actor->attach_timer = ticks;
      if (ticks <= 0)
        actor->attach_timer = 0;
    }
    if (actor->attach_timer <= 0 && (short)actor->destination_seat_index != -1) {
      if (actors_vehicle_ready_for_lockdown(actor->attached_vehicle_index, (e_game_team)actor->team)) {
        ai_vehicle_lock_actors(actor->attached_vehicle_index, (e_game_team)actor->team);
        return;
      }
      actor->attach_timer = (short)actors_vehicle_seconds_to_ticks(0.5f);
      actor->vehicle_entry_flags |= FLAG(1);
    }
    break;
  case 2:
    if (++actor->reserved_ticks >= actors_vehicle_seconds_to_ticks(k_vehicle_seat_timeout_seconds)) {
      actor_attach_to_vehicle(actor_index, -1, false);
      return;
    }
    if (desirable) {
      bool seat_chosen = ai_vehicle_choose_seat(actor_index);
      long vehicle_index = actor->attached_vehicle_index;
      short attach_ticks = (short)actors_vehicle_seconds_to_ticks(k_vehicle_seat_attach_delay_seconds);
      c_multi_unit_passenger_iterator iterator;
      iterator.begin(vehicle_index);
      while (iterator.next()) {
        if (((biped_datum *)object_get(iterator.m_passenger_index))->unit.player_index != -1) {
          attach_ticks = (short)actors_vehicle_seconds_to_ticks(0.2f);
          break;
        }
      }
      if (seat_chosen && actor->reserved_ticks >= attach_ticks) {
        actor_handle_seat_attach(actor_index, actor->destination_unit_index, actor->destination_seat_index);
        actor->vehicle_desire = 3;
        actor->reserved_timer = 0;
        return;
      }
    }
    break;
  case 3:
    if (!ai_unit_seat_allowed(actor_index, actor->destination_unit_index, actor->destination_seat_index, (e_game_team)actor->team)) {
      actor->vehicle_desire = 2;
      actor->reserved_ticks = 0;
      actor->vehicle_entry_flags &= ~FLAG(2);
    }
    if (actor->root_vehicle_index == -1) {
      real_point3d origin;
      real_point3d *vehicle_origin = &origin;
      object_get_origin(actor->attached_vehicle_index, vehicle_origin);
      if (distance_squared3d(&actor->position.body, &origin) > 25.0f || actor_executing_layer(actor_index, 102) == -1)
        actor->reserved_timer++;
      if (actor->reserved_timer >= actors_vehicle_seconds_to_ticks(k_vehicle_enter_timeout_seconds)) {
        actor_attach_to_vehicle(actor_index, -1, false);
        return;
      }
    }
    break;
  }
}

/* 0x5054d0: whether the actor may charge with its vehicle: its task style's attitude decides, or (attitude 0) the
   number of the task's actors charging in vehicles of the same type stays under the character's limit */

// @address 0x5054d0 ?actor_can_vehicle_charge@@YA_NJ@Z
bool actor_can_vehicle_charge(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool result = true;
  if (actor->squad_index != -1) {
    squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
    if (::squad_valid_task((squad_datum *)(const s_variant_reference *)squad)) {
      short attitude = 0;
      style_definition *style = actor_get_task_style(actor_index);
      if (style)
        attitude = style->attitude;
      switch (attitude) {
      case 0: {
        long vehicle_definition = object_get(actor->root_vehicle_index)->definition;
        character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, vehicle_definition);
        short maximum_count = 0x7fff;
        short count = 0;
        if (properties && properties->charge_limit > 0)
          maximum_count = properties->charge_limit;
        task_actor_iterator iterator;
        task_actor_iterator_new(squad->objective_index, squad->task_index, &iterator);
        actor_datum *other;
        while ((other = task_actor_iterator_next(&iterator)) != 0) {
          if (other->root_vehicle_index != -1
            && (actor_executing(iterator.actor_index, 109) || actor_executing(iterator.actor_index, 111))
            && object_get(other->root_vehicle_index)->definition == vehicle_definition)
            count++;
        }
        if (count >= maximum_count)
          result = false;
        break;
      }
      case 1:
        result = false;
        break;
      }
    }
  }
  return result;
}

// 0x505670
// @address 0x505670 ?actor_find_props_in_sphere@@YAFJPBTreal_point3d@@MPAJF@Z
short actor_find_props_in_sphere(long actor_index, const real_point3d *point, float radius, long *object_indices, short maximum_count)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  short count = 0;
  long prop_ref_index = actor->first_prop_ref_index;
  while (count < 16 && prop_ref_index != -1) {
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_ref_index = prop_ref->next_prop_ref_index;
    if (prop_ref->type == 1) {
      object_datum *object = object_get(prop_ref->object_index);
      real_point3d center = object->bounding_center;
      if (point_in_sphere(point, &center, object->bounding_radius + radius))
        object_indices[count++] = prop_ref->object_index;
    }
  }
  return count;
}

/* the restricted-region managers of the actor data (0x505778) and of the reference frame data (0x505838) */
template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
  // match: 0x505838: the member is registered with alignment 0, not with the alignment argument
  m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<k_set_allocator_function> >(
      k_region_index, name, type, size, 0, k_set_allocator_function);
  m_thread_id = g_registered_thread_index;
  return restricted_region_get_member_address(k_region_index, m_member_index);
}

// 0x505818
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_data_allocator>::deallocate(void *memory)
{
  actor_data = 0;
  restricted_region_free_member(1, m_member_index);
}

// 0x505778
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_data_allocator>::allocate(unsigned int size, const char *name)
{
  return ((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
      &__tls_set_g_object_header_data_allocator> *)this)->reserve_memory(name, "", size, 0);
}


// 0x505838
template void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_reference_frame_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment);


// 0x54eb88
void c_ai_direction::flush_object(long object_index)
{
  if (m_type == 7 && m_object_index == object_index)
    clear();
}

// @address 0x5058d8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_reference_frame_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_reference_frame_data_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}

// 0x5059b8
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_firing_position_hash_allocator>::deallocate(void *address)
{
  g_actor_firing_position_hash = 0;
  restricted_region_free_member(1, m_member_index);
}

/* ===== grown functions (wave 181) ===== */




static __forceinline bool actors_unit_is_mission_critical(long unit_index)
{
  bool result = false;
  for (short i = 0; i < 8; i++) {
    if (ai_globals->mission_critical[i].unit_index == unit_index) {
      result = true;
      break;
    }
  }
  return result;
}

static __forceinline long actors_create_internal_inline(long unit_index, long swarm_index, long character_index, long squad_index,
    bool initially_asleep)
{
  long actor_index = -1;
  squad_datum *squad = 0;
  if (squad_index != -1)
    squad = datum_get<squad_datum>(squad_data, squad_index);
  if ((unit_index != -1 || swarm_index != -1) && character_index != -1) {
    actor_index = actor_new(character_index);
    if (actor_index != -1) {
      actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
      object_datum *unit = 0;
      squad_definition *definition = 0;
      if (unit_index != -1)
        actor_attach_unit(actor_index, unit_index);
      else if (swarm_index != -1)
        actor_attach_swarm(actor_index, swarm_index);
      if (squad_index == -1) {
        squadless_attach_actor(actor_index);
      } else {
        if ((squad_index >> 16) == 0)
          squad_index = (squad_index & 0xffff) | ((*(short *)squad) << 16);
        ::squad_attach_actor(actor_index, (short)squad_index, false);
      }
      unit = unit_index != -1 ? object_get(unit_index) : 0;
      if (squad_index != -1)
        definition = (squad_definition *)((char *)global_scenario->squads.address + (squad_index & 0xffff) * sizeof(squad_definition));
      if (definition && definition->team)
        ::actor_set_team(actor_index, definition->team);
      else if (unit && ((unit_datum *)unit)->unit.team != -1 && ((unit_datum *)unit)->unit.team != 0)
        ::actor_set_team(actor_index, (short)((unit_datum *)unit)->unit.team);
      else
        ::actor_set_team(actor_index, g_actor_type_default_team[actor->type]);
      if (squad && !squad->team && actor->team)
        squad->team = (char)actor->team;
      if (initially_asleep)
        actor->behavior_mode = 0;
      else
        actor->behavior_mode = 3;
      actor_input_update(actor_index);
      if (!actors_location_valid(&actor->position.location)) {
        bool mission_critical = actors_unit_is_mission_critical(unit_index);
        if (!mission_critical) {
          if (actor->swarm_index != -1)
            actor_detach_swarm(actor_index);
          ::actor_delete(actor_index, false);
          actor_index = -1;
        }
      }
    }
  }
  return actor_index;
}

// @address 0x4fdf70 ?actor_create@@YAJJJJ_N@Z
long actor_create(long unit_index, long character_index, long squad_index, bool initially_asleep)
{
  return actors_create_internal_inline(unit_index, -1, character_index, squad_index, initially_asleep);
}

// @address 0x4fe198 ?actor_create_internal@@YAJJJJJ_N@Z
long actor_create_internal(long unit_index, long swarm_index, long character_index, long squad_index, bool initially_asleep)
{
  return actors_create_internal_inline(unit_index, swarm_index, character_index, squad_index, initially_asleep);
}

/* 0x500ab8 */

static __forceinline real_point3d *actors_switch_point_from_line3d(const real_point3d *p, const real_point3d *v, float t, real_point3d *r)
{
  r->x = p->x + v->x * t;
  r->y = p->y + v->y * t;
  r->z = p->z + v->z * t;
  return r;
}
static __forceinline bool actors_switch_ai_find_location(const real_point3d *p, s_location *l)
{
  real_point3d raised;
  const real_point3d up = {0.0f, 0.0f, 1.0f};
  actors_switch_point_from_line3d(p, &up, 0.1f, &raised);
  scenario_location_from_point((s_location *)l, &raised);
  return l->leaf_index != 65535;
}

static __forceinline prop_ref_datum *actors_switch_prop_next(actor_prop_ref_iterator *i)
{
  prop_ref_datum *result = 0;
  i->index = i->next_index;
  if (i->index != -1) {
    result = (prop_ref_datum *)((char *)((prop_ref_datum *)prop_ref_data->data) + (i->index & 0xffff) * 60);
    i->next_index = result->next_prop_ref_index;
  }
  return result;
}

// @address 0x500ab8 actor_switch_bsp
unsigned char actor_switch_bsp(long index)
{
  bool result = false;
  actor_datum *a = s_flush_actor_get(index);
  result = actor_runtime_within_structure_bsp(index);
  if (result) {
    actor_flush_structure_indices(index);
    actor_flush_position_indices(index, false);
    actor_dynamic_firing_set_support_object(index, -1);
    ::ai_find_location(&a->position.body, &a->position.location);
    actor_prop_ref_iterator iterator;
    actor_prop_ref_iterator_new(&iterator, index);
    prop_ref_datum *ref;
    while ((ref = actors_switch_prop_next(&iterator)) != 0) {
      prop_state *state = s_flush_prop_state_get(ref);
      prop_view *view = s_flush_prop_view_get(ref);
      actors_switch_ai_find_location(&state->position, &state->location);
      state->pathfinding_sector.clear();
      if (view && view->pursuit_type == 1)
        s_flush_prop_view_reset_search(view);
    }
  }
  return result;
}

// @address 0x503138 actor_change_squad
void actor_change_squad(long actor_index, short squad_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  short old_task_index = -1;
  short old_objective_index = -1;
  short new_task_index = -1;
  short new_objective_index = -1;
  if (squad_index != -1) {
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    new_objective_index = squad->objective_index;
    new_task_index = squad->task_index;
  }
  actor_flush_position_indices(actor_index, false);
  if (actor->squadless) {
    squadless_detach_actor(actor_index);
  } else if (actor->squad_index != -1) {
    squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
    old_objective_index = squad->objective_index;
    old_task_index = squad->task_index;
    ::squad_detach_actor(actor_index, false);
  }
  if (squad_index == -1)
    squadless_attach_actor(actor_index);
  else
    ::squad_attach_actor(actor_index, squad_index, true);
  if (new_objective_index != -1 && new_task_index != -1 && new_objective_index != old_objective_index && new_task_index != old_task_index)
    ::actor_handle_task_change(actor_index, objective_get_task(new_objective_index, new_task_index));
}

// @address 0x503918 ?actor_suppress_combat_update@@YAXJ@Z
void actor_suppress_combat_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->suppress_combat != 0) {
    long squad_index = actor->squad_index;
    bool allow_combat = true;
    if (squad_index != -1) {
      squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
      if ((unsigned char)::squad_valid_task((squad_datum *)(const s_variant_reference *)squad) != 0) {
        if (actor->situation.best_prop_class >= 11 && (_rotl(objective_get_task(squad->objective_index, squad->task_index)->flags, 28) & 1) == 0) {
          objective_task_request_engage(squad->objective_index, squad->task_index, true);
          allow_combat = true;
        } else {
          allow_combat = _rotl((unsigned short)objective_get_task_record(squad->objective_index, squad->task_index)->flags.get_unsafe(), 21) & 1;
        }
      }
    }
    if ((unsigned int)allow_combat != 0)
      actor->suppress_combat = 0;
  }
}

static __forceinline long actors_change_seconds_to_ticks_round(float seconds)
{
  float ticks = seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
  return (long)(ticks + k_real_constant_pool_half[0]);
}


// @address 0x504ee8 ?actor_handle_change_internal@@YAXJ_N0@Z


// match: preserve the extra floating argument and caller register allocation.
// match: 0x504ee8: passes the legacy transition-time argument, which the callee ignores.
void actor_handle_change_internal(long index, bool changed, bool camo)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, index);
  if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE)) {
    if (!::actor_move_to_firing_position_update_status(index, actor->firing_position_reference, false)) {
      actor_set_current_firing_position(index, FP_REF_NONE.value);
      actor->search_active = false;
    }
  } else {
    actor->search_active = false;
  }
  if (!changed || actor->combat_status <= 4)
    actor->suppress_combat = changed;
#if defined(_XBOX) && !defined(__clang__)
  // match: 0x504ee8: the float loads through the constant-pool base register the function already holds
  ((bool (*)(long, bool, float))actor_set_active_camo)(index, camo, k_real_constant_pool_half[7417]);
#else
  ((bool (*)(long, bool, float))actor_set_active_camo)(index, camo, 4.0f);
#endif
  if (actor->dynamic_firing_set_index != -1)
    actor_dynamic_firing_set_support_object(index, -1);
  if (actor->activation_ticks != -1 && game_time_get() - actor->activation_ticks > actors_change_seconds_to_ticks_round(k_real_constant_pool_half[0])) {
    actor_datum *reset_actor = datum_get<actor_datum>(actor_data, index);
    reset_actor->activity_index = -1;
    reset_actor->activity_variant = -1;
    reset_actor->activity_parameter = -1;
  }
  actor_prop_ref_iterator iterator;
  actor_prop_ref_iterator_new(&iterator, index);
  while (actor_prop_ref_iterator_next(&iterator) != 0)
    ::actor_perception_unassailable(index, iterator.index, false, true);
  actor_update_goal_status(index);
}

// @address 0x505120 ?actor_handle_task_change@@YAXJPAUs_task@@@Z
void actor_handle_task_change(long actor_index, struct s_task * task)
{
  bool changed = false;
  bool camo = false;
  if (task) {
    changed = true;
    camo = (bool)(_rotl(task->flags, 27) & 1);
  }
  ::actor_handle_change_internal(actor_index, changed, camo);
  if (task && task->command_script_index != -1)
    cs_attach_command_script(actor_index, task->command_script_index, -1);
}


// @address 0x505078 ?actor_update_goal_status@@YAXJ@Z
void actor_update_goal_status(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  long squad_index = actor->squad_index;
  actor->goal_status = 0;
  actor->goal_area_valid = false;
  if (squad_index != -1)
  {
    squad_area_iterator iterator;
    squad_area_iterator_new(&iterator, (short)squad_index, actor_get_area_flags(actor_index), actor_get_area_type(actor_index), true,
        (e_pureform)actor_pureform_get_firing_point_selection_form(actor_index));
    if (iterator.iterator_type == 2 && iterator.enabled_only)
    {
      actor->goal_status = 2;
    }
    else
    {
      actor->goal_status = 1;
    }
  }
}

/* ---- 0xadd38 actor_input_sample_position (CLEAN RECORD, exact on bus) ---- */
static __forceinline object_datum *actors3_input_object_get(long object_index)
{
  return (object_datum *)object_get(object_index);
}

// @address 0xadd38 ?actor_input_sample_position@@YAXJJPAUactor_position_data@@@Z
void actor_input_sample_position(long actor_index, long object_index, actor_position_data *data)
{
  object_datum *object = actors3_input_object_get(object_index);
  object_get_origin(object_index, &data->body);
  data->forward = object->forward;
  if ((1 << object->object_type) & 0x1003)
    unit_get_head_position(object_index, &data->head);
  else
    data->head = data->body;
  object_get_velocities(object_index, &data->velocity, 0);
  long parent_index = inl_object_get_ultimate_parent(object_index);
  data->location = actors3_input_object_get(parent_index)->location;
}


static __forceinline biped_datum *actors_input_unit_get(long index)
{
  return (biped_datum *)object_get(index);
}
static __forceinline vehicle_definition *actors_input_definition_get(long index)
{
  return TAG_GET('vehi', vehicle_definition, index);
}
static __forceinline long actors_input_motor_program(long index)
{
  biped_datum *unit = actors_input_unit_get(index);
  return *(long *)((char *)unit + unit->unit.motor_program_offset);
}
static __forceinline bool actors_input_variable_camera_up(long index)
{
  bool result = false;
  long program = actors_input_motor_program(index);
  if (program == 7 || program == 8)
    result = true;
  return result;
}
static __forceinline bool actors_input_test_flag(unsigned long flags, long bit)
{
  return ((flags >> bit) & 1) != 0;
}
static __forceinline long actors_input_root_object(long index)
{
  while (index != -1) {
    biped_datum *object = actors_input_unit_get(index);
    if (!actors_input_test_flag(object->object.flags, 26))
      break;
    index = object->object.parent_object_index;
  }
  return index;
}
static __forceinline real_vector3d *actors_input_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
  float i = a->j * b->k - a->k * b->j;
  float k = a->i * b->j - a->j * b->i;
  float j = a->k * b->i - a->i * b->k;
  result->i = i;
  result->j = j;
  result->k = k;
  return result;
}
static __forceinline void actors_input_set_gravity_scale(long unit_index, float gravity_scale)
{
  actors_input_unit_get(unit_index)->biped.gravity_scale = gravity_scale;
}
// @address 0x5044d8 ?actor_input_update@@YAXJ@Z
void actor_input_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->swarm) {
    swarm_input_update(actor->swarm_index);
  } else {
    character_general_properties *general_properties = character_definition_general_properties_get(actor->character_index);
    biped_datum *unit = actors_input_unit_get(actor->unit_index);
    long parent_index = unit->object.parent_object_index;
    biped_datum *parent = parent_index == -1 ? 0 : actors_input_unit_get(parent_index);
    short unit_type = unit->object.object_type;
    actor_input_sample_position(actor_index, actor->unit_index, &actor->position);
    real_point3d body_position;
    unit_get_body_position(actor->unit_index, &body_position);
    s_water_physics_test_sphere_result water_result;
    actor->underwater = water_physics_test_sphere(true, &body_position, 0.0f, &water_result);
    actor->use_object = (general_properties->flags & 2) || (unit_type == 0 && actors_input_unit_get(actor->unit_index)->biped.physics.m_mode == 2);
    actor->variable_up = parent_index == -1 && actors_input_motor_program(actor->unit_index) == 7;
    long vehicle_index = -1;
    long old_vehicle_index = actor->root_vehicle_index;
    actor->driver = false;
    actor->passenger = false;
    actor->controlling_other_unit = false;
    actor->vehicle_style_disabled = false;
    actor->root_vehicle_index = -1;
    actor->vehicle_driver_type = 0;
    actor->controlled_unit_index = -1;
    actor->friendly_player_in_vehicle = false;
    if (parent && parent->object.object_type == 1)
      vehicle_index = parent_index;
    else if (unit->object.object_type == 1)
      vehicle_index = actor->unit_index;
    if (vehicle_index != -1) {
      long root_vehicle_index = ai_get_root_vehicle(vehicle_index);
      biped_datum *vehicle = actors_input_unit_get(root_vehicle_index);
      e_vehicle_type vehicle_type = vehicle_get_type(root_vehicle_index);
      actor->root_vehicle_index = root_vehicle_index;
      if (vehicle->unit.driver_object_index == actor->unit_index || vehicle->unit.actor_index == actor_index) {
        vehicle_definition *definition = actors_input_definition_get(vehicle->object.definition);
        if (vehicle_type != 5) {
          actor->driver = true;
          actor->vehicle_driver_type = 1;
          if (actors_input_test_flag(definition->flags, 5)) {
            if (actors_input_test_flag(definition->flags, 6)) {
              actor->use_object = true;
              actor->vehicle_driver_type = actors_input_test_flag(definition->flags, 7) ? 2 : 4;
            } else {
              actor->vehicle_driver_type = actors_input_test_flag(definition->flags, 7) ? 3 : 5;
            }
          }
        }
      }
      c_data_iterator<player_datum> iterator;
      actors_data_iterator_new(&iterator.m_iterator, player_data);
      while (iterator.next()) {
        player_datum *player = iterator.m_datum;
        if (player->unit_index != -1 && game_team_is_friendly((e_game_team)actor->team, (e_game_team)player->configuration.host.team_index)) {
          biped_datum *player_unit = actors_input_unit_get(player->unit_index);
          if (player_unit->object.parent_object_index != -1 && actors_input_root_object(player_unit->object.parent_object_index) == root_vehicle_index) {
            actor->friendly_player_in_vehicle = true;
            break;
          }
        }
      }
    }
    if (old_vehicle_index != actor->root_vehicle_index)
      actor_update_goal_status(actor_index);
    if (unit->object.parent_object_index != -1 && ((1 << object_header_get(unit->object.parent_object_index)->object_type) & 0x1003)) {
      biped_datum *parent_unit = actors_input_unit_get(unit->object.parent_object_index);
      if (parent_unit->unit.gunner_object_index == actor->unit_index) {
        long gunned_unit_index = -1;
        long weapon_index = unit_get_active_primary_weapon(unit->object.parent_object_index, &gunned_unit_index);
        if (weapon_index != -1) {
          biped_datum *weapon = actors_input_unit_get(weapon_index);
          character_weapon_properties *properties = character_weapon_properties_get(
              datum_get<actor_datum>(actor_data, actor_index)->character_index, weapon->object.definition);
          if (properties) {
            actor->controlling_other_unit = true;
            actor->controlled_unit_index = gunned_unit_index;
          }
        }
      }
      if (!actor->controlling_other_unit && !actor->driver && unit->unit.parent_seat_index != -1) {
        unit_definition *definition = TAG_GET('unit', unit_definition, parent_unit->object.definition);
        actor->vehicle_style_disabled = (definition->seats[unit->unit.parent_seat_index].flags & (1UL << 11)) != 0;
      }
    }
    if (unit->object.parent_object_index != -1)
      actor->passenger = !actor->driver;
    character_movement_properties *movement_properties = character_definition_movement_properties_get(actor->character_index);
    actor->movement_3d = actor->passenger;
    if (movement_properties && (movement_properties->flags & (1UL << 11)))
      actor->movement_3d = true;
    if (actor_pathfinding_available(actor_index)) {
      actor->pathfinding_sector.clear();
      actor->sector_valid = false;
    }
    bool airborne = false;
    if (unit->object.object_type == 0 && actor->root_vehicle_index == -1 && motor_system_in_airborne_state(actor->unit_index))
      airborne = true;
    if (actor->airborne && !airborne)
      actors_input_set_gravity_scale(actor->unit_index, 1.0f);
    actor->airborne = airborne;
    if (actor->driver)
      object_get_orientation(actor->root_vehicle_index, &actor->facing_vector, 0);
    else
      object_get_orientation(actor->unit_index, &actor->facing_vector, 0);
    if (!actor->use_object && !actors_input_variable_camera_up(actor->unit_index)) {
      if (normalize2d(&actor->facing_vector.xy) > 0.0f)
        actor->facing_vector.k = 0.0f;
      else
        actor->facing_vector = *global_forward3d;
    }
    if (actor->controlling_other_unit) {
      biped_datum *gunned_unit = actors_input_unit_get(actor->controlled_unit_index);
      bool use_facing_vector = false;
      if (object_header_get(actor->controlled_unit_index)->object_type == 1) {
        vehicle_definition *definition = actors_input_definition_get(gunned_unit->object.definition);
        use_facing_vector = (definition->flags & (1UL << 3)) != 0;
      }
      if (use_facing_vector)
        object_get_orientation(actor->controlled_unit_index, &actor->aiming_vector, 0);
      else
        actor->aiming_vector = gunned_unit->unit.aiming_vector;
    } else {
      actor->aiming_vector = unit->unit.aiming_vector;
    }
    actor->looking_vector = unit->unit.looking_vector;
    actors_input_cross_product(global_up3d, &actor->looking_vector, &actor->left_vector);
    normalize3d(&actor->left_vector);
    actors_input_cross_product(&actor->looking_vector, &actor->left_vector, &actor->up_vector);
    if (actor->had_shield && !(unit->object.shield_vitality > 0.0f))
      actor->had_shield = false;
    actor->body_vitality = unit->object.body_vitality;
    actor->shield_vitality = unit->object.shield_vitality;
    actor->body_damage = unit->object.body_damage;
    actor->shield_damage = unit->object.shield_damage;
  }
  actor->inherent_scariness = actor_get_inherent_scariness(actor->unit_index);
  actor->player_weight_valid = false;
}

/* ---- 0x4fd620 actors_initialize ---- */
struct s_attachment_state;

// @address 0x4fd620 ?actors_initialize@@YAXXZ
void actors_initialize(void)
{
  actors3_hash_table *hash;
  actor_data = (s_data_array *)data_new("actor", 0x100, 0xa8c, 0, (c_allocation_base *)&g_actor_data_allocator);
  g_ai_reference_frame_data = (s_attachment_state *)g_ai_reference_frame_data_allocator.reserve_memory("ai_reference_frame", 0, 0x4b0, 0);
  memset(g_ai_reference_frame_data, 0, sizeof(s_attachment_state) * 150);
  hash = (actors3_hash_table *)((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
      &__tls_set_g_actor_firing_position_hash_allocator> *)(void *)&g_actor_firing_position_hash_allocator)
             ->allocate(offsetof(s_hash_table, buckets) + 0x400 * sizeof(s_hash_table_bucket *) + 0x100 * (4 + sizeof(s_hash_table_bucket)),
                 "actor firing-position owners");
  if (hash) {
    strncpy(hash->name, "actor firing-position owners", 32);
    // match: 0x4fd620: storing through a pointer local keeps the allocator store (stw 0x34) after it; name[31] = 0 swaps them
    char *name_end = &hash->name[31];
    *name_end = 0;
    hash->allocator = (c_allocation_base *)&g_actor_firing_position_hash_allocator;
    hash->count = 0x400;
    hash->entry_count = 0x100;
    hash->hash = (unsigned long (*)(void *))firing_position_ref_hash_function;
    hash->equal = (unsigned char (*)(void *, void *))firing_position_ref_hash_compare;
    hash->entry_size = 4;
    hash_table_reset((s_hash_table *)hash);
  }
  g_actor_firing_position_hash = (s_hash_table *)hash;
}

/* ---- 0x504cd0 actor_unit_control ---- */
// @address 0x504cd0 ?actor_unit_control@@YAXJ@Z
void actor_unit_control(long actor_index)
{
  int unit_control_buffer[32];
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
  if (unit->unit.player_index != -1 && !players_globals->inhibited)
    return;
  unit_control_data *control = (unit_control_data *)unit_control_buffer;
  unit_control_clear((unit_control_data *)control);
  int flag = actor->flags;
  control->animation_state = actor->output_animation_state;
  long base_control_flags = actor->control_flags;
  control->primary_trigger = actor->output_primary_trigger;
  control->secondary_trigger = actor->output_secondary_trigger;
  control->throttle = actor->output_throttle;
  control->facing = actor->output_facing;
  control->aiming = actor->output_aiming;
  control->looking = actor->output_looking;
  control->aiming_speed = actor->output_aiming_speed;
  if ((flag & 2) != 0)
    control->control_flags = base_control_flags | 0x10000;
  else
    control->control_flags = base_control_flags & ~0x10000;
  control->gaze_position = actor->output_gaze;
  if (actor->actively_controlled != 0) {
    ::unit_set_actively_controlled(actor->unit_index, true);
    actor->actively_controlled = 0;
  }
  int sim_index = unit->unit.simulation_actor_datum_index;
  if (sim_index != -1) {
    if (simulation_globals_get()->aborted != 0)
      return;
    c_simulation_world *world = simulation_globals_get()->world;
    long world_type = world->m_world_type;
    if (world_type == 0)
      return;
    bool valid = world_type != 4 && world_type != 6 && world_type != 8;
    if (!valid)
      return;
    {
      if (world->m_actors[sim_index].m_simulation_actor_index != -1) {
        c_simulation_actor *sim_actor = &world->m_actors[sim_index];
        sim_actor->m_control = *control;
        sim_actor->m_control_time = TLS_GAME_TIME_GLOBALS->time;
        return;
      }
    }
  } else {
    unit_control(actor->unit_index, (const unit_control_data *)control);
  }
}

/* ---- 0x502d80 (actor_freeze) ---- */

static __forceinline unit_datum *actors_freeze_unit_get(long unit_index)
{
  return (unit_datum *)object_get(unit_index);
}


static __forceinline void actors_unit_get_aiming_vector(long unit_index, real_vector3d *aiming_vector)
{
  unit_datum *unit = actors_freeze_unit_get(unit_index);
  *aiming_vector = unit->unit.aiming_vector;
}

static __forceinline void actors_unit_get_looking_vector(long unit_index, real_vector3d *looking_vector)
{
  unit_datum *unit = actors_freeze_unit_get(unit_index);
  *looking_vector = unit->unit.looking_vector;
}

static __forceinline void actors_freeze_unit_control(long unit_index)
{
  unit_control_data control;
  unit_control_clear((unit_control_data *)&control);
  control.animation_state = 137;
  control.aiming_speed = 1;
  object_get_orientation(unit_index, &control.facing, 0);
  actors_unit_get_aiming_vector(unit_index, &control.aiming);
  actors_unit_get_looking_vector(unit_index, &control.looking);
  unit_control(unit_index, (const unit_control_data *)&control);
  ::unit_set_actively_controlled(unit_index, false);
}

// @address 0x502d80 ?actor_freeze@@YAXJ@Z
void actor_freeze(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->swarm && actor->swarm_index != -1)
    swarm_freeze(actor->swarm_index);
  else
    actors_freeze_unit_control(actor->unit_index);
  actor->actively_controlled = true;
}


static __forceinline actor_datum *actors8_actor_from_offset(long actor_offset)
{
  return (actor_datum *)((char *)((actor_datum *)actor_data->data) + actor_offset);
}
static __forceinline bool actors8_invitation_valid(const invitation_data *invitation)
{
  return invitation->type != -1 && invitation->handle != -1;
}
static __forceinline prop_ref_datum *actors8_prop_ref_get(long prop_ref_index)
{
  return datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
}
static void actors8_prop_ref_iterator_new(actor_prop_ref_iterator *iterator, long first_prop_ref_index)
{
  iterator->index = -1;
  iterator->next_index = first_prop_ref_index;
}
static prop_ref_datum *actors8_prop_ref_iterator_next(actor_prop_ref_iterator *iterator)
{
  prop_ref_datum *result = 0;
  if (iterator->next_index != -1) {
    iterator->index = iterator->next_index;
    result = (prop_ref_datum *)((char *)((prop_ref_datum *)prop_ref_data->data) + (unsigned short)iterator->next_index * 60);
    iterator->next_index = result->next_prop_ref_index;
  }
  return result;
}

static __forceinline scenario *actors9_global_scenario_get()
{
  return global_scenario;
}
static __forceinline void *actors9_tag_block_get_element(const s_tag_block *block, long index, long size)
{
  return (char *)block->address + index * size;
}
static __forceinline bool actors9_squad_valid_task(squad_datum *squad)
{
  bool valid = false;
  short objective_index = squad->objective_index;
  if (objective_index >= 0) {
    if (objective_index < actors9_global_scenario_get()->objectives.count && squad->task_index != -1) {
      s_objective *objective = (s_objective *)actors9_tag_block_get_element(&actors9_global_scenario_get()->objectives, objective_index,
          sizeof(s_objective));
      valid = (squad->task_index >= 0 && squad->task_index < objective->tasks.count);
    }
  }
  return valid;
}

// @address 0x501138 ?actor_delete@@YAXJ_N@Z
void actor_delete(long actor_index, bool erase)
{
  long actor_offset = (unsigned short)actor_index * 0xa8c;
  actor_datum *actor = (actor_datum *)((char *)((actor_datum *)actor_data->data) + actor_offset);
  long squad_index = actor->squad_index;
  short layer = (short)actor->layer_count;
  if (layer >= 0) {
    do {
      short behavior_index = -1;
      if (layer >= 0 && layer < 8)
        behavior_index = *(short *)(actor->behavior_state_data + actor->state_offsets[layer]);
      if (behavior_index >= 0 && behavior_index < 197) {
        Behavior *header = static_cast<Behavior *>(global_behavior_info[behavior_index].header);
        if (header != 0 && header->deactivate != 0) {
          c_behavior_state *state = 0;
          if (layer >= 0 && layer < 8)
            state = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
          header->deactivate(actor_index, state);
        }
      }
      layer = (short)(layer - 1);
    } while (layer >= 0);
  }
  actor->layer_count = -1;
  long script_index = actors8_actor_from_offset(actor_offset)->command_index;
  if (script_index != -1) {
    long next_script;
    do {
      next_script = ((command_script_datum *)tls_command_script_data()->data)[(unsigned short)script_index].next_cs;
      cs_remove_command_script(actor_index, script_index);
      script_index = next_script;
    } while (next_script != -1);
  }
  for (short i = 0; i < 4; i = (short)(i + 1)) {
    invitation_data *invitation = &actors8_actor_from_offset(actor_offset)->invitations[i];
    if (actors8_invitation_valid(invitation) && invitation->mode == 1)
      joint_decline(invitation);
  }
  if (actor->attached_vehicle_index != -1)
    ai_vehicle_cancel_reservation(actor_index);
  if (actor->squadless)
    squadless_detach_actor(actor_index);
  else
    squad_detach_actor(actor_index, erase);
  long clump_index = actor->clump_index;
  if (clump_index != -1)
    clump_remove_actor(clump_index, actor_index);
  actor_prop_ref_iterator prop_ref_iterator;
  actors8_prop_ref_iterator_new(&prop_ref_iterator, actors8_actor_from_offset(actor_offset)->first_prop_ref_index);
  while (actors8_prop_ref_iterator_next(&prop_ref_iterator) != 0) {
    actor_flush_prop(actor_index, prop_ref_iterator.index);
    prop_ref_datum *prop_ref = actors8_prop_ref_get(prop_ref_iterator.index);
    if (prop_ref->view_index != -1)
      ::datum_delete(TLS_PROP_DATA, prop_ref->view_index);
    ::datum_delete((s_data_array *)prop_ref_data, prop_ref_iterator.index);
  }
  actor->first_prop_ref_index = -1;
  {
    c_data_iterator<prop_datum> prop_iterator;
    prop_iterator.begin(prop_data);
    while (prop_iterator.next()) {
      prop_datum *prop = prop_iterator.get_datum();
      if (prop->actor_index == actor_index)
        prop->actor_index = -1;
    }
  }
  {
    actor_iterator iterator;
    actors_actor_iterator_new(&iterator, false);
    actor_datum *other;
    while ((other = actor_iterator_next(&iterator)) != 0) {
      if (actor != other)
        actor_handle_deleted_actor(iterator.index, actor_index);
    }
  }
  actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
  if (actor->dynamic_firing_set_index != -1)
    dynamic_firing_set_discard(actor_index);
  if (actor->swarm != 0) {
    if (actor->swarm_index != -1)
      actor_detach_swarm(actor_index);
  } else {
    long unit_index = actor->unit_index;
    if (unit_index != -1) {
      object_datum *object = object_get(unit_index);
      if (object->object_type == 0)
        ((biped_datum *)object)->biped.swarm_actor_index = actor->character_index;
    }
    actor_datum *unit_actor = actors8_actor_from_offset(actor_offset);
    long controlled_index = unit_actor->unit_index;
    if (controlled_index != -1) {
      ((biped_datum *)object_get(controlled_index))->unit.actor_index = -1;
      unit_actor->unit_index = -1;
      ::unit_set_actively_controlled(controlled_index, false);
    }
  }
  if (erase != 0 && squad_index != -1) {
    squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)squad_index];
    if (actors9_squad_valid_task(squad))
      objective_handle_death(squad->objective_index, squad->task_index, actor_index);
  }
  actor_set_active(actor_index, false);
  ::datum_delete((s_data_array *)actor_data, actor_index);
}

static __forceinline void actors_set_attention_to_object(actor_datum *actor, long object_index)
{
  if (actor->attention_level <= 2) {
    short &attention_ticks = actor->attention_ticks;
    attention_ticks = (short)game_seconds_to_ticks_round(2.0f);
    actor->attention_direction.type = 7;
    actor->attention_direction.object_index = object_index;
    actor->attention_level = 2;
  }
}

// @address 0x501020 actor_handle_weapon_trade_refusal
extern "C" void actor_handle_weapon_trade_refusal(long actor_index, long object_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  short &timer = actor->move_freeze_ticks;
  short ticks = (short)game_seconds_to_ticks_round(2.0f);
  if (timer < ticks)
    timer = ticks;
  actors_set_attention_to_object(actor, object_index);
  c_ai_action *event = &actor->action;
  event->type = 9;
  event->point_valid = false;
  event->direction = actor->attention_direction;
  event->ticks = ticks;
}

static __forceinline bool actors_weapon_ammo_reduced()
{
    unsigned long flags = game_globals->options.game_mode == 1 ? game_globals->options.campaign_skulls : 0;
    return (flags & (1UL << 5)) != 0;
}
// @address 0x4fe3f8 actor_initialize_weapon_ammo
extern "C" void actor_initialize_weapon_ammo(long character_index, long weapon_index)
{
    object_datum *weapon = object_get(weapon_index);
    character_weapon_properties *properties = character_weapon_properties_get(character_index, weapon->definition);
    if (properties) {
        if (properties->amount_min > 0.0f || properties->amount_max > 0.0f) {
            float amount = _real_random_range(TLS_RANDOM_SEED, properties->amount_min, properties->amount_max);
            if (actors_weapon_ammo_reduced()) amount *= 0.5f;
            weapon_set_current_amount(weapon_index, amount);
        }
        if (properties->rounds_min > 0 || properties->rounds_max > 0) {
            struct s_actors_weapon_rounds { short values[2]; };
            s_actors_weapon_rounds rounds = {};
            rounds.values[0] = random_range(TLS_RANDOM_SEED, properties->rounds_min, (short)(properties->rounds_max + 1));
            if (actors_weapon_ammo_reduced()) rounds.values[0] = (short)((float)rounds.values[0] * 0.5f);
            weapon_set_total_rounds(weapon_index, rounds.values);
        }
    }
}
// @address 0x4fe5d8 ?actor_customize_unit@@YAXJJJJFJ@Z
void actor_customize_unit(
    long character_index, long unit_index, long primary_weapon_index, long secondary_weapon_index, short grenade_type, long equipment_index)
{
    float shield_vitality;
    float body_vitality;
    character_vitality_properties *vitality = character_definition_vitality_properties_get(character_index);
    if (vitality) {
        game_globals_storage *globals = game_globals;
        long game_mode = globals->options.game_mode;
        short difficulty = game_mode == 1 ? globals->options.campaign_difficulty : 1;
        if (difficulty != 2) {
            if (difficulty != 3) {
                body_vitality = vitality->body_vitality;
                shield_vitality = vitality->shield_vitality;
            } else {
                body_vitality = vitality->heroic_body_vitality;
                shield_vitality = vitality->heroic_shield_vitality;
                if (body_vitality <= 0.0f && shield_vitality <= 0.0f) {
                    body_vitality = vitality->body_vitality;
                    shield_vitality = vitality->shield_vitality;
                }
            }
        } else {
            if (vitality->heroic_body_vitality > 0.0f || vitality->heroic_shield_vitality > 0.0f) {
                body_vitality = (vitality->body_vitality + vitality->heroic_body_vitality) * 0.5f;
                shield_vitality = (vitality->shield_vitality + vitality->heroic_shield_vitality) * 0.5f;
            } else {
                body_vitality = vitality->body_vitality;
                shield_vitality = vitality->shield_vitality;
            }
        }
        unsigned long skull_flags = game_mode == 1 ? globals->options.campaign_skulls : 0;
        if ((_rotl(skull_flags, 24) & 1) != 0) {
            body_vitality *= 2.0f;
            shield_vitality *= 2.0f;
        }
        if (body_vitality > 0.0f || shield_vitality > 0.0f)
            object_initialize_vitality(unit_index, &body_vitality, &shield_vitality);
    }
    if (primary_weapon_index != -1) {
        object_placement_data placement;
        unit_delete_all_weapons(unit_index);
        object_placement_data_new(&placement, primary_weapon_index, unit_index, 0);
        long weapon_index = object_new(&placement);
        if (weapon_index != -1) {
            if (unit_add_weapon_to_inventory(unit_index, weapon_index, _weapon_addition_method_silent))
                actor_initialize_weapon_ammo(character_index, weapon_index);
            else
                object_delete(weapon_index);
        }
    }
    if (secondary_weapon_index != -1) {
        object_placement_data placement;
        object_placement_data_new(&placement, secondary_weapon_index, unit_index, 0);
        long weapon_index = object_new(&placement);
        if (weapon_index != -1) {
            character_general_properties *general_properties = character_definition_general_properties_get(character_index);
            if (!((general_properties->flags & 4) != 0 &&
                    unit_add_weapon_to_inventory(unit_index, weapon_index, _weapon_addition_method_dual_wield)) &&
                !unit_add_weapon_to_inventory(unit_index, weapon_index, _weapon_addition_method_silent))
                object_delete(weapon_index);
            else
                actor_initialize_weapon_ammo(character_index, weapon_index);
        }
    }
    if (grenade_type != -1) {
        character_grenade_properties *grenade_properties = character_grenade_properties_get(character_index, grenade_type);
        if (grenade_properties) {
            short minimum_count = grenade_properties->minimum_count;
            short count = _random_range(TLS_RANDOM_SEED, 0, 0, 0, minimum_count, (short)(grenade_properties->maximum_count + 1)) - minimum_count;
            unit_datum *unit = (unit_datum *)object_get(unit_index);
            unit->unit.grenade_counts[grenade_type] = (signed char)(count + unit->unit.grenade_counts[grenade_type] + minimum_count);
            unit = (unit_datum *)object_get(unit_index);
            unit->unit.desired_grenade_index = (signed char)grenade_type;
            unit->unit.current_grenade_index = (signed char)grenade_type;
        }
    }
    if (equipment_index != -1 && equipment_definition_get_type(equipment_index, 0) != _equipment_type_none) {
        object_placement_data placement;
        object_placement_data_new(&placement, equipment_index, unit_index, 0);
        long item_index = object_new(&placement);
        if (item_index != -1)
            unit_add_equipment_to_inventory(unit_index, item_index);
    }
}

static __forceinline bool actors_unit_controllable(long unit_index)
{
    return (OBJECT_GET(unit_index, biped_datum, NONE)->unit.unit_flags & 1) != 0;
}
static __forceinline short actors_unit_get_current_grenade_type(long unit_index)
{
    return OBJECT_GET(unit_index, biped_datum, NONE)->unit.current_grenade_index;
}
static __forceinline void actors_unit_persistent_control(long unit_index, long ticks, unsigned long flags)
{
    biped_datum *unit = OBJECT_GET(unit_index, biped_datum, NONE);
    unit->unit.persistent_control_flags = flags;
    unit->unit.persistent_control_ticks = ticks;
}
static __forceinline bool actors_grenades_kept()
{
    unsigned long flags = game_globals->options.game_mode == 1 ? game_globals->options.campaign_skulls : 0;
    return (flags & (1UL << 3)) != 0;
}
static __forceinline void actors_clump_notify_death(long clump_index)
{
    clump_datum *clump = datum_get<clump_datum>(clump_data, clump_index);
    clump->death_count++;
}
static __forceinline s_actors_unit_index actors_actor_unit_index(const actor_datum *actor)
{
    return actor->unit_index_value;
}
static __forceinline long actors_actor_slot_weapon(const actor_datum *actor, bool secondary)
{
    biped_datum *unit = OBJECT_GET(actors_actor_unit_index(actor).value, biped_datum, NONE);
    long slot = secondary ? unit->unit.secondary_weapon_index : unit->unit.current_weapon_index;
    long weapon_index = -1;
    if (slot != -1)
        weapon_index = unit->unit.weapon_object_indices[slot];
    return weapon_index;
}
// @address 0x502798 ?actor_died@@YAXJ@Z
void actor_died(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_weapon_properties *weapon_properties = actor_weapon_properties_get(actor_index);
    if (actor->behavior_mode == 4 && actor->combat_status >= 3 && actor->unit_index != -1)
    {
        long unit_index = actors_actor_unit_index(actor).value;
        biped_datum *unit = OBJECT_GET(unit_index, biped_datum, NONE);
        if (actors_unit_controllable(unit_index) && unit->unit.berserk_charges > 0 &&
            (actors_actor_slot_weapon(actor, false) != -1 ||
             actors_actor_slot_weapon(actor, true) != -1))
        {
            float chance = weapon_properties ? PIN(weapon_properties->berserk_chance, 0.1f, 0.6f) : 0.1f;
            if (actor->berserking || (actor->current_fire_target_type > 0 && actor->attack_distance < 3.0f))
            {
                float boosted_chance = chance * 4.0f;
                if (boosted_chance > 0.6f)
                    boosted_chance = 0.6f;
                chance = MAX(chance, boosted_chance);
            }
            if (_real_random(TLS_RANDOM_SEED, __FILE__, __FUNCTION__, __LINE__) < chance)
            {
                float duration;
                if (weapon_properties && weapon_properties->berserk_duration != 0.0f)
                    duration = (weapon_properties->berserk_duration > 0.8f &&
                        weapon_properties->berserk_duration >= 1.3f)
                        ? 1.3f : MAX(weapon_properties->berserk_duration, 0.8f);
                else
                    duration = _real_random_range(TLS_RANDOM_SEED, 0.8f, 1.3f);
                short ticks = (short)game_seconds_to_ticks_round(duration);
                actors_unit_persistent_control(actor->unit_index, ticks, 0x840000);
                unit->unit.berserk_charges = (signed char)ticks;
            }
        }
    }
    if (actor->unit_index != -1)
    {
        long unit_index = actors_actor_unit_index(actor).value;
        biped_datum *unit = OBJECT_GET(unit_index, biped_datum, NONE);
        character_grenade_properties *grenade_properties = character_grenade_properties_get(
            actor->character_index, actors_unit_get_current_grenade_type(unit_index));
        long weapon_indices[2];
        weapon_indices[0] = actors_actor_slot_weapon(actor, false);
        weapon_indices[1] = actors_actor_slot_weapon(actor, true);
        if (!actors_grenades_kept())
        {
            if (!ai_globals->drop_grenades
                || (grenade_properties && _real_random(TLS_RANDOM_SEED, __FILE__, __FUNCTION__, __LINE__) < grenade_properties->drop_chance))
                memset(unit->unit.grenade_counts, 0, sizeof(unit->unit.grenade_counts));
        }
        if (weapon_properties)
        {
            for (long i = 0; i < 2; i++)
            {
                long weapon_index = weapon_indices[i];
                if (weapon_index != -1)
                {
                    if (weapon_properties->amount_min > 0.0f || weapon_properties->amount_max > 0.0f)
                    {
                        float amount = _real_random_range(TLS_RANDOM_SEED, weapon_properties->amount_min, weapon_properties->amount_max);
                        if (ice_cream_flavor_available(5))
                            amount *= 0.5f;
                        weapon_set_current_amount(weapon_index, amount);
                    }
                    if (weapon_properties->rounds_min > 0 || weapon_properties->rounds_max > 0)
                    {
                        short rounds[2];
                        memset(rounds, 0, sizeof(rounds));
                        rounds[0] = random_range(TLS_RANDOM_SEED, weapon_properties->rounds_min, (short)(weapon_properties->rounds_max + 1));
                        if (ice_cream_flavor_available(5))
                            rounds[0] = (short)((float)rounds[0] * 0.5f);
                        weapon_set_total_rounds(weapon_index, rounds);
                    }
                }
            }
        }
        if ((1 << unit->object.object_type) & 1)
            unit->biped.squad_index = (short)actor->squad_index;
    }
    if (actor->clump_index != -1)
        actors_clump_notify_death(actor->clump_index);
    actor_delete(actor_index, true);
}
static __forceinline void actors_direction_flush_object(c_ai_direction *direction, long object_index)
{
  if (direction->m_type == 7 && direction->m_object_index == object_index)
    direction->clear();
}

// @address 0x5695a8 actor_handle_deleted_object_directions
extern "C" void actor_handle_deleted_object_directions(long actor_index, long object_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  actors_direction_flush_object(&actor->look_direction, object_index);
  actors_direction_flush_object(&actor->aim_direction, object_index);
  actors_direction_flush_object(&actor->attention_direction, object_index);
  if (actor->secondary_aim_direction.m_type == 7 && actor->secondary_aim_direction.m_object_index == object_index)
  {
    c_ai_direction direction = actor->secondary_aim_direction;
    real_vector3d vector;
    bool valid;
    real_point3d point;
    if (direction.get(actor_index, _ai_direction_mode_object_point, NULL, &vector, &valid, &point) && valid)
    {
      actor->secondary_aim_direction.set_point(&point);
      actor->previous_aim_direction = actor->secondary_aim_direction;
    }
  }
  actors_direction_flush_object(&actor->secondary_aim_direction, object_index);
  actors_direction_flush_object(&actor->previous_aim_direction, object_index);
  if (actor->dialogue_object_index == object_index)
  {
    actor->dialogue_object_index = -1;
    actor->dialogue_object_distance = 0.0f;
  }
}


static __forceinline bool actors_path_executing(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  bool executing = false;
  if (actor->path.valid && actor->path_state == 1)
    executing = true;
  return executing;
}

void actor_handle_deleted_object(long actor_index, long object_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->root_vehicle_index == object_index)
  {
    actor->root_vehicle_index = -1;
    actor->driver = false;
    actor->passenger = false;
    actor->vehicle_driver_type = 0;
  }
  if (actor->controlled_unit_index == object_index)
  {
    actor->controlled_unit_index = -1;
    actor->controlling_other_unit = false;
  }
  if (actor->danger_zone.object_index == object_index)
  {
    actor->danger_zone.object_index = -1;
    actor->danger_zone.danger_type = 0;
  }
  if (actor->path.obstacle_object_index == object_index)
  {
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
  }
  if (actor->movement_obstacle_index == object_index)
  {
    actor->movement_obstacle_index = -1;
    actor->movement_obstacle_ticks = 0;
  }
  if (actor->target_object_index == object_index)
    actor->target_object_index = -1;
  c_recent_obstacle *obstacle = actor_find_recent_obstacle(actor_index, object_index);
  if (obstacle)
    obstacle->clear();
  if (actor->exited_vehicle_index == object_index)
    actor->exited_vehicle_index = -1;
  if (actor->attached_vehicle_index == object_index || actor->destination_unit_index == object_index)
    actor_attach_to_vehicle(actor_index, -1, false);
  if (actor->grenade_target_index == object_index)
    actor->grenade_target_index = -1;
  actor_handle_deleted_object_directions(actor_index, object_index);
  actors_direction_flush_object(&actor->action.direction, object_index);
  if (actors_path_executing(actor_index) && actor->destination_orders.action.valid())
    actor->destination_orders.action.direction.flush_object(object_index);
  if (actor->support_object_index == object_index)
    actor_dynamic_firing_set_support_object(actor_index, -1);
  for (short layer = 0; layer <= actor->layer_count; layer++)
  {
    c_behavior_state *state = (c_behavior_state *)actors_behavior_state_get(actor, layer);
    if (state)
    {
      Behavior *behavior = static_cast<Behavior *>(global_behavior_info[state->behavior_index].header);
      if (!state->impulse.mode && state->impulse.object_index == object_index)
      {
        state->impulse.type = false;
        state->impulse.object_index = -1;
      }
      if (behavior->handle_deleted_object)
        behavior->handle_deleted_object(actor_index, state, object_index);
    }
  }
  cs_handle_deleted_object(actor_index, object_index);
}


// @address 0x503800 ?actor_berserk_update@@YAXJ@Z
void actor_berserk_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->berserking)
  {
    if (actor->combat_status >= 5 && actor->target_prop_index != -1 && !pref_unreachable(actor->target_prop_index))
    {
      character_charge_properties *charge = character_definition_charge_multi_properties_get(actor_index);
      if (charge)
      {
        if (actor->berserk_ticks < game_seconds_to_ticks_round(charge->berserk_duration))
          actor->berserk_ticks++;
      }
    }
    else
    {
      if (--actor->berserk_ticks <= 0)
        actor_set_berserk(actor_index, false, false);
    }
  }
}

static __forceinline bool actors_actor_vocalize(actor_datum *actor, short vocalization, short speech_type)
{
  bool result = false;
  if (actor->unit_index != -1)
  {
    unit_dialogue_sound_permutation permutation;
    permutation.pitch_range = actor->vocalization_pitch_range;
    permutation.permutation = actor->vocalization_permutation;
    result = unit_dialogue_test_scream(actor->unit_index, -1, vocalization, speech_type, &permutation);
  }
  return result;
}

// @address 0x503b98 ?actor_memory_update@@YAXJ@Z
void actor_memory_update(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  if (actor->ticks_since_combat >= 0 && actor->ticks_since_combat < 0x7fff)
    actor->ticks_since_combat++;
  if (actor->morph.morph_delay_ticks > 0)
    actor->morph.morph_delay_ticks--;
  if (actor->morph.turtle_timer > 0)
    actor->morph.turtle_timer--;
  if (actor->throttle_ticks > 0)
    actor->throttle_ticks--;
  actor_berserk_update(actor_index);
  actor_broken_update(actor_index);
  actor_suppress_combat_update(actor_index);
  actor_cover_update(actor_index);
  if (actor->danger_ticks != -1)
  {
    if (actor->danger_ticks == 127)
      actor->danger_ticks = -1;
    else
      actor->danger_ticks++;
  }
  if (actor->vocalization_delay_ticks > 0)
  {
    actor->vocalization_delay_ticks--;
    if (actor->vocalization_delay_ticks == 0)
    {
      if (actor->pending_vocalization != -1)
        actors_actor_vocalize(actor, actor->pending_vocalization, 2);
      actor->pending_vocalization = -1;
      actor->vocalization_delay_ticks = -1;
      actor->vocalization_pitch_range = 0;
      actor->vocalization_permutation = 0;
    }
  }
}

static __forceinline void actors_swarm_kill(long swarm_index, bool killed_by_player)
{
  long object_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
  while (object_index != -1) {
    creature_datum *creature = (creature_datum *)object_get(object_index);
    swarm_creature_state *link = 0;
    if (creature->creature.link_type == 0)
      link = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
    if (link == 0)
      break;
    if (killed_by_player)
      creature->object.damage_flags |= 0x80;
    else
      creature->object.damage_flags |= 0x40;
    object_wake(object_index);
    object_index = link->next_swarm_creature_index;
  }
}

// @address 0x502650 ?actor_kill@@YAXJ_N0@Z
void actor_kill(long actor_index, bool killed_by_player, bool silent)
{
  actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
  long squad_index = actor->squad_index;
  if (actor->swarm)
  {
    if (actor->swarm_index != -1)
      actors_swarm_kill(actor->swarm_index, killed_by_player);
  }
  else
  {
    object_datum *object = (object_datum *)object_get(actor->unit_index);
    if (killed_by_player)
      object->damage_flags |= 0x80;
    else
      object->damage_flags |= 0x40;
    object_wake(actor->unit_index);
  }
  if (!silent)
  {
    actor_died(actor_index);
    if (squad_index != -1)
      ::squad_update_status(squad_index);
  }
}

// @rdata 0x8202251c 4
static const float k_actors_general_zero = 0.0f;
// @rdata 0x82022dc0 4
static const float k_actors_general_half = 0.5f;
// @rdata 0x82028bec 4
static const float k_actors_general_damage_decay = 0.99f;
static __forceinline const float &actors_general_get_decay() { return k_actors_general_damage_decay; }
static __forceinline long actors_general_seconds_to_ticks_round(float seconds)
{
    float ticks = seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
    return (long)(ticks + 0.5f);
}
// @address 0x5035b8 ?actor_general_update@@YA_NJ@Z
bool actor_general_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = true;
    if (actor->swarm && actor->swarm_index == -1) {
        actor_erase(actor_index, false);
        result = false;
    } else {
        actor->path_refresh_disabled = false;
    }
    if (actor->track_threshold > k_actors_general_zero)
        actor->track_threshold *= actors_general_get_decay();
    if (!actor_runtime_within_structure_bsp(actor_index)) {
        actor->outside_bsp_ticks++;
        if (actor->outside_bsp_ticks >= actors_general_seconds_to_ticks_round(k_actors_general_half)) {
            if (actor->unit_index == -1 || !action_executing(actor->unit_index, _action_resurrect)) {
                actor_set_active(actor_index, false);
                result = false;
            } else {
                actor->outside_bsp_ticks = 0;
            }
        }
    } else {
        actor->outside_bsp_ticks = 0;
    }
    actor->update_valid = result;
    return result;
}


static __forceinline actor_datum *actor_damage_actor_get(long index)
{
  return &((actor_datum *)actor_data->data)[(unsigned short)index];
}
static __forceinline prop_datum *actor_damage_prop_get(long index)
{
  return datum_get<prop_datum>(prop_data, index);
}
// @rdata 0x82026484
static const float k_actor_handle_damage_perceive_seconds = 1.5f;
// @rdata 0x82022514
static const float k_actor_handle_damage_player_seconds = 0.6f;
// @address 0x502128 actor_handle_damage
extern "C" void actor_handle_damage(long actor_index, short damage_type, long damage_definition_index, long responsible_unit_index,
    float damage, const real_vector3d *direction)
{
  if (responsible_unit_index != -1)
  {
    long prop_index = prop_ref_get_by_object_index(actor_index, responsible_unit_index, true, _clump_importance_critical);
    if (prop_index != -1)
    {
      prop_perceive(actor_index, responsible_unit_index, 0, _prop_stimulus_damage,
                    (short)game_seconds_to_ticks_round(k_actor_handle_damage_perceive_seconds),
                    prop_index, -1, -1);
      prop_ref_datum *prop_ref = &((prop_ref_datum *)((prop_ref_datum *)prop_ref_data->data))[(unsigned short)prop_index];
      prop_datum *prop = actor_damage_prop_get(prop_ref->prop_index);
      if (prop_ref->knowledge_state < 1)
      {
        prop_index = -1;
      }
      else if (prop->valid != 0 && pref_unassailable(prop_index))
      {
        actor_datum *actor = actor_damage_actor_get(actor_index);
        if (actor->attached_vehicle_index != -1 && vehicle_get_type(actor->attached_vehicle_index) == 5)
          actor_attach_to_vehicle(actor_index, -1, false);
      }
      if (prop->flag != 0 && prop->valid == 0)
      {
        actor_datum *actor = actor_damage_actor_get(actor_index);
        if (actor->root_vehicle_index != -1)
        {
          object_datum *unit = (object_datum *)object_get(responsible_unit_index);
          long player_index = ((unit_datum *)unit)->unit.player_index;
          if (player_index != -1)
          {
            short found = -1;
            for (long i = 0; i < 4; i++)
            {
              if (tls_ai_players()[i].player_index == player_index)
              {
                found = (short)i;
                break;
              }
            }
            short current = (short)actor->pickup_player;
            if (found == current || found == -1)
            {
              actor->pickup_player = found;
              actor->pickup_wait_ticks += (short)game_seconds_to_ticks_round(k_actor_handle_damage_player_seconds);
            }
          }
        }
      }
    }
    actor_stimulus_damage(actor_index, damage_type, damage_definition_index, prop_index, damage, direction);
  }

  actor_datum *actor = actor_damage_actor_get(actor_index);
  if (damage > 0.0f)
  {
    long last = actor->last_damage_dialogue_time;
    long now = game_time_get();
    if (last == -1 || now - last > game_seconds_integer_to_ticks(1))
    {
      c_dialogue_event event(2, actor->unit_index, responsible_unit_index, -1, damage_type, 0);
      ai_dialogue_event(&event);
      actor->last_damage_dialogue_time = now;
    }
    long unit_index = actor->unit_index;
    if (unit_index != -1)
    {
      if (damage_type < 3 || (damage_type > 4 && damage_type != 9))
      {
        ai_sound_data sound;
        sound.type = 0;
        sound.volume = 1;
        ai_information_data information = {0};
        information.values[0] = 3;
        information.values[1] = -1;
        information.values[2] = responsible_unit_index;
        sound.information_data = information;
        handle_sound(actor_index, unit_index, 0, true, &sound);
      }
      dialogue_emotion(actor_index, 7, (short)game_seconds_integer_to_ticks(2));
    }
  }

  if (damage_type == 11)
  {
    actor_datum *actor = actor_damage_actor_get(actor_index);
    long unit_index = actor->unit_index;
    if (unit_index != -1)
    {
      long child = object_get(unit_index)->first_child_object_index;
      projectile_definition *first = 0;
      short count = 0;
      short threshold = 50;
      if (child != -1)
      {
        do
        {
          object_datum *obj = object_get(child);
          if (obj->object_type == 5)
          {
            projectile_definition *def = TAG_GET('proj', projectile_definition, obj->definition);
            if (actors_input_test_flag(def->targeting_flags, 3))
            {
              if (first == 0)
              {
                threshold = (short)def->supercombine_count;
                first = def;
              }
              count = (short)(count + 1);
            }
          }
          child = obj->next_object_index;
        } while (child != -1);
      }
      if (count >= threshold)
        stimulus_register(actor_index, _behavior_stimulus_super_detonation, 1, 3);
    }
  }
}

// @address 0x72fbf8 ?g_updating_actor_index@@3JA
long g_updating_actor_index = -1;

// @address 0x72fbfc ?g_movement_mode_names@@3PAJA
long g_movement_mode_names[9] = { 0x89, 0x88, 0x8a, 0x91, 0x47, 0xba, 0x33, 0x8b, 0x8c };

// @address 0xad7f0 ?actors_update@@YAXXZ
void actors_update()
{
    short budget = 10;
    short last = 0;
    ai_globals_type *state = ai_globals;
    actor_iterator iterator;
    actor_datum *actor;
    state->firing_positions_evaluated = false;
    state->pad_04 = false;
    state->timeslice_given_a = false;
    state->timeslice_given_b = false;
    ai_globals->maximum_ticks_a = ai_globals->worst_ticks_a;
    state->worst_ticks_a = 0;
    ai_globals->maximum_ticks_b = ai_globals->worst_ticks_b;
    state->worst_ticks_b = 0;
    if (ai_globals->throttled)
        budget = 5;
    actors_actor_iterator_new(&iterator, true);
    actor = actor_iterator_next(&iterator);
    if (actor != 0) {
        do {
            actor->perception_timeslice = false;
            actor = actor_iterator_next(&iterator);
        } while (actor != 0);
    }
    actors_actor_iterator_new(&iterator, true);
    actor = actor_iterator_next(&iterator);
    if (actor != 0) {
        do {
            if (budget <= 0)
                break;
            short index = (short)iterator.index;
            if (index > state->last_actor_index) {
                budget--;
                actor->perception_timeslice = true;
            }
            last = index;
            actor = actor_iterator_next(&iterator);
        } while (actor != 0);
    }
    if (budget > 0) {
        actors_actor_iterator_new(&iterator, true);
        actor = actor_iterator_next(&iterator);
        if (actor != 0) {
            do {
                if (budget <= 0)
                    break;
                short index = (short)iterator.index;
                if (index > state->last_actor_index)
                    break;
                budget--;
                actor->perception_timeslice = true;
                last = index;
                actor = actor_iterator_next(&iterator);
            } while (actor != 0);
        }
        if (budget > 0)
            last = -1;
    }
    state->last_actor_index = last;
    actors_actor_iterator_new(&iterator, true);
    actor = actor_iterator_next(&iterator);
    if (actor != 0) {
        do {
            if (actor->deleted) {
                actor_erase(iterator.index, false);
            } else if ((short)actor->behavior_mode > 0) {
                long actor_index = iterator.index;
                g_updating_actor_index = actor_index;
                if (actor_general_update(actor_index))
                    actor_input_update(actor_index);
                g_updating_actor_index = -1;
            }
            actor = actor_iterator_next(&iterator);
        } while (actor != 0);
    }
    actors_actor_iterator_new(&iterator, true);
    actor = actor_iterator_next(&iterator);
    if (actor != 0) {
        do {
            if ((short)actor->behavior_mode > 0)
                actor_update(iterator.index);
            actor = actor_iterator_next(&iterator);
        } while (actor != 0);
    }
    actors_actor_iterator_new(&iterator, false);
    actor = actor_iterator_next(&iterator);
    if (actor != 0) {
        do {
            long actor_index = iterator.index;
            if ((short)actor->behavior_mode <= 0 || !actor->active)
                cs_actor_update(actor_index);
            if (!actor->active && actor->erase_time != -1 && actor->erase_time > TLS_GAME_TIME_GLOBALS->time)
                actor_erase(actor_index, false);
            actor = actor_iterator_next(&iterator);
        } while (actor != 0);
    }
}

// @address 0x5002b8 actor_new_child_from_object
extern "C" long actor_new_child_from_object(
    long object_index, long swarm_index, long character_index, unsigned short initial_state, short movement_mode, real_point3d *origin)
{
    long result = -1;
    long squad_index = -1;
    bool disposable = false;
    bool inherit_velocity = false;
    object_datum *object = object_get(object_index);
    actor_starting_location_definition location;
    starting_location_new(&location);
    location.position.set(origin);
    long requested_state = (short)initial_state;
    location.facing.yaw = 0.0f;
    location.facing.pitch = 0.0f;
    location.initial_movement_mode = movement_mode;
    if (requested_state <= 0)
        requested_state = 3;
    long parent_actor_index = ((biped_datum *)object)->unit.actor_index;
    location.initial_state = requested_state;
    if (parent_actor_index != -1)
    {
        actor_datum *parent_actor = datum_get<actor_datum>(actor_data, parent_actor_index);
        squad_index = parent_actor->squad_index;
        disposable = parent_actor->disposable;
    }
    else if ((1L << object->object_type) & 1)
        squad_index = ((biped_datum *)object)->biped.squad_index;
    real_vector3d linear_velocity;
    real_point3d center;
    real_vector3d angular_velocity;
    if ((1L << object->object_type) & 1)
    {
        object_get_center_of_mass(object_index, &center);
        inherit_velocity = object_get_early_mover_local_space_velocity(object_index, &center, &linear_velocity, &angular_velocity, true, false);
    }
    if (swarm_index != -1)
    {
        character_definition *definition = TAG_GET('char', character_definition, character_index);
        long creature_definition_index = definition->creature.index;
        if (creature_definition_index != -1)
        {
            long creature_index = swarm_create_creature(swarm_index, character_index, creature_definition_index, &location, false, true);
            result = datum_get<swarm_datum>(g_swarm_data, swarm_index)->actor_index;
            if (inherit_velocity)
                creature_accelerate(creature_index, &linear_velocity);
        }
    }
    else
    {
        result = actor_place(character_index, (short)squad_index, -1, &location, 4);
        if (result != -1)
        {
            actor_datum *actor = datum_get<actor_datum>(actor_data, result);
            if (actor->squad_index == -1)
            {
                actor->squad_seek_ticks = TLS_GAME_TIME_GLOBALS->ticks_per_second;
                actor->squad_seek_attempts = 0;
                actor->seeking_squad = true;
                actor_set_active(result, true);
            }
            actor->disposable = disposable;
            if (inherit_velocity && actor->swarm && actor->swarm_index != -1)
            {
                long creature_index = datum_get<swarm_datum>(g_swarm_data, actor->swarm_index)->first_object_index;
                while (creature_index != -1)
                {
                    creature_datum *creature = (creature_datum *)object_get(creature_index);
                    swarm_creature_state *link = creature->creature.link_type == 0
                        ? (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset) : 0;
                    creature_accelerate(creature_index, &linear_velocity);
                    if (!link)
                        break;
                    creature_index = link->next_swarm_creature_index;
                }
            }
        }
    }
    return result;
}


static __forceinline void actors_new_clear_layer(actor_datum *actor, short layer)
{
    c_behavior_state *state = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
    state->behavior_index = -1;
    state->start_time = -1;
    state->execution_state = 0;
    actor->state_offsets[layer] = -1;
}
static __forceinline invitation_data *actors_new_get_invitation(long actor_index, short invitation_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    return &actor->invitations[invitation_index];
}
static __forceinline void actors_new_clear_invitation(invitation_data *invitation)
{
    invitation->type = -1;
    invitation->time = 0;
    invitation->handle = -1;
    invitation->slot = -1;
    invitation->mode = 0;
}
static __forceinline void actors_new_reset_activity(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->activity_index = -1;
    actor->activity_variant = -1;
    actor->activity_parameter = -1;
}
static __forceinline void actors_new_clear_action(c_ai_action *action)
{
    action->type = 0;
    action->point_valid = false;
    action->b5 = false;
    action->ticks = 0;
    action->posture_name = 0;
    action->point.set(global_origin3d);
    action->direction.clear();
}
static __forceinline void actors_new_clear_interaction(c_interaction_target *target)
{
    memset(target, 0, sizeof(*target));
    target->object_index = 0xffff;
    target->type = 0;
}
static __forceinline void actors_new_clear_pending_form(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->morph.pending_form = -1;
    actor->morph.pending_form_priority = -1;
    actor->morph.immediate_morph = false;
    actor->morph.force_morph = false;
}
static __forceinline e_pureform actors_new_get_current_form(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    e_pureform form = _pureform_none;
    if ((1 << actor->type) & 0x700000)
        form = character_get_form(actor->character_index);
    return form;
}

// @address 0x4fd748
long actor_new(long character_index)
{
    long actor_index = -1;
    if (character_index != -1)
    {
        actor_index = datum_new((s_data_array *)actor_data);
        if (actor_index != -1)
        {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            character_general_properties *properties = character_definition_general_properties_get(character_index);
            actor->character_index = character_index;
            actor->swarm = (properties->flags & 1) != 0;
            actor->type = (short)properties->type;
            actor->unit_index = -1;
            actor->swarm_index = -1;
            actor->squadless = false;
            actor->squad_index = -1;
            actor->starting_location_index = -1;
            actor->original_squad_index = -1;
            actor->squad_seek_ticks = -1;
            actor->seeking_squad = false;
            actor->squad_seek_attempts = 0;
            actor->actively_controlled = true;
            actor->active = false;
            actor->disposable = false;
            actor->deactivation_ticks = -1;
            actor->attach_time = -1;
            actor->creation_time = game_time_get();
            actor->weight = 0.0f;
            actor->activation_ticks = -1;
            actor->outside_bsp_ticks = 0;
            actor->equipment_condition_flags = 0;
            actor->erase_time = -1;
            actor->pathfinding_ticks_a = 0;
            actor->pathfinding_ticks_b = 0;
            actor->first_prop_ref_index = -1;
            actor->track_threshold = 0.0f;
            actor->clump_index = -1;
            actor->highest_status_refresh_ticks = 0;
            actor->highest_state_refresh_ticks = 0;
            actor->equipment_index = -1;
            actor->situation.best_prop_index = -1;
            actor->firing_position_reference = FP_REF_NONE;
            actor->flying = false;
            actor->goal_area_valid = false;
            actor->goal_status = 0;
            actor->dynamic_firing_set_index = -1;
            actor->support_object_index = -1;
            actor->support_index = -1;
            actor->discarded_position_valid = false;
            actor_clear_discarded_firing_positions(actor_index, false);
            actor->behavior_mode = 3;
            actor->combat_status = 1;
            actor->combat_status_time = 0;
            actor->searching = false;
            actor->berserking = false;
            actor->search_allowed = false;
            actor->use_object = (properties->flags & 2) != 0;
            actor->search_active = false;
            actor->blind = false;
            actor->deaf = false;
            actor->search_flag = false;
            actor->deployed_turret = false;
            actor->suppress_combat = false;
            actor->broken = false;
            actor->cover_flags = 0;
            actor->weapon_up = false;
            actor->avoidance_disabled = false;
            actor->berserk_ticks = -1;
            actor->cover_index = -1;
            actor->danger_ticks = -1;
            memset(actor->behavior_state_data, 0, sizeof(actor->behavior_state_data));
            actors_new_clear_layer(actor, 0);
            actor->layer_count = -1;
            for (short layer = 0; layer < 8; ++layer)
                actor->state_offsets[layer] = -1;
            if (!actor->swarm)
                behavior_change(actor_index, 0, 1, 0);
            else
                behavior_change(actor_index, 0, 142, 0);
            for (short index = 0; index < 14; ++index)
                actor->child_test_times[index] = -1;
            for (short index = 0; index < 5; ++index)
            {
                s_stimulus *stimulus = &actor->stimuli[index];
                stimulus->type = -1;
            }
            actor->stimulus_flags = 0;
            for (short invitation_index = 0; invitation_index < 4; ++invitation_index)
                actors_new_clear_invitation(actors_new_get_invitation(actor_index, invitation_index));
            actor->command_index = -1;
            actor->command_script_index = -1;
            actor->pathfinding_sector.clear();
            actor->root_vehicle_index = -1;
            actor->controlled_unit_index = -1;
            actor->player_weight_valid = false;
            actor->driver = false;
            actor->controlling_other_unit = false;
            actor->passenger = false;
            actor->vehicle_driver_type = 0;
            actor->movement_3d = false;
            actors_new_reset_activity(actor_index);
            actor->danger_zone_tracking_start_time = -1;
            actor->danger_zone_tracking_last_time = -1;
            memset(&actor->activity_scariness, 0,
                offsetof(actor_datum, firing_position_locked) - offsetof(actor_datum, activity_scariness));
            actor->charge_deactivate_time = -1;
            actor->pathfinding_type = false;
            actors_new_clear_action(&actor->action);
            actors_path_clear(actor_index);
            actor->flags = 0;
            actor->flags |= FLAG(15);
            actor->flags |= FLAG(11);
            actor->flags |= FLAG(27);
            actor->weapon_flags = 0;
            actor->target_state = 1;
            actor->aim_ticks = 0;
            actor->aim_target_index = -1;
            actor->aim_error = 0.0f;
            actor->firing_count = 0;
            actor->aim_timer = 0;
            actor->aim_field910 = 0;
            actor->firing_ticks = 0;
            actor->attack_time = 0;
            actor->current_fire_target_prop_index = -1;
            actor->grenade_timer = 0;
            actor->aim_field9c4 = -1;
            actor->target_unit = -1;
            actor->aim_field9a8 = 0.0f;
            actor->burst_length = 1.0f;
            actor->attention_ticks = 0;
            actor->aim_object_index = -1;
            actor->forward8ec = *global_forward3d;
            actor->forward8e0 = *global_forward3d;
            actor->forward8f8 = *global_forward3d;
            actor->aim_flags = 0;
            actor->move_freeze_ticks = 0;
            actor->movement_obstacle_index = -1;
            actor->movement_obstacle_ticks = 0;
            actor->movement_object_index = -1;
            actor->attention_direction.clear();
            actor->secondary_aim_direction.clear();
            actor->previous_aim_direction.clear();
            actor->attention_object_index = -1;
            actor->attention_prop_index = -1;
            actor->attention_actor_index = -1;
            memset(&actor->vehicle_turn, 0, sizeof(actor->vehicle_turn));
            actor->vehicle_turn.index = -1;
            actor->movement_point_ticks = 0;
            actor->movement_point = *global_origin3d;
            actor->movement_target_index = -1;
            actor->isolation_level = 0;
            actor->movement_target_valid = false;
            actor->movement_target = *global_origin3d;
            actor->movement_index = -1;
            actor->attack_flags = 0;
            actor->attack_object_index = -1;
            actor->target_flags = 0;
            actor->scripted_target_index = -1;
            actor->prefer_target_team = -1;
            actor->target_prop_index = -1;
            actor->target_marker_name = 0;
            actor->target_age = -1;
            actor->target_actor_index = -1;
            actor->target_object_index = -1;
            actor->target_ticks = -1;
            actor->unowned_target_salient = false;
            actor->peer_death_time = -1;
            actor->ticks_since_combat = 0;
            actor->last_damage_dialogue_time = -1;
            actor->dialogue_object_index = -1;
            actor->dialogue_object_distance = 0.0f;
            actor->dialogue_type = -1;
            for (short index = 0; index < 4; ++index)
            {
                actor->dialogue_entries[index].index = -1;
                actor->dialogue_entries[index].time = -1;
            }
            actor->had_shield = false;
            actor->throttle_ticks = 0;
            actor->dialogue_point.point = *global_origin3d;
            actor->dialogue_point.location[0] = -1;
            actor->dialogue_point.location[1] = -1;
            actor->dialogue_sector.clear();
            actor->pending_vocalization = -1;
            actor->vocalization_delay_ticks = 0;
            actor->vocalization_pitch_range = 0;
            actor->vocalization_permutation = 0;
            for (short index = 0; index < 3; ++index)
                actor->recent_obstacles[index].clear();
            actors_new_clear_interaction(&actor->interaction_target);
            actors_new_clear_pending_form(actor_index);
            actor->morph.last_morph_time = -1;
            actor->morph.morph_delay_ticks = 0;
            actor->morph.distance_damage_ticks = 0;
            actor->morph.distance_damage_reset_ticks = 0;
            actor->morph.current_form = (short)actors_new_get_current_form(actor_index);
            actor->morph.morph_locked = true;
            actor->morph.force_morph = false;
            actor->morph.turtle_timer = 0;
            actor->attached_vehicle_index = -1;
            actor->vehicle_seat_index = -1;
            actor->destination_unit_index = -1;
            actor->vehicle_desire = 0;
            actor->exit_timer = 0;
            actor->reserved_timer = 0;
            actor->attach_timer = -1;
            actor->vehicle_entry_flags = 0;
            actor->exited_vehicle_index = -1;
            actor->exited_seat_index = -1;
            actor->detach_timer = -1;
            actor->seat_attach_timer = (short)game_seconds_integer_to_ticks(10);
            actor->seat_reentry_timer = -1;
            actor->turtle_time = 0;
            actor->boarding_dialogue_active = false;
            actor->player_seen = false;
            actor->boarding_dialogue_ticks = 0;
            actor->boarding_dialogue_player_index = -1;
            actor->nearest_object_distance = 0.0f;
            actor->pickup_player_signed = -1;
            actor->pickup_wait_ticks = 0;
            actor->pickup_state = 0;
            actor->player_visible = false;
            actor->charge_timer = 0;
            actor->player_visible_last = false;
            actor->player_visibility_ticks = 0;
        }
    }
    return actor_index;
}


static __forceinline void actor_obstacles_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    for (short index = 0; index < 3; index++)
    {
        c_recent_obstacle *obstacle = &actor->recent_obstacles[index];
        if (obstacle->valid())
        {
            obstacle->ticks--;
            if (obstacle->ticks <= 0)
                obstacle->clear();
        }
    }
}

static __forceinline void actor_clear_control(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    memset(&actor->look_mode, 0, offsetof(actor_datum, action) - offsetof(actor_datum, look_mode));
    actor->control_type = -1;
    actor->leap_prop_index = -1;
}

static __forceinline void actor_clear_output(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->output_facing = actor->facing_vector;
    actor->output_aiming = actor->aiming_vector;
    actor->output_looking = actor->looking_vector;
    actor->control_flags = 0;
    actor->output_primary_trigger = 0.0f;
    actor->output_secondary_trigger = 0.0f;
    actor->output_throttle = *global_zero_vector3d;
    actor->output_weapon_index = -1;
    actor->output_animation_state = 138;
}


static __forceinline void behavior_control(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    short behavior_index = actor_behavior_index_get(actor, actor->layer_count);
    Behavior_header *behavior = global_behavior_info[behavior_index].header;
    if (behavior->type == 2 && static_cast<Primitive_behavior *>(behavior)->control != 0)
        static_cast<Primitive_behavior *>(behavior)->control(actor_index, actor_behavior_state_get(actor, actor->layer_count));
}

// @address 0x503288 ?actor_update@@YAXJ@Z
void actor_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    g_updating_actor_index = actor_index;
    if (actor->update_valid)
    {
        mark_executing_behaviors(actor_index);
        actor_get_timeslices(actor_index);
        actor_perception_update(actor_index);
        actor_update_squad_status(actor_index);
        actor_dynamic_firing_positions_update(actor_index);
        actor_perception_update_best_target(actor_index);
        actor_update_emotion(actor_index);
        ai_player_update_boarding_dialogue(actor_index);
        ai_player_update_vehicle_boarding(actor_index);
        actor_memory_update(actor_index);
        actor_obstacles_update(actor_index);
        actor_vehicle_update(actor_index);
        actor_clear_control(actor_index);
        cs_actor_update(actor_index);
        actor_invitations_update(actor_index);
        action_selection(actor_index);
        actor_clear_output(actor_index);
        behavior_control(actor_index);
        const unsigned long &is_swarm = actor->swarm;
        if (is_swarm)
        {
            if (actor->swarm_index != 0)
                swarm_update(actor_index);
        }
        else
        {
            cs_apply_persistent_state(actor_index);
            actor_destination_update(actor_index);
            actor_check_firing_position(actor_index);
            actor_move_update(actor_index);
            actor_look_update(actor_index);
            actor_combat_update(actor_index);
            actor_unit_control(actor_index);
        }
        actor_stimuli_update(actor_index);
    }
    g_updating_actor_index = -1;
}


static __forceinline long actors_place_object_new_from_scenario(long type, long index, s_scenario_object *object, s_tag_block *palette,
    bool a, bool b)
{
    return object_new_from_scenario_internal(type, (short)index, object, palette, -1, true, a, false);
}
static __forceinline character_definition *actors_place_character_get(long index)
{
    return (character_definition *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)].definition;
}
static __forceinline s_tag_block *actors_place_equipment_get(long index)
{
    s_tag_block *result = 0;
    while (index != -1) {
        character_definition *character = actors_place_character_get(index);
        if (character->equipment_properties.count > 0) {
            result = &character->equipment_properties;
            break;
        }
        index = character->parent_character.index;
    }
    return result;
}
static __forceinline bool actors_place_motor(long index, e_motor_program program)
{
    biped_datum *unit = (biped_datum *)object_get(index);
    c_motor_program_state *state = (c_motor_program_state *)((char *)unit + unit->unit.motor_program.offset);
    return state->program == program || state->set_program(index, program);
}
static __forceinline void actors_place_set_physics_flag(unsigned long *flags, long bit, bool value)
{
    *flags = (*flags & ~(1UL << bit)) | ((unsigned long)value << bit);
}
static __forceinline bool actors_place_set_override(
    long index, const real_point3d *point, const real_vector3d *direction, float distance, short ticks, const bool &enabled)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, index);
    real_plane3d *plane = &actor->persistent_movement.plane;
    plane3d_from_point_and_normal(plane, point, direction);
    actor->persistent_movement.tolerance = distance;
    actor->persistent_movement.ticks = ticks;
    unsigned long enabled_bits = enabled;
    actor->flags = (actor->flags & ~8UL) | (_rotl(enabled_bits, 5) & 8UL);
    actor->flags = (actor->flags & ~32UL) | (_rotl(enabled_bits, 5) & 32UL);
    return true;
}
static __forceinline long actors_place_inventory_weapon(long unit_index, short inventory_index)
{
    biped_datum *unit = (biped_datum *)object_get(unit_index);
    // match: 0x4ff238: hoists the object header/datum load above the inventory_index != -1 test
    __assume(unit != 0);
    long result = -1;
    if (inventory_index != -1) result = unit->unit.weapon_object_indices[inventory_index];
    return result;
}
static __forceinline real_point3d *actors_place_origin(real_point3d *(*const &get)(long, real_point3d *), const long &index,
    action_request *request)
{
    request->action = 28;
    request->motion.value = 0;
    return get(index, &request->motion.position);
}
static unsigned long *actors_place_random_seed_address()
{
    return TLS_RANDOM_SEED;
}
static unsigned long *actors_place_use_random_seed(unsigned long *seed)
{
    TLS_RANDOM_SEED = seed;
    return seed;
}
static __forceinline long actors_place_game_mode(const game_globals_storage *globals)
{
    return globals->options.game_mode;
}
// @address 0x4ff238 ?actor_place@@YAJJFFPAUactor_starting_location_definition@@F@Z
long actor_place(long character_index, short squad_index, short fire_team_index, actor_starting_location_definition *starting_location,
    short flags)
{
    character_definition *character = actors_place_character_get(character_index);
    character_general_properties *general = character_definition_general_properties_get(character_index);
    squad_definition *squad = squad_index == -1 ? 0 : &((squad_definition *)global_scenario->squads.address)[(unsigned short)squad_index];
    fire_team_definition *fire_team = fire_team_index != -1 && squad ? &squad->fire_teams[fire_team_index] : 0;
    long actor_index = -1;
    long unit_index = -1;
    long swarm_index = -1;
    if ((flags & 1) && squad && squad_index != -1 && fire_team) {
        bool upgrade = true;
        while (upgrade && character->major_character.index != -1) {
            bool random = false;
            float chance = 0.0f;
            upgrade = false;
            major_upgrade_chance(character_index, fire_team->upgrade, &upgrade, &random, &chance);
            if (random) upgrade = squad_consider_major_upgrade(squad_index, chance);
            if (upgrade) {
                character_index = character->major_character.index;
                character = actors_place_character_get(character_index);
                general = character_definition_general_properties_get(character_index);
            }
        }
    }
    do { if (general) {
        long equipment_index = -1;
        long attachment = -1;
        bool initially_asleep = false;
        bool force_airborne = false;
        short movement_mode = starting_location->movement_mode;
        if (squad) initially_asleep = (squad->flags & 8) != 0;
        if (general->flags & 1) {
            swarm_index = swarm_place(character_index, starting_location, flags);
        } else {
            if (starting_location->giant_index != -1) {
                scenario *scenario = global_scenario;
                s_scenario_giant *giant = TAG_BLOCK_GET_ELEMENT(&scenario->giants, starting_location->giant_index, s_scenario_giant);
                unit_index = giant->object_identifier.find_object_index();
                if (unit_index != -1) {
                    if (((biped_datum *)object_get(unit_index))->unit.actor_index != -1) unit_index = -1;
                } else {
                    unit_index = actors_place_object_new_from_scenario(
                        12, starting_location->giant_index, (s_scenario_object *)giant, &scenario->giant_palette, false, false);
                }
            } else if (starting_location->biped_index != -1) {
                scenario *scenario = global_scenario;
                s_scenario_biped *biped = TAG_BLOCK_GET_ELEMENT(&scenario->bipeds, starting_location->biped_index, s_scenario_biped);
                unit_index = biped->object_identifier.find_object_index();
                if (unit_index != -1) {
                    biped_datum *unit = (biped_datum *)object_get(unit_index);
                    bool dead = (unit->object.damage_flags >> 2) & 1;
                    if (dead) {
                        action_request request = {};
                        bool marker = false;
                        ai_resurrect_get_marker(unit_index, 0, &marker);
                        request.action = 5;
                        request.resurrect.ticks = TLS_GAME_TIME_GLOBALS->ticks_per_second;
                        request.resurrect.type = marker ? 3 : 4;
                        if (!action_submit(unit_index, &request)) break;
                    }
                }
            } else if (character->unit.index != -1) {
                character_movement_properties *movement = character_definition_movement_properties_get(character_index);
                object_placement_data placement;
                real_point3d position;
                real_vector3d up;
                real_vector3d forward;
                bool tilted = false;
                object_placement_data_new(&placement, character->unit.index, -1, 0);
                ai_point_get_position(&starting_location->position, &position);
                vectors3d_from_euler_angles3d(&forward, &up, (real_euler_angles3d *)&starting_location->facing);
                placement.position = position;
                placement.forward = forward;
                placement.up = up;
                if (starting_location->actor_variant == 0) placement.variant = actor_select_character_variant(character_index);
                else placement.variant = starting_location->actor_variant;
                placement.placement_flag = (flags & 2) != 0;
                if (placement.up.k < 0.70710677f && movement &&
                    ((movement->flags & (1 << 6)) || (movement->flags & (1 << 7)) || (movement->flags & (1 << 9))))
                    tilted = true;
                if (!tilted) {
                    placement.up = *global_up3d;
                    placement.forward.k = 0.0f;
                    if (normalize3d(&placement.forward) == 0.0f) placement.forward = *global_forward3d;
                }
                unit_index = object_new(&placement);
                if (unit_index != -1) {
                    attachment = starting_location->position.attachment_index;
                    if (attachment >= 0 && attachment < global_scenario->object_reference_frames.count) {
                        s_scenario_object_reference_frame *attached = TAG_BLOCK_GET_ELEMENT(
                            &global_scenario->object_reference_frames, attachment, s_scenario_object_reference_frame);
                        s_attachment_state *attachment_state = &g_ai_reference_frame_data[attachment];
                        long early_mover = attachment_state->object_index;
                        if (early_mover != -1 && attached->node_index != -1 && object_early_mover_is_owner(early_mover, true)) {
                            biped_datum *unit = (biped_datum *)object_get(unit_index);
                            if (object_early_mover_local_space_index(early_mover) != -1) {
                                unit->object.local_physics_object_index = early_mover;
                                actors_place_set_physics_flag(&unit->object.physics_flags, 1, true);
                                actors_place_set_physics_flag(&unit->object.physics_flags, 2, true);
                                actors_place_set_physics_flag(&unit->object.physics_flags, 3, true);
                            }
                        }
                    }
                    if (tilted) {
                        if (movement->flags & (1 << 6)) { movement_mode = 2; force_airborne = true; }
                        else if (movement->flags & (1 << 7)) movement_mode = 1;
                    }
                }
            }
            if (unit_index != -1) {
                s_tag_block *equipment = actors_place_equipment_get(character_index);
                scenario *scenario = global_scenario;
                long primary_palette = -1;
                long primary = -1;
                long equipment_palette = -1;
                long equipment_definition = -1;
                long secondary_palette = -1;
                long secondary = -1;
                short grenade_type = -1;
                actor_select_character_voice(character_index, unit_index);
                if (starting_location->equipment_palette_index != -1) equipment_palette = starting_location->equipment_palette_index;
                else if (fire_team && fire_team->equipment != -1) equipment_palette = fire_team->equipment;
                if (equipment_palette >= 0 && equipment_palette < scenario->equipment_palette.count) {
                    equipment_definition = TAG_BLOCK_GET_ELEMENT(&scenario->equipment_palette, equipment_palette,
                        scenario_object_palette_entry)->object.index;
                    if (equipment) {
                        for (short i = 0; i < equipment->count; ++i) {
                            if (TAG_BLOCK_GET_ELEMENT(equipment, i, character_equipment_properties)->equipment.index == equipment_definition)
                            {
                                equipment_index = i;
                                break;
                            }
                        }
                    }
                } else if (equipment) {
                    float total = 0.0f;
                    for (short i = 0; i < equipment->count; ++i) {
                        character_equipment_properties *entry = TAG_BLOCK_GET_ELEMENT(equipment, i, character_equipment_properties);
                        total += entry->weight;
                    }
                    if (total > 0.0f) {
                        unsigned long *seed = TLS_RANDOM_SEED;
                        float pick = _real_random_range(seed, __FILE__, __FUNCTION__, __LINE__, 0.0f, total);
                        for (short i = 0; i < equipment->count; ++i) {
                            character_equipment_properties *entry = TAG_BLOCK_GET_ELEMENT(equipment, i, character_equipment_properties);
                            pick -= entry->weight;
                            if (pick <= 0.0f) { equipment_definition = entry->equipment.index; equipment_index = i; break; }
                        }
                    }
                }
                if (equipment_definition == -1) equipment_index = -1;
                if (starting_location->primary_weapon != -1) primary_palette = starting_location->primary_weapon;
                else if (fire_team && fire_team->primary_weapon != -1) primary_palette = fire_team->primary_weapon;
                if (primary_palette >= 0 && primary_palette < scenario->weapon_palette.count) {
                    scenario_object_palette_entry *palette = TAG_BLOCK_GET_ELEMENT(&scenario->weapon_palette, primary_palette,
                        scenario_object_palette_entry);
                    primary = palette->object.index;
                }
                if (starting_location->secondary_weapon != -1) secondary_palette = starting_location->secondary_weapon;
                else if (fire_team && fire_team->secondary_weapon != -1) secondary_palette = fire_team->secondary_weapon;
                if (secondary_palette >= 0 && secondary_palette < scenario->weapon_palette.count) {
                    long palette_offset = secondary_palette * (long)sizeof(scenario_object_palette_entry);
                    scenario_object_palette_entry *palette
                        = (scenario_object_palette_entry *)tag_block_get_element_with_size(&scenario->weapon_palette,
                        palette_offset,
                        1);
                    secondary = palette->object.index;
                }
                long grenade = starting_location->grenade_type - 1;
                if (grenade != -1) grenade_type = (short)grenade;
                else if (fire_team && fire_team->grenade - 1 != -1) grenade_type = fire_team->grenade - 1;
                else if (general->grenade_type > 0) grenade_type = general->grenade_type - 1;
                // match: 0x4ff238: without the const-reference temp r15/r16 swap (cmpwi/lwz -244 at 0x824fff70, mr r4 at 0x82500118)
                const long &customize_equipment_definition = (long)_rotl(equipment_definition, 0);
                actor_customize_unit(character_index, unit_index, primary, secondary, grenade_type, customize_equipment_definition);
            }
        }
        if (unit_index != -1) actor_index = actor_create(unit_index, character_index, squad_index, initially_asleep);
        else if (swarm_index != -1) actor_index = actor_create_internal(-1, swarm_index, character_index, squad_index, initially_asleep);
        if (actor_index != -1) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            if (unit_index != -1) {
                long child_index = object_get(unit_index)->first_child_object_index;
                while (child_index != -1) {
                    object_datum *child = object_get(child_index);
                    if ((1L << child->object_type) & 0x1003) {
                        unit_definition *definition = TAG_GET('unit', unit_definition, child->definition);
                        if (definition->spawned_actor.index != -1) actor_create(child_index, definition->spawned_actor.index,
                            squad_index, initially_asleep);
                    }
                    child_index = child->next_object_index;
                }
            }
            if (starting_location->initial_movement_distance > 0.0f) {
                real_point3d position;
                real_vector3d direction;
                ai_point_get_position(&starting_location->position, &position);
                vector3d_from_euler_angles2d(&direction, &starting_location->facing);
                actors_place_set_override(actor_index, &position, &direction, starting_location->initial_movement_distance, -1, true);
            }
            if (actor->unit_index != -1) {
                switch (movement_mode) {
                case 1:
                    actors_place_motor(unit_index, _motor_program_climb);
                    if (object_header_get(unit_index)->object_type == 0) {
                        biped_datum *unit = (biped_datum *)object_get(unit_index);
                        real_point3d position;
                        real_vector3d forward, up;
                        object_get_origin(actor->unit_index, &position);
                        object_get_orientation(actor->unit_index, &forward, &up);
                        unit->biped.physics.get_mode_ground()->set_ground_plane(&forward);
                        object_set_position_direct(unit_index, &position, &forward, &up, 0, false);
                    }
                    break;
                case 2:
                    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
                    if (movement && (movement->flags & (1 << 9))) {
                        actors_place_motor(unit_index, _motor_program_flying);
                        if (force_airborne) {
                            action_request request = {};
                            actors_place_origin(object_get_origin, actor->unit_index, &request);
                            object_get_orientation(actor->unit_index, 0, &request.motion.velocity);
                            action_submit(unit_index, &request);
                        }
                    }
                    break;
                }
            }
            if (actor->unit_index != -1) {
                long weapon = actors_place_inventory_weapon(
                    actor->unit_index,
                    ((biped_datum *)((s_object_header *)object_header_data->data)
                        [DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->unit_index)].datum)->unit.current_weapon_index);
                if (weapon != -1) {
                    weapon_definition *definition =
                        TAG_GET('weap', weapon_definition, object_get(weapon)->definition);
                    if (definition->deployed_vehicle != -1) actor->deployed_turret = true;
                }
            }
            if (actor->unit_index != -1) {
                long activity = 0;
                if (starting_location->activity_name) activity = starting_location->activity_name;
                else if (fire_team && fire_team->activity) activity = fire_team->activity;
                if (activity) {
                    short variant = -1;
                    bool forced = false;
                    if (starting_location->activity_variant != -1) { variant = starting_location->activity_variant; forced = true; }
                    else if (fire_team && fire_team->activity_variant != -1) variant = fire_team->activity_variant;
                    short activity_index = -1, variation_index = -1;
                    if (activity_index_get_by_name(activity, &activity_index, &variation_index))
                        actor_activity_set(actor_index, activity_index, variation_index, variant, forced);
                }
            }
            actor->equipment_index = equipment_index;
            if (equipment_index != -1) {
                s_tag_block *equipment = actors_place_equipment_get(character_index);
                character_equipment_properties *entry = TAG_BLOCK_GET_ELEMENT(equipment, equipment_index, character_equipment_properties);
                for (short i = 0; i < entry->condition_count; ++i) {
                    character_equipment_condition *drop = &entry->conditions[i];
                    float chance;
                    long difficulty = (short)(actors_place_game_mode(game_globals) == 1 ? game_globals->options.campaign_difficulty : 1);
                    switch (difficulty) {
                    case 0:
                    case 1:
                        chance = drop->normal_chance;
                        break;
                    case 2:
                        chance = (drop->heroic_chance + drop->normal_chance) * 0.5f;
                        break;
                    case 3:
                        chance = drop->heroic_chance;
                        break;
                    default:
                        // match: 0x4ff238: without it chance gets a frame slot and starting_location moves from its 0x3ac spill to r14
                        __assume(0);
                    }
                    chance = MIN(MAX(chance, 0.0f), 1.0f);
                    if (_real_random(TLS_RANDOM_SEED, __FILE__, __FUNCTION__, __LINE__) > chance) actor->equipment_condition_flags |= 1 << drop->condition;
                }
            }
            if (actor->unit_index != -1 && (starting_location->initial_movement_mode & 8)) object_set_hidden(actor->unit_index, true);
            if (actor->unit_index != -1 && (general->flags & (1 << 7))) actor_set_mission_critical(actor_index, character_index);
            if (actor->unit_index != -1 && actor->shield_vitality > 0.0f && actor_test_movement_type(actor_index, 139)) actor->had_shield = true;
            if (unit_index != -1) simulation_action_actor_create(unit_index);
        } else {
            if (unit_index != -1) object_delete(unit_index);
            else if (swarm_index != -1) swarm_delete(swarm_index);
        }
    } } while (0);
    return actor_index;
}

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_data_allocator> t_g_actor_data_allocator;
t_g_actor_data_allocator g_actor_data_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_reference_frame_data_allocator> t_g_ai_reference_frame_data_allocator;
t_g_ai_reference_frame_data_allocator g_ai_reference_frame_data_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_actor_firing_position_hash_allocator> t_g_actor_firing_position_hash_allocator;
t_g_actor_firing_position_hash_allocator g_actor_firing_position_hash_allocator;
