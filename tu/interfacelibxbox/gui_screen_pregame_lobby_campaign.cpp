// interfacelibxbox/gui_screen_pregame_lobby_campaign.obj: from the records 0x4c4b58 0x4c4b78 0x4c4b88 0x4c4c10 0x4c4d48 0x4c5068 0x4c50e8 0x4c54e0 0x4c5588 0x4c5618
// @flags /TP /O2 /Oc
#include "game/game_types.h"
#include "interface/session_game_mode_types.h"
#include <stddef.h>
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_game_mode_types.h"
#include "memory/static_arrays.h"
#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "main/game_preferences.h"
#include "networking/logic/network_banhammer.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_campaign.h"
#include "interface/c_controller.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/user_interface_allocation.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_life_cycle.h"
#include <new.h>
#include "interface/user_interface_messages.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
#include "interface/gui_screens/campaign/campaign_select_level_message.h"
#include "main/levels_globals.h"
#include "main/levels.h"
#include "interface/gui_selected_items_difficulty.h"
#include "interface/game_tag_parser.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/gui_screen_pregame_campaign_dependencies.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "interface/gui_pregame_setup_manager.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_item_storage.h"
#include "interface/user_interface_clock.h"

class c_message;
struct s_data_array;

// @address 0x4c4b58 ?get_start_status_text@c_gui_screen_pregame_lobby_campaign@@UAAJ_N@Z
// @address 0x4c4b78 ?get_lobby_header@c_gui_screen_pregame_lobby_campaign@@UAAJXZ
// @address 0x4c4b88 ?get_lobby_title@c_gui_screen_pregame_lobby_campaign@@UAAJXZ
// @address 0x4c4c10 ?initialize@c_gui_screen_pregame_lobby_campaign@@UAAXXZ
// @address 0x4c54e0 parse_xml_lobby_campaign_map_name
// @address 0x4c5588 parse_xml_lobby_campaign_difficulty
// @address 0x4c5618 parse_xml_lobby_campaign_insertion_point
// @address 0x4c50e8 ?handle_list_item_chosen@c_gui_screen_pregame_lobby_campaign@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z

long c_gui_screen_pregame_lobby_campaign::get_start_status_text(bool is_leader)
{
    return is_leader ? _pregame_lobby_campaign_string_options_leader_campaign : _pregame_lobby_campaign_string_options_member_campaign;
}

long c_gui_screen_pregame_lobby_campaign::get_lobby_header()
{
    return _pregame_lobby_campaign_string_header_campaign;
}

long c_gui_screen_pregame_lobby_campaign::get_lobby_title()
{
    return _pregame_lobby_campaign_string_title_campaign;
}

// @address 0x42d298 ?get_gui_game_mode@c_gui_screen_pregame_lobby_campaign@@UAAJXZ
long c_gui_screen_pregame_lobby_campaign::get_gui_game_mode()
{
    return _gui_game_mode_campaign;
}

// @address 0x53c54 ??_7c_load_campaign_select_level_screen_message@@6B@
// @address 0x4c4bb8 ??_Gc_load_campaign_select_level_screen_message@@UAAPAXI@Z
// @address 0x4c4bb8 ??_Ec_load_campaign_select_level_screen_message@@UAAPAXI@Z

void c_gui_screen_pregame_lobby_campaign::initialize()
{
    c_gui_screen_pregame_lobby::initialize();
    set_initial_focus(_pregame_lobby_campaign_string_lobby_list, 4);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-campaign-level", this, parse_xml_lobby_campaign_map_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-campaign-difficulty", this, parse_xml_lobby_campaign_difficulty));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-campaign-insertion", this, parse_xml_lobby_campaign_insertion_point));
}

extern "C" {
bool parse_xml_lobby_campaign_map_name(void *context, wchar_t *output, long capacity)
{
    c_static_wchar_string<1024> string;
    e_campaign_id campaign_id;
    e_map_id map_id;
    levels_map_datum level;

    if (network_life_cycle_session_get_map(&campaign_id, &map_id) &&
        levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&level))
    {
        string.set(level.display_name);
    }
    string.copy_to(output, capacity);
    return true;
}

bool parse_xml_lobby_campaign_difficulty(void *context, wchar_t *output, long capacity)
{
    c_static_wchar_string<1024> string;
    c_static_wchar_string<1024> title;
    c_static_wchar_string<1024> description;

    if (c_gui_difficulty_selected_item::find_title_and_description(
            user_interface_game_settings_get_campaign_difficulty(),
            &title,
            &description))
    {
        string.set(title.m_string);
    }
    string.copy_to(output, capacity);
    return true;
}

bool parse_xml_lobby_campaign_insertion_point(void *context, wchar_t *output, long capacity)
{
    e_map_id map_id;
    e_campaign_id campaign_id;
    levels_insertion_datum list;
    bool result = false;

    if (network_life_cycle_session_get_map(&campaign_id, &map_id))
    {
        short index = (short)network_banhammer_find_cheaters_in_current_session();
        if (levels_try_and_get_insertion(map_id, &list) && index >= 0 && index < list.insertion_count)
        {
            ustrnzcpy(output, list.names[index], capacity);
            result = true;
        }
    }
    return result;
}
}

bool c_gui_screen_pregame_lobby_campaign::handle_list_item_chosen(
    const c_controller_input_message *message, long list, c_gui_list_item_widget *chosen, c_gui_data *data)
{
    bool handled = false;
    if (list == _pregame_lobby_campaign_string_lobby_list)
    {
        long action = -1;
        if (data->get_string_id_value(chosen->get_element_handle(), _pregame_lobby_campaign_string_target, &action))
        {
            switch (action)
            {
            case _pregame_lobby_campaign_string_level:
            {
                e_campaign_id campaign_id = _campaign_id_pre_game_fallback;
                e_map_id map_id = _map_id_pre_game_fallback;
                network_life_cycle_session_get_map(&campaign_id, &map_id);
                c_load_campaign_select_level_screen_message *load = (c_load_campaign_select_level_screen_message *)user_interface_malloc_tracked(
                    sizeof(c_load_campaign_select_level_screen_message), 0, 0);
                if (load)
                {
                    long name = m_name;
                    e_controller_index controller = message->m_controller;
                    e_window_index window = get_render_window();
                    new (load) c_load_campaign_select_level_screen_message(controller, window, name, 1, campaign_id, map_id);
                    load->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(load);
                }
                handled = true;
                break;
            }
            case _pregame_lobby_campaign_string_difficulty:
            {
                c_load_campaign_select_difficulty_screen_message *load = (c_load_campaign_select_difficulty_screen_message *)user_interface_malloc_tracked(
                    sizeof(c_load_campaign_select_difficulty_screen_message), 0, 0);
                if (load)
                {
                    long name = m_name;
                    e_controller_index controller = message->m_controller;
                    e_campaign_difficulty_level difficulty = user_interface_game_settings_get_campaign_difficulty();
                    e_window_index window = get_render_window();
                    new (load) c_load_campaign_select_difficulty_screen_message(
                        controller, window, name, 2, _campaign_id_pre_game_fallback, _map_id_none, difficulty);
                    load->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(load);
                }
                handled = true;
                break;
            }
            }
        }
    }
    if (!handled)
    {
        handled = c_gui_screen_pregame_lobby::handle_list_item_chosen(message, list, chosen, data);
    }
    return handled;
}


static __forceinline bool gui_screen_pregame_lobby_campaign_controller_has_saved_game_state(e_controller_index controller)
{
    bool result = false;
    if (saved_game_files_get_personal_storage_content_item(controller, false) != -1)
        result = true;
    return result;
}

// @address 0x4c5068 ?get_start_button_name@c_gui_screen_pregame_lobby_campaign@@UAAJXZ
long c_gui_screen_pregame_lobby_campaign::get_start_button_name()
{
    bool resume = false;
    e_controller_index controller = user_interface_networking_get_saved_game_controller();
#ifdef HALO3_64BIT
    if ((unsigned long)controller <= 3 && m_player_count >= 1 && m_player_count <= 4 &&
        user_interface_squad_get_local_player_count() == m_player_count)
        resume = true;
#else
    if ((unsigned long)controller <= 3 && m_player_count == 1)
        resume = gui_screen_pregame_lobby_campaign_controller_has_saved_game_state(controller);
#endif
    return resume ? _pregame_lobby_campaign_string_start_campaign_resume : _pregame_lobby_campaign_string_start_campaign_new;
}

static __forceinline unsigned long gui_screen_pregame_lobby_campaign_milliseconds()
{
    unsigned long result = g_user_interface_globals.milliseconds.peek();
    return result;
}

static __forceinline bool gui_screen_pregame_lobby_campaign_content_ready(const c_content_catalogue *catalogue)
{
    return ((catalogue->m_flags.value >> 1) & 1) != 0;
}
// match: 0x4c4d48: without it the intrinsic memcpy lowers late, reordering the r3/r5 call setup and swapping r27/r28 colors across update
#pragma function(memcpy)
// @address 0x4c4d48 ?update@c_gui_screen_pregame_lobby_campaign@@UAAXK@Z
void c_gui_screen_pregame_lobby_campaign::update(unsigned long milliseconds)
{
    e_controller_index controller = _controller_index_none;
    c_gui_screen_pregame_lobby::update(milliseconds);
    if (user_interface_squad_exists() && user_interface_get_session_game_mode() == _session_game_mode_pregame)
    {
        c_gui_bitmap_widget *difficulty =
            static_cast<c_gui_bitmap_widget *>(get_child_widget(_gui_bitmap_widget, _pregame_lobby_campaign_string_difficulty_image));
        if (difficulty)
            difficulty->set_sprite_frame(user_interface_game_settings_get_campaign_difficulty());
        long player_count = user_interface_squad_get_player_count();
        s_gui_screen_pregame_lobby_campaign_players players = {};
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if (user_interface_squad_is_player_valid(player_index))
            {
                const s_player_identifier *identifier = user_interface_squad_get_player_identifier(player_index);
                players.identifiers[player_index] = *identifier;
            }
        }
#ifdef HALO3_64BIT
        if (m_player_count != -1 &&
            (player_count != m_player_count || memcmp(&players, &m_players, sizeof(players)) != 0))
        {
            if (player_count >= 1 && player_count <= 4 &&
                user_interface_squad_get_local_player_count() == player_count)
            {
                global_preferences_get_mode_setup(&g_pregame_setup_manager, 0);
                controller = user_interface_networking_get_saved_game_controller();
                if (controller == _controller_index_none) {
                    __lwsync();
                    _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
                }
            } else {
                __lwsync();
                _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
            }
        }
#else
        if (m_player_count != -1 && player_count == 1 && memcmp(&players, &m_players, sizeof(players)) != 0)
        {
            global_preferences_get_mode_setup(&g_pregame_setup_manager, 0);
            controller = controller_get_first_signed_in_controller();
        }
#endif
        m_player_count = player_count;
        memcpy(&m_players, &players, sizeof(m_players));
    }
    bool loading = false;
    bool was_loading = m_loading_screen_time != 0;
    if ((unsigned long)controller <= 3)
        m_saved_game_controller = controller;
    long saved_controller = m_saved_game_controller;
    if (m_saved_game_controller >= 0 && saved_controller < 4)
    {
        const c_content_catalogue *catalogue = &g_content_catalogue_globals.catalogues[saved_controller];
        if (content_catalogue_has_storage_device(catalogue))
        {
            if (gui_screen_pregame_lobby_campaign_content_ready(catalogue))
            {
                user_interface_initialize_coop_game_settings(this, (e_controller_index)saved_controller);
                m_saved_game_controller = _controller_index_none;
            }
            else
                loading = true;
        }
    }
    if (loading)
    {
        if (!was_loading)
        {
            e_controller_index loading_controller = m_saved_game_controller;
            void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
            if (memory)
            {
                c_load_in_progress_screen_message *message = new (memory) c_load_in_progress_screen_message(
                    loading_controller, get_render_window(), _pregame_lobby_campaign_string_pregame_selection_enumeration_in_progress_title,
                    _pregame_lobby_campaign_string_pregame_selection_enumeration_in_progress_message);
                if (message)
                {
                    message->m_parent_screen_index = m_screen_index;
                    ::user_interface_messaging_post((c_message const *)message);
                    m_loading_screen_time = gui_screen_pregame_lobby_campaign_milliseconds();
                }
            }
            __lwsync();
            _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
        }
    }
    else if (was_loading)
    {
        c_gui_screen_widget *screen = static_cast<c_gui_screen_widget *>(get_child_widget(_gui_screen_widget, _pregame_lobby_campaign_string_in_progress));
        if (screen && !screen->transitioning_out() && gui_screen_pregame_lobby_campaign_milliseconds() - m_loading_screen_time >= 3000)
        {
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
            m_loading_screen_time = 0;
        }
    }
}

