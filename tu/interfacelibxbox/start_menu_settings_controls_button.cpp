// interfacelibxbox/start_menu_settings_controls_button.obj: from the records 0x4b9c70 0x4b9cf0
// @flags /TP /O2 /J /Oc
#include "interface/gui_widget.h"
#include "interface/c_player_profile.h"
#include "interface/gui_screens/start_menu/panes/settings_controls_button/start_menu_settings_controls_button.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/gui_data.h"
#include "interface/c_gui_list_widget.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_widget.h"


// @address 0x4b9c70 ?post_initialize@c_start_menu_settings_controls_button@@UAAXXZ
void c_start_menu_settings_controls_button::post_initialize()
{
    c_gui_screen_widget *screen = this;
    unsigned long controller = (long)screen->get_any_responding_controller();
    if (controller <= 3 && controller_get((e_controller_index)controller)->in_use()) {
        c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
        if (list)
            list->set_focused_datasource_element_from_value(
                0x10a, controller_get((e_controller_index)controller)->get_player_profile_interface()->get_button_preset(), false);
    }
    c_gui_screen_widget::post_initialize();
}

// @address 0x4b9cf0 ?handle_controller_input_message@c_start_menu_settings_controls_button@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_controls_button::handle_controller_input_message(const c_controller_input_message *message)
{
    c_gui_screen_widget *screen = this;
    const c_controller_input_message *event = message;
    bool handled = false;
    c_gui_list_widget *list;
    c_gui_data *data;
    if (!screen->m_suppress_focus && screen->m_focused_widget && screen->m_focused_widget->m_type == 5) {
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
            list->m_name == _start_menu_string_start_menu_items) {
            if (element >= 0)
                controller_get((e_controller_index)controller)->get_player_profile_interface()->set_button_preset((e_button_presets)element, true);
            close_current_subpane();
            handled = true;
        }
    }
    if (!handled)
        return c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
