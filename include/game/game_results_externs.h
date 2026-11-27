#ifndef GAME_GAME_RESULTS_EXTERNS_H
#define GAME_GAME_RESULTS_EXTERNS_H

#include "networking/network_globals.h"

class c_game_results;
class c_bitstream;

extern "C" bool player_is_on_local_machine(unsigned short player_index);
extern "C" void game_results_grant_top_player_achievement(c_game_results *game_results);
extern "C" void network_game_definition_player_decode(c_bitstream *packet, void *player);

extern "C" {
extern bool g_game_results_player_left;
extern const long k_game_results_maximum_statistic;
}

#endif // GAME_GAME_RESULTS_EXTERNS_H
