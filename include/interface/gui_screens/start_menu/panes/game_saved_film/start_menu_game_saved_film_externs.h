#ifndef INTERFACE_START_MENU_GAME_SAVED_FILM_EXTERNS_H
#define INTERFACE_START_MENU_GAME_SAVED_FILM_EXTERNS_H

class c_gui_screen_widget;
struct s_saved_film_playback;
extern "C" {
extern s_saved_film_playback *g_saved_film_playback;
void start_menu_game_multiplayer_handle_leave_game(c_gui_screen_widget *screen, long controller_index);
}
#endif
