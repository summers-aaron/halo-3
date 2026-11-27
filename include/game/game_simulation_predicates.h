#ifndef GAME_GAME_SIMULATION_PREDICATES_H
#define GAME_GAME_SIMULATION_PREDICATES_H

#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "game/game_simulation_types.h"

namespace game_simulation
{
static __forceinline long game_simulation_get()
{
    return (signed char)tls_game_globals()->options.simulation_type;
}

static __forceinline bool game_is_predicted()
{
    bool result = false;
    long simulation_type = tls_game_globals()->options.simulation_type;
    if (simulation_type == 4)
        result = true;
    return result;
}
static __forceinline bool game_is_authoritative() { return !game_is_predicted(); }
static __forceinline bool game_is_server()
{
    bool result = true;
    long simulation_type = tls_game_globals()->options.simulation_type;
    if (simulation_type == _game_simulation_synchronous_client || simulation_type == _game_simulation_distributed_client)
        result = false;
    return result;
}
static __forceinline bool game_is_distributed()
{
    bool result = false;
    if ((unsigned int)(tls_game_globals()->options.simulation_type - 4) <= 1)
        result = true;
    return result;
}
}

#endif
