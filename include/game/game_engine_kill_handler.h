#ifndef GAME_GAME_ENGINE_KILL_HANDLER_H
#define GAME_GAME_ENGINE_KILL_HANDLER_H

#include "cseries/cseries.h"
#include "game/game_engine_kill_flags_types.h"

extern "C" void game_engine_handle_player_killed(void *engine, long killer, long assistant,
    long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage);

#endif
