#ifndef GAME_GAME_RESULTS_TEAM_STATISTICS_TYPES_H
#define GAME_GAME_RESULTS_TEAM_STATISTICS_TYPES_H

#include "game/game_results_statistic_shared_types.h"
#include "game/game_results_types.h"

struct s_game_results_team_statistics
{
    union {
        s_game_results_statistic statistics[k_game_results_statistic_count];
        unsigned short values[k_game_results_statistic_count];
    };
};

#endif // GAME_GAME_RESULTS_TEAM_STATISTICS_TYPES_H
