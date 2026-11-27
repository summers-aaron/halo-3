#ifndef INTERFACE_GUI_SCREENS_POSTGAME_LOBBY_GUI_SCREEN_POSTGAME_LOBBY_H
#define INTERFACE_GUI_SCREENS_POSTGAME_LOBBY_GUI_SCREEN_POSTGAME_LOBBY_H

// Functions defined in tu/interfacelibxbox/gui_screen_postgame_lobby.cpp.

struct s_life_cycle_matchmaking_progress;

extern "C" bool gui_screen_postgame_lobby_get_session_value_text(void *context, wchar_t *buffer, long count);
extern "C" bool gui_screen_postgame_lobby_has_leader_rematch_request(s_life_cycle_matchmaking_progress *progress);

#endif // INTERFACE_GUI_SCREENS_POSTGAME_LOBBY_GUI_SCREEN_POSTGAME_LOBBY_H
