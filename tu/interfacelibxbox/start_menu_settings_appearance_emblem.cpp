// interfacelibxbox/start_menu_settings_appearance_emblem.obj: from the records 0x4bc108 0x4bc1d0 0x4bc288 0x4bc328 0x4bc440 0x4bc5b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include <stddef.h>
#include "interface/user_interface_utilities.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_emblem/start_menu_settings_appearance_emblem.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_widget_tree_queries.h"
#include "interface/gui_list_widget_queries.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
enum e_emblem_list_name { _emblem_foreground_list = 0x10594, _emblem_background_list = 0x10595 };

enum e_emblem_navigation_component { _emblem_previous_color = 14, _emblem_next_color = 15 };

class c_font_cache;
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"

// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x47d698 ?set_emblem_info@c_player_profile_interface@@QAAXPBUs_emblem_info@@_N@Z
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x4e8c08 ?handle_list_item_chosen@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x4e8828 ?update@c_start_menu_pane_screen_widget@@UAAXK@Z
#include "interface/gui_screen_widget_text.h"
#include "interface/controller_inline.h"
// @address 0x11401e8 g_controller_globals
namespace {

static __forceinline c_player_profile_interface *get_player_profile_interface(c_controller_interface *controller) {
    return controller->get_player_profile_interface();
}
static __forceinline bool get_async_io_busy(const c_player_profile_interface *profile) {
    return (profile->flags & 0x100) != 0;
}
static __forceinline s_emblem_info get_emblem_info(const c_player_profile_interface *profile) {
    return profile->customization.emblem_info;
}
static __forceinline bool emblem_foreground_is_alternate(const s_emblem_info *info) {
    bool result = (info->flags & 1) != 0;
    return result;
}
static __forceinline void emblem_foreground_set_alternate(s_emblem_info *info, bool alternate) {
    if (alternate) info->flags |= 1;
    else info->flags &= ~1;
}
}
// @address 0x4bc108 ?post_initialize@c_start_menu_settings_appearance_emblem@@UAAXXZ
void c_start_menu_settings_appearance_emblem::post_initialize() {
    c_gui_screen_widget::post_initialize();
    e_controller_index controller_index = get_any_responding_controller();
    c_controller_interface *controller = controller_get(controller_index);
    c_player_profile_interface *profile = get_player_profile_interface(controller);
    s_emblem_info emblem = profile->customization.emblem_info;
    c_gui_widget *foreground_list = get_child_widget(_gui_list_widget, _emblem_foreground_list);
    c_gui_widget *background_list = get_child_widget(_gui_list_widget, _emblem_background_list);
    if (foreground_list != 0 && background_list != 0) {
        ((c_gui_list_widget *)foreground_list)->set_focused_datasource_element_from_value(0x10327, emblem.foreground, true);
        ((c_gui_list_widget *)background_list)->set_focused_datasource_element_from_value(0x10327, emblem.background, true);
    }
}
// @address 0x4bc1d0 ?update@c_start_menu_settings_appearance_emblem@@UAAXK@Z
void c_start_menu_settings_appearance_emblem::update(unsigned long current_milliseconds) {
    if (!m_suppress_focus && m_focused_widget) {
        c_gui_widget *list = gui_widget_get_parent_list_widget(m_focused_widget);
        if (list) {
            c_gui_data *data = ((c_gui_list_widget *)list)->get_data();
            if (data) {
                long value = -1;
                if (data->get_string_id_value(0, 0x10012, &value)) gui_screen_widget_set_display_group(this, 0, value);
            }
        }
    }
    c_start_menu_pane_screen_widget::update(current_milliseconds);
}
// @address 0x4bc288 ?update_render_state@c_start_menu_settings_appearance_emblem@@UAAXK@Z
void c_start_menu_settings_appearance_emblem::update_render_state(unsigned long current_milliseconds) {
    e_controller_index controller_index = get_any_responding_controller();
    c_controller_interface *controller = controller_get(controller_index);
    // match: 0x4bc288: retain the existing field-first indexed load; profile-pointer access shrinks this body by 4 bytes (check 3).
    long color = *(int *)((char *)g_controller_globals.controllers + offsetof(c_controller_interface, player_profile) +
        offsetof(c_player_profile_interface, customization.primary_change_color) + controller_index * sizeof(s_controller));
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_widget *child = get_child_widget(_gui_bitmap_widget, _start_menu_string_base_color);
    while (child != 0) {
        if (child->m_name == _start_menu_string_base_color) tint_widget_to_change_color(child, color, false);
        child = gui_widget_get_next_sibling_of_type(child, _gui_bitmap_widget);
    }
}
// @address 0x4bc328 ?handle_list_item_chosen@c_start_menu_settings_appearance_emblem@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_settings_appearance_emblem::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    bool foreground = false;
    switch (list) {
    case _emblem_foreground_list:
        foreground = true;
    case _emblem_background_list: {
        c_controller_interface *controller = controller_get(message->get_controller());
        c_player_profile_interface *profile = get_player_profile_interface(controller);
        s_emblem_info info = get_emblem_info(profile);
        long element = item->get_element_handle();
        long image = -1;
        if (data->get_integer_value(element, 0x10327, &image)) {
            bool valid = image >= 0;
            if (valid) {
                if (foreground) info.foreground = (unsigned char)image;
                else info.background = (unsigned char)image;
                profile->set_emblem_info(&info, true);
            }
        }
        handled = true;
    } break;
    }
    if (!handled) handled = c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}
// @address 0x4bc440 ?handle_controller_input_message@c_start_menu_settings_appearance_emblem@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_appearance_emblem::handle_controller_input_message(const c_controller_input_message *message) {
    bool handled = false;
    switch (message->get_event_type()) {
    case 2:
    case 4: {
        c_gui_widget *focused = get_focused_widget();
        if (focused) {
            c_gui_widget *list = gui_widget_get_parent_list_widget(focused);
            if (list) {
                c_gui_widget *next_list = 0;
                if (message->get_event_type() == 2)
                    next_list = gui_widget_get_previous_sibling_of_type(list, _gui_list_widget);
                else if (message->get_event_type() == 4)
                    next_list = gui_widget_get_next_sibling_of_type(list, _gui_list_widget);
                if (next_list) {
                    long element = ((c_gui_list_widget *)next_list)->get_focused_element_handle();
                    if (element != -1) {
                        c_gui_widget *item = gui_list_widget_find_item_by_element_handle(next_list, element);
                        if (item) {
                            gui_screen_widget_transfer_focus_without_animations(this, item, true, true);
                            handled = true;
                        }
                    }
                }
            }
        }
    } break;
    case _event_type_button_press:
        switch (message->get_component()) {
        case 2:
            player_emblem_toggle_alternate_emblem_foreground(message->get_controller());
            handled = true;
            break;
        case 1:
            close_current_subpane();
            handled = true;
            break;
        case _emblem_previous_color:
            handled = true;
            break;
        case _emblem_next_color:
            handled = true;
            break;
        }
        break;
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
// @address 0x4bc5b0 ?player_emblem_toggle_alternate_emblem_foreground@@YAXW4e_controller_index@@@Z
void player_emblem_toggle_alternate_emblem_foreground(e_controller_index controller_index) {
    c_controller_interface *controller = controller_get(controller_index);
    c_player_profile_interface *profile = get_player_profile_interface(controller);
    if (!get_async_io_busy(profile)) {
        s_emblem_info info = get_emblem_info(profile);
        bool alternate = emblem_foreground_is_alternate(&info);
        emblem_foreground_set_alternate(&info, !alternate);
        profile->set_emblem_info(&info, true);
    }
}
