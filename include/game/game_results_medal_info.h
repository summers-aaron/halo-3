#ifndef GAME_GAME_RESULTS_MEDAL_INFO_H
#define GAME_GAME_RESULTS_MEDAL_INFO_H

struct s_medal_info
{
    long priority;
    short order;
    short pad;
};

extern const s_medal_info g_game_results_medal_info[58];

#endif // GAME_GAME_RESULTS_MEDAL_INFO_H
