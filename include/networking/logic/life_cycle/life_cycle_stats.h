#ifndef INCLUDE_NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATS_H
#define INCLUDE_NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATS_H

class c_life_cycle_state_handler;
class c_network_session;
class c_game_results;

extern "C" void network_leaderboard_write_stats(long session, const c_game_results *results);
extern "C" bool life_cycle_is_session_peers_ready(c_life_cycle_state_handler *handler, c_network_session *session);

#endif // INCLUDE_NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_STATS_H
