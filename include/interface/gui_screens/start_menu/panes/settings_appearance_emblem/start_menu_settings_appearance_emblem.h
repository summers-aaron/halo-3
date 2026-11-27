#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_EMBLEM_START_MENU_SETTINGS_APPEARANCE_EMBLEM_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_EMBLEM_START_MENU_SETTINGS_APPEARANCE_EMBLEM_H

#include "shell/shell.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_start_menu_settings_appearance_emblem : public c_start_menu_pane_screen_widget {
public:
    __forceinline c_start_menu_settings_appearance_emblem(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = -1;
    }
    virtual ~c_start_menu_settings_appearance_emblem() {}
    virtual void post_initialize();
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }
};

// Functions defined in tu/interfacelibxbox/start_menu_settings_appearance_emblem.cpp that other units call.

void player_emblem_toggle_alternate_emblem_foreground(e_controller_index controller_index);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_EMBLEM_START_MENU_SETTINGS_APPEARANCE_EMBLEM_H
