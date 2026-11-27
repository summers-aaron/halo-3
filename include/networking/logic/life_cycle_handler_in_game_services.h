#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_IN_GAME_SERVICES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_IN_GAME_SERVICES_H

extern "C++" {
#include "game/game_options.h"
#include "game/game_globals_storage.h"
class c_gui_queued_error;
enum e_controller_index;
class c_life_cycle_state_handler_in_game;

extern "C" c_gui_queued_error *gui_error_manager_find_queued_error(c_gui_queued_error *errors, e_controller_index controller_index,
    long error_name, const wchar_t *custom_message);
extern "C" void life_cycle_precache_mainmenu_after_game(c_life_cycle_state_handler_in_game *handler);
extern "C" bool saved_film_can_record(const s_game_options *options);
}

#endif // NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_IN_GAME_SERVICES_H
