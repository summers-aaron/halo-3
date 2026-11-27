#ifndef GAME_GAME_ENGINE_SCORING_H
#define GAME_GAME_ENGINE_SCORING_H

#include "game/game_types.h"

enum e_game_engine_score_comparison_key
{
    _game_engine_score_key_player_playing = 0,
    _game_engine_score_key_player_active = 1,
    _game_engine_score_key_player_alive = 2,
    _game_engine_score_key_player_reserved = 3,
    _game_engine_score_key_player_round_score = 4,
    _game_engine_score_key_player_score = 5,
    _game_engine_score_key_player_stat_1 = 6,
    _game_engine_score_key_player_kills = 7,
    _game_engine_score_key_player_deaths = 8,
    _game_engine_score_key_player_assists = 9,
    _game_engine_score_key_team_active = 10,
    _game_engine_score_key_team_alive = 11,
    _game_engine_score_key_team_round_score = 12,
    _game_engine_score_key_team_score = 13,
    _game_engine_score_key_team_stat_1 = 14,
    _game_engine_score_key_team_kills = 15,
    _game_engine_score_key_team_deaths = 16,
    _game_engine_score_key_team_assists = 17,
    _game_engine_score_key_team_index = 18,
    _game_engine_score_key_player_engine = 19,
    _game_engine_score_key_team_engine = 20,
};

// Functions defined in tu/blamlibxbox/game_engine_scoring.cpp.

struct s_game_engine_score_list;

long game_engine_scoring_get_player_compound_rank(long player_index, long team);
void game_engine_scoring_build_score_list(s_game_engine_score_list *score_list, long mode, bool include_inactive);
long game_engine_get_best_player_score(void);
void game_engine_scoring_update_leaders_internal(s_game_engine_score_list *old_leaders, s_game_engine_score_list *new_leaders);
extern "C" long game_engine_scoring_get_team_compound_rank(long team, long mode);
bool game_engine_scoring_sort_players_compare_function(long player_a, long player_b, const void *context_data);
extern "C" long game_engine_scoring_compute_player_compound_rank(long player, const s_game_engine_score_list *score_list, unsigned long mode);

struct s_team_comparison;

long game_engine_scoring_compare_teams_by_key(e_game_team a, e_game_team b, char key, bool deep);
extern "C" bool game_engine_scoring_sort_teams_compare(e_game_team team_a, e_game_team team_b, const s_team_comparison *context);
extern "C" long game_engine_scoring_compute_team_compound_rank(e_game_team team, const s_game_engine_score_list *teams, unsigned long mode);

// The score-sort key strings game_engine_scoring.cpp reads from .rdata, one per sort mode.
extern "C" {
extern const char k_game_engine_scoring_keys_default[];
extern const char k_game_engine_scoring_keys_mode0[];
extern const char k_game_engine_scoring_keys_mode1[];
}

#endif // GAME_GAME_ENGINE_SCORING_H
