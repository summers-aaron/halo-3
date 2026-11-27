// networkinglibxbox/network_configuration.obj: from the records 0x8f698 0x2114b0 0x3a5518 0x3a6238 0x3a6498 0x3a6578 0x3a68f8 0x3a6ac8 0x3a7200 0x4f32b8 0x4f3360 0x4f33a0
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
// @address 0x94a58 ?append_print@?$c_static_string@$0BAA@@@QAAPADPBDZZ
// @address 0x2114b0 network_configuration_get_player_class
// @address 0x3a6ac8 ?network_configuration_initialize_observer_constants@@YAXXZ
// @address 0x3a7200 network_configuration_try_and_get_data
// @address 0x4f32b8 network_configuration_parse_lobby_film_max_players
// @address 0x4f3360 network_configuration_parse_lobby_forge_max_players
// @address 0x4f33a0 network_configuration_parse_lobby_coop_max_players
// @address 0x3a6238 ?network_configuration_initialize_simulation_event_constants@@YAXXZ
// @address 0x3a6498 ?network_configuration_initialize_simulation_entity_creation_constants@@YAXXZ
// @address 0x3a68f8 ?network_configuration_initialize_simulation_constants@@YAXXZ
#include "cseries/crt_xdk.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "networking/network_configuration.h"
#include "main/player_profile_summary_types.h"


#include "networking/network_configuration_types.h"
#include "networking/tools/network_blf.h"



#include "networking/network_configuration_globals.h"
// @address 0x79e228 g_network_configuration_state
// Other compilers only: the Xbox link takes this object's constant-initialized .data words and its initializer from the image.
#if !defined(_XBOX) || defined(__clang__)
s_network_configuration_state g_network_configuration_state;
#endif
#include "networking/network_configuration_dependencies.h"
// @address 0x11db094 g_network_configuration_update_version
long g_network_configuration_update_version;




#include "networking/tools/blf_authentication_types.h"

// @address 0x3ada60 ?network_blf_find_chunk_in_file@@YAPAXPBDKJFW4e_blf_file_authentication_type@@PAK@Z

#include "networking/logic/storage/network_http_buffer_downloader.h"


#include "networking/simulation/simulation_event_configuration.h"
#include "simulation/simulation_queue_event_types.h"
#include <string.h>
#include "networking/online/online_url.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "networking/logic/storage/network_http_buffer_url.h"

// @address 0x58674 k_projectile_attached_minimum_relevance
const float k_projectile_attached_minimum_relevance = 0.27f;

// @address 0x5867c k_player_respawn_request_relevance
const float k_player_respawn_request_relevance = 0.61f;

// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
#if !defined(_XBOX) || defined(__clang__)
// Other compilers only: the Xbox link takes this object from the image (absolute 0x8087a8).
s_network_configuration g_network_configuration;
#endif



// 0x3a6238
void network_configuration_initialize_simulation_event_constants(void)
{
    s_simulation_event_relevance_entry *events = g_network_configuration.event_relevance;
    events[_simulation_event_type_game_engine_request_boot_player].relevance = 0.9f;
    events[_simulation_event_type_game_engine_request_boot_player].delay = 10000;
    events[_simulation_event_type_weapon_pickup].relevance = 0.75f;
    events[_simulation_event_type_weapon_pickup].delay = 5000;
    events[_simulation_event_type_unit_equipment_use].relevance = 0.75f;
    events[_simulation_event_type_unit_equipment_use].delay = 5000;
    events[_simulation_event_type_unit_equipment_pickup].relevance = 0.75f;
    events[_simulation_event_type_unit_equipment_pickup].delay = 5000;
    events[_simulation_event_type_unit_enter_vehicle].relevance = 0.75f;
    events[_simulation_event_type_unit_enter_vehicle].delay = 5000;
    events[_simulation_event_type_device_touch].relevance = 0.75f;
    events[_simulation_event_type_device_touch].delay = 5000;
    events[_simulation_event_type_player_editor_request].relevance = 0.7f;
    events[_simulation_event_type_player_editor_request].delay = -1;
    events[_simulation_event_type_weapon_put_away].relevance = 0.7f;
    events[_simulation_event_type_weapon_put_away].delay = -1;
    events[_simulation_event_type_weapon_drop].relevance = 0.7f;
    events[_simulation_event_type_weapon_drop].delay = -1;
    events[_simulation_event_type_unit_grenade_release].relevance = 0.68f;
    events[_simulation_event_type_unit_grenade_release].delay = 5000;
    events[_simulation_event_type_projectile_supercombine_request].relevance = 0.65f;
    events[_simulation_event_type_projectile_supercombine_request].delay = 5000;
    events[_simulation_event_type_player_respawn_request].relevance = k_player_respawn_request_relevance;
    events[_simulation_event_type_player_respawn_request].delay = 10000;
    events[_simulation_event_type_unit_melee_damage].relevance = 0.6f;
    events[_simulation_event_type_unit_melee_damage].delay = 5000;
    events[_simulation_event_type_unit_melee_clang].relevance = 0.6f;
    events[_simulation_event_type_unit_melee_clang].delay = 5000;
    events[_simulation_event_type_vehicle_flip].relevance = 0.6f;
    events[_simulation_event_type_vehicle_flip].delay = 5000;
    events[_simulation_event_type_vehicle_trick].relevance = 0.35f;
    events[_simulation_event_type_vehicle_trick].delay = 5000;
    events[32].relevance = 0.3f;
    events[32].delay = 5000;
    events[_simulation_event_type_weapon_fire].input = 100.0f;
    events[_simulation_event_type_weapon_fire].delay = 1500;
    events[_simulation_event_type_weapon_fire].minimum = 0.51f;
    events[_simulation_event_type_unit_melee_initiate].delay = 5000;
    events[_simulation_event_type_weapon_fire].maximum = 0.75f;
    events[_simulation_event_type_unit_exit_vehicle].delay = 5000;
    events[_simulation_event_type_unit_melee_initiate].input = 100.0f;
    events[_simulation_event_type_unit_grenade_initiate].delay = 5000;
    events[_simulation_event_type_unit_melee_initiate].minimum = 0.5f;
    events[_simulation_event_type_weapon_reload].delay = 5000;
    events[_simulation_event_type_unit_melee_initiate].maximum = 0.75f;
    events[_simulation_event_type_projectile_attached].delay = 5000;
    events[_simulation_event_type_unit_exit_vehicle].input = 100.0f;
    events[_simulation_event_type_damage_aftermath].delay = 3000;
    events[_simulation_event_type_unit_exit_vehicle].minimum = 0.5f;
    events[_simulation_event_type_weapon_effect].delay = 1000;
    events[_simulation_event_type_unit_exit_vehicle].maximum = 0.75f;
    events[_simulation_event_type_unit_board_vehicle].delay = 5000;
    events[_simulation_event_type_unit_grenade_initiate].input = 100.0f;
    events[_simulation_event_type_game_engine].delay = 3000;
    events[_simulation_event_type_unit_grenade_initiate].minimum = 0.5f;
    events[_simulation_event_type_weapon_empty_click].delay = 5000;
    events[_simulation_event_type_unit_grenade_initiate].maximum = 0.75f;
    events[_simulation_event_type_weapon_reload].input = 100.0f;
    events[_simulation_event_type_weapon_reload].minimum = 0.3f;
    events[_simulation_event_type_weapon_reload].maximum = 0.7f;
    events[_simulation_event_type_projectile_attached].input = 100.0f;
    events[_simulation_event_type_projectile_attached].minimum = k_projectile_attached_minimum_relevance;
    events[_simulation_event_type_projectile_attached].maximum = 0.73f;
    events[_simulation_event_type_damage_aftermath].input = 100.0f;
    events[_simulation_event_type_damage_aftermath].minimum = 0.25f;
    events[_simulation_event_type_damage_aftermath].maximum = 0.73f;
    events[_simulation_event_type_weapon_effect].input = 100.0f;
    events[_simulation_event_type_weapon_effect].minimum = 0.2f;
    events[_simulation_event_type_weapon_effect].maximum = 0.7f;
    events[_simulation_event_type_unit_board_vehicle].input = 100.0f;
    events[_simulation_event_type_unit_board_vehicle].minimum = 0.2f;
    events[_simulation_event_type_unit_board_vehicle].maximum = 0.73f;
    events[_simulation_event_type_game_engine].input = 100.0f;
    events[_simulation_event_type_game_engine].minimum = 0.16f;
    events[_simulation_event_type_game_engine].maximum = 0.73f;
    events[_simulation_event_type_weapon_empty_click].input = 100.0f;
    events[_simulation_event_type_weapon_empty_click].minimum = 0.15f;
    events[_simulation_event_type_weapon_empty_click].maximum = 0.73f;
    events[_simulation_event_type_unit_pickup].input = 100.0f;
    events[_simulation_event_type_unit_pickup].minimum = 0.15f;
    events[_simulation_event_type_unit_pickup].maximum = 0.73f;
    events[_simulation_event_type_unit_pickup].delay = 5000;
    events[_simulation_event_type_damage_section_response].input = 100.0f;
    events[_simulation_event_type_damage_section_response].minimum = 0.1f;
    events[_simulation_event_type_damage_section_response].delay = 3000;
    events[_simulation_event_type_damage_section_response].maximum = 0.73f;
    events[_simulation_event_type_breakable_surface_damage].delay = 5000;
    events[_simulation_event_type_breakable_surface_damage].input = 100.0f;
    events[_simulation_event_type_projectile_effect].delay = 5000;
    events[_simulation_event_type_breakable_surface_damage].minimum = 0.1f;
    events[_simulation_event_type_projectile_impact_effect].delay = 5000;
    events[_simulation_event_type_breakable_surface_damage].maximum = 0.73f;
    events[_simulation_event_type_projectile_object_impact_effect].delay = 5000;
    events[_simulation_event_type_projectile_effect].input = 100.0f;
    events[_simulation_event_type_projectile_effect].minimum = 0.1f;
    events[_simulation_event_type_projectile_effect].maximum = 0.73f;
    events[_simulation_event_type_projectile_impact_effect].input = 100.0f;
    events[_simulation_event_type_projectile_impact_effect].minimum = 0.1f;
    events[_simulation_event_type_projectile_impact_effect].maximum = 0.73f;
    events[_simulation_event_type_projectile_object_impact_effect].input = 100.0f;
    events[_simulation_event_type_projectile_object_impact_effect].minimum = 0.1f;
    events[_simulation_event_type_projectile_object_impact_effect].maximum = 0.73f;
}

// 0x3a6498
void network_configuration_initialize_simulation_entity_creation_constants(void)
{
    float base_priority = 0.7f;
    for (long entity_index = 0; entity_index <= 9; ++entity_index)
    {
        g_network_configuration.records[entity_index].creation.base_priority = base_priority;
    }
    g_network_configuration.records[11].creation.base_priority = base_priority;
    g_network_configuration.records[10].creation.minimum_priority = 0.5f;
    g_network_configuration.records[10].creation.maximum_priority = 0.8f;
    g_network_configuration.records[10].creation.relevance_radius = 100.0f;
    g_network_configuration.records[14].creation.minimum_priority = 0.5f;
    g_network_configuration.records[14].creation.maximum_priority = 0.8f;
    g_network_configuration.records[14].creation.relevance_radius = 100.0f;
    g_network_configuration.records[19].creation.minimum_priority = 0.4f;
    g_network_configuration.records[19].creation.maximum_priority = 0.8f;
    g_network_configuration.records[19].creation.relevance_radius = 100.0f;
    g_network_configuration.records[18].creation.minimum_priority = 0.35f;
    g_network_configuration.records[18].creation.maximum_priority = 0.8f;
    g_network_configuration.records[18].creation.relevance_radius = 100.0f;
    g_network_configuration.records[20].creation.minimum_priority = 0.1f;
    g_network_configuration.records[20].creation.maximum_priority = 0.8f;
    g_network_configuration.records[20].creation.relevance_radius = 100.0f;
    g_network_configuration.records[15].creation.minimum_priority = 0.1f;
    g_network_configuration.records[15].creation.maximum_priority = base_priority;
    g_network_configuration.records[15].creation.relevance_radius = 100.0f;
    g_network_configuration.records[16].creation.minimum_priority = 0.1f;
    g_network_configuration.records[16].creation.maximum_priority = base_priority;
    g_network_configuration.records[16].creation.relevance_radius = 100.0f;
    g_network_configuration.records[21].creation.minimum_priority = 0.1f;
    g_network_configuration.records[21].creation.maximum_priority = 0.65f;
    g_network_configuration.records[21].creation.relevance_radius = 100.0f;
    g_network_configuration.records[12].creation.minimum_priority = 0.1f;
    g_network_configuration.records[12].creation.maximum_priority = 0.65f;
    g_network_configuration.records[12].creation.relevance_radius = 100.0f;
    g_network_configuration.records[13].creation.minimum_priority = 0.1f;
    g_network_configuration.records[13].creation.maximum_priority = 0.65f;
    g_network_configuration.records[13].creation.relevance_radius = 100.0f;
    g_network_configuration.records[17].creation.minimum_priority = 0.1f;
    g_network_configuration.records[17].creation.maximum_priority = 0.6f;
    g_network_configuration.records[17].creation.relevance_radius = 100.0f;
    return;
}

static __forceinline void simulation_entity_update_initialize_defaults(s_network_configuration *updates)
{
    for (long i = 0; i <= 9; i++) {
        updates->records[i].update.relevance.upper = 1.0f;
        updates->records[i].update.relevance.lower = 0.6f;
        updates->records[i].update.minimum_period = 0;
        updates->records[i].update.maximum_period = 1000;
        updates->records[i].update.initial_maximum_priority = 0.2f;
        updates->records[i].update.maximum_delay = 1000;
        updates->records[i].update.initial_minimum_priority = 0.05f;
        updates->records[i].update.late_maximum_priority = 0.67f;
        updates->records[i].update.late_minimum_priority = 0.2f;
        updates->records[i].update.maximum_priority = 0.8f;
        updates->records[i].update.relevance_radius = 100.0f;
    }
}
// @address 0x3a6578 ?network_configuration_initialize_simulation_entity_update_constants@@YAXXZ
void network_configuration_initialize_simulation_entity_update_constants()
{
    s_network_configuration *updates = &g_network_configuration;
    simulation_entity_update_initialize_defaults(updates);
    updates->records[11].update.relevance.upper = 1.0f;
    updates->records[11].update.relevance.lower = 0.6f;
    updates->records[11].update.minimum_period = 0;
    updates->records[11].update.maximum_period = 1000;
    updates->records[11].update.initial_maximum_priority = 0.2f;
    updates->records[11].update.initial_minimum_priority = 0.1f;
    updates->records[11].update.maximum_delay = 1000;
    updates->records[11].update.late_maximum_priority = 0.67f;
    updates->records[10].update.minimum_period = 0;
    updates->records[11].update.late_minimum_priority = 0.35f;
    updates->records[10].update.maximum_period = 1000;
    updates->records[11].update.maximum_priority = 0.8f;
    updates->records[10].update.maximum_delay = 1000;
    updates->records[11].update.relevance_radius = 100.0f;
    updates->records[14].update.minimum_period = 0;
    updates->records[10].update.relevance.upper = 1.0f;
    updates->records[14].update.maximum_period = 800;
    updates->records[10].update.relevance.lower = 0.6f;
    updates->records[14].update.maximum_delay = 1000;
    updates->records[10].update.initial_maximum_priority = 0.2f;
    updates->records[18].update.minimum_period = 0;
    updates->records[10].update.initial_minimum_priority = 0.1f;
    updates->records[18].update.maximum_period = 800;
    updates->records[10].update.late_maximum_priority = 0.67f;
    updates->records[18].update.maximum_delay = 1000;
    updates->records[10].update.late_minimum_priority = 0.35f;
    updates->records[19].update.minimum_period = 0;
    updates->records[10].update.maximum_priority = 0.8f;
    updates->records[19].update.maximum_period = 500;
    updates->records[10].update.relevance_radius = 100.0f;
    updates->records[19].update.maximum_delay = 1000;
    updates->records[14].update.relevance.upper = 1.0f;
    updates->records[21].update.minimum_period = 200;
    updates->records[14].update.relevance.lower = 0.6f;
    updates->records[21].update.maximum_period = 1000;
    updates->records[14].update.initial_maximum_priority = 0.55f;
    updates->records[21].update.maximum_delay = 1000;
    updates->records[14].update.initial_minimum_priority = 0.32f;
    updates->records[20].update.minimum_period = 200;
    updates->records[14].update.late_maximum_priority = 0.7f;
    updates->records[20].update.maximum_period = 1000;
    updates->records[14].update.late_minimum_priority = 0.55f;
    updates->records[20].update.maximum_delay = 1000;
    updates->records[14].update.maximum_priority = 0.83f;
    updates->records[14].update.relevance_radius = 100.0f;
    updates->records[14].update.special_priority = 0.9f;
    updates->records[14].update.flagged_priority = 0.9f;
    updates->records[18].update.relevance.upper = 1.0f;
    updates->records[18].update.relevance.lower = 0.6f;
    updates->records[18].update.initial_maximum_priority = 0.55f;
    updates->records[18].update.initial_minimum_priority = 0.25f;
    updates->records[18].update.late_maximum_priority = 0.7f;
    updates->records[18].update.late_minimum_priority = 0.55f;
    updates->records[18].update.maximum_priority = 0.83f;
    updates->records[18].update.relevance_radius = 100.0f;
    updates->records[18].update.occupied_vehicle_bonus = 0.1f;
    updates->records[19].update.relevance.upper = 1.0f;
    updates->records[19].update.relevance.lower = 0.6f;
    updates->records[19].update.initial_maximum_priority = 0.6f;
    updates->records[19].update.initial_minimum_priority = 0.3f;
    updates->records[19].update.late_maximum_priority = 0.73f;
    updates->records[19].update.late_minimum_priority = 0.6f;
    updates->records[19].update.maximum_priority = 0.82f;
    updates->records[19].update.relevance_radius = 100.0f;
    updates->records[21].update.relevance.upper = 1.0f;
    updates->records[21].update.relevance.lower = 0.8f;
    updates->records[21].update.initial_maximum_priority = 0.23f;
    updates->records[21].update.initial_minimum_priority = 0.13f;
    updates->records[21].update.late_maximum_priority = 0.67f;
    updates->records[21].update.late_minimum_priority = 0.23f;
    updates->records[21].update.maximum_priority = 0.8f;
    updates->records[21].update.relevance_radius = 100.0f;
    updates->records[20].update.relevance.upper = 1.0f;
    updates->records[20].update.relevance.lower = 0.8f;
    updates->records[20].update.initial_maximum_priority = 0.23f;
    updates->records[20].update.initial_minimum_priority = 0.1f;
    updates->records[20].update.late_maximum_priority = 0.72f;
    updates->records[20].update.late_minimum_priority = 0.2f;
    updates->records[20].update.maximum_priority = 0.82f;
    updates->records[20].update.relevance_radius = 100.0f;
    updates->records[15].update.minimum_period = 200;
    updates->records[15].update.relevance.upper = 1.0f;
    updates->records[15].update.maximum_period = 1000;
    updates->records[15].update.relevance.lower = 0.8f;
    updates->records[15].update.maximum_delay = 1000;
    updates->records[15].update.initial_maximum_priority = 0.23f;
    updates->records[16].update.minimum_period = 200;
    updates->records[15].update.initial_minimum_priority = 0.1f;
    updates->records[16].update.maximum_period = 1000;
    updates->records[15].update.late_maximum_priority = 0.67f;
    updates->records[16].update.maximum_delay = 1000;
    updates->records[15].update.late_minimum_priority = 0.3f;
    updates->records[12].update.minimum_period = 0;
    updates->records[15].update.maximum_priority = 0.82f;
    updates->records[12].update.maximum_period = 1000;
    updates->records[15].update.relevance_radius = 100.0f;
    updates->records[12].update.maximum_delay = 1000;
    updates->records[16].update.relevance.upper = 1.0f;
    updates->records[13].update.minimum_period = 200;
    updates->records[16].update.relevance.lower = 0.8f;
    updates->records[13].update.maximum_period = 1000;
    updates->records[16].update.initial_maximum_priority = 0.23f;
    updates->records[13].update.maximum_delay = 1000;
    updates->records[16].update.initial_minimum_priority = 0.1f;
    updates->records[17].update.minimum_period = 200;
    updates->records[16].update.late_maximum_priority = 0.67f;
    updates->records[17].update.maximum_period = 1000;
    updates->records[16].update.late_minimum_priority = 0.3f;
    updates->records[17].update.maximum_delay = 1000;
    updates->records[16].update.maximum_priority = 0.82f;
    updates->records[16].update.relevance_radius = 100.0f;
    updates->records[12].update.relevance.upper = 1.0f;
    updates->records[12].update.relevance.lower = 0.6f;
    updates->records[12].update.initial_maximum_priority = 0.2f;
    updates->records[12].update.initial_minimum_priority = 0.05f;
    updates->records[12].update.late_maximum_priority = 0.63f;
    updates->records[12].update.late_minimum_priority = 0.2f;
    updates->records[12].update.maximum_priority = 0.8f;
    updates->records[12].update.relevance_radius = 100.0f;
    updates->records[13].update.relevance.upper = 1.0f;
    updates->records[13].update.relevance.lower = 0.8f;
    updates->records[13].update.initial_maximum_priority = 0.24f;
    updates->records[13].update.initial_minimum_priority = 0.1f;
    updates->records[13].update.late_maximum_priority = 0.67f;
    updates->records[13].update.late_minimum_priority = 0.3f;
    updates->records[13].update.maximum_priority = 0.83f;
    updates->records[13].update.relevance_radius = 100.0f;
    updates->records[17].update.relevance.upper = 1.0f;
    updates->records[17].update.relevance.lower = 0.6f;
    updates->records[17].update.initial_maximum_priority = 0.19f;
    updates->records[17].update.initial_minimum_priority = 0.04f;
    updates->records[17].update.late_maximum_priority = 0.6f;
    updates->records[17].update.late_minimum_priority = 0.19f;
    updates->records[17].update.maximum_priority = 0.79f;
    updates->records[17].update.relevance_radius = 100.0f;
}

// @address 0x3a68f8 ?network_configuration_initialize_simulation_constants@@YAXXZ
void network_configuration_initialize_simulation_constants(void)
{
    s_network_configuration *block = &g_network_configuration;
    block->entity_relevance.zoom_cosine[0] = 0.994f;
    block->control_probe_time = 0;
    block->entity_relevance.zoom_relevance[0] = 0.95f;
    block->control_recover_time = 1000;
    block->entity_relevance.zoom_cosine[1] = 0.999f;
    block->maximum_value = 6;
    block->entity_relevance.zoom_relevance[1] = 1.0f;
    block->ranges[0].first = 60;
    block->control_growth_base = 100.0f;
    block->ranges[0].second = 800;
    block->control_growth_shift = 1.0f;
    block->ranges[1].first = 20;
    block->control_maximum_growth_shift = 0.6f;
    block->ranges[1].second = 2;
    block->position_relevance.minimum_distance = 1.0f;
    block->ranges[2].first = 30;
    block->position_relevance.maximum_cosine = 0.99f;
    block->ranges[2].second = 6;
    block->position_relevance.minimum_cosine = 0.5f;
    block->ranges[3].first = 0;
    block->position_relevance.minimum_view_distance = 5.0f;
    block->ranges[3].second = 20;
    block->game_results_update_interval = 10000;
    block->simulation_actor_control_timeout = 2.0f;
    block->synchronous_client_block_duration_msec = 5000;
    block->projectile_supercombine_minimum_fraction = 0.5f;
    block->maximum_catchup_views = 8;
    block->simulation_join_active_fraction = 0.25f;
    block->simulation_join_timeout = 45000;
    block->simulation_join_completion_threshold = 0.5f;
    block->simulation_join_partial_timeout = 30000;
    block->simulation_queue_allocated_bytes_limit = 0.9f;
    block->simulation_join_complete_timeout = 0x20f58;
    block->simulation_queue_allocated_elements_limit = 0.9f;
    block->simulation_join_total_timeout = 0x249f0;
    block->simulation_maximum_catchup_attempts = 3;
    block->simulation_catchup_retry_delay = 2000;
    block->simulation_maximum_join_attempts = 5;
    block->simulation_join_failure_timeout = 0x2bf20;
    network_configuration_initialize_simulation_event_constants();
    network_configuration_initialize_simulation_entity_creation_constants();
    network_configuration_initialize_simulation_entity_update_constants();
    s_network_configuration *tail = &g_network_configuration;
    tail->position_update_distance = 3.0f;
    tail->position_update_distance_remote = 2.5f;
    tail->vehicle_position_update_distance = 9.0f;
    tail->vehicle_position_update_distance_remote = 7.5f;
    tail->supercombine_minimum_time = 5.0f;
    tail->supercombine_minimum_distance = 0.25f;
    tail->trigger_mispredict_time = 0.1f;
    tail->weapon_ticks_seconds = 1.0f;
    tail->weapon_ticks_threshold = 0.66f;
    tail->weapon_ticks_minimum = 0.1f;
}


// 0x3a6ac8
void network_configuration_initialize_observer_constants(void)
{
    s_network_configuration *block = &g_network_configuration;
    block->observer.initiation_retry[0].retry_count = 1;
    block->observer.safe_sequence_threshold = 0.5f;
    block->observer.initiation_retry[0].retry_timeouts[0] = 0;
    block->observer.safe_packet_rate_minimum = 1.0f;
    block->observer.initiation_retry[1].retry_count = 3;
    block->observer.safe_packet_rate_maximum = 30.0f;
    block->observer.initiation_retry[1].retry_timeouts[0] = 0;
    block->observer.stream_packet_rate_thresholds[0] = 1.0f;
    block->observer.initiation_retry[1].retry_timeouts[1] = 500;
    block->observer.stream_packet_rate_thresholds[1] = 0.6666667f;
    block->observer.initiation_retry[1].retry_timeouts[2] = 500;
    block->observer.stream_packet_rate_thresholds[2] = 0.5f;
    block->observer.initiation_retry[2].retry_count = 3;
    block->observer.stream_packet_rate_thresholds[3] = 1.0f / 3.0f;
    block->observer.initiation_retry[2].retry_timeouts[0] = 500;
    block->observer.stream_packet_rate_thresholds[4] = 0.25f;
    block->observer.initiation_retry[2].retry_timeouts[1] = 500;
    block->observer.stream_packet_rate_thresholds[5] = 0.2f;
    block->observer.initiation_retry[2].retry_timeouts[2] = 1000;
    block->observer.stream_packet_rate_thresholds[6] = 0.1f;
    block->observer.initiation_retry[3].retry_count = 4;
    block->observer.voice_fraction_authority = 0.25f;
    block->observer.initiation_retry[3].retry_timeouts[0] = 500;
    block->observer.voice_fraction_client = 0.5f;
    block->observer.initiation_retry[3].retry_timeouts[1] = 500;
    block->observer.voice_fraction_unattached = 0.75f;
    block->observer.initiation_retry[3].retry_timeouts[2] = 1000;
    block->observer.simulation_packet_rate = 30.0f; // PC broadband: match 30 Hz sim tick
    block->observer.initiation_retry[3].retry_timeouts[3] = 2000;
    block->observer.flood_packet_fraction = 0.5f;
    block->observer.dead_observer_retry_delay = 15000;
    block->observer.stream_packet_rate_limiting_fraction = 1.0f / 3.0f;
    block->observer.heartbeat_interval_msec = 1000;
    block->observer.bandwidth_flood_channel_fraction = 0.5f;
    block->observer.channel_activity_timeout = 2000;
    block->observer.channel_activity_first_timeout = 500;
    block->observer.channel_activity_second_timeout = 2000;
    block->observer.undesired_connection_drained_timeout = 1000;
    block->observer.undesired_connection_timeout = 5000;
    block->observer.stream_inactivity_timeout = 4000;
    block->observer.stream_receive_timeout = 4000;
    block->observer.inactivity_timeout = 4000;
    block->observer.receive_timeout = 10000;
    block->observer.stream_packet_rate_count = 7;
    block->observer.packet_window_minimum_bytes = 200;
    block->observer.voice_receive_fast_acknowledge_time = 2000;
    block->observer.time_statistics_interval = 2000;
    block->observer.window_statistics_count = 0x1e;
    block->observer.flood_max_aperture_msec = 4000;
    block->observer.stream_packet_overhead_low = 0x40;
    block->observer.stream_packet_overhead_high = 0x60;
    block->observer.stream_minimum_estimated_bandwidth_bps = 0x2800;
    block->observer.bandwidth_adjust_interval = 1000;
    block->observer.bandwidth_flood_check_threshold = 3;
    block->observer.bandwidth_flood_backoff_kbps = 0x28;
    block->observer.bandwidth_flood_backoff_repeat_interval = 5000;
    block->observer.bandwidth_clear_check_threshold = 0xf;
    block->observer.bandwidth_check_threshold_maximum = 100;
    block->observer.bandwidth_safety_margin_kbps = 0x32;
    block->observer.bandwidth_minimum_bps = 0x32;
    block->observer.bandwidth_increment_known_bad_threshold_kbps = 0xf;
    block->observer.bandwidth_increment_kbps = 8;
    block->observer.stream_debug_bandwidth = 1;
    block->observer.observer_stream_expansion_interval_msec = 2000;
    block->observer.stream_minimum_bandwidth_bps = 0x400;
    block->observer.stream_maximum_bandwidth_bps = 0x400000; // PC broadband: 4 Mbps cap
    block->observer.stream_initial_startup_msec = 1000;
    block->observer.stream_minimum_game_bps = 0x8430;
    block->observer.stream_initial_maximum_bps = 0x200000; // PC broadband: 2 Mbps initial
    block->observer.stream_initial_low_maximum_bps = 0x200000; // PC broadband: 2 Mbps initial low
    block->observer.stream_minimum_host_bps = 0x2000;
    block->observer.stream_maximum_probe_bps = 0x40000; // PC broadband: 256 kbps probe
    block->observer.stream_desired_bps = 0x20000; // PC broadband: 128 kbps/peer
    block->observer.stream_desired_high_priority_bps = 0x30000; // PC broadband: 192 kbps/peer
    block->observer.stream_desired_low_priority_bps = 0x40000; // PC broadband: 256 kbps/peer
    block->observer.stream_voice_high_minimum_bps = 0x7800;
    block->observer.stream_voice_low_minimum_bps = 0xa000;
    block->observer.stream_voice_active_minimum_bps = 0xa000;
    block->observer.stream_voice_high_desired_bps = 0x7800;
    block->observer.stream_voice_low_desired_bps = 0xb400;
    block->observer.stream_voice_active_desired_bps = 0x80000;
    block->observer.stream_voice_shared_desired_bps = 0x3000;
    block->observer.stream_loss_throttle_fraction = 0.1f;
    block->observer.stream_voice_host_minimum_bps = 0x7800;
    block->observer.stream_throttle_reduce_multiplier = 0.8f;
    block->observer.stream_voice_low_host_minimum_bps = 0xa000;
    block->observer.stream_bandwidth_step_max_fraction = 0.2f;
    block->observer.stream_rtt_noise_msec = 0x28;
    block->observer.stream_bandwidth_backoff_max_fraction = 0.3f;
    block->observer.stream_minimum_rtt_msec = 0x140;
    block->observer.stream_saturated_ratio = 0.75f;
    block->observer.stream_latency_average_shift = 3;
    block->observer.stream_growth_maximum_fraction = 0.5f;
    block->observer.stream_loss_window_size = 0x20;
    block->observer.stream_growth_desire_maximum = 20.0f;
    block->observer.stream_throttle_rtt_multiplier = 4;
    block->observer.stream_growth_desire_penalty_bandwidth_multiplier = 0.000390625f;
    block->observer.stream_throttle_cookie_event_count = 10;
    block->observer.bandwidth_monitor_ratio = 1.0f / 3.0f;
    block->observer.stream_throttle_cookie_backoff_threshold = 0x15;
    block->observer.stream_bandwidth_step_bps = 0x4000; // PC broadband: 16 kbps step
    block->observer.stream_bandwidth_backoff_bps = 0x1400;
    block->observer.stream_bandwidth_interval = 5000;
    block->observer.stream_growth_period_minimum_msec = 0x5dc;
    block->observer.stream_throughput_recording_period_minimum_msec = 0x5dc;
    block->observer.stream_total_average_shift = 2;
    block->observer.stream_congestion_offender_bandwidth_increment_bps = 0x1800;
    block->observer.stream_congestion_offender_timeout_msec = 20000;
    block->observer.stream_saturated_bps = 0x20000; // PC broadband: 128 kbps saturated
    block->observer.stream_saturated_interval = 60000;
    block->observer.stream_growth_maximum_count = 6;
    block->observer.stream_growth_desire_delay_multiplier = 0;
    block->observer.stream_latency_overhead = 0xa0;
    block->observer.stream_latency_overhead_probe = 0x14;
    block->observer.stream_latency_overhead_host = 0x78;
    block->observer.stream_growth_backoff_rtt_event_threshold = 3;
    block->observer.stream_growth_incremental_rtt_event_threshold = 10;
    block->observer.stream_growth_incremental_rtt_increase_msec = 10;
    block->observer.stream_cautious_expansion_limited_periods_count = 3;
    block->observer.stream_cautious_expansion_instability_timeout_msec = 8000;
    block->observer.bandwidth_monitor_interval = 6;
    block->observer.bandwidth_monitor_threshold = 0x2000;
    block->observer.bandwidth_monitor_constriction_threshold_bps = 0x1000;
    block->observer.time = 0x19;
    block->observer.bad_bandwidth_throughput_threshold = 0x32;
    block->observer.disable_bad_client_anticheating = 0;
    block->observer.disable_disconnection_badness = 0;
    block->observer.ignore_badness = 0;
    block->observer.stream_initial_packet_timeout = 9999;
    block->observer.stream_period = 1000;
    block->observer.stream_minimum_deviation = 5;
    block->observer.stream_growth_threshold_bps = 0x1000;
    block->observer.stream_growth_minimum_bps = 0x80;
    block->observer.stream_wants_more_allocation_fudge_factor = 0x1000;
    block->observer.stream_wants_more_allocation_fudge_factor_small = 0x80;
    block->observer.stream_maximum_instability_value = 5;
    block->observer.stream_backoff_failures = 2;
    block->observer.stream_backoff_time = 60000;
    block->observer.stream_loss_average_shift = 3;
    block->observer.stream_loss_deviation_shift = 2;
    block->observer.stream_latency_threshold_base = 0;
    block->observer.stream_loss_threshold_base = 2;
    block->observer.stream_loss_minimum_deviation = 4;
    block->observer.stream_loss_minimum_spread = 2;
    block->observer.stream_backoff_tolerance_bps = 0x1000;
    block->observer.stream_maximum_backoff_level = 4;
    block->observer.stream_loss_average_percentage = 0x19;
    block->observer.stream_loss_deviation_percentage = 200;
    block->observer.stream_bad_bandwidth_intervals = 2;
    block->observer.stream_bad_latency_intervals = 3;
    block->observer.stream_bad_loss_intervals = 2;
    block->observer.stream_bad_packet_loss_intervals = 2;
    block->observer.stream_low_bandwidth_bps = 0x2000;
    block->observer.stream_low_bandwidth_budget_bps = 0x4000;
    block->observer.stream_high_bandwidth_bps = 0x1000;
    block->observer.stream_high_bandwidth_budget_bps = 0x2000;
    block->observer.stream_bad_latency = 400;
    block->observer.stream_bad_loss_average = 0xf;
    block->observer.stream_bad_packet_loss_events = 4;
    block->observer.stream_latency_threshold_percentage = 0x7d;
    block->observer.stream_initial_bandwidth_percentage = 0x32;
    block->observer.stream_initial_client_bandwidth_percentage = 0x32;
    block->observer.stream_congested_percentage = 0x32;
    block->observer.stream_parameters[0].settle_scale = 0.8f;
    block->observer.stream_uncongested_percentage = 100;
    block->observer.stream_parameters[1].settle_scale = 0.8f;
    block->observer.stream_initial_low_percentage = 0x4b;
    block->observer.stream_parameters[2].settle_scale = 0.8f;
    block->observer.stream_initial_percentage = 0x32;
    block->observer.stream_parameters[3].settle_scale = 0.8f;
    block->observer.stream_quality_percentage = 0x32;
    block->observer.qos_to_live_interval_msec = 600000;
    block->observer.stream_maximum_growth_shift_voice = 6;
    block->observer.stream_maximum_growth_low_bps = 0x4000;
    block->observer.stream_maximum_growth_bps = 0x2000;
    block->observer.stream_throttle_base_time = 0xfa;
    block->observer.stream_throttle_time_per_stream = 100;
    block->observer.stream_throttle_random_percentage = 0x14;
    block->observer.stream_latency_spike_percentage = 0x78;
    block->observer.stream_total_growth_shift = 0x10;
    block->observer.stream_maximum_total_growth_shift = 2;
    block->observer.stream_backoff_on_spike = 1;
    block->observer.stream_maximum_probe_class = 4;
    block->observer.stream_congestion_minimum_rate = 5;
    block->observer.stream_congestion_maximum_latency = 0x400;
    block->observer.stream_retry_congestion = 0;
    block->observer.stream_clear_threshold_on_congestion = 0;
    block->observer.stream_exclude_probed = 1;
    block->observer.stream_always_count_failure = 0;
    block->observer.stream_count_throttle_as_failure = 0;
    block->observer.stream_limit_overprobe = 1;
    block->observer.stream_latency_spike_scaled = 1;
    block->observer.stream_ignore_failures = 1;
    block->observer.stream_packet_loss_percentage = 0x3c;
    block->observer.stream_packet_loss_minimum_packets = 4;
    block->observer.stream_packet_loss_ignore = 1;
    block->observer.stream_parameters[0].growth_base_bps = 0x400;
    block->observer.stream_parameters[0].growth_shift = 0;
    block->observer.stream_parameters[0].maximum_growth_shift = 2;
    block->observer.stream_parameters[0].probe_time = 5000;
    block->observer.stream_parameters[0].recover_time = 2000;
    block->observer.stream_parameters[0].settle_time = 6000;
    block->observer.stream_parameters[0].backoff_bps = 0x400;
    block->observer.stream_parameters[0].stable_time = 5000;
    block->observer.stream_parameters[0].overprobe_minimum_bps = 0x80;
    block->observer.stream_parameters[0].overprobe_maximum_bps = 0x2000;
    block->observer.stream_parameters[0].overprobe_percentage = 2;
    block->observer.stream_parameters[1].growth_base_bps = 0x400;
    block->observer.stream_parameters[1].growth_shift = 0;
    block->observer.stream_parameters[1].maximum_growth_shift = 2;
    block->observer.stream_parameters[1].probe_time = 2000;
    block->observer.stream_parameters[1].recover_time = 2000;
    block->observer.stream_parameters[1].settle_time = 6000;
    block->observer.stream_parameters[1].backoff_bps = 0x400;
    block->observer.stream_parameters[1].stable_time = 5000;
    block->observer.stream_parameters[1].overprobe_minimum_bps = 0;
    block->observer.stream_parameters[1].overprobe_maximum_bps = 0;
    block->observer.stream_parameters[1].overprobe_percentage = 0;
    block->observer.stream_parameters[2].growth_base_bps = 0x400;
    block->observer.stream_parameters[2].growth_shift = 0;
    block->observer.stream_parameters[2].maximum_growth_shift = 2;
    block->observer.stream_parameters[2].probe_time = 5000;
    block->observer.stream_parameters[2].recover_time = 2000;
    block->observer.stream_parameters[2].settle_time = 6000;
    block->observer.stream_parameters[2].backoff_bps = 0x400;
    block->observer.stream_parameters[2].stable_time = 5000;
    block->observer.stream_parameters[2].overprobe_minimum_bps = 0x80;
    block->observer.stream_parameters[2].overprobe_maximum_bps = 0x2000;
    block->observer.stream_parameters[2].overprobe_percentage = 2;
    block->observer.stream_parameters[3].growth_base_bps = 0x400;
    block->observer.stream_parameters[3].growth_shift = 0;
    block->observer.stream_parameters[3].maximum_growth_shift = 2;
    block->observer.stream_parameters[3].probe_time = 5000;
    block->observer.stream_parameters[3].recover_time = 2000;
    block->observer.stream_parameters[3].settle_time = 6000;
    block->observer.stream_parameters[3].backoff_bps = 0x400;
    block->observer.stream_parameters[3].stable_time = 5000;
    block->observer.stream_parameters[3].overprobe_minimum_bps = 0x80;
    block->observer.stream_parameters[3].overprobe_maximum_bps = 0x2000;
    block->observer.stream_parameters[3].overprobe_percentage = 2;
}

// 0x3a7200
extern "C" long network_configuration_try_and_get_data(s_network_configuration_downloader *downloader, void **out_data, unsigned long *out_data_size)
{
    long size;
    const char *data;
    long result;
    size = 0;
    data = 0;
    result = downloader->m_downloader.get_data(&data, &size);
    if (result == 2 && data != 0)
    {
        if (downloader->m_last_retry_count != downloader->m_downloader.m_retry_count)
        {
            downloader->m_data = network_blf_find_chunk_in_file(data, size, 0x6e657463, 125, _blf_file_authentication_none, &downloader->m_data_size);
        }
    }
    else
    {
        downloader->m_data = 0;
        downloader->m_data_size = 0;
    }
    if (out_data != 0)
    {
        *out_data = downloader->m_data;
    }
    if (out_data_size != 0)
    {
        *out_data_size = downloader->m_data != 0 ? downloader->m_data_size : 0;
    }
    return result;
}

// 0x4f32b8
extern "C" bool network_configuration_parse_lobby_film_max_players(void *, wchar_t *buffer, int size)
{
    usnzprintf(buffer, size, network_configuration_player_count_format(), g_network_configuration.maximum_theater_player_count);
    return true;
}

// 0x4f3360
extern "C" bool network_configuration_parse_lobby_forge_max_players(void *, wchar_t *buffer, int size)
{
    usnzprintf(buffer, size, network_configuration_player_count_format(), g_network_configuration.maximum_map_editor_player_count);
    return true;
}

// 0x4f33a0
extern "C" bool network_configuration_parse_lobby_coop_max_players(void *, wchar_t *buffer, int size)
{
    usnzprintf(buffer, size, network_configuration_player_count_format(), g_network_configuration.maximum_player_count);
    return true;
}

int network_configuration_get_player_class(s_player_profile_summary *profile)
{
    int result = 0;
    if (profile->highest_skill != -1 && !g_network_configuration_state.download_failed && g_network_configuration_state.downloaded_data_valid) {
        if (profile->global_rank >= 7 && profile->highest_skill >= g_network_configuration.rank_experience_thresholds[7])
            result = 4;
        else if (profile->global_rank >= 5)
            result = 3;
        else if (profile->experience_base >= g_network_configuration.basic_training_completion_minimum_experience
            || profile->games_completed >= g_network_configuration.basic_training_completion_minimum_games_completed)
            result = 2;
        else if (profile->global_rank >= 1 || profile->games_completed > 0)
            result = 1;
    }
    return result;
}


#if !defined(_XBOX) || defined(__clang__)
// The title server delivers the Xbox configuration layout in big-endian byte order.
static_assert(sizeof(unsigned long) == 4, "Network configuration words must be four bytes");
static_assert(sizeof(s_bandwidth_host_preference_weights) == 0x34, "Network configuration layout");
static_assert(sizeof(s_matchmaking_desirability_configuration) == 0x5c, "Network configuration layout");
static_assert(sizeof(s_entity_relevance_configuration) == 0x10, "Network configuration layout");
static_assert(sizeof(s_position_relevance_configuration) == 0x10, "Network configuration layout");
static_assert(sizeof(s_simulation_range) == 0x8, "Network configuration layout");
static_assert(sizeof(s_simulation_entity_creation_configuration) == 0x10, "Network configuration layout");
static_assert(sizeof(real_bounds) == 0x8, "Network configuration layout");
static_assert(sizeof(s_simulation_entity_update_configuration) == 0x3c, "Network configuration layout");
static_assert(sizeof(s_observer_connection_initiation_retry_parameters) == 0x24, "Network configuration layout");
static_assert(sizeof(s_network_observer_stream_parameters) == 0x30, "Network configuration layout");
static_assert(sizeof(s_channel_configuration) == 0x10, "Network configuration layout");
static_assert(sizeof(s_online_lsp_configuration) == 0x14, "Network configuration layout");
static_assert(sizeof(s_network_configuration_map_entry) == 0x8, "Network configuration layout");

template<typename t_scalar>
static void network_configuration_byte_swap(t_scalar &value)
{
    unsigned long bits;
    static_assert(sizeof(value) == sizeof(bits), "Network configuration scalar width");
    memcpy(&bits, &value, sizeof(bits));
    bits = network_blf_payload_unsigned_long(bits);
    memcpy(&value, &bits, sizeof(bits));
}

static void network_configuration_byte_swap(s_bandwidth_host_preference_weights &configuration)
{
    network_configuration_byte_swap(configuration.connectivity);
    network_configuration_byte_swap(configuration.local_host);
    network_configuration_byte_swap(configuration.peer_capacity);
    network_configuration_byte_swap(configuration.machine_capacity);
    network_configuration_byte_swap(configuration.player_capacity);
    network_configuration_byte_swap(configuration.latency);
    network_configuration_byte_swap(configuration.gamestate);
    network_configuration_byte_swap(configuration.open_nat);
    network_configuration_byte_swap(configuration.flagged);
    network_configuration_byte_swap(configuration.player_count);
    network_configuration_byte_swap(configuration.quality_tier);
    network_configuration_byte_swap(configuration.latency_tier);
    network_configuration_byte_swap(configuration.bandwidth_capacity);
}

static void network_configuration_byte_swap(s_bandwidth_configuration &configuration)
{
    network_configuration_byte_swap(configuration.trim_fraction);
    network_configuration_byte_swap(configuration.minimum_measurements);
    network_configuration_byte_swap(configuration.safety_margin);
    network_configuration_byte_swap(configuration.minimum_bandwidth);
    network_configuration_byte_swap(configuration.interval);
    network_configuration_byte_swap(configuration.ratio);
    network_configuration_byte_swap(configuration.dispute_threshold);
    network_configuration_byte_swap(configuration.dispute_margin);
    network_configuration_byte_swap(configuration.dispute_increment);
    network_configuration_byte_swap(configuration.host_migration_interval_campaign);
    network_configuration_byte_swap(configuration.host_migration_interval_matchmaking);
    network_configuration_byte_swap(configuration.host_migration_interval_group);
    network_configuration_byte_swap(configuration.host_migration_minimum_interval);
    network_configuration_byte_swap(configuration.host_speculative_migration_required_lobby_peer_connectivity_difference);
    network_configuration_byte_swap(configuration.latency_tier_threshold);
    network_configuration_byte_swap(configuration.capacity_threshold);
    network_configuration_byte_swap(configuration.other_latency_tier_threshold);
    network_configuration_byte_swap(configuration.other_capacity_threshold);
    for (s_bandwidth_host_preference_weights &entry : configuration.host_preference_weights)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.latency_tier_delta);
    network_configuration_byte_swap(configuration.quality_tier_delta);
}

static void network_configuration_byte_swap(s_matchmaking_desirability_configuration &configuration)
{
    network_configuration_byte_swap(configuration.base_desirability);
    network_configuration_byte_swap(configuration.desirability_per_player);
    network_configuration_byte_swap(configuration.minimum_player_count_reached_desirability);
    network_configuration_byte_swap(configuration.preferred_party_size_desirability);
    network_configuration_byte_swap(configuration.gather_seconds_threshold);
    network_configuration_byte_swap(configuration.gather_seconds_desirability);
    network_configuration_byte_swap(configuration.search_seconds_threshold);
    network_configuration_byte_swap(configuration.search_seconds_desirability);
    network_configuration_byte_swap(configuration.skill_difference_maximum);
    network_configuration_byte_swap(configuration.skill_difference_desirability);
    network_configuration_byte_swap(configuration.experience_difference_maximum);
    network_configuration_byte_swap(configuration.experience_difference_desirability);
    network_configuration_byte_swap(configuration.rank_difference_maximum);
    network_configuration_byte_swap(configuration.rank_difference_desirability);
    network_configuration_byte_swap(configuration.single_guest_desirability);
    network_configuration_byte_swap(configuration.region_desirability);
    network_configuration_byte_swap(configuration.country_desirability);
    network_configuration_byte_swap(configuration.language_desirability);
    network_configuration_byte_swap(configuration.composition_flag_desirability);
    network_configuration_byte_swap(configuration.unused_desirability);
    network_configuration_byte_swap(configuration.latency_maximum);
    network_configuration_byte_swap(configuration.latency_divisor);
    network_configuration_byte_swap(configuration.latency_scale);
}

static void network_configuration_byte_swap(s_entity_relevance_configuration &configuration)
{
    for (float &entry : configuration.zoom_cosine)
        network_configuration_byte_swap(entry);
    for (float &entry : configuration.zoom_relevance)
        network_configuration_byte_swap(entry);
}

static void network_configuration_byte_swap(s_position_relevance_configuration &configuration)
{
    network_configuration_byte_swap(configuration.minimum_distance);
    network_configuration_byte_swap(configuration.maximum_cosine);
    network_configuration_byte_swap(configuration.minimum_cosine);
    network_configuration_byte_swap(configuration.minimum_view_distance);
}

static void network_configuration_byte_swap(s_simulation_range &configuration)
{
    network_configuration_byte_swap(configuration.first);
    network_configuration_byte_swap(configuration.second);
}

static void network_configuration_byte_swap(s_simulation_event_configuration &configuration)
{
    network_configuration_byte_swap(configuration.relevance);
    network_configuration_byte_swap(configuration.delay_msec);
    network_configuration_byte_swap(configuration.input);
    network_configuration_byte_swap(configuration.minimum);
    network_configuration_byte_swap(configuration.maximum);
}

static void network_configuration_byte_swap(s_simulation_entity_creation_configuration &configuration)
{
    network_configuration_byte_swap(configuration.base_priority);
    network_configuration_byte_swap(configuration.relevance_radius);
    network_configuration_byte_swap(configuration.minimum_priority);
    network_configuration_byte_swap(configuration.maximum_priority);
}

static void network_configuration_byte_swap(real_bounds &configuration)
{
    network_configuration_byte_swap(configuration.lower);
    network_configuration_byte_swap(configuration.upper);
}

static void network_configuration_byte_swap(s_simulation_entity_update_configuration &configuration)
{
    network_configuration_byte_swap(configuration.base_priority);
    network_configuration_byte_swap(configuration.relevance_radius);
    network_configuration_byte_swap(configuration.relevance);
    network_configuration_byte_swap(configuration.minimum_period);
    network_configuration_byte_swap(configuration.maximum_period);
    network_configuration_byte_swap(configuration.initial_minimum_priority);
    network_configuration_byte_swap(configuration.initial_maximum_priority);
    network_configuration_byte_swap(configuration.maximum_delay);
    network_configuration_byte_swap(configuration.late_minimum_priority);
    network_configuration_byte_swap(configuration.late_maximum_priority);
    network_configuration_byte_swap(configuration.maximum_priority);
    network_configuration_byte_swap(configuration.special_priority);
    network_configuration_byte_swap(configuration.flagged_priority);
    network_configuration_byte_swap(configuration.occupied_vehicle_bonus);
}

static void network_configuration_byte_swap(s_simulation_entity_configuration &configuration)
{
    network_configuration_byte_swap(configuration.creation);
    network_configuration_byte_swap(configuration.update);
}

static void network_configuration_byte_swap(s_observer_connection_initiation_retry_parameters &configuration)
{
    network_configuration_byte_swap(configuration.retry_count);
    for (long &entry : configuration.retry_timeouts)
        network_configuration_byte_swap(entry);
}

static void network_configuration_byte_swap(s_network_observer_stream_parameters &configuration)
{
    network_configuration_byte_swap(configuration.growth_base_bps);
    network_configuration_byte_swap(configuration.growth_shift);
    network_configuration_byte_swap(configuration.maximum_growth_shift);
    network_configuration_byte_swap(configuration.probe_time);
    network_configuration_byte_swap(configuration.recover_time);
    network_configuration_byte_swap(configuration.settle_time);
    network_configuration_byte_swap(configuration.settle_scale);
    network_configuration_byte_swap(configuration.backoff_bps);
    network_configuration_byte_swap(configuration.stable_time);
    network_configuration_byte_swap(configuration.overprobe_minimum_bps);
    network_configuration_byte_swap(configuration.overprobe_maximum_bps);
    network_configuration_byte_swap(configuration.overprobe_percentage);
}

static void network_configuration_byte_swap(s_observer_configuration &configuration)
{
    for (s_observer_connection_initiation_retry_parameters &entry : configuration.initiation_retry)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.dead_observer_retry_delay);
    network_configuration_byte_swap(configuration.heartbeat_interval_msec);
    network_configuration_byte_swap(configuration.channel_activity_timeout);
    network_configuration_byte_swap(configuration.channel_activity_first_timeout);
    network_configuration_byte_swap(configuration.channel_activity_second_timeout);
    network_configuration_byte_swap(configuration.stream_inactivity_timeout);
    network_configuration_byte_swap(configuration.stream_receive_timeout);
    network_configuration_byte_swap(configuration.inactivity_timeout);
    network_configuration_byte_swap(configuration.receive_timeout);
    network_configuration_byte_swap(configuration.minimum_undesired_connection_timeout_msec);
    network_configuration_byte_swap(configuration.maximum_undesired_connection_timeout_msec);
    network_configuration_byte_swap(configuration.safe_sequence_threshold);
    network_configuration_byte_swap(configuration.safe_packet_rate_minimum);
    network_configuration_byte_swap(configuration.safe_packet_rate_maximum);
    network_configuration_byte_swap(configuration.stream_packet_rate_count);
    for (float &entry : configuration.stream_packet_rate_thresholds)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.packet_window_minimum_bytes);
    network_configuration_byte_swap(configuration.voice_fraction_authority);
    network_configuration_byte_swap(configuration.voice_fraction_client);
    network_configuration_byte_swap(configuration.voice_fraction_unattached);
    network_configuration_byte_swap(configuration.voice_maximum_packet_rate);
    network_configuration_byte_swap(configuration.voice_receive_fast_acknowledge_time);
    network_configuration_byte_swap(configuration.time_statistics_interval);
    network_configuration_byte_swap(configuration.window_statistics_count);
    network_configuration_byte_swap(configuration.flood_max_aperture_msec);
    network_configuration_byte_swap(configuration.flood_packet_fraction);
    network_configuration_byte_swap(configuration.stream_packet_overhead_low);
    network_configuration_byte_swap(configuration.stream_packet_overhead_high);
    network_configuration_byte_swap(configuration.stream_packet_rate_limiting_fraction);
    network_configuration_byte_swap(configuration.stream_minimum_estimated_bandwidth_bps);
    network_configuration_byte_swap(configuration.bandwidth_adjust_interval);
    network_configuration_byte_swap(configuration.bandwidth_flood_channel_fraction);
    network_configuration_byte_swap(configuration.bandwidth_flood_check_threshold);
    network_configuration_byte_swap(configuration.bandwidth_flood_backoff_kbps);
    network_configuration_byte_swap(configuration.bandwidth_flood_backoff_repeat_interval);
    network_configuration_byte_swap(configuration.bandwidth_clear_check_threshold);
    network_configuration_byte_swap(configuration.bandwidth_check_threshold_maximum);
    network_configuration_byte_swap(configuration.bandwidth_safety_margin_kbps);
    network_configuration_byte_swap(configuration.bandwidth_minimum_bps);
    network_configuration_byte_swap(configuration.bandwidth_increment_known_bad_threshold_kbps);
    network_configuration_byte_swap(configuration.bandwidth_increment_kbps);
    network_configuration_byte_swap(configuration.observer_stream_expansion_interval_msec);
    network_configuration_byte_swap(configuration.stream_minimum_bandwidth_bps);
    network_configuration_byte_swap(configuration.stream_maximum_bandwidth_bps);
    network_configuration_byte_swap(configuration.stream_initial_startup_msec);
    network_configuration_byte_swap(configuration.stream_minimum_game_bps);
    network_configuration_byte_swap(configuration.stream_initial_maximum_bps);
    network_configuration_byte_swap(configuration.stream_initial_low_maximum_bps);
    network_configuration_byte_swap(configuration.stream_minimum_host_bps);
    network_configuration_byte_swap(configuration.stream_maximum_probe_bps);
    network_configuration_byte_swap(configuration.stream_desired_bps);
    network_configuration_byte_swap(configuration.stream_desired_high_priority_bps);
    network_configuration_byte_swap(configuration.stream_desired_low_priority_bps);
    network_configuration_byte_swap(configuration.stream_voice_high_minimum_bps);
    network_configuration_byte_swap(configuration.stream_voice_low_minimum_bps);
    network_configuration_byte_swap(configuration.stream_voice_active_minimum_bps);
    network_configuration_byte_swap(configuration.stream_voice_high_desired_bps);
    network_configuration_byte_swap(configuration.stream_voice_low_desired_bps);
    network_configuration_byte_swap(configuration.stream_voice_active_desired_bps);
    network_configuration_byte_swap(configuration.stream_voice_shared_desired_bps);
    network_configuration_byte_swap(configuration.stream_voice_host_minimum_bps);
    network_configuration_byte_swap(configuration.stream_voice_low_host_minimum_bps);
    network_configuration_byte_swap(configuration.stream_rtt_noise_msec);
    network_configuration_byte_swap(configuration.stream_minimum_rtt_msec);
    network_configuration_byte_swap(configuration.stream_latency_average_shift);
    network_configuration_byte_swap(configuration.stream_loss_window_size);
    network_configuration_byte_swap(configuration.stream_loss_throttle_fraction);
    network_configuration_byte_swap(configuration.stream_throttle_rtt_multiplier);
    network_configuration_byte_swap(configuration.stream_throttle_reduce_multiplier);
    network_configuration_byte_swap(configuration.stream_throttle_cookie_event_count);
    network_configuration_byte_swap(configuration.stream_throttle_cookie_backoff_threshold);
    network_configuration_byte_swap(configuration.stream_bandwidth_step_bps);
    network_configuration_byte_swap(configuration.stream_bandwidth_step_max_fraction);
    network_configuration_byte_swap(configuration.stream_bandwidth_backoff_bps);
    network_configuration_byte_swap(configuration.stream_bandwidth_backoff_max_fraction);
    network_configuration_byte_swap(configuration.stream_bandwidth_interval);
    network_configuration_byte_swap(configuration.stream_growth_period_minimum_msec);
    network_configuration_byte_swap(configuration.stream_throughput_recording_period_minimum_msec);
    network_configuration_byte_swap(configuration.stream_saturated_bps);
    network_configuration_byte_swap(configuration.stream_saturated_ratio);
    network_configuration_byte_swap(configuration.stream_saturated_interval);
    network_configuration_byte_swap(configuration.stream_total_average_shift);
    network_configuration_byte_swap(configuration.stream_congestion_offender_bandwidth_increment_bps);
    network_configuration_byte_swap(configuration.stream_congestion_offender_timeout_msec);
    network_configuration_byte_swap(configuration.stream_growth_maximum_count);
    network_configuration_byte_swap(configuration.stream_growth_maximum_fraction);
    network_configuration_byte_swap(configuration.stream_growth_desire_maximum);
    network_configuration_byte_swap(configuration.stream_growth_desire_delay_multiplier);
    network_configuration_byte_swap(configuration.stream_growth_desire_penalty_bandwidth_multiplier);
    network_configuration_byte_swap(configuration.stream_latency_overhead);
    network_configuration_byte_swap(configuration.stream_latency_overhead_probe);
    network_configuration_byte_swap(configuration.stream_latency_overhead_host);
    network_configuration_byte_swap(configuration.stream_growth_backoff_rtt_event_threshold);
    network_configuration_byte_swap(configuration.stream_growth_incremental_rtt_event_threshold);
    network_configuration_byte_swap(configuration.stream_growth_incremental_rtt_increase_msec);
    network_configuration_byte_swap(configuration.stream_cautious_expansion_limited_periods_count);
    network_configuration_byte_swap(configuration.stream_cautious_expansion_instability_timeout_msec);
    network_configuration_byte_swap(configuration.bandwidth_monitor_interval);
    network_configuration_byte_swap(configuration.bandwidth_monitor_threshold);
    network_configuration_byte_swap(configuration.bandwidth_monitor_ratio);
    network_configuration_byte_swap(configuration.bandwidth_monitor_constriction_threshold_bps);
    network_configuration_byte_swap(configuration.client_badness_rating_threshold);
    network_configuration_byte_swap(configuration.bad_bandwidth_throughput_threshold);
    network_configuration_byte_swap(configuration.stream_initial_packet_timeout);
    network_configuration_byte_swap(configuration.stream_period);
    network_configuration_byte_swap(configuration.stream_minimum_deviation);
    network_configuration_byte_swap(configuration.stream_growth_threshold_bps);
    network_configuration_byte_swap(configuration.stream_growth_minimum_bps);
    network_configuration_byte_swap(configuration.stream_wants_more_allocation_fudge_factor);
    network_configuration_byte_swap(configuration.stream_wants_more_allocation_fudge_factor_small);
    network_configuration_byte_swap(configuration.stream_maximum_instability_value);
    network_configuration_byte_swap(configuration.stream_backoff_failures);
    network_configuration_byte_swap(configuration.stream_backoff_time);
    network_configuration_byte_swap(configuration.stream_loss_average_shift);
    network_configuration_byte_swap(configuration.stream_loss_deviation_shift);
    network_configuration_byte_swap(configuration.stream_latency_threshold_base);
    network_configuration_byte_swap(configuration.stream_loss_threshold_base);
    network_configuration_byte_swap(configuration.stream_loss_minimum_deviation);
    network_configuration_byte_swap(configuration.stream_loss_minimum_spread);
    network_configuration_byte_swap(configuration.stream_backoff_tolerance_bps);
    network_configuration_byte_swap(configuration.stream_maximum_backoff_level);
    network_configuration_byte_swap(configuration.stream_loss_average_percentage);
    network_configuration_byte_swap(configuration.stream_loss_deviation_percentage);
    network_configuration_byte_swap(configuration.stream_bad_bandwidth_intervals);
    network_configuration_byte_swap(configuration.stream_bad_latency_intervals);
    network_configuration_byte_swap(configuration.stream_bad_loss_intervals);
    network_configuration_byte_swap(configuration.stream_bad_packet_loss_intervals);
    network_configuration_byte_swap(configuration.stream_low_bandwidth_bps);
    network_configuration_byte_swap(configuration.stream_low_bandwidth_budget_bps);
    network_configuration_byte_swap(configuration.stream_high_bandwidth_bps);
    network_configuration_byte_swap(configuration.stream_high_bandwidth_budget_bps);
    network_configuration_byte_swap(configuration.stream_bad_latency);
    network_configuration_byte_swap(configuration.stream_bad_loss_average);
    network_configuration_byte_swap(configuration.stream_bad_packet_loss_events);
    network_configuration_byte_swap(configuration.stream_latency_threshold_percentage);
    network_configuration_byte_swap(configuration.stream_initial_bandwidth_percentage);
    network_configuration_byte_swap(configuration.stream_initial_client_bandwidth_percentage);
    network_configuration_byte_swap(configuration.stream_congested_percentage);
    network_configuration_byte_swap(configuration.stream_uncongested_percentage);
    network_configuration_byte_swap(configuration.stream_initial_low_percentage);
    network_configuration_byte_swap(configuration.stream_initial_percentage);
    network_configuration_byte_swap(configuration.stream_quality_percentage);
    network_configuration_byte_swap(configuration.qos_to_live_interval_msec);
    network_configuration_byte_swap(configuration.stream_maximum_growth_shift_voice);
    network_configuration_byte_swap(configuration.stream_maximum_growth_low_bps);
    network_configuration_byte_swap(configuration.stream_maximum_growth_bps);
    network_configuration_byte_swap(configuration.stream_throttle_base_time);
    network_configuration_byte_swap(configuration.stream_throttle_time_per_stream);
    network_configuration_byte_swap(configuration.stream_throttle_random_percentage);
    network_configuration_byte_swap(configuration.stream_latency_spike_percentage);
    network_configuration_byte_swap(configuration.stream_total_growth_shift);
    network_configuration_byte_swap(configuration.stream_maximum_total_growth_shift);
    network_configuration_byte_swap(configuration.stream_backoff_on_spike);
    network_configuration_byte_swap(configuration.stream_maximum_probe_class);
    network_configuration_byte_swap(configuration.stream_congestion_minimum_rate);
    network_configuration_byte_swap(configuration.stream_congestion_maximum_latency);
    network_configuration_byte_swap(configuration.stream_packet_loss_percentage);
    network_configuration_byte_swap(configuration.stream_packet_loss_minimum_packets);
    network_configuration_byte_swap(configuration.stream_packet_loss_ignore);
    for (s_network_observer_stream_parameters &entry : configuration.stream_parameters)
        network_configuration_byte_swap(entry);
}

static void network_configuration_byte_swap(s_channel_configuration &configuration)
{
    network_configuration_byte_swap(configuration.connect_packet_interval);
    network_configuration_byte_swap(configuration.connect_packet_count);
    network_configuration_byte_swap(configuration.connect_timeout);
    network_configuration_byte_swap(configuration.establishment_timeout);
}

static void network_configuration_byte_swap(s_online_lsp_configuration &configuration)
{
    network_configuration_byte_swap(configuration.port);
    network_configuration_byte_swap(configuration.port_count);
    network_configuration_byte_swap(configuration.minimum_score);
    network_configuration_byte_swap(configuration.search_timeout);
    network_configuration_byte_swap(configuration.idle_timeout);
}

static void network_configuration_byte_swap(s_network_configuration_map_entry &configuration)
{
    network_configuration_byte_swap(configuration.map_id);
    network_configuration_byte_swap(configuration.map_status);
}

static void network_configuration_byte_swap(s_network_configuration &configuration)
{
    network_configuration_byte_swap(configuration.request_cache_clear_interval);
    network_configuration_byte_swap(configuration.required_file_invalidation_check_interval_msec);
    network_configuration_byte_swap(configuration.required_file_download_retry_interval_msec);
    network_configuration_byte_swap(configuration.bandwidth);
    for (long &entry : configuration.upstream_bandwidth_absolute_minimum_bps)
        network_configuration_byte_swap(entry);
    for (long &entry : configuration.machine_capacity_bandwidth_thresholds)
        network_configuration_byte_swap(entry);
    for (long &entry : configuration.player_capacity_bandwidth_thresholds)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.matchmaking_host_minimum_machine_capacity);
    network_configuration_byte_swap(configuration.matchmaking_host_minimum_downstream_bandwidth);
    network_configuration_byte_swap(configuration.matchmaking_host_minimum_upstream_bandwidth);
    network_configuration_byte_swap(configuration.matchmaking_host_downstream_bandwidth_improvement);
    network_configuration_byte_swap(configuration.matchmaking_good_host_downstream_bandwidth);
    network_configuration_byte_swap(configuration.matchmaking_good_host_upstream_bandwidth);
    network_configuration_byte_swap(configuration.minimum_voice_repeater_downstream_bandwidth);
    network_configuration_byte_swap(configuration.minimum_voice_repeater_upstream_bandwidth);
    network_configuration_byte_swap(configuration.minimum_bandwidth_bps);
    network_configuration_byte_swap(configuration.join_host_timeout);
    network_configuration_byte_swap(configuration.join_disband_timeout);
    network_configuration_byte_swap(configuration.squad_join_timeout);
    network_configuration_byte_swap(configuration.request_migration_start_rety_interval_msec);
    network_configuration_byte_swap(configuration.request_migration_abort_rety_interval_msec);
    network_configuration_byte_swap(configuration.joining_search_qos_bps);
    network_configuration_byte_swap(configuration.group_join_timeout);
    network_configuration_byte_swap(configuration.desperation_wait_time_seconds);
    network_configuration_byte_swap(configuration.matchmaking_strict_nat_host_percentage);
    network_configuration_byte_swap(configuration.squad_start_failure_give_up_time_msec);
    network_configuration_byte_swap(configuration.minimum_joinable_sessions);
    network_configuration_byte_swap(configuration.join_delay);
    network_configuration_byte_swap(configuration.search_stale_timeout);
    network_configuration_byte_swap(configuration.search_join_delay);
    network_configuration_byte_swap(configuration.stream_period);
    network_configuration_byte_swap(configuration.group_join_delay);
    network_configuration_byte_swap(configuration.matchmaking_veto_countdown);
    network_configuration_byte_swap(configuration.matchmaking_loading_timeout);
    network_configuration_byte_swap(configuration.matchmaking_verify_countdown);
    network_configuration_byte_swap(configuration.matchmaking_ready_countdown);
    network_configuration_byte_swap(configuration.matchmaking_loading_countdown);
    network_configuration_byte_swap(configuration.end_game_write_stats_timeout_seconds);
    network_configuration_byte_swap(configuration.matchmaking_arbitration_registration_timeout_seconds);
    network_configuration_byte_swap(configuration.matchmaking_arbitration_completion_timeout_seconds);
    network_configuration_byte_swap(configuration.post_match_leaderboard_refresh_countdown);
    network_configuration_byte_swap(configuration.nat_warning_interval_seconds);
    network_configuration_byte_swap(configuration.simulation_abort_host_timeout);
    network_configuration_byte_swap(configuration.simulation_abort_peer_timeout);
    network_configuration_byte_swap(configuration.unsuitable_count);
    network_configuration_byte_swap(configuration.qos_bandwidth);
    network_configuration_byte_swap(configuration.qos_refresh_interval);
    network_configuration_byte_swap(configuration.qos_timeout);
    network_configuration_byte_swap(configuration.maximum_qos_target_count);
    network_configuration_byte_swap(configuration.session_qos_target_limit);
    network_configuration_byte_swap(configuration.matchmaking_desirability);
    network_configuration_byte_swap(configuration.search_start_delay);
    network_configuration_byte_swap(configuration.search_stage_timeout);
    network_configuration_byte_swap(configuration.search_final_delay);
    network_configuration_byte_swap(configuration.strict_skill_steps);
    network_configuration_byte_swap(configuration.strict_mu_delta_start);
    network_configuration_byte_swap(configuration.strict_mu_delta_end);
    network_configuration_byte_swap(configuration.strict_skill_range_start);
    network_configuration_byte_swap(configuration.strict_skill_range_end);
    network_configuration_byte_swap(configuration.strict_skill_level_delta_start);
    network_configuration_byte_swap(configuration.strict_skill_level_delta_end);
    network_configuration_byte_swap(configuration.strict_experience_delta_start);
    network_configuration_byte_swap(configuration.maximum_latency);
    network_configuration_byte_swap(configuration.skill_steps);
    network_configuration_byte_swap(configuration.skill_limit);
    network_configuration_byte_swap(configuration.matchmaking_search_state_timeout);
    network_configuration_byte_swap(configuration.any_maximum_latency);
    network_configuration_byte_swap(configuration.any_mu_delta);
    network_configuration_byte_swap(configuration.any_skill_level_delta);
    network_configuration_byte_swap(configuration.search_stage_any_final_desired_skill_range);
    network_configuration_byte_swap(configuration.leaderboard_query_failure_count);
    network_configuration_byte_swap(configuration.leaderboard_query_retry_delay_msec);
    network_configuration_byte_swap(configuration.leaderboard_query_maximum_retry_delay_msec);
    network_configuration_byte_swap(configuration.leaderboard_refresh_msec);
    network_configuration_byte_swap(configuration.peer_properties_update_interval);
    network_configuration_byte_swap(configuration.player_add_interval);
    network_configuration_byte_swap(configuration.player_remove_interval);
    network_configuration_byte_swap(configuration.user_properties_update_interval);
    network_configuration_byte_swap(configuration.film_reset_interval);
    network_configuration_byte_swap(configuration.post_match_recover_time);
    network_configuration_byte_swap(configuration.post_match_settle_time);
    network_configuration_byte_swap(configuration.qos_reply_update_interval);
    network_configuration_byte_swap(configuration.qos_reply_bandwidth_limit);
    network_configuration_byte_swap(configuration.squad_qos_reply_bandwidth_limit);
    network_configuration_byte_swap(configuration.squad_pregame_qos_reply_bandwidth_limit);
    network_configuration_byte_swap(configuration.group_qos_reply_bandwidth_limit);
    network_configuration_byte_swap(configuration.group_in_game_qos_reply_bandwidth_bps);
    network_configuration_byte_swap(configuration.maximum_player_count);
    network_configuration_byte_swap(configuration.maximum_map_editor_player_count);
    network_configuration_byte_swap(configuration.machine_file_timeout_seconds);
    network_configuration_byte_swap(configuration.machine_file_refresh_seconds);
    network_configuration_byte_swap(configuration.user_file_timeout_msec);
    network_configuration_byte_swap(configuration.user_file_request_timeout_msec);
    network_configuration_byte_swap(configuration.qos_quality_scale);
    network_configuration_byte_swap(configuration.controller_idle_timeout_seconds);
    network_configuration_byte_swap(configuration.entity_relevance);
    network_configuration_byte_swap(configuration.control_growth_base);
    network_configuration_byte_swap(configuration.control_growth_shift);
    network_configuration_byte_swap(configuration.control_maximum_growth_shift);
    network_configuration_byte_swap(configuration.control_probe_time);
    network_configuration_byte_swap(configuration.control_recover_time);
    network_configuration_byte_swap(configuration.position_relevance);
    network_configuration_byte_swap(configuration.maximum_value);
    for (s_simulation_range &entry : configuration.ranges)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.game_results_update_interval);
    network_configuration_byte_swap(configuration.synchronous_client_block_duration_msec);
    network_configuration_byte_swap(configuration.simulation_actor_control_timeout);
    network_configuration_byte_swap(configuration.projectile_supercombine_minimum_fraction);
    network_configuration_byte_swap(configuration.maximum_catchup_views);
    network_configuration_byte_swap(configuration.simulation_join_timeout);
    network_configuration_byte_swap(configuration.simulation_join_partial_timeout);
    network_configuration_byte_swap(configuration.simulation_join_complete_timeout);
    network_configuration_byte_swap(configuration.simulation_join_total_timeout);
    network_configuration_byte_swap(configuration.simulation_join_active_fraction);
    network_configuration_byte_swap(configuration.simulation_join_completion_threshold);
    network_configuration_byte_swap(configuration.simulation_maximum_catchup_attempts);
    network_configuration_byte_swap(configuration.simulation_catchup_retry_delay);
    network_configuration_byte_swap(configuration.simulation_maximum_join_attempts);
    network_configuration_byte_swap(configuration.simulation_join_failure_timeout);
    network_configuration_byte_swap(configuration.simulation_queue_allocated_bytes_limit);
    network_configuration_byte_swap(configuration.simulation_queue_allocated_elements_limit);
    for (s_simulation_event_configuration &entry : configuration.event_relevance)
        network_configuration_byte_swap(entry);
    for (s_simulation_entity_configuration &entry : configuration.records)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.position_update_distance);
    network_configuration_byte_swap(configuration.position_update_distance_remote);
    network_configuration_byte_swap(configuration.vehicle_position_update_distance);
    network_configuration_byte_swap(configuration.vehicle_position_update_distance_remote);
    network_configuration_byte_swap(configuration.supercombine_minimum_time);
    network_configuration_byte_swap(configuration.supercombine_minimum_distance);
    network_configuration_byte_swap(configuration.trigger_mispredict_time);
    network_configuration_byte_swap(configuration.weapon_ticks_seconds);
    network_configuration_byte_swap(configuration.weapon_ticks_threshold);
    network_configuration_byte_swap(configuration.weapon_ticks_minimum);
    network_configuration_byte_swap(configuration.event_timeout);
    network_configuration_byte_swap(configuration.default_priority);
    network_configuration_byte_swap(configuration.stale_timeout);
    network_configuration_byte_swap(configuration.stale_priority);
    network_configuration_byte_swap(configuration.stale_offset);
    network_configuration_byte_swap(configuration.stale_scale);
    network_configuration_byte_swap(configuration.current_offset);
    network_configuration_byte_swap(configuration.current_scale);
    network_configuration_byte_swap(configuration.replication_request_limit);
    network_configuration_byte_swap(configuration.managed_session_retry_interval);
    network_configuration_byte_swap(configuration.peer_join_request_check_interval);
    network_configuration_byte_swap(configuration.peer_join_secure_connection_timeout);
    network_configuration_byte_swap(configuration.peer_join_initial_update_timeout);
    network_configuration_byte_swap(configuration.join_queue_timeout);
    network_configuration_byte_swap(configuration.join_queue_acknowledge_period);
    network_configuration_byte_swap(configuration.peer_join_abort_check_interval);
    network_configuration_byte_swap(configuration.peer_join_abort_timeout);
    network_configuration_byte_swap(configuration.host_rejoin_accept_timeout_msec);
    network_configuration_byte_swap(configuration.peer_leave_timeout);
    network_configuration_byte_swap(configuration.peer_leave_request_check_interval);
    network_configuration_byte_swap(configuration.host_handoff_acknowledge_timeout);
    network_configuration_byte_swap(configuration.host_handoff_stabilize_timeout);
    network_configuration_byte_swap(configuration.host_handoff_timeout);
    network_configuration_byte_swap(configuration.host_handoff_transition_timeout);
    network_configuration_byte_swap(configuration.host_reestablish_acknowledge_timeout);
    network_configuration_byte_swap(configuration.host_reestablish_message_interval);
    network_configuration_byte_swap(configuration.election_timeout);
    network_configuration_byte_swap(configuration.election_peer_timeout);
    network_configuration_byte_swap(configuration.election_ignore_dissension_msec);
    network_configuration_byte_swap(configuration.election_message_interval);
    network_configuration_byte_swap(configuration.time_synchronization_interval_a);
    network_configuration_byte_swap(configuration.time_synchronization_interval_b);
    network_configuration_byte_swap(configuration.minimum_election_send_interval_msec);
    network_configuration_byte_swap(configuration.host_migration_on_channel_death);
    network_configuration_byte_swap(configuration.observer);
    network_configuration_byte_swap(configuration.channel_configuration);
    network_configuration_byte_swap(configuration.packet_statistics_interval);
    network_configuration_byte_swap(configuration.connection_sequence_rate);
    network_configuration_byte_swap(configuration.connection_ancient_packet_threshold);
    network_configuration_byte_swap(configuration.connection_drop_threshold);
    network_configuration_byte_swap(configuration.connection_packet_retire_time);
    network_configuration_byte_swap(configuration.latency_average_gain_bits);
    network_configuration_byte_swap(configuration.latency_deviation_shift);
    network_configuration_byte_swap(configuration.stream_timeout_deviation_scale);
    network_configuration_byte_swap(configuration.stream_timeout_minimum_margin);
    network_configuration_byte_swap(configuration.stream_timeout_minimum);
    network_configuration_byte_swap(configuration.timeout_deviation_scale);
    network_configuration_byte_swap(configuration.timeout_minimum_margin);
    network_configuration_byte_swap(configuration.timeout_minimum);
    network_configuration_byte_swap(configuration.initial_latency_minimum_msec);
    network_configuration_byte_swap(configuration.initial_latency_average_msec);
    network_configuration_byte_swap(configuration.initial_latency_deviation);
    network_configuration_byte_swap(configuration.initial_timeout_msec);
    network_configuration_byte_swap(configuration.backoff_increment_msec);
    network_configuration_byte_swap(configuration.backoff_max_msec);
    network_configuration_byte_swap(configuration.backoff_decrement_msec);
    network_configuration_byte_swap(configuration.bandwidth_warning_latency_multiplier);
    network_configuration_byte_swap(configuration.bandwidth_warning_latency_minimum_increase);
    network_configuration_byte_swap(configuration.address_resolution_timeout_msec);
    for (long &entry : configuration.transport_qos_probe_count)
        network_configuration_byte_swap(entry);
    for (long &entry : configuration.transport_qos_bandwidth)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.upstream_cap_multiplier);
    network_configuration_byte_swap(configuration.upstream_cap_divisor);
    network_configuration_byte_swap(configuration.push_to_talk_idle_timeout);
    network_configuration_byte_swap(configuration.push_to_talk_timeout);
    network_configuration_byte_swap(configuration.maximum_voice_repeater_players);
    network_configuration_byte_swap(configuration.ui_upload_record_threshold);
    network_configuration_byte_swap(configuration.ui_upload_time_threshold);
    network_configuration_byte_swap(configuration.betrayal_decay_interval);
    network_configuration_byte_swap(configuration.quit_decay_interval);
    network_configuration_byte_swap(configuration.betrayal_threshold);
    network_configuration_byte_swap(configuration.ejection_cutoff);
    network_configuration_byte_swap(configuration.friendly_assist_amount);
    network_configuration_byte_swap(configuration.network_heap_large_size);
    network_configuration_byte_swap(configuration.network_heap_small_size);
    network_configuration_byte_swap(configuration.allow_matchmaking_abort_interval_msec);
    network_configuration_byte_swap(configuration.maximum_theater_player_count);
    network_configuration_byte_swap(configuration.basic_training_completion_minimum_games_completed);
    network_configuration_byte_swap(configuration.basic_training_completion_minimum_experience);
    for (long &entry : configuration.skill_level_experience_thresholds)
        network_configuration_byte_swap(entry);
    for (long &entry : configuration.rank_grade_thresholds)
        network_configuration_byte_swap(entry);
    for (long &entry : configuration.rank_experience_thresholds)
        network_configuration_byte_swap(entry);
    network_configuration_byte_swap(configuration.alpha_locked_down);
    network_configuration_byte_swap(configuration.maximum_players_per_peer);
    network_configuration_byte_swap(configuration.crash_minidump_mode);
    network_configuration_byte_swap(configuration.online_lsp);
    for (s_network_configuration_map_entry &entry : configuration.map_table)
        network_configuration_byte_swap(entry);
}
#endif

// 0x8f698


// @address 0x8f698 network_configuration_update
extern "C" void network_configuration_update(void)
{
    bool changed = false;
    if (g_network_configuration_update_version != network_session_interface_get_online_url_version()) {
        g_network_configuration_update_version = network_session_interface_get_online_url_version();
        changed = true;
    }
    if (changed) {
        s_network_http_buffer_url url;
        url.name[0] = 0;
        url.service_type = 0;
        url.cache_mode = 0;
        url.queue_type = 0;
        url.lifetime_seconds = 0x258;
        online_url_set_base_path((c_online_url_string *)&url);
        ((c_static_string<256> *)url.name)->append_print("network_configuration_%03u.bin", 125);
        url.queue_type = 0;
        url.cache_mode = 1;
        http_buffer_downloader_set_url(&g_network_configuration_state.download.m_downloader, &url);
    }
    g_network_configuration_state.download.m_downloader.get_data();
    if (g_network_configuration_state.last_processed_retry_count == g_network_configuration_state.download.m_downloader.m_retry_count)
        return;
    void *out_data;
    unsigned long out_data_size;
    switch (network_configuration_try_and_get_data(&g_network_configuration_state.download, &out_data, &out_data_size)) {
    case 0:
    case 3:
    case 4:
        g_network_configuration_state.download_failed = true;
        break;
    case 2:
        if (out_data != 0
#if !defined(_XBOX) || defined(__clang__)
            && out_data_size == sizeof(g_network_configuration)
#endif
        ) {
            memcpy(&g_network_configuration, out_data, sizeof(g_network_configuration));
#if !defined(_XBOX) || defined(__clang__)
            network_configuration_byte_swap(g_network_configuration);
#endif
            g_network_configuration_state.download_failed = false;
            g_network_configuration_state.downloaded_data_valid = true;
            break;
        }
        g_network_configuration_state.download_failed = true;
        break;
    case 1:
    default:
        return;
    }
    g_network_configuration_state.last_processed_retry_count = g_network_configuration_state.download.m_downloader.m_retry_count;
}

// @address 0x3a5518 ?network_configuration_initialize_private@@YAX_N@Z
void network_configuration_initialize_private(bool unused)
{
    s_network_configuration *block = &g_network_configuration;
    memset(block, 0, sizeof(*block));
    block->request_cache_clear_interval = 3600000;
    block->required_file_invalidation_check_interval_msec = 5000;
    block->bandwidth.trim_fraction = 0.25f;
    block->required_file_download_retry_interval_msec = 5000;
    block->bandwidth.ratio = 0.5f;
    block->bandwidth.minimum_measurements = 2;
    block->bandwidth.safety_margin = 0x5000;
    block->bandwidth.minimum_bandwidth = 0xc800;
    block->bandwidth.interval = 300000;
    block->bandwidth.dispute_threshold = 5;
    block->bandwidth.dispute_margin = 0x10000;
    block->bandwidth.dispute_increment = 0x8000;
    block->bandwidth.host_migration_interval_campaign = 30000;
    block->bandwidth.host_migration_interval_matchmaking = 30000;
    block->bandwidth.host_migration_interval_group = 30000;
    block->bandwidth.host_migration_minimum_interval = 60000;
    block->bandwidth.host_speculative_migration_required_lobby_peer_connectivity_difference = 1;
    block->bandwidth.latency_tier_threshold = 0;
    block->bandwidth.capacity_threshold = 0;
    block->bandwidth.other_latency_tier_threshold = 0;
    block->bandwidth.other_capacity_threshold = 1;
    block->bandwidth.latency_scores[0] = 0;
    block->bandwidth.latency_scores[1] = 15;
    block->bandwidth.latency_scores[2] = 15;
    block->bandwidth.latency_scores[3] = 14;
    block->bandwidth.latency_scores[4] = 13;
    block->bandwidth.latency_scores[5] = 12;
    block->bandwidth.latency_scores[6] = 11;
    block->bandwidth.latency_scores[7] = 10;
    block->bandwidth.latency_scores[8] = 9;
    block->bandwidth.latency_scores[9] = 8;
    block->bandwidth.latency_scores[10] = 7;
    block->bandwidth.latency_scores[11] = 6;
    block->bandwidth.latency_scores[12] = 5;
    block->bandwidth.latency_scores[13] = 4;
    block->bandwidth.latency_scores[14] = 3;
    block->bandwidth.latency_scores[15] = 2;
    block->bandwidth.latency_scores[16] = 1;
    block->bandwidth.latency_scores[17] = 0;
    block->bandwidth.latency_scores[18] = 0;
    block->bandwidth.latency_scores[19] = 0;
    block->bandwidth.player_count_scores[0] = 3;
    block->bandwidth.player_count_scores[1] = 2;
    block->bandwidth.player_count_scores[2] = 1;
    block->bandwidth.player_count_scores[3] = 0;
    block->bandwidth.host_preference_weights[0].connectivity = 0x10000000;
    block->bandwidth.host_preference_weights[0].local_host = 1;
    block->bandwidth.host_preference_weights[0].peer_capacity = 0;
    block->bandwidth.host_preference_weights[0].machine_capacity = 0;
    block->bandwidth.host_preference_weights[0].player_capacity = 0;
    block->bandwidth.host_preference_weights[0].latency = 0;
    block->bandwidth.host_preference_weights[0].gamestate = 0;
    block->bandwidth.host_preference_weights[0].open_nat = 0x100000;
    block->bandwidth.host_preference_weights[0].flagged = 0x10000;
    block->bandwidth.host_preference_weights[0].player_count = 0;
    block->bandwidth.host_preference_weights[0].quality_tier = 0;
    block->bandwidth.host_preference_weights[0].latency_tier = 0x1000000;
    block->bandwidth.host_preference_weights[0].bandwidth_capacity = 0;
    block->bandwidth.host_preference_weights[1].connectivity = 0x10000000;
    block->bandwidth.host_preference_weights[1].local_host = 1;
    block->bandwidth.host_preference_weights[1].peer_capacity = 0x1000000;
    block->bandwidth.host_preference_weights[1].machine_capacity = 256;
    block->bandwidth.host_preference_weights[1].player_capacity = 16;
    block->bandwidth.host_preference_weights[1].latency = 0x1000;
    block->bandwidth.host_preference_weights[1].gamestate = 4;
    block->bandwidth.host_preference_weights[1].open_nat = 0;
    block->bandwidth.host_preference_weights[1].flagged = 2;
    block->bandwidth.host_preference_weights[1].player_count = 0x10000;
    block->bandwidth.host_preference_weights[1].quality_tier = 0x100000;
    block->bandwidth.host_preference_weights[1].latency_tier = 0;
    block->bandwidth.host_preference_weights[1].bandwidth_capacity = 0;
    block->bandwidth.host_preference_weights[2].connectivity = 0x10000000;
    block->bandwidth.host_preference_weights[2].local_host = 1;
    block->bandwidth.host_preference_weights[2].peer_capacity = 0x1000000;
    block->bandwidth.host_preference_weights[2].machine_capacity = 2048;
    block->bandwidth.host_preference_weights[2].player_capacity = 128;
    block->bandwidth.host_preference_weights[2].latency = 0x8000;
    block->bandwidth.host_preference_weights[2].gamestate = 4;
    block->bandwidth.host_preference_weights[2].open_nat = 8;
    block->bandwidth.host_preference_weights[2].flagged = 2;
    block->bandwidth.host_preference_weights[2].player_count = 0x80000;
    block->bandwidth.host_preference_weights[2].quality_tier = 0x200000;
    block->bandwidth.host_preference_weights[2].latency_tier = 16;
    block->bandwidth.host_preference_weights[2].bandwidth_capacity = 0;
    block->bandwidth.host_preference_weights[3].connectivity = 0x10000000;
    block->bandwidth.host_preference_weights[3].local_host = 1;
    block->bandwidth.host_preference_weights[3].peer_capacity = 0;
    block->bandwidth.host_preference_weights[3].machine_capacity = 0;
    block->bandwidth.host_preference_weights[3].player_capacity = 16;
    block->bandwidth.host_preference_weights[3].latency = 0x1000;
    block->bandwidth.host_preference_weights[3].gamestate = 4;
    block->bandwidth.host_preference_weights[3].open_nat = 0;
    block->bandwidth.host_preference_weights[3].flagged = 2;
    block->bandwidth.host_preference_weights[3].player_count = 0x10000;
    block->bandwidth.host_preference_weights[3].quality_tier = 0x100000;
    block->bandwidth.host_preference_weights[3].latency_tier = 0;
    block->bandwidth.host_preference_weights[3].bandwidth_capacity = 0x1000000;
    block->bandwidth.latency_tier_delta = 5;
    block->bandwidth.quality_tier_delta = 10;
    for (long i = 0; i <= 16; i++)
    {
        long a;
        long b;
        if (i <= 1)
        {
            a = 0;
            b = 0;
        }
        else
        {
            long x;
            long y;
            if (i < 4)
            {
                x = 0x2000;
                y = 0xa000;
            }
            else
            {
                float t = (float)(i - 4) * (1.0f / 12.0f);
                float fx = t * 9216.0f + 8192.0f;
                float fy = t * 30720.0f + 40960.0f;
                x = (long)(fx + 0.5f);
                y = (long)(fy + 0.5f);
            }
            a = (i - 1) * x + 0x3c00;
            b = (i - 1) * y + 0xf000;
        }
        if (i > 0)
            block->upstream_bandwidth_absolute_minimum_bps[i] = a;
        block->machine_capacity_bandwidth_thresholds[i] = a;
        block->player_capacity_bandwidth_thresholds[i] = b;
    }
    block->matchmaking_host_minimum_machine_capacity = 8;
    block->matchmaking_host_minimum_downstream_bandwidth = 0x20000;
    block->matchmaking_strict_nat_host_percentage = 0.25f;
    block->matchmaking_host_minimum_upstream_bandwidth = 0x20000;
    block->matchmaking_desirability.experience_difference_maximum = 0.5f;
    block->matchmaking_host_downstream_bandwidth_improvement = 0x2800;
    block->strict_mu_delta_start = 0.05f;
    block->matchmaking_good_host_downstream_bandwidth = 0x40000;
    block->strict_mu_delta_end = 0.5f;
    block->matchmaking_good_host_upstream_bandwidth = 0x40000;
    block->minimum_voice_repeater_downstream_bandwidth = 0x20000;
    block->minimum_voice_repeater_upstream_bandwidth = 0x20000;
    block->minimum_bandwidth_bps = 0x3000;
    block->join_disband_timeout = 10000;
    block->join_host_timeout = 30000;
    block->squad_join_timeout = 30000;
    block->request_migration_start_rety_interval_msec = 100;
    block->request_migration_abort_rety_interval_msec = 100;
    block->joining_search_qos_bps = 0x10000;
    block->group_join_timeout = 10000;
    block->desperation_wait_time_seconds = 180;
    block->matchmaking_disallow_restricted_nat_hosts = 0;
    block->squad_start_failure_give_up_time_msec = 5000;
    block->minimum_joinable_sessions = 10;
    block->join_delay = 4000;
    block->stream_period = 600;
    block->search_stale_timeout = 25000;
    block->search_join_delay = 15000;
    block->group_join_delay = 3000;
    block->prepare_map_display_map_during_loading = 1;
    block->matchmaking_veto_countdown = 10;
    block->matchmaking_loading_timeout = 5;
    block->matchmaking_verify_countdown = 5;
    block->matchmaking_ready_countdown = 5;
    block->matchmaking_loading_countdown = 5;
    block->end_game_write_stats_timeout_seconds = 30;
    block->matchmaking_arbitration_registration_timeout_seconds = 60;
    block->matchmaking_arbitration_completion_timeout_seconds = 30;
    block->return_to_pre_game = 0;
    block->post_match_leaderboard_refresh_countdown = 5;
    block->nat_warning_interval_seconds = 14400;
    block->simulation_abort_host_timeout = 4000;
    block->simulation_abort_peer_timeout = 8000;
    block->unsuitable_count = 250;
    block->qos_bandwidth = 0x10000;
    block->qos_refresh_interval = 20000;
    block->qos_timeout = 5000;
    block->maximum_qos_target_count = 1;
    block->session_qos_target_limit = 5;
    block->matchmaking_desirability.base_desirability = 10;
    block->matchmaking_desirability.desirability_per_player = 15;
    block->matchmaking_desirability.minimum_player_count_reached_desirability = 75;
    block->matchmaking_desirability.preferred_party_size_desirability = 50;
    block->matchmaking_desirability.gather_seconds_threshold = 30;
    block->matchmaking_desirability.gather_seconds_desirability = 25;
    block->matchmaking_desirability.search_seconds_threshold = 30;
    block->matchmaking_desirability.search_seconds_desirability = 25;
    block->matchmaking_desirability.skill_difference_maximum = 2;
    block->matchmaking_desirability.skill_difference_desirability = 50;
    block->matchmaking_desirability.experience_difference_desirability = 50;
    block->matchmaking_desirability.rank_difference_maximum = 3;
    block->matchmaking_desirability.rank_difference_desirability = 50;
    block->matchmaking_desirability.single_guest_desirability = 10;
    block->matchmaking_desirability.region_desirability = 10;
    block->matchmaking_desirability.country_desirability = 10;
    block->matchmaking_desirability.language_desirability = 10;
    block->matchmaking_desirability.composition_flag_desirability = 10;
    block->matchmaking_desirability.unused_desirability = 10;
    block->matchmaking_desirability.latency_maximum = 400;
    block->matchmaking_desirability.latency_divisor = 50;
    block->matchmaking_desirability.latency_scale = 50;
    block->search_start_delay = 1000;
    block->search_stage_timeout = 5000;
    block->search_final_delay = 10000;
    block->strict_skill_steps = 5;
    block->strict_skill_range_start = 0;
    block->strict_skill_range_end = 3;
    block->strict_skill_level_delta_start = 0;
    block->strict_skill_level_delta_end = 2;
    block->strict_experience_delta_start = 0;
    block->any_mu_delta = 6.0f;
    block->maximum_latency = 200;
    block->skill_steps = 3;
    block->skill_limit = 200;
    block->matchmaking_search_state_timeout = 4;
    block->any_maximum_latency = 1000;
    block->any_skill_level_delta = 10;
    block->search_stage_any_final_desired_skill_range = 50;
    block->leaderboard_query_failure_count = 3;
    block->leaderboard_query_retry_delay_msec = 5000;
    block->leaderboard_query_maximum_retry_delay_msec = 300000;
    block->leaderboard_refresh_msec = 5000;
    block->peer_properties_update_interval = 1000;
    block->player_add_interval = 1000;
    block->player_remove_interval = 1000;
    block->user_properties_update_interval = 1000;
    block->film_reset_interval = 5000;
    block->post_match_recover_time = 2000;
    block->post_match_settle_time = 5000;
    block->qos_reply_update_interval = 100;
    block->qos_reply_bandwidth_limit = 0x4000;
    block->squad_qos_reply_bandwidth_limit = 0x8000;
    block->squad_pregame_qos_reply_bandwidth_limit = 0x4000;
    block->group_qos_reply_bandwidth_limit = 0x10000;
    block->group_in_game_qos_reply_bandwidth_bps = 0x4000;
    block->maximum_player_count = 4;
    block->maximum_map_editor_player_count = 8;
    block->machine_file_timeout_seconds = 900;
    block->machine_file_refresh_seconds = 30;
    block->user_file_timeout_msec = 900;
    block->user_file_request_timeout_msec = 30;
    block->qos_quality_scale = 50;
    block->controller_idle_timeout_seconds = 120;
    network_configuration_initialize_simulation_constants();
    block->event_timeout = 20000;
    block->default_priority = 0.5f;
    block->stale_timeout = 1000;
    block->stale_priority = 0.84f;
    block->stale_offset = 0.62f;
    block->replication_request_limit = 0;
    block->stale_scale = 0.25f;
    block->current_offset = 0.32f;
    block->current_scale = 0.3f;
    network_configuration_initialize_observer_constants();
    block->managed_session_retry_interval = 15000;
    block->peer_join_request_check_interval = 1000;
    block->peer_join_secure_connection_timeout = 8000;
    block->peer_join_initial_update_timeout = 16000;
    block->join_queue_timeout = 60000;
    block->join_queue_acknowledge_period = 2000;
    block->peer_join_abort_check_interval = 200;
    block->peer_join_abort_timeout = 1000;
    block->host_rejoin_accept_timeout_msec = 6000;
    block->peer_leave_timeout = 1500;
    block->peer_leave_request_check_interval = 500;
    block->host_handoff_acknowledge_timeout = 10000;
    block->host_handoff_stabilize_timeout = 30000;
    block->host_handoff_timeout = 60000;
    block->host_handoff_transition_timeout = 10000;
    block->host_reestablish_acknowledge_timeout = 15000;
    block->host_reestablish_message_interval = 10000;
    block->election_timeout = 45000;
    block->election_peer_timeout = 4000;
    block->election_ignore_dissension_msec = 6000;
    block->election_message_interval = 1000;
    block->time_synchronization_interval_a = 120000;
    block->time_synchronization_interval_b = 5000;
    block->minimum_election_send_interval_msec = 500;
    block->host_migration_on_channel_death = 1;
    block->channel_configuration.connect_packet_interval = 500;
    block->channel_configuration.connect_packet_count = 3;
    block->channel_configuration.connect_timeout = 2000;
    block->channel_configuration.establishment_timeout = 6000;
    block->packet_statistics_interval = 5000;
    block->connection_sequence_rate = 20;
    block->connection_ancient_packet_threshold = 64;
    block->connection_drop_threshold = 3;
    block->connection_packet_retire_time = 5000;
    block->latency_average_gain_bits = 3;
    block->latency_deviation_shift = 2;
    block->bandwidth_warning_latency_multiplier = 3.0f;
    block->push_to_talk_idle_timeout = 1.0f;
    block->push_to_talk_timeout = 10.0f;
    block->betrayal_threshold = 2.9999f;
    block->stream_timeout_deviation_scale = 4;
    block->stream_timeout_minimum_margin = 80;
    block->stream_timeout_minimum = 150;
    block->timeout_deviation_scale = 2;
    block->timeout_minimum_margin = 15;
    block->timeout_minimum = 50;
    block->initial_latency_minimum_msec = 9999;
    block->initial_latency_average_msec = 150;
    block->initial_latency_deviation = 1000;
    block->initial_timeout_msec = 200;
    block->backoff_increment_msec = 100;
    block->backoff_max_msec = 700;
    block->backoff_decrement_msec = 200;
    block->bandwidth_warning_latency_minimum_increase = 400;
    block->address_resolution_timeout_msec = 10000;
    block->transport_qos_probe_count[1] = 4;
    block->transport_qos_probe_count[0] = 0;
    block->transport_qos_bandwidth[1] = 0x4000;
    block->transport_qos_bandwidth[0] = 0x1000;
    block->cap_live_service_upstream = 1;
    block->upstream_cap_multiplier = 75;
    block->upstream_cap_divisor = 4;
    block->voice_repeater_enabled = false;
    block->maximum_voice_repeater_players = 4;
    block->ui_upload_record_threshold = 350;
    block->ui_upload_time_threshold = 600000;
    block->record_uploads_prevent_game_from_starting = 0;
    block->betrayal_decay_interval = 600000;
    block->quit_decay_interval = 3600000;
    block->ejection_cutoff = 2;
    block->network_heap_large_size = 0x180000;
    block->network_heap_small_size = 0x180000;
    block->allow_matchmaking_abort_interval_msec = 60000;
    block->maximum_theater_player_count = 4;
    block->basic_training_completion_minimum_games_completed = 10;
    block->basic_training_completion_minimum_experience = 5;
    // The initial fill uses the skill level count as its byte count.
    memset(&block->skill_level_experience_thresholds[0], -1, k_network_configuration_skill_level_count);
    block->skill_level_experience_thresholds[0] = 0;
    block->skill_level_experience_thresholds[1] = 1;
    block->skill_level_experience_thresholds[2] = 2;
    block->skill_level_experience_thresholds[3] = 5;
    block->skill_level_experience_thresholds[4] = 9;
    block->rank_grade_thresholds[4] = 1;
    block->rank_grade_thresholds[8] = 2;
    block->rank_grade_thresholds[9] = 3;
    block->rank_grade_thresholds[12] = 5;
    block->rank_grade_thresholds[13] = 7;
    block->rank_grade_thresholds[16] = 10;
    block->rank_grade_thresholds[17] = 15;
    block->rank_grade_thresholds[20] = 20;
    block->rank_grade_thresholds[21] = 30;
    block->rank_grade_thresholds[22] = 40;
    block->rank_grade_thresholds[24] = 50;
    block->rank_grade_thresholds[25] = 60;
    block->rank_grade_thresholds[26] = 150;
    block->rank_grade_thresholds[27] = 300;
    block->rank_grade_thresholds[28] = 70;
    block->rank_grade_thresholds[29] = 85;
    block->rank_grade_thresholds[30] = 200;
    block->rank_grade_thresholds[31] = 400;
    block->rank_grade_thresholds[32] = 100;
    block->rank_grade_thresholds[33] = 150;
    block->rank_grade_thresholds[34] = 300;
    block->rank_grade_thresholds[35] = 600;
    block->rank_grade_thresholds[36] = 200;
    block->rank_grade_thresholds[37] = 300;
    block->rank_grade_thresholds[38] = 600;
    block->rank_grade_thresholds[39] = 1200;
    block->rank_grade_thresholds[40] = 300;
    block->rank_grade_thresholds[41] = 450;
    block->rank_grade_thresholds[42] = 900;
    block->rank_grade_thresholds[43] = 1800;
    block->rank_grade_thresholds[44] = 400;
    block->rank_grade_thresholds[45] = 600;
    block->rank_grade_thresholds[46] = 1200;
    block->rank_grade_thresholds[47] = 2400;
    block->rank_grade_thresholds[48] = 500;
    block->rank_grade_thresholds[49] = 1000;
    block->rank_grade_thresholds[50] = 2000;
    block->rank_grade_thresholds[51] = 4000;
    block->rank_grade_thresholds[52] = 600;
    block->rank_grade_thresholds[53] = 1200;
    block->rank_grade_thresholds[54] = 2500;
    block->rank_grade_thresholds[55] = 5000;
    block->rank_experience_thresholds[0] = 0;
    block->rank_experience_thresholds[1] = 0;
    block->rank_experience_thresholds[2] = 0;
    block->rank_experience_thresholds[3] = 0;
    block->rank_experience_thresholds[4] = 0;
    block->rank_experience_thresholds[5] = 0;
    block->rank_experience_thresholds[6] = 0;
    block->rank_experience_thresholds[7] = 10;
    block->rank_experience_thresholds[8] = 20;
    block->rank_experience_thresholds[9] = 30;
    block->rank_experience_thresholds[10] = 35;
    block->rank_experience_thresholds[11] = 40;
    block->rank_experience_thresholds[12] = 45;
    block->rank_experience_thresholds[13] = 50;
    block->disable_game = 0;
    block->custom_games_disabled = 0;
    block->maximum_players_per_peer = 4;
    block->alpha_locked_down = 1;
    block->crash_reporting_enabled = 0;
    block->crash_minidump_mode = 2;
    block->online_lsp.port = 1001;
    block->online_lsp.port_count = 2;
    block->online_lsp.minimum_score = 3;
    block->online_lsp.search_timeout = 300000;
    block->online_lsp.idle_timeout = 60000;
    {
        s_network_configuration_map_entry *entry = block->map_table;
        for (long i = 0; i < 32; i++, entry++)
        {
            entry->map_id = -1;
            entry->map_status = 0;
        }
    }
    block->map_table[0].map_id = 380;
    block->map_table[1].map_id = 300;
    block->map_table[2].map_id = 390;
    block->map_table[3].map_id = 310;
    block->map_table[4].map_id = 320;
    block->map_table[5].map_id = 330;
    block->map_table[6].map_id = 340;
    block->map_table[7].map_id = 350;
    block->map_table[8].map_id = 400;
    block->map_table[9].map_id = 360;
    block->map_table[10].map_id = 30;
    block->map_table[11].map_id = 301;
    block->map_table[12].map_id = 302;
    block->map_table[13].map_id = 303;
    block->allow_mismatched_peer_properties = 0;
    block->require_matching_peer_properties = 0;
    block->disallow_cross_language_coop = 0;
}
