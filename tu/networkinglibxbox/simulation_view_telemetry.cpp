// networkinglibxbox/simulation_view_telemetry.obj: from the records 0x446a90 0x446d00 0x446e00
// @flags /TP /O2
#include "networking/simulation/simulation_view_telemetry.h"
#include "networking/simulation/simulation_view_telemetry_internals.h"
#include "math/real_math_types.h"
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "objects/object_datum.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_world.h"
#include "simulation/simulation_players_types.h"
#include "objects/objects.h"
#include "simulation/simulation.h"
#ifdef HALO3_64BIT
#include "simulation/simulation_world_types.h"
#endif
#include "simulation/game_interface/simulation_game_internal.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "networking/network_configuration_types.h"
#include "simulation/simulation_view.h"
#include "networking/network_configuration.h"
#include "units/unit_types.h"
#include "simulation/simulation_distributed_view.h"
#include "networking/replication/replication_entity_manager.h"

// @address 0x446e00 ?entity_is_active@c_simulation_view_telemetry_provider@@QBA_NJ@Z
// @address 0x446d00 ?calculate_control_relevance@c_simulation_view_telemetry_provider@@QAAXJPBXPAMPAJ@Z
// @address 0x446a90 ?get_telemetry_for_for_view@c_simulation_view_telemetry_provider@@QAAPBXXZ
// @address 0x80618 ?simulation_starting_up@@YA_NXZ
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
// @address 0x40ea10 ?simulation_calculate_position_relevance@@YAMPBTreal_point3d@@MPBUs_simulation_view_telemetry_data@@@Z
// @address 0x3d3598 ?simulation_calculate_transmission_period@@YAJMMMJJ@Z

// 0x446a90
const void *c_simulation_view_telemetry_provider::get_telemetry_for_for_view()
{
  const c_simulation_view *view = m_view;
  s_machine_identifier machine_identifier = view->m_machine_identifier;
  c_simulation_world *world = view->m_world;
  m_telemetry_data.provider = this;
  m_telemetry_data.number_of_players = 0;
  if ((1u << view->m_view_type) & 20u)
  {
    m_telemetry_data.view_inactive = !view->m_active;
    m_telemetry_data.acknowledged_player_mask = view->m_acknowledged_player_mask;
    if (simulation_starting_up())
    {
      for (long user_index = 0; user_index < 4; user_index++)
      {
        c_simulation_player *player = simulation_world_find_player(world, &machine_identifier, user_index);
        if (player)
        {
          unsigned short player_index = (unsigned short)player->m_player_index;
          s_data_array *players = tls_player_data();
          player_datum *player_record =
              &((player_datum *)players->data)[player_index];
          long object_index = player_record->unit_index;
          if (object_index == -1)
          {
            long unit_index = player_record->dead_unit_index;
            if (unit_index != -1 && object_try_and_get_and_verify_type(unit_index, 0x1003))
              object_index = unit_index;
            if (player_record->respawn_target_player_index != -1)
            {
              player_datum *target =
                  (player_datum *)datum_try_and_get(players, player_record->respawn_target_player_index);
              if (target)
              {
                if (target->unit_index != -1)
                  object_index = target->unit_index;
                else
                {
                  unit_index = target->dead_unit_index;
                  if (unit_index != -1 && object_try_and_get_and_verify_type(unit_index, 0x1003))
                    object_index = unit_index;
                }
              }
            }
          }
          if (object_index != -1)
          {
            unit_datum *object =
                (unit_datum *)((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)].datum;
            long entity_index = -1;
            if (object->object.gamestate_index != -1)
              entity_index = ((s_simulation_gamestate_entity_datum *)
                  tls_simulation_gamestate_entity_data()->data)[
                      datum_index_to_absolute_index(object->object.gamestate_index)].entity_index;
            m_telemetry_data.players[m_telemetry_data.number_of_players].entity_index = entity_index;
            object_get_origin(object_index, &m_telemetry_data.players[m_telemetry_data.number_of_players].position);
            m_telemetry_data.players[m_telemetry_data.number_of_players].forward = object->unit.desired_aiming_vector;
            m_telemetry_data.players[m_telemetry_data.number_of_players].zoom_level = (unsigned char)object->unit.desired_zoom_level;
            m_telemetry_data.players[m_telemetry_data.number_of_players].player_absolute_index = player_index;
            m_telemetry_data.number_of_players++;
          }
        }
      }
    }
  }
  else
  {
    m_telemetry_data.view_inactive = false;
    m_telemetry_data.acknowledged_player_mask = -1;
  }
  return &m_telemetry_data;
}

void c_simulation_view_telemetry_provider::calculate_control_relevance(long control_index, const void *telemetry, float *relevance, long *transmission_period)
{
  const c_simulation_view *owner = m_view;
  char *data = (char *)owner->m_world;
  *relevance = 0.0f;
  *transmission_period = 0;
  long object_index = -1;
  if (control_index < 16)
  {
    s_data_array *players = tls_player_data();
    if (control_index != -1 && control_index >= 0 && control_index < players->first_unallocated)
    {
      player_datum *datum = (player_datum *)(players->data + players->size * control_index);
      if (datum->identifier != 0)
        object_index = datum->unit_index;
    }
  }
  else if (control_index < 32)
  {
#ifdef HALO3_64BIT
    const c_simulation_actor *actor = &owner->m_world->m_actors[control_index - 16];
    if (actor->m_simulation_actor_index != -1)
      object_index = actor->m_unit_index;
#else
    // match: 0x446d00: separate stride cursor preserves the -16 addi before the actor field loads.
    const s_simulation_world_actor_stride *actor =
        (const s_simulation_world_actor_stride *)data + (control_index - 16);
    const s_simulation_world_actor_control_view *actor_view =
        (const s_simulation_world_actor_control_view *)actor;
    if (actor_view->actor_index != -1)
      object_index = actor_view->unit_index;
#endif
  }
  if (object_index != -1)
  {
    real_point3d origin;
    object_get_origin(object_index, &origin);
    const s_network_configuration *config = &g_network_configuration;
    float position_relevance = simulation_calculate_position_relevance(
        &origin, config->control_growth_base, (const s_simulation_view_telemetry_data *)telemetry);
    long recover_time = config->control_recover_time;
    float maximum_growth_shift = config->control_maximum_growth_shift;
    long probe_time = config->control_probe_time;
    float growth_shift = config->control_growth_shift;
    *relevance = position_relevance;
    *transmission_period = simulation_calculate_transmission_period(position_relevance, growth_shift, maximum_growth_shift, probe_time, recover_time);
  }
}

bool c_simulation_view_telemetry_provider::entity_is_active(long handle) const
{
  long index = handle & 0x3ff;
  bool result = false;
  const c_simulation_view *owner = m_view;
  c_replication_entity_manager_view *table = &((s_simulation_distributed_view *)owner->m_distributed_view)->entities;
  if (index >= 0 && (unsigned long)index < 1024) {
    s_replication_entity_view_entry *entry = &table->entries[index];
    if (entry->id == handle) {
      unsigned char flags = table->definitions->m_entity_data[index].flags;
      if (flags & 4)
        result = (unsigned short)entry->state.m_storage == 3 && !(flags & 2);
      else
        result = true;
    }
  }
  return result;
}
