#ifndef GAME_GAME_ENGINE_SLAYER_EVENTS_H
#define GAME_GAME_ENGINE_SLAYER_EVENTS_H

#include "game/game_engine_default.h"

// Accepted slayer caller signature; the engine argument is read-only here.
extern "C" void game_engine_handle_player_killed(const void *, long, long, long, bool,
    c_flags<e_game_engine_kill_flags, long, 24>, long);

#endif
