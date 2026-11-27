// OBJ: from the records 0x42d388 0x42d4a8 0x446628 0x446698 0x446800
// @compiler xdk6130
#include "cseries/crt_xdk.h"
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_database.h"
#include "memory/tls_globals.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "game/game_globals_storage.h"
#include "simulation/simulation.h"
#include "game/game_engine_sandbox.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_priority.h"
#include "simulation/game_interface/simulation_game_map_variant.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/game_interface/simulation_game_internal.h"
// @address 0x446628 ?simulation_priority_entity_uses_special_configuration@@YA_NPBUs_simulation_entity@@@Z
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z


#include "networking/network_configuration_types.h"




#include "networking/network_time_types.h"





#include "networking/network_configuration.h"

extern "C" {
}

enum
{
    _simulation_entity_type_player = 11,
    _simulation_entity_type_first_object = 14,
    _simulation_entity_type_equipment = 15,
    _simulation_entity_type_vehicle = 18,
    _simulation_entity_type_last_object = 21,
    k_simulation_entity_type_count = 19,
};

// 0x446698: the priority of creating the entity for a view (no dev counterpart: simulation_priority.obj is not in
// the March build); a projectile whose damage owner is one of the view's players gets the maximum, raised a little
enum
{
    _simulation_entity_type_projectile = 19,
};

static __forceinline const s_simulation_entity_update_configuration *network_configuration_get_entity_update_configuration(long entity_type)
{
    return &g_network_configuration.records[entity_type].update;
}

static __forceinline bool simulation_entity_type_is_object(long entity_type)
{
    bool result = false;
    long ordinal = entity_type;
    if (ordinal >= _simulation_entity_type_first_object && ordinal <= _simulation_entity_type_last_object)
        result = true;
    return result;
}



// 0x42d388: the "map-variant creation" debug line with the entity's creation relevance
// @address 0x42d388 ?write_creation_description_to_string@c_simulation_map_variant_group_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_map_variant_group_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "map-variant creation: relevance=%5.3f", relevance);
}

// 0x42d4a8: the "map-variant update" debug line with the entity's update relevance and period
// @address 0x42d4a8 ?write_update_description_to_string@c_simulation_map_variant_group_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_map_variant_group_entity_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    long period = 0;
    simulation_calculate_entity_update_priority(entity, update_data, &relevance, &period);
    csnzprintf(buffer, buffer_size, "map-variant update: relevance=%5.3f:period=%d", relevance, period);
}

bool simulation_priority_entity_uses_special_configuration(const s_simulation_entity *entity)
{
    bool result = false;
    if (entity->entity_type == _simulation_entity_type_equipment && entity->gamestate_index != -1)
    {
        long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
        if (object_index != -1 && object_header_get(object_index)->object_type == _object_type_equipment)
            result = true;
    }
    return result;
}

float simulation_calculate_entity_update_priority(const s_simulation_entity *entity,
    const s_entity_update_data *update_data, float *relevance_out, long *period_out)
{
    if (relevance_out)
        *relevance_out = -1.0f;
    if (period_out)
        *period_out = -1;

    long entity_type = entity->entity_type;
    const s_simulation_entity_update_configuration *configuration = network_configuration_get_entity_update_configuration(entity_type);
    if (simulation_priority_entity_uses_special_configuration(entity))
        configuration = &g_network_configuration.records[k_simulation_entity_type_count].update;

    if (configuration->base_priority > 0.0f)
        return configuration->base_priority;

    float priority = 0.0f;
    bool special = false;
    unsigned long update_flags = update_data->update_mask;
    float relevance = simulation_calculate_entity_relevance(1, &entity->entity_index, configuration->relevance_radius, update_data->telemetry, &special);
    if (relevance_out)
        *relevance_out = relevance;

    if (entity_type == _simulation_entity_type_first_object)
    {
        if (special)
            return configuration->special_priority;
        if (update_flags & 1)
            return configuration->flagged_priority;
    }

    if (simulation_entity_type_is_object(entity_type) &&
        ::game_engine_is_object_being_edited(simulation_gamestate_entity_get_object_index(entity->gamestate_index), (long *)(0)))
        return configuration->maximum_priority;

    if (entity_type == _simulation_entity_type_player && simulation_starting_up() && tls_game_globals()->game_finished != 0)
        return configuration->maximum_priority + 0.05f;

    long period = simulation_calculate_transmission_period(relevance, configuration->relevance.upper,
        configuration->relevance.lower, configuration->minimum_period, configuration->maximum_period);
    if (period_out)
        *period_out = period;

    unsigned long last_update_time = update_data->last_update_time;
    long time_since_update = (g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount()) - last_update_time;
    if (time_since_update < period)
    {
        priority = configuration->initial_minimum_priority +
            (configuration->initial_maximum_priority - configuration->initial_minimum_priority) * relevance;
    }
    else if (time_since_update < configuration->maximum_delay)
    {
        priority = configuration->late_minimum_priority +
            (configuration->late_maximum_priority - configuration->late_minimum_priority) * relevance;
    }
    else
    {
        priority = configuration->maximum_priority;
    }

    if (entity_type == _simulation_entity_type_vehicle)
    {
        if (entity->gamestate_index != -1)
        {
            long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
            if (object_index != -1)
            {
                unit_datum *vehicle = (unit_datum *)object_get_and_verify_type(object_index, 0x2003);
                if (vehicle->unit.driver_object_index != -1)
                    priority += configuration->occupied_vehicle_bonus;
            }
        }
    }

    return priority;
}

// @address 0x446698 simulation_calculate_entity_creation_priority
float simulation_calculate_entity_creation_priority(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, float *relevance_out)
{
    if (relevance_out)
        *relevance_out = -1.0f;

    long entity_type = entity->entity_type;
    const s_simulation_entity_creation_configuration *configuration = &g_network_configuration.records[entity_type].creation;
    if (simulation_priority_entity_uses_special_configuration(entity))
        configuration = &g_network_configuration.records[k_simulation_entity_type_count].creation;

    float priority = configuration->base_priority;
    if (priority <= 0.0f)
    {
        float relevance = 0.0f;
        float priority_range = configuration->maximum_priority - configuration->minimum_priority;
        relevance = simulation_calculate_entity_relevance(1, &entity->entity_index, configuration->relevance_radius, telemetry, 0);
        if (relevance_out)
            *relevance_out = relevance;
        priority = configuration->minimum_priority + priority_range * relevance;
    }

    if (entity_type == _simulation_entity_type_projectile && entity->gamestate_index != -1)
    {
        long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
        if (object_index != -1 && object_header_get(object_index)->object_type == _object_type_projectile)
        {
            long owner_player_index = ((object_datum *)object_get_and_verify_type(object_index, FLAG(_object_type_projectile)))->damage_owner.player_index;
            if (owner_player_index != -1)
            {
                long owner_absolute_index = owner_player_index & 0xffff;
                bool owner_in_view = false;
                for (long player_index = 0; player_index < telemetry->number_of_players; player_index++)
                {
                    if (telemetry->players[player_index].player_absolute_index == owner_absolute_index)
                    {
                        owner_in_view = true;
                        break;
                    }
                }
                if (owner_in_view)
                    priority = configuration->maximum_priority + 0.05f;
            }
        }
    }
    return priority;
}
