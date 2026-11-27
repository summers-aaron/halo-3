#ifndef GAME_GAME_ENGINE_JUGGERNAUT_EVENTS_H
#define GAME_GAME_ENGINE_JUGGERNAUT_EVENTS_H

// Accepted juggernaut caller uses the raw kill flag word.
extern "C" void game_engine_handle_player_killed(const void *engine, long killer, long assistant, long victim, bool betrayal, unsigned long flags, long damage);

#endif // GAME_GAME_ENGINE_JUGGERNAUT_EVENTS_H
