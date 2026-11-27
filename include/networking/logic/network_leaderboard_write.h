#ifndef NETWORKING_LOGIC_NETWORK_LEADERBOARD_WRITE_H
#define NETWORKING_LOGIC_NETWORK_LEADERBOARD_WRITE_H

struct game_player_options;
extern "C" void network_leaderboard_write_initial_stats(long, unsigned short, bool, bool, long, float, float, long, unsigned long, const game_player_options *);

#endif // NETWORKING_LOGIC_NETWORK_LEADERBOARD_WRITE_H
