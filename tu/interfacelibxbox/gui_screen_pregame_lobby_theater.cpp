// interfacelibxbox/gui_screen_pregame_lobby_theater.obj: from the records 0x4c6ba8 0x4c6bc8 0x4c6bd8 0x4c6be8 0x4c6bf8 0x4c6d78 0x4c6e58 0x4c7170 0x4c72b0 0x4c7338 0x4c7440
// @flags /TP /O2 /Oc
#include "interface/gui_selected_items_saved_film.h"
#include "shell/shell.h"
#include "interface/user_interface_utilities.h"
#include "interface/user_interface_time.h"
#include "interface/session_game_mode_types.h"
#include <stddef.h>
#include "game/game_types.h"
#include "game/game_variant.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/c_deletable.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "interface/gui_pregame_setup_manager.h"
#include "game/map_variant.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_theater.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "saved_games/saved_film_description.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include <new>
#include "interface/game_tag_parser.h"
#include "interface/gui_widget.h"

// @address 0x4c6bf8 ?initialize@c_gui_screen_pregame_lobby_theater@@UAAXXZ
// @address 0x4c7170 ?handle_list_item_chosen@c_gui_screen_pregame_lobby_theater@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x4c72b0 gui_screen_pregame_lobby_theater_get_film_name
// @address 0x4c7338 gui_screen_pregame_lobby_theater_get_map_name
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x4f2520 ?handle_list_item_chosen@c_gui_screen_pregame_lobby@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x4f0a18 ?initialize@c_gui_screen_pregame_lobby@@UAAXXZ
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x3b5178 ?user_interface_game_settings_get_game_variant@@YAPBVc_game_variant@@XZ
// @address 0x484760 user_interface_get_timespan_string
// @address 0x54084 k_theater_film_name_tag
// @address 0x540ac k_theater_map_name_tag
// @address 0x540d4 k_theater_film_map_title_tag
// @address 0x540fc k_theater_film_duration_tag
// @rdata 0x82054084..0x82054138
extern "C" {
const wchar_t k_theater_film_name_tag[] = {
    '<', 'l', 'o', 'b', 'b', 'y', '-', 't', 'h', 'e', 'a', 't', 'e', 'r', '-', 'f', 'i', 'l', 'm', 0
};
const wchar_t k_theater_map_name_tag[] = {
    '<', 'l', 'o', 'b', 'b', 'y', '-', 't', 'h', 'e', 'a', 't', 'e', 'r', '-', 'g', 'a', 'm', 'e', 0
};
const wchar_t k_theater_film_map_title_tag[] = {
    '<', 'l', 'o', 'b', 'b', 'y', '-', 't', 'h', 'e', 'a', 't', 'e', 'r', '-', 'm', 'a', 'p', 0, 0
};
const wchar_t k_theater_film_duration_tag[] = {
    '<', 'l', 'o', 'b', 'b', 'y', '-', 't', 'h', 'e', 'a', 't', 'e', 'r', '-', 'f', 'i', 'l', 'm', '-', 'l', 'e', 'n', 'g', 't', 'h', 0, 0, 0, 0
};
}
// @rdata-end

// @address 0x4c6ba8 ?get_start_status_text@c_gui_screen_pregame_lobby_theater@@UAAJ_N@Z
long c_gui_screen_pregame_lobby_theater::get_start_status_text(bool is_leader)
{
    if (is_leader)
        return _pregame_lobby_theater_string_options_leader_theater;
    return _pregame_lobby_theater_string_options_member_theater;
}

// @address 0x4c6bc8 ?get_lobby_header@c_gui_screen_pregame_lobby_theater@@UAAJXZ
long c_gui_screen_pregame_lobby_theater::get_lobby_header()
{
    return _pregame_lobby_theater_string_header_theater;
}

// @address 0x4c6bd8 ?get_lobby_title@c_gui_screen_pregame_lobby_theater@@UAAJXZ
long c_gui_screen_pregame_lobby_theater::get_lobby_title()
{
    return _pregame_lobby_theater_string_title_theater;
}

// @address 0x4c6be8 ?get_start_button_name@c_gui_screen_pregame_lobby_theater@@UAAJXZ
long c_gui_screen_pregame_lobby_theater::get_start_button_name()
{
    return _pregame_lobby_theater_string_start_theater;
}

void c_gui_screen_pregame_lobby_theater::initialize()
{
    c_gui_screen_pregame_lobby::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_theater_film_name_tag, this,
        gui_screen_pregame_lobby_theater_get_film_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_theater_map_name_tag, this,
        gui_screen_pregame_lobby_theater_get_map_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_theater_film_map_title_tag, this,
        gui_pregame_setup_manager_get_film_map_title));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_theater_film_duration_tag, this,
        gui_screen_pregame_lobby_theater_get_film_duration));
    set_initial_focus(_pregame_lobby_theater_string_lobby_list, 3);
}

bool c_gui_screen_pregame_lobby_theater::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _pregame_lobby_theater_string_lobby_list)
    {
        long selected = -1;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), _pregame_lobby_theater_string_target, &selected))
        {
            e_gui_selected_item_type type = _gui_selected_item_none;
            switch (selected)
            {
            case _pregame_lobby_theater_string_film:
                type = _gui_selected_item_film;
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

bool gui_screen_pregame_lobby_theater_get_film_name(void *context, wchar_t *output, long capacity)
{
    (void)context;
    c_static_wchar_string<1024> string;
    const s_saved_film_description *film = user_interface_game_settings_get_film();
    if (film && film->film_type != 0 && film->film_type != 6)
    {
        string.set(film->name);
    }
    string.copy_to(output, capacity);
    return true;
}

bool gui_screen_pregame_lobby_theater_get_map_name(void *context, wchar_t *output, long capacity)
{
    (void)context;
    bool result = false;
    const s_saved_film_description *film = user_interface_game_settings_get_film();
    if (film)
    {
        if (film->film_type != 0 && film->film_type != 6)
        {
            c_static_wchar_string<1024> string;
            const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
            if (game_variant)
            {
                string.set(game_variant->get_name());
            }
            string.copy_to(output, capacity);
            result = true;
        }
    }
    return result;
}

// @address 0x4c7440 gui_screen_pregame_lobby_theater_get_film_duration
bool gui_screen_pregame_lobby_theater_get_film_duration(void *context, wchar_t *output, long capacity)
{
    (void)context;
    bool result = false;
    const s_saved_film_description *film = user_interface_game_settings_get_film();
    if (film)
    {
        if (film->film_type != 0 && film->film_type != 6)
        {
            c_static_wchar_string<1024> string;
            if (user_interface_get_timespan_string(film->length_seconds, 0, false, &string))
            {
                long copy_size = capacity;
                if ((unsigned long)copy_size > 0x400)
                    copy_size = 0x400;
                ustrnzcpy(output, string.m_string, copy_size);
                result = true;
            }
        }
    }
    return result;
}

static __forceinline void gui_screen_pregame_lobby_theater_delete_data(c_gui_data *data)
{
    data->~c_gui_data();
    user_interface_free(data);
}

// @address 0x4c6d78 ?initialize_datasource@c_gui_screen_pregame_lobby_theater@@UAAXXZ
void c_gui_screen_pregame_lobby_theater::initialize_datasource()
{
    c_gui_screen_pregame_lobby::initialize_datasource();
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_saved_film_subitem_datasource), 0, 0);
    c_gui_data *data = memory != 0
        ? static_cast<c_gui_data *>(static_cast<void *>(new (memory)
              c_gui_saved_film_subitem_datasource(get_any_responding_controller(), 1)))
        : 0;
    if (data != 0)
    {
        if (data->initialize(_pregame_lobby_theater_string_recent_films))
            add_datasource(data);
        else
            gui_screen_pregame_lobby_theater_delete_data(data);
    }
}

static __forceinline void gui_screen_pregame_lobby_theater_set_widget_visible(
    c_gui_screen_pregame_lobby_theater *screen, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *widget = screen->get_child_widget(type, name);
    if (widget != 0)
        widget->set_visible(visible);
}

// @address 0x4c6e58 ?update@c_gui_screen_pregame_lobby_theater@@UAAXK@Z
void c_gui_screen_pregame_lobby_theater::update(unsigned long milliseconds)
{
    c_gui_data *film_data = 0;
    for (long i = 0; i < m_datasource_count; i++)
    {
        if (m_datasource[i]->m_name == _pregame_lobby_theater_string_recent_films)
        {
            film_data = m_datasource[i];
            break;
        }
    }

    c_gui_screen_pregame_lobby::update(milliseconds);

    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_bitmap_widget, _pregame_lobby_theater_string_gametype_image, false);
    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_bitmap_widget, _pregame_lobby_theater_string_difficulty_image, false);
    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_bitmap_widget, _pregame_lobby_theater_string_map_image, false);
    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_text_widget, _pregame_lobby_theater_string_game_name, false);
    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_text_widget, _pregame_lobby_theater_string_theater_no_film, false);
    gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_text_widget, _pregame_lobby_theater_string_theater_invalid_film, false);

    if (user_interface_squad_exists() && user_interface_get_session_game_mode() == _session_game_mode_pregame)
    {
        const s_saved_film_description *film = user_interface_game_settings_get_film();
        if (film != 0)
        {
            long text_name;
            if (film->film_type == 0)
            {
                text_name = _pregame_lobby_theater_string_theater_no_film;
            }
            else if (film->film_type == 6)
            {
                text_name = _pregame_lobby_theater_string_theater_invalid_film;
            }
            else
            {
                if (film->campaign_id == -1)
                {
                    c_gui_widget *sprite = get_child_widget(_gui_bitmap_widget, _pregame_lobby_theater_string_gametype_image);
                    const c_game_variant *variant = gui_pregame_lobby_get_current_variant();
                    if (sprite != 0 && variant != 0)
                    {
                        sprite->set_visible(true);
                        static_cast<c_gui_bitmap_widget *>(sprite)->set_sprite_frame(variant->m_game_engine_index);
                    }
                }
                else
                {
                    c_gui_widget *sprite = get_child_widget(_gui_bitmap_widget, _pregame_lobby_theater_string_difficulty_image);
                    if (sprite != 0)
                    {
                        sprite->set_visible(true);
                        static_cast<c_gui_bitmap_widget *>(sprite)->set_sprite_frame(
                            user_interface_game_settings_get_campaign_difficulty());
                    }
                }
                gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_bitmap_widget, _pregame_lobby_theater_string_map_image, true);
                text_name = _pregame_lobby_theater_string_game_name;
            }
            gui_screen_pregame_lobby_theater_set_widget_visible(this, _gui_text_widget, text_name, true);
        }
    }

    if (!m_initial_film_selected)
    {
        const s_saved_film_description *film = user_interface_game_settings_get_film();
        if (film == 0 || film->film_type == 0)
        {
            if (film_data == 0 || film_data->is_busy())
                return;

            long item_index = film_data->get_first_element_handle();
            if (item_index != -1)
            {
                const c_gui_selected_item *item = film_data->get_gui_selected_item(item_index);
                if (item != 0)
                    g_pregame_setup_manager.set_selected_item(get_any_responding_controller(), item, true, -1);
            }
        }
        m_initial_film_selected = true;
    }
}
