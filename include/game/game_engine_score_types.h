#ifndef GAME_GAME_ENGINE_SCORE_TYPES_H
#define GAME_GAME_ENGINE_SCORE_TYPES_H

#include <string.h>

struct s_game_engine_score_list
{
    long player_indices[16];
    long team_indices[8];
    char player_ranks[16];
    char team_ranks[8];
    short player_count;
    short team_count;
};

struct s_game_engine_score_cache
{
    __forceinline s_game_engine_score_cache()
    {
        time = -1;
        memset(&score_list, 0, sizeof(score_list));
    }

    long time;
    s_game_engine_score_list score_list;
};

extern "C" {
extern s_game_engine_score_cache g_game_engine_score_cache[2];
}

#endif // GAME_GAME_ENGINE_SCORE_TYPES_H
