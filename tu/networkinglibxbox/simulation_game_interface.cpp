// networkinglibxbox/simulation_game_interface.obj: from the records 0x3d3598 0x3d3608 0x3d3930 0x3d3b60 0x3d3cf8 0x3d3ef8 0x3d40f8 0x3d4260 0x3d4490 0x3d45a0 0x3d4678 0x3d47a8 0x70c590
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_constant_pool.h"
#include "math/real_math_constants.h"
#include "motor/mover_action_state.h"
// @address 0x7bb328 g_simulation_object_refresh_event_definition
#include "simulation/player_action.h"
#include "math/real_math_functions.h"
#include "simulation/simulation_game_position_update_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "game/players.h"
#include "objects/objects.h"
#include "units/units.h"
#include "interface/chud/chud_motion_sensor.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/game_interface/simulation_game_engine_sandbox.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "motor/action_system.h"
#include "networking/replication/replication_prediction.h"
#include "cache/cache_files.h"
struct s_player_prediction;
struct s_aim_assist_targeting_result;
struct s_player_predicted_aim_assist;
// @address 0x3d47a8 ?simulation_game_build_predicted_weapon@@YAXJPBUs_aim_assist_targeting_result@@PAUs_player_motion@@@Z
// @address 0x3d3b60 simulation_game_build_motion_data_from_control
// @address 0x3d40f8 ?simulation_game_build_player_prediction@@YA_NJPAUs_player_prediction@@@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x1342c8 ?object_velocity_exceeds_threshold@@YA_NJ@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xa1970 ?player_action_clear@@YAXPAUplayer_action@@@Z
// @address 0x40aac0 ?simulation_object_get_authoritative_entity_internal@@YAJJ_N@Z
// @address 0x40abb8 ?simulation_object_get_predictive_entity_internal@@YAJJ_N@Z
// @address 0x40a9f0 ?simulation_object_get_replicated_object_from_entity@@YAJJ@Z
// @address 0x995c8 ?vector3d_from_euler_angles2d@@YAPATreal_vector3d@@PAT1@PBTreal_euler_angles2d@@@Z
// @address 0xa8730 ?unit_control_clear@@YAXPAUunit_control_data@@@Z
union real_vector3d;

#include "networking/replication/replication_motion.h"
#include "units/unit_inventory_inline.h"
#include "units/unit_types.h"
#include "ai/weapon_datum_types.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
// match: 0x38e088: canonical C TLS linkage changes biped_leap post-call reloads.
// Retain this shared legacy slot declaration until its other users migrate together.
#include "simulation/game_interface/simulation_game_interface_globals.h"
#include "game/game_time_access.h"

// dev: simulation_game_build_predicted_aim_assist(const s_aim_assist_targeting_result *, s_player_predicted_aim_assist *)
#include "simulation/simulation_globals.h"
// @address 0x808560 g_simulation_globals
#include "simulation/simulation_world_types.h"



#include "items/weapon_definitions.h"

#include "networking/simulation/simulation_type_collection.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_game_event_definitions.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/game_interface/simulation_game_vehicles_internals.h"
#include "simulation/game_interface/simulation_game_generics.h"
#include "simulation/game_interface/simulation_game_projectiles.h"
#include "simulation/game_interface/simulation_game_engine_player_definitions.h"
#include "simulation/game_interface/simulation_game_statborg_entity_definition.h"
#include "simulation/game_interface/simulation_game_units.h"
#include "simulation/game_interface/simulation_game_weapons.h"
#include "simulation/game_interface/simulation_game_items.h"
#include "simulation/game_interface/simulation_game_device_definitions.h"
#include "simulation/game_interface/simulation_game_breakable_surfaces.h"
#include <string.h>
#include "cseries/bit_operations.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header_access.h"
#include "math/real_constant_pool_array_legacy.h"
#include "memory/datum_index.h"

static __forceinline const s_unit_weapon_set *simulation_game_motion_weapon(const s_player_motion *motion)
{
    return (const s_unit_weapon_set *)&motion->weapon_set;
}



// 0x3d4260


long simulation_calculate_transmission_period(float relevance, float maximum_relevance,
    float minimum_relevance, long minimum_period, long maximum_period)
{
    long period;
    if (relevance >= maximum_relevance)
        period = minimum_period;
    else if (relevance <= minimum_relevance)
        period = maximum_period;
    else
        period = (long)((float)maximum_period -
            ((relevance - minimum_relevance) / (maximum_relevance - minimum_relevance)) *
            (float)(maximum_period - minimum_period));
    return period;
}


// @address 0x3d3cf8 simulation_game_reconstruct_simulated_action
// match: 0x3d3cf8: pointer __restrict qualifier prevents alias reload
extern "C" void simulation_game_reconstruct_simulated_action(const s_player_motion *motion,player_action *__restrict action){
 player_action_clear(action);
 action->unit_valid=motion->type;
 action->unit_flags=motion->identifier;
 action->facing=motion->facing;
 action->move_vector=motion->throttle;
 SET_BIT(action->flags,0,TEST_BIT(motion->flags,0));
 SET_BIT(action->flags,1,TEST_BIT(motion->flags,1));
 SET_BIT(action->flags,11,TEST_BIT(motion->flags,2));
 SET_BIT(action->flags,14,TEST_BIT(motion->flags,3));
 SET_BIT(action->extra_short,1,TEST_BIT(motion->flags,5));
 SET_BIT(action->extra_short,0,TEST_BIT(motion->flags,4));
 action->weapons = *simulation_game_motion_weapon(motion);
 action->zoom=motion->zoom;
 SET_BIT(action->flags,20,motion->fire[0]);
 SET_BIT(action->flags,21,motion->fire[1]);
 SET_BIT(action->flags,25,motion->fire[2]);
 SET_BIT(action->flags,26,motion->fire[3]);
 ::simulation_game_reconstruct_predicted_aim_assist(&motion->aim_assist, &action->aim_assist);
 action->velocity_exceeds_motion_tracker_threshold = motion->exceeded;
}


union real_euler_angles2d;


// @address 0x3d3ef8 simulation_game_reconstruct_simulated_control
// match: 0x3d3ef8: pointer __restrict qualifier prevents alias reload
extern "C" void simulation_game_reconstruct_simulated_control(const s_player_motion *motion, unit_control_data *__restrict control)
{
    unit_control_clear(control);
    if (motion->type == 1)
    {
        vector3d_from_euler_angles2d(&control->aiming, (const real_euler_angles2d *)&motion->facing);
        control->facing = control->aiming;
        control->looking = control->aiming;
        control->throttle.xy = motion->throttle;
        if (TEST_BIT(motion->flags, 4))
#if defined(_XBOX) && !defined(__clang__)
            // match: 0x3d3ef8: 1.0f loads off the 0x8202251c base (lis/addi, lfs -0x320); a spelled literal drops the addi
            control->throttle.k = real_constant_pool_zero_array()[-200];
#else
            control->throttle.k = 1.0f;
#endif
        else if (TEST_BIT(motion->flags, 5))
            control->throttle.k = -1.0f;
        else
            control->throttle.k = 0.0f;
        SET_BIT(control->control_flags, 0, TEST_BIT(motion->flags, 0));
        SET_BIT(control->control_flags, 1, TEST_BIT(motion->flags, 1));
        SET_BIT(control->control_flags, 11, TEST_BIT(motion->flags, 2));
        SET_BIT(control->control_flags, 14, TEST_BIT(motion->flags, 3));
        control->weapon_set = *simulation_game_motion_weapon(motion);
        control->zoom_level = motion->zoom;
        SET_BIT(control->control_flags, 20, motion->fire[0]);
        SET_BIT(control->control_flags, 21, motion->fire[1]);
        SET_BIT(control->control_flags, 25, motion->fire[2]);
        SET_BIT(control->control_flags, 26, motion->fire[3]);
        ::simulation_game_reconstruct_predicted_aim_assist(&motion->aim_assist, &control->targeting);
    }
}



// @address 0x3d4490 ?simulation_game_build_predicted_aim_assist@@YAXPBUs_aim_assist_targeting_result@@PAUs_player_predicted_aim_assist@@@Z
void simulation_game_build_predicted_aim_assist(struct s_aim_assist_targeting_result const * aim_assist, struct s_player_predicted_aim_assist * prediction)
{
    long object_index;
    long entity;
    bool authoritative;
    if ((aim_assist->flags & 4) == 0)
        return;
    object_index = aim_assist->target_object_index;
    if (object_index == -1)
        return;
    if (object_try_and_get_and_verify_type(object_index, (unsigned long)-1) == 0)
        return;
    authoritative = g_simulation_globals.world->m_world_type != 4 && g_simulation_globals.world->m_world_type != 6
        && g_simulation_globals.world->m_world_type != 8;
    if (authoritative)
        entity = simulation_object_get_authoritative_entity_internal(object_index, true);
    else
        entity = simulation_object_get_predictive_entity_internal(object_index, true);
    if (entity == -1)
        return;
    prediction->target = entity;
    prediction->model_target = aim_assist->model_target;
    prediction->primary_autoaim_level = aim_assist->primary_autoaim_level;
    prediction->secondary_autoaim_level = aim_assist->secondary_autoaim_level;
    prediction->only_lead_vector_valid = (aim_assist->flags & 1) != 0;
    prediction->lead_vector_valid = ((aim_assist->flags >> 2) & 1) != 0;
    if (prediction->lead_vector_valid)
        prediction->lead_vector = aim_assist->lead_vector;
}

// @address 0x3d45a0 ?simulation_game_reconstruct_predicted_aim_assist@@YAXPBUs_player_predicted_aim_assist@@PAUs_aim_assist_targeting_result@@@Z
void simulation_game_reconstruct_predicted_aim_assist(struct s_player_predicted_aim_assist const * prediction,
    struct s_aim_assist_targeting_result * aim_assist)
{
    if (prediction->target != -1) {
        int id = simulation_object_get_replicated_object_from_entity(prediction->target);
        if (id != -1) {
            aim_assist->target_object_index = id;
            aim_assist->model_target = prediction->model_target;
            aim_assist->primary_autoaim_level = prediction->primary_autoaim_level;
            aim_assist->secondary_autoaim_level = prediction->secondary_autoaim_level;
            if (prediction->only_lead_vector_valid)
                aim_assist->flags |= 1;
            else
                aim_assist->flags &= ~1;
            if (prediction->lead_vector_valid)
                aim_assist->flags |= 4;
            else
                aim_assist->flags &= ~4;
            if (prediction->lead_vector_valid)
                aim_assist->lead_vector = prediction->lead_vector;
        }
    }
}




static __forceinline long u_networkinglibxbox_simulation_game_interface_object_get_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1)
    {
        result = object_index;
        object_index = ((unit_datum *)object_get_and_verify_type(
                            object_index, (unsigned long)-1))
                           ->object.parent_object_index;
    }
    return result;
}

void simulation_game_build_predicted_weapon(long unit_index, const s_aim_assist_targeting_result *aim_assist_targeting, s_player_motion *player_motion)
{
    long weapon_indices[2];
    player_motion->aim_assist.primary_autoaim_level = 0.0f;
    player_motion->fire[0] = 0;
    player_motion->aim_assist.secondary_autoaim_level = 0.0f;
    player_motion->aim_assist.target = -1;
    player_motion->aim_assist.model_target = -1;
    player_motion->fire[1] = 0;
    player_motion->fire[2] = 0;
    player_motion->fire[3] = 0;
    player_motion->aim_exists = 0;
    if (unit_index == -1)
        return;
    unit_datum *unit =
        (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    weapon_indices[0] = unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_index);
    weapon_indices[1] = unit_inventory_get_weapon_or_none(unit_index, unit->unit.secondary_weapon_index);
    if (weapon_indices[0] == -1 && weapon_indices[1] == -1)
    {
        long parent_index = u_networkinglibxbox_simulation_game_interface_object_get_ultimate_parent(unit_index);
        if (parent_index != -1)
        {
            unit_datum *parent =
                (unit_datum *)object_get_and_verify_type(
                    parent_index, (unsigned long)-1);
            if (((1 << parent->object.object_type) & 0x1003) != 0 && parent->unit.gunner_object_index == unit_index)
            {
                weapon_indices[0] = unit_inventory_get_weapon_or_none(parent_index, parent->unit.current_weapon_index);
                weapon_indices[1] = unit_inventory_get_weapon_or_none(parent_index, parent->unit.secondary_weapon_index);
            }
        }
        if (weapon_indices[0] == -1 && weapon_indices[1] == -1)
            get_ultimate_parent_weapon_control_weapon_indicies((unsigned int)unit_index, (int *)weapon_indices);
    }

    long *weapon_index = weapon_indices;
    for (long index = 0; index < 2; index++)
    {
        if (*weapon_index != -1)
        {
            s_data_array *header_array = object_header_data;
            s_object_header *weapon_header = &((s_object_header *)header_array->data)[datum_index_to_absolute_index(*weapon_index)];
            weapon_datum *weapon = (weapon_datum *)weapon_header->datum;
            weapon_definition *definition =
                (weapon_definition *)tag_get(0x77656170, weapon->definition_index);
            if (definition->trigger_count > 0)
            {
                long trigger_index = 0;
                for (;trigger_index < definition->trigger_count;trigger_index++)
                {
                    s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
                    s_weapon_trigger_definition *trigger_definition = &definition->triggers[trigger_index];
                    if (trigger_definition->prediction_type != 1)
                    {
                        if (trigger_definition->prediction_type == 2 && (trigger->state == 1 || trigger->state == 2))
                            player_motion->fire[(index << 1) + trigger_index] = 1;
                    }
                    else if ((trigger->flags & 0x20) != 0 || trigger->mispredict_ticks != 0)
                    {
                        if (trigger_definition->primary_barrel != -1 && definition->barrels[trigger_definition->primary_barrel].prediction_type == 1)
                        {
                            player_motion->fire[(index << 1) + trigger_index] = 1;
                            player_motion->aim_exists = 1;
                        }
                    }
                    if (trigger->state == 5 || trigger->state == 6)
                        player_motion->aim_exists = 1;
                }
            }
        }
        weapon_index++;

    }
    if (player_motion->aim_exists)
        ::simulation_game_build_predicted_aim_assist(aim_assist_targeting, &player_motion->aim_assist);
}


// @address 0x3d4678 get_ultimate_parent_weapon_control_weapon_indicies
extern "C" void get_ultimate_parent_weapon_control_weapon_indicies(unsigned int controlled_unit_index,int *control_weapon_indices)

{
  s_object_header *object_headers;
  unit_datum *candidate_datum;
  unsigned int control_chain_index;
  int resolved_weapon_index;
  unsigned int ultimate_parent_index;
  int current_object_index;
  current_object_index = controlled_unit_index;
  
  if (controlled_unit_index == 0xffffffff) {
    return;
  }
  object_headers = (s_object_header *)object_header_data->data;
  do {
    ultimate_parent_index = current_object_index;
    current_object_index = object_headers[datum_index_to_absolute_index(ultimate_parent_index)].datum->parent_object_index;
  } while (current_object_index != 0xffffffff);
  if (ultimate_parent_index == 0xffffffff) {
    return;
  }
  current_object_index = object_headers[datum_index_to_absolute_index(ultimate_parent_index)].datum->first_child_object_index;
  if (current_object_index == 0xffffffff) {
    return;
  }
  do {
    candidate_datum = (unit_datum *)object_headers[datum_index_to_absolute_index(current_object_index)].datum;
    if (((int)((1 << ((int)candidate_datum->object.object_type) & 0x1003U)) != 0) &&
       ((unsigned int)candidate_datum->unit.gunner_object_index != 0xffffffff)) {
      unsigned __int64 link = (unsigned int)candidate_datum->unit.gunner_object_index;
      current_object_index = -1;
      while (control_chain_index = (int)link, (int)link != -1) {
        current_object_index = control_chain_index;
        link = (unsigned int)((unit_datum *)object_headers[datum_index_to_absolute_index(control_chain_index)].datum)->unit.gunner_object_index;
      }
      if ((int)current_object_index == (int)controlled_unit_index) {
        if ((int)(*control_weapon_indices) == -1) {
          resolved_weapon_index = -1;
          if (candidate_datum->unit.current_weapon_index != -1) {
            resolved_weapon_index = candidate_datum->unit.weapon_object_indices[candidate_datum->unit.current_weapon_index];
          }
          *control_weapon_indices = resolved_weapon_index;
        }
        if ((int)(control_weapon_indices[1]) == -1) {
          resolved_weapon_index = -1;
          if (candidate_datum->unit.secondary_weapon_index != -1) {
            resolved_weapon_index = candidate_datum->unit.weapon_object_indices[candidate_datum->unit.secondary_weapon_index];
          }
          control_weapon_indices[1] = resolved_weapon_index;
        }
      }
    }
    current_object_index = candidate_datum->object.next_object_index;
  } while (current_object_index != 0xffffffff);
  return;
}

// @address 0x7bb470 g_simulation_sandbox_engine_globals_definition

// @address 0x7bb448 g_simulation_player_respawn_request_event_definition
// @address 0x7bb44c g_simulation_player_editor_request_event_definition
// @address 0x7bb488 g_simulation_unit_enter_vehicle_event_definition
// @address 0x7bb498 g_simulation_unit_grenade_release_event_definition
// @address 0x7bb49c g_simulation_unit_melee_initiate_event_definition
// @address 0x7bb4a0 g_simulation_unit_melee_damage_event_definition
// @address 0x7bb4a8 g_simulation_vehicle_flip_event_definition
// @address 0x7bb4ac g_simulation_vehicle_trick_event_definition
// @address 0x7bb4b0 g_simulation_unit_pickup_event_definition
// @address 0x7bb4b4 g_simulation_unit_equipment_use_event_definition
// @address 0x7bb4e0 g_simulation_projectile_effect_event_definition
// @address 0x7bb508 g_simulation_weapon_reload_event_definition
// @address 0x7bb514 g_simulation_weapon_drop_event_definition
// @address 0x7bb518 g_simulation_weapon_effect_event_definition
// @address 0x7bb51c g_simulation_weapon_empty_click_event_definition
// @address 0x7bb524 g_simulation_device_touch_event_definition
// @address 0x7bb528 g_simulation_game_engine_event_definition
// @address 0x7bb454 g_simulation_damage_aftermath_event_definition
// @address 0x7bb458 g_simulation_damage_section_response_event_definition
// @address 0x7bb45c g_simulation_breakable_surface_damage_event_definition
// @address 0x7bb48c g_simulation_unit_exit_vehicle_event_definition
// @address 0x7bb490 g_simulation_unit_board_vehicle_event_definition
// @address 0x7bb494 g_simulation_unit_grenade_initiate_event_definition
// @address 0x7bb4dc g_simulation_projectile_attached_event_definition
// @address 0x7bb4e4 g_simulation_projectile_impact_effect_event_definition
// @address 0x7bb504 g_simulation_weapon_fire_event_definition
// @address 0x7bb50c g_simulation_weapon_pickup_event_definition
// @address 0x7bb510 g_simulation_weapon_put_away_event_definition
// @address 0x7bb4e8 g_simulation_projectile_object_impact_effect_event_definition

// @address 0x45920 ??_7c_simulation_sandbox_engine_globals_definition@@6B@
// @address 0x70c590 ??__Fthe_instance@?1??instance@c_simulation_sandbox_engine_globals_definition@@SAAAV1@XZ@YAXXZ
// @address 0x7c88a0 ??_B?1??instance@c_simulation_sandbox_engine_globals_definition@@SAAAV1@XZ@51
// @address 0x119a490 ?the_instance@?1??instance@c_simulation_sandbox_engine_globals_definition@@SAAAV2@XZ@4V2@A
inline c_simulation_sandbox_engine_globals_definition::c_simulation_sandbox_engine_globals_definition()
{
    g_simulation_sandbox_engine_globals_next_available_update_bit = 6;
}

inline c_simulation_sandbox_engine_globals_definition &c_simulation_sandbox_engine_globals_definition::instance()
{
    static c_simulation_sandbox_engine_globals_definition the_instance;
    return the_instance;
}

// @address 0x3d3608 ?simulation_game_register_types@@YAXPAVc_simulation_type_collection@@PAJ1@Z
void simulation_game_register_types(c_simulation_type_collection *simulation_type_collection,long *entity_count,long *event_count) {
 // match: 0x3d3608: the collection qualifier lets the sandbox guard load precede the stores for entity types 3 through 6.
 c_simulation_type_collection *__restrict types=simulation_type_collection;
 long * entity_type_count=entity_count;
 // match: 0x3d3608: the event-count qualifier keeps the final count store in r10 through the pointer in r11.
 long *__restrict event_type_count=event_count;
 types->register_entity_definition(0,&g_simulation_slayer_engine_globals_definition);
 types->register_entity_definition(1,&g_simulation_ctf_engine_globals_definition);
 types->register_entity_definition(2,&g_simulation_assault_engine_globals_definition);
 types->register_entity_definition(3,&g_simulation_oddball_engine_globals_definition);
 types->register_entity_definition(4,&g_simulation_king_engine_globals_definition);
 types->register_entity_definition(5,&g_simulation_territories_engine_globals_definition);
 types->register_entity_definition(6,&g_simulation_juggernaut_engine_globals_definition);
 types->register_entity_definition(7,&c_simulation_sandbox_engine_globals_definition::instance());
 types->register_entity_definition(8,&g_simulation_infection_engine_globals_definition);
 types->register_entity_definition(9,&g_simulation_vip_engine_globals_definition);
 types->register_entity_definition(10,&g_simulation_game_engine_player_entity_definition);
 types->register_entity_definition(11,&g_simulation_game_statborg_entity_definition);
 types->register_entity_definition(12,&g_simulation_breakable_surface_group_entity_definition);
 types->register_entity_definition(13,&g_simulation_sandbox_engine_globals_definition);
 types->register_entity_definition(14,&g_simulation_unit_entity_definition);
 types->register_entity_definition(15,&g_simulation_item_entity_definition);
 types->register_entity_definition(16,&g_simulation_generic_entity_definition);
 types->register_entity_definition(17,&g_simulation_generic_garbage_entity_definition);
 types->register_entity_definition(18,&g_simulation_vehicle_entity_definition);
 types->register_entity_definition(19,&g_simulation_projectile_entity_definition);
 types->register_entity_definition(20,&g_simulation_weapon_entity_definition);
 types->register_entity_definition(21,&g_simulation_device_entity_definition);
 types->register_event_definition(0,&g_simulation_damage_aftermath_event_definition);
 types->register_event_definition(1,&g_simulation_damage_section_response_event_definition);
 types->register_event_definition(3,&g_simulation_projectile_attached_event_definition);
 types->register_event_definition(4,&g_simulation_projectile_effect_event_definition);
 types->register_event_definition(5,&g_simulation_projectile_impact_effect_event_definition);
 types->register_event_definition(6,&g_simulation_projectile_object_impact_effect_event_definition);
 types->register_event_definition(7,&g_simulation_game_engine_event_definition);
 types->register_event_definition(8,&g_simulation_unit_board_vehicle_event_definition);
 types->register_event_definition(9,&g_simulation_unit_pickup_event_definition);
 types->register_event_definition(2,&g_simulation_breakable_surface_damage_event_definition);
 types->register_event_definition(10,&g_simulation_weapon_effect_event_definition);
 types->register_event_definition(11,&g_simulation_weapon_empty_click_event_definition);
 types->register_event_definition(12,&g_simulation_unit_exit_vehicle_event_definition);
 types->register_event_definition(13,&g_simulation_weapon_fire_event_definition);
 types->register_event_definition(14,&g_simulation_weapon_reload_event_definition);
 types->register_event_definition(15,&g_simulation_unit_grenade_initiate_event_definition);
 types->register_event_definition(16,&g_simulation_unit_melee_initiate_event_definition);
 types->register_event_definition(17,&g_simulation_weapon_pickup_event_definition);
 types->register_event_definition(18,&g_simulation_weapon_put_away_event_definition);
 types->register_event_definition(19,&g_simulation_weapon_drop_event_definition);
 types->register_event_definition(20,&g_simulation_vehicle_flip_event_definition);
 types->register_event_definition(21,&g_simulation_vehicle_trick_event_definition);
 types->register_event_definition(22,&g_simulation_device_touch_event_definition);
 types->register_event_definition(23,&g_simulation_unit_grenade_release_event_definition);
 types->register_event_definition(24,&g_simulation_unit_melee_damage_event_definition);
 types->register_event_definition(25,&g_simulation_unit_melee_clang_event_definition);
 types->register_event_definition(26,&g_simulation_unit_enter_vehicle_event_definition);
 types->register_event_definition(27,&g_simulation_engine_request_boot_player_event_definition);
 types->register_event_definition(29,&g_simulation_unit_equipment_use_event_definition);
 types->register_event_definition(30,&g_simulation_unit_equipment_pickup_event_definition);
 types->register_event_definition(28,&g_simulation_player_respawn_request_event_definition);
 types->register_event_definition(33,&g_simulation_player_editor_request_event_definition);
 types->register_event_definition(31,&g_simulation_supercombine_request_event_definition);
 types->register_event_definition(32,&g_simulation_object_refresh_event_definition);
 *entity_type_count=22;
 *event_type_count=34;
}


static __forceinline const s_unit_weapon_set *simulation_game_control_weapon(const unit_control_data *control)
{
    return (const s_unit_weapon_set *)&control->weapon_set;
}
static __forceinline s_unit_weapon_set *simulation_game_motion_weapon(s_player_motion *motion)
{
    return (s_unit_weapon_set *)&motion->weapon_set;
}

static __forceinline const s_unit_weapon_set *simulation_game_action_weapon(const player_action *action)
{
    return (const s_unit_weapon_set *)&action->weapons;
}

// @address 0x3d3930 simulation_game_build_motion_data_from_action
// match: 0x3d3930: pointer __restrict qualifier prevents alias reload
extern "C" void simulation_game_build_motion_data_from_action(const player_action *action, long unit_index, s_player_motion *__restrict motion)
{
    memset(motion, 0, sizeof(s_player_motion));
    motion->type = action->unit_valid;
    motion->identifier = action->unit_flags;
    motion->facing = action->facing;
    if (!action->byte)
        motion->throttle = action->move_vector;
    SET_BIT(motion->flags, 0, TEST_BIT(action->flags, 0));
    SET_BIT(motion->flags, 1, TEST_BIT(action->flags, 1));
    SET_BIT(motion->flags, 2, TEST_BIT(action->flags, 11));
    SET_BIT(motion->flags, 3, TEST_BIT(action->flags, 14));
    SET_BIT(motion->flags, 4, TEST_BIT(action->extra_short, 0));
    SET_BIT(motion->flags, 5, TEST_BIT(action->extra_short, 1));
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        motion->zoom = unit->unit.zoom_level;
        *simulation_game_motion_weapon(motion) = unit->unit.current_weapon_set;
        motion->exceeded = object_velocity_exceeds_threshold(unit_index);
    }
    else
    {
        simulation_game_motion_weapon(motion)->identifier = -1;
        simulation_game_motion_weapon(motion)->weapon_indices[0] = -1;
        simulation_game_motion_weapon(motion)->weapon_indices[1] = -1;
        motion->zoom = -1;
    }
    // match: 0x3d3930: pointer __restrict qualifier prevents alias reload
    const player_action *__restrict da = action;
    if ((unsigned short)simulation_game_action_weapon(da)->identifier == (unsigned short)simulation_game_motion_weapon(motion)->identifier)
        *simulation_game_motion_weapon(motion) = *simulation_game_action_weapon(action);
    if (action->zoom != -1)
        motion->zoom = action->zoom;
    if (simulation_game_motion_weapon(motion)->weapon_indices[0] != -1 &&
        simulation_game_motion_weapon(motion)->weapon_indices[0] == simulation_game_motion_weapon(motion)->weapon_indices[1])
        simulation_game_motion_weapon(motion)->weapon_indices[1] = -1;
    simulation_game_build_predicted_weapon(unit_index, (const s_aim_assist_targeting_result *)&action->aim_assist, motion);
}

// dev: simulation_game_build_motion_data_from_control(const unit_control_data *, long, s_player_motion *)
extern "C" void simulation_game_build_motion_data_from_control(const unit_control_data *control_data, long unit_index, s_player_motion *motion_data)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    memset(motion_data, 0, sizeof(s_player_motion));
    motion_data->type = 1;
    motion_data->identifier = unit->unit.unit_input_flags;
    euler_angles2d_from_vector3d((real_euler_angles2d *)&motion_data->facing, &control_data->aiming);
    if (motion_data->facing.yaw < 0.0f)
        motion_data->facing.yaw += 6.2831855f;
    motion_data->throttle = control_data->throttle.xy;
    SET_BIT(motion_data->flags, 0, TEST_BIT(control_data->control_flags, 0));
    SET_BIT(motion_data->flags, 1, TEST_BIT(control_data->control_flags, 1));
    SET_BIT(motion_data->flags, 2, TEST_BIT(control_data->control_flags, 11));
    SET_BIT(motion_data->flags, 3, TEST_BIT(control_data->control_flags, 14));
    *simulation_game_motion_weapon(motion_data) = unit->unit.current_weapon_set;
    if ((unsigned short)simulation_game_control_weapon(control_data)->identifier == (unsigned short)simulation_game_motion_weapon(motion_data)->identifier)
        *simulation_game_motion_weapon(motion_data) = *simulation_game_control_weapon(control_data);
    motion_data->zoom = control_data->zoom_level;
    simulation_game_build_predicted_weapon(unit_index, (const s_aim_assist_targeting_result *)&control_data->targeting, motion_data);
}

static __forceinline player_datum *u_networkinglibxbox_simulation_game_interface_player_get(long i) {
    return &((player_datum *)player_data->data)[(unsigned short)i];
}
static __forceinline unit_datum *u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(long i, unsigned long mask)
{
    (void)mask;
    return (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)i].datum;
}
bool simulation_game_build_player_prediction(long player_index, struct s_player_prediction * prediction)
{
    player_datum *player = u_networkinglibxbox_simulation_game_interface_player_get(player_index);
    long object_index = -1;
    long edit_object_index = -1;
    bool result = false;
    unsigned char flags;
    if (player->unit_index != -1) {
        unit_datum *unit = u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(player->unit_index, 0x1003);
        flags = unit->unit.unit_input_flags;
        if (unit->object.parent_object_index != -1) {
            unit_datum *parent = u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(unit->object.parent_object_index, ~0UL);
            if (((1 << parent->object.object_type) & 0x1003)
                && player->unit_index == parent->unit.driver_object_index) object_index = unit->object.parent_object_index;
        } else {
            if (!game_engine_is_player_in_edit_mode(player_index, &edit_object_index, 0) || edit_object_index == -1 ||
                (!player->look_override && (game_time_globals->time & 1)))
                object_index = player->unit_index;
            else object_index = edit_object_index;
        }
    }
    if (object_index != -1) {
        long entity = simulation_object_get_predictive_entity_internal(object_index, true);
        if (entity != -1) {
            memset(prediction, 0, sizeof(*prediction));
            prediction->entity = entity;
            prediction->identifier = -1;
            prediction->flags = flags;
            object_get_origin(object_index, &prediction->origin);
            ::object_get_orientation(object_index, &prediction->forward, &prediction->up);
            object_get_velocities(object_index, &prediction->velocity, &prediction->angular);
            result = true;
        }
    }
    return result;
}

static __forceinline mover_state *u_networkinglibxbox_simulation_game_interface_unit_control_get(unit_datum *unit)
{
    return (mover_state *)((char *)unit + unit->unit.action_offset);
}
static __forceinline void u_networkinglibxbox_simulation_game_interface_apply_predicted_object(long object_index, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
{
    e_simulation_game_position_update_result result = simulation_evaluate_position_update(object_index,
        _simulation_game_position_update_context_client_prediction,
        position, forward, up, linear_velocity, angular_velocity);
    simulation_apply_position_update(object_index, 2, result, position, forward, up, linear_velocity, angular_velocity);
}
// @address 0x3d4260 ?simulation_game_process_player_prediction@@YAXJPBUs_player_prediction@@@Z
void simulation_game_process_player_prediction(long player_index, const s_player_prediction *prediction)
{
    if (prediction->identifier != -1)
    {
        long object_index = simulation_gamestate_entity_get_object_index(prediction->identifier);
        if (object_index != -1)
        {
            unit_datum *object = u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(object_index, ~0UL);
            bool valid = false;
            if (game_engine_is_player_in_edit_mode(player_index, 0, 0))
                valid = true;
            else if ((1 << object->object.object_type) & 0x1003)
            {
                if (object->unit.player_index == player_index && !(bool)((object->object.damage_flags >> 2) & 1))
                    valid = true;
                else if (object->unit.driver_object_index != -1)
                {
                    unit_datum *driver = u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(object->unit.driver_object_index, 0x1003);
                    if (driver->unit.player_index == player_index)
                        valid = true;
                }
            }
            if (((1 << object->object.object_type) & 0x1003) && action_executing(object_index, _action_melee_player) &&
                !u_networkinglibxbox_simulation_game_interface_unit_control_get(object)->prediction_active)
                valid = false;
            long player_unit = u_networkinglibxbox_simulation_game_interface_player_get(player_index)->unit_index;
            if (player_unit != -1 &&
                u_networkinglibxbox_simulation_game_interface_object_get_and_verify_type(player_unit, 0x1003)->unit.unit_input_flags != prediction->flags)
            {
                long edited_object_index = -1;
                if (!game_engine_is_player_in_edit_mode(player_index, &edited_object_index, 0) || edited_object_index != object_index)
                    valid = false;
            }
            valid = valid && object->object.parent_object_index == -1;
            if (valid)
                u_networkinglibxbox_simulation_game_interface_apply_predicted_object(object_index,
                    &prediction->origin, &prediction->forward, &prediction->up, &prediction->velocity, &prediction->angular);
        }
    }
}

extern "C" {
// @address 0x80a38c g_simulation_sandbox_engine_globals_next_available_update_bit
long g_simulation_sandbox_engine_globals_next_available_update_bit;
}
