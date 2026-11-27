// blamlibxbox/game_engine.obj: from the records 0x80000 0xa7888 0xc4020 0xf5268 0xf6048 0x112380 0x117508 0x12c708 0x12cc30 0x12cd58 0x12f628 0x1ceb10 0x1ced38 0x1d2e28 0x200d68 0x2025e8 0x205e80 0x205e90 0x205fc8 0x2063d8 0x2064e0 0x206560 0x206868 0x2068d8 0x206c00 0x206ce0 0x206d98 0x206e50 0x206f00 0x206f90 0x207080 0x207148 0x2071f0 0x207248 0x2072b0 0x207368 0x2074a8 0x2075a8 0x207780 0x2077f0 0x207890 0x2078f8 0x207960 0x207a78 0x207b38 0x207bf0 0x208008 0x208080 0x2084a0 0x208550 0x2085c8 0x208668 0x2086c8 0x208758 0x208bb0 0x208c38 0x208d20 0x208da0 0x208e80 0x208f08 0x209018 0x209148 0x2094c8 0x209710 0x209a00 0x209dc8 0x209ef0 0x209f60 0x20a158 0x20a238 0x20a290 0x20a3e8 0x20a480 0x20a4b8 0x20a500 0x20a588 0x20a6c0 0x20aa68 0x20ac98 0x20af70 0x20b4b8 0x20bbf8 0x20bd50 0x20bd88 0x20bf98 0x20c140 0x20c1a8 0x20c330 0x20c4a0 0x20c538 0x20c620 0x20c6a8 0x20c798 0x20c8a8 0x20cb18 0x20cc50 0x20cd78 0x20cdf8 0x20ce68 0x20cf08 0x20cf40 0x20cfe0 0x20cff8 0x20d018 0x29acb8 0x350f90 0x46f498 0x479060 0x491c10
// @flags /TP /O2
#include "units/unit_seat_flags.h"
#include "units/units_runtime_dependencies.h"
#include "game/game_engine_current_entry.h"
#include "objects/object_header.h"
#include "memory/data_absolute_index.h"
#include "simulation/simulation_watcher_types.h"
#include "game/game_results_storage_types.h"
#include "game/player_control_globals.h"
#include "objects/light_globals.h"
#include "cseries/cseries_unicode.h"
#include "game/game_engine_local_globals.h"
#include "game/game_grief.h"
#include "game/game_grief_types.h"
#include "memory/tls_data_arrays.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "math/real_math_globals.h"
#include "game/player_with_unit_iterator.h"
#include "game/game_time_inline.h"
#include "game/game_engine_score_types.h"
#include <time.h>
#include "cseries/cseries_types.h"
#include <stdio.h>
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "objects/light_datum.h"
#include "devices/device_group.h"
#include "objects/object_definitions.h"
#include "ai/weapon_datum_types.h"
#include "items/multiplayer_weapon_tracker.h"
#include "items/item_types.h"
#include "items/equipment.h"
#include "interface/gui_widget_types.h"
#include "interface/chud/chud_navpoints_runtime.h"
#include "game/map_variant.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include <stdarg.h>
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/game_engine_globals_messages.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "objects/objects.h"
#include "objects/object_lifecycle.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "simulation/simulation.h"
#include "sound/game_sound.h"
#include "text/unicode.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "devices/devices.h"
#include "effects/effects.h"
#include "game/game_engine_candy_monitor.h"
#include "game/game_engine_notifications.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_scoring.h"
#include "scenario/scenario_tag.h"
#include "objects/object_reference_types.h"
#include "networking/online/voice_interface_types.h"
#include "interface/user_interface_messages.h"
#include "interface/chud/chud_navpoint_types.h"
#include "game/player_traits.h"
#include "game/game_types.h"
#include "cache/restricted_memory.h"
#include "game/game_engine_simulation.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "hs/hs_runtime.h"
#include "items/weapons.h"
#include "main/main.h"
#include "networking/online/voice_xenon.h"
#include "objects/lights.h"
#include "objects/object_placement.h"
#include "physics/breakable_surfaces.h"
#include "render/render_objects_static_lighting.h"
#include "game/game_engine_display.h"
#include "interface/user_interface_controller.h"
#include "motor/action_system.h"
#include "networking/logic/network_session_interface.h"
#include "scenario/scenario_map_variant.h"
#include "game/game_globals.h"
#include "game/player_types.h"
#include "game/player_access.h"
#include "camera/observer_director.h"
#include "game/game_engine_multiplayer_sounds.h"
#include "game/game_engine_teleporters.h"
#include "saved_games/saved_film.h"
#include "game/game_allegiance.h"
#include "game/game_engine_sandbox.h"
#include "networking/network_utilities.h"
#include "cache/restricted_memory_layout.h"
#include "game/game_engine_default.h"
#include "game/game_variant.h"
#include "interface/c_window_manager.h"
#include "game/game_engine_event_data.h"
#include "simulation/simulation_globals.h"
#include "simulation/simulation_connection_statistics_types.h"
#include "networking/transport/transport_security_globals.h"
#include "networking/network_globals.h"
#include "game/statborg_entry_types.h"
#include "cache/cache_files.h"
#include "camera/observer.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "game/player_mapping_types.h"
#include "cseries/data_mining_event_types.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_globals_storage.h"
#include "items/weapon_definitions.h"
#include "game/game_engine_list.h"
#include "game/game_engine_ctf.h"
#include "game/game_engine_slayer.h"
#include "game/game_engine_oddball.h"
#include "game/game_engine_king.h"
#include "game/game_engine_sandbox_engine.h"
#include "game/game_engine_vip.h"
#include "game/game_engine_juggernaut.h"
#include "game/game_engine_territories.h"
#include "game/game_engine_assault.h"
#include "game/game_engine_infection_types.h"
#include "game/game_engine_runtime_externs.h"
#include "input/input_xenon.h"
#include "game/game_variant_inline.h"
#include "game/player_in_game_iterator.h"
#include "memory/data_iterator_noalias.h"
#include "game/game_results_types.h"
#include "game/game_engine_string_types.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "scenario/scenario.h"
#include <ppcintrinsics.h>
#include "networking/session/network_session.h"
#include "game/game_engine_message_queue_types.h"
#include "multithreading/interlocked_slist.h"
#include "multithreading/interlocked_slist_push.h"
#include "camera/director_storage_types.h"
#include "game/game_engine_candy_monitor_types.h"
#include "game/game_engine_end_condition.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "game/player_control_types.h"
#include "networking/online/voice_observer_types.h"
#include "multithreading/thread_tls.h"
#include "game/game_engine_teleporter_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "simulation/simulation_world_types.h"
#include "game/player_color_types.h"
#include "game/network_player_colors.h"
#include "game/game_engine_interface_state_types.h"
#include "input/input.h"
#include "game/game_engine_respawn_types.h"
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "tag_files/string_ids.h"
#include <new>

// @address 0x1142940 g_window_manager

// @address 0x7c5f00 ?game_engines@@3PAPAVc_game_engine@@A
c_game_engine *game_engines[11] = {
    0,
    &internal_ctf_engine,
    &internal_slayer_engine,
    &internal_oddball_engine,
    &internal_king_engine,
    &internal_sandbox_engine,
    &internal_vip_engine,
    &internal_juggernaut_engine,
    &internal_territories_engine,
    &internal_assault_engine,
    &internal_infection_engine,
};

enum e_statborg_entry;

enum { k_maximum_teams = 8 };

/* ---- game engines and variants ------------------------------------------------ */
template<long size> class c_static_wchar_string;
struct game_engine_interface_state;


// @address 0xf03be0 g_game_engine_score_cache

/* ---- game globals (tls game_globals) ------------------------------------------ */

/* ---- objects, tags, owners ---------------------------------------------------- */


/* ---- game engine events ----------------------------------------------------------- */
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z

enum { _weapon_multiplayer_inventory_flag = 7 };

struct s_player_waypoint_timers { s_equipment_slot timers[8]; };

/* ---- c_game_statborg (game_engine_globals + 0xfea8; same layout as game_engine_scoring.cpp) ----- */
// @address 0x29ad18 ?stats_reset_for_round_switch@c_game_statborg@@QAAXXZ
// @address 0x29b5e8 ?adjust_player_stat@c_game_statborg@@QAAXJW4e_statborg_entry@@FJ_N@Z

/* ---- globals ---------------------------------------------------------------- */

// @address 0x808560 g_simulation_globals

/* player iterators: layout as players.cpp, whose next() serves both */


/* ---- functions declared here and defined in another object ------------------- */
// @address 0x82370 _vsnprintf

// @address 0x277ee8 ?game_engine_has_teams@@YA_NXZ
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xbb2a8 ?angle_between_vectors3d@@YAMPBTreal_vector3d@@0@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// match: noalias declaration preserves caller live registers
// @address 0x192420 ?unlock_resources_and_resume_render_thread@@YAXK@Z
// @address 0x3aa0a0 ?simulation_action_game_engine_player_delete@@YAXF@Z

/* ---- functions of this unit ------------------------------------------------------ */

/* ---- helpers ---------------------------------------------------------------- */

static __forceinline c_game_engine *get_current_entry(void)
{
    if (game_engine_globals->game_engine_index >= 1 && game_engine_globals->game_engine_index <= 10)
        return game_engines[game_engine_globals->game_engine_index];
    return 0;
}

/* the statborg test of game_engine_get_statborg and game_engine_get_team_score */
static __forceinline c_game_statborg *game_engine_statborg_in_range(void)
{
    s_game_engine_globals *state = game_engine_globals;
    c_game_statborg *result = 0;
    if (state->game_engine_index >= 1 && state->game_engine_index <= 10 && game_engines[state->game_engine_index])
        result = &state->statborg;
    return result;
}

/* the statborg test of game_engine_get_player_kills */
static __forceinline c_game_statborg *game_engine_statborg_if_current(void)
{
    c_game_statborg *result = 0;
    if (current_game_engine_entry())
        result = &game_engine_globals->statborg;
    return result;
}

static __forceinline bool game_is_finished(void)
{
    return game_globals->game_finished_state;
}

static __forceinline int game_team_out_of_range(long team)
{
    if (team < 0)
        return 1;
    if (team >= k_maximum_teams)
        return 1;
    return 0;
}

static __forceinline bool game_engine_team_activity_valid(void)
{
    return current_game_engine_entry() && TEST_BIT(game_engine_globals->flags, 3);
}

static __forceinline bool team_is_ever_active(long team)
{
    return game_engine_team_activity_valid() && !game_team_out_of_range(team) &&
        TEST_BIT(game_engine_globals->ever_active_teams, team);
}

static __forceinline bool c_data_iterator_advance(c_data_iterator<void> *it)
{
    it->m_datum = data_iterator_next(&it->m_iterator);
    return it->m_datum != 0;
}

static __forceinline c_candy_monitor *entry_get(long id)
{
    c_multiplayer_candy_monitor_manager *table = &game_engine_globals->candy_monitor_manager;
    c_candy_monitor *result = 0;
    if (id != -1) {
        for (long i = 0; i < 512; i++) {
            if (table->monitors[i].object_index == id) {
                result = &table->monitors[i];
                break;
            }
        }
    }
    return result;
}

static __forceinline bool thread_state_test(void)
{
    bool result = false;
    if (game_globals->options.game_simulation == 4)
        result = true;
    return result;
}

// @address 0x244bb8 ?game_results_statistic_increment@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z

// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ
// @address 0x27b730 ?game_engine_limited_lives_active@@YA_NXZ

static __forceinline bool team_designator_is_allowed(long designator)
{
    bool allowed = true;
    if (current_game_engine_entry())
        allowed = allowed && current_game_engine()->validate_team_designator_for_new_map((e_multiplayer_team_designator)designator);
    return allowed;
}


// @address 0x9e6c80 g_game_engine_globals_message_queue
s_list_globals g_game_engine_globals_message_queue;


static __forceinline s_pending_entry *list_reverse(s_pending_entry *list)
{
  s_pending_entry *result = list;
  if (list) {
    s_pending_entry *previous = 0;
    do {
      s_pending_entry *next = list->next;
      list->next = previous;
      previous = list;
      list = next;
    } while (list);
    result = previous;
  }
  return result;
}

// @address 0x80000 process_game_engine_globals_messages
void process_game_engine_globals_messages(void)
{
  s_pending_entry *list = (s_pending_entry *)InterlockedFlushSList(&g_game_engine_globals_message_queue.pending);
  __lwsync();
  s_pending_entry *entry = list_reverse(list);
  while (entry) {
    if (entry->value)
      game_engine_globals->pending_flags |= 1 << entry->bit;
    else
      game_engine_globals->pending_flags &= ~(1 << entry->bit);
    s_pending_entry *next = entry->next;
    __lwsync();
    InterlockedPushEntrySList(&g_game_engine_globals_message_queue.free, entry);
    entry = next;
  }
}


void game_engine_update_player_sitting_out(void)
{
    if (!thread_state_test()) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next()) {
            player_datum *player = iterator.m_iterator.m_datum;
            if (TEST_BIT(player->flags, 12)) {
                player->flags &= ~0x1000;
                ::simulation_action_game_engine_player_update((short)iterator.get_index(), 0x4000);
            }
        }
    }
}

bool game_engine_running(void)
{
    return get_current_entry() != 0;
}

c_game_statborg *game_engine_get_statborg(void)
{
    return game_engine_statborg_in_range();
}

// @address 0x112380 game_engine_update_user_directors
void game_engine_update_user_directors(long object_index)
{
    for (long user_index = 0; user_index < 4; user_index++)
        g_director_globals->directors[user_index].get_director()->handle_deleted_object(object_index);
}

/* natural 0x117508 */
enum e_game_engine_end_condition;
class c_game_results;


static __forceinline void game_engine_set_desired_state(long state)
{
    if (!thread_state_test())
        game_engine_globals->desired_game_engine_state = state;
}

// @address 0x117508 ?game_engine_update_after_game@@YAXXZ
void game_engine_update_after_game(void)
{
    if (!get_current_entry() || thread_state_test())
        return;
    if (!game_is_finished())
    {
        switch ((short)game_engine_globals->game_engine_state)
        {
        case 2:
            if (--game_engine_globals->state_change_timer <= 0)
            {
                if (game_engine_globals->round_index < 31)
                {
                    game_engine_set_desired_state(3);
                    game_engine_globals->candy_monitor_manager.dispose_from_old_round();
                }
                else
                {
                    game_engine_end_game(game_engine_end_condition_game_end_exceeded_maximum_rounds);
                }
            }
            break;
        case 3:
            if (--game_engine_globals->state_change_timer <= 0)
                game_engine_set_desired_state(1);
            break;
        }
        game_engine_server_respond_to_new_state();
        return;
    }
    if (game_engine_globals->game_finished)
        return;
    game_engine_globals->game_finished = 1;
    simulation_action_game_engine_globals_update(8);
    if ((short)game_engine_globals->game_engine_state == 1)
    {
        if (game_engine_globals->desired_game_engine_state == 1)
        {
            game_engine_end_round_with_winner(game_engine_choose_winner_index(), false, game_engine_end_condition_game_end_external);
        }
        if (game_engine_globals->desired_game_engine_state != 1)
            game_engine_server_respond_to_new_state();
    }
    game_engine_reset_stats_for_round_switch();
    s_game_engine_event_data event_data;
    game_engine_initialize_event(0, 19, &event_data);
    game_engine_send_event(&event_data);
    if (g_game_results_globals.active)
    {
        bool finalized = g_current_game_results.initialized && g_current_game_results.finalized;
        if (!finalized)
        {
            c_game_statborg *statborg = game_engine_get_statborg();
            if (statborg)
            {
                statborg->m_stats_started = false;
                game_engine_get_statborg()->m_finalized = true;
                simulation_action_game_statborg_update(0x1000000);
            }
            g_game_results_globals.finished = false;
            game_engine_update_score_and_standing();
            long host_index = g_current_game_results.get_host_machine_index();
            g_current_game_results.finish_valid = true;
            g_current_game_results.finish_value = (long)_time64(0);
            if (host_index == -1)
            {
                long machine_index = g_current_game_results.get_machine_index(&g_transport_security_globals.local_machine_identifier);
                for (long i = 0; i < 16; i++)
                {
                    if (g_current_game_results.entries[i].valid)
                        g_current_game_results.entries[i].host = (i == machine_index);
                }
            }
            g_game_results_globals.finished = true;
            g_current_game_results.finalized = true;
            game_results_grant_top_player_achievement(&g_current_game_results);
        }
    }
}

/* natural 0x12f628 */
bool game_engine_player_has_shield(long player_index)
{
    bool result = true;
    if (game_engine_running() && player_index != -1)
    {
        player_datum *player = player_get(player_index);
        if (player->traits.m_shield_vitality_traits.get_maximum_shield_vitality() <= 0.0f)
            result = false;
    }
    return result;
}

/* natural 0x1ceb10: no March name or pairing is available. */

// @address 0x1ceb10 game_engine_set_map_info_name
void game_engine_set_map_info_name(s_game_engine_map_info *info, const word *source)
{
    word *name = (word *)info->metadata.name;
    const word *character = source;
    for (long index = 0; index < 4; index++)
        ((__int64 *)name)[index] = 0;
    for (long index = 0; index <= 15; index++, character++)
    {
        name[index] = *character;
        if (name[index] == 0)
            break;
    }
    name[15] = 0;
    info->m_built_in = 0;
}

 /* natural 0x2063d8 */
void game_engine_build_allowable_team_designators(void)
{
    if (current_game_engine_entry() && game_engine_has_teams()) {
        word allowable_designators = 0;
        for (long designator = 0; designator < 8; designator++) {
            bool allowed = team_designator_is_allowed(designator);
            if (allowed && !TEST_BIT(allowable_designators, designator))
                allowable_designators |= 1 << designator;
        }
        game_engine_globals->allowable_team_designators = allowable_designators;
    }
}

void game_engine_reset_player_unit_timers_for_new_round(void)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next()) {
        iterator.m_iterator.m_datum->unitless_countdown_ticks = 0;
        iterator.m_iterator.m_datum->ticks_with_unit = -1;
    }
}

static __forceinline int current_index(void) { return game_engine_globals->game_engine_index; }

// @address 0x206868 ?game_engine_dispose_from_old_map@@YAXXZ
void game_engine_dispose_from_old_map(void)

{
  int game_engine_index;
  
  game_engine_index = game_engine_globals->game_engine_index;
  if ((current_index() >= 1) && (current_index() <= 10)) {
    if (game_engines[game_engine_index] != 0) {
      game_engines[game_engine_index]->dispose_from_old_map();
      game_engine_globals->game_engine_index = 0;
    }
  }
  return;
}


extern "C" {
/* natural 0x206ce0 */
// @address 0x206ce0 game_engine_get_scores_player_score
long game_engine_get_scores_player_score(long player_index, bool combined)
{
    long result = 0;
    if (current_game_engine_entry()) {
        c_game_statborg *scores = &game_engine_globals->statborg;
        if (scores) {
            if (combined) {
                short value = scores->teams[player_index].round_locked ? scores->teams[player_index].total
                    : scores->teams[player_index].total + scores->teams[player_index].pending;
                result = value;
            }
            else
                result = scores->teams[player_index].pending;
        }
    }
    return result;
}

}
// @address 0x206d98 ?game_engine_get_player_score@@YAJJ_N@Z
long game_engine_get_player_score(long team_index, bool combined)
{
    long result = 0;
    if (current_game_engine_entry()) {
        s_statborg_player *teams = game_engine_globals->statborg.players;
        if (teams) {
            if (combined) {
                short value = teams[datum_index_to_absolute_index(team_index)].round_locked ? teams[datum_index_to_absolute_index(team_index)].total
                    : teams[datum_index_to_absolute_index(team_index)].total + teams[datum_index_to_absolute_index(team_index)].pending;
                result = value;
            }
            else
                result = teams[datum_index_to_absolute_index(team_index)].pending;
        }
    }
    return result;
}


long game_engine_get_player_kills(long player_index)
{
    long kills = 0;
    if (game_engine_statborg_if_current())
        kills = game_engine_get_statborg()->get_player_stat(player_index, game_is_finished());
    return kills;
}

extern "C" {
// @address 0x206f00 game_engine_get_team_score
long game_engine_get_team_score(unsigned short team)
{
    long score = 0;
    if (game_engine_statborg_in_range())
        score = game_engine_get_statborg()->m_player_stats[team][2];
    return score;
}
}

/* natural 0x206f90 */
long game_engine_get_team_rounds_won(e_game_team team)
{
    long rounds_won = -1;
    if (game_engine_statborg_if_current() && game_engine_has_teams() && game_engine_is_team_ever_active((unsigned int)team))
        rounds_won = game_engine_get_statborg()->get_team_stat(team, game_is_finished());
    return rounds_won;
}

long game_engine_get_score_to_win_round(void)
{
    long result = 0;
    const c_game_engine *engine = current_game_engine_entry();
    if (engine)
        result = engine->get_score_to_win_round();
    else if (current_game_variant())
        result = current_game_variant()->get_active_variant()->get_score_to_win_round();
    return result;
}

// @address 0x2071f0 game_engine_player_is_awaiting_respawn
bool game_engine_player_is_awaiting_respawn(unsigned long player_index)
{
    player_datum *player = player_get(player_index);
    return (player->unit_index == -1 && player->respawn_timer <= 0) || TEST_BIT(player->flags, 3);
}

// match: 0x207248: unchecked-divide form keeps plain divw for ticks / game_tick_rate()
#pragma optimize("c", on)
/* natural 0x207248 */
extern "C" {
// @address 0x207248 game_engine_player_set_respawn_timer
void game_engine_player_set_respawn_timer(long player_index, long ticks)
{
    short handle = (short)player_index;
    player_datum *player = player_get(player_index);
    player->respawn_timer = ticks;
    player->respawn_seconds = ticks / game_tick_rate();
    player->respawn_seconds = MIN(MAX(player->respawn_seconds, 0), 0x3ff);
    ::simulation_action_game_engine_player_update(handle, 0x800);
}
}

// match: 0x207248: restore default lowering after unchecked-divide region
#pragma optimize("c", off)

bool game_engine_player_is_playing(long player_index)
{
    player_datum *player = player_get(player_index);
    bool playing = false;
    if (TEST_BIT(player->flags, 0) && !TEST_BIT(player->flags, 1) &&
        player->configuration.host.team_index != -1) {
        if (player->unit_index != -1) {
            playing = true;
        } else if (!TEST_BIT(player->flags, 12) && !game_engine_player_is_out_of_lives(player_index)) {
            playing = true;
        }
    }
    return playing;
}

/* natural 0x2074a8 */
void get_living_or_connecting_team_count(long *player_index, long *playing_team_count, long *living_team_count)
{
    long playing_teams = 0;
    long living_teams = 0;
    long playing_count = 0;
    long living_count = 0;
    c_data_iterator<void> iterator;
    iterator.begin(player_data);
    while (c_data_iterator_advance(&iterator)) {
        player_datum *player = (player_datum *)iterator.m_datum;
        dword flags = player->flags;
        long team = player->configuration.host.team_index;
        if (!TEST_BIT(flags, 1) && (dword)team <= 15) {
            long team_bit = 1 << team;
            if ((team_bit & living_teams) == 0) {
                living_teams = team_bit | living_teams;
                living_count = living_count + 1;
            }
            long index = iterator.m_iterator.index;
            if (!game_engine_player_is_playing(index) && TEST_BIT(flags, 0))
                continue;
            *player_index = index;
            if ((team_bit & playing_teams) == 0) {
                playing_teams = team_bit | playing_teams;
                playing_count = playing_count + 1;
            }
        }
    }
    if (living_team_count)
        *living_team_count = living_count;
    if (playing_team_count)
        *playing_team_count = playing_count;
}

short get_ever_active_team_or_player_count(void)
{
    long count = 0;
    if (game_engine_has_teams()) {
        for (long team = 0; team < k_maximum_teams; team++) {
            if (team_is_ever_active(team))
                count++;
        }
    } else {
        c_data_iterator<void> iterator;
        iterator.begin(player_data);
        while (c_data_iterator_advance(&iterator))
            count++;
    }
    return (short)count;
}

// @address 0x29e388 ?game_engine_scoring_get_player_compound_rank@@YAJJJ@Z

// @address 0x2075a8 ?game_engine_should_end_round@@YA_NPAJPA_N0@Z
bool game_engine_should_end_round(long *winner, bool *all_dead, long *condition)
{
    bool end_round = false;
    bool dead = false;
    long reason = 0;
    if (current_game_engine_entry())
    {
        long last_player = -1;
        long ever_active = get_ever_active_team_or_player_count();
        long living, connecting;
        get_living_or_connecting_team_count(&last_player, &living, &connecting);
        if (ever_active > 1 && current_game_engine()->get_type() != 5)
        {
            end_round |= living < 2;
            dead |= connecting < 2;
            if (end_round)
                reason = 2;
        }
        else
        {
            end_round = living <= 0;
            dead = connecting <= 0;
            if (end_round)
                reason = 3;
        }
        if (end_round && last_player != -1)
        {
            if (game_engine_has_teams())
                *winner = player_get(last_player)->configuration.host.team_index;
            else
                *winner = last_player;
        }
        if (!end_round)
        {
            const c_game_engine *engine = current_game_engine();
            if (engine->should_end_round(winner))
            {
                end_round = true;
                reason = 4;
            }
        }
    }
    if (all_dead)
        *all_dead = dead;
    if (condition)
        *condition = reason;
    return end_round;
}

/* natural 0x207780: no March name or pairing is available. */

// @address 0x207780 game_engine_get_winning_team
long game_engine_get_winning_team(void)
{
    long result = 8;
    for (long index = 0; index < 8; index++)
    {
        if (game_engine_scoring_get_team_compound_rank(index, game_globals->game_finished_state != 0) == 0)
        {
            result = index;
            break;
        }
    }
    return result;
}

/* natural 0x2077f0: no March name or pairing is available. */

// @address 0x2077f0 game_engine_get_winning_player
long game_engine_get_winning_player(void)
{
    long result = -1;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        if (game_engine_scoring_get_player_compound_rank(iterator.get_index(), game_globals->game_finished_state != 0) == 0)
        {
            result = iterator.get_index();
            break;
        }
    }
    return result;
}

long game_engine_get_player_place(long player_index)
{
    long place = game_engine_scoring_get_player_compound_rank(player_index, game_globals->game_finished_state != 0);
    if (place == -1)
        return place;
    return place / 2;
}

static __forceinline long game_engine_are_players_tied_place(long player_index)
{
    long place = game_engine_scoring_get_player_compound_rank(player_index, game_globals->game_finished_state != 0);
    if (place == -1)
        return place;
    return place / 2;
}

/* natural 0x2078f8: no March name or pairing is available. */
// @address 0x2078f8 game_engine_find_team_place
long game_engine_find_team_place(long team)
{
    long place = game_engine_scoring_get_team_compound_rank(team, game_globals->game_finished_state != 0);
    if (place == -1)
        return place;
    return place / 2;
}

/* game_engine_get_team_place (0x2078f8), inlined into game_engine_are_teams_tied */
static __forceinline long game_engine_get_team_place_inline(long team)
{
    long mode = game_globals->game_finished_state != 0;
    long place = game_engine_scoring_get_team_compound_rank(team, mode);
    if (place == -1)
        return place;
    return place / 2;
}

/* 0x207960 */
bool game_engine_are_teams_tied(void)
{
    bool tied = true;
    for (long team = 0; team < k_maximum_teams; team++)
    {
        if (team_is_ever_active(team) && game_engine_get_team_place_inline(team) > 0)
        {
            tied = false;
            break;
        }
    }
    return tied;
}

bool game_engine_are_players_tied(void)
{
    long player_count = 0;
    bool tied = true;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        player_count = player_count + 1;
        if (game_engine_are_players_tied_place(iterator.get_index()) > 0)
        {
            tied = false;
            break;
        }
    }
    if (player_count == 1)
        tied = false;
    return tied;
}

/* natural 0x207b38: no March name or pairing is available. */
// @address 0x207b38 game_engine_get_player_winning_status
// match: 0x207b38: pointer __restrict qualifier prevents alias reload
void game_engine_get_player_winning_status(long p0, long *__restrict p1)
{
    player_datum *player = player_get(p0);
    *p1 = 1;
    long tr29 = game_engine_get_winning_team();
    long tr8 = game_engine_get_winning_player();
    if (game_engine_has_teams()) {
        long owner = player->configuration.host.team_index;
        if (tr29 == owner)
            *p1 = 0;
        else if (tr29 == 8)
            *p1 = (game_engine_find_team_place(owner) == 0) + 2;
    } else {
        if (tr8 == p0)
            *p1 = 0;
        else if (tr8 == -1)
            *p1 = (game_engine_get_player_place(p0) == 0) + 2;
    }
}


// @address 0x208008 game_engine_player_grants_infinite_ammo
bool game_engine_player_grants_infinite_ammo(long player_index)
{
    bool result = false;
    if (current_game_engine_entry() && player_get(player_index)->traits.m_weapon_traits.m_infinite_ammo == 2)
        result = true;
    return result;
}

/* natural 0x2084a0 */
// @address 0x2084a0 ?game_engine_get_type@@YAJXZ
long game_engine_get_type(void)
{
    long type = 0;
    if (game_engine_running())
        type = get_current_entry()->get_type();
    return type;
}

bool game_engine_monitoring_object(long id)
{
    return entry_get(id) != 0;
}

bool game_engine_in_round(void)
{
    bool result = false;
    if (current_game_engine_entry() && game_engine_globals->game_engine_state == 1 &&
        (thread_state_test() || game_engine_globals->desired_game_engine_state == 1))
        result = true;
    return result;
}

/* natural 0x208668 */
long game_engine_get_round(void)
{
    if (current_game_engine_entry())
        return game_engine_globals->round_index;
    else
        return 0;
}

/* natural 0x2086c8 */
// @address 0x29e4a8 ?game_engine_scoring_build_score_list@@YAXPAUs_game_engine_score_list@@J_N@Z

long game_engine_choose_winner_index(void)
{
    s_game_engine_score_list score_list;
    game_engine_scoring_build_score_list(&score_list, 0, false);
    if (game_engine_has_teams())
    {
        if (score_list.team_count > 0 && score_list.team_ranks[0] == 0)
            return score_list.team_indices[0];
    }
    else
    {
        if (score_list.player_count > 0 && score_list.player_ranks[0] == 0)
            return score_list.player_indices[0];
    }
    return -1;
}

/* natural 0x208bb0 */
extern "C" {
// @address 0x208bb0 game_engine_adjust_team_score
void game_engine_adjust_team_score(long value, int flag)
{
    c_game_engine *engine = get_current_entry();
    if (engine)
        engine->recompute_team_score((e_game_team)value, flag, (e_team_scoring_method)current_game_variant()->team_scoring_method);
}
}

// @address 0x208c38 ?game_engine_adjust_player_score@@YA_NJJ@Z
/* natural 0x208c38 */
bool game_engine_adjust_player_score(long player_index, long adjustment)
{
    bool result = false;
    if (game_engine_in_round() && player_index != -1) {
        long absolute_index = player_index & 0xffff;
        player_datum *player = player_get(player_index);
        game_engine_get_statborg()->adjust_player_stat(absolute_index, _statborg_entry_score, (short)adjustment, 6, false);
        result = true;
        if (game_engine_has_teams() && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < k_maximum_teams)
            game_engine_adjust_team_score(player->configuration.host.team_index, adjustment);
        for (long i = 0; i < 2; i++)
            g_game_engine_score_cache[i].time = -1;
        game_engine_globals->flags |= 1;
    }
    return result;
}

// @address 0x208d20 ?game_engine_adjust_team_score_for_composition@@YAXW4e_game_team@@@Z
/* natural 0x208d20 */
void game_engine_adjust_team_score_for_composition(e_game_team team)
{
    if (!thread_state_test() && game_engine_in_round() && game_engine_has_teams() && (unsigned long)team <= 7)
        game_engine_adjust_team_score(team, 0);
}

// @address 0x208da0 ?game_engine_end_round_internal@@YAX_N@Z
/* natural 0x208da0 */
void game_engine_end_round_internal(bool skip_event)
{
    game_globals_storage *globals = game_globals;
    if (!thread_state_test())
        game_engine_globals->desired_game_engine_state = 2;
    if (!thread_state_test() && !globals->game_finished_state && !skip_event) {
        s_game_engine_event_data event;
        game_engine_initialize_event(0, 32, &event);
        game_engine_send_event(&event);
    }
}

// @address 0x1b9100 ?game_finish@@YAXXZ
// @address 0x20cb18 ?data_mine_insert_multiplayer_game_options@@YAXPBDPBVc_game_variant@@@Z
// @address 0x20cc50 ?data_mine_insert_multiplayer_statistics@@YAXXZ

// @address 0x208e80 ?game_engine_end_game@@YAXW4e_game_engine_end_condition@@@Z
/* natural 0x208e80 */
void game_engine_end_game(e_game_engine_end_condition game_engine_end_condition)
{
    if (!game_is_finished() && !thread_state_test()) {
        data_mine_insert_multiplayer_game_options("game ending", current_game_variant());
        data_mine_insert_multiplayer_statistics();
        game_finish();
    }
}


// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
static __forceinline bool game_engine_end_round_has_teams_running(void)
{
    return game_engine_running() && (game_engine_globals->flags & 8) != 0;
}
// @address 0x208758 ?game_engine_end_round_with_winner@@YAXJ_NW4e_game_engine_end_condition@@@Z
/* natural 0x208758 */
void game_engine_end_round_with_winner(long winner, bool end_game, e_game_engine_end_condition end_condition)
{
    long winner_score = 0;
    if (winner != -1) {
        if (game_engine_has_teams() && winner >= 0 && winner < 8) {
            game_engine_get_statborg()->adjust_team_stat((e_game_team)winner, _statborg_entry_rounds_won, 1, 5);
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next()) {
                if (iterator.m_iterator.m_datum->configuration.host.team_index == winner) {
                    long player_index = iterator.m_iterator.m_iterator.index;
                    game_engine_get_statborg()->adjust_player_stat(player_index, _statborg_entry_rounds_won, 1, 5, false);
                    if (game_engine_get_current_variant()->engine_type == 2) {
                        const short *stats = game_engine_get_statborg()->m_player_stats[datum_index_to_absolute_index(player_index)];
                        if (stats[3] == 0 && stats[2] >= 15) {
                            long event = 0;
                            ::game_engine_handle_time_messages(1, (short)player_index, player_index, -1, (int *)(&event));
                        }
                    }
                }
            }
            winner_score = game_engine_get_statborg()->get_team_stat(winner, _statborg_entry_rounds_won);
        } else if (datum_try_and_get(player_data, winner)) {
            game_engine_get_statborg()->adjust_player_stat(winner, _statborg_entry_rounds_won, 1, 5, false);
            long absolute_index = winner & 0xffff;
            const short *stats = game_engine_get_statborg()->m_player_stats[absolute_index];
            winner_score = stats[7];
            if (current_game_variant()->engine_type == 2 && stats[3] == 0 && stats[2] >= 15) {
                long event = 0;
                ::game_engine_handle_time_messages(1, absolute_index, winner, -1, (int *)(&event));
            }
        }
    }
    {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next())
            game_results_statistic_increment((short)iterator.m_iterator.m_iterator.index, _game_team_none, _game_results_statistic_rounds_completed, 1);
    }
    if (game_engine_has_teams()) {
        for (long team = 0; team < 8; team++) {
            if (game_engine_is_team_active((e_game_team)team))
                game_results_statistic_increment(-1, (e_game_team)team, _game_results_statistic_rounds_completed, 1);
        }
    }
    long round_limit = (end_condition == 22 || end_condition == 21) ? -1 : current_game_variant()->round_limit;
    long early_victory_win_count = current_game_variant()->early_victory_win_count;
    if (round_limit > 0 && round_limit <= game_engine_globals->round_index + 1) {
        end_condition = game_engine_end_condition_game_end_rounds;
        end_game = true;
    }
    if (early_victory_win_count > 0) {
        if (game_engine_end_round_has_teams_running()) {
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next()) {
                if (game_engine_get_winning_status((e_game_team)iterator.m_iterator.m_datum->configuration.host.team_index) >= early_victory_win_count) {
                    end_condition = game_engine_end_condition_game_end_rounds_team_early_victory;
                    end_game = true;
                    break;
                }
            }
        } else {
            end_condition = game_engine_end_condition_game_end_rounds_early_victory;
            end_game = winner_score >= early_victory_win_count;
        }
    }
    if (game_engine_globals->round_index >= 31) {
        end_condition = game_engine_end_condition_game_end_rounds_exceeded;
        end_game = true;
    }
    if (end_game) {
        game_engine_end_round_internal(true);
        game_engine_end_game(end_condition);
    } else {
        game_engine_end_round_internal(false);
    }
    for (long i = 0; i < 2; i++)
        g_game_engine_score_cache[i].time = -1;
    game_engine_globals->flags |= 1;
}

// match: 0x208f08: unchecked-divide form keeps plain divw for game_time_get() % game_tick_rate()
#pragma optimize("c", on)
// @address 0x208f08 ?game_engine_update_team@@YAXW4e_game_team@@@Z
/* natural 0x208f08 */
void game_engine_update_team(e_game_team team)
{
    if (!thread_state_test() && game_engine_in_round())
    {
        if (game_time_get() % game_tick_rate() == 0)
        {
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next())
            {
                if (iterator.get_datum()->configuration.host.team_index == team && iterator.get_datum()->unit_index != -1)
                {
                    game_results_statistic_increment(-1, team, _game_results_statistic_seconds_alive, 1);
                    break;
                }
            }
        }
    }
}

// match: 0x208f08: restore default lowering after unchecked-divide region
#pragma optimize("c", off)

// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
namespace {
static __forceinline bool game_engine_session_is_established(const c_network_session *session)
{
    return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}
static __forceinline bool game_engine_session_is_offline(const c_network_session *session)
{
    return game_engine_session_is_established(session) && session->m_session_class == 0;
}
}

/* natural 0x209018: no March name or pairing is available. */
// @address 0x209018 game_engine_try_and_get_betrayer
extern "C" long game_engine_try_and_get_betrayer(unsigned long player_index, long *respawn_player_index)
{
    long result = 0;
    player_datum *player = player_get(player_index);
    if (player->unit_index == -1 && player->respawn_player_index != -1 && !game_is_cooperative()) {
        if (!game_engine_session_is_offline(network_life_cycle_globals_get()->manager.m_active_squad_session)) {
            player_datum *respawn_player = (player_datum *)datum_try_and_get(player_data, player->respawn_player_index);
            if (respawn_player && respawn_player->configuration.client.is_griefer &&
                player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player->respawn_player_index)] == -1 &&
                !TEST_BIT(respawn_player->flags, 1) &&
                !game_team_is_enemy((e_game_team)player->configuration.host.team_index, (e_game_team)respawn_player->configuration.host.team_index)) {
                result = 1;
                *respawn_player_index = player->respawn_player_index;
            }
        }
    }
    return result;
}

/* natural 0x209ef0 */
bool game_engine_player_is_out_of_lives(long player_index)
{
    bool out_of_lives = false;
    if (game_engine_limited_lives_active()) {
        player_datum *player = player_get(player_index);
        if (player->unit_index == -1 && player->count == 0)
            out_of_lives = true;
    }
    return out_of_lives;
}

void game_engine_reset_stats_for_round_switch(void)
{
    if (game_engine_statborg_in_range())
        game_engine_get_statborg()->stats_reset_for_round_switch();
    c_game_engine *engine = get_current_entry();
    if (engine)
        engine->stats_reset_for_round_switch();
    for (long i = 0; i < 2; i++)
        g_game_engine_score_cache[i].time = -1;
    game_engine_globals->flags |= 1;
}


/* global_identity4x3 */

void game_engine_multiplayer_weapon_reset(long weapon_index, const real_point3d *position)
{
    if (current_game_engine_entry())
    {
        weapon_datum *weapon = (weapon_datum *)object_get(weapon_index);
        bool in_inventory = TEST_BIT(weapon->flags, _weapon_multiplayer_inventory_flag);
        if (in_inventory)
            unit_force_drop_inventory_weapon(weapon->item.unit_object_index, weapon_index, true, 0);
        object_set_position_direct(weapon_index, position, &global_identity4x3->forward, &global_identity4x3->up, 0, false);
        object_reset_after_teleport(weapon_index);
        render_object_invalidate_render_state(weapon_index);
        weapon->item.relevance_time = -1;
        weapon->item.toss_unit_index = -1;
    }
}

/* The last entry with this identifier, or NULL. */
s_multiplayer_weapon_tracker *game_engine_multiplayer_weapon_get_internal(long identifier)
{
    s_multiplayer_weapon_tracker *result = 0;
    s_game_engine_globals *globals = game_engine_globals;
    for (long i = 0; i < globals->multiplayer_weapon_count; i++) {
        if (globals->multiplayer_weapons[i].weapon_index == identifier) {
            result = &globals->multiplayer_weapons[i];
        }
    }
    return result;
}

const s_multiplayer_weapon_tracker *game_engine_multiplayer_weapon_get(short id)
{
    s_multiplayer_weapon_tracker *result = 0;
    if (get_current_entry() != 0) {
        s_game_engine_globals *globals = game_engine_globals;
        for (int i = 0; i < globals->multiplayer_weapon_count; i++) {
            if (globals->multiplayer_weapons[i].id == id) {
                result = &globals->multiplayer_weapons[i];
                break;
            }
        }
    }
    return result;
}


void game_engine_multiplayer_weapons_delete_all(void)
{
    long weapon_indices[8];
    long weapon_count = 0;
    for (long i = 0; i < game_engine_globals->multiplayer_weapon_count; i++)
        weapon_indices[weapon_count++] = game_engine_globals->multiplayer_weapons[i].weapon_index;
    for (long i = 0; i < weapon_count; i++)
    {
        game_engine_multiplayer_weapon_deregister(weapon_indices[i]);
        object_delete(weapon_indices[i]);
    }
}


// @address 0x20a480 game_engine_has_grenades_enabled
bool game_engine_has_grenades_enabled(void)
{
    return (current_game_variant()->variant_flags & 1) != 0;
}


// @address 0x20a4b8 game_engine_has_symmetric_rules
bool game_engine_has_symmetric_rules(void)
{
    return current_game_variant() ? current_game_variant()->get_rules_are_symmetric() : false;
}


// @address 0x20a500 game_engine_has_asymmetric_rules
bool game_engine_has_asymmetric_rules(void)
{
    return current_game_variant() ? !current_game_variant()->get_rules_are_symmetric() : false;
}

namespace {

static __forceinline object_datum *nearby_unit_get(long unit_index)
{
    return (object_datum *)object_get(unit_index);
}
}

// @address 0x20bbf8 ?game_engine_nearby_player_running_towards_object@@YA_NJ@Z
/* natural 0x20bbf8 */
bool game_engine_nearby_player_running_towards_object(long object_index)
{
    bool nearby = false;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    c_player_with_unit_iterator iterator;
    iterator.begin();
    while (iterator.next()) {
        player_datum *player = iterator.datum;
        object_datum *unit = nearby_unit_get(player->unit_index);
        real_vector3d velocity = unit->linear_velocity;
        real_point3d player_origin;
        object_get_origin(player->unit_index, &player_origin);
        real_vector3d offset;
        vector_from_points3d(&player_origin, &origin, &offset);
        velocity.k = 0.0f;
        offset.k = 0.0f;
        if (magnitude_squared3d(&offset) < 100.0f && magnitude_squared3d(&velocity) > 2.25f) {
            float angle = angle_between_vectors3d(&velocity, &offset);
            if (angle < 0.2617994f) {
                nearby = true;
                break;
            }
        }
    }
    return nearby;
}

/* Object fields used by game-engine recycling. */
static __forceinline float game_engine_20_ticks_to_seconds(float ticks);
// @address 0x34acb0 ?weapon_can_be_fired@@YA_NJ@Z


static __forceinline unsigned long game_engine28_weapon_multiplayer_type(unsigned short object_index)
{
    object_datum *weapon = (object_datum *)object_get(object_index);
    weapon_definition *definition = ((weapon_definition *)tag_get('weap', weapon->definition_index));
    return (unsigned long)(unsigned short)definition->multiplayer_weapon_type;
}

/* garbage collection times in seconds, indexed by the game engine's garbage collection speed */
static const float k_garbage_collect_item_time[2] = { 30.0f, 30.0f };
static const float k_garbage_collect_biped_time[2] = { 30.0f, 5.0f };
static const float k_garbage_collect_biped_damage_time[2] = { 90.0f, 15.0f };
static const float k_garbage_collect_garbage_time[2] = { 15.0f, 5.0f };
static const float k_garbage_collect_garbage_timeout[2] = { 90.0f, 10.0f };

// @address 0x207bf0 ?game_engine_garbage_collect@@YAXXZ
void game_engine_garbage_collect()
{
    if (!thread_state_test()) {
        long time = game_time_get();
        long speed = game_engine_globals->garbage_collection_speed;
        c_object_iterator<object_datum> iterator;
        iterator.begin(1039, 0);
        while (iterator.next()) {
            long object_index = iterator.get_index();
            if ((unsigned short)object_index % 30 == game_time_get() % 30) {
                object_datum *object = iterator.get_datum();
                long type = object->identifier_type;
                bool garbage = false;
                if ((1 << type) & 0xc) {
                    unsigned long multiplayer_type = 0;
                    bool can_be_fired = true;
                    if ((1 << object->identifier_type) & 4) {
                        multiplayer_type = game_engine28_weapon_multiplayer_type((unsigned short)object_index);
                        can_be_fired = weapon_can_be_fired(object_index);
                    }
                    if (multiplayer_type == 0) {
                        if ((!game_engine_monitoring_object(object_index) || (((item_datum *)object)->inventory_flags & 1) || !can_be_fired) &&
                            (!game_engine_nearby_player_running_towards_object(object_index) || !can_be_fired) &&
                            !((item_datum *)object)->owner_relation &&
                            game_engine_20_ticks_to_seconds((float)(time - ((item_datum *)object)->relevance_time)) >= k_garbage_collect_item_time[speed] &&
                            !((1 << object->identifier_type) & 8))
                            garbage = true;
                    } else {
                        const c_game_engine *engine = current_game_engine();
                        if ((!engine || engine->should_purge_multiplayer_item(object_index)) &&
                            !game_engine_nearby_player_running_towards_object(object_index))
                            garbage = true;
                    }
                } else if ((1 << type) & 1) {
                    bool dead = (object->damage_flags >> 2) & 1;
                    if (dead) {
                        if (game_engine_20_ticks_to_seconds((float)(time - object->last_motion_time)) >= k_garbage_collect_biped_time[speed] ||
                            game_engine_20_ticks_to_seconds((float)(time - ((unit_datum *)object)->unit.time_of_death)) >=
                                k_garbage_collect_biped_damage_time[speed])
                            garbage = true;
                    }
                } else if ((object->recycling_flags & 2) && time >= object->garbage_time) {
                    if (game_engine_20_ticks_to_seconds((float)(time - object->last_motion_time)) >= k_garbage_collect_garbage_time[speed] ||
                        game_engine_20_ticks_to_seconds((float)(time - object->garbage_time)) >= k_garbage_collect_garbage_timeout[speed])
                        garbage = true;
                }
                if (garbage && ((1 << type) & 0x1003)) {
                    c_data_iterator<void> players;
                    players.begin(player_data);
                    while (c_data_iterator_advance(&players)) {
                        player_datum *player = (player_datum *)players.m_datum;
                        if (player->unit_index == object_index || player->dead_unit_index == object_index) {
                            garbage = false;
                            break;
                        }
                    }
                }
                if (garbage) {
                    object_set_at_rest(object_index, false);
                    object_delete(object_index);
                }
            }
        }
    }
}

long game_engine_round_time_get(void)
{
    if (game_time_get() - game_engine_globals->round_start_time < 0)
        return 0;
    return game_time_get() - game_engine_globals->round_start_time;
}


/* 0x20bd88 */
long game_engine_compare_players(long player_index, long other_player_index, bool compare_statistics)
{
    c_game_statborg *statborg = game_engine_statborg_in_range();
    player_datum *player = player_get(player_index);
    player_datum *other_player = player_get(other_player_index);
    bool has_teams = game_engine_has_teams();
    long result = 0;
    if (!has_teams)
        result = !TEST_BIT(other_player->flags, 1) - !TEST_BIT(player->flags, 1);
    if (result == 0) {
        result = TEST_BIT(other_player->flags, 0) - TEST_BIT(player->flags, 0);
        if (result == 0) {
            bool player_out_of_lives = game_engine_player_is_out_of_lives(player_index);
            bool other_player_out_of_lives = game_engine_player_is_out_of_lives(other_player_index);
            result = !other_player_out_of_lives - !player_out_of_lives;
            if (result == 0) {
                if (!has_teams || (result = game_engine_compare_teams(player->configuration.host.team_index,
                    other_player->configuration.host.team_index, true)) == 0) {
                    if (!game_is_finished()) {
                        result = statborg->get_player_stat(other_player_index, 0) - statborg->get_player_stat(player_index, 0);
                        if (result == 0)
                            result = statborg->get_player_stat(other_player_index, _statborg_entry_rounds_won)
                                - statborg->get_player_stat(player_index, _statborg_entry_rounds_won);
                    } else {
                        result = statborg->get_player_stat(other_player_index, _statborg_entry_rounds_won)
                            - statborg->get_player_stat(player_index, _statborg_entry_rounds_won);
                        if (result == 0)
                            result = statborg->get_player_stat(other_player_index, 1) - statborg->get_player_stat(player_index, 1);
                    }
                    if (result == 0) {
                        result = statborg->get_player_stat(other_player_index, 2) - statborg->get_player_stat(player_index, 2);
                        if (result == 0) {
                            result = statborg->get_player_stat(player_index, 3) - statborg->get_player_stat(other_player_index, 3);
                            if (result == 0)
                                result = statborg->get_player_stat(other_player_index, 6) - statborg->get_player_stat(player_index, 6);
                        }
                    }
                }
            }
        }
    }
    return result;
}

/* natural 0x20bf98 */
long game_engine_compare_teams(long first_team, long second_team, bool compare_statistics)
{
    long first = first_team;
    long second = second_team;
    c_game_statborg *statborg = game_engine_statborg_if_current();
    bool first_active = game_engine_is_team_active((e_game_team)(unsigned int)first);
    bool second_active = game_engine_is_team_active((e_game_team)(unsigned int)second);
    long result = (second_active != 0) - (first_active != 0);
    if (result == 0) {
        if (!game_is_finished()) {
            result = statborg->get_team_stat(second, 0) - statborg->get_team_stat(first, 0);
            if (result == 0)
                result = statborg->get_team_stat(second, _statborg_entry_rounds_won) - statborg->get_team_stat(first, _statborg_entry_rounds_won);
        } else {
            result = statborg->get_team_stat(second, _statborg_entry_rounds_won) - statborg->get_team_stat(first, _statborg_entry_rounds_won);
            if (result == 0)
                result = statborg->get_team_stat(second, 1) - statborg->get_team_stat(first, 1);
        }
        if (result == 0) {
            result = statborg->get_team_stat(second, 2) - statborg->get_team_stat(first, 2);
            if (result == 0) {
                result = statborg->get_team_stat(first, 3) - statborg->get_team_stat(second, 3);
                if (result == 0)
                    result = statborg->get_team_stat(second, 6) - statborg->get_team_stat(first, 6);
            }
        }
    }
    return result;
}

/* natural 0x20c140 */
void game_engine_get_universal_string(long string_index, c_static_wchar_string<256> *string)
{
    s_game_globals *game_globals = global_game_globals;
    string->clear();
    if (game_globals && game_globals->multiplayer_globals_index != -1) {
        s_multiplayer_globals_definition *multiplayer_globals =
            (s_multiplayer_globals_definition *)(tag_get('mulg', game_globals->multiplayer_globals_index));
        if (multiplayer_globals->universal.count > 0) {
            s_multiplayer_universal_globals_definition *universal = (s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
            if (universal->strings_index != -1)
                string_list_get_normal_string(universal->strings_index, string_index, string);
        }
    }
}

typedef char game_engine_target_player_offset[(offsetof(s_player_control_globals, states) + offsetof(s_player_control_state, action) +
    offsetof(control, aim_assist) + offsetof(s_aim_assist_targeting_result, target_player_index) == 0x3f8) ? 1 : -1];
typedef char game_engine_scoreboard_zoom_offset[(offsetof(s_player_control_globals, states) + offsetof(s_player_control_state, action) +
    offsetof(control, zoom) == 0x3ea) ? 1 : -1];
namespace {
static __forceinline float target_seconds_to_ticks_real(float seconds)
{
    return (float)game_tick_rate() * seconds;
}
static __forceinline long target_player_from_user(long user_index)
{
    long result = -1;
    long target = ((s_player_control_globals *)player_control_globals)->states[user_index].action.aim_assist.target_player_index;
    if (target != -1) {
        player_datum *player = (player_datum *)datum_try_and_get(player_data, target);
        if (player && player->unit_index != -1)
            result = target;
    }
    return result;
}
}

// @address 0x20c1a8 ?game_engine_update_player_target@@YAXJ@Z
/* natural 0x20c1a8 */
void game_engine_update_player_target(long player_index)
{
    player_datum *player = player_get(player_index);
    long target_player_index = -1;
    float acquire_seconds = 0.25f;
    float lose_seconds = 1.0f;
    long acquire_ticks = (long)(256.0f / target_seconds_to_ticks_real(acquire_seconds) + 0.5f);
    long lose_ticks = (long)(256.0f / target_seconds_to_ticks_real(lose_seconds) + 0.5f);
    long user_index = player_get_first_output_user(player_index);
    if (user_index != -1)
        target_player_index = target_player_from_user(user_index);
    if (player->target_player_index != target_player_index) {
        if (player->target_confidence > 0 && target_player_index == -1) {
            player->target_confidence = (short)(player->target_confidence - lose_ticks > 0 ? player->target_confidence - lose_ticks : 0);
        } else if (player->target_confidence > 0 && target_player_index != -1) {
            player->target_confidence = (short)(player->target_confidence - acquire_ticks > 0 ? player->target_confidence - acquire_ticks : 0);
        }
        if (player->target_confidence == 0)
            player->target_player_index = target_player_index;
    } else {
        player->target_confidence = (short)(player->target_confidence + acquire_ticks > 256 ? 256 : player->target_confidence + acquire_ticks);
    }
}

bool game_engine_weapon_is_bomb(long object_index)
{
    bool result = false;
    if (current_game_variant()->engine_type == 9 && object_index != -1) {
        object_datum *object = (object_datum *)object_get(object_index);
        if (((weapon_definition *)tag_get('weap', object->definition_index))->multiplayer_weapon_type == 1)
            result = true;
    }
    return result;
}

// @address 0xfc278 ?players_next_active_user@@YAJJ@Z
// @address 0x3c02f0 ?voice_player_is_talking@@YA_NJ@Z

namespace {
static __forceinline long first_active_talker_user(void)
{
    long result = -1;
    for (long user_index = 0; user_index < 4; user_index++) {
        if (player_mapping_globals->player_indices[user_index] != -1) {
            result = user_index;
            break;
        }
    }
    return result;
}
static __forceinline long talker_player_from_user(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_indices[user_index];
    return player_index;
}
static __forceinline bool local_player_hears_talker(long local_player_index, long remote_player_index)
{
    bool hears = false;
    if (g_voice_interface_globals.initialized && voice_player_is_talking(remote_player_index)) {
        unsigned long speakers = g_voice_observer.get_player_incoming_voice_to_speakers_mask(local_player_index);
        unsigned long headset = g_voice_observer.get_player_incoming_voice_to_headset_mask(local_player_index);
        hears = TEST_BIT(speakers | headset, remote_player_index);
    }
    return hears;
}
}

// @address 0xadef0 ?unit_get_head_position@@YAXJPATreal_point3d@@@Z
static __forceinline void game_engine_item_get_position_even_if_in_inventory(long object_index, real_point3d *position)
{
    item_datum *item = (item_datum *)object_get(object_index);
    if (item->owner_relation)
        unit_get_head_position(item->unit_object_index, position);
    else
        *position = item->object.bounding_center;
}

// @address 0x20c6a8 ?game_engine_game_object_has_fallen_too_far@@YA_NJ@Z
/* natural 0x20c6a8 */
bool game_engine_game_object_has_fallen_too_far(long object_index)
{
    ::scenario *scenario = global_scenario;
    bool result = false;
    if (scenario->spawn_data.count() > 0) {
        s_scenario_spawn_data *spawn_data = scenario->spawn_data.begin();
        if (spawn_data->game_object_reset_height != 0.0f) {
            real_point3d position;
            if ((1 << object_header_get(object_index)->object_type) & 12)
                game_engine_item_get_position_even_if_in_inventory(object_index, &position);
            else
                object_get_origin(object_index, &position);
            if (position.z < spawn_data->game_object_reset_height)
                result = true;
        }
    }
    return result;
}

/* natural 0x20c8a8 */
void game_engine_update_talkers(void)
{
    word updated_users = 0;
    for (long user_index = first_active_talker_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
        long player_index = talker_player_from_user(user_index);
        if (player_index != -1) {
            word talking_players = 0;
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next()) {
                if (iterator.get_index() != player_index
                    && player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(iterator.get_index())] == -1) {
                    long remote_player_index = iterator.get_index() & 0xffff;
                    bool talking = local_player_hears_talker(player_index & 0xffff, remote_player_index);
                    if (talking) {
                        ((s_local_game_engine_globals *)g_local_game_engine_globals)->user_talker_data[user_index].talk_time[remote_player_index]++;
                        talking_players |= 1 << remote_player_index;
                    }
                }
            }
            for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++) {
                if (!TEST_BIT(talking_players, player_absolute_index))
                    ((s_local_game_engine_globals *)g_local_game_engine_globals)->user_talker_data[user_index].talk_time[player_absolute_index] = 0;
            }
            updated_users |= 1 << user_index;
        }
    }
    for (long user_index = 0; user_index < 4; user_index++) {
        if (!TEST_BIT(updated_users, user_index)) {
            for (long player_index = 0; player_index < 16; player_index++)
                ((s_local_game_engine_globals *)g_local_game_engine_globals)->user_talker_data[user_index].talk_time[player_index] = 0;
        }
    }
}

/* natural 0x20cd78 */
static __forceinline s_pending_entry *allocate_game_engine_message(void)
{
    s_pending_entry *message = (s_pending_entry *)InterlockedPopEntrySList(&g_game_engine_globals_message_queue.free);
    __lwsync();
    if (!message) {
        while (!message) {
            message = (s_pending_entry *)InterlockedPopEntrySList(&g_game_engine_globals_message_queue.free);
            __lwsync();
        }
    }
    return message;
}
static __forceinline void queue_game_engine_message(s_pending_entry *message)
{
    __lwsync();
    if (message)
        InterlockedPushEntrySList(&g_game_engine_globals_message_queue.pending, message);
}
void post_game_engine_globals_message(e_game_engine_globals_message_type type, char bit, short value)
{
    s_pending_entry *message = allocate_game_engine_message();
    message->type = 0;
    message->bit = bit;
    message->value = 0;
    queue_game_engine_message(message);
}

typedef void (*field_proc_t)(void *field);

// @address 0x205e70 ?__tls_set_g_local_game_engine_globals_allocator@@YAXPAX@Z
// @address 0x205e80 ?__tls_set_g_game_engine_globals_allocator@@YAXPAX@Z
/* natural 0x200d68 */
// @address 0x200d68 ?__tls_set_g_director_globals_allocator@@YAXPAX@Z
void __tls_set_g_director_globals_allocator(void *address)
{
    g_director_globals = (s_controllers *)address;
}
void __tls_set_g_game_engine_globals_allocator(void *address)
{
    game_engine_globals = (s_game_engine_globals *)address;
}


// @address 0x2025e8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    g_director_globals = 0;
    restricted_region_free_member(3, value);
}

static __forceinline void *restricted_member_address(long region, long member_index)
{
    char *base = g_restricted_address[region];
    unsigned int offset = ((s_field_layout *)g_restricted_regions)[region].fields[member_index].offset;
    return base + offset;
}
// These are the two inlined restricted-region registration paths, including their TLS setters.
static __forceinline long register_local_game_engine_member(const char *name, const char *type, unsigned int size)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_local_game_engine_globals_allocator, 0, 0);
    void *address = restricted_member_address(3, member_index);
    g_local_game_engine_globals = (s_local_game_engine_globals *)address;
    return member_index;
}
static __forceinline long register_game_engine_member(const char *name, const char *type, unsigned int size)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_game_engine_globals_allocator, 0, 0);
    void *address = restricted_member_address(3, member_index);
    game_engine_globals = (s_game_engine_globals *)address;
    return member_index;
}

/* natural 0x20cdf8 */
extern "C" {
// @address 0x20cdf8 game_engine_get_multiplayer_string
void game_engine_get_multiplayer_string(long string_index, c_static_wchar_string_1024 *string)
{
    s_game_globals *game_globals = global_game_globals;
    string->string[0] = 0;
    if (string_index == -1 || string_index == 0)
        return;
    if (game_globals && game_globals->multiplayer_globals_index != -1) {
        s_multiplayer_globals_definition *multiplayer_globals =
            (s_multiplayer_globals_definition *)(tag_get('mulg', game_globals->multiplayer_globals_index));
        if (multiplayer_globals->universal.count > 0)
            text_group_get_string(((s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address)->strings_index, string_index, string);
    }
}
}

/* natural 0x20ce68 */
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_local_game_engine_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = register_local_game_engine_member(name, type, size);
    m_thread_id = get_current_thread_index();
    return restricted_member_address(3, m_member_index);
}


// @address 0x20cf08 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_local_game_engine_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_local_game_engine_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_local_game_engine_globals_allocator>::deallocate(void *allocation)
{
    free_memory();
}

/* natural 0x20cf40 */
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_engine_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = register_game_engine_member(name, type, size);
    m_thread_id = get_current_thread_index();
    return restricted_member_address(3, m_member_index);
}


// @address 0x20cfe0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_engine_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_engine_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x20cff8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_game_engine_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_game_engine_globals_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    game_engine_globals = 0;
    restricted_region_free_member(3, value);
}

// @address 0x20d018
template<>
const char *c_static_string<32>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, 31, format, args);
    m_string[31] = 0;
    return m_string;
}


/* natural 0x350f90 */


static __forceinline unit_datum *game_engine_object_get_datum(long object_index)
{
    return (unit_datum *)object_get(object_index);
}

/* natural 0x29acb8 (c_game_statborg::stats_start): no March name or pairing is available. */

// @address 0x29acb8 game_statborg_start_stats
void game_statborg_start_stats(c_game_statborg *statborg)
{
    statborg->m_stats_started = true;
    if (!thread_state_test())
    {
        for (long player_index = 0; player_index < 16; player_index++)
            statborg->m_update_mask |= 1 << player_index;
        for (long team_index = 0; team_index < k_maximum_teams; team_index++)
            statborg->m_update_mask |= 1 << (team_index + 16);
    }
}

// @address 0x350f90 ?game_engine_player_has_infinite_ammo@@YA_NJ@Z
bool game_engine_player_has_infinite_ammo(long object_index)
{
    bool result = false;
    if (game_globals->options.game_mode == 2)
    {
        unit_datum *object = game_engine_object_get_datum(object_index);
        if (object->unit.kind && object->unit.actor_index != -1)
        {
            unit_datum *parent = game_engine_object_get_datum(object->unit.actor_index);
            if (parent->unit.player_index != -1 && game_engine_player_grants_infinite_ammo(parent->unit.player_index))
                result = true;
        }
    }
    return result;
}

/* natural 0x479060: no March name or pairing is available. */

// @address 0x479060 game_engine_get_player_rating
long game_engine_get_player_rating(long index)
{
    long result = -1;
    long value = -1;
    if (simulation_compute_connection_rating(index, &game_engine_globals->connection_statistics[index], &value))
        result = value;
    return result;
}

// @address 0x98818 ?game_campaign_is_multiplayer@@YA_NXZ

// @address 0x12cc30 game_engine_update_waypoint_timers
void game_engine_update_waypoint_timers(s_player_waypoint_timers *waypoints)
{
    if (waypoints->timers[0].active == 0)
        waypoints->timers[0].in_use = false;
    if (waypoints->timers[1].active == 0)
        waypoints->timers[1].in_use = false;
    if (waypoints->timers[2].active == 0)
        waypoints->timers[2].in_use = false;
    if (waypoints->timers[3].active == 0)
        waypoints->timers[3].in_use = false;
    if (waypoints->timers[4].active == 0)
        waypoints->timers[4].in_use = false;
    if (waypoints->timers[5].active == 0)
        waypoints->timers[5].in_use = false;
    if (waypoints->timers[6].active == 0)
        waypoints->timers[6].in_use = false;
    if (waypoints->timers[7].active == 0)
        waypoints->timers[7].in_use = false;
    if (game_globals->options.game_mode == 1 && game_campaign_is_multiplayer()) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        if (iterator.next()) {
            s_game_engine_fade_entry *waypoint = chud_navpoint_globals()->player_waypoints;
            do {
                game_engine_update_waypoint(waypoint);
                waypoint++;
            } while (iterator.next());
        }
    }
}
static const float game_engine_half_second = 0.5f;
static __forceinline long game_engine_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)(double)game_tick_rate();
    float tick_count = ticks * seconds;
    tick_count += game_engine_half_second;
    return (long)tick_count;
}
// @address 0x12cd58 game_engine_update_waypoint
extern "C" void game_engine_update_waypoint(s_game_engine_fade_entry *waypoint)
{
    if (waypoint->action_ticks != 0) {
        waypoint->action_ticks--;
        if (waypoint->action_ticks == 0) {
            if (waypoint->queued_state != 0) {
                waypoint->state = waypoint->queued_state;
                waypoint->action_ticks = game_engine_seconds_to_ticks_round(game_engine_half_second);
                waypoint->queued_state = 0;
            } else {
                waypoint->state = 0;
                waypoint->action_ticks = 0;
            }
        }
    }
}

// @address 0x207080 game_engine_get_winning_status
extern "C" long game_engine_get_winning_status(e_game_team team)
{
    long result = 0;
    if (game_engine_statborg_if_current() && game_engine_has_teams() && game_engine_is_team_ever_active((unsigned int)team))
        result = game_engine_get_statborg()->get_team_stat(team, _statborg_entry_rounds_won);
    return result;
}




// @address 0x20cb18 ?data_mine_insert_multiplayer_game_options@@YAXPBDPBVc_game_variant@@@Z
void data_mine_insert_multiplayer_game_options(const char *reason, const c_game_variant *variant)
{
    long player_count = 0;
    long machine_count = 0;
    long machine_players[16] = {0};
    {
    c_data_iterator<void> iterator;
    iterator.begin(player_data);
    while (c_data_iterator_advance(&iterator)) {
        player_datum *player = (player_datum *)iterator.m_datum;
        if (player->machine_index != -1 && !TEST_BIT(player->flags, 1)) {
            if ((unsigned short)player->machine_index < 16)
                machine_players[player->machine_index]++;
            if (player->machine_index + 1 > machine_count)
                machine_count = player->machine_index + 1;
            player_count++;
        }
    }
    }
    if (machine_count > 0 && player_count > 0) {
        c_datamine event(0, reason, 2, "game", "engine");
        event.add_long("game type", variant->engine_type);
        event.add_wchar_string("variant", reinterpret_cast<const wchar_t *>(&variant->get_active_variant()->m_checksum));
        event.add_long("player count", player_count);
        event.add_long("box count", machine_count);
        for (unsigned int machine_index = 0; machine_index < 16; machine_index++) {
            c_static_string<32> name;
            name.print("box %d", machine_index);
            event.add_long(name.get_string(), machine_players[machine_index]);
        }
    }
}



static __forceinline void game_engine_initialize_message_queue(long limit)
{
#ifdef HALO3_64BIT
    g_game_engine_globals_message_queue.pending = 0;
    g_game_engine_globals_message_queue.pad804 = 0;
    g_game_engine_globals_message_queue.free = 0;
    g_game_engine_globals_message_queue.pad80c = 0;
#else
    g_game_engine_globals_message_queue.pending_head = 0;
    g_game_engine_globals_message_queue.free_head = 0;
#endif
    for (unsigned long index = 0; index < 128; index++)
        g_game_engine_globals_message_queue.entries[index].next = 0;
    g_game_engine_globals_message_queue.initialized = true;
    g_game_engine_globals_message_queue.limit = limit;
    if (g_game_engine_globals_message_queue.limit != 0) {
        for (long index = g_game_engine_globals_message_queue.limit - 1; index >= 0; index--) {
            __lwsync();
            InterlockedPushEntrySList(&g_game_engine_globals_message_queue.free, &g_game_engine_globals_message_queue.entries[index]);
        }
    }
}
// @address 0x205e90 ?game_engine_initialize@@YAXXZ
void game_engine_initialize(void)
{
    game_engine_initialize_message_queue(128);
    memset(&g_game_engine_display_user_flags, 0, sizeof(g_game_engine_display_user_flags));
    game_engine_globals = (s_game_engine_globals *)g_game_engine_globals_allocator.reserve_memory("game engine globals", 0, 0x157f0, 0);
    g_local_game_engine_globals = (s_local_game_engine_globals *)g_local_game_engine_globals_allocator.reserve_memory(
        "local game engine globals", 0, sizeof(*g_local_game_engine_globals), 0);
    memset(game_engine_globals, 0, sizeof(*game_engine_globals));
    memset(g_local_game_engine_globals, 0, sizeof(*g_local_game_engine_globals));
    game_engine_globals->game_engine_index = 0;
    for (long index = 0; index < 64; index++)
        game_engine_globals->dead_players[index].player_index = -1;
}

/* ---- 0x205fc8 game_engine_initialize_for_new_map -------------------------------- */
struct s_map_variant_validation;


// @address 0x205fc8 ?game_engine_initialize_for_new_map@@YAXXZ
void game_engine_initialize_for_new_map(void)
{
    memset(game_engine_globals, 0, sizeof(*game_engine_globals));
    memset(g_local_game_engine_globals, 0, sizeof(*g_local_game_engine_globals));
    game_engine_globals->game_engine_index = 0;
    if (current_game_variant()) {
        game_engine_globals->game_engine_index = current_game_variant()->engine_type;
        game_engine_globals->long_2c = -1;
        game_engine_globals->long_30 = -1;
        game_engine_globals->short_f9a4 = 42;
        for (long i = 0; i < 16; i++)
            game_engine_globals->longs_34[i] = -1;
        for (long i = 0; i < 9; i++)
            game_engine_globals->shorts_10[i] = -1;
#ifdef HALO3_64BIT
        char team_lives = current_game_variant()->team_lives_per_round;
#else
        char team_lives = current_game_variant()->variant_storage[0x106];
#endif
        for (long i = 0; i < 8; i++)
            game_engine_globals->team_lives_22[i] = team_lives;
#ifdef HALO3_64BIT
        if (current_game_variant()->has_teams())
#else
        if (TEST_BIT((byte)current_game_variant()->variant_storage[0x100], 0))
#endif
            game_engine_globals->flags |= 8;
        else
            game_engine_globals->flags &= ~8;
        if (!thread_state_test())
            game_engine_globals->flags |= 4;
        c_game_statborg *statborg = game_engine_get_statborg();
        if (statborg) {
            memset(statborg->m_player_stats, 0, sizeof(statborg->m_player_stats));
            memset(statborg->m_team_rows, 0, sizeof(statborg->m_team_rows));
            statborg->m_stats_started = false;
            statborg->m_update_mask = 0;
            statborg->m_finalized = false;
        }
        if (current_game_engine_entry()) {
            bool no_multiplayer_globals = false;
            long tag_index = global_game_globals->multiplayer_globals_index;
            if (tag_index == -1 || ((s_multiplayer_globals_definition *)tag_get('mulg', tag_index))->runtime.count == 0)
                no_multiplayer_globals = true;
            if (no_multiplayer_globals) {
                game_engine_globals->game_engine_index = 0;
            } else {
                game_engine_initialize_multiplayer_sounds();
                game_engine_globals->candy_monitor_manager.initialize_for_new_map();
                if (!current_game_engine()->initialize_for_new_map())
                    game_engine_globals->game_engine_index = 0;
                if (game_engine_has_teams()) {
                    game_engine_globals->designator_mask = -1;
                    game_engine_globals->valid_team_mask = -1;
                }
                game_engine_globals->timer_seconds = current_game_variant()->round_time_limit_minutes * 60;
            }
        }
        statborg = game_engine_get_statborg();
        if (statborg)
            game_statborg_start_stats(statborg);
        for (long i = 0; i < 4; i++)
            ((s_local_game_engine_globals *)g_local_game_engine_globals)->player_states[i].index = -1;
    }
    ((c_map_variant *)&game_engine_globals->map_info)->initialize_for_scenario();
    s_game_options *options = &game_globals->options;
    if (options->map_variant.map_id != -1 &&
        map_variant_is_valid((const s_map_variant_validation *)&options->map_variant)) {
        s_game_engine_map_info *info = &game_engine_globals->map_info;
        game_engine_set_map_info_name(info, (const word *)options->map_variant.metadata.name);
        memset(info->metadata.description, 0, sizeof(info->metadata.description));
        csstrnzcpy(info->metadata.description, options->map_variant.metadata.description, sizeof(info->metadata.description));
        info->m_built_in = 0;
        if (thread_state_test() && options->map_variant.variant_object_count != 0)
            game_engine_globals->map_info.variant_object_count = options->map_variant.variant_object_count;
    }
    for (long i = 0; i < 64; i++)
        game_engine_globals->dead_players[i].player_index = -1;
    memset(game_engine_globals->teleporter_slot_table, 0, sizeof(game_engine_globals->teleporter_slot_table));
    ((c_area_set<c_teleporter_area, 32> *)game_engine_globals->teleporter_slot_table)->initialize(0x440, 112, _multiplayer_team_designator_none);
    game_engine_globals->last_respawn_time = 0;
    game_engine_globals->respawn_count = 0;
}

c_datamine::~c_datamine() {}
s_datamine_parameter *c_datamine::fill_and_get_parameter(const char *name, long type, long size) { return 0; }
void c_datamine::add_long(const char *name, long value)
{
    if (event && name) {
        s_datamine_parameter *parameter = fill_and_get_parameter(name, 0, 40);
        if (parameter)
            parameter->value = value;
    }
}
void c_datamine::add_wchar_string(const char *name, const wchar_t *value)
{
}



static __forceinline player_datum *game_engine_datamine_try_player(const s_data_array *data, long player_index)
{
    player_datum *player = 0;
    if (player_index != -1) {
        long absolute_index = (unsigned short)player_index;
        long identifier = player_index >> 16;
        if (absolute_index < data->first_unallocated) {
            player_datum *candidate = (player_datum *)(data->data + absolute_index * data->size);
            if (candidate->identifier != 0 && candidate->identifier == identifier)
                player = candidate;
        }
    }
    return player;
}
// @address 0x20cc50 ?data_mine_insert_multiplayer_statistics@@YAXXZ
void data_mine_insert_multiplayer_statistics(void)
{
    s_game_engine_score_list score_list;
    game_engine_scoring_build_score_list(&score_list, 1, false);
    short player_count = score_list.player_count;
    if (player_count > 0) {
        bool team_game = game_engine_has_teams();
        for (long player_number = 0; player_number < player_count; player_number++) {
            long player_index = score_list.player_indices[player_number];
            if (player_index != -1) {
                player_datum *player = game_engine_datamine_try_player(player_data, player_index);
                if (player) {
                    c_static_string<32> name(player->configuration.host.name);
                    c_datamine event(0, "game ending statistics", 3, "game", "engine",
                        k_units_datamine_multiplayer);
                    if (team_game) {
                        if (player->configuration.host.team_index != -1)
                            game_engine_find_team_place(player->configuration.host.team_index);
                    } else {
                        game_engine_get_player_place(player_index);
                    }
                }
            }
        }
    }
}

static __forceinline int game_engine_time_valid_index(long index)
{
    if (index < 1) return 0;
    if (index > 10) return 0;
    return 1;
}
static __forceinline c_game_engine *game_engine_time_current_engine(long index)
{
    c_game_engine *result;
    if (game_engine_time_valid_index(index)) result = game_engines[index];
    else result = 0;
    return result;
}
// @address 0x20c330 ?game_engine_get_time_left_in_ticks@@YAJ_N@Z
long game_engine_get_time_left_in_ticks(bool update_event)
{
    long round_time_limit_ticks = game_time_globals->ticks_per_second * current_game_variant()->round_time_limit_minutes * 60;
    long round_time = game_time_globals->time - game_engine_globals->round_start_time;
    if (game_time_globals->time - game_engine_globals->round_start_time < 0)
        round_time = 0;
    long time_left_in_ticks = round_time_limit_ticks - round_time;
    if (time_left_in_ticks < 0)
        time_left_in_ticks = 0;
    long original_time_left_in_ticks = time_left_in_ticks;
    time_left_in_ticks = game_engine_time_current_engine(game_engine_globals->game_engine_index)->get_time_left_in_ticks(time_left_in_ticks,
        update_event, true);
    if (update_event) {
        if (time_left_in_ticks != original_time_left_in_ticks) {
            if ((game_engine_globals->flags & 2) == 0) {
                s_game_engine_event_data event_data;
                event_data.type = 0;
                event_data.data = 0;
                event_data.event = 38;
                event_data.identifier = -1;
                event_data.flags = -1;
                event_data.cause_player_index = -1;
                event_data.cause_team_index = -1;
                event_data.effect_player_index = -1;
                event_data.effect_team_index = -1;
                event_data.data_word = -1;
                game_engine_send_event(&event_data);
                game_engine_globals->flags |= 2;
                return time_left_in_ticks;
            } else {
                return time_left_in_ticks;
            }
        } else {
            game_engine_globals->flags &= ~2;
            return time_left_in_ticks;
        }
    } else {
        return time_left_in_ticks;
    }
    return 0;
}

// @address 0x278490 ?game_engine_next_active_team@@YA?AW4e_game_team@@W41@@Z
// @address 0x206e50 ?game_engine_get_player_kills@@YAJJ@Z
// @address 0x206f90 ?game_engine_get_team_rounds_won@@YAJW4e_game_team@@@Z

/* ---- round winner check (0x20aa68) ------------------------------------------------ */
static __forceinline long game_engine_check_round_time_limit_seconds(void)
{
    return current_game_variant()->round_time_limit_minutes * 60;
}

static __forceinline long game_engine_check_first_active_team(void)
{
    long result = -1;
    for (long team = 0; team < k_maximum_teams; team++)
    {
        if (game_engine_globals->active_teams & (1 << team))
        {
            result = team;
            break;
        }
    }
    return result;
}

// @address 0x20aa68 ?game_engine_check_for_round_winner@@YAXXZ
void game_engine_check_for_round_winner(void)
{
    bool playback = false;
    if (game_globals->options.game_simulation == 4)
        playback = true;
    if (playback || !game_engine_in_round())
        return;
    long round_time_limit = game_engine_check_round_time_limit_seconds();
    long score_to_win = current_game_variant()->get_active_variant()->get_score_to_win_round();
    long time_left = game_engine_get_time_left_in_ticks(false);
    bool round_ended = false;
    if (round_time_limit > 0 && time_left <= 0)
    {
        long winner = game_engine_choose_winner_index();
        game_engine_end_round_with_winner(winner, false, game_engine_end_condition_round_end_time_ran_out);
        round_ended = true;
    }
    if (!round_ended)
    {
        long winner = -1;
        bool end_game = false;
        long condition = 0;
        if (game_engine_should_end_round(&winner, &end_game, &condition))
        {
            game_engine_end_round_with_winner(winner, end_game, (e_game_engine_end_condition)condition);
            round_ended = true;
        }
    }
    if (round_ended || score_to_win <= 0)
        return;
    if (game_engine_has_teams())
    {
        for (long team = game_engine_check_first_active_team(); team != -1; team = game_engine_next_active_team((e_game_team)team))
        {
            long score = game_engine_get_team_rounds_won((e_game_team)team);
            if (score >= score_to_win)
            {
                game_engine_end_round_with_winner(team, false, game_engine_end_condition_round_end_team_scoring);
                return;
            }
        }
    }
    else
    {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next())
        {
            long player = iterator.get_index();
            long score = game_engine_get_player_kills(player);
            if (score >= score_to_win)
            {
                game_engine_end_round_with_winner(player, false, game_engine_end_condition_round_end_scoring);
                return;
            }
        }
    }
}

// @address 0x20ac98 ?game_engine_get_player_state_index@@YAJJ@Z
long game_engine_get_player_state_index(long player_index)
{
    bool valid = false;
    long code;
    long engine_index = game_engine_globals->game_engine_index;
    c_game_engine *engine;
    if (current_index() >= 1 && current_index() <= 10)
        engine = game_engines[engine_index];
    else
        engine = 0;
    long result = engine->get_player_state_index(player_index, &valid);
    if (!valid && result != -1)
        return result;
    if (game_is_finished())
    {
        game_engine_get_player_winning_status(player_index, &code);
        switch (code)
        {
        case 0:
            result = 8;
            break;
        case 3:
            result = 9;
            break;
        case 1:
            result = 10;
            break;
        case 2:
            result = 11;
            break;
        default:
            break;
        }
        return result;
    }
    player_datum *player = player_get(player_index);
    long timer = game_engine_globals->timer_seconds;
    if (player->unit_index == -1)
    {
        if (game_engine_player_is_out_of_lives(player_index))
            result = 4;
        else if (TEST_BIT(player->flags, 12))
            result = 3;
        else if (player->respawn_timer >= 0)
            result = 2;
        return result;
    }
    long elapsed_round_ticks;
    if (game_time_get() - game_engine_globals->round_start_time < 0)
        elapsed_round_ticks = 0;
    else
        elapsed_round_ticks = game_time_get() - game_engine_globals->round_start_time;
    short ticks = game_tick_rate();
    if (elapsed_round_ticks < ticks + (ticks << 1))
        result = 19;
    else if (timer != -1 && timer != 0 && timer < 5)
        result = 18;
    else if (result == -1)
    {
        long score_to_win = game_engine_get_score_to_win_round();
        bool unlimited_score = score_to_win <= 0;
        unsigned char flag = unlimited_score;
        game_engine_get_player_winning_status(player_index, &code);
        if ((unsigned int)code >= 1)
        {
            if ((unsigned int)code != 1)
            {
                if ((unsigned int)code == 3)
                    result = ((flag != 0 ? -1 : 0) & 21) + 6;
            }
            else
                result = ((flag != 0 ? -1 : 0) & 21) + 7;
        }
        else
            result = ((flag != 0 ? -1 : 0) & 21) + 5;
    }
    return result;
}

// @address 0x8b288 ?get_scoreboard_screen@c_gui_screen_scoreboard@@SAPAV1@W4e_controller_index@@@Z
// @address 0x8b8e8 ?hide_scoreboard@c_gui_screen_scoreboard@@SAXW4e_controller_index@@@Z
// @address 0x491048 ?show_scoreboard@c_gui_screen_scoreboard@@SAXW4e_controller_index@@_N@Z

// @address 0x46f498 ?game_engine_update_console_scoreboard@@YAXXZ
void game_engine_update_console_scoreboard(void)
{
    float value = 0.0f;
    c_gui_screen_scoreboard *owner = c_gui_screen_scoreboard::get_scoreboard_screen(k_any_controller);
    if (owner != 0)
    {
        c_gui_widget *comp = ((c_gui_widget *)owner)->get_child_widget(_gui_text_widget, 0x29d);
        if (comp != 0)
        {
            s_animation_transform *sub = comp->get_animated_state();
            if (sub != 0)
                value = sub->color.alpha;
        }
    }
    c_gui_screen_scoreboard::m_any_controller_scoreboard_alpha = value;
    if (game_is_scoreboard_blocked(0))
    {
        bool ok = false;
        c_gui_screen_scoreboard *user = c_gui_screen_scoreboard::get_scoreboard_screen(k_any_controller);
        if (user != 0 && g_window_manager.get_screen_above(_console_window, user) != 0)
            ok = true;
        if (!ok)
            c_gui_screen_scoreboard::show_scoreboard(k_any_controller, false);
    }
    else
    {
        c_gui_screen_scoreboard::hide_scoreboard(k_any_controller);
    }
}

// @address 0x3a97c0 ?simulation_action_game_engine_globals_update@@YAXK@Z

static __forceinline float game_engine_20_ticks_to_seconds(float ticks)
{
    return ticks * game_tick_length();
}

static __forceinline unit_datum *game_engine_20_unit_get(long unit_index)
{
    return (unit_datum *)object_get(unit_index);
}

static __forceinline void game_engine_20_player_clear_shield(long player_index)
{
    player_datum *player = player_get(player_index);
    if (player->unit_index != -1)
        game_engine_20_unit_get(player->unit_index)->object.shield_vitality = 0.0f;
}

// match: 0x209148: unchecked-divide form keeps plain divw for time % ticks_per_second
#pragma optimize("c", on)
// @address 0x209148 game_engine_update_player
extern "C" void game_engine_update_player(long player_index)
{
    player_datum *player = player_get(player_index);
    game_engine_assemble_player_traits(player_index);
    if (!game_engine_player_has_shield(player_index))
        game_engine_20_player_clear_shield(player_index);
    if (player->grace_ticks > 0)
        player->grace_ticks--;
    if (!thread_state_test() && player->unit_index != -1 && game_engine_in_round()) {
        game_time_globals_definition *game_time = game_time_globals;
        if (game_time->time % game_time->ticks_per_second == 0)
            game_results_statistic_increment(player_index & 0xffff, _game_team_none, _game_results_statistic_seconds_alive, 1);
        if (game_time->time % (game_time->ticks_per_second * 5) == 0)
            simulation_action_object_latch_update(player->unit_index, 2);
    }
    long unit_index = player->unit_index;
    if (unit_index != -1) {
        unit_datum *unit = game_engine_20_unit_get(unit_index);
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1) {
            if (player->vehicle_entry_ticks == -1)
                player->vehicle_entry_ticks = game_tick_rate() * 2;
            else if (player->vehicle_entry_ticks > 0)
                player->vehicle_entry_ticks--;
            if (!thread_state_test()) {
                bool in_seat;
                switch (player->traits.m_movement_traits.m_vehicle_usage) {
                case 1:
                    in_seat = true;
                    break;
                case 2:
                    in_seat = unit_seat_test_flag(unit->object.parent_object_index, unit->unit.parent_seat_index, _unit_seat_driver_bit);
                    break;
                default:
                    in_seat = false;
                    break;
                }
                if (in_seat)
                    action_submit_event(unit_index, 13);
            }
        } else {
            player->vehicle_entry_ticks = -1;
        }
    }
    if (player->unit_index != -1) {
        if (player->unitless_countdown_ticks != -1 && thread_state_test())
            player->respawn_counter++;
        player->unitless_countdown_ticks = -1;
        if (player->ticks_with_unit == -1) {
            player->ticks_with_unit = 0;
            if (!thread_state_test()) {
                player->respawn_player_index = -1;
                simulation_action_game_engine_player_update((short)player_index, 0x400);
            }
        } else if (player->ticks_with_unit < 127) {
            player->ticks_with_unit++;
        }
    } else {
        if (player->unitless_countdown_ticks == -1)
            player->unitless_countdown_ticks = game_tick_rate();
        else if (player->unitless_countdown_ticks > 0)
            player->unitless_countdown_ticks--;
        player->ticks_with_unit = -1;
    }
    game_engine_update_player_target(player_index);
    get_current_entry()->player_update(player_index);
    update_player_waypoint_data(player_index);
}

// match: 0x209148: restore default lowering after unchecked-divide region
#pragma optimize("c", off)

// @address 0x2094c8 ?game_engine_update_after_game_update_state@@YAXXZ
void game_engine_update_after_game_update_state(void)
{
    bool in_round = game_engine_in_round();
    if (in_round)
    {
        bool predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (!predicted)
        {
            long round_time_limit = game_engine_check_round_time_limit_seconds();
    if (round_time_limit != 0)
    {
        long time_left_in_ticks = game_engine_get_time_left_in_ticks(true);
        long time_left_in_seconds = (long)game_engine_20_ticks_to_seconds((float)time_left_in_ticks);
        if (time_left_in_seconds != game_engine_globals->timer_seconds)
        {
            long event_index = -1;
            game_engine_globals->timer_seconds = (short)time_left_in_seconds;
            simulation_action_game_engine_globals_update(32);
            if (time_left_in_seconds == 1800)
                event_index = 14;
            else if (time_left_in_seconds == 900)
                event_index = 15;
            else if (time_left_in_seconds == 300)
                event_index = 16;
            else if (time_left_in_seconds == 60)
                event_index = 17;
            else if (time_left_in_seconds == 30)
                event_index = 33;
            else if (time_left_in_seconds == 10)
                event_index = 34;
            if (event_index != -1)
            {
                s_game_engine_event_data event_data;
                game_engine_initialize_event(0, event_index, &event_data);
                game_engine_send_event(&event_data);
            }
        }
        if (time_left_in_ticks <= 0)
        {
            s_game_engine_event_data event_data;
            game_engine_initialize_event(0, 18, &event_data);
            game_engine_send_event(&event_data);
        }
    }
    else
    {
        long round_time;
        if (game_time_get() - game_engine_globals->round_start_time < 0)
            round_time = 0;
        else
            round_time = game_time_get() - game_engine_globals->round_start_time;
        long round_time_in_seconds = (long)game_engine_20_ticks_to_seconds((float)round_time);
        round_time_in_seconds = round_time_in_seconds > 32767 ? 32767 : round_time_in_seconds;
        if (round_time_in_seconds != game_engine_globals->timer_seconds)
        {
            game_engine_globals->timer_seconds = (short)round_time_in_seconds;
            simulation_action_game_engine_globals_update(32);
        }
        }
    }
}
}


/* 0x209710: server-side new-state response (cf. game_engine_client_respond_to_new_state) */
// @address 0x209710 game_engine_server_respond_to_new_state
extern "C" void game_engine_server_respond_to_new_state(void)
{
    long desired_state = game_engine_globals->desired_game_engine_state;
    if (desired_state == (short)game_engine_globals->game_engine_state)
        return;
    switch (desired_state)
    {
    case 1:
    {
        game_engine_initialize_for_new_round();
        c_game_statborg *statborg = game_engine_get_statborg();
        if (statborg)
            game_statborg_start_stats(statborg);
        game_engine_globals->round_start_time = game_time_get();
        bool predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (!predicted)
        {
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next())
            {
                long player_index = (unsigned short)iterator.m_iterator.m_iterator.index;
                long lives_limit = game_engine_get_current_variant()->lives_per_round;
                if (lives_limit == 0u)
                    lives_limit = -1;
                player_datum *player = iterator.m_iterator.m_datum;
                player->count = lives_limit;
                ::simulation_action_game_engine_player_update((short)player_index, 0x200);
                player->flags |= 8;
            }
        }
        break;
    }
    case 2:
    {
        c_game_statborg *statborg = game_engine_get_statborg();
        if (statborg)
            statborg->m_stats_started = false;
        bool predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (!predicted && g_game_results_globals.active)
            g_game_results_globals.finished = true;
        const c_game_engine *engine = current_game_engine();
        if (engine)
            engine->player_hump(-1);
        game_engine_globals->state_change_timer = game_tick_rate() * 5;
        break;
    }
    case 3:
    {
        game_engine_globals->round_index++;
        simulation_action_game_engine_globals_update(16);
        game_engine_globals->state_change_timer = game_tick_rate();
        c_data_iterator<void> iterator;
        iterator.begin(player_data);
        while (c_data_iterator_advance(&iterator))
        {
            player_datum *player = (player_datum *)iterator.m_datum;
            player->traits.m_shield_vitality_traits.set_shield_multiplier_setting(
                game_engine_get_current_variant()->base_player_traits.m_shield_vitality_traits.m_shield_multiplier, true);
            simulation_action_game_engine_player_update((short)iterator.m_iterator.index, 2);
        }
        break;
    }
    }
    if (current_game_engine_entry())
        current_game_engine()->prepare_for_new_state(game_engine_globals->desired_game_engine_state);
    game_engine_globals->game_engine_state = game_engine_globals->desired_game_engine_state;
    simulation_action_game_engine_globals_update(4);
}

/* 0x209dc8 */
void game_engine_client_respond_to_new_state(long state)
{
    switch (state)
    {
    case 1:
        game_engine_globals->round_start_time = game_time_get();
        game_engine_reset_player_unit_timers_for_new_round();
        if (current_game_engine_entry())
            current_game_engine()->initialize_for_new_round();
        break;
    case 3:
    {
        game_engine_garbage_collect_for_round_switch();
        c_data_iterator<void> iterator;
        iterator.begin(player_data);
        while (c_data_iterator_advance(&iterator))
        {
            player_datum *player = (player_datum *)iterator.m_datum;
            player->traits.m_shield_vitality_traits.set_shield_multiplier_setting(
                game_engine_get_current_variant()->base_player_traits.m_shield_vitality_traits.m_shield_multiplier, true);
        }
        break;
    }
    }
}

/* 0x209a00 game_engine_update_score_and_standing */

static __forceinline long game_engine_score_list_mode(void)
{
    unsigned char finished = game_globals->game_finished_state; return finished != 0;
}

// @address 0x209a00 ?game_engine_update_score_and_standing@@YAXXZ
void game_engine_update_score_and_standing(void)
{
    if (game_engine_has_teams())
    {
        bool tied = game_engine_are_teams_tied();
        unsigned short completed_teams = 0;
        for (long player_index = 0; player_index < 16; player_index++)
        {
            player_datum *player = static_cast<player_datum *>(datum_try_and_get_absolute(player_data, player_index));
            if (player)
            {
                if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < k_maximum_teams)
                {
                    long index = (player->identifier << 16) | player_index;
                    short score = game_engine_get_statborg()->get_player_stat(index, 1);
                    long place = game_engine_get_player_place(index);
                    game_results_set_final_player_score_and_standing(player_index, score, place);
                    if (!TEST_BIT(player->flags, 1))
                    {
                        long team_index = player->configuration.host.team_index;
                        if (team_index != -1)
                        {
                            game_results_statistic_set(player_index, (e_game_team)team_index, _game_results_statistic_games_completed, 1);
                            if (!tied && game_engine_find_team_place(team_index) == 0)
                                game_results_statistic_set(player_index, (e_game_team)player->configuration.host.team_index,
                                    _game_results_statistic_games_won, 1);
                        }
                    }
                    if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < k_maximum_teams &&
                        ((1 << player->configuration.host.team_index) & completed_teams) == 0)
                    {
                        short team_score = game_engine_get_statborg()->get_team_stat(player->configuration.host.team_index, 1);
                        long team_place = game_engine_find_team_place(player->configuration.host.team_index);
                        game_results_set_final_team_score_and_standing((e_game_team)player->configuration.host.team_index, team_score, team_place);
                        completed_teams = (unsigned short)((1 << player->configuration.host.team_index) | completed_teams);
                    }
                }
            }
        }
        for (long team_index = 0; team_index <= k_maximum_teams - 1; team_index++)
        {
            if (((1 << team_index) & completed_teams) == 0 && team_is_ever_active(team_index))
            {
                long score = game_engine_get_statborg()->get_team_stat(team_index, 1);
                long place = game_engine_scoring_get_team_compound_rank(team_index, game_engine_score_list_mode());
                long standing = place == -1 ? -1 : place / 2;
                game_results_set_final_team_score_and_standing((e_game_team)team_index, score, standing);
                completed_teams = (unsigned short)((1 << team_index) | completed_teams);
            }
        }
    }
    else
    {
        bool tied = game_engine_are_players_tied();
        for (long player_index = 0; player_index < 16; player_index++)
        {
            player_datum *player = static_cast<player_datum *>(datum_try_and_get_absolute(player_data, player_index));
            if (player)
            {
                if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16)
                {
                    long index = (player->identifier << 16) | player_index;
                    short score = game_engine_get_statborg()->get_player_stat(index, 1);
                    long place = game_engine_get_player_place(index);
                    game_results_set_final_player_score_and_standing(player_index, score, place);
                    if (!TEST_BIT(player->flags, 1))
                    {
                        game_results_statistic_set(player_index, _game_team_none, _game_results_statistic_games_completed, 1);
                        if (!tied && game_engine_get_player_place(index) == 0)
                            game_results_statistic_set(player_index, _game_team_none, _game_results_statistic_games_won, 1);
                    }
                }
            }
        }
    }
}

static __forceinline long game_engine_24_first_user(void)
{
    long result = -1;
    for (long i = 0; i < 4; i++) {
        if (player_mapping_globals->player_indices[i] != -1) {
            result = i;
            break;
        }
    }
    return result;
}

// @address 0x20a588 ?game_engine_update_player_states@@YAXXZ
void game_engine_update_player_states(void)
{
    long user = game_engine_24_first_user();
    if (user == -1)
        return;
    do {
        long handle = player_mapping_globals->player_indices[user];
        if (handle != -1) {
            long id = game_engine_get_player_state_index(handle);
            s_local_game_engine_globals *cache = g_local_game_engine_globals;
            if (id != cache->player_states[user].index) {
                long i = 0;
                long next = user + 2;
                const s_multiplayer_runtime_globals_definition *definition = (const s_multiplayer_runtime_globals_definition *)
                    ((const s_multiplayer_globals_definition *)tag_get('mulg', global_game_globals->multiplayer_globals_index))->runtime.address;
                cache->player_states[user].index = id;
                cache->player_states[next - 2].reserved = 0;
                cache->player_states[user].state_response_index = -1;
                long count = definition->state_responses.count;
                if (count > 0) {
                    const short *element = (const short *)definition->state_responses.address + 2;
                    do {
                        if (*element == id) {
                            cache->player_states[user].state_response_index = i;
                            break;
                        }
                        i++;
                        element += 18;
                    } while (i < count);
                }
            }
        }
        user = players_next_active_user(user);
    } while (user != -1);
}

/* 0x2068d8 game_engine_game_starting */

static __forceinline bool game_engine_starting_is_distributed(signed char simulation)
{
    bool result = false;
    switch (simulation)
    {
    case 4:
    case 5:
        result = true;
        break;
    }
    return result;
}
// @address 0x2068d8 ?game_engine_game_starting@@YAXXZ
void game_engine_game_starting(void)
{
    if (current_game_engine_entry()) {
        unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\game\\game_engine.cpp", 755);
        bool predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (!predicted)
            data_mine_insert_multiplayer_game_options("game starting", current_game_variant());
        simulation_action_game_engine_globals_create();
        simulation_action_game_statborg_create();
        if (game_engine_sandbox_is_active()) {
            bool ok = true;
            signed char simulation = (signed char)game_globals->options.game_simulation;
            if (simulation == 2 || simulation == 4)
                ok = false;
            if (ok) {
                ok = game_engine_starting_is_distributed(simulation);
                if (ok) {
                    for (long i = 0; i < 80; i++)
                        simulation_action_sandbox_group_create(i);
                }
            }
        }
        for (long i = 0; i < 16; i++)
            simulation_action_game_engine_player_create((short)i);
        simulation_action_breakable_surfaces_create();
        bool predicted2 = false;
        if (game_globals->options.game_simulation == 4)
            predicted2 = true;
        if (!predicted2) {
            if (game_globals->options.game_mode == 2)
                game_engine_spawn_variant_objects(&game_globals->options.map_variant);
            if (current_game_engine_entry()) {
                game_engine_build_allowable_team_designators();
                game_engine_build_teams();
            }
            game_engine_initialize_for_new_round();
        }
        long first_user = -1;
        long *user_slot = player_mapping_globals->player_indices;
        for (long i = 0; i < 4; i++, user_slot++) {
            if (*user_slot != -1) {
                first_user = i;
                break;
            }
        }
        for (long user = first_user; user != -1; user = players_next_active_user(user)) {
            game_engine_globals->user_fade[user] = 1.0f;
            game_engine_globals->pending_flags |= 1 << user;
        }
        game_engine_globals->game_engine_state = 1;
        game_engine_globals->desired_game_engine_state = 1;
        bool predicted3 = false;
        if (game_globals->options.game_simulation == 4)
            predicted3 = true;
        if (predicted3)
            game_engine_client_respond_to_new_state((short)game_engine_globals->game_engine_state);
        bool predicted4 = false;
        if (game_globals->options.game_simulation == 4)
            predicted4 = true;
        if (!predicted4) {
            s_game_options *options = &game_globals->options;
            if (options->map_variant.map_id != -1 &&
                map_variant_is_valid((const s_map_variant_validation *)&options->map_variant))
                game_engine_apply_map_variant_quotas(&game_engine_globals->map_info, (const s_game_engine_map_variant_quotas *)&options->map_variant);
        }
        get_current_entry()->game_starting();
        unlock_resources_and_resume_render_thread(lock);
    }
}

// @address 0x206c00 game_engine_game_ending
extern "C" void game_engine_game_ending(void)
{
    if (current_game_engine_entry()) {
        unsigned long lock = ::_internal_halt_render_thread_and_lock_resources("..\\game\\game_engine.cpp", 841);
        c_game_engine *engine = get_current_entry();
        engine->game_ending();
        simulation_action_game_engine_globals_delete();
        simulation_action_game_statborg_delete();
        if (game_engine_sandbox_is_active() != 0)
            simulation_action_sandbox_groups_delete();
        long player_index = 0;
        do {
            simulation_action_game_engine_player_delete((short)player_index);
            player_index++;
        } while (player_index < 16);
        unlock_resources_and_resume_render_thread(lock);
    }
}


// @address 0x20c798 game_engine_boot_player
extern "C" void game_engine_boot_player(long player_index)
{
    s_data_array *players = player_data;
    if (player_index == -1)
        return;
    long player_absolute_index = player_index & 0xffff;
    if (player_absolute_index >= players->first_unallocated)
        return;
    player_datum *player = (player_datum *)(players->data + players->size * player_absolute_index);
    if (player->identifier == 0 || player->identifier != (player_index >> 16))
        return;
    s_game_engine_event_data event;
    game_engine_initialize_event(0, 39, &event);
    event.effect_player_index = player_index;
    event.effect_team_index = ((player_datum *)(players->data + player_absolute_index * sizeof(player_datum)))->configuration.host.team_index;
    game_engine_send_event(&event);
    if (game_is_cooperative())
        return;
    c_simulation_watcher *watcher = g_simulation_globals.watcher;
    long peer = watcher->m_session->get_session_membership()->get_peer_from_unique_identifier(
        (const s_transport_unique_identifier *)&player->machine);
    if (peer == -1)
        return;
    watcher->m_session->host_boot_machine(peer, _network_session_boot_reason_player_booted_player);
}


// @address 0x39d880 ?network_get_team_color@@YA?ATreal_rgb_color@@W4e_game_team@@@Z

void game_engine_get_change_colors(long player_index, const s_player_appearance *appearance, e_game_team team, real_rgb_color *colors, bool *valid)
{
    bool running = game_engine_running();
    bool changed = false;
    // match: 0x39d880: pointer __restrict qualifier prevents alias reload
    real_rgb_color *__restrict cl = colors;
    if (running)
        changed = true;
    network_get_player_colors(team, false, (const c_enum<e_player_color_index, char, -1, 32> *)&appearance->primary_color, colors);
    if (running && game_globals->options.game_mode == 2 && player_index != -1)
    {
        player_datum *player = player_get(player_index);
        bool has_teams = game_engine_has_teams();
        short forced_color = player->traits.m_appearance_traits.m_forced_change_color;
        if (forced_color >= 2 && forced_color <= 13)
        {
            e_game_team team = (e_game_team)(forced_color - 2);
            real_rgb_color V0 = network_get_team_color(team);
            real_rgb_color V1 = V0;
            real_rgb_color V2 = V1;
            real_rgb_color V3 = V2;
            for (long index = 0; index < 4; index++)
            {
                if (index == 0)
                    cl[index] = V2;
                else if (index == 1)
                    cl[index] = V3;
                else
                {
                    cl[index].red = (cl[index].red + V1.red * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].red + V1.red * 0.5f);
                    cl[index].green = (cl[index].green + V1.green * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].green + V1.green * 0.5f);
                    cl[index].blue = (cl[index].blue + V1.blue * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].blue + V1.blue * 0.5f);
                }
            }
            changed = true;
        }
        long aura = player->traits.m_appearance_traits.m_aura;
        if (aura != 2)
        {
            if (aura != 3)
            {
                if (aura == 4)
                {
                    float aura_scale = 4.0f;
                    float red_aura_scale = aura_scale;
                    float green_aura_scale = aura_scale;
                    float blue_aura_scale = aura_scale;
                    for (long index = 0; index < 4; index++)
                    {
                        cl[index].red = (cl[index].red * red_aura_scale) > 1.0f
                            ? 1.0f : (cl[index].red * red_aura_scale);
                        cl[index].green = (cl[index].green * green_aura_scale) > 1.0f
                            ? 1.0f : (cl[index].green * green_aura_scale);
                        cl[index].blue = (cl[index].blue * blue_aura_scale) > 1.0f
                            ? 1.0f : (cl[index].blue * blue_aura_scale);
                    }
                    changed = true;
                }
            }
            else
            {
                float red_dim_scale = 0.1f;
                float green_dim_scale = 0.1f;
                float blue_dim_scale = 0.1f;
                for (long index = 0; index < 4; index++)
                {
                    cl[index].red = cl[index].red * red_dim_scale;
                    cl[index].green = cl[index].green * green_dim_scale;
                    cl[index].blue = cl[index].blue * blue_dim_scale;
                }
                changed = true;
            }
        }
        else if (has_teams)
        {
            e_game_team team = (e_game_team)player->configuration.host.team_index;
            const real_rgb_color &team_color = network_get_team_color(team);
            real_rgb_color W0 = team_color;
            for (long index = 0; index < 4; index++)
            {
                if (index == 0)
                    cl[index] = team_color;
                else
                {
                    cl[index].red = (cl[index].red + W0.red * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].red + W0.red * 0.5f);
                    cl[index].green = (cl[index].green + W0.green * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].green + W0.green * 0.5f);
                    cl[index].blue = (cl[index].blue + W0.blue * 0.5f) > 1.0f
                        ? 1.0f : (cl[index].blue + W0.blue * 0.5f);
                }
            }
            changed = true;
        }
    }
    *valid = changed;
}
//END-PENDING 0x20af70
// @address 0x20af70 ?game_engine_get_change_colors@@YAXJPBUs_player_appearance@@W4e_game_team@@PATreal_rgb_color@@PA_N@Z

/* natural 0x491c10 / 0x20c4a0: per-player fade (screen effect) state machine */
static __forceinline unsigned char game_engine_fade_timer_ticks(void)
{
    return (unsigned char)game_engine_seconds_to_ticks_round(0.5f);
}
// @address 0x491c10 game_engine_fade_set_state
extern "C" s_game_engine_fade_entry *game_engine_fade_set_state(s_game_engine_fade_entry *entry, signed char state)
{
    if (state == 0)
    {
        entry->state = state;
        entry->queued_state = 0;
        entry->timer = 0;
    }
    else if ((entry->state) == 0)
    {
        entry->state = state;
        entry->timer = game_engine_fade_timer_ticks();
    }
    else if (state != (entry->state))
    {
        unsigned char queued_raw = entry->queued_state;
        signed char queued = (signed char)queued_raw;
        if (state != queued)
        {
            if (queued == 0 || (entry->state) == 3)
                entry->queued_state = state;
            else
            {
                entry->state = queued_raw;
                entry->timer = game_engine_fade_timer_ticks();
                entry->queued_state = state;
            }
        }
    }
    else if (state == 3 || entry->queued_state != 3)
        entry->timer = game_engine_fade_timer_ticks();
    return entry;
}

// @address 0x20c4a0 game_engine_player_set_fade_state
extern "C" void game_engine_player_set_fade_state(unsigned short player_index, signed char state)
{
    bool replay = false;
    long index = player_index;
    s_game_engine_fade_entry *entry = &game_engine_globals->player_fades[index];
    if (game_globals->options.game_simulation == 4)
        replay = true;
    if (!replay)
    {
        long previous_state = entry->state;
        entry = game_engine_fade_set_state(entry, state);
        if (previous_state != entry->state)
            ::simulation_action_game_engine_player_update((short)index, 0x40);
    }
}

static __forceinline byte game_engine_pin_quota(byte value, byte minimum, byte maximum)
{
    return MIN(MAX(value, minimum), maximum);
}
// @address 0x1ced38 game_engine_apply_map_variant_quotas
extern "C" void game_engine_apply_map_variant_quotas(s_game_engine_map_info *info, const s_game_engine_map_variant_quotas *map_variant)
{
    if (map_variant->quota_count > 0)
    {
        long index = 0;
        do
        {
            s_variant_quota *quota = &info->quotas[index];
            const s_variant_quota *source = &map_variant->quotas[index];
            quota->maximum_count = MIN(MAX(source->maximum_count, quota->placed_on_map), quota->maximum_allowed);
            quota->minimum_count = MIN(MAX(source->minimum_count, 0), quota->placed_on_map);
            ++index;
        } while (index < 256);
    }
}

/* ---- hud interface state (0x20b4b8) ---------------------------------------- */

static __forceinline long game_engine_get_current_talker(long user_index)
{
    long talker_index = -1;
    short best_talk_time = 0x7fff;
    const short minimum_talk_time = 4;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        long player_absolute_index = iterator.get_index() & 0xffff;
        long talk_time = ((s_local_game_engine_globals *)g_local_game_engine_globals)->user_talker_data[user_index].talk_time[player_absolute_index];
        if (talk_time >= minimum_talk_time && talk_time < best_talk_time)
        {
            best_talk_time = (short)talk_time;
            talker_index = iterator.get_index();
        }
    }
    return talker_index;
}

static __forceinline long game_engine_hud_round_time_get(void)
{
    if (game_time_get() - game_engine_globals->round_start_time < 0)
        return 0;
    return game_time_get() - game_engine_globals->round_start_time;
}

static __forceinline unsigned long game_engine_real_rgb_color_to_pixel32(const real_rgb_color *color)
{
    return ((((int)(color->red * 255.0f) << 8) | 0xff0000 | (int)(color->green * 255.0f)) << 8) | (int)(color->blue * 255.0f);
}

// @address 0x20b4b8 ?game_engine_get_hud_interface_state@@YA_NJPAUgame_engine_interface_state@@@Z
bool game_engine_get_hud_interface_state(long user_index, game_engine_interface_state *state)
{
    c_game_statborg *statborg = game_engine_statborg_in_range();
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_indices[user_index];
    bool result = true;
    memset(state, 0, sizeof(*state));
    state->game_type = game_engine_get_type();
    if (player_index != -1 && statborg)
    {
        player_datum *player = player_get(player_index);
        bool combined = false;
        if (game_engine_get_score_to_win_round() == 1)
            combined = true;
        long talker_index = game_engine_get_current_talker(user_index);
        if (talker_index != -1)
        {
            player_datum *talker = player_get(talker_index);
            state->talker_valid = true;
            usnzprintf(state->talker_name, 32, L"%s", talker->configuration.host.name);
        }
        if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16)
        {
            usnzprintf(state->variant_name, 32, L"%s", current_game_variant()->name);
            state->sudden_death_active = game_engine_check_round_time_limit_seconds() != 0;
            game_engine_get_time_string(state->sudden_death_text);
            state->show_time_remaining = game_engine_hud_round_time_get() < game_tick_rate() * 5;
            if (combined)
            {
                unsigned char round_limit = current_game_variant()->round_limit;
                state->score_to_win = game_engine_get_score_to_win_round() * round_limit;
            }
            else
                state->score_to_win = game_engine_get_score_to_win_round();
            if (game_engine_has_teams())
            {
                long own_score = game_engine_get_scores_player_score(player->configuration.host.team_index, combined);
                long best_team = -1;
                long best_score = -0x8000;
                for (long team_index = 0; team_index < 8; team_index++)
                {
                    if (team_index != player->configuration.host.team_index && game_engine_is_team_active((e_game_team)team_index))
                    {
                        long score = game_engine_get_scores_player_score(team_index, combined);
                        if (score > best_score)
                        {
                            best_team = team_index;
                            best_score = score;
                        }
                    }
                }
                if (best_team != -1)
                {
                    state->rival_valid = true;
                    state->rival_score = best_score;
                    real_rgb_color color = network_get_team_color((e_game_team)best_team);
                    state->rival_color = game_engine_real_rgb_color_to_pixel32(&color);
                    state->leading = own_score > best_score;
                }
                else
                {
                    state->rival_valid = false;
                    state->leading = true;
                }
                state->stats_valid = true;
                state->own_score = own_score;
                real_rgb_color own_color = network_get_team_color((e_game_team)player->configuration.host.team_index);
                state->rival_index = best_team;
                state->own_color = game_engine_real_rgb_color_to_pixel32(&own_color);
            }
            else
            {
                long own_score = ::game_engine_get_player_score(player_index, combined);
                long best_player = -1;
                long best_score = -0x8000;
                c_player_in_game_iterator iterator;
                iterator.begin();
                while (iterator.next())
                {
                    if (iterator.get_index() != player_index)
                    {
                        long score = ::game_engine_get_player_score(iterator.get_index(), combined);
                        if (score > best_score)
                        {
                            best_player = iterator.get_index();
                            best_score = score;
                        }
                    }
                }
                if (best_player != -1)
                {
                    real_rgb_color colors[4];
                    player_datum *rival = player_get(best_player);
                    state->rival_valid = true;
                    network_get_player_colors(_game_team_none, false, (const c_enum<e_player_color_index, char, -1,
                        32> *)rival->configuration.client.appearance.change_colors, colors);
                    state->rival_color = game_engine_real_rgb_color_to_pixel32(&colors[0]);
                    state->rival_score = best_score;
                    state->leading = own_score > best_score;
                }
                else
                {
                    state->rival_valid = false;
                    state->leading = true;
                }
                state->stats_valid = true;
                real_rgb_color colors[4];
                network_get_player_colors(_game_team_none, false, (const c_enum<e_player_color_index, char, -1,
                    32> *)player->configuration.client.appearance.change_colors, colors);
                state->own_color = game_engine_real_rgb_color_to_pixel32(&colors[0]);
                state->own_score = own_score;
                state->rival_index = best_player;
            }
        }
        else
        {
            result = false;
        }
        const c_game_engine *engine = current_game_engine();
        if (engine)
            engine->get_hud_interface_state(player_index, state);
        if (state->sudden_death)
        {
            c_static_wchar_string<256> text;
            text.clear();
            s_multiplayer_globals_definition *multiplayer_globals = (s_multiplayer_globals_definition *)(tag_get('mulg',
                global_game_globals->multiplayer_globals_index));
            string_list_get_normal_string(((s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address)->strings_index,
                0x4000b, &text);
            if (state->sudden_death_active)
            {
                ustrnzcat(text.m_string, state->sudden_death_text, 256);
                text.m_string[255] = 0;
            }
            ustrnzcpy(state->sudden_death_text, text.m_string, 20);
        }
        if (state->score_to_win <= 0)
        {
            state->score_to_win = 1;
            if (state->stats_valid)
            {
                long score = state->own_score;
                if (score < 1)
                    score = 1;
                state->score_to_win = score;
            }
            if (state->rival_valid)
            {
                long score = state->score_to_win;
                long rival = state->rival_score;
                if (score <= rival)
                    score = rival;
                state->score_to_win = score;
            }
        }
    }
    else
    {
        result = false;
    }
    return result;
}

class c_observer_director;

// @address 0x4ecb0 k_boot_betrayer_screen_message_vtable

static __forceinline short game_engine_user_scoreboard_state(long user_index)
{
    short state = -1;
    if (user_index != -1)
        state = ((s_player_control_globals *)player_control_globals)->states[user_index].action.zoom;
    return state;
}

static __forceinline bool game_engine_player_was_betrayed(long controller)
{
    bool betrayed = false;
    if (!game_is_cooperative())
        betrayed = g_game_grief_globals.player_betrayed[controller];
    return betrayed;
}
// @address 0x12c708 game_engine_update_scoreboard
extern "C" void game_engine_update_scoreboard(float param)
{
    game_globals_storage *globals = game_globals;
    bool ready = false;
    if (globals != 0 && globals->game_in_progress != 0 && globals->initializing == 0 && globals->map_active != 0)
        ready = true;
    if (ready != 0 && globals->options.game_mode != 3) {
        for (long user = first_active_talker_user(); user != -1; user = players_next_active_user(user)) {
            long player_index = talker_player_from_user(user);
            player_datum *player = player_get(player_index);
            long controller = user_interface_controller_get_rumble_enabled(user);
            if (controller == -1)
                continue;
            const gamepad_state *settings = input_get_gamepad_state((short)controller);
            short tick_rate = game_tick_rate();
            long limit = tick_rate * 3;
            bool enabled = settings != 0 && settings->button_frames[7] != 0;
            bool no_unit = player->unit_index == -1;
            if (current_game_engine() != 0 && !game_engine_in_round()) {
                if (g_local_game_engine_globals->next_stats_report_time == 0x7fffffff)
                    g_local_game_engine_globals->next_stats_report_time = game_time_get() + tick_rate;
            }
            else {
                g_local_game_engine_globals->next_stats_report_time = 0x7fffffff;
            }
            if (game_is_scoreboard_blocked(0)) {
                c_gui_screen_scoreboard::hide_scoreboard((e_controller_index)controller);
            }
            else {
                bool handled = false;
                c_gui_screen_scoreboard *screen = c_gui_screen_scoreboard::get_scoreboard_screen((e_controller_index)controller);
                if (screen != 0) {
                    if (g_window_manager.get_screen_above(screen->get_render_window(), screen) != 0)
                        handled = true;
                }
                if (handled == 0) {
                    bool show = enabled && game_engine_user_scoreboard_state(user) == -1;
                    if (current_game_engine() != 0) {
                        long now = game_time_get();
                        if (now >= g_local_game_engine_globals->next_stats_report_time)
                            show = true;
                        if (no_unit && player->respawn_seconds <= 1) {
                            unsigned long flags = player->flags;
                            if ((flags & 0x1000) == 0 && !game_engine_player_is_out_of_lives(player_index) && now > limit && (flags & 1) != 0) {
                                unsigned char ok = game_is_cooperative()
                                    && g_director_globals->directors[user].get_director()->get_type() == 1
                                    && observer_director_is_free_camera((c_observer_director *)&g_director_globals->directors[user]);
                                if (!ok)
                                    show = true;
                            }
                        }
                    }
                    if (show)
                        c_gui_screen_scoreboard::show_scoreboard((e_controller_index)controller, enabled);
                    else
                        c_gui_screen_scoreboard::hide_scoreboard((e_controller_index)controller);
                }
            }
            gui_screen_scoreboard_save_text_alpha(controller);
            if (current_game_engine() != 0) {
                long other_index = -1;
                if ((unsigned char)game_engine_try_and_get_betrayer((unsigned long)player_index, &other_index) != 0 &&
                    game_grief_can_boot_betrayer(player_index))
                {
                    if (!game_engine_player_was_betrayed(controller)) {
                        player_datum *other = player_get(other_index);
                        long name = player_get_user_from_controller(controller);
                        c_load_boot_betrayer_screen_message *message =
                            (c_load_boot_betrayer_screen_message *)user_interface_malloc_tracked(
                                sizeof(c_load_boot_betrayer_screen_message), 0, 0);
                        if (message != 0) {
                            long screen_name;
                            if (players_get_active_user_count() > 1)
                                screen_name = _string_id_boot_betrayer_splitscreen;
                            else
                                screen_name = _string_id_boot_betrayer;
#if defined(_XBOX) && !defined(__clang__)
                            new (message) c_load_screen_message(screen_name, (e_controller_index)controller, (e_window_index)name, _string_id_top_most);
                            // match: 0x12c708: the real ctor hoists the vtable lis/addi above input_globals and g_game_grief_globals
                            *reinterpret_cast<char **>(message) = k_boot_betrayer_screen_message_vtable;
                            message->set_current_player(player->player_identifier);
                            message->set_target_player(other->player_identifier);
#else
                            message = new (message) c_load_boot_betrayer_screen_message(screen_name,
                                (e_controller_index)controller, (e_window_index)name, _string_id_top_most,
                                player->player_identifier, other->player_identifier);
#endif
                            ::user_interface_messaging_post(message);
                            if (!game_is_cooperative()) {
                                g_game_grief_globals.player_betrayed[controller] = 1;
                                g_game_grief_globals.reprieve_time = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    game_engine_update_global_fade_timers(param);
}


// @address 0x20c538 ?update_player_waypoint_data@@YAXJ@Z
void update_player_waypoint_data(long player_index)
{
    long index = player_index & 0xffff;
    s_game_engine_fade_entry *waypoint = &game_engine_globals->player_fades[index];
    if (!thread_state_test()) {
        long state = waypoint->state;
        game_engine_update_waypoint(waypoint);
        if (state != waypoint->state)
            simulation_action_game_engine_player_update((short)index, 64);
    }
    if (waypoint->countdown_active) {
        waypoint->countdown = MAX(waypoint->countdown - 1, 0);
        if (waypoint->countdown == 0) {
            waypoint->active = 0;
            waypoint->countdown_active = 0;
            waypoint->countdown = 0;
        }
    }
}

/* ---- 0x208080 game_engine_garbage_collect_for_round_switch --------------------- */

static __forceinline bool game_engine_round_switch_predicted(void)
{
    bool result = false;
    if ((signed char)game_globals->options.game_simulation == 4)
        result = true;
    return result;
}

static __forceinline bool game_engine_round_switch_round_reset_map(void)
{
    const c_game_variant *variant = current_game_variant();
    return (variant->get_active_variant()->get_miscellaneous_options()->m_flags.get_unsafe() & (1 << 2)) != 0;
}

static __forceinline short game_engine_round_switch_weapon_type(long object_index)
{
    object_datum *weapon = (object_datum *)object_get(object_index);
    weapon_definition *definition = ((weapon_definition *)tag_get('weap', weapon->definition_index));
    return definition->multiplayer_weapon_type;
}


// @address 0x208080 ?game_engine_garbage_collect_for_round_switch@@YAXXZ
void game_engine_garbage_collect_for_round_switch(void)
{
    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\game\\game_engine.cpp", 0x748);
    bool round_reset_map = game_engine_round_switch_round_reset_map();

    bool predicted = game_engine_round_switch_predicted();
    if (!predicted)
    {
        c_player_with_unit_iterator player_iterator;
        player_iterator.begin();
        while (player_iterator.next())
            objects_purge_deleted_object(player_iterator.datum->unit_index);

        unsigned int type_mask = 0xffffffff;
        if (!round_reset_map)
            type_mask = 0xfffffc2f;

        c_object_iterator<object_datum> iterator;
        iterator.begin(type_mask, 0);
        while (iterator.next())
        {
            long object_index = iterator.get_index();
            long ultimate_parent = -1;
            long current_parent = object_index;
            if (object_index != -1)
            {
                do
                {
                    ultimate_parent = current_parent;
                    current_parent = object_get(current_parent)->parent_object_index;
                } while (current_parent != -1);
            }

            object_datum *object = iterator.get_datum();
            bool keep = true;
            long type = object->identifier_type;
            bool predicted_object = game_engine_sandbox_is_active() != 0 && (unsigned short)object->map_variant_index != 0xffff;
            if (game_engine_monitoring_object(object_index) == 0 && game_engine_monitoring_object(ultimate_parent) == 0)
            {
                switch (type)
                {
                case _object_type_weapon:
                    keep = game_engine_round_switch_weapon_type(object_index) == 0;
                    break;
                case _object_type_equipment:
                    keep = round_reset_map;
                    break;
                case _object_type_crate:
                    keep = round_reset_map || TEST_BIT(object->recycling_flags, 1);
                    break;
                }
            }
            else
            {
                keep = round_reset_map;
            }
            if (keep && !predicted_object)
                objects_purge_deleted_object(object_index);
        }
    }
    else if (game_engine_round_switch_predicted() && round_reset_map)
    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0xffffffff, 0);
        while (iterator.next())
        {
            object_datum *object = iterator.get_datum();
            if (object->parent_object_index == -1 && object->gamestate_index == -1)
            {
                if (!(game_engine_sandbox_is_active() != 0 && (unsigned short)object->map_variant_index != 0xffff))
                    objects_purge_deleted_object(iterator.get_index());
            }
        }
    }

    if (round_reset_map)
    {
        breakable_surfaces_reset();
        effects_reset();

        c_data_iterator<void> iterator;
        iterator.begin(g_device_groups_data);
        while ((iterator.m_datum = data_iterator_next(&iterator.m_iterator)) != 0)
        {
            s_device_group_datum *device_group = (s_device_group_datum *)iterator.m_datum;
            device_group_set_actual_value(iterator.m_iterator.index, device_group->desired_value);
            device_group->flags &= 0xfffd;
        }

        c_data_iterator<void> light_iterator;
        light_iterator.begin(light_data);
        while ((light_iterator.m_datum = data_iterator_next(&light_iterator.m_iterator)) != 0)
        {
            light_datum *light = (light_datum *)light_iterator.m_datum;
            if (light->attachment_object_index == -1)
                light_delete_internal(light_iterator.m_iterator.index, true);
        }

        game_sound_reset_at_round_finish();
        hs_runtime_dispose_from_old_map();
        hs_runtime_initialize_for_new_map();
        game_create_objects(_game_create_mode_dont_lock);
        ::object_placement_create_active_zone_set_objects(0);
    }

    unlock_resources_and_resume_render_thread(lock);
}


// @address 0x20a6c0 ?game_engine_update_global_fade_timers@@YAXM@Z
void game_engine_update_global_fade_timers(float delta)
{
    for (long user_index = 0; user_index < 4; user_index++)
    {
        bool user_active = false;
        if (user_index != -1)
            user_active = player_mapping_globals->player_indices[user_index] != -1;
        if (user_active)
        {
            if (user_index != -1)
            {
                long player_index = player_mapping_globals->player_indices[user_index];
                if (player_index != -1)
                {
                    player_datum *player = player_get(player_index);
                    long ticks_per_second = game_tick_rate();
                    long fade_limit = 4 * ticks_per_second;
                    long round_time = game_engine_round_time_get();
                    s_game_engine_globals *globals = game_engine_globals;
                    bool recent_round_start = true;
                    if (game_time_get() - globals->round_start_time >= fade_limit)
                        recent_round_start = false;
                    bool end_timer_elapsed = true;
                    const long &end_round_timer = globals->end_round_timer;
                    if (end_round_timer <= fade_limit)
                        end_timer_elapsed = false;
                    bool cooperative_starting = simulation_starting_up() && game_is_cooperative() && game_globals->game_finished_state;
                    bool raise_fade = false;
                    long unit_index;
                    bool special_flag;
                    do
                    {
                        if (!simulation_in_progress())
                            { raise_fade = true; break; }
                        if (current_game_engine() == 0)
                            { raise_fade = false; break; }
                        if (recent_round_start)
                        {
                            if (player->unit_index != -1)
                                break;
                            else
                                { raise_fade = true; break; }
                        }
                        if (end_timer_elapsed && !cooperative_starting)
                            { raise_fade = true; break; }
                        unit_index = player->unit_index;
                        if (unit_index == -1 && player->respawn_seconds <= 0 && game_engine_player_is_playing(player_index))
                        {
                            const unsigned int &observer_mode = g_director_globals->slots[user_index].perspective;
                            if (((1u << observer_mode) & 3u) != 0)
                                { raise_fade = true; break; }
                        }
                        if (TEST_BIT(globals->pending_flags, user_index))
                            { raise_fade = true; break; }
                        if (unit_index != -1)
                            break;
                        if (!game_engine_player_is_playing(player_index))
                        {
                            special_flag = false;
                            if (player->configuration.host.team_index != -1)
                                special_flag = ((player->flags >> 3) & 1) != 0;
                            if (!special_flag && round_time > ticks_per_second)
                                break;
                        }
                        const unsigned int &player_flags = player->flags;
                        if ((player_flags & 8) == 0)
                            break;
                        raise_fade = true;
                    } while (false);
                    if (raise_fade)
                    {
                        float value = MIN(globals->user_fade[user_index] + delta, 1.0f);
                        globals->user_fade[user_index] = value;
                    }
                    else
                    {
                        float value;
                        bool snap_to_full = round_time < 5;
                        if (snap_to_full)
                            value = 1.0f;
                        else
                        {
                            value = globals->user_fade[user_index] - delta * 3.0f;
                            if (value < 0.0f)
                                value = 0.0f;
                        }
                        globals->user_fade[user_index] = value;
                    }
                }
            }
            float elapsed_seconds;
            if (simulation_starting_up() && game_is_cooperative() && saved_film_try_and_get_timer_elapsed(&elapsed_seconds))
            {
                float film_fade = MIN(MAX(elapsed_seconds * 0.33333334f, 0.0f), 1.0f);
                if (!(film_fade > game_engine_globals->user_fade[user_index]))
                    film_fade = game_engine_globals->user_fade[user_index];
                game_engine_globals->user_fade[user_index] = film_fade;
            }
        }
        else
        {
            game_engine_globals->user_fade[user_index] = 0.0f;
        }
    }
}


// @address 0xc4020 ?game_engine_update@@YAXXZ
void game_engine_update(void)
{
    if (!current_game_engine_entry())
        return;
    game_engine_update_after_game_update_state();
    game_engine_update_multiplayer_sound();
    game_engine_garbage_collect();
    game_engine_globals->candy_monitor_manager.update();
    game_engine_update_talkers();
    game_engine_teleporters_update();
    c_player_in_game_iterator iterator;
    iterator.begin();
    if (iterator.next()) {
        do {
            game_engine_update_player(iterator.get_index());
        } while (iterator.next());
    }
    game_engine_spawn_influencers_update();
    if (game_engine_has_teams()) {
        for (long team = 0; team < 8; team++) {
            if (game_engine_is_team_active((e_game_team)team))
                game_engine_update_team((e_game_team)team);
        }
    }
    get_current_entry()->update();
    if (!thread_state_test() && game_engine_in_round() && TEST_BIT(game_engine_globals->flags, 0)) {
        s_game_engine_score_list score_list;
        game_engine_scoring_build_score_list(&score_list, 0, false);
        game_engine_scoring_update_leaders_internal((s_game_engine_score_list *)game_engine_globals->leaders, &score_list);
        memcpy(game_engine_globals->leaders, &score_list, sizeof(score_list));
        game_engine_globals->flags &= ~1;
    }
    game_engine_update_player_states();
    if (!thread_state_test() &&
        game_time_get() >= game_tick_rate() * 3 + game_engine_globals->player_update_time) {
        for (long player_index = 0; player_index < 16; player_index++)
            simulation_action_game_engine_player_update((short)player_index, 0x100);
        game_engine_globals->player_update_time = game_time_get();
    }
    game_engine_update_player_sitting_out();
    game_engine_check_for_round_winner();
    bool teams_active = game_engine_running() && (game_engine_globals->flags & 8) != 0;
    if (teams_active && !thread_state_test()) {
        if ((game_engine_globals->valid_team_mask & ~game_engine_globals->initial_team_mask) != 0 ||
            (game_engine_globals->ever_active_teams & ~game_engine_globals->valid_team_mask) != 0 ||
            (game_engine_globals->active_teams & ~game_engine_globals->ever_active_teams) != 0)
            game_engine_end_round_with_winner(-1, true, game_engine_end_condition_game_end_invalid_team_mapping);
    }
    long new_timer;
    if (game_engine_in_round())
        new_timer = 0;
    else
        new_timer = game_engine_globals->end_round_timer + 1;
    game_engine_globals->end_round_timer = (short)new_timer;
    game_engine_events_update();
}

static __forceinline void game_engine_variant_restore_parent(s_game_engine_globals *globals, long object_index,
    const s_variant_object_datum *source)
{
    globals->map_info.objects[object_index].parent_object_identifier = source->parent_object_identifier;
}

static __forceinline void game_engine_variant_restore_position(s_game_engine_globals *globals, long object_index,
    const s_variant_object_datum *source)
{
    globals->map_info.objects[object_index].position = source->position;
}

static __forceinline long game_engine_variant_scenario_engine(long engine_type)
{
    long result = 10;
    switch (engine_type) {
    case 1: result = 0; break;
    case 2: result = 1; break;
    case 3: result = 2; break;
    case 4: result = 3; break;
    case 6: result = 7; break;
    case 7: result = 4; break;
    case 8: result = 5; break;
    case 9: result = 6; break;
    case _object_type_crate: result = 8; break;
    }
    return result;
}

// @address 0x1d2e28 game_engine_spawn_variant_objects
extern "C" void game_engine_spawn_variant_objects(void *map_variant_data)
{
    c_map_variant *map_variant = static_cast<c_map_variant *>(map_variant_data);
    if (map_variant_is_valid((const s_map_variant_validation *)map_variant) &&
        map_variant->m_number_of_variant_objects > map_variant->m_number_of_scenario_objects) {
        for (long object_index = map_variant->m_number_of_scenario_objects;
            object_index < map_variant->m_number_of_variant_objects; ++object_index) {
            s_variant_object_datum object = map_variant->objects[object_index];
            if (!object.flags.is_empty()) {
                unsigned short owner_flags = 0;
                if (object.flags.test(_variant_object_placement_spawn_from_object_bit)) {
                    owner_flags = 0x100;
                    game_engine_variant_restore_parent(game_engine_globals, object_index, &object);
                    game_engine_variant_restore_position(game_engine_globals, object_index, &object);
                    if (object.flags.test(_variant_object_placement_spawn_relative_bit))
                        owner_flags = 0x300;
                }
                long definition_index = -1;
                bool create_object = object.quota_index != -1;
                if (create_object) {
                    definition_index = map_variant->quotas[object.quota_index].object_definition_index;
                    if (!game_engine_sandbox_is_active()) {
                        create_object &=
                            (game_engine_has_symmetric_rules() && object.properties.placement_flags.test(_variant_placement_symmetric_bit)) ||
                            (game_engine_has_asymmetric_rules() && object.properties.placement_flags.test(_variant_placement_asymmetric_bit));
                        long scenario_engine = game_engine_variant_scenario_engine(game_engine_get_type());
                        create_object &= object.properties.game_engine_flags.test(static_cast<e_scenario_game_engine>(scenario_engine));
                        if (game_get_grenade_type_index_from_item_defintion(definition_index) != -1)
                            create_object &= game_engine_has_grenades_enabled();
                        const s_object_definition *definition =
                            static_cast<const s_object_definition *>(tag_get('obje', definition_index));
                        switch (definition->object_type) {
                        case _object_type_vehicle:
                            definition_index = game_engine_remap_vehicle_definition_index(definition_index);
                            break;
                        case _object_type_weapon:
                            definition_index = game_engine_remap_weapon_definition_index(definition_index);
                            break;
                        }
                    }
                    create_object &= definition_index != -1;
                }
                if (create_object)
                    map_variant_new_object(&game_engine_globals->map_info, definition_index, 1, object_index,
                        &object.position, &object.forward, &object.up, -1, _object_type_none, &object.properties, owner_flags);
            }
        }
    }
}


// @address 0x206560 ?game_engine_initialize_for_new_round@@YAXXZ
void game_engine_initialize_for_new_round(void)
{
    unsigned long lock = _internal_halt_render_thread_and_lock_resources("..\\game\\game_engine.cpp", 620);
    s_game_engine_globals *globals = game_engine_globals;
    if (current_game_engine_entry() != 0)
    {
        memset(globals->leaders, 0, sizeof(globals->leaders));
        globals->timer_seconds = current_game_variant()->round_time_limit_minutes * 60;
        if (game_engine_globals->round_index > 0)
        {
            game_engine_garbage_collect_for_round_switch();
            if (current_game_engine()->get_type() != 5)
            {
                bool reset_map = false;
                if (game_globals->options.game_simulation == 4)
                    reset_map = true;
                if (!reset_map)
                    game_engine_spawn_variant_objects(&game_engine_globals->map_info);
            }
            bool in_game = false;
            if ((unsigned long)((signed char)game_globals->options.game_simulation - 4) <= 1)
                in_game = true;
            if (in_game && !game_is_cooperative())
                g_simulation_globals.world->drop_simulation_from_active_to_joining();
        }
        bool predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (!predicted)
        {
            map_variant_update_object_parents((c_map_variant *)&game_engine_globals->map_info);
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next())
            {
                long player_index = iterator.get_index();
                player_datum *player = (player_datum *)(player_data->data + (unsigned short)player_index * (long)sizeof(player_datum));
                long absolute_index = (unsigned short)player_index;
                s_game_engine_fade_entry *waypoint = &game_engine_globals->player_fades[absolute_index];
                player->respawn_penalty_ticks = 0;
                player = (player_datum *)(player_data->data + (unsigned short)player_index * (long)sizeof(player_datum));
                player->respawn_timer = 0;
                waypoint->active = 0;
                simulation_action_game_engine_player_update((short)absolute_index, 1);
            }
            game_engine_reset_stats_for_round_switch();
            if (g_game_results_globals.active)
                g_game_results_globals.finished = false;
        }
#ifdef HALO3_64BIT
        char team_lives = current_game_variant()->team_lives_per_round;
#else
        char team_lives = current_game_variant()->variant_storage[0x106];
#endif
        for (long i = 0; i < 8; i++)
            game_engine_globals->team_lives_22[i] = team_lives;
        game_engine_reset_player_unit_timers_for_new_round();
        if (game_engine_has_teams())
            game_engine_build_valid_team_mapping();
        get_current_entry()->initialize_for_new_round();
        unsigned long remaining = 64;
        s_dead_player_info *dead_player = &game_engine_globals->dead_players[0];
        do
        {
            remaining--;
            dead_player->player_index = -1;
            ++dead_player;
        } while (remaining != 0);
    }
    unlock_resources_and_resume_render_thread(lock);
}

c_local_game_engine_globals_allocation_manager g_local_game_engine_globals_allocator;
c_game_engine_globals_allocation_manager g_game_engine_globals_allocator;

// @address 0x11df664 game_engine_globals
__declspec(thread) s_game_engine_globals *game_engine_globals;

// @address 0x11df778 g_local_game_engine_globals
extern "C" { __declspec(thread) s_local_game_engine_globals *g_local_game_engine_globals; }
