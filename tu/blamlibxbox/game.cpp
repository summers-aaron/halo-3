// blamlibxbox/game.obj: from the records 0x807a0 0x8b4f8 0x8bfe0 0x905c8 0x98818 0xa3918 0xa39f0 0xa7860 0xa7940 0xaaca8 0xc1510 0xd5b10 0x1194d8 0x119b98 0x1b7088 0x1b7098 0x1b7160 0x1b7258 0x1b7318 0x1b7440 0x1b7538 0x1b76a8 0x1b77c8 0x1b7828 0x1b7880 0x1b7e40 0x1b8100 0x1b84f8 0x1b8578 0x1b8600 0x1b8708 0x1b8750 0x1b8a00 0x1b8b08 0x1b8c30 0x1b8ca0 0x1b8cf8 0x1b8d38 0x1b9010 0x1b9100 0x1b9180 0x1b9248 0x1b9358 0x1b9548 0x1b98b8 0x1b99d8 0x1b9b38 0x1b9be8 0x1b9c70 0x1b9d88 0x1b9e28 0x1b9e40 0x1e1b58 0x1e2300 0x31d7f8
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x11df85c g_director_camera_scripted
// @address 0x94e3b0 g_saved_film_timer_globals
#include "cache/restricted_memory.h"
#include "rasterizer/rasterizer_hue_saturation.h"
#include "simulation/simulation_watcher_types.h"
#include "render/water_interaction_tls.h"
// @flags /TP /O2
#include "objects/objects_tracked_object_types.h"
#include "game/campaign_metagame_globals.h"
#include "objects/object_reference_types.h"
#include "interface/user_interface_clock.h"
#include "objects/ragdoll_source_types.h"
#include "game/game_simulation_types.h"
#include "game/game_achievements_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "interface/c_player_profile.h"
#include <stdlib.h>
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/game_variant.h"
#include "game/map_variant.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "game/game_runtime_globals.h"
#include "game/game_globals.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/game_allegiance.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "physics/havok_proxies.h"
#include "simulation/simulation.h"
#include "sound/game_sound.h"
#include "ai/ai.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "camera/camera_scripting.h"
#include "cutscene/cinematics.h"
#include "cutscene/recorded_animations.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_update_types.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "scenario/scenario_tag.h"
#include "networking/network_configuration_types.h"
#include "effects/effects.h"
#include "effects/player_effects.h"
#include "game/game_engine_variant.h"
#include "game/game_results.h"
#include "hs/hs_runtime.h"
#include "interface/first_person_weapons.h"
#include "main/game_preferences.h"
#include "main/main.h"
#include "networking/transport/transport_security.h"
#include "objects/lights.h"
#include "objects/object_placement.h"
#include "physics/havok.h"
#include "rasterizer/rasterizer_main.h"
#include "saved_games/game_state.h"
#include "scenario/scenario_pvs.h"
#include "sound/sound_manager.h"
#include "game/game_time.h"
#include "interface/chud/chud_messaging.h"
#include "main/levels.h"
#include "physics/breakable_surfaces.h"
#include "saved_games/saved_film.h"
#include "scenario/scenario_map_variant.h"
#include "structures/structure_bsp_definitions.h"
#include "game/campaign_metagame.h"
#include "game/game_save.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "render/render_water.h"
#include "bink/bink_playback.h"
#include "camera/director.h"
#include "camera/observer.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/runtime_state.h"
#include "effects/decals_globals.h"
#include "effects/decals.h"
#include "effects/decal_system_types.h"
#include "effects/decal_types.h"
#include "effects/light_volume_gpu.h"
#include "game/game_grief.h"
#include "game/player_control.h"
#include "game/player_rumble.h"
#include "game/player_training.h"
#include "hs/hs.h"
#include "input/input_abstraction.h"
#include "interface/closed_caption.h"
#include "interface/interface.h"
#include "interface/overhead_map.h"
#include "interface/user_interface.h"
#include "interface/watch_window.h"
#include "math/random_math.h"
#include "memory/static_arrays.h"
#include "networking/online/online_achievements.h"
#include "networking/tools/network_webstats.h"
#include "networking/transport/transport.h"
#include "objects/object_broadphase.h"
#include "objects/object_early_movers.h"
#include "objects/object_globals.h"
#include "objects/object_header.h"
#include "objects/object_scripting.h"
#include "physics/physics_constants.h"
#include "physics/point_physics.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "render/camera_fx_settings.h"
#include "render/render.h"
#include "render/render_objects.h"
#include "render/render_texture_camera.h"
#include "render/structure_renderer.h"
#include "render/views/render_view.h"
#include "render/water_renderer.h"
#include "saved_games/autosave_queue.h"
#include "saved_games/game_state_procs.h"
#include "saved_games/saved_game_files.h"
#include "scenario/scenario_interpolators.h"
#include "scenario/scenario_kill_trigger_volumes_state.h"
#include "scenario/scenario_runtime_symbols.h"
#include "scenario/scenario_sky.h"
#include "scenario/scenario_soft_ceilings_allocator.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "sound/game_sound_deterministic.h"
#include "sound/game_sound_player_effects.h"
#include "sound/sound_classes.h"
#include "structures/structures.h"
#include "tag_files/files.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_portal_activation.h"
#include "cache/restricted_memory_layout.h"
#include "game/player_in_game_iterator.h"
#include "interface/controller_types.h"
#include "simulation/simulation_world_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "input/input.h"
#include "simulation/simulation_queue.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "game/date_and_time_types.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "items/equipment_globals.h"
#include "networking/network_configuration.h"
#include "scenario/scenario_pvs_types.h"
#include "game/game_achievements.h"
#include "networking/logic/network_session_interface.h"
#include "game/game_globals_allocator.h"
#include "memory/bit_vectors.h"
#include "text/unicode.h"
#include "cubemaps/cubemap_sample.h"
struct s_scenario_light;
struct simulation_update;
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x98818 ?game_campaign_is_multiplayer@@YA_NXZ
// @address 0xaaca8 ?game_difficulty_level_get@@YAFXZ
// @address 0xc1510 ?game_test_cluster_activation@@YA_NPBUs_cluster_reference@@@Z
// @address 0xd5b10 ?game_clusters_or@@YAXPBUs_game_cluster_bit_vectors@@0PAU1@@Z
// @address 0x1b7088 ?__tls_set_g_game_globals_allocator@@YAXPAX@Z

struct s_map_variant_validation;
class c_bitstream;

#include "structures/cluster_bit_vectors.h"

#include "game/game_grief_types.h"
#include "interface/user_interface_controller_globals.h"

#include "game/game_system_types.h"

enum e_campaign_game_mode
{
    _campaign_game_mode_solo,
    _campaign_game_mode_cooperative,
    k_number_of_campaign_game_modes
};

enum e_game_time_holiday
{
    _game_time_holiday_none = 0,
    _game_time_holiday_new_years_day,
    _game_time_holiday_earth_day,
    _game_time_holiday_cinco_de_mayo,
    _game_time_holiday_independence_day,
    _game_time_holiday_july_seventh,
    _game_time_holiday_halloween,
    _game_time_holiday_veterans_day,
    _game_time_holiday_december_twelfth,
    _game_time_holiday_christmas,
};

#include "game/game_simulation_accessors.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session.h"
#include "game/game_runtime_dependencies.h"
extern "C" {
#include "cseries/crt_xdk.h"
}
#include "interface/user_interface_controller.h"
#include "game/game_runtime_globals.h"
extern "C" {
#include <float.h>
}
#include "scenario/scenario.h"
#include "main/main_game.h"
#include "simulation/simulation_globals.h"
#include "memory/bitstream.h"
#include "game/game_campaign_armaments_encoding.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "input/input_globals.h"
#include <stdio.h>
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_definitions.h"
#include "game/player_mapping_types.h"
#include "camera/observer_globals_types.h"
#include "camera/saved_film_director_storage.h"
#include "game/players_pvs.h"
#include "structures/structure_bsp_tag.h"
#include "main/main_flag_state.h"
#include "cseries/system_time.h"
#include "saved_games/saved_film_timer_globals.h"
extern "C" {
#include <time.h>
}
#include "scenario/scenario_light_types.h"
#include "simulation/simulation_update_metadata_types.h"
#include "rasterizer/rasterizer_implicit_geometry_types.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "game/game_allegiance_types.h"
#include "render/depth_of_field_types.h"
#include "render/depth_of_field_globals.h"
#include "simulation/simulation_game_update.h"

#include "interface/controller_inline.h"
#include "memory/datum_index.h"

// @address 0x11df680 game_globals
__declspec(thread) game_globals_storage *game_globals;

bool game_is_cooperative(void)
{
    game_globals_storage *globals = game_globals;
    bool active = globals && (globals->initializing || globals->map_active);
    return active && globals->options.game_playback != 0;
}

// @address 0x8bfe0 game_is_map_active
extern "C" bool game_is_map_active(void)
{
    game_globals_storage *globals = game_globals;
    return globals && (globals->initializing || globals->map_active);
}

namespace {
static __forceinline bool game_is_predicted_inline()
{
    bool result = false;
    e_game_simulation_type simulation_type = game_simulation_get();
    if (simulation_type == _game_simulation_distributed_client)
        result = true;
    return result;
}
}

bool game_is_distributed(void)
{
    bool result = false;
    e_game_simulation_type simulation_type = game_simulation_get();
    if (simulation_type >= _game_simulation_distributed_client && simulation_type <= _game_simulation_distributed_server)
        result = true;
    return result;
}

bool game_is_predicted(void)
{
    bool result = false;
    e_game_simulation_type simulation_type = game_simulation_get();
    if (simulation_type == _game_simulation_distributed_client)
        result = true;
    return result;
}

bool game_is_authoritative(void)
{
    return !game_is_predicted_inline();
}

bool game_campaign_is_multiplayer(void)
{
    game_globals_storage *globals = game_globals;
    bool result = false;
    if (globals->options.game_mode == 1)
    {
        long player_count = players_get_active_and_in_game_count(true);
        result = 1 < player_count;
    }
    return result;
}

short game_difficulty_level_get(void)
{
    game_globals_storage *globals = game_globals;
    long result;
    if (globals->options.game_mode == 1)
        result = (unsigned short)globals->options.campaign_difficulty;
    else
        result = 1;
    return result;
}

bool game_test_cluster_activation(const s_cluster_reference *cluster_reference)
{
  bool result = false;
  if (cluster_reference->bsp_index != -1) {
    long group = cluster_reference->bsp_index;
    long bit = cluster_reference->cluster_index;
    result = bit_vector_test(game_globals->cluster_activation[group], bit);
  }
  return result;
}

void game_clusters_or(const s_game_cluster_bit_vectors *a, const s_game_cluster_bit_vectors *b, s_game_cluster_bit_vectors *out)
{
    for (long i = 0; i < 16; i++)
        bit_vector_or(256, b->bit_vectors[i].get_bits_direct(), a->bit_vectors[i].get_bits_direct(),
            out->bit_vectors[i].get_writeable_bits_direct());
}

// @address 0x8b4f8 game_is_scoreboard_blocked
extern "C" unsigned char game_is_scoreboard_blocked(int *out_reason)
{
    bool result = false;
    bool can_run = false;
    game_globals_storage *state = game_globals;
    if (state != 0 && state->in_progress != 0 && state->initializing == 0 && state->map_active != 0) {
        can_run = true;
    }
    can_run = can_run && state->options.game_mode != 3;
    if (state->options.game_mode == 1) {
        can_run = can_run && network_squad_session_get_peer_count() > 1;
    }
    if (can_run) {
        int reason = 0;
        bool mode_hit = false;
        void *session;
        if (network_life_cycle_in_session(&session)) {
            int mode = static_cast<c_network_session *>(session)->m_local_state;
            if (mode == 8 || mode == 10) mode_hit = true;
        }
        if (mode_hit) {
            reason = 3;
        } else {
            mode_hit = false;
            if (network_life_cycle_in_session(&session) && static_cast<c_network_session *>(session)->m_local_state == 9) mode_hit = true;
            if (mode_hit) {
                reason = 4;
            } else if (simulation_is_pending_start()) {
                reason = 1;
            } else {
                bool wanted_mode = false;
                if ((*network_life_cycle_globals_get()).initialized != 0) {
                    int mode = (*network_life_cycle_globals_get()).manager.m_current_state;
                    if (mode == 4 || mode == 16) wanted_mode = true;
                }
                if (wanted_mode) {
                    reason = 2;
                } else if (!::simulation_in_progress()) {
                    reason = 5;
                }
            }
        }
        if (out_reason != 0) *out_reason = reason;
        if (state->game_finished != 0) {
            if (g_user_interface_globals.scoreboard_blocked_until_tick == 0x7fffffff) {
#if defined(_XBOX) && !defined(__clang__)
                g_user_interface_globals.scoreboard_blocked_until_tick = (int)((float)(short)game_time_globals->ticks_per_second
                    + game_scoreboard_real_constants()[0]) + game_time_globals->time;
#else
                g_user_interface_globals.scoreboard_blocked_until_tick = (int)((float)(short)game_time_globals->ticks_per_second
                    + 0.5f) + game_time_globals->time;
#endif
            }
        } else {
            g_user_interface_globals.scoreboard_blocked_until_tick = 0x7fffffff;
        }
        result = game_time_globals->time >= g_user_interface_globals.scoreboard_blocked_until_tick || reason != 0;
    }
    return result;
}

// @address 0x19b440 ?game_state_prepare_for_revert@@YAXXZ
static __forceinline long game_seconds_to_ticks_round_inline(float seconds)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + 0.5f);
}
// @address 0x1194d8 ?game_lost@@YAX_N@Z
void game_lost(bool lost)
{
    if (lost && !game_globals->lost)
    {
        game_globals->lost = true;
        game_globals->revert_ticks = game_seconds_to_ticks_round_inline(6.0f);
        game_state_prepare_for_revert();
    }
    else if (!lost && game_globals->lost)
    {
        game_globals->lost = false;
    }
}

static __forceinline long game_decrement_grief_count(long count)
{
    return count - 1 > 0 ? count - 1 : 0;
}

// @address 0x119b98 game_update_grief_decay
extern "C" void game_update_grief_decay(void)
{
    if (!game_is_cooperative())
    {
        long now = GetTickCount();
        long betrayal_count = global_preferences_get_betrayal_count();
        long quit_count = global_preferences_get_ejection_count();
        if (betrayal_count > 0)
        {
            long elapsed = now - g_game_grief_globals.betrayal_decay_time;
            if (elapsed >= 0 && (unsigned long)elapsed > g_network_configuration.betrayal_decay_interval)
            {
                g_game_grief_globals.betrayal_decay_time = now;
                betrayal_count = game_decrement_grief_count(betrayal_count);
                global_preferences_set_betrayal_count(betrayal_count);
                if ((float)betrayal_count < g_network_configuration.betrayal_threshold)
                {
                    for (e_controller_index controller = first_controller(); controller != _controller_none; controller = next_controller(controller))
                    {
                        g_user_interface_controller_globals.controllers[controller].griefer = false;
                        user_interface_controller_update_network_properties(controller);
                    }
                }
            }
        }
        if (quit_count > 0 &&
            (unsigned long)(now - g_game_grief_globals.quit_decay_time) >
                g_network_configuration.quit_decay_interval)
        {
            quit_count = game_decrement_grief_count(quit_count);
            g_game_grief_globals.quit_decay_time = now;
            global_preferences_set_ejection_count(quit_count);
        }
        if (g_game_grief_globals.reprieve_time > 0 && now - g_game_grief_globals.reprieve_time >= 10000)
            g_game_grief_globals.reprieve_time = 0;
        g_game_grief_globals.last_update_time = now;
    }
}

void __tls_set_g_game_globals_allocator(void *value)
{
    game_globals = (game_globals_storage *)value;
}

// @address 0x5bec98 ?determinism_debug_manager_initialize@@YAXXZ
void determinism_debug_manager_initialize()
{
}

// @address 0x5bec98 ?determinism_debug_manager_dispose@@YAXXZ
void determinism_debug_manager_dispose()
{
}

// @address 0x5bec98 ?determinism_debug_manager_initialize_for_new_map@@YAXXZ
void determinism_debug_manager_initialize_for_new_map()
{
}

// @address 0x5bec98 ?determinism_debug_manager_dispose_from_old_map@@YAXXZ
void determinism_debug_manager_dispose_from_old_map()
{
}

// @address 0x5bec98 ?telnet_console_initialize@@YAXXZ
void telnet_console_initialize()
{
}

// @address 0x5bec98 ?telnet_console_dispose@@YAXXZ
void telnet_console_dispose()
{
}

// @address 0x5bec98 ?collision_log_initialize@@YAXXZ
void collision_log_initialize()
{
}

// @address 0x5bec98 ?collision_log_dispose@@YAXXZ
void collision_log_dispose()
{
}

// @address 0x5bec98 ?profiler_initialize@@YAXXZ
void profiler_initialize()
{
}

// @address 0x5bec98 ?profiler_dispose@@YAXXZ
void profiler_dispose()
{
}

// @address 0x5bec98 ?profiler_initialize_for_new_map@@YAXXZ
void profiler_initialize_for_new_map()
{
}

// @address 0x5bec98 ?profiler_dispose_from_old_map@@YAXXZ
void profiler_dispose_from_old_map()
{
}

// @address 0x5bec98 ?collision_debug_initialize@@YAXXZ
void collision_debug_initialize()
{
}

// @address 0x5bec98 ?collision_debug_dispose@@YAXXZ
void collision_debug_dispose()
{
}

// @address 0x5bec98 ?cheats_initialize@@YAXXZ
void cheats_initialize()
{
}

// @address 0x5bec98 ?cheats_dispose@@YAXXZ
void cheats_dispose()
{
}

// @address 0x5bec98 ?cheats_initialize_for_new_map@@YAXXZ
void cheats_initialize_for_new_map()
{
}

// @address 0x5bec98 ?cheats_dispose_from_old_map@@YAXXZ
void cheats_dispose_from_old_map()
{
}

// @address 0x5bec98 ?console_initialize@@YAXXZ
void console_initialize()
{
}

// @address 0x5bec98 ?console_dispose@@YAXXZ
void console_dispose()
{
}

// @address 0x5bec98 ?data_mine_initialize@@YAXXZ
void data_mine_initialize()
{
}

// @address 0x5bec98 ?data_mine_dispose@@YAXXZ
void data_mine_dispose()
{
}

// @address 0x5bec98 ?debug_menu_initialize@@YAXXZ
void debug_menu_initialize()
{
}

// @address 0x5bec98 ?debug_menu_dispose@@YAXXZ
void debug_menu_dispose()
{
}

// @address 0x5bec98 ?debug_menu_initialize_for_new_map@@YAXXZ
void debug_menu_initialize_for_new_map()
{
}

// @address 0x5bec98 ?debug_menu_dispose_from_old_map@@YAXXZ
void debug_menu_dispose_from_old_map()
{
}

// @address 0x5bec98 ?game_engine_dispose@@YAXXZ
void game_engine_dispose()
{
}

// @address 0x5bec98 ?remote_command_initialize@@YAXXZ
void remote_command_initialize()
{
}

// @address 0x5bec98 ?remote_command_dispose@@YAXXZ
void remote_command_dispose()
{
}

// @address 0x5bec98 ?xbox_connection_initialize@@YAXXZ
void xbox_connection_initialize()
{
}

// @address 0x5bec98 ?xbox_connection_dispose@@YAXXZ
void xbox_connection_dispose()
{
}

// Shared empty procs for the table slots whose system has nothing to do; one per field signature.
// @address 0x5bec98 ?game_system_empty_proc@@YAXXZ
void game_system_empty_proc()
{
}

// @address 0x5bec98 ?game_system_empty_proc@@YAXKK@Z
void game_system_empty_proc(unsigned long, unsigned long)
{
}

// @address 0x5bec98 ?game_system_empty_proc@@YAXK@Z
void game_system_empty_proc(unsigned long)
{
}

// @address 0x5bec98 ?game_system_empty_proc@@YAXPBUs_game_non_bsp_zone_set@@0PAVc_scenario_resource_registry@@@Z
void game_system_empty_proc(const s_game_non_bsp_zone_set *, const s_game_non_bsp_zone_set *, c_scenario_resource_registry *)
{
}

// @address 0x5bec98 ?game_system_empty_proc@@YAXPBUs_game_non_bsp_zone_set@@@Z
void game_system_empty_proc(const s_game_non_bsp_zone_set *)
{
}

// @address 0x397f0 g_game_systems
// @rdata 0x820397f0..0x8203a930
const s_game_system g_game_systems[92] =
{
    { determinism_debug_manager_initialize, determinism_debug_manager_dispose, determinism_debug_manager_initialize_for_new_map,
        determinism_debug_manager_dispose_from_old_map },
    { game_state_procs_initialize, game_system_empty_proc },
    { game_state_procs_set_autosave_listener, game_state_procs_reset_autosave_listener },
    { transport_initialize, transport_dispose },
    { runtime_state_initialize, runtime_state_dispose, runtime_state_initialize_for_new_map, runtime_state_dispose_from_old_map },
    { c_structure_renderer::initialize, c_structure_renderer::dispose, c_structure_renderer::initialize_for_new_map,
        c_structure_renderer::dispose_from_old_map, NULL, c_structure_renderer::initialize_for_new_structure_bsp,
        c_structure_renderer::dispose_from_old_structure_bsp },
    { data_mine_initialize, data_mine_dispose },
    { overlapped_initialize, overlapped_dispose },
    { random_math_initialize, random_math_dispose },
    { network_initialize, network_dispose },
    { network_webstats_initialize, network_webstats_dispose },
    { xbox_connection_initialize, xbox_connection_dispose },
    { remote_command_initialize, remote_command_dispose },
    { telnet_console_initialize, telnet_console_dispose },
    { console_initialize, console_dispose },
    { input_abstraction_initialize, input_abstraction_dispose, input_abstraction_initialize_for_new_map, game_system_empty_proc },
    { collision_log_initialize, collision_log_dispose },
    { levels_initialize, levels_dispose, levels_initialize_for_new_map, levels_dispose_from_old_map },
    { visibility_collection_initialize, visibility_collection_dispose },
    { game_grief_initialize, game_grief_dispose, game_grief_reset_betrayals, game_system_empty_proc },
    { achievements_initialize, achievements_dispose },
    { game_state_initialize, game_state_dispose, game_state_initialize_for_new_map, game_state_dispose_from_old_map, NULL,
        game_state_initialize_for_new_structure_bsp, game_state_dispose_from_old_structure_bsp, NULL, NULL, NULL,
        game_state_initialize_for_new_designer_zone_set, game_state_dispose_from_old_designer_zone_set },
    { game_time_initialize, game_time_dispose, game_time_initialize_for_new_map, game_time_dispose_from_old_map },
    { profiler_initialize, profiler_dispose, profiler_initialize_for_new_map, profiler_dispose_from_old_map },
    { game_allegiance_initialize, game_allegiance_dispose, game_allegiance_initialize_for_new_map, game_allegiance_dispose_from_old_map },
    { players_initialize, players_dispose, players_initialize_for_new_map, players_dispose_from_old_map, NULL,
        players_initialize_for_new_structure_bsp, players_dispose_from_old_structure_bsp },
    { player_control_initialize, player_control_dispose, player_control_initialize_for_new_map, player_control_dispose_from_old_map },
    { player_training_initialize, player_training_dispose, player_training_initialize_for_new_map, player_training_dispose_from_old_map },
    { game_engine_initialize, game_engine_dispose, game_engine_initialize_for_new_map, game_engine_dispose_from_old_map },
    { simulation_initialize, simulation_dispose, simulation_initialize_for_new_map, simulation_dispose_from_old_map },
    { scenario_initialize, scenario_dispose, scenario_initialize_for_new_map, scenario_dispose_from_old_map, NULL,
        scenario_initialize_for_new_structure_bsp, scenario_dispose_from_old_structure_bsp },
    { physics_constants_initialize, physics_constants_dispose, physics_constants_initialize_for_new_map, physics_constants_dispose_from_old_map },
    { collision_debug_initialize, collision_debug_dispose },
    { objects_initialize, objects_dispose, objects_initialize_for_new_map, objects_dispose_from_old_map, NULL,
        objects_initialize_for_new_structure_bsp, objects_dispose_from_old_structure_bsp, NULL, objects_activation },
    { object_early_movers_initialize, object_early_movers_dispose, object_early_movers_initialize_for_new_map, object_early_movers_dispose_from_old_map },
    { object_scripting_initialize, object_scripting_dispose, object_scripting_initialize_for_new_map, object_scripting_dispose_from_old_map },
    { water_interaction_ripples_initialize, water_interaction_ripples_dispose, water_interaction_ripples_clear, game_system_empty_proc },
    { object_activation_regions_initialize, object_activation_regions_dispose, object_activation_regions_delete_all, object_activation_regions_invalidate },
    { scenario_kill_trigger_volumes_initialize, scenario_kill_trigger_volumes_dispose, scenario_kill_trigger_volumes_initialize_for_new_map,
        scenario_kill_trigger_volumes_dispose_from_old_map },
    { scenario_sky_objects_initialize, scenario_sky_objects_dispose, scenario_sky_objects_initialize_for_new_map, scenario_sky_objects_dispose_from_old_map },
    { scenario_soft_ceilings_initialize, scenario_soft_ceilings_dispose, scenario_soft_ceilings_initialize_for_new_map,
        scenario_soft_ceilings_dispose_from_old_map },
    { campaign_metagame_initialize, campaign_metagame_dispose, campaign_metagame_initialize_for_new_map, campaign_metagame_dispose_from_old_map },
    { file_initialize_save_directory, game_system_empty_proc, saved_game_cleanup_storage, game_system_empty_proc },
    { saved_game_files_initialize, saved_game_files_dispose, saved_game_files_reset, main_reset_saved_film_state },
    { c_rasterizer::initialize, rasterizer_main_dispose_shared_shaders, c_rasterizer::initialize_for_new_map, c_rasterizer::dispose_from_old_map,
        NULL, c_rasterizer::initialize_for_new_structure_bsp, c_rasterizer::dispose_from_old_structure_bsp },
    { render_initialize, render_dispose, render_initialize_for_new_map, render_dispose_from_old_map, NULL, render_initialize_for_new_structure_bsp,
        render_dispose_from_old_structure_bsp },
    { structures_initialize, structures_dispose, structures_initialize_for_new_map, structures_dispose_from_old_map, NULL,
        structures_initialize_for_new_structure_bsp, structures_dispose_from_old_structure_bsp },
    { breakable_surfaces_initialize, breakable_surfaces_dispose, breakable_surfaces_initialize_for_new_map, breakable_surfaces_dispose_from_old_map,
        NULL, breakable_surfaces_initialize_for_new_structure_bsp, breakable_surfaces_dispose_from_old_structure_bsp },
    { director_initialize, director_dispose, director_initialize_for_new_map, director_dispose_from_old_map },
    { observer_initialize, observer_dispose, observer_initialize_for_new_map, observer_dispose_from_old_map, NULL,
        observer_initialize_for_new_structure_bsp, observer_dispose_from_old_structure_bsp },
    { s_depth_of_field::initialize, s_depth_of_field::dispose, s_depth_of_field::initialize_for_new_map, s_depth_of_field::dispose_from_old_map },
    { c_water_renderer::initialize, c_water_renderer::dispose, c_water_renderer::initialize_for_new_map, c_water_renderer::dispose_from_old_map },
    { render_texture_camera_initialize, render_texture_camera_dispose, render_texture_camera_initialize_for_new_map,
        render_texture_camera_dispose_from_old_map },
    { camera_fx_allocate_globals, game_system_empty_proc, camera_fx_initialize_globals, game_system_empty_proc },
    { restricted_region_reserve_region2_storage, game_system_empty_proc, render_view_reset_camera_fx, game_system_empty_proc },
    { c_decal_system::initialize, c_decal::dispose_from_game, c_decal_system::initialize_for_new_map, c_decal_system::dispose_from_old_map,
        game_system_empty_proc, c_decal_system::initialize_for_new_structure_bsp, c_decal_system::dispose_from_old_structure_bsp,
        c_decal_system::change_pvs, NULL, decal_system_remove_invalid_definitions, game_system_empty_proc, game_system_empty_proc },
    { effects_initialize, effects_dispose, effects_initialize_for_new_map, effects_dispose_from_old_map, game_system_empty_proc,
        effects_initialize_for_new_structure_bsp, effects_process_pending_effects, NULL, NULL, effects_delete_unused_for_new_map,
        game_system_empty_proc, game_system_empty_proc },
    { point_physics_initialize, point_physics_dispose, point_physics_initialize_for_new_map, point_physics_dispose_from_old_map },
    { c_atmosphere_fog_interface::initialize, c_atmosphere_fog_interface::dispose, render_objects_reset_cached_indices,
        c_atmosphere_fog_interface::dispose_from_old_map, NULL, c_atmosphere_fog_interface::initialize_for_new_structure_bsp,
        c_atmosphere_fog_interface::dispose_from_old_structure_bsp, c_atmosphere_fog_interface::change_pvs },
    { light_volume_row_gpu_initialize, light_volume_row_gpu_dispose, light_volume_row_gpu_reset, light_volume_row_gpu_disable },
    { sound_classes_initialize, sound_classes_dispose, sound_classes_initialize_for_new_map, sound_classes_dispose_from_old_map },
    { sound_initialize, sound_dispose, sound_initialize_for_new_map, sound_dispose_from_old_map, NULL, sound_initialize_for_new_structure_bsp,
        sound_dispose_from_old_structure_bsp },
    { game_sound_deterministic_initialize, game_sound_deterministic_dispose, game_sound_deterministic_initialize_for_new_map,
        game_sound_deterministic_dispose_from_old_map },
    { game_sound_initialize, game_sound_dispose, game_sound_initialize_for_new_map, game_sound_dispose_from_old_map, NULL,
        game_sound_initialize_for_new_structure_bsp, game_sound_dispose_from_old_structure_bsp },
    { game_sound_player_effects_initialize, game_sound_player_effects_dispose, game_sound_player_effects_initialize_for_new_map,
        game_sound_player_effects_dispose_from_old_map },
    { rumble_initialize, rumble_dispose, rumble_initialize_for_new_map, rumble_dispose_from_old_map },
    { player_effect_initialize, player_effect_dispose, player_effect_initialize_for_new_map, player_effect_dispose_from_old_map },
    { user_interface_initialize, user_interface_dispose, user_interface_initialize_for_new_map, user_interface_dispose_from_old_map },
    { interface_initialize, interface_dispose, interface_initialize_for_new_map, interface_dispose_from_old_map },
    { chud_initialize, chud_dispose, chud_initialize_for_new_map, chud_dispose_from_old_map },
    { overhead_map_initialize, overhead_map_dispose, overhead_map_initialize_for_new_map, overhead_map_dispose_from_old_map },
    { cheats_initialize, cheats_dispose, cheats_initialize_for_new_map, cheats_dispose_from_old_map },
    { cinematic_initialize, cinematic_dispose, cinematic_initialize_for_new_map, cinematic_dispose_from_old_map, game_system_empty_proc,
        game_system_empty_proc, game_system_empty_proc, NULL, NULL, cinematic_update_activation, game_system_empty_proc, game_system_empty_proc },
    { closed_caption_initialize, closed_caption_dispose, closed_caption_initialize_for_new_map, closed_caption_dispose_from_old_map },
    { game_system_empty_proc, game_system_empty_proc, game_system_empty_proc, saved_film_reset_active_playback },
    { user_interface_controller_clear_signin_cache, game_system_empty_proc },
    { hs_initialize, hs_dispose, hs_initialize_for_new_map, hs_dispose_from_old_map },
    { recorded_animations_initialize, recorded_animations_dispose, recorded_animations_initialize_for_new_map, recorded_animations_dispose_from_old_map },
    { debug_menu_initialize, debug_menu_dispose, debug_menu_initialize_for_new_map, debug_menu_dispose_from_old_map },
    { object_placement_initialize, object_placement_dispose, object_placement_initialize_for_new_map, object_placement_dispose_from_old_map,
        object_placement_prepare_for_new_zone_set, object_placement_initialize_for_new_structure_bsp, object_placement_dispose_from_old_structure_bsp,
        NULL, NULL, object_placement_prepare_for_designer_zone_set_switch },
    { havok_initialize, havok_dispose, havok_initialize_for_new_map, havok_dispose_from_old_map, NULL, havok_handle_new_structure_bsp,
        havok_handle_old_structure_bsp },
    { object_broadphase_initialize, object_broadphase_dispose, object_broadphase_initialize_for_new_map, object_broadphase_dispose_from_old_map, NULL,
        object_broadphase_initialize_for_new_structure_bsp, object_broadphase_dispose_from_old_structure_bsp },
    { havok_proxies_initialize, havok_proxies_dispose, havok_proxies_initialize_for_new_map, havok_proxies_dispose_from_old_map },
    { player_positions_initialize, player_positions_dispose, player_positions_initialize_for_new_map, player_positions_dispose_from_old_map, NULL,
        player_positions_initialize_for_new_structure_bsp, player_positions_dispose_from_old_structure_bsp },
    { ai_initialize, ai_dispose, ai_initialize_for_new_map, ai_dispose_from_old_map, NULL, ai_initialize_for_new_structure_bsp,
        ai_dispose_from_old_structure_bsp },
    { portal_activation_initialize, portal_activation_dispose, portal_activation_initialize_for_new_map, portal_activation_dispose_from_old_map, NULL,
        portal_activation_initialize_for_new_structure_bsp, portal_activation_dispose_from_old_structure_bsp },
    { scenario_interpolators_initialize, scenario_interpolators_dispose, scenario_interpolators_initialize_for_new_map,
        scenario_interpolators_dispose_from_old_map },
    { game_save_initialize, game_save_dispose, game_save_initialize_for_new_map, game_save_dispose_from_old_map },
    { watch_window_initialize, watch_window_dispose },
    { bink_playback_initialize, bink_playback_dispose, bink_playback_clear_playing, bink_playback_dispose_from_old_map,
        bink_playback_handle_after_load, game_system_empty_proc, game_system_empty_proc, NULL, NULL, game_system_empty_proc, game_system_empty_proc,
        game_system_empty_proc },
    { game_system_empty_proc, game_system_empty_proc, game_system_empty_proc, game_system_empty_proc, NULL, game_system_empty_proc, game_system_empty_proc },
    { game_system_empty_proc, game_system_empty_proc, game_system_empty_proc, game_system_empty_proc, NULL, rasterizer_xenon_memory_reset,
        game_system_empty_proc },
};
// @rdata-end

// 0x1b7098
void game_initialize(void)
{
    game_globals = (game_globals_storage *)g_game_globals_allocator.reserve_memory("game globals", 0,
        sizeof(*game_globals), 0);
    memset(game_globals, 0, sizeof(*game_globals));
    game_globals->active_structure_bsp_mask = 0;
    if (g_shell_floating_point_exceptions_enabled)
        _controlfp(0x100001f, 0xfffff);
    unsigned int remaining = 92;
    const s_game_system *game_system = g_game_systems;
    do
    {
        game_system->initialize_proc();
        remaining--;
        game_system++;
    } while (remaining != 0);
}

// @address 0x8a1f02 ?g_default_playtest_mode@@3_NA
bool g_default_playtest_mode;

static __forceinline int game_progression_level_out_of_range(long level)
{
    if (level < 0)
        return 1;
    if (level >= 12)
        return 1;
    return 0;
}

extern "C" {
// @address 0x74b578 k_game_simulation_names
const char *k_game_simulation_names[] = {
    "none",
    "local",
    "sync-client",
    "sync-server",
    "dist-client",
    "dist-server",
};

// @address 0x74b590 k_game_playback_names
const char *k_game_playback_names[] = {
    "none",
    "local",
    "network-server",
    "network-client",
};

}

static __forceinline bool game_is_multiplayer_inline()
{
    return game_globals->options.game_mode == 2;
}
static __forceinline void game_set_random_seed_inline(unsigned long seed)
{
    *tls_random_seed() = seed;
}

// @address 0x1b7258 ?game_dispose_from_old_map@@YAXXZ
void game_dispose_from_old_map(void)
{
    game_globals_storage *globals = game_globals;
    globals->active_designer_zone_mask = 0;
    globals->active_cinematic_zone_mask = 0;
    int wait_flags = ((int)::_internal_halt_render_thread_and_lock_resources("..\\game\\game.cpp", 745));
    game_globals->in_progress = 0;
    for (long system_index = 91; system_index >= 0; system_index--)
    {
        if (g_game_systems[system_index].dispose_from_old_map_proc)
            g_game_systems[system_index].dispose_from_old_map_proc();
    }
    main_set_status_value("game_instance", 0);
    main_set_status_value("game_simulation", 0);
    main_set_status_value("game_playback", 0);
    game_globals->map_active = 0;
    unlock_resources_and_resume_render_thread(wait_flags);
}

// @address 0x1b7318 game_dispose_from_old_structure_bsp
extern "C" void game_dispose_from_old_structure_bsp(const game_options *options)
{
    memcpy(&game_globals->options, options, sizeof(game_options));
    game_globals->options.load_level_only = false;
    if (game_is_multiplayer_inline() || options->game_variant.engine_type != _game_engine_base_variant)
    {
        game_globals->options.game_variant.copy_from_and_validate(&options->game_variant);
        game_engine_variant_validate(&game_globals->options.game_variant);
    }
    game_set_random_seed_inline(game_globals->options.random_seed);
    game_globals->lost = false;
    game_globals->finished = false;
    game_globals->scripted_type = 0;
    game_globals->ragdoll_count = 0;
    main_set_status_value("game_instance", "%016I64X", options->game_instance);
    main_set_status_value("game_simulation", "%s", k_game_simulation_names[options->game_simulation]);
    main_set_status_value("game_playback", "%s", k_game_playback_names[options->game_playback]);
}

// @address 0x1b7160 ?game_initialize_for_new_map@@YAXPBUgame_options@@@Z
void game_initialize_for_new_map(const game_options *options)
{
    game_globals_storage *globals = game_globals;
    memset(globals->cluster_pvs, 0, sizeof(globals->cluster_pvs));
    memset(globals->cluster_pvs_local, 0, sizeof(globals->cluster_pvs_local));
    memset(globals->cluster_activation, 0, sizeof(globals->cluster_activation));
    int wait_flags = ((int)::_internal_halt_render_thread_and_lock_resources("..\\game\\game.cpp", 698));
    if (g_shell_floating_point_exceptions_enabled)
        _controlfp(0x100001f, 0xfffff);
    game_globals->initializing = true;
    game_dispose_from_old_structure_bsp(options);
    unsigned int remaining = 92;
    const s_game_system *game_system = g_game_systems;
    do
    {
        if (game_system->initialize_for_new_map_proc)
            game_system->initialize_for_new_map_proc();
        remaining--;
        game_system++;
    } while (remaining != 0);
    game_globals->initializing = false;
    game_globals->map_active = true;
    unlock_resources_and_resume_render_thread(wait_flags);
}

static __forceinline bool game_is_cooperative_inline()
{
    game_globals_storage *globals = game_globals;
    bool active = globals && (globals->initializing || globals->map_active);
    return active && globals->options.game_playback != 0;
}

static __forceinline void game_simulation_notify_players_created()
{
    if ((*simulation_globals_get()).initialized && !(*simulation_globals_get()).loading_saved_game)
    {
        if (!(*simulation_globals_get()).world->is_playback())
            (*simulation_globals_get()).watcher->handle_player_creation();
    }
}

// @address 0x1b7538 ?game_create_players@@YAXXZ
void game_create_players()
{
    if (!main_game_reset_in_progress())
    {
        const game_options *options = &game_globals->options;
        players_set_machines(options->machine_valid_mask, options->machines);
        if (options->local_machine_exists)
            players_set_local_machine(&options->local_machine);
        else
            players_set_local_machine(NULL);
        for (long player_index = 0; player_index < 16; player_index++)
        {
            if (options->players[player_index].valid)
                player_new(player_index, &options->players[player_index], false);
        }
        if (game_is_cooperative_inline())
            players_remap_cooperative_users(1);
        game_simulation_notify_players_created();
    }
}

// @address 0x1b76a8 game_options_initialize_default
extern "C" void game_options_initialize_default(game_options *options)
{
    memset(options, 0, sizeof(game_options));
    options->game_mode = 1;
    options->game_simulation = 1;
    options->playtest_mode = g_default_playtest_mode;
    options->game_network_type = 0;

    int rate = (int)(rasterizer_main_get_video_mode()->RefreshRate + 0.5f);
    if (rate <= 0)
        rate = 60;
    options->game_tick_rate = (short)(rate / 2);

    long language = 0;
    if (global_scenario != 0)
    {
        long cache_language = global_game_globals->language;
        if (!game_progression_level_out_of_range(cache_language))
            language = cache_language;
    }
    options->language = language;

    options->campaign_difficulty = 1;
    options->campaign_id = _campaign_id_none;
    options->map_id = _map_id_none;

    long *p = &options->determinism_version;
    if (p != 0)
        *p = 1;

    transport_secure_random(8, options->random_data);
    options->random_seed = 0x78a8;
    options->game_variant.set_game_engine_index(_game_engine_base_variant);
    options->map_variant.create_default(_map_id_none);
}

static __forceinline void game_options_validate(game_options *options)
{
    game_engine_variant_validate(&options->game_variant);
    c_map_variant *map_variant = &options->map_variant;
    if (!map_variant_is_valid((const s_map_variant_validation *)map_variant))
        map_variant->create_default((e_map_id)map_variant->map_id);
}

// @address 0x1b77c8 game_options_validate_active_variants
extern "C" void game_options_validate_active_variants(void)
{
    game_options_validate(&game_globals->options);
}

// @address 0x1b7828 game_options_validate_variants
extern "C" void game_options_validate_variants(game_options *options)
{
    game_options_validate(options);
}
// @address 0x3ab90 k_zero_identifier
#if !defined(_XBOX) || defined(__clang__)
// The port links the zero identifier by its C name (game/game_runtime_globals.h): game_options_verify reads
// 8 zero bytes from it, so this unit owns the one external definition. (360 .rdata placement stays tag-only;
// see REPORT.md for the image-side blocker.)
extern "C" {
const unsigned char k_zero_identifier[sizeof(s_player_identifier)] = { 0 };
}
#endif

// match: 0x1b7880: the canonical break-based helper unrolls the validation scan (+96 bytes).
static __forceinline unsigned int game_csstrnlen(const char *string, unsigned int maximum)
{
    unsigned int length = 0;
    while (length < maximum)
    {
        if (!*string++)
            return length;
        ++length;
    }
    return length;
}

// @address 0x1b7880 ?game_options_verify@@YA_NPBUgame_options@@PADJ@Z
bool game_options_verify(const game_options *options, char *error_string, long error_string_length)
{
    bool valid = true;
    if (valid && (options->game_mode < 0 || options->game_mode >= 5))
        valid = false;
    if (valid && (options->game_simulation < 0 || options->game_simulation >= 6))
        valid = false;
    if (valid && (options->game_tick_rate <= 0 || options->game_tick_rate > 300))
        valid = false;
    if (valid && game_csstrnlen(options->scenario_path, 260) == 0)
        valid = false;
    if (valid && (options->initial_zone_set_index < 0 || options->initial_zone_set_index >= 48))
        valid = false;
    if (valid && (options->game_playback < 0 || options->game_playback >= 4))
        valid = false;
    if (valid && options->record_saved_film && options->game_playback != 0)
        valid = false;
    if (valid && options->map_variant.m_map_id != -1 && options->map_id != options->map_variant.m_map_id)
        valid = false;
    if (valid && (options->campaign_metagame_scoring < 0 || options->campaign_metagame_scoring >= 3))
        valid = false;
    if (valid && (options->campaign_insertion_point < 0 || options->campaign_insertion_point >= 4))
        valid = false;
    if (valid)
    {
        long player_count = 0;
        if (valid && (options->machine_valid_mask & 0xffff0000) != 0)
            valid = false;
        for (long machine_index = 0; valid && machine_index < 16; machine_index++)
        {
            if (options->machine_valid_mask & (1UL << machine_index))
            {
                for (long other_machine_index = machine_index + 1; valid && other_machine_index < 16; other_machine_index++)
                {
                    if ((options->machine_valid_mask & (1UL << other_machine_index)) &&
                        memcmp(&options->machines[machine_index], &options->machines[other_machine_index],
                            sizeof(s_machine_identifier)) == 0)
                        valid = false;
                }
            }
        }
        for (long player_index = 0; valid && player_index < 16; player_index++)
        {
            if (options->players[player_index].valid)
            {
                player_count++;
                if (valid && memcmp(&options->players[player_index].player_identifier, k_zero_identifier, sizeof(s_player_identifier)) == 0)
                    valid = false;
                for (long other_player_index = player_index + 1; valid && other_player_index < 16; other_player_index++)
                {
                    if (options->players[other_player_index].valid &&
                        memcmp(&options->players[player_index].player_identifier,
                            &options->players[other_player_index].player_identifier, sizeof(s_player_identifier)) == 0)
                        valid = false;
                }
                if (options->players[player_index].left_game)
                {
                    if (valid && memcmp(&options->players[player_index].machine_identifier, k_zero_identifier,
                        sizeof(s_machine_identifier)) != 0)
                        valid = false;
                }
                else
                {
                    if (valid && (options->players[player_index].user_index < 0 || options->players[player_index].user_index >= 4))
                        valid = false;
                    long matching_machine_index = -1;
                    for (long machine_index = 0; machine_index < 16; machine_index++)
                    {
                        if ((options->machine_valid_mask & (1UL << machine_index)) &&
                            memcmp(&options->players[player_index].machine_identifier, &options->machines[machine_index],
                                sizeof(s_machine_identifier)) == 0)
                        {
                            matching_machine_index = machine_index;
                            break;
                        }
                    }
                    if (valid && matching_machine_index == -1)
                        valid = false;
                }
            }
        }
        switch (options->game_mode)
        {
        case 1:
            if (valid && (options->campaign_difficulty < 0 || options->campaign_difficulty >= 4))
                valid = false;
            if (valid && (player_count < 1 || player_count > 4))
                valid = false;
            if (valid && !options->local_machine_exists)
                valid = false;
            break;
        case 2:
            if (valid && (player_count < 1 || player_count > 16))
                valid = false;
            if (valid && !options->local_machine_exists)
                valid = false;
            break;
        case 3:
            if (valid && player_count == 0)
                valid = false;
            if (valid && !options->local_machine_exists)
                valid = false;
            break;
        case 4:
            break;
        default:
            if (!valid)
                valid = false;
            break;
        }
    }
    return valid;
}

// @address 0x1b7e40 game_options_encode
extern "C" void game_options_encode(c_bitstream *packet, const game_options *options)
{
    packet->write_integer("game-mode", options->game_mode, 3);
    packet->write_integer("game-simulation", options->game_simulation, 3);
    packet->write_integer("game-network-type", options->game_network_type, 2);
    packet->write_integer("game-tick-rate", options->game_tick_rate, 6);
    packet->write_raw_data("game-instance", options->random_data, 64);
    packet->write_integer("random-seed", options->random_seed, 32);
    packet->write_integer("language", options->language, 4);
    packet->write_integer("determinism-version", options->determinism_version, 8);
    packet->write_integer("campaign-id", options->campaign_id, 32);
    packet->write_integer("map-id", options->map_id, 32);
    packet->write_string("scenario-path", options->scenario_path, 260);
    packet->write_integer("initial-zone-set", options->initial_zone_set_index, 6);
    game_options_encode_campaign_armaments(packet, &options->campaign_armaments);
    packet->write_bool("load-level-only", options->load_level_only);
    packet->write_integer("campaign-insertion", options->dump_machine_index + 1, 5);
    packet->write_bool("dump-object-log", options->dump_object_log);
    packet->write_bool("dump-random-seeds", options->dump_random_seeds);
    packet->write_bool("survival-enabled", options->survival_enabled);
    packet->write_integer("game-playback", options->game_playback, 2);
    packet->write_bool("record-saved-film", options->record_saved_film);
    packet->write_integer("playback-start", options->playback_start_in_ticks, 32);
    packet->write_integer("playback-length", options->playback_length_in_ticks, 32);
    packet->write_integer("campaign-difficulty", options->campaign_difficulty, 2);
    packet->write_integer("campaign-insertion-point", options->campaign_insertion_point, 2);
    packet->write_integer("campaign-metagame-scoring", options->campaign_metagame_scoring, 2);
    packet->write_bool("campaign-metagame-enabled", options->campaign_metagame_enabled);
    packet->write_bool("survival-mode-enabled", options->survival_mode_enabled);
    packet->write_integer("primary-skulls", options->primary_skulls, 32);
    packet->write_integer("secondary-skulls", options->secondary_skulls, 32);
    packet->write_bool("matchmade-game", options->matchmade_game);
    packet->write_raw_data("campaign-progress", &options->matchmaking_options, 736);
    packet->write_bool("game-variant-present", options->game_variant.engine_type != 0);
    if (options->game_variant.engine_type != 0)
        options->game_variant.encode(packet);
    options->map_variant.encode(packet);
    packet->write_raw_data("machines", &options->machine_valid_mask, 864);
    packet->write_raw_data("players", options->players, 37888);
}

// @address 0x1b84f8 game_options_encode_campaign_armaments
extern "C" void game_options_encode_campaign_armaments(c_bitstream *packet, const s_campaign_armaments *armaments)
{
    bool data_present = false;
    for (unsigned long index = 0; index < 4; index++)
    {
        if (armaments->players[index].valid)
        {
            data_present = true;
            break;
        }
    }
    packet->write_bool("data-present", data_present);
    if (data_present)
        packet->write_raw_data("data", armaments, 960);
}

// @address 0x1b8578 game_options_decode_campaign_armaments
bool game_options_decode_campaign_armaments(c_bitstream *packet, s_campaign_armaments *armaments)
{
    if (packet->read_bool("data-present"))
        packet->read_raw_data("data", armaments, 960);
    bool success = true;
    success = success && !packet->error_occurred();
    return success;
}

// @address 0x1b8600 ?game_options_match@@YA_NPBUgame_options@@0@Z
bool game_options_match(const game_options *options1, const game_options *options2)
{
    bool match = true;
    match = match && strcmp(options1->scenario_path, options2->scenario_path) == 0;
    match = match && options1->game_mode == options2->game_mode;
    match = match && options1->game_tick_rate == options2->game_tick_rate;
    if (match)
    {
        switch (options1->game_mode)
        {
        case 1:
            match = match && options1->campaign_difficulty == options2->campaign_difficulty;
            break;
        case 2:
            match = match && options1->game_variant.is_equal_to(&options2->game_variant);
            break;
        }
    }
    return match;
}

// 0x1b8708
int sort_controllers_ascending(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    if (x == -1 && y != -1)
        return 1;
    if (x != -1 && y == -1)
        return -1;
    if (x > y)
        return 1;
    if (x < y)
        return -1;
    return 0;
}


// @address 0x1b8750 ?game_options_setup_default_players@@YAXJPAUgame_options@@@Z
// The default launch options contain one player.
void game_options_setup_default_players(long player_count, game_options *options)
{
    const long default_player_count = 1;
    options->machine_valid_mask = 1;
    options->machines[0] = g_transport_security_globals.local_machine_identifier;
    options->local_machine_exists = true;
    options->local_machine = options->machines[0];

    bool occupied_controllers[k_number_of_controllers];
    e_controller_index controller_indices[k_number_of_controllers];
    for (e_controller_index controller_index = first_controller(); controller_index != _controller_none;
        controller_index = next_controller(controller_index))
    {
        occupied_controllers[controller_index] = false;
        controller_indices[controller_index] = _controller_none;
    }

    for (long player_index = 0; player_index < default_player_count; player_index++)
    {
        e_controller_index controller_index = _controller_none;
        for (e_controller_index test_controller = first_controller(); test_controller != _controller_none;
            test_controller = next_controller(test_controller))
        {
            if (input_globals.gamepad_connected[(short)test_controller] && !occupied_controllers[test_controller])
            {
                controller_index = test_controller;
                break;
            }
        }
        if (controller_index == _controller_none)
            controller_index = _controller_index_0;
        if (occupied_controllers[controller_index])
        {
            for (e_controller_index test_controller = first_controller(); test_controller != _controller_none;
                test_controller = next_controller(test_controller))
            {
                if (!occupied_controllers[test_controller])
                {
                    controller_index = test_controller;
                    break;
                }
            }
        }
        controller_indices[player_index] = controller_index;
        occupied_controllers[controller_index] = true;
    }
    qsort(controller_indices, k_number_of_controllers, sizeof(e_controller_index), sort_controllers_ascending);

    memset(options->players, 0, sizeof(options->players));
    for (long player_index = 0; player_index < default_player_count; player_index++)
    {
        e_controller_index controller_index = controller_indices[player_index];
        game_player_options *player = &options->players[player_index];
        player->valid = true;
        player->left_game = false;
        player->machine_identifier = options->local_machine;
        player->user_index = (short)player_index;
        player->controller_index = controller_index;
        memset(&player->player_identifier, 0, sizeof(s_player_identifier));
        player->player_identifier.data[0] = (unsigned char)player_index;
        player->player_identifier.data[1] = 255;
        player->player_identifier.data[2] = 255;
        player->player_identifier.data[3] = 255;
        ustrnzcpy_inline(player->configuration.client.name, game_empty_player_name(), 16);
        player_appearance_initialize(&player->configuration.client.appearance);
        if (options->game_mode == 2)
            player->configuration.host.team = player_index;
        else
            player->configuration.host.team = 1;
    }
}

static __forceinline const game_options *game_options_get_inline()
{
    return &game_globals->options;
}
static __forceinline long game_options_valid_player_count(const game_options *options)
{
    long player_count = 0;
    for (long player_index = 0; player_index < 16; player_index++)
    {
        if (options->players[player_index].valid)
            player_count++;
    }
    return player_count;
}

// @address 0x1b8b08 game_campaign_has_multiple_players
extern "C" bool game_campaign_has_multiple_players(void)
{
    bool result = false;
    if (game_options_get_inline()->game_mode == 1)
    {
        long player_count = 0;
        long options_player_count = game_options_valid_player_count(game_options_get_inline());
        if (player_count <= options_player_count)
            player_count = options_player_count;
        long datum_count = 0;
        c_data_iterator<player_datum> iterator;
        iterator.begin(player_data);
        while (iterator.next())
            datum_count++;
        if (player_count <= datum_count)
            player_count = datum_count;
        result = player_count > 1;
    }
    return result;
}

static __forceinline bool game_map_active_inline()
{
    game_globals_storage *globals = game_globals;
    return globals && (globals->initializing || globals->map_active);
}
// @address 0x1b8c30 game_is_film_playback_active
extern "C" bool game_is_film_playback_active()
{
    return game_map_active_inline() && game_globals->options.game_playback != 0 && game_globals->options.game_playback != 3;
}


static __forceinline void game_simulation_notify_game_start()
{
    if ((*simulation_globals_get()).initialized)
        (*simulation_globals_get()).world->attach_to_map();
}

// @address 0x1b8a00 ?game_start@@YAXW4e_game_create_mode@@@Z
void game_start(e_game_create_mode mode)
{
    game_globals->in_progress = true;
    game_simulation_notify_game_start();
    int lock = ::tag_resources_lock_game();
    game_engine_game_starting();
    FILE *file = fopen("d:\\launch.txt", "r");
    if (file)
    {
        char script_name[256];
        if (fscanf(file, "%*s\n%*s\n") == 0 && fgets(script_name, 256, file))
        {
            char *newline = strpbrk(script_name, "\r\n");
            if (newline)
                *newline = 0;
            long thread_index = hs_find_thread_by_name(script_name);
            if (thread_index != -1)
                hs_wake(thread_index, -1);
        }
        fclose(file);
    }
    ::tag_resources_unlock_game((*(long *)(&lock)));
}

// @address 0x1b8ca0 ?game_simulation_set@@YAXW4e_game_simulation_type@@@Z
void game_simulation_set(e_game_simulation_type simulation_type)
{
    game_globals->options.game_simulation = simulation_type;
    main_set_status_value("game_simulation", "%s", k_game_simulation_names[simulation_type]);
}

static __forceinline bool game_skull_active_inline(long skull)
{
    unsigned long active_skulls = game_globals->options.game_mode == 1 ? game_globals->options.active_skulls : 0;
    if ((active_skulls >> skull) & 1)
        return true;
    return false;
}
// @address 0x1b8cf8 game_is_coop_respawn_allowed
extern "C" bool game_is_coop_respawn_allowed()
{
    return !game_skull_active_inline(0);
}

// @address 0x1b9100 ?game_finish@@YAXXZ
void game_finish()
{
    if (!game_globals->finished)
    {
        game_globals->finished = true;
        game_globals->finish_ticks = game_seconds_to_ticks_round_inline(7.0f);
        game_globals->finish_immediate = false;
    }
}

// @address 0x1b9180 game_mark_map_completed_at_difficulty
void game_mark_map_completed_at_difficulty(short campaign_difficulty)
{
    if (campaign_difficulty < 0 || campaign_difficulty >= 4)
        return;

    game_globals_storage *globals = game_globals;
    if (globals->options.game_mode != 1)
        return;

    if (game_is_cooperative())
        return;

    int map = levels_get_campaign_map_index(globals->options.campaign_id, globals->options.map_id);
    if ((unsigned int)map > 31)
        return;

    e_controller_index controller_index = first_controller();
    do
    {
        c_controller_interface *controller = controller_get(controller_index);
        if (controller->in_use())
        {
            c_player_profile_interface *profile = controller->get_player_profile_interface();
            profile->set_map_completed_at_difficulty_level(map, campaign_difficulty, true, true);
        }
        controller_index = next_controller(controller_index);
    } while (controller_index != _controller_none);
}

// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
static __forceinline object_datum *game_scripted_object_get(long index)
{
    return (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(index)].datum;
}
static __forceinline long game_scripted_ultimate_parent(long index)
{
    long result = -1;
    while (index != -1)
    {
        result = index;
        index = game_scripted_object_get(index)->parent;
    }
    return result;
}

static __forceinline bool game_scripted_object_location_valid(const object_datum *object)
{
    bool result = ((object->flags >> 8) & 1) != 0;
    return result;
}
// @address 0x1b9248 ?game_pvs_scripted_get_cluster_reference@@YA?AUs_cluster_reference@@XZ
s_cluster_reference game_pvs_scripted_get_cluster_reference()
{
    s_cluster_reference result;
    cluster_reference_invalidate(&result);
    switch (game_globals->scripted_type)
    {
    case 1:
        if (object_try_and_get_and_verify_type(game_globals->scripted_object, ~0ul))
        {
            object_datum *object = game_scripted_object_get(game_scripted_ultimate_parent(game_globals->scripted_object));
            if (game_scripted_object_location_valid(object) && (unsigned char)object->location.cluster_reference.bsp_index != 255)
                result = object->location.cluster_reference;
        }
        else
            game_globals->scripted_type = 0;
        break;
    case 2:
        result = game_globals->scripted_cluster;
        break;
    }
    return result;
}

static __forceinline long game_pvs_first_active_user()
{
    long user = 0;
    do {
        if (player_mapping_globals->player_indices[user] != -1)
            return user;
        user++;
    } while (user < 4);
    return -1;
}
static __forceinline const s_observer_result *game_pvs_observer_get(long user)
{
    return &TLS_OBSERVER_GLOBALS->blocks[user].camera_result;
}

static __forceinline const s_scenario_zone_set *game_pvs_zone_get(const s_tag_block &block, const long &index)
{
    return TAG_BLOCK_GET_ELEMENT(&block, index, s_scenario_zone_set);
}

static __forceinline long game_pvs_zone_value(const s_tag_block &block, long index)
{
    long byte_offset = index * 36;
    return ((s_scenario_zone_set *)((char *)block.address + (byte_offset & ~3)))->pvs_index;
}
static __forceinline void game_pvs_clear_inactive(s_game_cluster_bit_vectors *pvs, long zone_index)
{
    scenario *scenario_definition = global_scenario;
    unsigned long active_mask = g_active_structure_bsp_mask;
    s_scenario_zone_set zone_value; zone_value.pvs_index = game_pvs_zone_value(scenario_definition->zone_sets, zone_index);
    const s_scenario_zone_set *zone = &zone_value;
    const s_scenario_zone_set_visibility *visibility = TAG_BLOCK_GET_ELEMENT(&scenario_definition->zone_set_pvs, zone->pvs_index,
        s_scenario_zone_set_visibility);
    for (long bsp_index = 0; bsp_index < 16; bsp_index++) {
        if (visibility->contains_bsp(bsp_index) && !(active_mask & (1UL << bsp_index))) {
            for (long word_index = 0; word_index < 8; word_index++)
                pvs->bit_vectors[bsp_index].get_writeable_bits_direct()[word_index] = 0;
        }
    }
}

// @address 0x1b9358 game_pvs_compute_combined
extern "C" void game_pvs_compute_combined(
    s_game_cluster_bit_vectors *pvs, bool use_observers, c_static_array<c_static_array<long, 256>, 16> *activation_reasons)
{
    memset(pvs, 0, sizeof(*pvs));
    const game_globals_storage *globals = game_globals;
    long zone_index = global_zone_set_index;
    long scenario_index = global_scenario_index;
    if (globals->options.game_mode != 3) {
        players_compute_combined_pvs(pvs, use_observers, 0);
    } else {
        long user = game_pvs_first_active_user();
        if (user != -1) {
            const s_observer_result *observer = game_pvs_observer_get(user);
            s_cluster_reference reference = observer->location.cluster_reference;
            if ((unsigned char)reference.bsp_index != 255) {
                s_scenario_pvs_row row;
                scenario_zone_set_pvs_get_row(scenario_index, &row, zone_index, observer->location.cluster_reference, false);
                scenario_zone_set_pvs_write_row(pvs, &row);
            }
        }
    }
    if (globals->scripted_camera_pvs) {
        s_cluster_reference reference;
        if (*g_director_camera_scripted) {
            reference = scripted_camera_get_deterministic_cluster_reference();
            scenario_index = global_scenario_index;
            zone_index = global_zone_set_index;
        } else {
            cluster_reference_invalidate(&reference);
        }
        if ((unsigned char)reference.bsp_index != 255) {
            s_scenario_pvs_row row;
            s_game_cluster_bit_vectors camera_pvs;
            scenario_zone_set_pvs_get_row(scenario_index, &row, zone_index, reference, false);
            scenario_zone_set_pvs_write_row(&camera_pvs, &row);
            game_clusters_or(pvs, &camera_pvs, pvs);
        }
    }
    game_pvs_clear_inactive(pvs, zone_index);
}

static __forceinline long game_pvs_lowest_bit(unsigned long mask)
{
    return 31 - _CountLeadingZeros(-(long)mask & mask);
}
static __forceinline long game_pvs_first_active_bsp()
{
    return game_pvs_lowest_bit(g_active_structure_bsp_mask);
}
static __forceinline long game_pvs_next_active_bsp(long index)
{
    return game_pvs_lowest_bit(g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1));
}
static __forceinline structure_bsp *game_pvs_active_bsp_get(long index)
{
    const scenario_structure_bsp_reference *reference = TAG_BLOCK_GET_ELEMENT(&global_scenario->structure_bsps, index,
        scenario_structure_bsp_reference);
    return TAG_GET('sbsp', structure_bsp, reference->structure_bsp.index);
}
static __forceinline void game_pvs_bit_vector_or(long count, const unsigned long *a, const unsigned long *b, unsigned long *output)
{
    for (long word = ((count + 31) >> 5) - 1; word >= 0; --word)
        output[word] = a[word] | b[word];
}

// match: 0x1b9548: the typed copy boundary preserves six register-allocation words.
static __forceinline void *game_pvs_csmemcpy(c_static_flags_no_init<256> *destination, const c_static_flags_no_init<256> *source, unsigned int size)
{
    return memcpy(destination, source, size);
}

// @address 0x1b9548 game_pvs_update
extern "C" void game_pvs_update()
{
    s_game_cluster_bit_vectors old_activation = *(const s_game_cluster_bit_vectors *)game_globals->cluster_activation;
    s_game_cluster_bit_vectors old_pvs = *(const s_game_cluster_bit_vectors *)game_globals->cluster_pvs;
    s_game_cluster_bit_vectors old_local_pvs = *(const s_game_cluster_bit_vectors *)game_globals->cluster_pvs_local;

    game_pvs_compute_combined((s_game_cluster_bit_vectors *)game_globals->cluster_pvs, false, 0);
    game_pvs_compute_combined((s_game_cluster_bit_vectors *)game_globals->cluster_pvs_local, true, 0);

    for (long bsp_index = game_pvs_first_active_bsp(); bsp_index != -1; bsp_index = game_pvs_next_active_bsp(bsp_index))
    {
        structure_bsp *bsp = game_pvs_active_bsp_get(bsp_index);
        unsigned long active_clusters[8];
        if (structure_bsp_get_actor_clusters(bsp_index, active_clusters, bsp->clusters.count))
            game_pvs_bit_vector_or(bsp->clusters.count, game_globals->cluster_pvs[bsp_index], active_clusters,
                game_globals->cluster_activation[bsp_index]);
        else
            game_pvs_csmemcpy((c_static_flags_no_init<256> *)game_globals->cluster_activation[bsp_index],
                (const c_static_flags_no_init<256> *)game_globals->cluster_pvs[bsp_index], sizeof(c_static_flags_no_init<256>));
    }

    s_cluster_reference reference = game_pvs_scripted_get_cluster_reference();
    if ((unsigned char)reference.bsp_index != 255)
    {
        s_game_cluster_bit_vectors scripted_pvs;
        s_scenario_pvs_row row;
        scenario_zone_set_pvs_get_row(global_scenario_index, &row, global_zone_set_index, reference, false);
        scenario_zone_set_pvs_write_row(&scripted_pvs, &row);
        game_clusters_or((s_game_cluster_bit_vectors *)game_globals->cluster_activation, &scripted_pvs,
            (s_game_cluster_bit_vectors *)game_globals->cluster_activation);
    }

    if (memcmp(&old_pvs, game_globals->cluster_pvs, sizeof(old_pvs)) != 0)
        for (unsigned long i = 0; i < NUMBEROF(g_game_systems); ++i)
            if (g_game_systems[i].change_pvs_proc)
                g_game_systems[i].change_pvs_proc(&old_pvs, (const s_game_cluster_bit_vectors *)game_globals->cluster_pvs, false);

    if (memcmp(&old_local_pvs, game_globals->cluster_pvs_local, sizeof(old_local_pvs)) != 0)
        for (unsigned long i = 0; i < NUMBEROF(g_game_systems); ++i)
            if (g_game_systems[i].change_pvs_proc)
                g_game_systems[i].change_pvs_proc(&old_local_pvs, (const s_game_cluster_bit_vectors *)game_globals->cluster_pvs_local,
                    true);

    if (memcmp(&old_activation, game_globals->cluster_activation, sizeof(old_activation)) != 0)
        for (unsigned long i = 0; i < NUMBEROF(g_game_systems); ++i)
            if (g_game_systems[i].change_activation_proc)
                g_game_systems[i].change_activation_proc(&old_activation,
                    (const s_game_cluster_bit_vectors *)game_globals->cluster_activation);
}

// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

typedef void (*field_proc_t)(void *field);

static __forceinline bool game_update_in_progress(void)
{
    game_globals_storage *globals = game_globals;
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}
static __forceinline bool game_update_is_campaign(void)
{
    return game_globals->options.game_mode == 1;
}

static __forceinline unsigned short game_update_random_next(unsigned long *seed)
{
    *seed = *seed * 1664525UL + 1013904223UL;
    return (unsigned short)(*seed >> 16);
}

// @address 0x1b98b8 ?game_tick_pulse_random_seed_deterministic@@YAXPBUsimulation_update@@@Z
void game_tick_pulse_random_seed_deterministic(struct simulation_update const * update)
{
    if (game_update_in_progress() && game_update_is_campaign())
    {
        for (unsigned long player_index = 0; player_index < 16; player_index++)
        {
            if (update->player_valid_mask & (1UL << player_index))
            {
                const player_action *action = &update->actions[player_index];
                long pulses = 0;
                if (action->facing.pitch > 0.0f)
                    pulses |= 1;
                else if (action->facing.pitch < 0.0f)
                    pulses |= 2;
                if (action->facing.yaw > 0.0f)
                    pulses |= 4;
                else
                    pulses |= 8;
                for (long pulse = 0; pulse < pulses; pulse++)
                    game_update_random_next(tls_random_seed());
            }
        }
    }
}

static __forceinline bool game_is_campaign_inline()
{
    return game_globals->options.game_mode == 1;
}
static __forceinline bool game_coop_allow_respawn_inline()
{
    return !game_skull_active_inline(0);
}
static __forceinline bool game_survival_reset_inline()
{
    bool result = false;
    if (!game_campaign_is_multiplayer() && game_skull_active_inline(0))
        result = true;
    return result;
}
static __forceinline void game_main_reset_map(bool random)
{
    game_main_requests().reset_map = true;
    game_main_requests().reset_map_parameter = random;
}
static __forceinline void game_main_revert_map()
{
    game_main_requests().revert_map = true;
}

// @address 0x1b99d8 ?game_loss_update@@YAXXZ
void game_loss_update()
{
    if (game_is_campaign_inline() && !game_is_predicted_inline())
    {
        bool players_dead;
        bool players_alive;
        if (game_campaign_is_multiplayer() && !game_coop_allow_respawn_inline())
            players_death_status_calculate((unsigned char *)&players_dead, (unsigned char *)&players_alive);
        else
            players_death_status_calculate((unsigned char *)&players_alive, (unsigned char *)&players_dead);
        game_lost(players_dead);
        if (game_globals->lost)
        {
            if (game_globals->revert_ticks > 0)
                game_globals->revert_ticks--;
            if (game_globals->revert_ticks == 0)
            {
                if (!game_survival_reset_inline())
                    game_main_revert_map();
                else
                    game_main_reset_map(false);
            }
        }
    }
}

/* as tu/blamlibxbox/players.cpp declares it */

// @address 0x1b9b38 ?game_can_create_additonal_ragdoll@@YA_NW4e_ragdoll_source@@@Z
bool game_can_create_additonal_ragdoll(e_ragdoll_source ragdoll_source)
{
    bool result = false;
    long player_count = 0;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
        player_count++;
    if (game_globals->ragdoll_count < 5)
    {
        switch (ragdoll_source)
        {
        case _ragdoll_source_scripted:
            result = true;
            break;
        case _ragdoll_source_death:
            result = game_globals->ragdoll_count < 4;
            break;
        default:
            // match: 0x1b9b38: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    return result;
}

extern "C" {
      // this thread's base of each layout
}
class c_restricted_memory;

// @address 0x1b9d88 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = g_restricted_regions[3].add_member((const char *)name, (const char *)type, size, 0,
      __tls_set_g_game_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = g_registered_thread_index;
  m_thread_id = owner;
  m_member_index = member_index;
  game_globals = (game_globals_storage *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0x1b9be8 ?game_time_get_date_and_time@@YAXPAUs_date_and_time@@@Z
void game_time_get_date_and_time(s_date_and_time *date_and_time)
{
    memset(date_and_time, 0, sizeof(s_date_and_time));
    restricted_region_is_available(1);
    SYSTEMTIME local_time;
    GetLocalTime(&local_time);
    date_and_time->year = local_time.wYear;
    date_and_time->month = local_time.wMonth;
    date_and_time->day = local_time.wDay;
    date_and_time->hour = local_time.wHour;
    date_and_time->minute = local_time.wMinute;
    date_and_time->second = local_time.wSecond;
}

// 0x1b9c70: game_time_get_holiday (local/debugnames pairs that name with 0x31d7f8, its caller)
// @address 0x1b9c70 game_time_compute_holiday
extern "C" e_game_time_holiday game_time_compute_holiday()
{
    e_game_time_holiday holiday = _game_time_holiday_none;
    s_date_and_time date_and_time;
    game_time_get_date_and_time(&date_and_time);
    if (date_and_time.month == 1 && date_and_time.day == 1)
        holiday = _game_time_holiday_new_years_day;
    else if (date_and_time.month == 4 && date_and_time.day == 22)
        holiday = _game_time_holiday_earth_day;
    else if (date_and_time.month == 5 && date_and_time.day == 5)
        holiday = _game_time_holiday_cinco_de_mayo;
    else if (date_and_time.month == 7 && date_and_time.day == 4)
        holiday = _game_time_holiday_independence_day;
    else if (date_and_time.month == 7 && date_and_time.day == 7)
        holiday = _game_time_holiday_july_seventh;
    else if (date_and_time.month == 10 && date_and_time.day == 31)
        holiday = _game_time_holiday_halloween;
    else if (date_and_time.month == 11 && date_and_time.day == 11)
        holiday = _game_time_holiday_veterans_day;
    else if (date_and_time.month == 12 && date_and_time.day == 12)
        holiday = _game_time_holiday_december_twelfth;
    else if (date_and_time.month == 12 && date_and_time.day == 25)
        holiday = _game_time_holiday_christmas;
    return holiday;
}

extern "C" {
// @address 0x58a04 k_game_time_july_seventh
const float k_game_time_july_seventh = 0.625f;
}

// @rdata 0x827c6054
static const float k_game_time_cinco_de_mayo = 0.375f;

// @address 0x31d7f8 game_time_get_holiday
extern "C" float game_time_get_holiday(void)
{
    float result = game_holiday_real_constants()[200];
    e_game_time_holiday holiday = game_time_compute_holiday();
    switch (holiday - 1)
    {
    case 0:
        result = game_holiday_real_constants()[7836];
        break;
    case 1:
        result = 0.25f;
        break;
    case 2:
        result = k_game_time_cinco_de_mayo;
        break;
    case 3:
        result = game_holiday_real_constants()[753];
        break;
    case 4:
        result = k_game_time_july_seventh;
        break;
    case 5:
        result = game_holiday_real_constants()[582];
        break;
    case 6:
        result = 0.875f;
        break;
    case 8:
        result = game_holiday_real_constants()[0];
        break;
    }
    return result;
}

// 0x1b9e28
template <>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x1b9e40
template <>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    game_globals = 0;
    restricted_region_free_member(3, value);
}

// @address 0x1e1b58 game_saved_film_start_timer
extern "C" void game_saved_film_start_timer(void)
{
    bool saved_film_in_progress = false;
    game_globals_storage *globals = game_globals;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        saved_film_in_progress = true;
    if (saved_film_in_progress && game_is_cooperative() && game_is_film_playback_active())
        saved_film_initialize_timer();
}

static __forceinline bool game_saved_film_in_progress_inline()
{
    game_globals_storage *globals = game_globals;
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

// @address 0x1e2300 game_saved_film_reset_playback_speed
extern "C" void game_saved_film_reset_playback_speed(void)
{
    if (game_saved_film_in_progress_inline() && game_is_cooperative() && game_is_film_playback_active() && saved_film_test_slot_available())
    {
        g_saved_film_timer_globals.timer_other = 0;
        saved_film_set_playback_rate(1.0f, 0);
    }
}
// (resubmitted unchanged in code: the earlier refusal was a link placement flake in another unit, see NOTES.md)

static __forceinline bool game_won_in_progress(void)
{
    game_globals_storage *globals = game_globals;
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}


static __forceinline bool game_won_controller_signed_in(c_controller_interface *controller)
{
    return controller->is_signed_in_to_machine() || (_rotl(((s_controller *)controller)->flags, 30) & 1) != 0;
}

static __forceinline void game_won_unlock_map(c_player_profile_interface *profile, short map_index)
{
    if (map_index >= 0 && map_index < 32)
    {
        profile->set_unknown_flag_9c(map_index, true, true);
        if (map_index + 1 < 32)
        {
            short next_map_index = map_index + 1;
            profile->set_unknown_flag_9c(next_map_index, true, true);
            profile->set_map_completed_at_difficulty_level(next_map_index, 0, true, true);
        }
    }
}

// 0x1b8d38
// @address 0x1b8d38 game_record_campaign_completion
extern "C" void game_record_campaign_completion(void)
{
    if (!game_won_in_progress() || game_is_cooperative())
        return;

    game_options *options = &game_globals->options;
    if (options->game_mode != 1 || options->map_id == -1)
        return;

    long map_index = levels_get_campaign_map_index(options->campaign_id, options->map_id);
    e_campaign_game_mode campaign_game_mode = game_campaign_is_multiplayer() ? _campaign_game_mode_cooperative : _campaign_game_mode_solo;
    long difficulty = options->campaign_difficulty;
    __int64 time = _time64(0);
    bool first_insertion_point = options->campaign_insertion_point == 0;

    for (e_controller_index controller_index = first_controller(); controller_index != _controller_none;
        controller_index = next_controller(controller_index))
    {
        c_controller_interface *controller = controller_get(controller_index);
        if (!controller->in_use())
            continue;

        c_player_profile_interface *profile = controller->get_player_profile_interface();
        if (first_insertion_point)
        {
            if (difficulty > 0)
                campaign_metagame_grant_map_achievement(controller_index, options->map_id);
            profile->set_unknown_level_flag_40(campaign_game_mode, map_index, (e_campaign_difficulty_level)difficulty, true, true);
            if (game_won_controller_signed_in(controller))
            {
                e_achievement achievements[4] = {
                    _achievement_none,
                    _achievement_campaign_complete_normal,
                    _achievement_campaign_complete_heroic,
                    _achievement_campaign_complete_legendary
                };
                long highest = campaign_metagame_get_highest_completed_difficulty(controller_index);
                for (long i = 0; i < 4; i++)
                {
                    if (achievements[i] != _achievement_none && highest >= i)
                        game_grant_achievement_to_local_player(controller_index, achievements[i], false);
                }
            }
        }
        profile->set_last_campaign_played_time(campaign_game_mode, time, true);
        profile->set_last_campaign_map_played_absolute_index((short)map_index, true);
        if ((unsigned int)map_index <= 31)
            game_won_unlock_map(profile, (short)map_index);
    }

    if (!game_main_requests().campaign_progress_saved)
    {
        game_main_requests().campaign_progress_saved = true;
        if (simulation_starting_up())
            chud_messaging_special_progress(true);
    }
}

class c_interlocked_long;

// @address 0xa3918 game_update_ticks
extern "C" void game_update_ticks(long count, float *output)
{
    g_render_thread_enabled.peek();
    long i;
    for (i = 0; i < count; i++)
    {
        if (main_has_pending_state_changes() || simulation_aborted())
            break;
        game_tick();
    }
    if (i < count)
    {
        if (i == 0)
        {
            *output = 0.0f;
        }
        else
        {
            float elapsed = (float)(count - i) * game_time_globals->tick_length;
            *output = *output - elapsed > 0.0f ? *output - elapsed : 0.0f;
        }
    }
}

static __forceinline s_campaing_metagame_globals *game_won_metagame_globals_get(void)
{
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    s_campaing_metagame_globals *result = 0;
    if (globals && (globals->flags & 2))
        result = globals;
    return result;
}

static __forceinline s_campaign_metagame_player_globals *game_won_slot_globals_get(void)
{
    s_campaign_metagame_player_globals *result = 0;
    if (g_campaign_metagame_player_globals.active)
        result = &g_campaign_metagame_player_globals;
    return result;
}

// @address 0x1b9010 ?game_won@@YAXXZ
void game_won(void)
{
    if (simulation_starting_up())
    {
        long game_mode = game_globals->options.game_mode;
        if (game_mode == 1)
        {
            game_record_campaign_completion();
            if (campaign_metagame_is_enabled())
                campaign_metagame_handle_game_won();
            if (!game_is_cooperative())
                game_lsp_submit_campaign_result(&game_globals->options, game_won_metagame_globals_get(), game_won_slot_globals_get());
            main_game_globals.game_won = true;
        }
        else if (game_mode == 2)
        {
            game_finish();
        }
    }
}

// @address 0x1b7440 ?game_create_objects@@YAXW4e_game_create_mode@@@Z
void game_create_objects(enum e_game_create_mode game_create_mode)
{
    int lock_token[1];
    int lock = 0;

    if (game_create_mode == _game_create_mode_lock)
        lock = ::tag_resources_lock_game();
    lock_token[0] = lock;

    ::object_placement_create_global_objects(game_globals->options.game_mode, 0);

    int index = 0;
    scenario *sc = global_scenario;
    long *count_ptr = &sc->light_volumes.count;
    bool light_mode = (game_globals->options.game_mode - 4) == 0;
    if (sc->light_volumes.count > 0)
    {
        int offset = 0;
        do
        {
            s_scenario_light *light = (s_scenario_light *)((char *)sc->light_volumes.address + offset);
            int palette_index = light->palette_index;
            if (palette_index != -1 && palette_index >= 0 && palette_index < sc->light_volume_palette.count)
            {
                int datum = ((scenario_object_palette_entry *)sc->light_volume_palette.address)[palette_index].object.index;
                if (datum != -1)
                {
                    ::light_new_from_scenario(datum, index, light_mode, light);
                    sc = global_scenario;
                }
            }
            ++index;
            offset += sizeof(s_scenario_light);
        } while (index < *count_ptr);
    }

    if (game_create_mode == _game_create_mode_lock)
        ::tag_resources_unlock_game((*(long *)(lock_token)));
}

struct s_chud_globals_definition;
extern "C" {
// @address 0x10f5c80 g_render_postprocess_color_tweaking

}
static inline long game_tick_user_unit(long user)
{
    long unit = -1;
    if (user != -1)
        unit = player_mapping_globals->unit_indices[user];
    return unit;
}
static __forceinline void game_tick_destroy_update(simulation_update *update)
{
    c_simulation_queue *queue = &update->bookkeeping;
    if (queue->initialized)
    {
        queue->clear();
        queue->initialized = false;
    }
    queue = &update->simulation;
    if (queue->initialized)
        queue->clear();
}

// @address 0xa39f0 ?game_tick@@YAXXZ
void game_tick()
{
    game_globals->game_had_an_update_tick_this_frame = true;
    const char *status_name = "game_tick";
    long time = game_time_globals->time;
    {
        simulation_update update;
        s_simulation_update_metadata metadata;
        main_set_status_value(status_name, "time %d", time);
        if (g_shell_floating_point_exceptions_enabled)
            _controlfp(0x100001f, 0xfffff);
        simulation_build_game_update(1, &update, &metadata);
        saved_film_update_recording(&update);
        g_water_interaction_event_count = 0;
        ((s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data)->count = 0;
        for (unsigned long index = 0; index < 128; ++index)
            g_water_interaction_events[index].type = -1;
        g_tracked_object_globals->active = true;
        simulation_apply_before_game(&update);
        levels_update();
        if ((static_cast<unsigned short>(update.flags) & 1u) != 0u)
        {
            if (chud_globals)
            {
                g_chud_scripting_globals->user_index = game_time_globals->time;
                s_equipment_slot *slot = g_chud_scripting_globals->equipment_slots;
                unsigned long count = 8;
                do
                {
                    slot->active = false;
                    ++slot;
                } while (--count);
            }
            game_sound_update_timeline();
            players_update_before_game(&update);
            refresh_listener(false);
            game_tick_pulse_random_seed_deterministic(&update);
            ai_update();
            recorded_animations_update();
            game_engine_update();
            game_results_update();
            for (long index = 0; index < game_cinematic_objects()->count; ++index)
            {
                long object = game_cinematic_objects()->entries[index].object_index;
                if (object != -1 && !object_try_and_get_and_verify_type(object, ~0UL))
                    cinematic_object_destroy_internal(index);
            }
            g_render_postprocess_color_tweaking = *g_screenshot_hue_saturation;
            hs_runtime_update();
            *g_screenshot_hue_saturation = g_render_postprocess_color_tweaking;
            game_pvs_update();
            game_time_update_water_ripples();
            object_activation_regions_update();
            objects_update();
            objects_flush_tracked_accelerations();
            havok_proxies_update();
            havok_update();
            havok_proxies_move();
            objects_move();
            objects_post_update();
            havok_entity_post_update();
            breakable_surfaces_update();
            effects_update();
            lights_update();
            game_engine_update_after_game();
            players_update_after_game(&update);
            campaign_metagame_update();
            game_allegiance *entry = tls_game_allegiance_globals()->allegiances;
            for (short index = 0; index < tls_game_allegiance_globals()->count; ++index, ++entry)
            {
                if (entry->timer > 0 && --entry->timer == 0)
                {
                    if (--entry->incidents == 0)
                        game_allegiance_broken(entry, false, false, true);
                    else
                        entry->timer = entry->delay;
                }
            }
            game_loss_update();
            if (game_globals->finished && game_globals->finish_ticks > 0)
            {
                long ticks = --game_globals->finish_ticks;
                if (!game_globals->finish_immediate && !(game_time_globals->tick_length * (float)ticks > 2.0f))
                {
                    if (!game_is_cooperative())
                        game_sound_disable_at_game_finish();
                    game_globals->finish_immediate = true;
                }
            }
            game_save_update();
            cinematic_update();
            g_depth_of_field_game_globals->update();
            game_update_grief_decay();
            for (long user = 0; user < 4; ++user)
                if (first_person_weapon_attach(user, game_tick_user_unit(user), false))
                    first_person_weapon_update(user);
            player_effect_update();
            s_player_globals *observer = observer_get_globals();
            if (observer->flag) observer->flag = false;
            if (observer->block_one_frame) observer->block_one_frame = false;
            scripted_camera_game_tick();
        }
        else
            objects_flush_tracked_accelerations();
        c_simulation_world::distribute_update(&update, &metadata);
        if ((static_cast<unsigned short>(update.flags) & 1u) != 0u)
            ++game_time_globals->time;
        game_tick_destroy_update(&update);
    }
    main_set_status_value(status_name, 0);
}

static __forceinline bool game_options_decode_validate_map(c_map_variant *variant)
{
    bool valid = map_variant_is_valid((const s_map_variant_validation *)variant);
    if (!valid)
    {
        variant->create_default((e_map_id)variant->map_id);
        valid = true;
    }
    return valid;
}
// @address 0x1b8100 game_options_decode
extern "C" unsigned long game_options_decode(c_bitstream *packet, game_options *options)
{
    options->game_mode = packet->read_integer("game-mode", 3);
    options->game_simulation = (signed char)packet->read_integer("game-simulation", 3);
    options->game_network_type = (signed char)packet->read_integer("game-network-type", 2);
    options->game_tick_rate = (short)packet->read_integer("game-tick-rate", 6);
    packet->read_raw_data("game-instance", options->random_data, 64);
    options->random_seed = packet->read_integer("random-seed", 32);
    options->language = packet->read_integer("language", 4);
    options->determinism_version = packet->read_integer("determinism-version", 8);
    options->campaign_id = (e_campaign_id)packet->read_integer("campaign-id", 32);
    options->map_id = (e_map_id)packet->read_integer("map-id", 32);
    packet->read_string("scenario-path", options->scenario_path, 260);
    options->initial_zone_set_index = (short)packet->read_integer("initial-zone-set", 6);
    bool success = true;
    success = success && game_options_decode_campaign_armaments(packet, &options->campaign_armaments);
    options->load_level_only = packet->read_bool("load-level-only");
    options->dump_machine_index = (signed char)(packet->read_integer("campaign-insertion", 5) - 1);
    options->dump_object_log = packet->read_bool("dump-object-log");
    options->dump_random_seeds = packet->read_bool("dump-random-seeds");
    options->survival_enabled = packet->read_bool("survival-enabled");
    options->game_playback = (short)packet->read_integer("game-playback", 2);
    options->record_saved_film = packet->read_bool("record-saved-film");
    options->playback_start_in_ticks = packet->read_integer("playback-start", 32);
    options->playback_length_in_ticks = packet->read_integer("playback-length", 32);
    options->campaign_difficulty = (short)packet->read_integer("campaign-difficulty", 2);
    options->campaign_insertion_point = (short)packet->read_integer("campaign-insertion-point", 2);
    options->campaign_metagame_scoring = (short)packet->read_integer("campaign-metagame-scoring", 2);
    options->campaign_metagame_enabled = packet->read_bool("campaign-metagame-enabled");
    options->survival_mode_enabled = packet->read_bool("survival-mode-enabled");
    options->primary_skulls = packet->read_integer("primary-skulls", 32);
    options->secondary_skulls = packet->read_integer("secondary-skulls", 32);
    options->matchmade_game = packet->read_bool("matchmade-game");
    packet->read_raw_data("campaign-progress", &options->matchmaking_options, 736);
    if (packet->read_bool("game-variant-present"))
    {
        success = success && game_variant_decode(&options->game_variant, packet);
        success = success && options->game_variant.engine_type != 0;
        success = success && game_engine_variant_is_valid(&options->game_variant);
    }
    else
        options->game_variant.set_game_engine_index(_game_engine_base_variant);
    success = success && map_variant_decode(&options->map_variant, packet) != 0;
    const unsigned long decoded = (unsigned char)(success && game_options_decode_validate_map(&options->map_variant));
    BLAM_ASSUME(decoded <= 1);
    packet->read_raw_data("machines", &options->machine_valid_mask, 864);
    packet->read_raw_data("players", options->players, 37888);
    if (!game_options_verify(options, 0, 0))
        return false;
    return decoded;
}

t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_game_globals_allocator> g_game_globals_allocator;
