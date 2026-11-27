#ifndef GAME_GAME_RESULTS_STATISTICS_TYPES_H
#define GAME_GAME_RESULTS_STATISTICS_TYPES_H

#include "game/game_results_damage_statistics_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_types.h"

struct s_game_results_player_statistics
{
    union
    {
        s_game_results_statistic statistics[k_game_results_statistic_count];
        unsigned short values[524];
        struct
        {
            s_game_results_statistic game_statistics[k_game_results_statistic_count];
            s_game_results_statistic medals[58];
            s_game_results_statistic achievements[49];
            s_game_results_damage_statistics damage_statistics[61];
        };
    };
};

struct s_game_results_statistics
{
    s_game_results_player_statistics player[16];
    union
    {
        unsigned short pvp_stats[16][16][2];
        s_game_results_statistic pvp_statistics[16][16][2];
    };
    s_game_results_team_statistics team[16];
};

typedef char game_results_player_statistics_size[(sizeof(s_game_results_player_statistics) == 0x418) ? 1 : -1];
typedef char game_results_statistics_size[(sizeof(s_game_results_statistics) == 0x4be0) ? 1 : -1];

#endif // GAME_GAME_RESULTS_STATISTICS_TYPES_H
