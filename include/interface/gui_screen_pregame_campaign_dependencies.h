#ifndef INTERFACE_GUI_SCREEN_PREGAME_CAMPAIGN_DEPENDENCIES_H
#define INTERFACE_GUI_SCREEN_PREGAME_CAMPAIGN_DEPENDENCIES_H

#include "saved_games/saved_game_files.h"
#include "interface/user_interface_session.h"

class c_gui_screen_pregame_lobby_campaign;
e_controller_index user_interface_networking_get_saved_game_controller();
extern "C" void user_interface_initialize_coop_game_settings(c_gui_screen_pregame_lobby_campaign *, e_controller_index);

#endif
