// blamlibxbox/game_engine_scoring.obj: from the records 0x29e388 0x29e418 0x29e4a8 0x29e7b0 0x29e930 0x29ec68 0x29efa0 0x29f058 0x29f0c8 0x29f1f8 0x29f300 0x708a30
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "memory/data_types.h"
#include "memory/data_typed_access.h"
#include <string.h>
#include "game/game_globals_storage.h"
#include "game/game_types.h"
#include "game/game_engine_score_types.h"

#include "game/game_engine_scoring.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "memory/data.h"


#include "game/game_engine_default.h"
#include "game/game_engine_list.h"
#include "game/game_engine_get_inline.h"
#include "game/game_engine_event_data.h"
#include "game/game_engine.h"

#include "game/game_engine_scoring_globals.h"


#include "game/player_in_game_iterator.h"

#include "game/game_time_types.h"
#include "cseries/long_sort.h"

#include "cseries/cseries.h"
// @address 0x207148 ?game_engine_get_score_to_win_round@@YAJXZ
// @address 0x277ee8 ?game_engine_has_teams@@YA_NXZ
// @address 0x209ef0 ?game_engine_player_is_out_of_lives@@YA_NJ@Z
// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ

extern "C" {
s_game_engine_score_cache g_game_engine_score_cache[2];
}

struct s_team_comparison
{
    const char *keys;
    bool descending;
};

struct s_player_comparison
{
    const char *keys;
    bool deep;
};






static __forceinline c_game_statborg *game_engine_get_statborg()
{
    return game_engine_get() ? &tls_game_engine_globals()->statborg : 0;
}

long game_engine_scoring_get_player_compound_rank(long player_index, long team)
{
    s_game_engine_score_list score_list;
    game_engine_scoring_build_score_list(&score_list, team, false);
    long rank = NONE;
    for (long index = 0; index < score_list.player_count; index++)
    {
        if (player_index == score_list.player_indices[index])
        {
            rank = score_list.player_ranks[index];
            break;
        }
    }
    return rank;
}

// @address 0x29e418 game_engine_scoring_get_team_compound_rank
extern "C" long game_engine_scoring_get_team_compound_rank(long team, long mode)
{
    s_game_engine_score_list score_list;
    game_engine_scoring_build_score_list(&score_list, mode, false);
    long rank = NONE;
    for (long index = 0; index < score_list.team_count; index++)
    {
        if (team == score_list.team_indices[index])
        {
            rank = score_list.team_ranks[index];
            break;
        }
    }
    return rank;
}

// @address 0x41002 k_game_engine_scoring_keys_default
// @address 0x41004 k_game_engine_scoring_keys_mode0
// @address 0x4100c k_game_engine_scoring_keys_mode1
#if !defined(_XBOX) || defined(__clang__)
const char k_game_engine_scoring_keys_default[] = {'\xff'};
const char k_game_engine_scoring_keys_mode0[] = {
    _game_engine_score_key_player_playing, _game_engine_score_key_player_alive,
    _game_engine_score_key_player_reserved, _game_engine_score_key_team_active,
    _game_engine_score_key_team_score, _game_engine_score_key_team_index,
    _game_engine_score_key_player_score, '\xff'};
const char k_game_engine_scoring_keys_mode1[] = {
    _game_engine_score_key_player_playing, _game_engine_score_key_player_reserved,
    _game_engine_score_key_team_active, _game_engine_score_key_team_stat_1,
    _game_engine_score_key_player_alive, _game_engine_score_key_team_index,
    _game_engine_score_key_player_stat_1, _game_engine_score_key_team_engine,
    _game_engine_score_key_team_round_score, _game_engine_score_key_player_round_score,
    _game_engine_score_key_player_engine, _game_engine_score_key_player_kills,
    _game_engine_score_key_player_deaths, _game_engine_score_key_player_assists, '\xff'};
#endif

static __forceinline const char *game_engine_scoring_get_keys(unsigned long mode)
{
    const char *keys;
    switch (mode)
    {
    case 0:
        keys = k_game_engine_scoring_keys_mode0;
        break;
    case 1:
        keys = k_game_engine_scoring_keys_mode1;
        break;
    default:
        keys = k_game_engine_scoring_keys_default;
        break;
    }
    return keys;
}

static __forceinline bool game_engine_scoring_has_teams()
{
    return game_engine_get() && TEST_BIT(tls_game_engine_globals()->flags, 3);
}

static __forceinline int game_engine_scoring_team_out_of_range(long team)
{
    if (team < 0)
        return 1;
    if (team >= 8)
        return 1;
    return 0;
}

static __forceinline bool game_engine_scoring_is_team_ever_active(long team)
{
    return game_engine_scoring_has_teams() && !game_engine_scoring_team_out_of_range(team)
        && TEST_BIT(tls_game_engine_globals()->ever_active_teams, team);
}

// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z

// 0x29e4a8
void game_engine_scoring_build_score_list(s_game_engine_score_list *score_list, long mode, bool include_inactive)
{
    if (tls_game_time_globals()->time != g_game_engine_score_cache[mode].time)
    {
        s_game_engine_score_list *list = &g_game_engine_score_cache[mode].score_list;
        const char *keys = game_engine_scoring_get_keys(mode);
        memset(list, 0, sizeof(*list));
        c_data_iterator<player_datum> iterator;
        iterator.begin(tls_player_data());
        while (iterator.next())
        {
            list->player_indices[list->player_count] = iterator.get_index();
            list->player_count++;
        }
        s_player_comparison player_context = { keys, false };
        s_team_comparison team_context;
        long player_count = list->player_count;
        if ((unsigned long)player_count >= 2)
            templated_sort<long>::qsort(list->player_indices, player_count, game_engine_scoring_sort_players_compare_function, &player_context);
        for (long index = 0; index < list->player_count; index++)
            list->player_ranks[index] = game_engine_scoring_compute_player_compound_rank(list->player_indices[index], list, mode);
        if (game_engine_scoring_has_teams())
        {
            for (long team = 0; team < 8; team++)
            {
                if (game_engine_scoring_is_team_ever_active(team))
                {
                    list->team_indices[list->team_count] = team;
                    list->team_count++;
                }
            }
            team_context.keys = keys;
            team_context.descending = false;
            long team_count = list->team_count;
            if ((unsigned long)team_count >= 2)
                templated_sort<long>::qsort(list->team_indices, team_count,
                    (bool (*)(long, long, const void *))game_engine_scoring_sort_teams_compare, &team_context);
            for (long index = 0; index < list->team_count; index++)
                list->team_ranks[index] = game_engine_scoring_compute_team_compound_rank((e_game_team)list->team_indices[index], list, mode);
        }
        g_game_engine_score_cache[mode].time = tls_game_time_globals()->time;
    }
    *score_list = g_game_engine_score_cache[mode].score_list;
}

// @address 0x29e7b0 ?game_engine_get_best_player_score@@YAJXZ
long game_engine_get_best_player_score(void)
{
    long best_score = -32768;
    c_game_statborg *statborg = game_engine_get_statborg();
    if (statborg)
    {
        bool round_to_win = false;
        if (game_engine_get_score_to_win_round() == 1)
            round_to_win = true;
        if (game_engine_has_teams())
        {
            for (long team = 0; team < 8; team++)
            {
                if (::game_engine_is_team_active((e_game_team)(unsigned int)team))
                {
                    long score = statborg->get_team_stat(team, round_to_win ? 7 : 0);
                    if (score > best_score)
                        best_score = score;
                }
            }
        }
        else
        {
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next())
            {
                long score = statborg->get_player_stat(iterator.get_index(), round_to_win ? 7 : 0);
                if (score > best_score)
                    best_score = score;
            }
        }
        return best_score;
    }
    return 0;
}




long game_engine_scoring_compare_players_by_key(long first, long second, char key, bool deep)
{
    c_game_statborg *statborg = 0;
    if (game_engine_get())
        statborg = &tls_game_engine_globals()->statborg;
    player_datum *first_player = datum_get_typed<player_datum>(tls_player_data(), first);
    player_datum *second_player = datum_get_typed<player_datum>(tls_player_data(), second);
    long result = 0;
    switch (key)
    {
    case _game_engine_score_key_player_playing:
        if (!game_engine_has_teams())
            result = ((second_player->flags & 2) == 0) - ((first_player->flags & 2) == 0);
        break;
    case _game_engine_score_key_player_active:
        result = ((second_player->flags & 1) != 0) - ((first_player->flags & 1) != 0);
        break;
    case _game_engine_score_key_player_alive:
    {
        bool first_alive = !game_engine_player_is_out_of_lives(first);
        bool second_alive = !game_engine_player_is_out_of_lives(second);
        result = second_alive - first_alive;
        break;
    }
    case _game_engine_score_key_player_reserved:
        result = 0;
        break;
    case _game_engine_score_key_player_round_score:
        result = statborg->get_player_stat(second, 7) - statborg->get_player_stat(first, 7);
        break;
    case _game_engine_score_key_player_score:
        result = statborg->get_player_stat(second, 0) - statborg->get_player_stat(first, 0);
        break;
    case _game_engine_score_key_player_stat_1:
        result = statborg->get_player_stat(second, 1) - statborg->get_player_stat(first, 1);
        break;
    case _game_engine_score_key_player_kills:
        if (deep)
            result = statborg->get_player_stat(second, 2) - statborg->get_player_stat(first, 2);
        break;
    case _game_engine_score_key_player_deaths:
        if (deep)
            result = statborg->get_player_stat(first, 3) - statborg->get_player_stat(second, 3);
        break;
    case _game_engine_score_key_player_assists:
        if (deep)
            result = statborg->get_player_stat(second, 6) - statborg->get_player_stat(first, 6);
        break;
    case _game_engine_score_key_player_engine:
        if (deep)
        {
            const c_game_engine *engine = current_game_engine();
            if (engine)
                result = engine->compare_players(first, second);
        }
        break;
    case _game_engine_score_key_team_active:
    case _game_engine_score_key_team_alive:
    case _game_engine_score_key_team_round_score:
    case _game_engine_score_key_team_score:
    case _game_engine_score_key_team_stat_1:
    case _game_engine_score_key_team_kills:
    case _game_engine_score_key_team_deaths:
    case _game_engine_score_key_team_assists:
    case _game_engine_score_key_team_index:
    case _game_engine_score_key_team_engine:
        result = game_engine_scoring_compare_teams_by_key(
            (e_game_team)first_player->configuration.host.team_index,
            (e_game_team)second_player->configuration.host.team_index, key, deep);
        break;
    }
    return result;
}

long game_engine_scoring_compare_teams_by_key(e_game_team a, e_game_team b, char key, bool deep)
{
    e_game_team first = a;
    e_game_team second = b;
    c_game_statborg *statborg = 0;
    if (game_engine_get())
        statborg = &tls_game_engine_globals()->statborg;
    long result = 0;
    if (first >= 0 && first < 8 && second >= 0 && second < 8 && game_engine_has_teams() && first != second)
    {
        switch (key)
        {
        case _game_engine_score_key_team_active:
        {
            bool first_active = ::game_engine_is_team_active((e_game_team)(unsigned int)first);
            bool second_active = ::game_engine_is_team_active((e_game_team)(unsigned int)second);
            result = !!second_active - !!first_active;
            break;
        }
        case _game_engine_score_key_team_alive:
            result = !game_engine_team_is_out_of_lives(second) - !game_engine_team_is_out_of_lives(first);
            break;
        case _game_engine_score_key_team_round_score:
            result = statborg->get_team_stat(second, 7) - statborg->get_team_stat(first, 7);
            break;
        case _game_engine_score_key_team_score:
            result = statborg->get_team_stat(second, 0) - statborg->get_team_stat(first, 0);
            break;
        case _game_engine_score_key_team_stat_1:
            result = statborg->get_team_stat(second, 1) - statborg->get_team_stat(first, 1);
            break;
        case _game_engine_score_key_team_kills:
            if (deep)
                result = statborg->get_team_stat(second, 2) - statborg->get_team_stat(first, 2);
            break;
        case _game_engine_score_key_team_deaths:
            if (deep)
                result = statborg->get_team_stat(first, 3) - statborg->get_team_stat(second, 3);
            break;
        case _game_engine_score_key_team_assists:
            if (deep)
                result = statborg->get_team_stat(second, 6) - statborg->get_team_stat(first, 6);
            break;
        case _game_engine_score_key_team_index:
            if (deep)
                result = first - second;
            break;
        case _game_engine_score_key_team_engine:
            if (deep)
            {
                const c_game_engine *engine = current_game_engine();
                if (engine)
                    result = engine->compare_teams(first, second);
            }
            break;
        }
    }
    return result;
}

static __forceinline long game_engine_scoring_compare_teams_by_key_set(e_game_team team_a, e_game_team team_b, const char *keys, bool descending)
{
    long key_index = 0;
    long result = 0;
    while ((unsigned char)keys[key_index] != 255)
    {
        result = game_engine_scoring_compare_teams_by_key(team_a, team_b, keys[key_index], descending);
        if (result != 0)
            break;
        key_index++;
    }
    return result;
}

static __forceinline long game_engine_scoring_compare_players_by_key_set(long player_a, long player_b, const char *keys, bool deep)
{
    long key_index = 0;
    long result = 0;
    while ((unsigned char)keys[key_index] != 255)
    {
        result = game_engine_scoring_compare_players_by_key(player_a, player_b, keys[key_index], deep);
        if (result != 0)
            break;
        key_index++;
    }
    return result;
}

bool game_engine_scoring_sort_players_compare_function(long player_a, long player_b, const void *context_data)
{
    bool result = false;
    if (game_engine_get())
    {
        const s_player_comparison *context = (const s_player_comparison *)context_data;
        long comparison = game_engine_scoring_compare_players_by_key_set(player_a, player_b, context->keys, context->deep);
        result = comparison > 0;
    }
    return result;
}

// @address 0x29f058 game_engine_scoring_sort_teams_compare
extern "C" bool game_engine_scoring_sort_teams_compare(e_game_team team_a, e_game_team team_b, const s_team_comparison *context)
{
    return game_engine_scoring_compare_teams_by_key_set(team_a, team_b, context->keys, context->descending) > 0;
}

// @address 0x29f0c8 game_engine_scoring_compute_player_compound_rank
extern "C" long game_engine_scoring_compute_player_compound_rank(long player, const s_game_engine_score_list *score_list, unsigned long mode)
{
    long better = 0;
    long tied = 0;
    const char *keys = game_engine_scoring_get_keys(mode);
    for (long index = 0; index < score_list->player_count; index++)
    {
        long other = score_list->player_indices[index];
        if (player == (long)score_list->player_indices[index]) continue;
        if (other != NONE && datum_get_typed<player_datum>(tls_player_data(), other)->configuration.host.team_index == NONE) continue;
        long comparison = game_engine_scoring_compare_players_by_key_set(player, score_list->player_indices[index], keys, false);
        if (comparison > 0)
            better++;
        else if (comparison == 0)
            tied++;
        else
            break;
    }
    return (better << 1) + (tied > 0 ? 1 : 0);
}

// @address 0x29f1f8 game_engine_scoring_compute_team_compound_rank
extern "C" long game_engine_scoring_compute_team_compound_rank(e_game_team team, const s_game_engine_score_list *teams, unsigned long mode)
{
    long better = 0;
    long tied = 0;
    const char *keys;
    switch (mode)
    {
    case 0:
        keys = k_game_engine_scoring_keys_mode0;
        break;
    case 1:
        keys = k_game_engine_scoring_keys_mode1;
        break;
    default:
        keys = k_game_engine_scoring_keys_default;
        break;
    }
    for (long index = 0; index < (teams->team_count > 8 ? 8 : teams->team_count); index++)
    {
        e_game_team other = (e_game_team)teams->team_indices[index];
        if (team != other)
        {
            long comparison = game_engine_scoring_compare_teams_by_key_set(team, other, keys, false);
            if (comparison > 0)
                better++;
            else if (comparison == 0)
                tied++;
            else
                break;
        }
    }
    return (better << 1) + (tied > 0 ? 1 : 0);
}


// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z

static __forceinline int game_engine_scoring_engine_index_out_of_range(long index)
{
    if (index < 1)
        return 1;
    if (index > 10)
        return 1;
    return 0;
}

static __forceinline c_game_engine *game_engine_scoring_leaders_engine()
{
    long index = tls_game_engine_globals()->game_engine_index;
    c_game_engine *engine;
    if (!game_engine_scoring_engine_index_out_of_range(index))
        engine = game_engines[index];
    else
        engine = 0;
    return engine;
}

static __forceinline bool game_engine_scoring_is_predicted()
{
    bool result = false;
    if ((unsigned char)tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}

static __forceinline void game_engine_scoring_initialize_event(long type, long event, s_game_engine_event_data *event_data)
{
    event_data->type = type;
    event_data->event = event;
    event_data->identifier = -1;
    event_data->flags = -1;
    event_data->cause_player_index = -1;
    event_data->cause_team_index = -1;
    event_data->effect_player_index = -1;
    event_data->effect_team_index = -1;
    event_data->data = 0;
    event_data->data_word = -1;
}

static __forceinline int game_engine_scoring_find_team_rank(const s_game_engine_score_list *source, long index, const s_game_engine_score_list *list)
{
    int rank = -1;
    long key = source->team_indices[index];
    for (long i = 0; i < list->team_count; i++)
    {
        if (key == list->team_indices[i])
        {
            rank = (signed char)list->team_ranks[i];
            break;
        }
    }
    return rank;
}

static __forceinline int game_engine_scoring_find_player_rank(const s_game_engine_score_list *source, long index, const s_game_engine_score_list *list)
{
    int rank = -1;
    for (long i = 0; i < list->player_count; i++)
    {
        if (source->player_indices[index] == list->player_indices[i])
        {
            rank = (signed char)list->player_ranks[i];
            break;
        }
    }
    return rank;
}

// @address 0x29f300 ?game_engine_scoring_update_leaders_internal@@YAXPAUs_game_engine_score_list@@0@Z
void game_engine_scoring_update_leaders_internal(s_game_engine_score_list *old_leaders, s_game_engine_score_list *new_leaders)
{
    unsigned char teams = game_engine_has_teams();
    if (game_engine_scoring_is_predicted())
        return;
    if (!game_engine_scoring_leaders_engine()->leader_event_enabled())
        return;
    long old_count = teams ? old_leaders->team_count : old_leaders->player_count;
    for (long index = 0; index < old_count; index++)
    {
        signed char old_rank = teams ? old_leaders->team_ranks[index] : old_leaders->player_ranks[index];
        if (old_rank / 2 > 0)
            break;
        signed char new_rank = (signed char)(teams ? game_engine_scoring_find_team_rank(old_leaders, index, new_leaders)
                                                   : game_engine_scoring_find_player_rank(old_leaders, index, new_leaders));
        bool tied = old_rank & 1;
        if (new_rank != -1)
        {
            long event = -1;
            bool still_leading = new_rank < 2;
            bool same_lead = still_leading && new_rank == 1;
            if (still_leading)
            {
                if (tied && !same_lead)
                    event = (teams != 0) + 26;
            }
            else
            {
                event = (teams != 0) + 28;
            }
            if (event != -1)
            {
                s_game_engine_event_data event_data;
                game_engine_scoring_initialize_event(0, event, &event_data);
                if (teams)
                    event_data.cause_team_index = old_leaders->team_indices[index];
                else
                    event_data.cause_player_index = old_leaders->player_indices[index];
                game_engine_send_event(&event_data);
            }
        }
    }
    long new_count = teams ? new_leaders->team_count : new_leaders->player_count;
    for (long index = 0; index < new_count; index++)
    {
        signed char new_rank = teams ? new_leaders->team_ranks[index] : new_leaders->player_ranks[index];
        if (new_rank >= 2)
            break;
        signed char old_rank = (signed char)(teams ? game_engine_scoring_find_team_rank(new_leaders, index, old_leaders)
                                                   : game_engine_scoring_find_player_rank(new_leaders, index, old_leaders));
        if (old_rank != -1)
        {
            bool tied = new_rank - 1 == 0;
            long event = -1;
            bool old_leading = old_rank < 2;
            if (!old_leading)
            {
                if (tied)
                {
                    if (current_game_engine()->leader_event_lead_enabled())
                        event = (teams != 0) + 30;
                }
                else
                {
                    event = (teams != 0) + 26;
                }
            }
            if (event != -1)
            {
                s_game_engine_event_data event_data;
                game_engine_scoring_initialize_event(0, event, &event_data);
                if (teams)
                    event_data.cause_team_index = new_leaders->team_indices[index];
                else
                    event_data.cause_player_index = new_leaders->player_indices[index];
                game_engine_send_event(&event_data);
            }
        }
    }
}
