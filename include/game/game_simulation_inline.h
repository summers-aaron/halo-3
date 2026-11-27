#ifndef GAME_GAME_SIMULATION_INLINE_H
#define GAME_GAME_SIMULATION_INLINE_H
#include "game/game_globals_storage.h"
static __forceinline bool game_is_playback()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4) result = true;
    return result;
}
static __forceinline bool game_is_authoritative() { return !game_is_playback(); }
#endif
