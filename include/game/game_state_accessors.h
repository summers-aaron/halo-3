#ifndef GAME_GAME_STATE_ACCESSORS_H
#define GAME_GAME_STATE_ACCESSORS_H

#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"

__forceinline bool game_in_progress(game_globals_storage *globals = tls_game_globals())
{
    bool result = false;
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

__forceinline bool game_is_ui_shell()
{
    return tls_game_globals()->options.game_mode == 3;
}

#endif
