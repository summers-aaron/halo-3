#ifndef GAME_GAME_ENGINE_SCRIPTING_H
#define GAME_GAME_ENGINE_SCRIPTING_H

// Functions defined in tu/blamlibxbox/game_engine_scripting.cpp that other units call.

extern "C" void game_engine_respawn_override_timers(short team);
void game_engine_give_object_ownership_to_team(long object_index,short team_index);

#endif // GAME_GAME_ENGINE_SCRIPTING_H
