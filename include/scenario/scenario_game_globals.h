#ifndef SCENARIO_SCENARIO_GAME_GLOBALS_H
#define SCENARIO_SCENARIO_GAME_GLOBALS_H

#include "scenario/scenario.h"

static __forceinline s_game_globals *scenario_get_game_globals()
{
    return global_game_globals;
}

#endif
