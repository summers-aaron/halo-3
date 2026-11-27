#ifndef GAME_GAME_TIME_INLINES_H
#define GAME_GAME_TIME_INLINES_H

#include "memory/tls_globals.h"
#include "game/game_time_types.h"

static __forceinline long game_seconds_integer_to_ticks(long seconds)
{
    return seconds * tls_game_time_globals()->ticks_per_second;
}
#endif // GAME_GAME_TIME_INLINES_H
