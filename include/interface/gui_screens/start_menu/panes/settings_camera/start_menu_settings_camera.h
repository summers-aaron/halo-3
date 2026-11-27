#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CAMERA_START_MENU_SETTINGS_CAMERA_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CAMERA_START_MENU_SETTINGS_CAMERA_H

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "tag_files/string_ids.h"

class c_start_menu_settings_camera : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_camera(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_automatic_button_key = _string_id_a_confirm_b_cancel;
        m_translation = 0.0f;
        m_rotation = 0.0f;
    }

    virtual void initialize();
    virtual void initialize_datasource();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void submenu_invoked(c_gui_list_widget *list_widget);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }
};

typedef char start_menu_settings_camera_size[(sizeof(void *) != 4 || sizeof(c_start_menu_settings_camera) == 0x1b08) ? 1 : -1];

#endif
