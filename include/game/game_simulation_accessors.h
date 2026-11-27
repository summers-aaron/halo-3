#ifndef GAME_GAME_SIMULATION_ACCESSORS_H
#define GAME_GAME_SIMULATION_ACCESSORS_H

#include "game/game_globals_storage.h"
#include "game/game_simulation_types.h"

static __forceinline e_game_simulation_type game_simulation_get()
{
    return (e_game_simulation_type)game_globals->options.simulation_type;
}

#endif
