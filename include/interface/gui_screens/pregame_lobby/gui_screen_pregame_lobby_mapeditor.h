#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MAPEDITOR_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_MAPEDITOR_H

#include "interface/gui_game_mode_types.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"

// String ids the mapeditor lobby screen uses, named after their cache string table entries.
enum e_pregame_lobby_mapeditor_string_id
{
    _pregame_lobby_mapeditor_string_map = 0x270,
    _pregame_lobby_mapeditor_string_title_mapeditor = 0x102ac,
    _pregame_lobby_mapeditor_string_header_mapeditor = 0x102b1,
    _pregame_lobby_mapeditor_string_start_mapeditor = 0x102ba,
};

class c_gui_screen_pregame_lobby_mapeditor : public c_gui_screen_pregame_lobby
{
public:
    __forceinline c_gui_screen_pregame_lobby_mapeditor(long name) : c_gui_screen_pregame_lobby(name) {}
    virtual ~c_gui_screen_pregame_lobby_mapeditor() {}
    virtual void initialize();
    virtual void post_initialize();
    virtual void update(unsigned long milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual long get_lobby_header();
    virtual long get_lobby_title();
    virtual long get_start_button_name();
    virtual long get_start_status_text(bool is_leader);
    virtual long get_gui_game_mode() { return _gui_game_mode_mapeditor; }
};

#endif
