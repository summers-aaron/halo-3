#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MULTIPLAYER_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MULTIPLAYER_H

#include "interface/gui_game_mode_types.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_lobby_multiplayer.cpp.

extern "C" bool parse_xml_pregame_map_name(void *context, wchar_t *buffer, unsigned long size);

class c_gui_screen_pregame_lobby_multiplayer : public c_gui_screen_pregame_lobby
{
public:
    __forceinline c_gui_screen_pregame_lobby_multiplayer(long name) : c_gui_screen_pregame_lobby(name) {}
    virtual ~c_gui_screen_pregame_lobby_multiplayer() {}
    virtual void initialize();
    virtual void update(unsigned long milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual long get_lobby_header();
    virtual long get_lobby_title();
    virtual long get_start_button_name();
    virtual long get_start_status_text(bool is_leader);
    virtual long get_gui_game_mode() { return _gui_game_mode_multiplayer; }
};
#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MULTIPLAYER_H
