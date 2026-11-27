#ifndef GAME_GAME_ENGINE_CURRENT_INLINE_H
#define GAME_GAME_ENGINE_CURRENT_INLINE_H

#include "game/game_engine_current_entry.h"

static __forceinline const c_game_engine *current_game_engine(void)
{
    return current_game_engine_entry();
}
#endif
