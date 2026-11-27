#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_SAVED_FILM_START_MENU_GAME_SAVED_FILM_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_SAVED_FILM_START_MENU_GAME_SAVED_FILM_H

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_start_menu_game_saved_film : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_game_saved_film(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = 0x10047; // a_confirm_b_cancel
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }
    virtual ~c_start_menu_game_saved_film() {}
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *source);
protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return true; }

public:

    e_controller_index m_exit_controller;
};
typedef char saved_film_exit_controller_offset[(sizeof(void *) != 4 || ((offsetof(c_start_menu_game_saved_film, m_exit_controller) == 0x1b08))) ? 1 : -1];
typedef char start_menu_game_saved_film_size[(sizeof(void *) != 4 || ((sizeof(c_start_menu_game_saved_film) == 0x1b10))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_GAME_SAVED_FILM_START_MENU_GAME_SAVED_FILM_H
