#ifndef GAME_GAME_TIME_ACCESS_H
#define GAME_GAME_TIME_ACCESS_H

#include "game/game_time_types.h"


static __forceinline long game_time_get()
{
    return game_time_globals->time;
}

// Convert the tick count with the current tick length.
static __forceinline float game_ticks_to_seconds(float ticks)
{
    return ticks * game_time_globals->tick_length;
}

#endif // GAME_GAME_TIME_ACCESS_H
