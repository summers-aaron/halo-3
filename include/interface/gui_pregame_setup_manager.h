#ifndef INTERFACE_GUI_PREGAME_SETUP_MANAGER_H
#define INTERFACE_GUI_PREGAME_SETUP_MANAGER_H

// Functions defined in tu/interfacelibxbox/gui_pregame_setup_manager.cpp that other units call.

struct s_gui_game_setup_storage;
class c_gui_pregame_setup_manager;

extern c_gui_pregame_setup_manager g_pregame_setup_manager;

void global_preferences_set_last_game_setup(const s_gui_game_setup_storage *last_game_setup);
extern "C" bool gui_pregame_setup_manager_get_film_map_title(void *context, wchar_t *output, long capacity);
extern "C" bool gui_pregame_setup_manager_initialize_squad_for_mode(long game_mode);

#endif // INTERFACE_GUI_PREGAME_SETUP_MANAGER_H
