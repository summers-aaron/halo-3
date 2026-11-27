#ifndef GAME_GAME_RESULTS_INCREMENTAL_UPDATE_H
#define GAME_GAME_RESULTS_INCREMENTAL_UPDATE_H

#include "game/game_results_packet.h"
#include <stddef.h>

struct s_game_results_incremental_update
{
    bool summary_valid;
    long summary_value;
    bool finish_valid;
    long finish_value;
    bool finalized;
    s_game_results_packet_player players[16];
    s_game_results_packet_team teams[16];
    s_game_results_packet_player_statistics player_statistics[16];
    s_game_results_packet_player_vs_player_statistics player_vs_player_statistics[16][16];
    s_game_results_packet_team_statistics team_statistics[16];
    s_game_results_packet_machine machines[16];
};

typedef char game_results_incremental_update_size[(sizeof(s_game_results_incremental_update) == 25560) ? 1 : -1];
typedef char game_results_incremental_update_players_offset[(offsetof(s_game_results_incremental_update, players) == 24) ? 1 : -1];
typedef char game_results_incremental_update_machines_offset[(offsetof(s_game_results_incremental_update, machines) == 0x6258) ? 1 : -1];

#endif
