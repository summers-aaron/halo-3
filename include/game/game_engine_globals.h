#ifndef GAME_GAME_ENGINE_GLOBALS_H
#define GAME_GAME_ENGINE_GLOBALS_H

struct s_game_engine_globals;

// Thread-local game engine state, owned by game_engine.cpp.
extern __declspec(thread) s_game_engine_globals *game_engine_globals;

#endif // GAME_GAME_ENGINE_GLOBALS_H
