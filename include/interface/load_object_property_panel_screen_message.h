#ifndef INTERFACE_LOAD_OBJECT_PROPERTY_PANEL_SCREEN_MESSAGE_H
#define INTERFACE_LOAD_OBJECT_PROPERTY_PANEL_SCREEN_MESSAGE_H

#include "interface/user_interface_messages.h"

class c_load_object_property_panel_screen_message : public c_load_screen_message
{
public:
    c_load_object_property_panel_screen_message(e_controller_index controller, e_window_index window,
        long layered_position, long object_index, bool is_secondary);
    c_load_object_property_panel_screen_message(long screen_name, e_controller_index controller,
        e_window_index window, long layered_position, long object_index)
        : c_load_screen_message(screen_name, controller, window, layered_position), m_object_index(object_index) {}
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    long m_object_index;
    long m_property_index;
    long m_property_value;
};

#endif
