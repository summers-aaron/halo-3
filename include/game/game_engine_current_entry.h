#ifndef GAME_GAME_ENGINE_CURRENT_ENTRY_H
#define GAME_GAME_ENGINE_CURRENT_ENTRY_H

#include "game/game_engine_list.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"

static __forceinline c_game_engine *current_game_engine_entry(void)
{
    long index = tls_game_engine_globals()->game_engine_index;
    if (index < 1)
        return 0;
    if (index > 10)
        return 0;
    return game_engines[index];
}

#endif
