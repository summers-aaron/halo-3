// interfacelibxbox/start_menu_settings_appearance.obj: from the records 0x4ba118
// @flags /TP /O2 /Oc
#include "interface/gui_screens/start_menu/panes/settings_appearance/start_menu_settings_appearance.h"
#include "interface/gui_data.h"
#include "interface/user_interface.h"


#include "interface/controller_inline.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget_tree.h"
#include "interface/gui_widget_navigation.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/user_interface_utilities.h"
// @address 0x4ba118 ?post_initialize@c_start_menu_settings_appearance@@UAAXXZ
void c_start_menu_settings_appearance::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    if (get_alpha_is_internal_beta())
    {
        c_gui_data *data = 0;
        for (long index = 0; index < m_datasource_count; index++)
        {
            if (m_datasource[index]->get_name() == 0x1010f)
            {
                data = m_datasource[index];
                break;
            }
        }
        if (data)
            data->set_disabled_element(668, 0x10133);
    }
}

// @address 0x4bc288 ?update_render_state@c_start_menu_settings_appearance@@UAAXK@Z
void c_start_menu_settings_appearance::update_render_state(unsigned long current_milliseconds) {
    e_controller_index controller_index = get_any_responding_controller();
    long color = controller_get(controller_index)->player_profile.customization.primary_change_color;
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_widget *child = get_child_widget(_gui_bitmap_widget, _start_menu_string_base_color);
    while (child != 0) {
        if (child->m_name == _start_menu_string_base_color) tint_widget_to_change_color(child, color, false);
        child = gui_widget_get_next_sibling_of_type(child, _gui_bitmap_widget);
    }
}
