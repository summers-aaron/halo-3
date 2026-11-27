#ifndef GAME_GAME_VARIANT_INLINE_H
#define GAME_GAME_VARIANT_INLINE_H

#include "game/game_globals_storage.h"


static __forceinline const c_game_variant *current_game_variant()
{
    const game_options *options = &game_globals->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}

#endif
