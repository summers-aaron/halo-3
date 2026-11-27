// interfacelibxbox/gui_screen_pregame_lobby_mapeditor.obj: from the records 0x4c6820 0x4c6830 0x4c6840 0x4c6850 0x4c68f0 0x4c69b0 0x4c69b8
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "interface/gui_screen_mapeditor_externs.h"
#include <stddef.h>
#include "interface/gui_data.h"
// @address 0x4c6820 ?get_lobby_header@c_gui_screen_pregame_lobby_mapeditor@@UAAJXZ
// @address 0x4c6830 ?get_lobby_title@c_gui_screen_pregame_lobby_mapeditor@@UAAJXZ
// @address 0x4c6840 ?get_start_button_name@c_gui_screen_pregame_lobby_mapeditor@@UAAJXZ
// @address 0x4c6850 ?initialize@c_gui_screen_pregame_lobby_mapeditor@@UAAXXZ
// @address 0x4c68f0 ?post_initialize@c_gui_screen_pregame_lobby_mapeditor@@UAAXXZ
// @address 0x4c69b8 ?handle_list_item_chosen@c_gui_screen_pregame_lobby_mapeditor@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z

#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/gui_pregame_setup_manager.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_allocation.h"
#include "cseries/placement_new.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"

#include "interface/user_interface_messages.h"
#include "interface/user_interface_session.h"
#include "interface/gui_selected_items.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_mapeditor.h"

// @address 0x114bfd8 ?g_pregame_setup_manager@Vc_gui_pregame_setup_manager@

// 0x4c6820
long c_gui_screen_pregame_lobby_mapeditor::get_lobby_header()
{
    return _pregame_lobby_mapeditor_string_header_mapeditor;
}

// 0x4c6830
long c_gui_screen_pregame_lobby_mapeditor::get_lobby_title()
{
    return _pregame_lobby_mapeditor_string_title_mapeditor;
}

// 0x4c6840
long c_gui_screen_pregame_lobby_mapeditor::get_start_button_name()
{
    return _pregame_lobby_mapeditor_string_start_mapeditor;
}

// 0x4c6850
void c_gui_screen_pregame_lobby_mapeditor::initialize()
{
    c_gui_screen_pregame_lobby::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-mapeditor-map", this, (bool (*)(void *, wchar_t *, long))user_interface_text_get_map_name));
    set_initial_focus(_pregame_lobby_string_lobby_list, 3);
}

// 0x4c68f0
void c_gui_screen_pregame_lobby_mapeditor::post_initialize()
{
    c_gui_screen_pregame_lobby::post_initialize();
    const unsigned char *state = global_preferences_get_game_setup_storage()->storage;
    if (!state[0xe539]) {
        return;
    }
    s_gui_game_setup_storage copy;
    memcpy(&copy, state, sizeof(copy));
    copy.storage[0xe539] = 0;
    // Copy the map-editor variant over the multiplayer variant slot and the 0x110 bytes after it.
    memcpy(&copy.multiplayer_settings.map_variant.variant, &copy.map_editor_settings.map_variant,
        sizeof(c_map_variant) + 0x110);
    global_preferences_set_last_game_setup(&copy);
    global_preferences_get_mode_setup(&g_pregame_setup_manager, 2);
    global_preferences_get_mode_setup(&g_pregame_setup_manager, 3);
}

// @address 0x4c69b0 ?update@c_gui_screen_pregame_lobby_mapeditor@@UAAXK@Z
void c_gui_screen_pregame_lobby_mapeditor::update(unsigned long milliseconds)
{
    c_gui_screen_pregame_lobby::update(milliseconds);
}

// @address 0x4c66f8 ?handle_controller_input_message@c_gui_screen_pregame_lobby_mapeditor@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_lobby_mapeditor::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool in_countdown = ::user_interface_squad_in_or_after_countdown();
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_button_x)
    {
        if (in_countdown)
            ::user_interface_squad_stop_countdown_timer(message->get_controller(), NONE, false);
        else
            load_game_options(message->get_controller());
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_controller_input_message(message);
    return handled;
}

// 0x4c69b8
bool c_gui_screen_pregame_lobby_mapeditor::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _pregame_lobby_string_lobby_list)
    {
        long selected = -1;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), _pregame_lobby_string_target, &selected))
        {
            e_gui_selected_item_type type = _gui_selected_item_none;
            switch (selected)
            {
            case _pregame_lobby_mapeditor_string_map:
                type = _gui_selected_item_map;
                break;
            }
            if (type != _gui_selected_item_none)
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_pregame_selection_screen_message), 0, 0);
                if (memory)
                {
                    long layer = m_name;
                    e_controller_index controller = message->get_controller();
                    e_window_index window = get_render_window();
                    c_load_pregame_selection_screen_message *load =
                        new (memory) c_load_pregame_selection_screen_message(controller, window, layer, type);
                    load->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(load);
                }
                handled = true;
            }
        }
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_list_item_chosen(message, list_name, list_item_widget, data);
    return handled;
}

