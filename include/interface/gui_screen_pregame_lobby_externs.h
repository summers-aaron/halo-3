#ifndef INTERFACE_GUI_SCREEN_PREGAME_LOBBY_EXTERNS_H
#define INTERFACE_GUI_SCREEN_PREGAME_LOBBY_EXTERNS_H

#include "interface/gui_location_manager.h"


// External dependencies used by gui_screen_pregame_lobby.
class c_network_session;
class c_gui_widget;

extern "C" bool network_configuration_parse_lobby_film_max_players(void *context, wchar_t *output, long capacity);
extern "C" bool network_configuration_parse_lobby_forge_max_players(void *context, wchar_t *output, long capacity);
extern "C" bool network_configuration_parse_lobby_coop_max_players(void *context, wchar_t *output, long capacity);
extern "C" void user_interface_leave_squad(unsigned char leave_reason);
extern "C" bool user_interface_squad_try_and_get_session(c_network_session **session);
extern "C" bool user_interface_networking_is_team_switching_allowed(void);
extern "C" long user_interface_session_get_controller_index(long player_index);
extern "C" c_gui_widget *gui_screen_widget_find_list_item_by_value(c_gui_widget *widget, long first, long second, long third);
extern "C" int network_squad_session_get_countdown_delayed_culprit(void);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long name);

class c_game_variant;
class c_gui_roster_list_widget;
struct s_life_cycle_globals;
enum e_map_id;
extern "C" {
extern s_life_cycle_globals g_network_life_cycle_globals;
}




extern "C" void gui_pregame_lobby_commit_team_change(long player_index, c_gui_roster_list_widget *roster_list, long timestamp);
extern "C" const c_game_variant *gui_pregame_lobby_get_current_variant();
extern "C" e_map_id gui_pregame_lobby_get_current_map_id();

#endif // INTERFACE_GUI_SCREEN_PREGAME_LOBBY_EXTERNS_H
