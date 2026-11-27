#ifndef GAME_GAME_RESULTS_DAMAGE_STATISTICS_TYPES_H
#define GAME_GAME_RESULTS_DAMAGE_STATISTICS_TYPES_H

#include "game/game_results_statistic_shared_types.h"

struct s_game_results_damage_statistics
{
    bool active;
    char pad;
    s_game_results_statistic values[5];
};

#endif // GAME_GAME_RESULTS_DAMAGE_STATISTICS_TYPES_H
