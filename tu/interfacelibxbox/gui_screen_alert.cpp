// interfacelibxbox/gui_screen_alert.obj: from the records 0x4c0948 0x4c0d20 0x4c0e90
// @flags /TP /O2 /Oc
// @address 0x1143b50 g_gui_error_manager
#include "memory/static_arrays.h"
#include "interface/gui_render_data_size_types.h"
#include <string.h>
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_data.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_bitmap_widget.h"
#include "cache/cache_files.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/gui_screen_widget_legacy.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "interface/user_interface_error_manager.h"
#include "interface/gui_widget.h"
#include "tag_files/tag_types.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/alert/gui_screen_alert.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"

enum
{
    k_alert_primary_action = 0,
    k_alert_secondary_action = 1
};

// @address 0x4c0948 ?update@c_gui_alert_screen_widget@@UAAXK@Z
void c_gui_alert_screen_widget::update(unsigned long current_milliseconds)
{
    c_gui_queued_error *queued_error = gui_error_manager_find_alert_screen_error(this);
    if (queued_error)
    {
        long delay = get_constants_datasource_integer(0x10069);
        c_gui_data *data = 0;
        for (long index = 0; index < m_datasource_count; index++)
        {
            if (m_datasource[index]->get_name() == 0x10069)
            {
                data = m_datasource[index];
                break;
            }
        }
        c_gui_widget *footer = (c_gui_widget *)m_current_display_group_widgets
                                   [c_gui_screen_widget::_display_group_type_button_key]; // c_gui_group_widget: no shared header yet
        c_gui_bitmap_widget *icon = (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, 0x1006a);
        c_gui_widget *buttons = get_child_widget(_gui_group_widget, 0x10544);
        const wchar_t *title = queued_error->get_custom_title();
        const wchar_t *message = queued_error->get_custom_message();
        if (title && wcslen(title))
            gui_screen_widget_set_text_widget_text(this, 669, queued_error->get_custom_title());
        else
            set_text_widget_string_id(669, queued_error->title);
        if (message && wcslen(message))
            gui_screen_widget_set_text_widget_text(this, 0x10005, queued_error->get_custom_message());
        else
            set_text_widget_string_id(0x10005, queued_error->message);
        if (footer)
        {
            long name = !queued_error->requires_resolution ? get_initial_button_key_name() : 0x10043;
            if (footer->m_name != name)
                gui_screen_widget_set_display_group(this, 1, name);
        }
        if (icon)
        {
            icon->set_visible(!queued_error->show_buttons);
            bitmap_group *definition =
                TAG_GET('bitm', bitmap_group, icon->m_definition.bitmap_definition_index);
            short count = 0;
            if (definition)
            {
                if (definition->sequences.count != 0)
                    count = (short)((bitmap_sequence *)definition->sequences.address)->sprites.count;
                else
                    count = (short)definition->bitmaps.count;
            }
            long frame = queued_error->icon;
            long last = count - 1;
            // match: 0x4c0948: by-reference select spills frame/last to stack slots 0x54/0x50; a by-value select keeps them in registers
            const long &sprite_frame = frame > last ? last : frame;
            icon->set_sprite_frame(sprite_frame);
        }
        if (buttons)
            buttons->set_visible(queued_error->show_buttons);
        if (data)
        {
            for (long element = data->get_first_element_handle(); element != -1; element = data->get_next_element_handle(element))
            {
                long element_name;
                if (data->get_string_id_value(element, 668, &element_name) && element_name == queued_error->error_name)
                {
                    long element_value;
                    if (data->get_integer_value(element, 266, &element_value))
                        delay = element_value;
                }
            }
        }
        if (delay != -1 && current_milliseconds - m_creation_time_milliseconds > (unsigned long)delay && !queued_error->requires_resolution)
        {
            g_gui_error_manager.clear_error(m_error.error_name, m_error.controller_index, m_error.get_custom_message());
            transition_out_with_transition_type(1, _screen_transition_normal);
        }
    }
    else
    {
        transition_out_with_transition_type(1, _screen_transition_normal);
    }
    c_gui_screen_widget::update(current_milliseconds);
    if (m_name == _string_id_gui_alert_nonblocking)
        set_responds_to_controller_events(false);
}

// @address 0x4c0d20 ?handle_controller_input_message@c_gui_alert_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_alert_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press)
    {
        switch (message->get_component())
        {
        case k_alert_primary_action:
        case k_alert_secondary_action:
        case _controller_component_button_start:
        {
            c_gui_queued_error *queued_error = gui_error_manager_find_alert_screen_error(this);
            if (queued_error)
            {
                bool resolved = !queued_error->requires_resolution;
                if (resolved)
                {
                    g_gui_error_manager.clear_error(m_error.error_name, m_error.controller_index, m_error.get_custom_message());
                    transition_out_with_transition_type(1, _screen_transition_normal);
                }
                handled = true;
                if (message->get_component() == _controller_component_button_start && resolved)
                    handled = false;
            }
            break;
        }
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4c0e90 ?get_render_data_size@c_gui_alert_screen_widget@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_alert_screen_widget::get_render_data_size()
{
    return _render_data_size_alert;
}
