#ifndef GAME_GAME_TIME_INLINE_H
#define GAME_GAME_TIME_INLINE_H

#include "game/game_time_access.h"

// March game_time.cpp:168,577-578,585,617-618. The debug assertions check the TLS
// pointer and initialized flag.
static __forceinline bool game_time_initialized()
{
    return game_time_globals && game_time_globals->initialized;
}

static __forceinline long game_tick_rate()
{
    return game_time_globals->ticks_per_second;
}

static __forceinline float game_tick_length()
{
    return game_time_globals->tick_length;
}

static __forceinline long game_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + 0.5f);
}

static __forceinline long game_seconds_to_ticks_integer(long seconds)
{
    return seconds * game_time_globals->ticks_per_second;
}

static __forceinline float game_seconds_to_ticks_real(float seconds)
{
    return seconds * (float)game_time_globals->ticks_per_second;
}

#endif // GAME_GAME_TIME_INLINE_H
