// interfacelibxbox/gui_screen_maximum_party_size.obj: from the records 0x4d2d58 0x4d2de0 0x4d2f30 0x4d2f78 0x4d3040 0x4d3198
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "interface/gui_screen_maximum_party_size_externs.h"
#include "interface/gui_screen_menu_externs.h"
#include "interface/gui_widget_types.h"
#include <stddef.h>
#include "interface/gui_widget.h"
#include "interface/user_interface_memory.h"
#include "networking/logic/network_session_interface.h"
#include "interface/user_interface_networking.h"
#include "interface/game_tag_parser.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/c_gui_list_widget.h"
// @address 0x4d3198 ?parse_party_size@@YA_NPAXPA_WJ@Z
// @address 0x3b0b28 ?network_squad_session_get_maximum_player_count@@YAJXZ
// @address 0x3b2110 ?user_interface_networking_get_closed@@YA_NXZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z


#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_data.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_screens/gui_screen_maximum_party_size.h"

bool parse_party_size(void *context, wchar_t *buffer, long count)
{
 long party_size = user_interface_networking_get_current_party_size();
 usnzprintf(buffer, count, L"%d", party_size);
 return true;
}








// @address 0x4d2d58 ?initialize@c_gui_screen_maximum_party_size@@UAAXXZ
void c_gui_screen_maximum_party_size::initialize()
{
    c_gui_screen_widget::initialize();
    c_game_tag_parser *parser = new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<party_size", this, parse_party_size);
    add_game_tag_parser(parser);
}

static __forceinline bool c_gui_screen_maximum_party_size_size_is_available(long party_size)
{
    if (party_size == -1)
        return true;
    return party_size >= user_interface_networking_get_current_party_size() && party_size <= 16;
}

// @address 0x4d2de0 ?update_availability@c_gui_screen_maximum_party_size@@QAAXXZ
void c_gui_screen_maximum_party_size::update_availability()
{
    long name = m_name;
    c_gui_list_widget *list = get_child_list_widget(name);
    if (list == 0)
        return;
    c_gui_data *data = list->get_data();
    c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget);
    if (item == 0)
        return;
    do {
        bool enabled = false;
        bool focused = false;
        long party_size;
        if (data->get_integer_value(item->get_element_handle(), name, &party_size)) {
            if (c_gui_screen_maximum_party_size_size_is_available(party_size))
                enabled = true;
            if (item->get_element_handle() < list->get_focused_element_handle())
                focused = true;
        }
        item->set_enabled(enabled);
        gui_widget_set_alternate_state(item, focused);
        item = gui_widget_get_next_list_item(item, true);
    } while (item != 0);
}


// @address 0x4d2f30 ?update@c_gui_screen_maximum_party_size@@UAAXK@Z
void c_gui_screen_maximum_party_size::update(unsigned long current_milliseconds)
{
    update_availability();
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4d2f78 ?post_initialize@c_gui_screen_maximum_party_size@@UAAXXZ
void c_gui_screen_maximum_party_size::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    long name = m_name;
    c_gui_list_widget *list = get_child_list_widget(name);
    c_gui_data *data = list ? list->get_data() : 0;
    update_availability();
    if (list && data) {
        long value = user_interface_networking_get_closed() ? -1 : network_squad_session_get_maximum_player_count();
        if (list->set_focused_datasource_element_from_value(0x100b2, value, 0)) {
            long handle = list->get_focused_element_handle();
            c_gui_widget *item = gui_list_widget_find_item_by_element_handle(list, handle);
            this->transfer_focus_without_animations(item, 1, 1);
        }
    }
}

// @address 0x4d3040 ?handle_controller_input_message@c_gui_screen_maximum_party_size@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_maximum_party_size::handle_controller_input_message(const c_controller_input_message *message)
{
    long event_type = message->get_event_type();
    long component = message->get_component();
    bool handled = 0;
    if (event_type == 13 && component == 0) {
        c_gui_widget *focused = get_focused_widget();
        c_gui_widget *item = focused && focused->get_type() == _gui_list_item_widget ? focused : 0;
        c_gui_list_widget *list = item ? gui_widget_get_parent_list_widget(item) : 0;
        c_gui_data *data = list ? list->get_data() : 0;
        if (list && data) {
            if (item->get_enabled()) {
                long name = m_name;
                long party_size = -1;
                if (data->get_integer_value(list->get_focused_element_handle(), name, &party_size)) {
                    if (party_size == -1) {
                        user_interface_networking_set_closed(1);
                    } else {
                        user_interface_networking_set_closed(0);
                        long value = party_size;
                        if (value > 0 && value <= 16)
                            network_squad_session_set_maximum_player_count(value);
                    }
                    transition_out_with_transition_type(0, _screen_transition_normal);
                }
            }
        }
        handled = 1;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}
