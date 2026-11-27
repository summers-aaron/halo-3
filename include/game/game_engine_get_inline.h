#ifndef GAME_GAME_ENGINE_GET_INLINE_H
#define GAME_GAME_ENGINE_GET_INLINE_H

#include "game/game_engine_list.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"

static __forceinline c_game_engine *game_engine_get(void)
{
    if (((s_game_engine_globals *)game_engine_globals)->game_engine_index >= 1 &&
        ((s_game_engine_globals *)game_engine_globals)->game_engine_index <= 10)
        return game_engines[((s_game_engine_globals *)game_engine_globals)->game_engine_index];
    return 0;
}

static __forceinline c_map_variant *game_engine_get_runtime_map_variant(void)
{
    return &tls_game_engine_globals()->map_info;
}


#endif // GAME_GAME_ENGINE_GET_INLINE_H
