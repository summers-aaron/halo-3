#ifndef GAME_GAME_SKULLS_H
#define GAME_GAME_SKULLS_H

#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"

static __forceinline int skull_out_of_range(int skull, int count)
{
    if ((skull < 0)) return 1;
    if ((skull >= count)) return 1;
    return 0;
}

static __forceinline bool game_secondary_skull_is_active(long skull)
{
    unsigned long skulls = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.secondary_skulls : 0;
    return ((skulls >> skull) & 1) != 0;
}

#endif // GAME_GAME_SKULLS_H
