#ifndef INTERFACE_GUI_SCREENS_C_GUI_SCREEN_PREGAME_SWITCH_LOBBY_H
#define INTERFACE_GUI_SCREENS_C_GUI_SCREEN_PREGAME_SWITCH_LOBBY_H

#include "interface/c_gui_screen_widget.h"

class c_gui_screen_pregame_switch_lobby : public c_gui_screen_widget
{
public:
    enum e_lobby_selection
    {
        _lobby_selection_campaign = 0x10074,
        _lobby_selection_matchmaking = 0x10073,
        _lobby_selection_custom_games = 0x333,
        _lobby_selection_forge = 0x10078,
        _lobby_selection_theater = 0x10079
    };

    __forceinline c_gui_screen_pregame_switch_lobby(long name) : c_gui_screen_widget(name) {}
    virtual void post_initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
};

typedef char c_gui_screen_pregame_switch_lobby_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_pregame_switch_lobby) == 0x1a88))) ? 1 : -1];

#endif
