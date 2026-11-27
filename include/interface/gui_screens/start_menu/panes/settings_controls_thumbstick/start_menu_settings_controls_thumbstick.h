#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_THUMBSTICK_START_MENU_SETTINGS_CONTROLS_THUMBSTICK_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_THUMBSTICK_START_MENU_SETTINGS_CONTROLS_THUMBSTICK_H

#include "cseries/cseries.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_controller_input_message;

// The controls thumbstick pane of the start menu settings.
class c_start_menu_settings_controls_thumbstick : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings_controls_thumbstick(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = NONE;
    }
    virtual ~c_start_menu_settings_controls_thumbstick() {}
    virtual void post_initialize();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return false; }
};

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_CONTROLS_THUMBSTICK_START_MENU_SETTINGS_CONTROLS_THUMBSTICK_H
