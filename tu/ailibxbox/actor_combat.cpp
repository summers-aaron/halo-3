#include "ai/ai_math_constants.h"
// ailibxbox/actor_combat.obj: from the records 0xbd378 0xbf180 0xbf268 0xc01a8 0xc05e8 0xc0dd8 0xc16f8 0xc1b00 0xc4510 0x51c120 0x51c230 0x51c360 0x51c5c8 0x51c6d0 0x51c8a0 0x51cf68 0x51d068 0x51d1e8 0x51d400 0x51d5f8 0x51d8e8 0x51da98 0x51dce8 0x51df08 0x51e0f8
#include "ai/ai_tls_data.h"
// @flags /TP /O2 /Oc
#include "ai/squad_data.h"
// @compiler xdk6021
#include "ai/ai_collision_constants.h"
#include "ai/character_behavior_properties.h"
#include "ai/character_weapon_properties.h"
#include "physics/collisions.h"
#include "ai/actor_queries.h"
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "units/unit_seat_flags.h"
#include "memory/tls_globals.h"
#include <stdlib.h>
#include "game/game_globals_storage.h"
#include "game/game_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "units/vehicle_types.h"
#include "ai/ai_point3d.h"
#include "ai/prop_state.h"
#include "ai/actor_datum.h"
#include "ai/prop_datum.h"
#include "ai/weapon_datum_types.h"
#include "units/biped_types.h"
#include "units/vehicle_datum.h"
#include "units/unit_seat.h"
#include "ai/squad_types.h"
// @address 0xbf180 ?actor_should_lower_weapon@@YA_NJ@Z
// @address 0xafb40 ?actor_weapon_properties_get@@YAPAUcharacter_weapon_properties@@J@Z
// @address 0xbf268 ?actor_get_gun_position@@YAXJ_NPATreal_point3d@@@Z
// @address 0xc16f8 ?actor_get_weapon_bounds@@YA_NJPAUcharacter_weapon_properties@@PAM11@Z
// @address 0xc1b00 ?character_firing_pattern_calculate@@YA_NJMPIAUcharacter_firing_pattern@@@Z
// @address 0xc4510 ?actor_request_grenade_throw@@YA_NJ_N@Z
// @address 0x51df08 ?actor_handle_projectile_impact@@YAXJJ@Z
#include <math.h>

#include "math/real_math.h"
#include "game/game_time_types.h"
#include "ai/actors.h"
#include "game/game.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/actor_looking.h"
#include "ai/actor_perception.h"
#include "ai/ai.h"
#include "ai/ai_reference_frame.h"
#include "ai/objectives.h"
#include "ai/prop_dialogue.h"
#include "units/unit_seat_source.h"
#include "tag_files/tag_types.h"
#include "ai/ai_information_data.h"
#include "objects/cluster_reference.h"
#include "ai/prop_ref_datum.h"
#include "ai/props.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "units/unit_animation.h"
#include "motor/action_system.h"
#include "ai/behavior_misc.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/styles.h"
#include "memory/data.h"
#include "math/random_math_range.h"
#include "game/game_globals.h"
#include "game/game_allegiance.h"
#include "units/vehicles.h"
#include "game/player_types.h"
#include "physics/collision_types.h"
#include "cache/cache_files.h"
#include "math/matrix_math.h"
#include "ai/objective_storage.h"
#include "ai/actor_type_flood_types.h"
#include "ai/clump_datum.h"
#include "ai/swarms.h"
#include "ai/actor_nearby_iterator_types.h"
#include "items/weapon_definitions.h"
#include "units/vehicle_definitions.h"
#include "units/unit_definitions.h"
#include "objects/object_definitions.h"
#include "objects/object_header.h"
#include "models/model_types.h"
#include "objects/object_header_access.h"
#include "math/real_math_trigonometry.h"
#include "items/weapon_definition_access.h"
#include "game/game_time_access.h"
#include "game/game_time_inlines.h"
extern "C" {
#include "math/real_math_constants.h"
}
#include "physics/physics_constants_globals.h"






#include "ai/character_firing_pattern_properties.h"

struct character_firing_pattern_properties;
// @address 0xc1cd0 ?actor_firing_pattern_properties_get@@YAPAUcharacter_firing_pattern_properties@@J@Z





// @address 0xbf368 ?unit_get_aim_position@@YA_NJPATreal_point3d@@@Z
// @address 0xc04a8 ?unit_get_gun_position@@YA_NJ_NPATreal_point3d@@@Z
// @address 0x9a6a8 ?unit_get_camera_position@@YAXJPATreal_point3d@@@Z

// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xc00c0 ?prop_get_target_object_index@@YAJJ@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z








#include "items/projectile_definitions.h"
#include "items/projectile_types.h"
#include "tag_files/tag_groups.h"



#include "items/equipment_definitions.h"








#include "models/model_target_types.h"








// actor_combat_update needs the one-parameter getter shape below; the shared
// two-parameter accessor flips float ties inside that function.
static __forceinline actor_datum *actor_get_direct(long actor_index)
{
 return &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
}

static __forceinline object_datum *actor_combat_object_try_and_get(long object_index)
{
 object_datum *result = 0;
 if (object_index != -1) result = object_get(object_index);
 return result;
}

bool actor_should_lower_weapon(long actor_index)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool result;
 if (actor->lower_weapon_forced)
  result = true;
 else if (actor->lower_weapon_blocked_a || actor->lower_weapon_blocked_b || actor->lower_weapon_blocked_c)
  result = false;
 else if (actor->lower_weapon_alternate)
  result = true;
 else
  result = actor->weapon_state_ticks < 6;
 return result;
}

void actor_get_gun_position(long actor_index, bool unknown, real_point3d *position)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 real_point3d aim_position;
 if (!unit_get_aim_position(actor->unit_index, &aim_position))
  aim_position = actor->position.head;
 *position = aim_position;
 if (actor->gun_unit_index != -1)
 {
  object_datum *unit = actor_combat_object_try_and_get(actor->unit_index);
  if (unit && actor->gun_unit_index == unit->parent_object_index)
  {
   object_get_origin(unit->parent_object_index, position);
   position->z = aim_position.z;
  }
  else if (!unit_get_gun_position(actor->gun_unit_index, true, position))
  {
   unit_get_camera_position(actor->unit_index, position);
  }
 }
}

// @address 0xb4710 ?actor_get_task_style@@YAPAUstyle_definition@@J@Z

bool actor_get_weapon_bounds(long actor_index, character_weapon_properties *properties, float *minimum, float *maximum, float *distance)
{
 bool result = false;
 character_weapon_properties *weapon = (character_weapon_properties *)properties;
 if (!weapon)
  weapon = (character_weapon_properties *)actor_weapon_properties_get(actor_index);
 if (weapon)
 {
  short attitude = 0;
  style_definition *style = (style_definition *)actor_get_task_style(actor_index);
  if (style)
   attitude = style->attitude;
  switch (attitude)
  {
  default:
   if (minimum)
    *minimum = weapon->default_minimum;
   if (maximum)
    *maximum = weapon->default_maximum;
   break;
  case 2:
   if (minimum)
    *minimum = weapon->mode2_minimum;
   if (maximum)
    *maximum = weapon->mode2_maximum;
   break;
  case 1:
   if (minimum)
    *minimum = weapon->mode1_minimum;
   if (maximum)
    *maximum = weapon->mode1_maximum;
   break;
  }
  if (distance)
   *distance = weapon->maximum_range;
  result = true;
 }
 return result;
}

// match: 0xc1b00: __restrict lets the blend reuse pattern fields across output stores without alias reloads
bool character_firing_pattern_calculate(long actor_index, float weight, character_firing_pattern *__restrict output)
{
 bool result = false;
 character_firing_pattern_properties *properties = (character_firing_pattern_properties *)actor_firing_pattern_properties_get(actor_index);
 if (properties)
 {
  if (properties->patterns.count() > 0)
  {
   character_firing_pattern *first = &properties->patterns[0];
   result = true;
   if (properties->patterns.count() < 2)
   {
    *output = *first;
   }
   else
   {
    character_firing_pattern *second = &properties->patterns[1];
    output->rate = first->rate * (1.0f - weight) + second->rate * weight;
    output->burst_time = first->burst_time * (1.0f - weight) + second->burst_time * weight;
    output->burst_separation = first->burst_separation * (1.0f - weight) + second->burst_separation * weight;
    output->damage_modifier = first->damage_modifier * (1.0f - weight) + second->damage_modifier * weight;
    output->projectile_error = first->projectile_error * (1.0f - weight) + second->projectile_error * weight;
    output->projectile_error_minimum = first->projectile_error_minimum * (1.0f - weight) + second->projectile_error_minimum * weight;
    output->projectile_error_maximum = first->projectile_error_maximum * (1.0f - weight) + second->projectile_error_maximum * weight;
    output->burst_error = first->burst_error * (1.0f - weight) + second->burst_error * weight;
    output->burst_error_minimum = first->burst_error_minimum * (1.0f - weight) + second->burst_error_minimum * weight;
    output->burst_error_maximum = first->burst_error_maximum * (1.0f - weight) + second->burst_error_maximum * weight;
    output->leading = first->leading * (1.0f - weight) + second->leading * weight;
    output->leading_variance = first->leading_variance * (1.0f - weight) + second->leading_variance * weight;
    output->aiming = first->aiming * (1.0f - weight) + second->aiming * weight;
    output->aiming_variance = first->aiming_variance * (1.0f - weight) + second->aiming_variance * weight;
    output->tracking = first->tracking * (1.0f - weight) + second->tracking * weight;
    output->tracking_variance = first->tracking_variance * (1.0f - weight) + second->tracking_variance * weight;
   }
  }
 }
 return result;
}


static __forceinline long ai_seconds_to_ticks_round(float seconds)
{
 return (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds + AI_REAL_CONSTANTS[0]);
}

static __forceinline bool actor_combat_task_group_grenade_allowed(unsigned short objective_index, short task_index)
{
 bool result = false;
 s_task_record *task = &g_tasks_records[(short)(((s_objective_record *)g_objectives_data->data)[objective_index].first_task + task_index)];
 if (task->grenade_time != -1)
#if defined(_XBOX) && !defined(__clang__)
  result = TLS_GAME_TIME_GLOBALS->time - task->grenade_time < ai_seconds_to_ticks_round(AI_REAL_CONSTANTS[0x1518 / 4]);
#else
  result = TLS_GAME_TIME_GLOBALS->time - task->grenade_time < ai_seconds_to_ticks_round(2.0f);
#endif
 return result;
}

bool actor_request_grenade_throw(long actor_index, bool force)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 if (actor->squad_index != -1 && !force)
 {
  squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
  if (::squad_valid_task(squad) && actor_combat_task_group_grenade_allowed(squad->objective_index, squad->task_index))
   force = true;
 }
 return force;
}


static __forceinline prop_ref_datum *actor_combat_prop_ref_get(long prop_ref_index)
{
 return &((prop_ref_datum *)prop_ref_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(prop_ref_index)];
}

static __forceinline long object_get_ultimate_parent_in_headers(s_object_header *object_headers, long object_index)
{
 long result = -1;
 while (object_index != -1)
 {
  result = object_index;
  object_index = object_headers[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)].datum->parent_object_index;
 }
 return result;
}


void actor_handle_projectile_impact(long projectile_index, long object_index)
{
 s_object_header *object_headers = (s_object_header *)((s_object_header *)object_header_data->data);
 projectile_datum *projectile = (projectile_datum *)object_headers[DATUM_INDEX_TO_ABSOLUTE_INDEX(projectile_index)].datum;
 object_datum *owner = (object_datum *)object_try_and_get_and_verify_type(projectile->projectile.parent_unit_index, -1);
 if (owner && owner->object_type == 0)
 {
  if (((unit_datum *)owner)->unit.actor_index != -1)
  {
   actor_datum *actor = datum_get<actor_datum>(actor_data, ((unit_datum *)owner)->unit.actor_index);
   if (!((long)actor->flags & 0x8000) && actor->firing_start_time > 0 && actor->firing_type == 1 &&
    actor->target_prop_ref_index != -1 && TLS_GAME_TIME_GLOBALS->time - actor->firing_start_time >= projectile->projectile.age)
   {
    bool missed = true;
    if (projectile->projectile.time + 0.5f > actor->firing_time)
    {
     missed = false;
    }
    else if (object_try_and_get_and_verify_type(object_index, -1))
    {
     prop_ref_datum *prop_ref = actor_combat_prop_ref_get(actor->target_prop_ref_index);
     long parent_index = object_get_ultimate_parent_in_headers(object_headers, object_index);
     if (parent_index == prop_get_target_object_index(prop_ref->prop_index))
      missed = false;
    }
    if (missed)
    {
     actor->impact_count++;
    }
    else
    {
     actor->impact_count -= 2;
     if (actor->impact_count < 0)
      actor->impact_count = 0;
    }
   }
  }
 }
}

// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z

// @address 0x505260 ?actor_nearby_actors_iterator_new@@YAXPAUactor_nearby_actors_iterator@@PBTreal_point3d@@MFF@Z
// @address 0x5052d0 ?actor_nearby_actors_iterator_next@@YAPAUactor_datum@@PAUactor_nearby_actors_iterator@@@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z

// @address 0x51da98 ?actor_combat_check_collateral_damage@@YA_NJMMPBTreal_point3d@@PAF1@Z
bool actor_combat_check_collateral_damage(long actor_index, float radius, float distance, const real_point3d *point, short *count, short *flags)
{
 float search_radius;
 actor_datum *actor;
 float radius_squared;
 float distance_squared;
 short nearby_team;
 short actor_team;
 long swarm_index;
 long object_index;
 short player_actor_team;
 object_datum *object;
 long object_team;
 float object_distance_squared;
 actor_datum *nearby_actor;
 bool result;
 player_datum *player;
 short actor_count;
 short player_count;
 c_data_iterator<player_datum> scan;
 actor_nearby_actors_iterator nearby;
 real_vector3d vector;
 actor = datum_get<actor_datum>(actor_data, actor_index);
 if(radius > distance) search_radius = radius; else search_radius = distance;
 radius_squared = radius * radius;
 distance_squared = distance * distance;
 result = true;
 actor_count = 0;
 player_count = 0;
 actor_nearby_actors_iterator_new((actor_nearby_actors_iterator *)&nearby, point, search_radius, actor->team, 2);
 nearby_actor = (actor_datum *)actor_nearby_actors_iterator_next((actor_nearby_actors_iterator *)&nearby);
 while (nearby_actor != 0) {
  nearby_team = nearby_actor->team;
  actor_team = actor->team;
  if (actor_team != nearby_team && game_team_is_enemy((e_game_team)actor_team, (e_game_team)nearby_team)) {
   if (!(nearby.distance_squared >= radius_squared)) {
    swarm_index = nearby_actor->swarm_index;
    if (swarm_index != -1)
     actor_count = (short)(datum_get<swarm_datum>(g_swarm_data, swarm_index)->creature_count + actor_count);
    else
     actor_count = (short)(actor_count + 1);
   }
  } else if (nearby.distance_squared < distance_squared) {
   result = false;
   break;
  }
  nearby_actor = (actor_datum *)actor_nearby_actors_iterator_next(&nearby);
 }
 if (result) {
  scan.m_iterator.absolute_index = -1;
  scan.m_iterator.index = -1;
  scan.m_iterator.data = player_data;
  player = (player_datum *)data_iterator_next(&scan.m_iterator);
  scan.m_datum = player;
  while (player != 0) {
   object_index = player->unit_index;
   if (object_index != -1) {
    player_actor_team = actor->team;
    object = datum_get<s_object_header>(object_header_data, object_index)->datum;
    object_team = ((unit_datum *)object)->unit.team;
    vector_from_points3d(point, &object->bounding_center, &vector);
    object_distance_squared = magnitude_squared3d(&vector);
    if (actor->team != object_team && game_team_is_enemy((e_game_team)player_actor_team, (e_game_team)object_team)) {
     if (!(object_distance_squared >= radius_squared)) {
      actor_count = (short)(actor_count + 1);
      player_count = (short)(player_count + 1);
     }
    } else if (object_distance_squared < distance_squared) {
     result = false;
     break;
    }
   }
   player = (player_datum *)data_iterator_next(&scan.m_iterator);
   scan.m_datum = player;
  }
 }
 if (count) *count = actor_count;
 if (flags) *flags = player_count;
 return result;
}

// @address 0x11bb78 ?target_get_prop_class@@YAFPAUactor_datum@@@Z


// @address 0x51c120 ?actor_combat_enable_special_fire_situation@@YA_NJF@Z
bool actor_combat_enable_special_fire_situation(long actor_index, short situation)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool result = false;
 switch (situation) {
 case 1: result = actor->current_fire_target_type == 1 && target_get_prop_class(actor) >= 7; break;
 case 2:
 result = actor->current_fire_target_type == 0 && target_get_prop_class(actor) >= 5 &&
     game_ticks_to_seconds((float)actor->target_age) >= 2.5f;
 break;
 case 3: result = actor->current_fire_target_type == 1 && target_get_prop_class(actor) >= 7 && actor->controlling_other_unit; break;
 }
 return result;
}

// @address 0xaf9f0 ?prop_state_get@@YAPAUprop_state@@PBUprop_ref_datum@@@Z


// @address 0x51c230 ?actor_combat_allow_special_fire_situation@@YA_NJF@Z
static __forceinline prop_datum *actor_combat_prop_get(long prop_index)
{
 return &((prop_datum *)prop_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(prop_index)];
}

bool actor_combat_allow_special_fire_situation(long actor_index, short situation)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool result = true;
 if (!actor->bypass && situation == 3)
 {
  if (actor->firing_type == 1)
  {
   prop_ref_datum *prop_ref = actor_combat_prop_ref_get(actor->target_prop_ref_index);
   prop_state *state = prop_state_get((const prop_ref_datum *)prop_ref);
   if (state->object_index != -1)
    result = true;
   else if (datum_get<prop_datum>(prop_data, prop_ref->prop_index)->hostile)
    result = false;
   else
   {
    short count = 0;
    actor_combat_check_collateral_damage(actor_index, 6.0f, 0.0f, &state->position, &count, 0);
    result = count >= 3;
   }
  }
  else
   result = false;
 }
 return result;
}

static __forceinline bool actor_combat_uninspected_orphan(prop_ref_datum *ref)
{
 bool result;
 if (ref->knowledge_state >= 3 && ref->view_index != -1)
 {
  tracking_datum *orphan = datum_get<tracking_datum>(TLS_PROP_DATA, ref->view_index);
  if (orphan)
  {
   short *flags = &orphan->tracking_view.flags;
   if (flags)
    result = ((unsigned long)~*flags >> 6) & 1;
   else
    result = false;
  }
  else
   result = false;
 }
 else
  result = false;
 return result;
}

// @address 0x51d8e8 ?actor_combat_find_grenade_target@@YA_NJPBTreal_point3d@@JPAJ@Z
bool actor_combat_find_grenade_target(long actor_index, const real_point3d *position, long target_index, long *object_index)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool result = false;
 character_grenade_properties *properties = actor_grenade_properties_get(actor_index);
 if (properties)
 {
  float distance = distance3d(&actor->position.body, position);
  if (((long)actor->flags & 0x400000) || (distance > properties->minimum_distance && distance < properties->maximum_distance))
  {
   if (target_index != -1)
   {
    prop_ref_datum *ref = actor_combat_prop_ref_get(target_index);
    prop_state *state = prop_state_get((const prop_ref_datum *)ref);
    if (((long)actor->flags & 0x400000) ||
     (datum_get<prop_datum>(prop_data, ref->prop_index)->valid && !state->inactive &&
      ((ref->knowledge_state >= 1 && ref->knowledge_state <= 2) || actor_combat_uninspected_orphan(ref))))
    {
     *object_index = state->object_index;
     result = true;
    }
   }
   else
    result = true;
  }
 }
 return result;
}

// @address 0x51cf68 ?actor_start_pause@@YAXJPAUcharacter_weapon_properties@@PAUcharacter_firing_pattern@@@Z
void actor_start_pause(long actor_index, character_weapon_properties *weapon, character_firing_pattern *pattern)
{
 float leading = pattern->leading;
 float leading_variance = pattern->leading_variance;
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 unsigned long *seed = TLS_RANDOM_SEED;
 random_seed_next(seed);
 float unit = (float)(*seed >> 16) * 1.5259022e-05f;
 float pause = unit * (leading_variance - leading) + leading;
 pause *= ::game_difficulty_get_team_value(14, (e_game_team)actor->team);
 actor->delay = (short)ai_seconds_to_ticks_round(pause);
}

static __forceinline float actor_combat_game_difficulty_value_get(short value_type)
{
 short difficulty = game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty : 1;
 return game_difficulty_get_value_by_difficulty(value_type, difficulty);
}

// @address 0x51d068 ?actor_start_first_burst_delay@@YA_NJPAUcharacter_weapon_properties@@PAUcharacter_firing_pattern@@@Z
bool actor_start_first_burst_delay(long actor_index, character_weapon_properties *properties, character_firing_pattern *pattern)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool ready = actor->ready;
 if (actor->firing_type == 1)
 {
  prop_ref_datum *ref = actor_combat_prop_ref_get(actor->target_prop_ref_index);
  if (ref->knowledge_state >= 3)
   ready = true;
 }
 if (ready || !properties)
  actor->delay = 0;
 else
 {
  // match: 0x51d068: pointer __restrict qualifier prevents alias reload
  character_weapon_properties *__restrict weapon = (character_weapon_properties *)properties;
  float lower = weapon->first_delay_lower;
  float upper = weapon->first_delay_upper;
  unsigned long *seed = TLS_RANDOM_SEED;
  random_seed_next(seed);
  float fraction = (float)(*seed >> 16) * 1.5259022e-05f;
  float delay = lower + (upper - lower) * fraction;
  delay *= actor_combat_game_difficulty_value_get(13);
  actor->delay = (short)ai_seconds_to_ticks_round(delay);
 }
 return !ready;
}


static __forceinline long actor_combat_grenade_projectile_definition_index(short grenade_type)
{
 s_game_globals *globals = (s_game_globals *)global_game_globals;
 s_grenade_definition *definition = globals->grenades.count() ? &((s_grenade_definition *)globals->grenades.address)[grenade_type] : 0;
 long result = -1;
 if (definition && definition->projectile.index != -1)
  result = definition->projectile.index;
 return result;
}

static __forceinline float actor_combat_projectile_get_ballistic_acceleration(long definition_index)
{
 projectile_definition *definition =
     (projectile_definition *)tag_get(NONE, definition_index);
 return -(definition->air_gravity_scale * global_gravity_get());
}


// @address 0x51d400 ?actor_combat_build_grenade_trajectory@@YA_NFPBTreal_point3d@@MM0PAM_NPATreal_vector3d@@1131@Z
bool actor_combat_build_grenade_trajectory(short grenade_type, const real_point3d *origin, float speed, float speed_limit,
 // match: 0x51d400: pointer __restrict qualifier prevents alias reload
 const real_point3d *target, float *maximum_time_scale, bool high_arc, real_vector3d *__restrict velocity, float *time,
 // match: 0x51d400: pointer __restrict qualifier prevents alias reload
 float *ballistic_speed, real_vector3d *__restrict ballistic_velocity, float *gravity)
{
 bool result = false;
 long definition_index = actor_combat_grenade_projectile_definition_index(grenade_type);
 if (definition_index != -1)
 {
  bool linear = false;
  float minimum_speed;
  if (projectile_aim(definition_index, origin, target, &speed_limit, &minimum_speed, 0, 0, 0, velocity, time, ballistic_speed, 0, &linear) != 0)
   result = true;
  if (result == 0 && minimum_speed > 0.0f && minimum_speed < speed_limit)
  {
   minimum_speed = minimum_speed + 0.01f;
   if (projectile_aim(definition_index, origin, target, &minimum_speed, 0, 0, 0, 0, velocity, time, ballistic_speed, 0, &linear) != 0)
    result = true;
  }
  if (result != 0)
  {
   if (ballistic_velocity)
    scale_vector3d(velocity, *time, ballistic_velocity);
   if (gravity)
    *gravity = linear ? 0.0f : actor_combat_projectile_get_ballistic_acceleration(definition_index);
  }
 }
 return result;
}


static __forceinline bool actor_combat_game_skull_is_active_primary(long skull)
{
 unsigned long skulls = game_globals->options.game_mode == 1 ? game_globals->options.primary_skulls : 0;
 return (skulls & (1 << skull)) != 0;
}



// @address 0x51dce8 ?actor_combat_plan_grenade_trajectory@@YA_NJFPBVc_ai_point3d@@JJF@Z
bool actor_combat_plan_grenade_trajectory(long actor_index, short grenade_type, const c_ai_point3d *point, long target_type, long target_index, short flags)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 character_grenade_properties *properties = actor_grenade_properties_get(actor_index);
 bool result = false;
 real_point3d target;
 ai_point_get_position(point, &target);
 if (properties)
 {
  real_point3d origin = actor->position.head;
  float velocity_scale = actor_combat_game_skull_is_active_primary(3) ? 2.0f : 1.0f;
  real_vector3d velocity;
  float time;
  float ballistic_speed;
  real_vector3d ballistic_velocity;
  float gravity;
  if (actor_combat_build_grenade_trajectory(properties->grenade_type, &origin, properties->grenade_ideal_velocity * velocity_scale,
   properties->grenade_velocity * velocity_scale, &target, 0, false, &velocity, &time, &ballistic_speed, &ballistic_velocity, &gravity))
  {
   if ((flags & 2) ||
       ai_test_ballistic_line_of_fire(
           actor_index, &origin, ballistic_speed, &ballistic_velocity, gravity, target_index, actor->vehicle_index != -1))
   {
    actor->grenade_throw_ticks = (short)game_seconds_integer_to_ticks(1);
    actor->grenade_target_point = *point;
    actor->grenade_target_type = target_type;
    actor->grenade_target_index = target_index;
    actor->grenade_velocity = velocity;
    actor->grenade_time = time;
    if (flags & 1)
     actor->flags |= 0x200000;
    else
     actor->flags &= ~0x200000;
    if (flags & 2)
     actor->flags |= 0x400000;
    else
     actor->flags &= ~0x400000;
    result = true;
   }
  }
 }
 return result;
}


// @address 0x51c6d0 ?actor_combat_find_nearby_target@@YAXPATreal_point3d@@M@Z
void actor_combat_find_nearby_target(real_point3d *point, float distance)
{
    real_point3d origin;
    point_from_line3d(point, global_up3d, 1.5f, &origin);
    real_vector3d direction;
    unsigned long *seed = TLS_RANDOM_SEED;
    random_seed_next(seed);
    float unit = (float)(*seed >> 16) * 1.5259022e-05f;
    vector3d_from_angle(&direction, unit * 6.28318530717958647692f - 3.14159265358979323846f);
    real_point3d target;
    point_from_line3d(&origin, &direction, distance, &target);
    collision_result result;
    if ((unsigned char)collision_test_line(k_ai_alternate_collision_flags, point, &origin, -1, -1, (collision_result *)&result))
        origin = *point;
    if ((unsigned char)collision_test_line(k_ai_line_of_sight_default_flags, &origin, &target, -1, -1, (collision_result *)&result)) {
        float hit_distance = result.fraction * distance - 0.1f;
        if (hit_distance < 0.0f)
            hit_distance = 0.0f;
        point_from_line3d(&origin, &direction, hit_distance, &target);
    }
    *point = target;
}

static __forceinline real_point3d *actor_combat_prop_get_position(long prop_index, real_point3d *position)
{
    *position = ((prop_state *)prop_state_get_by_index(prop_index))->position;
    return position;
}

static __forceinline float actor_combat_dot_vec(const real_vector3d *a,const real_vector3d *b){return a->i*b->i+a->j*b->j+a->k*b->k;}
// @address 0x51e0f8 actor_combat_try_and_get_throw_velocity
extern "C" bool actor_combat_try_and_get_throw_velocity(long actor_index, long object_index, real_vector3d *velocity)
{
    bool result = false;
    if (object_index != -1) {
        s_object_header *header = datum_get<s_object_header>(object_header_data, object_index);
        if (((1 << header->object_type) & 8) != 0) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            bool valid = false;
            real_point3d target_point;
            if (((long)actor->flags & (1 << 30)) != 0) {
                ai_point_get_position(&actor->equipment_target, &target_point);
                valid = true;
            } else if (actor->combat_target_prop_index != -1) {
                const s_equipment_definition *definition = (const s_equipment_definition *)tag_get(NONE, header->datum->definition_index);
                if ((definition->ai_flags & 4) != 0) {
                    actor_combat_prop_get_position(actor->combat_target_prop_index, &target_point);
                    valid = true;
                }
            }
            bool have_target = valid;
            if (have_target) {
                {
                    real_vector3d check_storage;
                    real_vector3d &check = check_storage;
                    check.i = target_point.x - actor->position.body.x;
                    check.k = 0.0f;
                    check.j = target_point.y - actor->position.body.y;
                    if (normalize3d(&check) > 0.0f
                        && actor_combat_dot_vec(&actor->combat_direction, &check) > 0.5f)
                    {
                        real_vector3d direction_storage;
                        real_vector3d &direction = direction_storage;
                        real_point3d aim_position;
                        float time;
                        float result_speed;
                        float result_time;
                        unit_get_aim_position(actor->unit_index, &aim_position);
                        projectile_aim_ballistic(7.0f, 1.0f, &aim_position, &target_point, &time, 0, 0, false, &direction,
                        &result_speed, &result_time, 0, 0, 0);
                        if (time < 7.0f) {
                            float adjusted_time = time + 0.02f;
                            if (projectile_aim_ballistic(
                                    adjusted_time, 1.0f, &aim_position, &target_point, 0, 0, 0, false, &direction, &result_speed, &result_time, 0, 0, 0))
                            {
                                velocity->i = direction.i * adjusted_time;
                                velocity->j = direction.j * adjusted_time;
                                velocity->k = direction.k * adjusted_time;
                                actor->flags &= ~(1 << 30);
                                result = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

#define ACTOR_COMBAT_SET_BIT(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

static __forceinline object_datum *actor_combat_weapon_object_get(long object_index)
{
    object_datum *object = datum_get<s_object_header>(object_header_data, object_index)->datum;
    return object;
}





static __forceinline unsigned long *combat_random_seed_address(void)
{
    return TLS_RANDOM_SEED;
}

static __forceinline unsigned long actor_combat_random_boolean(unsigned long *seed)
{
    unsigned long next = random_seed_next(seed);
    return next & 0x80000000;
}

static __forceinline unsigned long actor_combat_random(unsigned long *seed)
{
    random_seed_next(seed);
    return *seed >> 16;
}


static __forceinline float combat_real_random_range(unsigned long *seed, float lower, float upper)
{
    return lower + (upper - lower) * ((float)actor_combat_random(seed) * 1.5259022e-05f);
}

static __forceinline long actor_combat_real_to_long_round(float value)
{
    return (long)(value + 0.5f);
}

static __forceinline long actor_combat_burst_seconds_to_ticks_round(float seconds)
{
    return actor_combat_real_to_long_round((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds);
}

static __forceinline float actor_combat_tangent(float angle)
{
    return (float)tan((double)angle);
}



static __forceinline float actor_combat_sine(float angle)
{
    return (float)sin((double)angle);
}


// @address 0x51c8a0 ?actor_start_burst@@YAXJJPAUcharacter_weapon_properties@@PAUcharacter_firing_pattern@@@Z
// match: 0x51c8a0: pointer __restrict qualifier prevents alias reload
void actor_start_burst(long actor_index, long weapon_object_index, character_weapon_properties *__restrict weapon_properties,
    // match: 0x51c8a0: pointer __restrict qualifier prevents alias reload
    character_firing_pattern *__restrict pattern)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    object_datum *weapon_object = 0;
    const weapon_definition *weapon_definition = 0;
    bool find_nearby_target = false;
    if (weapon_object_index != -1) {
        weapon_object = actor_combat_weapon_object_get(weapon_object_index);
        weapon_definition = weapon_definition_get(weapon_object->definition_index);
    }
    if (TEST_BIT(actor->flags, 14)) {
        if (!actor_combat_allow_special_fire_situation(actor_index, weapon_properties->fire_mode)) {
            ACTOR_COMBAT_SET_BIT(actor->flags, 14, false);
        } else if (weapon_definition && weapon_definition->barrel_count <= 1) {
            ACTOR_COMBAT_SET_BIT(actor->flags, 14, false);
        }
    }
    ACTOR_COMBAT_SET_BIT(actor->flags, 13, TEST_BIT(actor->flags, 14));
    ACTOR_COMBAT_SET_BIT(actor->flags, 14, false);
    float burst_time = combat_real_random_range(combat_random_seed_address(), pattern->burst_error_minimum, pattern->burst_error_maximum);
    actor->delay = (short)actor_combat_burst_seconds_to_ticks_round(burst_time);
    actor->firing_start_time = game_time_get();
    ACTOR_COMBAT_SET_BIT(actor->flags, 15, TEST_BIT(actor->flags, 0));
    actor->burst_aim_velocity = pattern->aiming_variance;
    actor->burst_aim_delay = 0.0f;
    if (pattern->aiming > 0.0f)
        actor->burst_aim_delay = pattern->aiming;
    if (TEST_BIT(actor->flags, 13) || TEST_BIT(actor->flags, 12)) {
        if (weapon_properties->special_fire_delay > 0.0f)
            actor->burst_aim_delay *= weapon_properties->special_fire_delay;
        actor->burst_aim_velocity += weapon_properties->special_fire_aim_velocity;
    }
    float nearby_target_distance = weapon_properties->bombardment_range;
    if (nearby_target_distance > 0.0f && actor->firing_type == 1) {
        prop_ref_datum *prop_ref = actor_combat_prop_ref_get(actor->target_prop_ref_index);
        find_nearby_target = prop_ref->knowledge_state < 1 || prop_ref->awareness == 0;
    }
    real_point3d target_point;
    ai_point_get_position(&actor->target_point, &target_point);
    if (find_nearby_target)
        actor_combat_find_nearby_target(&target_point, nearby_target_distance);
    real_vector3d vector_to_target;
    vector_from_points3d(&actor->position.head, &target_point, &vector_to_target);
    real_vector3d up;
    set_real_vector3d(&up, 0.0f, 0.0f, 1.0f);
    real_vector3d perpendicular;
    cross_product3d(&vector_to_target, &up, &perpendicular);
    normalize3d(&perpendicular);
    float distance = magnitude3d(&vector_to_target);
    actor->firing_time = distance;
    if (actor_combat_random_boolean(combat_random_seed_address()))
        negate_vector3d(&perpendicular, &perpendicular);
    float max_offset_angle = pattern->projectile_error;
    float max_return_angle = pattern->burst_error;
    float offset_radius = pattern->damage_modifier;
    float min_return_radius = pattern->projectile_error_minimum;
    float max_return_radius = pattern->projectile_error_maximum;
    float tracking = pattern->tracking;
    float offset_angle = combat_real_random_range(combat_random_seed_address(), -max_offset_angle, max_offset_angle);
    float return_angle = offset_angle + combat_real_random_range(combat_random_seed_address(), -max_return_angle, max_return_angle);
    float return_radius = combat_real_random_range(combat_random_seed_address(), min_return_radius, max_return_radius);
    if (offset_radius > 0.0f && distance > 0.0f) {
        float angle = arctangent(offset_radius, distance);
        if (angle > pattern->tracking_variance)
            offset_radius = distance * actor_combat_tangent(pattern->tracking_variance);
    }
    if (return_radius > 0.0f && distance > 0.0f) {
        float angle = arctangent(return_radius, distance);
        if (angle > 2.0f * pattern->tracking_variance)
            return_radius = distance * actor_combat_tangent(2.0f * pattern->tracking_variance);
    }
    if (actor->delay > 0 && tracking > 0.0f) {
        float max_track_angle = tracking * game_ticks_to_seconds((float)actor->delay);
        max_track_angle = (max_track_angle > 0.7853982f) ? 0.7853982f : max_track_angle;
        float max_track_radius = actor->target_distance * actor_combat_tangent(max_track_angle);
        if (offset_radius > max_track_radius) {
            float scale;
            if (offset_radius < max_track_radius * 1.5f) {
                scale = offset_radius / max_track_radius;
                actor->delay = (short)actor_combat_real_to_long_round(scale * (float)actor->delay);
            } else {
                scale = 1.5f;
                actor->delay = (short)actor_combat_real_to_long_round(scale * (float)actor->delay);
                max_track_radius *= scale;
                return_radius *= max_track_radius / offset_radius;
                offset_radius = max_track_radius;
            }
        }
    }
    float cos_offset = cosine_double(offset_angle);
    float sin_offset = actor_combat_sine(offset_angle);
    float cos_return = cosine_double(return_angle);
    float sin_return = actor_combat_sine(return_angle);
    real_vector3d burst_offset;
    burst_offset.i = offset_radius * (perpendicular.i * cos_offset + up.i * sin_offset);
    burst_offset.j = offset_radius * (perpendicular.j * cos_offset + up.j * sin_offset);
    burst_offset.k = offset_radius * (perpendicular.k * cos_offset + up.k * sin_offset);
    real_vector3d burst_return;
    burst_return.i = -return_radius * (perpendicular.i * cos_return + up.i * sin_return);
    burst_return.j = -return_radius * (perpendicular.j * cos_return + up.j * sin_return);
    burst_return.k = -return_radius * (perpendicular.k * cos_return + up.k * sin_return);
    if (actor->delay > 0)
        scale_vector3d(&burst_return, 1.0f / (float)actor->delay, &burst_return);
    actor->burst_target = actor->target_point;
    actor->burst_offset = burst_offset;
    actor->burst_return = burst_return;
    point_from_line3d(&target_point, &actor->burst_offset, 1.0f, &actor->burst_aim_point);
}

static __forceinline object_header_datum *actor_combat_weapon_header(long index)
{
    return datum_get<s_object_header>(object_header_data, index);
}
// @address 0x51c5c8 ?actor_get_weapon_vector@@YAXJPATreal_vector3d@@@Z
void actor_get_weapon_vector(long actor_index, real_vector3d *weapon_vector)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool found = false;
 long unit_index = actor->unit_index;
 if (actor->gun_uses_parent && actor->gun_unit_index != -1)
 {
  unit_index = actor->gun_unit_index;
  object_header_datum *header = actor_combat_weapon_header(actor->gun_unit_index);
  if (header->object_type == 1)
  {
   vehicle_definition *definition =
       (vehicle_definition *)tag_get(NONE, header->datum->definition_index);
   if (definition->definition_flags.test(_vehicle_definition_weapon_vector))
   {
    object_get_orientation(actor->gun_unit_index, weapon_vector, 0);
    found = true;
   }
  }
 }
 if (!found)
 {
  *weapon_vector = ((unit_datum *)object_get(unit_index))->unit.aiming_vector;
  unit_clip_vector_to_aiming_screen_bounds(unit_index, weapon_vector, false);
 }
}

// @address 0x51c360 ?actor_aim_projectile@@YA_NJPBTreal_point3d@@PATreal_vector3d@@PAM2PAJ@Z
bool actor_aim_projectile(long actor_index, const real_point3d *point, real_vector3d *vector, float *error, float *velocity, long *target_object_index)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 long target_object = -1;
 bool result = false;
 if (actor->combat_aim_mode == 4)
 {
  result = true;
 }
 else if (actor->combat_aim_mode == 2)
 {
  character_weapon_properties *weapon_properties = actor_weapon_properties_get(actor_index);
  bool aim_locked = weapon_properties && TEST_BIT(((character_weapon_properties *)weapon_properties)->flags, 3);
  result = true;
  if (actor->firing_type == 1 && actor->target_prop_ref_index != -1)
  {
   prop_ref_datum *prop_ref = actor_combat_prop_ref_get(actor->target_prop_ref_index);
   if (prop_ref->knowledge_state >= 1)
    target_object = prop_ref->object_index;
  }
  if (!aim_locked)
  {
   if (TEST_BIT(actor->flags, 24))
   {
    *vector = actor->combat_aim_vector;
    if (error)
     *error = actor->aim_field9a8;
   }
   else
   {
    vector_from_points3d(point, &actor->burst_aim_point, vector);
    normalize3d(vector);
   }
   real_vector3d weapon_vector;
   actor_get_weapon_vector(actor_index, &weapon_vector);
   if (dot_product3d(vector, &weapon_vector) < 0.8660254f)
   {
    bool rotate = true;
    real_vector3d axis;
    cross_product3d(&weapon_vector, vector, &axis);
    if (normalize3d(&axis) == 0.0f)
    {
     perpendicular3d(&weapon_vector, &axis);
     if (normalize3d(&axis) == 0.0f)
      rotate = false;
    }
    *vector = weapon_vector;
    if (rotate)
     rotate_vector_about_axis(vector, &axis, 0.5f, 0.8660254f);
   }
  }
  *velocity = actor->burst_aim_velocity;
 }
 if (target_object_index)
  *target_object_index = target_object;
 return result;
}


// @address 0x51d1e8 ?actor_combat_reaim_grenade@@YA_NJPBTreal_point3d@@@Z
bool actor_combat_reaim_grenade(long actor_index, const real_point3d *point)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 character_grenade_properties *properties = actor_grenade_properties_get(actor_index);
 bool result = false;
 if (properties)
 {
  long definition_index = actor_combat_grenade_projectile_definition_index(properties->grenade_type);
  real_vector3d aim_vector;
  float time;
  float ballistic_speed;
  bool linear;
  real_point3d target;
  ai_point_get_position(&actor->grenade_target_point, &target);
  if (projectile_aim(definition_index, point, &target, 0, 0, 0, &actor->grenade_time, false,
   &aim_vector, &time, &ballistic_speed, 0, &linear))
  {
   real_vector2d horizontal = aim_vector.xy;
   if (normalize2d(&horizontal) > 0.0f &&
    dot_product2d(&horizontal, &actor->combat_direction.xy) > 0.8660254f)
   {
    real_vector3d velocity;
    scale_vector3d(&aim_vector, time, &velocity);
    float gravity = linear ? 0.0f : actor_combat_projectile_get_ballistic_acceleration(definition_index);
    if (ai_test_ballistic_line_of_fire(actor_index, point, ballistic_speed, &velocity, gravity,
     actor->grenade_target_index, actor->vehicle_index != -1))
    {
     actor->grenade_time = time;
     actor->grenade_velocity = aim_vector;
     result = true;
    }
   }
  }
 }
 return result;
}

static bool actor_combat_update_grenade_target(long actor_index, const c_ai_point3d *target_point)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 character_grenade_properties *properties = actor_grenade_properties_get(actor_index);
 bool result = false;
 real_point3d position;
 ai_point_get_position(target_point, &position);
 if (properties && actor_combat_check_collateral_damage(actor_index, properties->throw_velocity, properties->damage_radius, &position, 0, 0))
 {
  actor->grenade_target_point = *target_point;
  result = true;
 }
 return result;
}

// @address 0x51d5f8 ?actor_aim_grenade@@YAJJPBTreal_point3d@@PATreal_vector3d@@@Z
long actor_aim_grenade(long actor_index, const real_point3d *point, real_vector3d *vector)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 long result = -1;
 long target_index = actor->grenade_target_type;
 if (target_index != -1)
 {
  prop_ref_datum *prop_ref = actor_combat_prop_ref_get(target_index);
  if (prop_ref->knowledge_state >= 1)
   result = prop_ref->object_index;
  if (prop_ref->knowledge_state > 0 && TEST_BIT(actor->flags, 21))
  {
   prop_state *state = prop_state_get((const prop_ref_datum *)prop_ref);
   prop_find_pathfinding_location(actor_index, target_index);
   c_ai_point3d target_point = state->pathfinding_point;
   target_point.point.z += 0.2f;
   actor_combat_update_grenade_target(actor_index, &target_point);
  }
 }
 actor_combat_reaim_grenade(actor_index, point);
 real_vector3d aim_vector = actor->grenade_velocity;
 if (actor->vehicle_index == -1)
 {
  real_vector2d flat_aim = aim_vector.xy;
  float length = normalize2d(&flat_aim);
  if (length > 0.0f)
  {
   if (dot_product2d(&actor->combat_direction.xy, &flat_aim) < 0.0f)
   {
    real_vector3d new_aim_vector = actor->combat_direction;
    bool clockwise = cross_product2d(&actor->combat_direction.xy, &flat_aim) > 0.0f;
    rotate_vector_about_axis(&new_aim_vector, global_up3d, (float)(clockwise ? 1 : -1) * 0.5f, 0.0f);
    if (normalize2d(&new_aim_vector.xy) > 0.0f)
    {
     new_aim_vector.i *= length;
     new_aim_vector.j *= length;
    }
    else
    {
     new_aim_vector.i = aim_vector.i;
     new_aim_vector.j = aim_vector.j;
    }
    new_aim_vector.k = aim_vector.k;
    aim_vector = new_aim_vector;
   }
  }
 }
 scale_vector3d(&aim_vector, actor->grenade_time, vector);
 return result;
}


// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
// @address 0xc1188 ?prop_find_pathfinding_location@@YAXJJ@Z
// @address 0xc1300 ?ai_reference_frame_inverse_transform_point@@YA_NFPBTreal_point3d@@PAT1@@Z
// @address 0xc14a8 ?scenario_structure_index_from_point@@YAFPBTreal_point3d@@@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z

// @address 0xc0dd8 actor_get_target_position
bool actor_get_target_position(long actor_index, long prop_ref_index, real_point3d *position, c_ai_point3d *ref_frame)
{
 prop_ref_datum *prop_ref = actor_combat_prop_ref_get(prop_ref_index);
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool result = false;
 short frame_index = -1;
 real_point3d aim_point;
 object_marker marker;
 prop_state *info;
 if (prop_ref->view_index != -1)
 {
  info = &datum_get<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index)->state;
 }
 else
 {
  info = &datum_get<prop_datum>(prop_data, prop_ref->prop_index)->state;
 }
 long target_object = prop_get_target_object_index(prop_ref->prop_index);
 object_datum *target;
 long object_type;
 {
  s_object_header *header = datum_get<s_object_header>(object_header_data, target_object);
  target = header->datum;
  object_type = header->object_type;
 }
 if (ref_frame)
 {
  if (object_type == 12)
   frame_index = -1;
  else
  {
   prop_find_pathfinding_location(actor_index, prop_ref_index);
   frame_index = info->pathfinding_point.location[0];
  }
 }
 character_weapon_properties *weapon_properties = actor_weapon_properties_get(actor_index);
 if ((((character_weapon_properties *)weapon_properties)->flags & 0x10) != 0)
 {
  long type_mask = 1 << object_type;
  bool valid = false;
  if ((type_mask & 1) != 0 && target->parent_object_index == -1)
   valid = true;
  else if ((type_mask & 2) != 0 && ((1 << vehicle_get_type(target_object)) & 0x10b) != 0)
  {
   void *definition = tag_get(NONE, target->definition_index);
   valid = ((vehicle_definition *)definition)->havok_vehicle_physics.size <= 0;
  }
  if (valid)
  {
   do {
    if ((type_mask & 0x1803) != 0)
    {
     bool blocked = false;
     object_datum *mover_object = datum_get<s_object_header>(object_header_data, target_object)->datum;
     long mover_type = *(long *)((char *)mover_object + ((unit_datum *)mover_object)->unit.motor_offset);
     if (mover_type == 7 || mover_type == 8)
      blocked = true;
     if (blocked)
      break;
    }
    prop_find_pathfinding_location(actor_index, prop_ref_index);
    ai_point_get_position(&info->pathfinding_point, &aim_point);
    float blend = info->position.z - actor->position.body.z;
    float dx = info->center.x - aim_point.x;
    float dy = info->center.y - aim_point.y;
    float dz = info->center.z - aim_point.z;
    blend = ((blend > 0.0f ? blend : 0.0f) < 1.0f) ? (blend > 0.0f ? blend : 0.0f) : 1.0f;
    aim_point.x = dx * blend + aim_point.x;
    aim_point.y = dy * blend + aim_point.y;
    result = true;
    aim_point.z = dz * blend + aim_point.z;
   } while (0);
  }
 }
 if (!result && prop_ref_index == actor->combat_target_prop_index)
 {
  if (((_rotl(actor->targeting_flags, 31)) & 1) == 0)
  {
   actor_target_choose_target_marker(actor_index, target_object);
   actor->targeting_flags |= 2;
  }
  if (actor->targeting_marker != 0 && internal_object_get_markers_by_string_id(target_object, actor->targeting_marker, &marker, 1, false) != 0)
  {
   result = true;
   aim_point = marker.matrix.position;
  }
 }
 if (!result)
 {
  result = true;
  aim_point = info->center;
 }
 if (result)
 {
  if (position)
   *position = aim_point;
  if (ref_frame)
  {
   bool frame_result = false;
   if (ai_reference_frame_inverse_transform_point(frame_index, &aim_point, &ref_frame->position))
   {
    ref_frame->location[0] = frame_index;
    ref_frame->location[1] = scenario_structure_index_from_point(&aim_point);
    frame_result = true;
   }
   return frame_result;
  }
 }
 return result;
}

static __forceinline void *actor_combat_tag_definition_get(long definition_index)
{
 void *definition = tag_get(NONE, definition_index);
 return definition;
}
// @address 0xc01a8 ?actor_target_choose_target_marker@@YA_NJJ@Z
bool actor_target_choose_target_marker(long actor_index, long object_index)
{
 bool result = false;
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 object_datum *object = object_get(object_index);
 const s_object_definition *object_definition =
     (const s_object_definition *)actor_combat_tag_definition_get(object->definition_index);
 character_weapon_properties *weapon = (character_weapon_properties *)actor_weapon_properties_get(actor_index);
 long chosen_marker = -1;
 float best_score = 0.0f;
 if (object->object_type != 0)
 {
  real_point3d origin;
  object_get_origin(object_index, &origin);
  if (weapon)
  {
   if (distance_squared3d(&actor->position.body, &origin) < weapon->maximum_targeting_distance * weapon->maximum_targeting_distance)
   {
    if (object_definition->model.index != -1)
    {
     const s_model_definition *model = (const s_model_definition *)actor_combat_tag_definition_get(
         object_definition->model.index);
     if (model->targets.count > 0)
     {
      real_point3d gun_position;
      actor_get_gun_position(actor_index, true, &gun_position);
      for (short i = 0; i < model->targets.count; i++)
      {
       const s_model_target *target = &((const s_model_target *)model->targets.address)[i];
       long marker_name = target->marker_name;
       if (target->damage_section_index < 0
        || (target->damage_section_index < (int)((unsigned int)object->damage_section_bytes >> 3)
         && ((const object_region_information *)((const char *)object + object->damage_section_offset))[target->damage_section_index].properties != 255))
       {
        object_marker markers[1];
        if (internal_object_get_markers_by_string_id(object_index, marker_name, markers, 1, false) != 0)
        {
         real_vector3d direction;
         vector_from_points3d(&markers[0].matrix.position, &gun_position, &direction);
         if (normalize3d(&direction) != 0.0f)
         {
          float angle = angle_between_vectors3d(&markers[0].matrix.forward, &direction);
          if (angle < target->cone_angle)
          {
           float favor = 1.0f - angle * 0.31830987f;
           if (marker_name == actor->targeting_marker)
            favor += 0.3f;
           float score = (target->targeting_relevance + 1.0f) * favor;
           if (score > best_score)
           {
            best_score = score;
            chosen_marker = marker_name;
           }
          }
         }
        }
       }
      }
     }
    }
   }
  }
 }
 if (chosen_marker != -1)
 {
  actor->targeting_marker = chosen_marker;
  result = true;
 }
 else
 {
  actor->targeting_marker = result;
 }
 return result;
}


// @address 0xc05e8 ?actor_weapon_aim@@YA_NJJFPBTreal_point3d@@0_NPATreal_vector3d@@PAM33PA_N@Z
bool actor_weapon_aim(long actor_index, long weapon_index, short barrel_index, const real_point3d *origin,
 const real_point3d *target, bool high_arc, real_vector3d *aim_vector, float *unused, float *result_distance,
 float *result_speed, bool *linear)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 object_datum *weapon = actor_combat_weapon_object_get(weapon_index);
 character_weapon_properties *properties = character_weapon_properties_get(actor->character_definition_index, weapon->definition_index);
 bool result = false;
 if (properties)
 {
  bool ballistic = false;
  float maximum_time_scale = 0.0f;
  const weapon_definition *definition = weapon_definition_get(weapon->definition_index);
  if (barrel_index >= 0 && barrel_index < definition->barrel_count)
  {
   long projectile_definition_index = definition->barrels[barrel_index].projectile_index;
   if (projectile_definition_index != -1)
   {
    const projectile_definition *projectile = (const projectile_definition *)tag_get(NONE, projectile_definition_index);
    if (((_rotl(projectile->targeting_flags, 31) & 1) != 0) && projectile->air_gravity_scale > 0.0f
     && properties->ballistic_firing_bounds.lower > 0.0f
     && properties->ballistic_firing_bounds.upper > properties->ballistic_firing_bounds.lower
     && properties->ballistic_fraction_bounds.lower > 0.0f
     && properties->ballistic_fraction_bounds.upper > 0.0f)
    {
     real_vector3d displacement;
     vector_from_points3d(origin, target, &displacement);
     float distance = sqrtf(displacement.k * displacement.k + displacement.j * displacement.j + displacement.i * displacement.i);
     float minimum_fraction = 1.0f - MIN(properties->ballistic_fraction_bounds.upper, 1.0f);
     float maximum_fraction = 1.0f - MIN(properties->ballistic_fraction_bounds.lower, 1.0f);
     if (distance >= properties->ballistic_firing_bounds.upper)
      maximum_time_scale = minimum_fraction;
     else if (distance <= properties->ballistic_firing_bounds.lower)
      maximum_time_scale = maximum_fraction;
     else
      maximum_time_scale = (distance - properties->ballistic_firing_bounds.lower) * (minimum_fraction - maximum_fraction)
       / (properties->ballistic_firing_bounds.upper - properties->ballistic_firing_bounds.lower) + maximum_fraction;
     ballistic = true;
    }
   }
   if (!ballistic && actor->vehicle_index == -1 && !actor_is_giant((actor_datum *)actor))
   {
    real_point3d aim_origin = actor->position.body;
    aim_origin.z = origin->z;
    return weapon_aim(weapon_index, barrel_index, &aim_origin, target, NULL, high_arc, aim_vector, NULL,
     result_distance, result_speed, linear);
   }
   result = weapon_aim(weapon_index, barrel_index, origin, target,
    (ballistic && maximum_time_scale > 0.0f) ? &maximum_time_scale : NULL, high_arc, aim_vector, NULL,
    result_distance, result_speed, linear);
  }
 }
 return result;
}



static __forceinline s_object_header *actor_combat_update_header_get(long object_index)
{
 return &((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)];
}

static __forceinline unit_datum *actor_combat_update_object_get(long object_index)
{
 return (unit_datum *)actor_combat_update_header_get(object_index)->datum;
}

static __forceinline long actor_combat_update_ultimate_parent(long object_index)
{
 long result = -1;
 while (object_index != -1)
 {
  result = object_index;
  object_index = actor_combat_update_object_get(object_index)->object.parent_object_index;
 }
 return result;
}

// @rdata 0x820221fc
static const float k_actor_combat_update_one = 1.0f;
// @rdata 0x8202974c
static const float k_actor_combat_update_sqrt_half = 0.70710677f;

static __forceinline bool actor_combat_update_flag_test(unsigned long flags, long bit)
{
 return (flags >> bit) & 1;
}

static __forceinline long actor_combat_update_seconds_to_ticks(float seconds)
{
 return (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds + 0.5f);
}

static __forceinline const weapon_definition *actor_combat_update_weapon_definition_get(long actor_index)
{
 const weapon_definition *definition = 0;
 long weapon_index = actor_get_weapon(actor_index);
 if (weapon_index != -1)
  definition = weapon_definition_get(actor_combat_update_object_get(weapon_index)->object.definition_index);
 return definition;
}

static __forceinline short actor_combat_update_unit_get_grenade_count(long unit_index, short grenade_type)
{
 return grenade_type != -1 ? actor_combat_update_object_get(unit_index)->unit.grenade_counts[grenade_type] : 0;
}

static __forceinline void actor_combat_update_unit_select_grenade(long unit_index, short grenade_type)
{
 unit_datum *unit = actor_combat_update_object_get(unit_index);
 unit->unit.desired_grenade_index = (signed char)grenade_type;
 unit->unit.current_grenade_index = unit->unit.desired_grenade_index;
}

static __forceinline short actor_combat_update_unit_add_grenade(long unit_index, short grenade_type, short count)
{
 unit_datum *unit = actor_combat_update_object_get(unit_index);
 unit->unit.grenade_counts[grenade_type] += count;
 actor_combat_update_unit_select_grenade(unit_index, grenade_type);
 return unit->unit.grenade_counts[grenade_type];
}

static __forceinline long actor_combat_update_unit_inventory_get_weapon(long unit_index, short slot)
{
 unit_datum *unit = actor_combat_update_object_get(unit_index);
 long weapon_index = -1;
 if (slot != -1)
  weapon_index = unit->unit.weapon_object_indices[slot];
 return weapon_index;
}

static __forceinline long actor_combat_update_secondary_weapon_get(long actor_index)
{
 actor_datum *actor = actor_get_direct(actor_index);
 long weapon_index = -1;
 if (actor_get_direct(actor_index)->unit_index != -1)
 {
  unit_datum *unit = actor_combat_update_object_get(actor->unit_index);
  weapon_index = actor_combat_update_unit_inventory_get_weapon(actor->unit_index, unit->unit.secondary_weapon_index);
 }
 return weapon_index;
}

static __forceinline short actor_combat_update_difficulty(void)
{
 return game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty : 1;
}

// @address 0xbd378 ?actor_combat_update@@YAXJ@Z
void actor_combat_update(long actor_index)
{
 actor_datum *actor = actor_get_direct(actor_index);
 long weapon_index = actor_get_weapon(actor_index);
 long secondary_weapon_index = actor_combat_update_secondary_weapon_get(actor_index);
 character_weapon_properties *weapon_properties = 0;
 const weapon_definition *weapon_definition = 0;
 character_grenade_properties *grenade_properties = actor_grenade_properties_get(actor_index);
 character_engage_properties *engage_properties = character_definition_engage_multi_properties_get(actor_index);
 unit_datum *unit = 0;
 bool hold_burst_start = false;
 bool should_fire = false;
 bool seat_prevents_firing = false;
 bool target_changed = false;
 bool target_valid = false;
 float collateral_damage_radius = 0.0f;
 short firing_decision = -1;
 real_point3d gun_position;
 real_point3d target_position;

 if (actor->unit_index != -1)
  unit = actor_combat_update_object_get(actor->unit_index);
 actor->weapon_flag908 = 0;
 if (actor->flags & 1)
 {
  actor->impact_count = 0;
  actor->flags |= 0x8000;
 }
 if (weapon_index != -1)
 {
  unit_datum *weapon = actor_combat_update_object_get(weapon_index);
  weapon_definition = actor_combat_update_weapon_definition_get(actor_index);
  weapon_properties = character_weapon_properties_get(actor_get_direct(actor_index)->character_definition_index, weapon->object.definition_index);
  if (actor_combat_update_flag_test(((weapon_datum *)weapon)->flags, 1))
   actor->weapon_flag908 = 1;
 }
 actor_unit_control_lower_weapon(actor_index, actor_should_lower_weapon(actor_index));
 bool has_ranged_weapon = actor_has_ranged_weapon(actor_index);
 if (actor->delay > 0)
  actor->delay--;
 if (actor->combat_ticks90e > 0)
  actor->combat_ticks90e--;
 if (actor->burst_cooldown_ticks > 0)
  actor->burst_cooldown_ticks--;
 if (actor->special_fire_ticks > 0)
  actor->special_fire_ticks--;
 if ((short)actor->firing_type > 0)
  actor->fire_target_ticks++;
 if (actor->combat_ticks838 > 0)
  actor->combat_ticks838--;

 if (unit && unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
 {
  unit_datum *vehicle = actor_combat_update_object_get(unit->object.parent_object_index);
  unit_definition *definition = (unit_definition *)tag_get(NONE, vehicle->object.definition_index);
  unsigned long *seat_flags = &((unit_seat *)definition->seats.address)[unit->unit.parent_seat_index].flags;
  if (actor_combat_update_flag_test(*seat_flags, 27))
  {
   seat_prevents_firing = true;
   actor->firing_type = 0;
  }
  if (actor_combat_update_flag_test(*seat_flags, 12) &&
   TEST_BIT(actor_combat_update_object_get(actor->unit_index)->unit.unit_flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 4)], 0))
  {
   actor->firing_type = 0;
   seat_prevents_firing = true;
   actor_dialogue_event(actor_index, 30, -1, 0);
  }
 }

 if (!seat_prevents_firing && actor->combat_aim_mode != 2)
 {
  short new_type = 0;
  if (actor->lower_weapon_blocked_b)
  {
   if (actor->fire_target_type == 2)
   {
    new_type = 2;
   }
   else
   {
    long prop_index = actor->fire_target_type == 1 ? actor->fire_target_prop_index : actor->combat_target_prop_index;
    if (prop_index != -1)
     new_type = 1;
   }
  }
  if (new_type != (short)actor->firing_type)
  {
   target_changed = true;
  }
  else if (new_type == 1)
  {
   if (actor->fire_target_type == 1)
    target_changed = actor->fire_target_prop_index != actor->target_prop_ref_index;
   else
    target_changed = actor->combat_target_prop_index != actor->target_prop_ref_index;
  }
  else if (new_type == 2)
  {
   target_changed = ai_distance_squared3d(&actor->target_manual_point, &actor->fire_target_point) > 0.25f;
  }
  if (target_changed)
   actor->fire_target_ticks = 0;
  actor->firing_type = new_type;
  if (new_type == 1)
  {
   if (actor->fire_target_type == 1)
    actor->target_prop_ref_index = actor->fire_target_prop_index;
   else
    actor->target_prop_ref_index = actor->combat_target_prop_index;
  }
  else if (new_type == 2)
  {
   actor->target_manual_point = actor->fire_target_point;
  }
 }

 actor->flags &= ~0x800000;
 actor_get_gun_position(actor_index, true, &gun_position);

 if (actor->grenade_throw_ticks > 0)
 {
  bool thrown = false;
  if (behavior_try_to_throw_grenade(actor_index))
  {
   if (grenade_properties)
   {
    if (actor_combat_update_unit_get_grenade_count(actor->unit_index, grenade_properties->grenade_type) == 0)
     actor_combat_update_unit_add_grenade(actor->unit_index, grenade_properties->grenade_type, 1);
   }
   if (action_submit_event(actor->unit_index, 56))
   {
    actor_dialogue_event(actor_index, 13, -1, 0);
    actor->grenade_throw_time = game_time_get();
    should_fire = false;
    thrown = true;
    actor->grenade_throw_ticks = 0;
   }
  }
  if (!thrown)
   actor->grenade_throw_ticks--;
 }
 else
 {
  if (actor->combat_ticks838 > 0 || !actor_has_ranged_weapon(actor_index))
   { should_fire = false; goto fire_decided; }
  long flags = actor->flags;
  if (flags & 0x100)
   { should_fire = false; goto fire_decided; }
  long unit_index = actor->unit_index;
  if (!unit_animation_weapon_raised(unit_index))
   { should_fire = false; goto fire_decided; }
  if ((engage_properties && TEST_BIT((unsigned long)engage_properties->flags, 4) && TEST_BIT(flags, 2)) ||
   (actor->vehicle_index != -1 && TEST_BIT(flags, 2)))
   { should_fire = false; goto fire_decided; }
  if (unit && unit->object.type == 0)
  {
   bool stunned = false;
   unit_datum *biped = actor_combat_update_object_get(unit_index);
   if (biped->object.type == 0)
    stunned = ((biped_datum *)biped)->biped.stun_ticks > 0;
   if (stunned)
    { should_fire = false; goto fire_decided; }
  }
  short fire_state = actor->combat_aim_mode;
  if (fire_state == 4)
  {
   should_fire = true;
   goto fire_decided;
  }
  if (actor->uses_clump)
  {
   if (actor->clump_index == -1 ||
    ((clump_datum *)clump_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->clump_index)].combat_state != 1)
    { should_fire = false; goto fire_decided; }
  }
  if (!weapon_properties)
   { should_fire = false; goto fire_decided; }
  character_weapon_properties *properties = (character_weapon_properties *)weapon_properties;
  if (unit)
  {
   if ((!actor->combat_flag384 && TEST_BIT(properties->flags, 1) && unit->unit.crouch_amount < k_actor_combat_update_one) ||
    (actor_combat_update_header_get(unit_index)->object_type == 12 && giant_is_animation_state_current(unit_index)))
    { should_fire = false; goto fire_decided; }
  }

  if (actor->bypass && weapon_definition->barrel_count > 1)
  {
   actor->flags = flags | 0x4000;
  }
  else if (properties->special_fire_mode > 0)
  {
   float chance = properties->special_fire_chance;
   bool special = true;
   if (fire_state == 2 || actor->special_fire_ticks > 0 || actor->special_fire_situation > 0)
    special = false;
   if (special)
   {
    game_difficulty_get_team_value(18, (e_game_team)actor->team);
    switch (properties->special_fire_mode)
    {
    case 1:
     chance *= game_difficulty_get_team_value(17, (e_game_team)actor->team);
     if (weapon_definition->barrel_count <= 0)
      special = false;
     break;
    case 2:
     special = weapon_definition->barrel_count > 1;
     break;
    }
   }
   if (special)
    special = actor_combat_enable_special_fire_situation(actor_index, properties->fire_mode);
   if (special)
   {
    float time = combat_real_random_range(combat_random_seed_address(), 0.0f, 1.5f) + properties->special_fire_time;
    float roll = (float)actor_combat_random(combat_random_seed_address()) * 1.5259022e-05f;
    actor->special_fire_ticks = (short)actor_combat_update_seconds_to_ticks(time);
    special = roll < chance;
   }
   if (special)
    special = actor_combat_allow_special_fire_situation(actor_index, properties->fire_mode);
   if (special)
   {
    if ((unsigned short)properties->fire_mode == 3)
     actor->special_fire_situation = properties->fire_mode;
    switch (properties->special_fire_mode)
    {
    case 1:
     actor->flags |= 0x1000;
     break;
    case 2:
     actor->flags |= 0x4000;
     break;
    }
   }
  }

  short firing_type = actor->firing_type;
  if (firing_type > 0)
  {
   long visibility_interval = actor_combat_update_seconds_to_ticks(0.33f);
   if (firing_type == 1)
   {
    long prop_ref_index = actor->target_prop_ref_index;
    prop_ref_datum *prop_ref = actor_combat_prop_ref_get(prop_ref_index);
    prop_state *state = prop_state_get((const prop_ref_datum *)prop_ref);
    actor->target_distance = prop_ref->distance;
    actor_get_target_position(actor_index, prop_ref_index, &target_position, &actor->target_point);
    target_valid = true;
    if (TEST_BIT(prop_ref->flags, 2))
    {
     actor->target_visibility = 4;
    }
    else
    {
     switch (prop_ref->awareness)
     {
     case 1:
      actor->target_visibility = 1;
      break;
     case 2:
     case 3:
      actor->target_visibility = 0;
      break;
     default:
      actor->target_visibility = 4;
      break;
     }
    }
    if (state->b6d)
     actor->flags |= 0x20000;
    else
     actor->flags &= ~0x20000;
    if (!game_test_cluster_activation(&state->location.cluster_reference))
     actor->flags |= 0x100000;
    else
     actor->flags &= ~0x100000;
   }
   else
   {
    actor->target_point = actor->target_manual_point;
    actor->target_distance = sqrtf(ai_distance_squared3d(&actor->target_manual_point, &actor->position.head));
    actor->flags &= ~0x20000;
    actor->flags &= ~0x100000;
    ai_point_get_position(&actor->target_point, &target_position);
    target_valid = true;
    if (actor->fire_target_ticks % visibility_interval == 0)
    {
     s_cluster_reference invalid_cluster;
     invalid_cluster.value() = -1;
     s_cluster_reference cluster;
     if ((unsigned char)actor->position.location.cluster_reference.bsp_index != 255)
      cluster = actor->position.location.cluster_reference;
     else
      cluster.value() = -1;
     long ignore_index = actor->unit_index;
     while (ignore_index != -1)
     {
      unit_datum *object = actor_combat_update_object_get(ignore_index);
      if (!actor_combat_update_flag_test(object->object.flags, 26))
       break;
      ignore_index = object->object.parent_object_index;
     }
     actor->target_visibility = ai_test_line_of_sight(&actor->position.head, cluster, &target_position,
      invalid_cluster, 0, true, -1, ignore_index, actor->vehicle_index != -1, target_valid, 0, 0);
    }
   }
   actor->flags &= ~0x40000;
   if (actor->combat_flag665 && !(properties->bombardment_range <= 0.0f))
    actor->flags |= 0x80000;
   else
    actor->flags &= ~0x80000;
   if (!actor_weapon_aim(actor_index, weapon_index, 0, &gun_position, &target_position, TEST_BIT(actor->flags, 18),
    &actor->target_aim_vector, 0, &actor->target_projectile_speed, 0, 0))
    actor->firing_type = 0;
  }

  if (actor->firing_type == 0)
   { should_fire = false; goto fire_decided; }
  flags = actor->flags;
  if (TEST_BIT(flags, 20))
   { should_fire = false; goto fire_decided; }
  bool ready = actor->ready;
  if ((!ready && actor->combat_ticks90e > 0) ||
   (actor_move_animation_busy(actor_index) && !actor_can_fire_while_moving(actor_index)) ||
   (!ready && actor->combat_flag380 && !actor->combat_flag339))
   { should_fire = false; goto fire_decided; }
  if (TEST_BIT(flags, 17) || actor->combat_flag381 || !TEST_BIT(flags, 11))
   { should_fire = false; goto fire_decided; }
  if (actor->fire_target_type == 0)
  {
   if (!aiming_at_target(actor_index))
    { should_fire = false; goto fire_decided; }
   bool looking = false;
   actor_datum *looker = actor_get_direct(actor_index);
   if (looker->combat_look_mode == 0)
   {
    if (looker->weapon_state_ticks >= 3)
     looking = aiming_at_target(actor_index);
   }
   else if (looker->combat_look_mode == 2)
   {
    if (looker->aim_direction.type == 2 || (looker->aim_direction.type == 1 && looker->aim_direction.object_index == looker->combat_target_prop_index))
     looking = true;
   }
   if (!looking)
    { should_fire = false; goto fire_decided; }
  }
  if (actor->combat_aim_mode == 2)
  {
   should_fire = true;
   goto fire_decided;
  }
  if (actor->target_visibility == 0 || actor->target_visibility == 1)
   actor->flags = flags | 0x10000;
  else
   actor->flags = flags & ~0x10000;
  if (!actor->allow_unseen_target_fire && !TEST_BIT(actor->flags, 16) && !TEST_BIT(actor->flags, 19))
   { should_fire = false; goto fire_decided; }
  if (!ready && has_ranged_weapon && actor->target_distance >= properties->maximum_range)
   { should_fire = false; goto fire_decided; }
  if (!ready && has_ranged_weapon && actor->target_distance <= properties->maximum_firing_range)
   { should_fire = false; goto fire_decided; }
  should_fire = true;
  actor->flags |= 0x800000;
  if (!TEST_BIT(properties->flags, 3))
  {
   float threshold;
   if (actor->target_distance < 1.5f)
    threshold = actor->target_distance * 0.17526217f + k_actor_combat_update_sqrt_half;
   else
    threshold = 0.97f;
   real_vector3d weapon_vector;
   actor_get_weapon_vector(actor_index, &weapon_vector);
   const real_vector3d &target_aim = actor->target_aim_vector;
   const float weapon_alignment = target_aim.k * weapon_vector.k + target_aim.j * weapon_vector.j + target_aim.i * weapon_vector.i;
   if (weapon_alignment < threshold)
    hold_burst_start = true;
  }
 }
fire_decided:

 if (should_fire && actor->firing_type == 1)
 {
  unsigned char *const &target_refs = (unsigned char *)tls_prop_ref_data()->data;
  const long &target_ref_offset = DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->target_prop_ref_index) * (long)sizeof(prop_ref_datum);
  const prop_ref_datum *target_ref = (const prop_ref_datum *)(target_refs + target_ref_offset);
  long target_index = actor_combat_prop_get(target_ref->prop_index)->object_index;
  s_object_header *target_header = actor_combat_update_header_get(target_index);
  if ((1 << target_header->object_type) & 0x1003)
  {
   unit_datum *target = (unit_datum *)target_header->datum;
   long vehicle_index = target->object.parent_object_index;
   if (vehicle_index != -1 && (unsigned short)target->unit.parent_seat_index != 0xffff)
   {
    s_object_header *vehicle_header = actor_combat_update_header_get(vehicle_index);
    if ((1 << vehicle_header->object_type) & 2)
    {
     long target_root = actor_combat_update_ultimate_parent(target_index);
     long actor_root = actor_combat_update_ultimate_parent(actor->unit_index);
     long squad_index;
     short squad_team;
     if (target_root != actor_root && unit_seat_test_flag(vehicle_index, target->unit.parent_seat_index, _unit_seat_boarding_seat_bit))
     {
      squad_index = ((vehicle_datum *)vehicle_header->datum)->vehicle.squad_index;
      if (squad_index != -1)
      {
       squad_team = datum_get<squad_datum>(squad_data, squad_index)->team;
       if (actor->team == squad_team || game_team_is_ally((e_game_team)actor->team, (e_game_team)squad_team))
       {
        should_fire = false;
       }
      }
     }
     else
     {
      unit_seat_source seats[64];
      short seat_count = 0;
      unit_get_all_seats_internal(target_root, (unit_seat_source *)seats, &seat_count, 64, 0);
      short count = seat_count;
      for (short seat = 0; seat < count; seat++)
      {
       if (actor_combat_update_flag_test(seats[seat].definition->flags, 11))
       {
        long occupant_index = unit_seat_get_occupant(seats[seat].unit_index, seats[seat].seat_index);
        if (occupant_index != -1)
        {
         unit_datum *occupant = actor_combat_update_object_get(occupant_index);
         if (actor->team == occupant->unit.team || game_team_is_ally((e_game_team)actor->team, (e_game_team)occupant->unit.team))
         {
          should_fire = false;
          break;
         }
        }
       }
      }
     }
    }
   }
  }
 }

 if (weapon_definition && weapon_definition->barrel_count > 0)
 {
  long projectile_definition_index = weapon_definition->barrels[0].projectile_index;
  if (projectile_definition_index != -1)
   collateral_damage_radius = ((const projectile_definition *)global_tag_instances[
       DATUM_INDEX_TO_ABSOLUTE_INDEX(projectile_definition_index)].definition)->ai_radius;
 }
 if (should_fire && target_valid && (short)actor->firing_type > 0 && collateral_damage_radius > 0.0f)
 {
  if (!actor_combat_check_collateral_damage(actor_index, 0.0f, collateral_damage_radius + k_actor_combat_update_one, &target_position, 0, 0))
   should_fire = false;
 }
 if (!should_fire)
  actor->combat_aim_mode = 0;
 long fire_state = actor->combat_aim_mode;
 switch (fire_state)
 {
 case 0:
  if (should_fire)
   firing_decision = 1;
  break;
 case 1:
  if (!hold_burst_start && actor->delay == 0)
   firing_decision = 2;
  break;
 case 3:
  if (!hold_burst_start && actor->delay == 0)
   firing_decision = 2;
  break;
 case 2:
  if (actor->delay == 0)
   firing_decision = 3;
  break;
 case 4:
  if (actor->delay == 0)
   firing_decision = 0;
  break;
 }

 character_firing_pattern pattern;
 if (weapon_properties)
 {
  character_weapon_properties *properties = (character_weapon_properties *)weapon_properties;
  const float *accuracy;
  switch (actor_combat_update_difficulty())
  {
  case 2:
   accuracy = properties->burst[1];
   break;
  case 3:
   accuracy = properties->burst[2];
   break;
  default:
   accuracy = properties->burst[0];
   break;
  }
  if (!target_changed && fire_state != 1 && fire_state != 4 && fire_state != 0)
   actor->accuracy = (accuracy[1] - accuracy[0]) / ((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * accuracy[2]) + actor->accuracy;
  else
   actor->accuracy = accuracy[0];
  actor->accuracy = PIN(actor->accuracy, 0.0f, accuracy[1]);
  if (!character_firing_pattern_calculate(actor_index, actor->accuracy, &pattern))
   firing_decision = fire_state == 0 ? -1 : 0;
 }
 if (firing_decision != -1)
 {
  switch (firing_decision)
  {
  case 1:
   if (!actor_start_first_burst_delay(actor_index, weapon_properties, &pattern) && !hold_burst_start)
   {
    firing_decision = 2;
    actor_start_burst(actor_index, weapon_index, weapon_properties, &pattern);
   }
   break;
  case 2:
   actor_start_burst(actor_index, weapon_index, weapon_properties, &pattern);
   break;
  case 3:
   actor_start_pause(actor_index, weapon_properties, &pattern);
   break;
  }
  actor->combat_aim_mode = firing_decision;
 }

 if (weapon_properties)
 {
  character_weapon_properties *properties = (character_weapon_properties *)weapon_properties;
  bool fire_primary = false;
  bool fire_secondary = false;
  actor->flags &= ~0x1000000;
  if (actor->combat_aim_mode == 4)
  {
   fire_primary = true;
   actor->firing_start_time = -1;
  }
  else if (actor->combat_aim_mode == 2)
  {
   long ignore_object_index = -1;
   if (TEST_BIT(actor->flags, 13) && weapon_definition)
   {
    if (weapon_definition->barrel_count > 1)
     actor->flags |= 0x2000;
    else
     actor->flags &= ~0x2000;
   }
   ai_point_get_position(&actor->burst_target, &actor->burst_target_position);
   if (actor->firing_type == 1)
   {
    long prop_ref_index = actor->target_prop_ref_index;
    prop_ref_datum *prop_ref = actor_combat_prop_ref_get(prop_ref_index);
    unit_datum *target = actor_combat_update_object_get(prop_ref->object_index);
    prop_state *state = prop_state_get((const prop_ref_datum *)prop_ref);
    if (target->object.parent_object_index != -1 && ((1 << actor_combat_update_header_get(target->object.parent_object_index)->object_type) & 0x1003))
     ignore_object_index = target->object.parent_object_index;
    const character_firing_pattern *firing_pattern = &pattern;
    float tracking = firing_pattern->burst_time;
    float tracking_value = game_difficulty_get_value_by_difficulty(15, actor_combat_update_difficulty()) + tracking;
    if ((tracking_value >= k_actor_combat_update_one || tracking_value > 0.0f) && !TEST_BIT(actor->flags, 19))
    {
     real_point3d burst_position;
     real_point3d target_point;
     real_vector3d delta;
     ai_point_get_position(&actor->burst_target, &burst_position);
     actor_get_target_position(actor_index, prop_ref_index, &target_point, 0);
     vector_from_points3d(&burst_position, &target_point, &delta);
     point_from_line3d(&actor->burst_target_position, &delta, tracking, &actor->burst_target_position);
    }
    float leading = firing_pattern->burst_separation;
    if (leading >= k_actor_combat_update_one || leading > 0.0f)
    {
     real_vector3d velocity;
     scale_vector3d(&state->velocity, weapon_estimate_time_to_target(weapon_index, TEST_BIT(actor->flags, 13), actor->target_projectile_speed), &velocity);
     point_from_line3d(&actor->burst_target_position, &velocity, leading, &actor->burst_target_position);
    }
   }
   const real_vector3d &burst_offset = actor->burst_offset;
   add_vectors3d(&burst_offset, &actor->burst_return, &actor->burst_offset);
   point_from_line3d(&actor->burst_target_position, &actor->burst_offset, 1.0f, &actor->burst_aim_point);
   if (TEST_BIT(actor->flags, 13))
    actor_get_gun_position(actor_index, false, &gun_position);
   float distance_squared = distance_squared3d(&gun_position, &actor->burst_aim_point);
   if (distance_squared < properties->maximum_firing_range * properties->maximum_firing_range)
   {
    real_vector3d direction;
    vector_from_points3d(&gun_position, &actor->burst_aim_point, &direction);
    if (normalize3d(&direction) > 0.0f)
     point_from_line3d(&actor->burst_aim_point, &direction, properties->maximum_firing_range - sqrtf(distance_squared), &actor->burst_aim_point);
   }
   float aim_speed;
   bool aiming_success;
   actor_weapon_aim(actor_index, weapon_index, TEST_BIT(actor->flags, 13), &gun_position, &actor->burst_aim_point, TEST_BIT(actor->flags, 18),
    &actor->combat_aim_vector, 0, 0, &aim_speed, &aiming_success);
   if (!aiming_success)
    actor->flags |= 0x1000000;
   else
    actor->flags &= ~0x1000000;
   if (TEST_BIT(actor->flags, 24))
    actor->aim_field9a8 = aim_speed;
   real_point3d unit_gun_position;
   if (unit_get_gun_position(actor->unit_index, true, &unit_gun_position))
    gun_position = unit_gun_position;
   real_vector3d fire_vector;
   long hit_prop_ref_index = -1;
   vector_from_points3d(&gun_position, &actor->burst_aim_point, &fire_vector);
   bool clear = false;
   if (!ai_test_line_of_fire(actor_index, ignore_object_index, &gun_position, &fire_vector, &hit_prop_ref_index) ||
    (collateral_damage_radius > 0.0f && !ai_test_short_line_of_sight(actor_index, ignore_object_index, &gun_position, &actor->burst_aim_point)))
    clear = true;
   if (!clear)
   {
    actor->clear_line_of_fire_ticks = 0;
    if (TEST_BIT(actor->flags, 13))
     fire_secondary = true;
    else
     fire_primary = true;
   }
   else
   {
    if (actor->clear_line_of_fire_ticks < TLS_GAME_TIME_GLOBALS->ticks_per_second * 60)
     actor->clear_line_of_fire_ticks++;
    actor->delay++;
    if (hit_prop_ref_index != -1)
    {
     bool exempt = false;
     actor_datum *self = datum_get<actor_datum>(actor_data, actor_index);
     if (self->path.valid && self->combat_value724 == 1)
      exempt = true;
     if (!exempt && actor->vehicle_index == -1 && target_get_prop_class((actor_datum *)actor) >= 7)
     {
      prop_ref_datum *hit_ref = actor_combat_prop_ref_get(hit_prop_ref_index);
      prop_datum *hit_prop = actor_combat_prop_get(hit_ref->prop_index);
      if (hit_prop->hostile && !hit_prop->valid)
      {
       if (actor->friendly_blocked_time != TLS_GAME_TIME_GLOBALS->time - 1)
        actor->friendly_blocked_ticks = 0;
       if (actor->friendly_blocked_ticks < 0x7fff)
        actor->friendly_blocked_ticks++;
       if (actor->friendly_blocked_ticks == actor_combat_update_seconds_to_ticks(k_actor_combat_update_one))
        actor_dialogue_event(actor_index, 110, hit_ref->object_index, 0);
       actor->friendly_blocked_time = TLS_GAME_TIME_GLOBALS->time;
      }
     }
    }
   }
  }

  float trigger = 0.0f;
  bool primary_trigger = false;
  bool secondary_trigger = false;
  if (fire_primary)
  {
   if (TEST_BIT(actor->flags, 12))
   {
    actor->flags &= ~0x1000;
   }
   else if (pattern.rate < 0.0001f)
   {
    trigger = k_actor_combat_update_one;
    primary_trigger = true;
   }
   else if (actor->burst_cooldown_ticks == 0)
   {
    trigger = k_actor_combat_update_one;
    primary_trigger = true;
    short ticks = (short)actor_combat_real_to_long_round((float)TLS_GAME_TIME_GLOBALS->ticks_per_second / pattern.rate);
    if (ticks < 2)
     ticks = 2;
    actor->burst_cooldown_ticks = ticks;
   }
  }
  else if (fire_secondary)
  {
   if (TEST_BIT(actor->flags, 12))
    actor->flags &= ~0x1000;
   else
    secondary_trigger = true;
  }
  else if (TEST_BIT(actor->flags, 12))
  {
   trigger = k_actor_combat_update_one;
   primary_trigger = true;
  }
  if (primary_trigger)
   actor->control_flags |= 0x40000;
  else
   actor->control_flags &= ~0x40000;
  actor->control_trigger_primary = trigger;
  if (secondary_trigger)
   actor->control_flags |= 0x80000;
  else
   actor->control_flags &= ~0x80000;
  if (actor->gun_uses_parent)
  {
   if (secondary_trigger)
    actor->control_flags |= 0x80000000;
   else
    actor->control_flags &= ~0x80000000;
  }
  if (secondary_weapon_index != -1)
  {
   if (primary_trigger)
    actor->control_flags |= 0x800000;
   else
    actor->control_flags &= ~0x800000;
   actor->control_trigger_secondary = trigger;
   if (secondary_trigger)
    actor->control_flags |= 0x1000000;
   else
    actor->control_flags &= ~0x1000000;
  }
 }
}
