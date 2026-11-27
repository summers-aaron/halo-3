#ifndef GAME_GAME_RESULTS_PACKET_H
#define GAME_GAME_RESULTS_PACKET_H

#include "game/game_results_player_data.h"
#include "game/game_results_damage_statistics_types.h"
#include "game/game_results_statistic_shared_types.h"
#include <stddef.h>

// Elements of s_game_results_incremental_update (game/game_results_incremental_update.h).
struct s_game_results_packet_player
{
    bool exists;
    bool has_definition;
    s_game_results_player_data definition;
};
struct s_game_results_packet_team
{
    bool exists;
    bool has_identifier;
    union
    {
        unsigned char identifier[4];
        struct
        {
            bool flag;
            signed char standing;
            short final_score;
        };
    };
};
struct s_game_results_packet_player_statistics
{
    bool exists;
    bool statistics_valid;
    s_game_results_statistic statistics[51];
    bool statistics_58_valid;
    s_game_results_statistic statistics_58[58];
    bool statistics_49_valid;
    s_game_results_statistic statistics_49[49];
    s_game_results_damage_statistics damage[61];
};
struct s_game_results_packet_player_vs_player_statistics
{
    bool valid;
    s_game_results_statistic statistics[2];
};
struct s_game_results_packet_team_statistics
{
    bool valid;
    s_game_results_statistic statistics[51];
};
struct s_game_results_packet_machine
{
    bool valid;
    char pad1;
    unsigned char identifier[6];
    bool exists;
    bool connected;
    bool host;
    bool initial_host;
    bool voluntary_quit;
    char padd;
    unsigned char bandwidth_estimate[10];
};

typedef char game_results_packet_player_size[(sizeof(s_game_results_packet_player) == 0x138) ? 1 : -1];
typedef char game_results_packet_player_definition_offset[(offsetof(s_game_results_packet_player, definition) == 8) ? 1 : -1];
typedef char game_results_packet_team_size[(sizeof(s_game_results_packet_team) == 6) ? 1 : -1];
typedef char game_results_packet_player_statistics_58_offset[
    (offsetof(s_game_results_packet_player_statistics, statistics_58) == 0x6a) ? 1 : -1];
typedef char game_results_packet_player_statistics_49_offset[
    (offsetof(s_game_results_packet_player_statistics, statistics_49) == 0xe0) ? 1 : -1];
typedef char game_results_packet_player_statistics_size[(sizeof(s_game_results_packet_player_statistics) == 0x41e) ? 1 : -1];
typedef char game_results_packet_player_vs_player_statistics_size[
    (sizeof(s_game_results_packet_player_vs_player_statistics) == 6) ? 1 : -1];
typedef char game_results_packet_team_statistics_size[(sizeof(s_game_results_packet_team_statistics) == 0x68) ? 1 : -1];
typedef char game_results_packet_machine_size[(sizeof(s_game_results_packet_machine) == 0x18) ? 1 : -1];
typedef char game_results_packet_machine_bandwidth_estimate_offset[
    (offsetof(s_game_results_packet_machine, bandwidth_estimate) == 0xe) ? 1 : -1];

class c_bitstream;
struct s_game_results_incremental_update;
extern "C" void game_results_encode_packet(c_bitstream *packet, const s_game_results_incremental_update *results);

#endif
