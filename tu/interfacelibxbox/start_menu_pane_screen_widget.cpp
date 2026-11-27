// interfacelibxbox/start_menu_pane_screen_widget.obj: from the records 0x4b1ed8 0x4e8828 0x4e89b8 0x4e8a28 0x4e8a98 0x4e8c08 0x4e8e40 0x4e8ed8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "interface/gui_widget_tree.h"
#include "interface/gui_widget_navigation.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_deletable.h"
#include "interface/screen_transition_types.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "interface/user_interface_error_manager.h"
#include "interface/gui_widget_hierarchy.h"
#include <new>
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"

class c_message;
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x4e8e40 ?handle_controller_input_message@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x48b738 gui_widget_get_parent_list_widget
// @address 0x489b58 ?get_element_handle@c_gui_widget@@UAAJXZ
// @address 0x4898b8 ?get_enabled@c_gui_widget@@UBA_NXZ

// @address 0x4e87a0 ??0c_xenon_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z

// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x48b098 ?start_animation_at_time@c_gui_widget@@QAAXW4e_animation_state@@K_N@Z
// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
// @address 0x498698 ?load_pane@c_start_menu_screen_widget@@QAAXJ_NW4e_screen_transition_type@@JJMM@Z

// The debug builds do not supply a name for this forwarding entry point.
// @address 0x4b1ed8 start_menu_pane_screen_widget_handle_controller_input
extern "C" void start_menu_pane_screen_widget_handle_controller_input(c_start_menu_pane_screen_widget *screen,
                          const c_controller_input_message *message)
{
    screen->c_start_menu_pane_screen_widget::handle_controller_input_message(message);
}

// @address 0x4e8828 ?update@c_start_menu_pane_screen_widget@@UAAXK@Z
void c_start_menu_pane_screen_widget::update(unsigned long current_milliseconds)
{
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, 0x1010a);
    if (list && list->within_focus_chain()) {
        c_gui_data *source = list->get_data();
        long value = -1;
        if (source && source->get_string_id_value(list->get_focused_element_handle(), 0x10012, &value))
            gui_screen_widget_set_display_group(this, 0, value);
    }
    if (m_automatic_button_key != -1) {
        c_gui_group_widget *button_key_group = m_current_display_group_widgets[_display_group_type_button_key];
        bool selected = false;
        for (c_gui_widget *column = gui_widget_find_child_by_name(this, _gui_group_widget); column && !selected;
            column = gui_widget_get_next_sibling_of_type(column, _gui_group_widget))
        {
            for (c_gui_widget *item = gui_widget_find_child_by_name(column, _gui_list_widget); item;
                item = gui_widget_get_next_sibling_of_type(item, _gui_list_widget)) {
                if (static_cast<c_gui_list_widget *>(item)->m_submenu_owner_list_item) {
                    selected = true;
                    break;
                }
            }
        }
        long shown;
        if (selected)
            shown = m_automatic_button_key;
        else
            shown = get_initial_button_key_name();
        if (button_key_group && button_key_group->get_name() != shown)
            gui_screen_widget_set_display_group(this, 1, shown);
    }
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4e89b8 ?close_current_subpane@c_start_menu_pane_screen_widget@@QAAXXZ
void c_start_menu_pane_screen_widget::close_current_subpane()
{
    void *storage = user_interface_malloc_tracked(sizeof(c_xenon_message), 0, 0);
    if (storage) {
        e_controller_index controller = get_any_responding_controller();
        c_xenon_message *message = new (storage) c_xenon_message(0, controller, get_render_window(), -1);
        if (message)
            ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
    }
}

// @address 0x4e8a28 ?close_start_menu@c_start_menu_pane_screen_widget@@QAAXXZ
void c_start_menu_pane_screen_widget::close_start_menu()
{
    void *storage = user_interface_malloc_tracked(sizeof(c_xenon_message), 0, 0);
    if (storage) {
        e_controller_index controller = get_any_responding_controller();
        c_xenon_message *message = new (storage) c_xenon_message(2, controller, get_render_window(), -1);
        if (message)
            ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
    }
}

// @address 0x4e8a98 ?controller_event_should_go_to_parent@c_start_menu_pane_screen_widget@@MAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_pane_screen_widget::controller_event_should_go_to_parent(const c_controller_input_message *message)
{
    bool result = false;
    if (message->m_event_type == _event_type_button_press && message->m_component == _controller_component_button_start) {
        result = true;
    } else {
        c_start_menu_screen_widget *parent = (c_start_menu_screen_widget *)get_parent_screen();
        c_gui_widget *focused = get_focused_widget();
        if (parent && focused && focused->m_type == 5 && gui_widget_get_parent_list_widget(focused)) {
            c_gui_widget *list = (c_gui_widget *)gui_widget_get_parent_list_widget(focused);
            if (list->name == _start_menu_string_start_menu_items || (unsigned long)(list->name - 0x10594) <= 1) {
                bool handle = false;
                long event = message->m_event_type;
                long component = message->m_component;
                if (parent->m_pane != -1) {
                    c_gui_widget *child = parent->get_child_widget(_gui_screen_widget, parent->m_pane);
#if defined(_XBOX) && !defined(__clang__)
                    // match: 0x4e8a98: the slot-50 call leaves r4 as it is; passing message adds
                    // `mr r4,r30` before the bctrl and a sixth saved register (__savegprlr_27).
                    if (child && ((bool (**)(c_gui_widget *))*(void **)child)[50](child))
#else
                    if (child && ((c_start_menu_pane_screen_widget *)child)->controller_event_should_go_to_parent(message))
#endif
                        handle = parent->m_breadcrumbs.count() == 1 && (event == 2 || event == 4);
                }
                result = handle;
                // match: 0x4e8a98: && true forces clrlwi normalization of this |= step (bare form feeds cntlzw/rlwinm into or without it).
                result |= component == _controller_component_button_start && true;
                // match: 0x4e8a98: && true forces clrlwi normalization of this |= step (bare form feeds cntlzw/rlwinm into or without it).
                result |= component == 1 && true;
            }
        }
    }
    return result;
}
// @address 0x4e8c08 ?handle_list_item_chosen@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_pane_screen_widget::handle_list_item_chosen(const c_controller_input_message *message, long list,
    c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list == 0x1010a) {
        long unused = -1;
        long element = item->get_element_handle();
        if (data->get_string_id_value(element, 0x29c, &unused)) {
            long name = -1;
            if (data->get_string_id_value(element, 0x10011, &name) && name != -1 && name != 0) {
                c_start_menu_screen_widget *screen = (c_start_menu_screen_widget *)get_parent_screen();
                if (screen) {
                    screen->load_pane(name, false, _screen_transition_custom0, -1, -1, m_translation, m_rotation);
                    item->start_animation_at_time(_animation_state_custom0, (unsigned long)-1, true);
                }
                handled = true;
            } else if (data->get_string_id_value(element, 0x10006, &name) && name != -1 && name != 0 && item->get_enabled()) {
                (gui_widget_get_parent_list_widget(item))->invoke_submenu(name, -1, false);
                handled = true;
            } else if (data->get_string_id_value(element, 0x10064, &name) && name != -1 && name != 0 && item->get_enabled()) {
                g_gui_error_manager.post_error_with_custom_message(name, 0, 0, message->get_controller(), false, true);
                handled = true;
            }
        }
    }
    if (!handled)
        handled = false;
    return handled;
}

// @address 0x4e8e40 ?handle_controller_input_message@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_pane_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (c_start_menu_pane_screen_widget::controller_event_should_go_to_parent(message) && get_parent_screen() && get_parent_screen() != this)
        handled = get_parent_screen()->handle_controller_input_message(message);
    if (!handled)
        return c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4e8ed8 start_menu_pane_is_valid_type
extern "C" unsigned char start_menu_pane_is_valid_type(const c_gui_widget *widget)
{
    bool result = false;
    if (widget) {
        switch (widget->get_name()) {
        case _start_menu_string_start_menu_game_saved_film:
        case _start_menu_string_start_menu_service_record:
        case _start_menu_string_start_menu_file_share:
        case _start_menu_string_start_menu_bungie_file_share:
        case _start_menu_string_start_menu_hq_screenshots:
        case _start_menu_string_start_menu_hq_transfers:
        case _start_menu_string_start_menu_hq:
        case _start_menu_string_start_menu_settings_controls:
        case _start_menu_string_start_menu_settings_controls_button:
        case _start_menu_string_start_menu_settings_controls_thumbstick:
        case _start_menu_string_start_menu_settings_appearance:
        case _start_menu_string_start_menu_settings_appearance_model:
        case _start_menu_string_start_menu_settings_appearance_emblem:
        case _start_menu_string_start_menu_settings_appearance_colors:
        case _start_menu_string_start_menu_settings_voice:
        case _start_menu_string_start_menu_settings_camera:
        case _start_menu_string_start_menu_settings_film_autosave:
        case _start_menu_string_start_menu_settings_display:
            result = true;
            break;
        }
    }
    return result;
}
