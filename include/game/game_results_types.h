#ifndef GAME_GAME_RESULTS_TYPES_H
#define GAME_GAME_RESULTS_TYPES_H

// The per-player game results statistics. Names and order: the statistic definition table at 0x827450e0
// (51 entries of {name, ...}; "games-played" ... "infection-time-as-human"), each enumerator the entry's name.
// Repeated "king-unused" slots use ordinal suffixes; unnamed gaps retain their numeric positions.
enum e_game_results_statistic
{
    _game_results_statistic_games_played = 0,
    _game_results_statistic_games_completed,
    _game_results_statistic_games_won,
    _game_results_statistic_games_tied,
    _game_results_statistic_rounds_completed,
    _game_results_statistic_rounds_won,
    _game_results_statistic_in_round_score,
    _game_results_statistic_in_game_total_score,
    _game_results_statistic_kills,
    _game_results_statistic_assists,
    _game_results_statistic_deaths,    // 10
    _game_results_statistic_betrayals,
    _game_results_statistic_suicides,
    _game_results_statistic_most_kills_in_a_row,
    _game_results_statistic_seconds_alive,
    _game_results_statistic_ctf_flag_scores,
    _game_results_statistic_ctf_flag_grabs,
    _game_results_statistic_ctf_flag_carrier_kills,
    _game_results_statistic_ctf_flag_returns,
    _game_results_statistic_assault_bomb_arms,
    _game_results_statistic_assault_bomb_grabs,    // 20
    _game_results_statistic_assault_bomb_disarms,
    _game_results_statistic_assault_bomb_detonations,
    _game_results_statistic_oddball_time_with_ball,
    _game_results_statistic_oddball_unused,
    _game_results_statistic_oddball_kills_as_carrier,
    _game_results_statistic_oddball_ball_carrier_kills,
    _game_results_statistic_king_time_on_hill,
    _game_results_statistic_king_total_control_time,
    _game_results_statistic_king_unused_0, // 29: "king-unused"
    _game_results_statistic_king_unused_1, // 30: "king-unused"
    // 31..33: "unused" (the table repeats the name)
    _game_results_statistic_vip_takedowns = 34,
    _game_results_statistic_vip_kills_as_vip,
    _game_results_statistic_vip_guard_time,
    _game_results_statistic_vip_time_as_vip,
    _game_results_statistic_vip_lives_as_vip,
    _game_results_statistic_juggernaut_kills,
    _game_results_statistic_juggernaut_kills_as_juggernaut,    // 40
    _game_results_statistic_juggernaut_total_control_time,
    // 42, 43: "juggernaut-unused" (the table repeats the name)
    _game_results_statistic_territories_owned = 44,
    _game_results_statistic_territories_captures,
    _game_results_statistic_territories_ousts,
    _game_results_statistic_territories_time_in_territory,
    _game_results_statistic_infection_zombie_kills,
    _game_results_statistic_infection_infections,
    _game_results_statistic_infection_time_as_human,    // 50

    k_game_results_statistic_count                          // 51
};

#endif // GAME_GAME_RESULTS_TYPES_H
