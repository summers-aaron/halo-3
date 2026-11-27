#ifndef NETWORKING_LOGIC_NETWORK_LEADERBOARD_H
#define NETWORKING_LOGIC_NETWORK_LEADERBOARD_H

// Functions defined in tu/networkinglibxbox/network_leaderboard.cpp.

void network_leaderboard_remove_users(void);
void network_leaderboard_update_queries(void);
void network_leaderboard_update_writes_in_progress(void);
void network_leaderboard_notify_user_stats_updated(long user_index);
void network_leaderboard_update_leaderboards(void);

class c_game_results;
struct s_network_leaderboard_game_results;
struct s_network_leaderboard_player_game_results;

extern "C" int network_leaderboard_get_write_status(void);
void network_leaderboard_build_leaderboard_player_game_results_from_game_results(long player_index,
    const s_network_leaderboard_game_results *leaderboard,const c_game_results *results,long relative_score,
    s_network_leaderboard_player_game_results *player_game_results_out);
void network_leaderboard_user_query_failed(long user_index);
void network_leaderboard_submit_query_for_user(long user_index);
void network_leaderboard_write_stats_internal(long session_index, const s_network_leaderboard_game_results *game,
    const s_network_leaderboard_player_game_results *players, bool final);
extern "C" void build_relative_player_scores_from_game_results(const c_game_results *results,
    const s_network_leaderboard_game_results *leaderboard,long *scores,long);
void network_leaderboard_build_leaderboard_game_results_from_game_results(const c_game_results *game,s_network_leaderboard_game_results *out);

// Data defined in tu/networkinglibxbox/network_leaderboard.cpp that other units use.

struct s_network_leaderboard_globals;

extern s_network_leaderboard_globals network_leaderboard_globals;

#endif // NETWORKING_LOGIC_NETWORK_LEADERBOARD_H
