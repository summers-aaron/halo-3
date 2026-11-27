#ifndef CAMERA_GAME_DIRECTOR_H
#define CAMERA_GAME_DIRECTOR_H

#include "shell/shell.h"

// Functions defined in tu/blamlibxbox/game_director.cpp.

class c_director;

extern "C" void director_update_scoreboard_screen(c_director *director);
void player_set_desired_respawn_player(long player_index, long target_index);

enum e_director_perspective;
enum e_seat_state;

e_director_perspective director_game_desired_perspective(long unit_index, e_seat_state *seat_state);
extern "C" unsigned char director_update_watched_player(c_director *director);

class c_gui_screen_widget;

extern "C" c_gui_screen_widget *director_get_scoreboard_screen(e_window_index window_index);

#endif // CAMERA_GAME_DIRECTOR_H
