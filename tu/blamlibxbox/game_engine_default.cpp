// blamlibxbox/game_engine_default.obj: from the records 0x2e0f20 0x2e0f78 0x2e1138 0x2e11d0 0x2e1300 0x2e16a8 0x2e1798 0x2e1a38 0x2e1a70 0x2e1a98 0x2e1bd8 0x2e1c00 0x2e1c60 0x2e1d18 0x2e1d20 0x2e1de8 0x2e1e50 0x2e1ef8 0x2e24f0 0x2e29a8 0x2e2a00 0x2e2cb8 0x2e2e28 0x2e2f50 0x2e3038 0x2e5cf0
#include "math/random_math_range.h"
#include "memory/data_absolute_index.h"
#include "game/game_results_storage_types.h"
#include "game/game_engine_waypoint_constants.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_variant_inline.h"
#include "scenario/scenario_game_globals.h"
#include "memory/data.h"
#include "camera/director.h"
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"
#include "game/game_engine_kill_flags_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "units/unit_types.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "units/units.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "networking/online/voice_xenon.h"
#include "game/game_engine_sandbox.h"
#include "game/player_traits.h"
#include "game/player_configuration.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "game/game_simulation_inline.h"
#include "interface/chud/chud.h"
#include "units/vehicle_utilities.h"
#include "game/game_globals.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/team_scoring_method_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/statborg_entry_types.h"
#include "simulation/simulation_player_netdebug_data_types.h"
#include "cache/cache_files.h"
#include "networking/online/voice_interface_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "game/player_mapping_types.h"
#include "math/real_math_globals.h"
#include "units/unit_facing.h"
#include "game/game_engine_base_variant.h"
#include "game/game_globals_storage.h"
#include "interface/chud/chud_navpoint_types.h"
#include "game/game_engine_default.h"
#include "simulation/simulation_entity_types.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_default_helpers.h"
#include "game/player_in_game_iterator.h"
#include "game/game_engine_list.h"
#include "math/real_math_functions.h"
#include "main/levels.h"
#include "main/levels_globals.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
// @flags /TP /O2
// @address 0x2e1bd8 ?team_is_enemy@c_game_engine@@UBA_NW4e_game_team@@0@Z
// @address 0x2e1d18 ?score_header_string@c_game_engine@@UBAXPAV?$c_static_wchar_string@$0BAA@@@@Z
// @address 0x2e1a38 ?assemble_baseline_traits_for_player@c_game_engine@@UBAXJPAVc_player_traits@@@Z
// @address 0x2e1a70 ?get_player_team_designator@c_game_engine@@UBA?AW4e_multiplayer_team_designator@@J@Z
// @address 0x278110 ?game_engine_team_index_to_team_designator@@YA?AW4e_multiplayer_team_designator@@W4e_game_team@@@Z
// @address 0x2e1de8 ?get_message_chud_reference@c_game_engine@@UBAJXZ
// @address 0x2e29a8 ?build_global_baseline@c_game_engine@@UBAXPAUs_game_engine_state_data@@@Z
// @address 0x2e5cf0 ?get_player_state_index@c_game_engine@@UBAJJPA_N@Z
// @address 0x2e0f20 ?get_score_to_win_round@c_game_engine@@UBAJXZ
// @address 0x2e1c60 game_engine_handle_player_killed
// @address 0x2e2f50 game_engine_update_respawn_growth
// @address 0x2e3038 game_engine_grant_kill_achievements

enum e_director_perspective;




struct s_game_engine_player_state_data
{
    unsigned char b0;
    long l4;
    long l8[3];
    c_player_traits traits;
    unsigned char b30;
    short lives_remaining;
    long last_betrayer;
    short s38;
    unsigned char b3a;
    unsigned char b3b;
    unsigned char b3c;
    short desired_respawn_player;
    real_vector3d forward;
    long l4c[4];
};

struct s_game_engine_player_update_data
{
    unsigned char waypoint_state;
    long respawn_timer_ticks;
    real_point3d waypoint_position;
    c_player_trait_shield_vitality shield_vitality_traits;
    c_player_trait_weapons weapon_traits;
    c_player_trait_movement movement_traits;
    c_player_trait_appearance appearance_traits;
    c_player_trait_sensors sensor_traits;
    bool blocking_teleporter;
    short lives_remaining;
    long last_betrayer;
    unsigned short respawn_timer;
    bool vehicle_entrance_ban;
    bool active_in_game;
    bool sitting_out;
    short desired_respawn_player;
    real_vector3d player_control_aiming;
    s_simulation_player_netdebug_data timers;
};

namespace {
}



namespace {

}






struct s_player_datum;

static __forceinline void default_player_iterator_begin(c_player_in_game_iterator *iterator)
{
    iterator->m_iterator.m_iterator.absolute_index = -1;
    iterator->m_iterator.m_iterator.index = -1;
    iterator->m_iterator.m_iterator.data = tls_player_data();
}
struct s_game_engine_globals;

namespace {

static __forceinline e_game_team default_team_increment(e_game_team &team)
{
    e_game_team previous_team = team;
    team = (e_game_team)(team + 1);
    return previous_team;
}
static __forceinline e_game_team default_team_index_from_designator(e_multiplayer_team_designator designator)
{
    return (e_game_team)((s_game_engine_globals *)tls_game_engine_globals())->team_mapping[designator];
}
static __forceinline const c_game_variant *default_current_game_variant()
{
    game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline player_datum *default_player_get(long player_index)
{
    return &((player_datum *)((s_data_array *)tls_player_data())->data)[(unsigned short)player_index];
}
static __forceinline void default_reset_player_respawn_timer(long player_index)
{
    long absolute_index = (unsigned short)player_index;
    ((player_datum *)((s_data_array *)tls_player_data())->data)[absolute_index].respawn_timer = 0;
    ((s_game_engine_globals *)tls_game_engine_globals())->player_waypoints[absolute_index].active = 0;
    ::simulation_action_game_engine_player_update((short)absolute_index, 1);
}
}

// 0x2e0f20
long c_game_engine::get_score_to_win_round() const
{
    long score_to_win = 0;
    if (current_game_variant())
        score_to_win = current_game_variant()->get_active_variant()->get_score_to_win_round();
    return score_to_win;
}



namespace {
static __forceinline const c_game_engine *default_current_game_engine()
{
    long index = tls_game_engine_globals()->game_engine_index;
    if (index < 1)
        return 0;
    if (index > 10)
        return 0;
    return game_engines[index];
}
static __forceinline c_game_statborg *default_game_engine_get_statborg()
{
    c_game_statborg *result = 0;
    if (default_current_game_engine())
        result = &tls_game_engine_globals()->statborg;
    return result;
}
}

// @address 0x2e0f78 ?recompute_team_score@c_game_engine@@UBAXW4e_game_team@@JW4e_team_scoring_method@@@Z
void c_game_engine::recompute_team_score(e_game_team team, long score_delta, e_team_scoring_method scoring_method) const
{
    c_game_statborg *statborg = default_game_engine_get_statborg();
    long old_score = statborg->get_team_stat(team, _statborg_entry_score);
    if (scoring_method == _team_scoring_method_none)
        return;
    long new_score;
    if (scoring_method == _team_scoring_method_sum) {
        new_score = old_score + score_delta;
    } else {
        new_score = scoring_method == _team_scoring_method_minimum ? 0x7fffffff : 0x80000000;
        c_player_in_game_iterator player_iterator;
        default_player_iterator_begin(&player_iterator);
        while (player_iterator.next()) {
            if (player_iterator.get_datum()->configuration.host.team_index == team) {
                short player_score = statborg->get_player_stat(player_iterator.get_index(), _statborg_entry_score);
                if (scoring_method == _team_scoring_method_minimum) {
                    if (new_score > player_score)
                        new_score = player_score;
                } else {
                    if (new_score <= player_score)
                        new_score = player_score;
                }
            }
        }
        if (new_score - score_delta != old_score)
            return;
    }
    if (new_score != old_score) {
        statborg->adjust_team_stat(team, _statborg_entry_score, (short)(new_score - old_score), 6);
        current_game_variant()->get_active_variant()->custom_team_score_stats(team, old_score, new_score);
    }
}

// @address 0x2e11d0 ?player_activated@c_game_engine@@UBAXJ@Z
void c_game_engine::player_activated(long player_index) const
{
    emit_game_start_event(player_index);
}



namespace {
static __forceinline unit_datum *default_object_get(long object_index)
{
    return (unit_datum *)(*datum_get<s_object_header>(tls_object_header_data(), object_index)).datum;
}
static __forceinline real_point3d *default_point_from_line3d(const real_point3d *point, const real_vector3d *vector, float t,
    real_point3d *result)
{
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    result->z = point->z + vector->k * t;
    return result;
}
static __forceinline e_voice_lifecycle default_voice_get_lifecycle()
{
    e_voice_lifecycle lifecycle = _voice_lifecycle_none;
    if (g_voice_interface_globals.initialized)
        lifecycle = g_voice_interface_globals.lifecycle;
    return lifecycle;
}
static __forceinline bool default_voice_lifecycle_in_game()
{
    bool in_game = false;
    if (g_voice_interface_globals.initialized) {
        e_voice_lifecycle lifecycle = default_voice_get_lifecycle();
        if (lifecycle > _voice_lifecycle_none && lifecycle <= _voice_lifecycle_in_game_pending)
            in_game = true;
    }
    return in_game;
}
static __forceinline long default_game_time_get()
{
    return tls_game_time_globals()->time;
}
static __forceinline long default_game_seconds_integer_to_ticks(long seconds)
{
    return seconds * tls_game_time_globals()->ticks_per_second;
}
}

// @address 0x2e1300 ?build_player_nav_point@c_game_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z
bool c_game_engine::build_player_nav_point(long user_index, long player_index, long target_player_index, bool target,
    s_chud_navpoint *navpoint) const
{
    long viewing_player_index = player_index_from_user_index_render(user_index);
    s_game_engine_player_waypoint *waypoint = &((s_game_engine_globals *)tls_game_engine_globals())->player_waypoints[(unsigned short)player_index];
    bool result = true;
    player_datum *viewing_player = default_player_get(viewing_player_index);
    player_datum *player = default_player_get(player_index);
    c_static_wchar_string<4> name;
    bool enemy = team_is_enemy((e_game_team)viewing_player->configuration.host.team_index, (e_game_team)player->configuration.host.team_index);
    navpoint->flags = 0;
    navpoint->type = target ? -1 : (enemy ? 1 : 0);
    navpoint->icon = -1;
    navpoint->state = enemy ? 2 : 0;
    navpoint->scale = 1.0f;
    if (target_player_index != -1 && target) {
        long unit_index = player_get(tls_player_data(), target_player_index)->unit_index;
        if (unit_index != -1 && default_object_get(unit_index)->unit.active_camouflage > 0.9f)
            navpoint->scale = 0.0f;
    }
    navpoint->object_index = player_index;
    if (player_index == target_player_index)
        navpoint->flags |= 32;
    else
        navpoint->flags &= ~32;
    if (enemy) {
        navpoint->name[0] = 0;
    } else {
        ustrnzcpy_inline(name.get_buffer(), player->configuration.client.appearance.service_tag, 4);
        name.copy_to(navpoint->name, 4);
    }
    if (player->unit_index != -1) {
        unit_get_head_position(player->unit_index, &navpoint->position);
        default_point_from_line3d(&navpoint->position, global_up3d, 0.05f, &navpoint->position);
        if (!enemy) {
            if (default_voice_lifecycle_in_game() && voice_player_is_talking((unsigned short)player_index)) {
                navpoint->flags |= 16;
            } else {
                switch ((signed char)waypoint->state) {
                case 1: navpoint->flags |= 4; break;
                case 2: navpoint->flags |= 16; break;
                case 3: navpoint->flags |= 8; break;
                default: goto nav_point_complete;
                }
            }
        }
    } else {
        if (waypoint->active && waypoint->ticks) {
            navpoint->position = waypoint->position;
        } else if (player->dead_unit_index != -1 &&
            default_game_time_get() - default_object_get(player->dead_unit_index)->unit.type_change_time < default_game_seconds_integer_to_ticks(3)) {
            unit_get_head_position(player->dead_unit_index, &navpoint->position);
            default_point_from_line3d(&navpoint->position, global_up3d, 0.05f, &navpoint->position);
        } else {
            result = false;
        }
        if (!enemy) {
            navpoint->icon = 1;
            navpoint->state = 3;
        }
    }
nav_point_complete:
    return result;
}

// @address 0x2e16a8 ?should_draw_nav_point@c_game_engine@@UBA_NJJ@Z
bool c_game_engine::should_draw_nav_point(long user_index, long player_index) const
{
    long viewing_player_index = player_index_from_user_index_render(user_index);
    bool result = false;
    if (viewing_player_index != -1 && viewing_player_index != player_index) {
        const player_datum *viewing_player = player_get(tls_player_data(), viewing_player_index);
        const player_datum *player = player_get(tls_player_data(), player_index);
        bool friendly = !team_is_enemy((e_game_team)viewing_player->configuration.host.team_index,
            (e_game_team)player->configuration.host.team_index);
        result = (friendly || player->unit_index != -1) &&
            (player->traits.get_appearance_traits()->get_waypoint_setting() == _waypoint_everyone ||
             friendly && player->traits.get_appearance_traits()->get_waypoint_setting() == _waypoint_allies);
    }
    return result;
}

void c_game_engine::assemble_baseline_traits_for_player(long player_index, c_player_traits *traits) const
{
    const c_game_variant *variant = current_game_variant();
    traits->set(&variant->base_player_traits, false);
}

e_multiplayer_team_designator c_game_engine::get_player_team_designator(long player_index) const
{
    const player_datum *player = player_get(tls_player_data(), player_index);
    return game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
}

// @address 0x2e1a98 ?get_defending_team_index@c_game_engine@@UBA?AW4e_game_team@@XZ
e_game_team c_game_engine::get_defending_team_index() const
{
    long team_count = 0;
    e_game_team teams[16];
    c_player_in_game_iterator player_iterator;
    default_player_iterator_begin(&player_iterator);
    while (player_iterator.next()) {
        if (player_iterator.get_datum()->configuration.host.team_index >= 0 && player_iterator.get_datum()->configuration.host.team_index < 16) {
            e_game_team team = (e_game_team)player_iterator.get_datum()->configuration.host.team_index;
            bool found = false;
            for (long team_index = 0; team_index < team_count; ++team_index) {
                if (teams[team_index] == team) {
                    found = true;
                    break;
                }
            }
            if (!found)
                teams[team_count++] = team;
        }
    }
    e_game_team defending_team = _game_team_none;
    if (team_count > 1) {
        defending_team = teams[random_range(tls_random_seed(), 0, (short)team_count)];
        if (defending_team == default_team_index_from_designator(_multiplayer_team_designator_defender)) {
            default_team_increment(defending_team);
            if (defending_team >= team_count)
                defending_team = _game_team_default;
        }
    }
    return defending_team;
}


bool c_game_engine::team_is_enemy(e_game_team team, e_game_team other_team) const
{
    bool enemy = team == _game_team_none || other_team == _game_team_none || team != other_team;
    return enemy;
}

// @address 0x2e1c00 ?allow_weapon_pickup@c_game_engine@@UBA_NJJ@Z
bool c_game_engine::allow_weapon_pickup(long player_index, long object_index) const
{
    bool result = game_engine_in_round();
    if (result) {
        const player_datum *player = player_get(tls_player_data(), player_index);
        result = player->traits.get_weapons_traits()->get_weapon_pickup_allowed();
    }
    return result;
}

// @address 0x2e1d18 ?score_header_string@c_game_engine@@UBAXPAV?$c_static_wchar_string@$0BAA@@@@Z
void c_game_engine::score_header_string(c_static_wchar_string<256> *static_wchar_string) const
{
    game_engine_get_universal_string(227, static_wchar_string);
}

long c_game_engine::get_message_chud_reference() const
{
    s_game_globals *globals = scenario_get_game_globals();
    s_multiplayer_globals_definition *multiplayer =
        (s_multiplayer_globals_definition *)global_tag_instances[(unsigned short)globals->multiplayer_globals.index].definition;
    s_multiplayer_runtime_globals_definition *runtime = (s_multiplayer_runtime_globals_definition *)multiplayer->runtime.address;
    return runtime->game_intro_messages[get_type()].index;
}




void c_game_engine::build_global_baseline(s_game_engine_state_data *state_data) const
{
    memset(state_data, 0, sizeof(*state_data));
    memset(state_data->team_designator_to_team_index, 0xff,
        sizeof(state_data->team_designator_to_team_index));
}


namespace {
static __forceinline unsigned char default_global_status()
{
    return tls_game_engine_globals()->game_finished;
}
}

// @address 0x2e2a00 ?build_global_update@c_game_engine@@UBAXKPAKPAUs_game_engine_state_data@@@Z
void c_game_engine::build_global_update(unsigned long update_mask, unsigned long *actual_update_mask, s_game_engine_state_data *state_data) const
{
    if (update_mask & 1) {
        s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
        if (state_data->initial_teams != globals->initial_team_mask ||
            state_data->valid_designators != globals->designator_mask ||
            state_data->valid_teams != globals->valid_team_mask ||
            state_data->active_teams != globals->active_teams ||
            state_data->ever_active_teams != globals->ever_active_teams ||
            memcmp(state_data->team_designator_to_team_index, &globals->team_mapping[0], sizeof(state_data->team_designator_to_team_index))) {
            state_data->initial_teams = globals->initial_team_mask;
            state_data->valid_designators = globals->designator_mask;
            state_data->valid_teams = globals->valid_team_mask;
            state_data->active_teams = globals->active_teams;
            state_data->ever_active_teams = globals->ever_active_teams;
            memcpy(state_data->team_designator_to_team_index, &globals->team_mapping[0], sizeof(state_data->team_designator_to_team_index));
            *actual_update_mask |= 1;
        }
    }
    if (update_mask & 2 && memcmp(state_data->shared_team_lives, &((s_game_engine_globals *)game_engine_globals )->team_lives[0],
        sizeof(state_data->shared_team_lives))) {
        memcpy(state_data->shared_team_lives, &((s_game_engine_globals *)game_engine_globals )->team_lives[0],
            sizeof(state_data->shared_team_lives));
        *actual_update_mask |= 2;
    }
    if (update_mask & 4 && state_data->current_state != ((s_game_engine_globals *)game_engine_globals)->game_engine_state) {
        state_data->current_state = (unsigned char)((s_game_engine_globals *)game_engine_globals)->game_engine_state;
        *actual_update_mask |= 4;
    }
    if (update_mask & 8 && ((unsigned char)state_data->game_finished != 0) != default_global_status()) {
        state_data->game_finished = default_global_status() != 0;
        *actual_update_mask |= 8;
    }
    if (update_mask & 16 && state_data->current_round != ((s_game_engine_globals *)game_engine_globals)->round_index) {
        state_data->current_round = ((s_game_engine_globals *)game_engine_globals)->round_index;
        *actual_update_mask |= 16;
    }
    if (update_mask & 32 && state_data->round_timer != ((s_game_engine_globals *)game_engine_globals)->timer_seconds) {
        state_data->round_timer = ((s_game_engine_globals *)game_engine_globals)->timer_seconds;
        *actual_update_mask |= 32;
    }
}

// @address 0x2e2cb8 ?apply_global_update@c_game_engine@@UBA_NKPBUs_game_engine_state_data@@@Z
bool c_game_engine::apply_global_update(unsigned long update_mask, const s_game_engine_state_data *state_data) const
{
    bool result = true;
    if (update_mask & 1) {
        s_game_engine_globals *globals = (s_game_engine_globals *)tls_game_engine_globals();
        globals->initial_team_mask = state_data->initial_teams;
        globals->designator_mask = state_data->valid_designators;
        globals->valid_team_mask = state_data->valid_teams;
        globals->active_teams = state_data->active_teams;
        globals->ever_active_teams = state_data->ever_active_teams;
        memcpy(&globals->team_mapping[0], state_data->team_designator_to_team_index, sizeof(state_data->team_designator_to_team_index));
        game_engine_handle_new_team_mapping();
        game_results_notify_active_teams_changed();
    }
    if (update_mask & 2)
        memcpy(&((s_game_engine_globals *)tls_game_engine_globals())->team_lives[0], state_data->shared_team_lives,
            sizeof(state_data->shared_team_lives));
    if ((update_mask & 4) && ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_state != state_data->current_state)
    {
        game_engine_client_respond_to_new_state(state_data->current_state);
        ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_state = state_data->current_state;
    }
    if (update_mask & 8)
        game_finish();
    if (update_mask & 16)
        ((s_game_engine_globals *)tls_game_engine_globals())->round_index = state_data->current_round;
    if (update_mask & 32)
        ((s_game_engine_globals *)tls_game_engine_globals())->timer_seconds = state_data->round_timer;
    return result;
}

// @address 0x2e2e28 ?player_killed_player_perform_respawn_on_kill_check@@YAXJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void player_killed_player_perform_respawn_on_kill_check(long killer_player_index,
    long victim_player_index, bool suicide, c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage_reporting_type)
{
    if (game_engine_has_teams() && (default_current_game_variant()->respawn_option_flags & 8) && !suicide) {
        player_datum *killer = default_player_get(killer_player_index);
        c_player_in_game_iterator player_iterator;
        default_player_iterator_begin(&player_iterator);
        while (player_iterator.next()) {
            player_datum *player = player_iterator.get_datum();
            if (player->unit_index == -1 && player->configuration.host.team_index == killer->configuration.host.team_index)
                default_reset_player_respawn_timer(player_iterator.get_index());
        }
    }
}


long c_game_engine::get_player_state_index(long player_index, bool *valid) const
{
    *valid = true;
    return -1;
}

// ---- 0x2e2f50 / 0x2e3038 / 0x2e1c60 (player_killed_player chain) ----
// data.h also defines this unit's private wchar string template.





namespace {
static __forceinline const c_game_variant *game_engine_default_current_variant()
{
    game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline long game_engine_default_seconds_to_ticks(float seconds)
{
    return (long)((float)((game_time_globals_definition *)tls_game_time_globals())->ticks_per_second * seconds + 0.5f);
}
}

// @address 0x2e2f50 game_engine_update_respawn_growth
extern "C" void game_engine_update_respawn_growth(long killer, long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long,
    24> flags, long damage)
{
    long ticks = game_engine_default_seconds_to_ticks((float)game_engine_default_current_variant()->respawn_growth_seconds);
    if (ticks > 0) {
        player_datum *p = datum_get<player_datum>(tls_player_data(), killer);
        p->respawn_penalty_ticks = p->respawn_penalty_ticks - ticks < 0 ? 0 : p->respawn_penalty_ticks - ticks;
        if (victim != -1) {
            long maximum = ticks * 5;
            p = datum_get<player_datum>(tls_player_data(), victim);
            p->respawn_penalty_ticks = p->respawn_penalty_ticks + ticks > maximum ? maximum : p->respawn_penalty_ticks + ticks;
        }
    }
}

// @address 0x2e3038 game_engine_grant_kill_achievements
extern "C" void game_engine_grant_kill_achievements(long killer, long object_index, long victim, bool betrayal, long flags, long damage)
{
    if (killer == victim || betrayal || killer == -1)
        return;
    player_datum *victim_player = &((player_datum *)tls_player_data()->data)[(unsigned short)victim];
    long root_index;
    long parent_index;
    long stat_91;
    if ((victim_player->flags & 2) != 0)
        return;
    player_datum *killer_player = (player_datum *)datum_try_and_get(tls_player_data(), killer);
    unsigned long killer_index = (unsigned short)killer;
    if (!killer_player)
        killer_index = -1;
    if ((_rotl(flags, 30) & 1) != 0 && killer_player) {
        parent_index = killer_player->unit_index;
        long object_root;
        if (parent_index != -1) {
            s_data_array *object_headers = tls_object_header_data();
            do {
                root_index = parent_index;
                parent_index = ((object_datum *)datum_get<s_object_header>(object_headers, parent_index)->datum)->parent_object_index;
            } while (parent_index != -1);
            object_root = root_index;
        } else {
            object_root = -1;
        }
        if (vehicle_has_element_value(object_root, 868) != 0)
            game_results_try_and_grant_achievement(killer, 40);
    }
    if ((flags & 0x1ff00) != 0 && killer_player) {
        long shifted_31a = killer_player->achievement_damage_info[0] >> 2;
        if (shifted_31a == 21 && ((killer_player->achievement_damage_info[1] & ~3u) == 84))
            game_results_try_and_grant_achievement(killer, shifted_31a + 13);
    }
    if ((_rotl(flags, 23) & 1) != 0) {
        game_results_try_and_grant_achievement(killer, 36);
        if (killer_player) {
            long shifted_31a = killer_player->achievement_damage_info[0] >> 2;
            long shifted_31b = killer_player->achievement_damage_info[1] >> 2;
            if (shifted_31a == 25 && shifted_31b == 25 && ((killer_player->achievement_damage_info[2] & ~3u) == 100))
                game_results_try_and_grant_achievement(killer, shifted_31a + shifted_31b - 11);
        }
    }
    if ((_rotl(flags, 22) & 1) != 0)
        game_results_try_and_grant_achievement(killer, 37);
    if ((_rotl(flags, 29) & 1) != 0 && killer_index <= 15 && g_current_game_results.initialized &&
        g_current_game_results.players[killer_index].player_exists &&
        ((stat_91 = g_current_game_results.statistics.player[killer_index].values[91] >> 1) >= 5) && (stat_91 >= 5))
        game_results_try_and_grant_achievement(killer, 42);
    if ((_rotl(flags, 15) & 1) != 0 && killer_player) {
        long shifted_31a = killer_player->achievement_damage_info[0] >> 2;
        long shifted_31b = killer_player->achievement_damage_info[1] >> 2;
        long shifted_31c = killer_player->achievement_damage_info[2] >> 2;
        long shifted_31d = killer_player->achievement_damage_info[3] >> 2;
        long shifted_31e = killer_player->achievement_damage_info[4] >> 2;
        if ((shifted_31a == 3 || shifted_31a == 31 || shifted_31a == 32 || shifted_31a == 34)
            && (shifted_31b == 3 || shifted_31b == 31 || shifted_31b == 32 || shifted_31b == 34)
            && (shifted_31c == 3 || shifted_31c == 31 || shifted_31c == 32 || shifted_31c == 34)
            && (shifted_31d == 3 || shifted_31d == 31 || shifted_31d == 32 || shifted_31d == 34)
            && (shifted_31e == 3 || shifted_31e == 31 || shifted_31e == 32 || shifted_31e == 34))
            game_results_try_and_grant_achievement(killer, 41);
    }
    if ((_rotl(flags, 14) & 1) != 0)
        game_results_try_and_grant_achievement(killer, 38);
    if (((unsigned long)flags & 1) != 0 && killer_index <= 15) {
        long medal_count_a = -1;
        if (g_current_game_results.initialized && g_current_game_results.players[killer_index].player_exists)
            medal_count_a = g_current_game_results.statistics.player[killer_index].values[94] >> 1;
        long medal_count_b = -1;
        if (g_current_game_results.initialized && g_current_game_results.players[killer_index].player_exists)
            medal_count_b = g_current_game_results.statistics.player[killer_index].values[93] >> 1;
        if (medal_count_b + medal_count_a >= 5)
            game_results_try_and_grant_achievement(killer, 41);
    }
    if ((_rotl(flags, 31) & 1) != 0 && killer_index <= 15) {
        long medal_count_a = -1;
        if (g_current_game_results.initialized && g_current_game_results.players[killer_index].player_exists)
            medal_count_a = g_current_game_results.statistics.player[killer_index].values[94] >> 1;
        long medal_count_b = -1;
        if (g_current_game_results.initialized && g_current_game_results.players[killer_index].player_exists)
            medal_count_b = g_current_game_results.statistics.player[killer_index].values[93] >> 1;
        if (medal_count_b + medal_count_a >= 5)
            game_results_try_and_grant_achievement(killer, 41);
    }
    if ((_rotl(flags, 28) & 1) != 0 && killer_player
        && game_results_count_damage_statistic(killer_index, _game_results_damage_statistic_headshots, _game_results_damage_breakdown_type_all) >= 10)
        game_results_try_and_grant_achievement(killer, 46);
}

// @address 0x2e1c60 game_engine_handle_player_killed
extern "C" void game_engine_handle_player_killed(void *engine, long killer, long object_index, long victim, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> flags, long damage)
{
    if (killer != -1 && victim != -1 && killer != victim) {
        if (game_is_authoritative()) {
            player_killed_player_perform_respawn_on_kill_check(killer, victim, betrayal, flags, damage);
            game_engine_update_respawn_growth(killer, victim, betrayal, flags, damage);
            game_engine_grant_kill_achievements(killer, object_index, victim, betrayal, flags, damage);
        }
    }
}






namespace {
static __forceinline real_vector3d *game_engine_default_copy_vector3d(const real_vector3d *source, real_vector3d *result)
{
    *result = *source;
    return result;
}
}

// @address 0x2e1e50 ?build_player_baseline@c_game_engine@@UBAXFJPAX@Z
void c_game_engine::build_player_baseline(short player_index, long state_data_size, void *state_data) const
{
    s_game_engine_player_state_data *baseline = (s_game_engine_player_state_data *)state_data;
    baseline->b0 = 0;
    baseline->l4 = 0;
    memset(baseline->l8, 0, sizeof(baseline->l8));
    baseline->traits.initialize();
    baseline->b30 = 0;
    baseline->lives_remaining = 0;
    baseline->last_betrayer = -1;
    baseline->s38 = 0;
    baseline->b3a = 0;
    baseline->b3b = 0;
    baseline->b3c = 0;
    baseline->desired_respawn_player = -1;
    game_engine_default_copy_vector3d(&global_identity4x3->forward, &baseline->forward);
    memset(baseline->l4c, 0, sizeof(baseline->l4c));
}

// @address 0x2e1120 ?get_score_string@c_game_engine@@UBAXJPAV?$c_static_wchar_string@$0BAA@@@@Z
void c_game_engine::get_score_string(long score, c_static_wchar_string<256> *string) const
{
    string->print(L"%d", score);
}

// @address 0x2e1138 ?validate_team_designator_for_new_map@c_game_engine@@UBA_NW4e_multiplayer_team_designator@@@Z
bool c_game_engine::validate_team_designator_for_new_map(e_multiplayer_team_designator designator) const
{
    s_datum datum;
    return !levels_try_and_get_datum(g_level_globals.multiplayer_levels, global_scenario->map_id, &datum)
        || designator < datum.maximum_teams[get_type()];
}

// @address 0x2e1d20 ?leader_event_enabled@c_game_engine@@UBA_NXZ
bool c_game_engine::leader_event_enabled() const
{
    bool result = true;
    if (game_engine_has_teams()) {
        for (long team = 0; team <= 7; team++) {
            if (game_engine_get_team_rounds_won((e_game_team)team) >= game_engine_get_score_to_win_round()) {
                result = false;
                break;
            }
        }
    } else {
        c_player_in_game_iterator iterator;
        default_player_iterator_begin(&iterator);
        while (iterator.next()) {
            if (game_engine_get_player_kills(iterator.get_index()) >= game_engine_get_score_to_win_round()) {
                result = false;
                break;
            }
        }
    }
    return result;
}






static __forceinline bool game_engine_default_player_update_recharging(const player_datum *player)
{
    long setting = player->traits.m_weapon_traits.m_recharging_grenades;
    if (setting != 1)
        return false;
    else
        return true;
}
static __forceinline bool game_engine_default_player_update_has_teams()
{
    unsigned char result = game_engine_running() && (tls_game_engine_globals()->flags & 8) != 0;
    return result;
}

const real game_engine_default_player_update_half = 0.5f;
static __forceinline long game_engine_default_player_update_ticks(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + game_engine_default_player_update_half);
}
static __forceinline s_game_globals *game_engine_default_player_update_get_globals()
{
    return (s_game_globals *)global_game_globals;
}

// @address 0x2e1798 ?player_update@c_game_engine@@UBAXJ@Z
void c_game_engine::player_update(long player_index) const
{
    bool predicted = false;
    if (tls_game_globals()->options.simulation_type == 4)
        predicted = true;
    if (!predicted) {
        player_datum *player = ((player_datum *)((s_data_array *)tls_player_data())->data) + (unsigned short)player_index;
        if (player->unit_index != -1) {
            unit_datum *object =
                (unit_datum *)(*datum_get<s_object_header>(tls_object_header_data(), player->unit_index)).datum;
            if (game_engine_default_player_update_recharging(player)) {
                long threshold = game_engine_default_player_update_ticks(5.0f);
                if (player->grenade_recharge_ticks++ >= threshold) {
                    for (unsigned long index = 0; index < 3; index++) {
                        s_grenade_definition *entry = game_engine_default_player_update_get_globals()->grenades.count()
                            ? game_engine_default_player_update_get_globals()->grenades.begin() + index
                            : 0;
                        signed char grenade_count = object->unit.grenade_counts[index];
                        if (grenade_count < entry->maximum_count && grenade_count < 4) {
                            object->unit.grenade_counts[index] = grenade_count + 1;
                            ::simulation_action_object_update(player->unit_index, 0x4000000);
                            simulation_action_pickup_equipment(player->unit_index, entry->equipment.index);
                            long perspective = -1;
                            do {
                                if (player_index != -1)
                                    perspective = ((long)::director_get_perspective(player_index, perspective));
                                if (perspective != -1) {
                                    chud_post_grenade_pickup(perspective, entry->equipment.index);
                                }
                            } while (perspective != -1);
                        }
                    }
                    player->grenade_recharge_ticks = 0;
                }
            }
            player->previous_crouch_state = player->crouch_state;
            player->crouch_state = object->unit.crouch_amount > game_engine_default_player_update_half;
            if (player->crouch_state != player->previous_crouch_state)
                player_changed_crouch_state(player_index);
        }
    }
    player_datum *player = ((player_datum *)((s_data_array *)tls_player_data())->data) + (unsigned short)player_index;
    long team = game_engine_default_player_update_has_teams() ? player->configuration.host.team_index : -1;
    real_rgb_color colors[4];
    bool valid;
    game_engine_get_change_colors(player_index, &player->configuration.client.appearance, (e_game_team)team, colors, &valid);
    if ((signed char)valid) {
        long object_index = player->unit_index;
        if (object_index != -1)
            ::player_copy_object_appearance(player_index, object_index);
    }
}



namespace {
static __forceinline s_game_engine_globals *game_engine_default_waypoint_globals()
{
    return (s_game_engine_globals *)tls_game_engine_globals();
}
}

// @address 0x2e1ef8 ?build_player_update@c_game_engine@@UBAXFPAKJPAX@Z
void c_game_engine::build_player_update(short player_index, unsigned long *update_mask, long state_data_size, void *state_data) const
{
    player_datum *player = (player_datum *)datum_try_and_get_absolute(tls_player_data(), player_index);
    s_game_engine_player_update_data *update = (s_game_engine_player_update_data *)state_data;
    unsigned long result_mask = 0;
    if (player) {
        long player_datum_index = datum_absolute_index_to_index(tls_player_data(), player_index);
        long respawn_timer_ticks = game_engine_default_waypoint_globals()->player_waypoints[player_index].ticks;
        unsigned char waypoint_state = game_engine_default_waypoint_globals()->player_waypoints[player_index].state;
        s_game_engine_player_waypoint *waypoint = &game_engine_default_waypoint_globals()->player_waypoints[player_index];
        real_point3d waypoint_position = waypoint->position;
        bool alive = player->death_timer > 0;
        short lives_remaining = player->lives;
        long last_betrayer;
        if (player->respawn_player_index == -1)
            last_betrayer = -1;
        else
            last_betrayer = (unsigned short)player->respawn_player_index;
        if (*update_mask & 1) {
            update->respawn_timer_ticks = respawn_timer_ticks;
            update->waypoint_position = waypoint_position;
            result_mask |= 1;
        }
        if (*update_mask & 2) {
            if (!update->shield_vitality_traits.equivalent(&player->traits.m_shield_vitality_traits)) {
                update->shield_vitality_traits.set(&player->traits.m_shield_vitality_traits, false);
                result_mask |= 2;
            }
        }
        if (*update_mask & 4) {
            if (!update->weapon_traits.equivalent(&player->traits.m_weapon_traits)) {
                update->weapon_traits.set(&player->traits.m_weapon_traits, false);
                result_mask |= 4;
            }
        }
        if (*update_mask & 8) {
            if (!update->movement_traits.equivalent(&player->traits.m_movement_traits)) {
                update->movement_traits.set(&player->traits.m_movement_traits, false);
                result_mask |= 8;
            }
        }
        if (*update_mask & 16) {
            if (!update->appearance_traits.equivalent(&player->traits.m_appearance_traits)) {
                update->appearance_traits.set(&player->traits.m_appearance_traits, false);
                result_mask |= 16;
            }
        }
        if (*update_mask & 32) {
            if (!update->sensor_traits.equivalent(&player->traits.m_sensor_traits)) {
                update->sensor_traits.set(&player->traits.m_sensor_traits, false);
                result_mask |= 32;
            }
        }
        if ((*update_mask & 64) && (signed char)update->waypoint_state != (signed char)waypoint_state) {
            result_mask |= 64;
            update->waypoint_state = waypoint_state;
        }
        if ((*update_mask & 128) && update->blocking_teleporter != alive) {
            result_mask |= 128;
            update->blocking_teleporter = alive;
        }
        if ((*update_mask & 256) && simulation_update_player_netdebug_data(player_datum_index, &update->timers)) {
            game_engine_default_waypoint_globals()->player_netdebug_data[player_index] = update->timers;
            result_mask |= 256;
        }
        if ((*update_mask & 512) && update->lives_remaining != lives_remaining) {
            result_mask |= 512;
            update->lives_remaining = lives_remaining;
        }
        if ((*update_mask & 1024) && update->last_betrayer != last_betrayer) {
            result_mask |= 1024;
            update->last_betrayer = last_betrayer;
        }
        if ((*update_mask & 2048) && update->respawn_timer != player->respawn_seconds) {
            result_mask |= 2048;
            update->respawn_timer = player->respawn_seconds;
        }
        if ((*update_mask & 4096) && update->vehicle_entrance_ban != ((player->flags >> 10) & 1)) {
            result_mask |= 4096;
            update->vehicle_entrance_ban = (player->flags >> 10) & 1;
        }
        if ((*update_mask & 8192) && update->active_in_game != ((player->flags & 1) != 0)) {
            result_mask |= 8192;
            update->active_in_game = (player->flags & 1) != 0;
        }
        if ((*update_mask & 16384) && update->sitting_out != ((player->flags >> 12) & 1)) {
            result_mask |= 16384;
            update->sitting_out = (player->flags >> 12) & 1;
        }
        if ((*update_mask & 32768) && update->desired_respawn_player != (player->respawn_target_player_index & 0xffff)) {
            result_mask |= 32768;
            update->desired_respawn_player = (short)player->respawn_target_player_index;
        }
        if ((*update_mask & 65536) && player->unit_index != -1) {
            update->player_control_aiming = default_object_get(player->unit_index)->unit.desired_aiming_vector;
            normalize3d(&update->player_control_aiming);
            result_mask |= 65536;
        }
    }
    *update_mask = result_mask;
}
// @address 0x2e24f0 ?apply_player_update@c_game_engine@@UBA_NFKJPBX@Z
bool c_game_engine::apply_player_update(short player_index, unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_game_engine_player_update_data *update = (const s_game_engine_player_update_data *)state_data;
    player_datum *player = (player_datum *)datum_try_and_get_absolute(tls_player_data(), player_index);
    bool result = false;
    if (player) {
        if (update_mask & 0x1) {
            player->respawn_timer = update->respawn_timer_ticks;
            game_engine_default_waypoint_globals()->player_waypoints[player_index].ticks = (short)update->respawn_timer_ticks;
            game_engine_default_waypoint_globals()->player_waypoints[player_index].initial_ticks = (short)update->respawn_timer_ticks;
            game_engine_default_waypoint_globals()->player_waypoints[player_index].position = update->waypoint_position;
            if (fabs(game_engine_default_waypoint_globals()->player_waypoints[player_index].position.x) < 0.0001f &&
                fabs(game_engine_default_waypoint_globals()->player_waypoints[player_index].position.y) < 0.0001f &&
                fabs(game_engine_default_waypoint_globals()->player_waypoints[player_index].position.z - k_game_engine_invalid_dead_point.z) < 0.0001f)
                game_engine_default_waypoint_globals()->player_waypoints[player_index].active = false;
            else
                game_engine_default_waypoint_globals()->player_waypoints[player_index].active = true;
        }
        if (update_mask & 0x2)
            player->traits.m_shield_vitality_traits.set(&update->shield_vitality_traits, false);
        if (update_mask & 0x4)
            player->traits.m_weapon_traits.set(&update->weapon_traits, false);
        if (update_mask & 0x8)
            player->traits.m_movement_traits.set(&update->movement_traits, false);
        if (update_mask & 0x10)
            player->traits.m_appearance_traits.set(&update->appearance_traits, false);
        if (update_mask & 0x20)
            player->traits.m_sensor_traits.set(&update->sensor_traits, false);
        if (update_mask & 0x40)
            game_engine_default_waypoint_globals()->player_waypoints[player_index].state = update->waypoint_state;
        if (update_mask & 0x80) {
            if (update->blocking_teleporter)
                player->flags |= 0x4;
            else
                player->flags &= ~0x4;
        }
        if (update_mask & 0x100) {
            game_engine_default_waypoint_globals()->player_netdebug_data[player_index] = update->timers;
        }
        if (update_mask & 0x200)
            player->lives = update->lives_remaining;
        if (update_mask & 0x400) {
            player_datum *other = (player_datum *)datum_try_and_get_absolute(tls_player_data(), update->last_betrayer);
            long index = -1;
            if (other)
                index = datum_absolute_index_to_index(tls_player_data(), update->last_betrayer);
            player->respawn_player_index = index;
        }
        if (update_mask & 0x800)
            player->respawn_seconds = update->respawn_timer;
        if (update_mask & 0x1000) {
            if (update->vehicle_entrance_ban)
                player->flags |= 0x400;
            else
                player->flags &= ~0x400;
        }
        if (update_mask & 0x2000) {
            if (update->active_in_game)
                player->flags |= 0x1;
            else
                player->flags &= ~0x1;
        }
        if (update_mask & 0x4000) {
            if (update->sitting_out)
                player->flags |= 0x1000;
            else
                player->flags &= ~0x1000;
        }
        if (update_mask & 0x8000)
            player->respawn_target_player_index = datum_absolute_index_to_index(tls_player_data(), update->desired_respawn_player);
        if (update_mask & 0x10000) {
            long player_datum_index = datum_absolute_index_to_index(tls_player_data(), player_index);
            long unit_index = player->unit_index;
            long user_index = tls_player_mapping_globals()->player_input_user_mapping[datum_index_to_absolute_index(player_datum_index)];
            if (unit_index != -1) {
                long control_unit_index = unit_index;
                if (game_engine_is_player_in_edit_mode(player_datum_index, 0, 0)) {
                    control_unit_index = game_engine_sandbox_find_player_original_unit(player_datum_index);
                    if (control_unit_index == -1)
                        control_unit_index = player->unit_index;
                }
                unit_set_facing(control_unit_index, &update->player_control_aiming, &update->player_control_aiming);
            }
            if (user_index != -1)
                player_control_set_facing(user_index, &update->player_control_aiming);
        }
        result = true;
    }
    return result;
}

// @address 0x42d298 ?get_type@c_game_engine@@UBAJXZ
long c_game_engine::get_type() const
{
    return _game_engine_type_none;
}

// @address 0x5bec98 ?get_hud_interface_state@c_game_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_game_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
}

// @address 0x42d380 ?initialize_for_new_map@c_game_engine@@UBA_NXZ
bool c_game_engine::initialize_for_new_map() const
{
    return true;
}

// @address 0x5bec98 ?dispose_from_old_map@c_game_engine@@UBAXXZ
void c_game_engine::dispose_from_old_map() const
{
}

// @address 0x42d380 ?initialize_for_new_round@c_game_engine@@UBA_NXZ
bool c_game_engine::initialize_for_new_round() const
{
    return true;
}

// @address 0x5bec98 ?stats_reset_for_round_switch@c_game_engine@@UBAXXZ
void c_game_engine::stats_reset_for_round_switch() const
{
}

// @address 0x5bec98 ?player_added@c_game_engine@@UBAXJ@Z
void c_game_engine::player_added(long player_index) const
{
}

// @address 0x5bec98 ?player_left@c_game_engine@@UBAXJ@Z
void c_game_engine::player_left(long player_index) const
{
}

// @address 0x5bec98 ?player_rejoined@c_game_engine@@UBAXJ@Z
void c_game_engine::player_rejoined(long player_index) const
{
}

// @address 0x5bec98 ?player_changed_indices@c_game_engine@@UBAXJJ@Z
void c_game_engine::player_changed_indices(long old_player_index, long new_player_index) const
{
}

// @address 0x5bec98 ?player_changed_teams@c_game_engine@@UBAXJ@Z
void c_game_engine::player_changed_teams(long player_index) const
{
}

// @address 0x5bec98 ?player_just_spawned@c_game_engine@@UBAXJ@Z
void c_game_engine::player_just_spawned(long player_index) const
{
}

// @address 0x5bec98 ?game_ending@c_game_engine@@UBAXXZ
void c_game_engine::game_ending() const
{
}

// @address 0x5bec98 ?game_starting@c_game_engine@@UBAXXZ
void c_game_engine::game_starting() const
{
}

// @address 0x5bec98 ?render@c_game_engine@@UBAXJ@Z
void c_game_engine::render(long user_index) const
{
}

// @address 0x5bec98 ?render_debug@c_game_engine@@UBAXJ@Z
void c_game_engine::render_debug(long user_index) const
{
}

// @address 0x5bec98 ?update@c_game_engine@@UBAXXZ
void c_game_engine::update() const
{
}

// @address 0x5bec98 ?apply_game_engine_traits_for_player@c_game_engine@@UBAXJPAVc_player_traits@@@Z
void c_game_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const
{
    ASSERT(traits != NULL);
}

// @address 0x5e15a8 ?assemble_spawn_influencers_for_player@c_game_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
void c_game_engine::assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const
{
    *influencer_count = 0;
}

// @address 0x42d298 ?compare_players@c_game_engine@@UBAJJJ@Z
long c_game_engine::compare_players(long player_index, long other_player_index) const
{
    return 0;
}

// @address 0x42d298 ?compare_teams@c_game_engine@@UBAJW4e_game_team@@0@Z
long c_game_engine::compare_teams(e_game_team team, e_game_team other_team) const
{
    return 0;
}

// @address 0x42d298 ?should_auto_pickup_weapon@c_game_engine@@UBA_NJJ@Z
bool c_game_engine::should_auto_pickup_weapon(long player_index, long object_index) const
{
    return false;
}

// @address 0x5bec98 ?player_nearby_multiplayer_weapon@c_game_engine@@UBAXJJ@Z
void c_game_engine::player_nearby_multiplayer_weapon(long player_index, long object_index) const
{
}

// @address 0x1c1258 ?object_get_emblem_player@c_game_engine@@UBAJJ@Z
long c_game_engine::object_get_emblem_player(long object_index) const
{
    return NONE;
}

// @address 0x42cc30 ?compute_object_function@c_game_engine@@UBAMJJ@Z
float c_game_engine::compute_object_function(long object_index, long function_name) const
{
    return 0.0f;
}

// @address 0x5bec98 ?multiplayer_weapon_register@c_game_engine@@UBAXJ@Z
void c_game_engine::multiplayer_weapon_register(long object_index) const
{
}

// @address 0x5bec98 ?multiplayer_weapon_deregister@c_game_engine@@UBAXJ@Z
void c_game_engine::multiplayer_weapon_deregister(long object_index) const
{
}

// @address 0x5bec98 ?multiplayer_weapon_picked_up@c_game_engine@@UBAXJJ@Z
void c_game_engine::multiplayer_weapon_picked_up(long object_index, long player_index) const
{
}

// @address 0x5bec98 ?multiplayer_weapon_dropped@c_game_engine@@UBAXJJ@Z
void c_game_engine::multiplayer_weapon_dropped(long object_index, long player_index) const
{
}

// @address 0x5bec98 ?handle_deleted_object@c_game_engine@@UBAXJ@Z
void c_game_engine::handle_deleted_object(long object_index) const
{
}

// @address 0x19a838 ?get_time_left_in_ticks@c_game_engine@@UBAJJ_N0@Z
long c_game_engine::get_time_left_in_ticks(long user_index, bool a, bool b) const
{
    return user_index;
}

// @address 0x42d380 ?is_multiplayer_object_type_allowed@c_game_engine@@UBA_NJ@Z
bool c_game_engine::is_multiplayer_object_type_allowed(long type) const
{
    return true;
}

// @address 0x5bec98 ?player_damaged_player@c_game_engine@@UBAXJJJ_N@Z
void c_game_engine::player_damaged_player(long player_index, long damage_index, long damaged_player_index, bool dead) const
{
}

// @address 0x5bec98 ?player_assisted_with_kill@c_game_engine@@UBAXJ@Z
void c_game_engine::player_assisted_with_kill(long player_index) const
{
}

// @address 0x1c1258 ?player_killed_player_get_kill_message@c_game_engine@@UBAJJJ_N@Z
long c_game_engine::player_killed_player_get_kill_message(long killer, long victim, bool betrayal) const
{
    return NONE;
}

// @address 0x42d298 ?test_flag@c_game_engine@@UBA_NJ@Z
bool c_game_engine::test_flag(long flag) const
{
    return false;
}

// @address 0x5bec98 ?adjust_object_gravity@c_game_engine@@UBAXJPATreal_vector3d@@@Z
void c_game_engine::adjust_object_gravity(long object_index, real_vector3d *gravity) const
{
}

// @address 0x5bec98 ?prepare_for_new_state@c_game_engine@@UBAXJ@Z
void c_game_engine::prepare_for_new_state(long state) const
{
}

// @address 0x42d298 ?should_end_round@c_game_engine@@UBA_NPAJ@Z
bool c_game_engine::should_end_round(long *reason) const
{
    return false;
}

// @address 0x42d298 ?should_purge_multiplayer_item@c_game_engine@@UBA_NJ@Z
bool c_game_engine::should_purge_multiplayer_item(long object_index) const
{
    return false;
}

// @address 0x5bec98 ?player_hump@c_game_engine@@UBAXJ@Z
void c_game_engine::player_hump(long player_index) const
{
}

// @address 0x1c1258 ?get_simulation_entity_type@c_game_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_game_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_none;
}

// @address 0x5bec98 ?promote_to_simulation_authority@c_game_engine@@UBAXXZ
void c_game_engine::promote_to_simulation_authority() const
{
}

// @address 0x5bec98 ?recover_state_before_promotion@c_game_engine@@UBAXXZ
void c_game_engine::recover_state_before_promotion() const
{
}

// @address 0x5bec98 ?build_simulation_baseline@c_game_engine@@UBAXJPAX@Z
void c_game_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
}

// @address 0x42d298 ?parse_multiplayer_string_token@c_game_engine@@UBAJJPB_WJPBUs_game_engine_event_data@@PA_WJ@Z
long c_game_engine::parse_multiplayer_string_token(long player_index, const wchar_t *token, long token_length,
                                                   const s_game_engine_event_data *event, wchar_t *buffer, long buffer_size) const
{
    return 0;
}

// @address 0x42d298 ?handle_incoming_simulation_event@c_game_engine@@UBA_NW4e_simulation_event_type@@PBX@Z
bool c_game_engine::handle_incoming_simulation_event(e_simulation_event_type event_type, const void *payload) const
{
    return false;
}

// @address 0x42d380 ?leader_event_lead_enabled@c_game_engine@@UBA_NXZ
bool c_game_engine::leader_event_lead_enabled() const
{
    return true;
}

// @address 0x1c1258 ?get_none_index@c_game_engine@@UBAJXZ
long c_game_engine::get_none_index() const
{
    return NONE;
}

// @address 0x5bec98 ?player_changed_crouch_state@c_game_engine@@UBAXJ@Z
void c_game_engine::player_changed_crouch_state(long player_index) const
{
}

// @address 0x5bec98 ?emit_game_start_event@c_game_engine@@UBAXJ@Z
void c_game_engine::emit_game_start_event(long player_index) const
{
}

// @address 0x11df664 game_engine_globals
