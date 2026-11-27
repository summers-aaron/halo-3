#ifndef GAME_GAME_ENGINE_INFECTION_H
#define GAME_GAME_ENGINE_INFECTION_H

// Functions defined in tu/blamlibxbox/game_engine_infection.cpp.

extern "C" bool player_is_zombie(long player_index);
bool player_is_human(long player_index);
long get_last_human_player_index(void);
void player_set_zombie_status(long player_index, bool zombie, bool starting);
extern "C" void infection_handle_player_infection(long p0, bool p1);

#endif // GAME_GAME_ENGINE_INFECTION_H
