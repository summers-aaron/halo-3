#ifndef GAME_GAME_GLOBALS_INLINE_H
#define GAME_GAME_GLOBALS_INLINE_H

#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"

__forceinline bool game_is_active()
{
    game_globals_storage *globals = tls_game_globals();
    return globals && (globals->initializing || globals->active);
}

__forceinline bool game_is_multiplayer()
{
    return tls_game_globals()->options.game_mode == 2;
}

__forceinline bool game_is_campaign()
{
    return tls_game_globals()->options.game_mode == 1;
}

#endif // GAME_GAME_GLOBALS_INLINE_H
