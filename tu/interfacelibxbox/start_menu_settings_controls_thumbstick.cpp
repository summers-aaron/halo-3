// interfacelibxbox/start_menu_settings_controls_thumbstick.obj: from the records 0x4b9e50 0x4b9ed0
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/gui_widget.h"
#include "interface/gui_screens/start_menu/panes/settings_controls_thumbstick/start_menu_settings_controls_thumbstick.h"
#include "interface/gui_data.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_widget.h"


// @address 0x4b9e50 ?post_initialize@c_start_menu_settings_controls_thumbstick@@UAAXXZ
void c_start_menu_settings_controls_thumbstick::post_initialize()
{
    e_controller_index controller = get_any_responding_controller();
    if ((unsigned long)controller <= 3 && controller_get((e_controller_index)controller)->in_use())
    {
        c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, 0x1010a);
        if (list)
            list->set_focused_datasource_element_from_value(
                0x10a, controller_get((e_controller_index)controller)->get_player_profile_interface()->get_joystick_preset(), false);
    }
    c_gui_screen_widget::post_initialize();
}

// @address 0x4b9ed0 ?handle_controller_input_message@c_start_menu_settings_controls_thumbstick@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_controls_thumbstick::handle_controller_input_message(const c_controller_input_message *message)
{
    c_gui_screen_widget *screen = this;
    const c_controller_input_message *event = message;
    bool handled = false;
    c_gui_list_widget *list;
    c_gui_data *data;
    if (!screen->m_suppress_focus && screen->m_focused_widget && screen->m_focused_widget->type == 5) {
        list = gui_widget_get_parent_list_widget(screen->m_focused_widget);
        data = list ? list->get_data() : 0;
    } else {
        list = 0;
        data = 0;
    }
    unsigned long controller = event->m_controller;
    long type = event->m_event_type;
    long value = event->m_component;
    if (controller <= 3 && controller_get((e_controller_index)controller)->in_use() && list && data &&
        type == 13 && value == 0) {
        long element = -1;
        if (data->get_integer_value(list->get_focused_element_handle(), 266, &element) &&
            list->get_name() == 0x1010a) {
            if (element >= 0)
                controller_get((e_controller_index)controller)->get_player_profile_interface()->set_joystick_preset((e_joystick_presets)element, true);
            close_current_subpane();
            handled = true;
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
