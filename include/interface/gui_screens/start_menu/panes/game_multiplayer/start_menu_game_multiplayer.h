#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_MULTIPLAYER_START_MENU_GAME_MULTIPLAYER_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_MULTIPLAYER_START_MENU_GAME_MULTIPLAYER_H

#include "shell/shell.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_start_menu_game_multiplayer : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_game_multiplayer(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = 0x10047; // a_confirm_b_cancel
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }
    virtual ~c_start_menu_game_multiplayer() {}
    virtual void initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    static bool handle_leave_game_response(c_start_menu_pane_screen_widget *screen, const c_dialog_result_message *message);
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual void submenu_invoked(c_gui_list_widget *list);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    wchar_t m_hopper_name[32];
    wchar_t m_hopper_description[256];

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return true; }
};

typedef char c_start_menu_game_multiplayer_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_game_multiplayer) == 0x1d48))) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/start_menu_game_multiplayer.cpp that other units call.

extern "C" void start_menu_game_multiplayer_set_player_team(long controller_index, long value);
bool is_change_teams_allowed(e_controller_index controller_index);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_MULTIPLAYER_START_MENU_GAME_MULTIPLAYER_H
