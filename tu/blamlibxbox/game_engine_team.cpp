// blamlibxbox/game_engine_team.obj: from the records 0x277ee8 0x277f40 0x277fb8 0x278020 0x278110 0x278198 0x278360 0x278410 0x278490 0x2784e0 0x278530 0x278638 0x2786b0 0x278790 0x278878 0x278a20
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "game/map_variant.h"
#include "memory/data_types.h"
#include "game/game_globals_storage.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_variant.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "interface/user_interface_session.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "main/levels.h"
#include "memory/data.h"
#include <string.h>
#include "game/player_in_game_iterator.h"
#include "game/game_results_types.h"
#include "memory/data_handle_access.h"
// @address 0x277ee8 ?game_engine_has_teams@@YA_NXZ
// @address 0x278020 ?game_engine_team_is_enemy@@YA_NW4e_game_team@@0@Z

// @address 0x278110 ?game_engine_team_index_to_team_designator@@YA?AW4e_multiplayer_team_designator@@W4e_game_team@@@Z
// @address 0x278490 ?game_engine_next_active_team@@YA?AW4e_game_team@@W41@@Z
// @address 0x2784e0 ?game_engine_next_inactive_team@@YA?AW4e_game_team@@W41@@Z

#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_default.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_list.h"

#include "main/levels_globals.h"
#include "game/game_engine_team_inline.h"


bool game_engine_has_teams(void)
{
    return game_engine_running() && (tls_game_engine_globals()->flags & 8) != 0;
}


// @address 0x277f40 ?game_engine_is_team_active@@YA_NW4e_game_team@@@Z
bool game_engine_is_team_active(enum e_game_team _sym_p0)
{
    bool result = false;
    if (game_engine_has_teams() && ((unsigned int)_sym_p0) <= 7) {
        result = (tls_game_engine_globals()->active_teams & (1 << ((unsigned int)_sym_p0))) != 0;
    }
    return result;
}

// @address 0x277fb8 game_engine_is_team_ever_active
extern "C" bool game_engine_is_team_ever_active(unsigned int index)
{
    return game_engine_has_teams() && index <= 7 && (tls_game_engine_globals()->ever_active_teams & (1 << index)) != 0;
}

bool game_engine_team_is_enemy(e_game_team team, e_game_team other_team)
{
    bool result = false;
    if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10) {
        c_game_engine *engine = game_engines[tls_game_engine_globals()->game_engine_index];
        if (engine)
            result = engine->team_is_enemy(team, other_team);
    }
    return result;
}

namespace {
static __forceinline bool game_engine_is_team_designator_valid(e_multiplayer_team_designator designator)
{
    bool result = false;
    if (designator >= 0 && designator < 9)
        result = (tls_game_engine_globals()->designator_mask & (1 << designator)) != 0;
    return result;
}
static __forceinline e_game_team game_engine_next_team(e_game_team team, bool active)
{
    e_game_team result = _game_team_none;
    for (team = (e_game_team)(team + 1); team < 8; team = (e_game_team)(team + 1)) {
        if (((tls_game_engine_globals()->active_teams & (1 << team)) != 0) == active) {
            result = team;
            break;
        } else {
            continue;
        }
    }
    return result;
}
}
e_multiplayer_team_designator game_engine_team_index_to_team_designator(e_game_team team)
{
    long designator;
    for (designator = 0; designator < 9; designator++) {
        if (game_engine_is_team_designator_valid((e_multiplayer_team_designator)designator) &&
            game_engine_team_designator_to_team_index((e_multiplayer_team_designator)designator) == team)
            break;
    }
    if (designator == 9)
        designator = 8;
    return (e_multiplayer_team_designator)designator;
}
e_game_team game_engine_next_active_team(e_game_team team)
{
    return game_engine_next_team(team, true);
}
e_game_team game_engine_next_inactive_team(e_game_team team)
{
    return game_engine_next_team(team, false);
}
namespace {
static __forceinline const game_options *game_options_get()
{
    return &tls_game_globals()->options;
}
static __forceinline bool game_engine_variant_has_teams(const c_game_variant *variant)
{
    return variant->get_active_variant()->get_miscellaneous_options()->get_teams_enabled();
}
static __forceinline void team_lives_set_all(char *values, const char &value)
{
    for (long index = 0; index < 8; index++)
        values[index] = value;
}
static __forceinline unsigned char game_engine_team_lives_per_round(const c_game_variant *variant)
{
    return variant->get_active_variant()->get_respawn_options()->get_team_lives_per_round();
}
}
// @address 0x278638 ?game_engine_variant_get_maximum_team_count@@YAJPBVc_game_variant@@W4e_map_id@@@Z
long game_engine_variant_get_maximum_team_count(const c_game_variant *variant, e_map_id multiplayer_map_id)
{
    long maximum_team_count = 0;
    if (game_engine_variant_has_teams(variant)) {
        s_datum level;
        long game_engine_index = variant->engine_type;
        if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, multiplayer_map_id, &level))
            maximum_team_count = level.maximum_teams[game_engine_index];
    }
    return maximum_team_count;
}



// @address 0x2786b0 game_engine_team_is_out_of_lives
extern "C" bool game_engine_team_is_out_of_lives(e_game_team team)
{
    bool out_of_lives = false;
    if (game_engine_has_teams() && game_engine_limited_lives_active())
    {
        long lives = 0;
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next())
        {
            if (iterator.get_datum()->configuration.host.team_index == team)
                lives += iterator.get_datum()->lives;
        }
        out_of_lives = lives <= 0;
    }
    return out_of_lives;
}

// @address 0x278790 game_engine_teams_get_number_of_players_alive
extern "C" int game_engine_teams_get_number_of_players_alive(int team)
{
    bool valid;
    int count = 0;
    if (game_engine_has_teams()) {
        if ((unsigned int)team > 7 ||
            (valid = true, (tls_game_engine_globals()->valid_team_mask & 1 << team) == 0)) {
            valid = false;
        }
        if (valid) {
            c_player_in_game_iterator iterator;
        iterator.begin();
            while (iterator.next()) {
                if (iterator.get_datum()->configuration.host.team_index == team && iterator.get_datum()->unit_index != -1) {
                    count++;
                }
            }
        }
    }
    return count;
}


// @address 0x278878 game_engine_teams_player_can_change_teams
extern "C" bool game_engine_teams_player_can_change_teams(int player_index)
{
    const c_game_variant *variant = user_interface_game_settings_get_game_variant();
    player_datum *player = (player_datum *)datum_try_and_get_inline(tls_player_data(), player_index);
    bool result = false;
    if (player && variant && variant->get_active_variant()->get_miscellaneous_options()->get_teams_enabled() &&
        ((variant->get_active_variant()->get_social_options()->get_team_changing_setting() != 0) ||
         (variant->get_active_variant()->get_social_options()->get_team_changing_setting() == 2))) {
        result = true;
        if ((variant->get_active_variant()->get_social_options()->get_team_changing_setting() == 2)) {
            result = false;
            if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 9) {
                int maximum_players = 0;
                int team_players[9];
                memset(team_players, 0, sizeof(team_players));
                c_player_in_game_iterator players;
                players.begin();
                while (players.next()) {
                    int team = players.get_datum()->configuration.host.team_index;
                    if (team >= 0 && team < 9) {
                        team_players[team]++;
                        if (team_players[team] > maximum_players)
                            maximum_players = team_players[team];
                    }
                }
                for (int team = 0; team < 8; team++) {
                    if (tls_game_engine_globals()->valid_team_mask & (1 << team)) {
                        if (team_players[team] < maximum_players - 1)
                            result = true;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x278a20 ?game_engine_build_initial_teams@@YAXXZ
void game_engine_build_initial_teams()
{
    unsigned short initial_team_mask = 0;
    long initial_team_count = 0;
    const game_options *options = game_options_get();
    for (long player_index = 0; player_index < 16; player_index++) {
        if (options->players[player_index].valid) {
            long team_index = options->players[player_index].configuration.host.team_index;
            if (team_index != -1 && (initial_team_mask & (1 << team_index)) == 0) {
                initial_team_mask |= 1 << team_index;
                initial_team_count++;
            }
        }
    }
    long maximum_team_count = 8;
    long valid_team_count = 0;
    for (long designator = 0; designator < 9; designator++) {
        if (tls_game_engine_globals()->allowable_team_designators & (1 << designator))
            valid_team_count++;
    }
    maximum_team_count = maximum_team_count > valid_team_count ? valid_team_count : maximum_team_count;
    if (initial_team_count > 0 && initial_team_count < maximum_team_count) {
        for (e_game_team team_index = _game_team_default; team_index < 8; team_index = (e_game_team)(team_index + 1)) {
            if ((initial_team_mask & (1 << team_index)) == 0) {
                initial_team_mask |= 1 << team_index;
                initial_team_count++;
                if (initial_team_count == maximum_team_count)
                    break;
                else
                    continue;
            } else {
                continue;
            }
        }
    }
    tls_game_engine_globals()->initial_team_mask = initial_team_mask;
    simulation_action_game_engine_globals_update(1);
    char team_lives = game_engine_team_lives_per_round(current_game_variant());
    team_lives_set_all(tls_game_engine_globals()->team_lives, team_lives);
    simulation_action_game_engine_globals_update(2);
}



static __forceinline const c_game_engine *game_engine_team_inline_current_engine(void)
{
    if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10)
        return game_engines[tls_game_engine_globals()->game_engine_index];
    return 0;
}

// @address 0x278198 ?game_engine_build_valid_team_mapping@@YAXXZ
void game_engine_build_valid_team_mapping(void)
{
    const c_game_engine *engine = game_engine_team_inline_current_engine();
    int team = engine->get_defending_team_index();
    bool inactive = false;
    if (team == _game_team_none) {
        for (int next = 0; next < 8; next++) {
            if (tls_game_engine_globals()->active_teams & (1 << next)) {
                team = next;
                break;
            }
        }
    }
    int first_team = team;
    tls_game_engine_globals()->designator_mask = 0;
    tls_game_engine_globals()->valid_team_mask = 0;
    for (int designator = 0; designator < 9; designator++)
        tls_game_engine_globals()->team_mapping[designator] = _game_team_none;
    for (int designator = 0; designator < 8; designator++) {
        const c_game_engine *current = current_game_engine();
        if (current->validate_team_designator_for_new_map((e_multiplayer_team_designator)designator)) {
            tls_game_engine_globals()->team_mapping[designator] = (short)team;
            tls_game_engine_globals()->designator_mask |= 1 << designator;
            tls_game_engine_globals()->valid_team_mask |= 1 << team;
            if (inactive) {
                team = game_engine_next_team((e_game_team)team, false);
            } else {
                team = game_engine_next_active_team((e_game_team)team);
                if (team == _game_team_none)
                    team = game_engine_next_active_team(_game_team_none);
                if (team == first_team) {
                    team = game_engine_next_inactive_team(_game_team_none);
                    inactive = true;
                }
            }
        }
    }
    game_engine_handle_new_team_mapping();
    simulation_action_game_engine_globals_update(1);
}

// @address 0x278410 ?game_engine_handle_new_team_mapping@@YAXXZ
void game_engine_handle_new_team_mapping()
{
    c_data_iterator<player_datum> it;
    s_player_configuration configuration;
    it.m_iterator.absolute_index = -1;
    it.m_iterator.index = -1;
    it.m_iterator.data = tls_player_data();
    while (it.next()) {
        configuration = it.get_datum()->desired_configuration;
        player_set_configuration(it.m_iterator.index, &configuration);
    }
}



namespace {
static __forceinline int game_engine_team_game_engine_index_out_of_range(int index)
{
    if (index < 1)
        return 1;
    if (index > 10)
        return 1;
    return 0;
}
}

// @address 0x278360 ?game_engine_build_teams@@YAXXZ
void game_engine_build_teams(void)
{
    bool simulation_is_predicted = false;
    if (game_options_get()->game_simulation == 4)
        simulation_is_predicted = true;
    if (!simulation_is_predicted) {
        s_game_engine_globals *globals = tls_game_engine_globals();
        int index = globals->game_engine_index;
        if (!game_engine_team_game_engine_index_out_of_range(index)) {
            if (game_engines[index] != 0 && game_engine_has_teams()) {
                if (globals->initial_team_mask == 0)
                    game_engine_build_initial_teams();
                game_engine_build_valid_team_mapping();
                game_engine_recompute_active_teams();
            }
        }
    }
}

// @address 0x278530 ?game_engine_recompute_active_teams@@YAXXZ
void game_engine_recompute_active_teams(void)
{
    int old_active_teams = tls_game_engine_globals()->active_teams;
    int old_ever_active_teams = tls_game_engine_globals()->ever_active_teams;
    int active_teams = 0;
    int ever_active_teams = 0;
    c_data_iterator<player_datum> players;
    players.m_iterator.data = tls_player_data();
    players.m_iterator.absolute_index = -1;
    players.m_iterator.index = -1;
    while (players.next()) {
        player_datum *player = (player_datum *)players.m_datum;
        int team = player->configuration.host.team_index;
        if (team != -1) {
            ever_active_teams |= 1 << team;
            if (!(player->flags & 2))
                active_teams |= 1 << player->configuration.host.team_index;
        }
    }
    if (active_teams != old_active_teams || ever_active_teams != old_ever_active_teams) {
        int newly_active_teams = active_teams & ~tls_game_engine_globals()->active_teams;
        tls_game_engine_globals()->active_teams = (unsigned short)active_teams;
        tls_game_engine_globals()->ever_active_teams |= ever_active_teams;
        simulation_action_game_engine_globals_update(1);
        game_results_notify_active_teams_changed();
        for (int team = 0; team < 8; team++) {
            if (newly_active_teams & (1 << team))
                game_results_statistic_set(-1, (e_game_team)team, _game_results_statistic_games_played, 1);
        }
    }
}

