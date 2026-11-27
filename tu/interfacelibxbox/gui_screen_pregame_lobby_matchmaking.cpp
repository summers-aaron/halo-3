// interfacelibxbox/gui_screen_pregame_lobby_matchmaking.obj: from the records 0x4c56b8 0x4c56d8 0x4c56e8 0x4c56f8 0x4c5778 0x4c58b0 0x4c5a18 0x4c5b68 0x4c5d48 0x4c5dc8 0x4c5ea8 0x4c6080 0x4c60e8 0x4c6170 0x4c6280 0x4c6308 0x4c6390
// @flags /TP /O2 /Oc
#include "interface/gui_network_session_advertisement_types.h"
#include "networking/logic/storage/hopper_load_status_types.h"
#include "interface/session_game_mode_types.h"
#include "memory/tls_globals.h"
#include "math/real_math_types.h"
#include <stddef.h>
#include "shell/shell.h"
#include "interface/gui_data.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_game_mode_types.h"
#include "interface/gui_widget.h"
#include "interface/core_widget_definition.h"
#include "memory/static_arrays.h"
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/logic/network_life_cycle.h"
#include "saved_games/saved_film.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_matchmaking.h"
#include "networking/online/online_url.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_bitmap_widget.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/session/network_session_parameters.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/c_start_menu_item_gui_data.h"
#include "interface/user_interface_networking.h"
#include "interface/hopper_information_types.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "main/game_preferences_types.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "tag_files/string_ids.h"
struct s_hopper_information;
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x4f0a18 ?initialize@c_gui_screen_pregame_lobby@@UAAXXZ
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x495440 ?network_squad_session_get_hopper@@YAGXZ
// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x4c56b8 ?get_start_status_text@c_gui_screen_pregame_lobby_matchmaking@@UAAJ_N@Z
// @address 0x4c56d8 ?get_lobby_header@c_gui_screen_pregame_lobby_matchmaking@@UAAJXZ
// @address 0x4c56e8 ?get_lobby_title@c_gui_screen_pregame_lobby_matchmaking@@UAAJXZ
// @address 0x4c56f8 ?get_start_button_name@c_gui_screen_pregame_lobby_matchmaking@@UAAJXZ
// @address 0x4c5778 ?initialize@c_gui_screen_pregame_lobby_matchmaking@@UAAXXZ
// @address 0x4c5d48 ?handle_dialog_result@c_gui_screen_pregame_lobby_matchmaking@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4c6080 ?get_hopper_statistics@c_gui_screen_pregame_lobby_matchmaking@@QAAPBUs_matchmaking_single_hopper_statistics@@XZ
// @address 0x4c6170 ?parse_xml_lobby_matchmaking_hopper@@YA_NPAXPA_WJ@Z
// @address 0x4c6280 ?parse_xml_lobby_matchmaking_hopper_population@@YA_NPAXPA_WJ@Z
// @address 0x4c6308 ?parse_xml_lobby_matchmaking_total_population@@YA_NPAXPA_WJ@Z
// @address 0x4e6438 ??0c_gui_bitmap_widget@@QAA@XZ
// @address 0x491eb0 ??0c_gui_custom_bitmap_widget@@QAA@XZ
// @address 0x4c60e8 ?create_bitmap_widget@c_gui_screen_pregame_lobby_matchmaking@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
// @address 0x4c5a18 ?update_population@c_gui_screen_pregame_lobby_matchmaking@@AAAXXZ

#include <new>

#include "interface/user_interface_messages.h"

#include "interface/gui_selected_items.h"








#include "interface/gui_custom_bitmap_widget.h"

#include "interface/c_gui_list_item_widget.h"


#include "interface/c_gui_screen_widget.h"


#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"


#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"



#include "networking/session/network_session_class.h"



long c_gui_screen_pregame_lobby_matchmaking::get_start_status_text(bool is_leader)
{
    return is_leader ? 0x102c0 : 0x102c5;
}

long c_gui_screen_pregame_lobby_matchmaking::get_lobby_header()
{
    return 0x102af;
}

long c_gui_screen_pregame_lobby_matchmaking::get_lobby_title()
{
    return 0x102aa;
}

long c_gui_screen_pregame_lobby_matchmaking::get_start_button_name()
{
    return 0x102b8;
}

// @address 0x42d380 ?advanced_options_enabled@c_gui_screen_pregame_lobby_matchmaking@@MAA_NXZ
bool c_gui_screen_pregame_lobby_matchmaking::advanced_options_enabled()
{
    return true;
}

// @address 0x42d380 ?get_gui_game_mode@c_gui_screen_pregame_lobby_matchmaking@@UAAJXZ
long c_gui_screen_pregame_lobby_matchmaking::get_gui_game_mode()
{
    return _gui_game_mode_matchmaking;
}

// @address 0x5bec98 ?start_fade_during_countdown@c_gui_screen_pregame_lobby_matchmaking@@UAAXXZ
void c_gui_screen_pregame_lobby_matchmaking::start_fade_during_countdown()
{
}

// @address 0x5bec98 ?stop_fade_during_countdown@c_gui_screen_pregame_lobby_matchmaking@@UAAXXZ
void c_gui_screen_pregame_lobby_matchmaking::stop_fade_during_countdown()
{
}

c_gui_bitmap_widget *c_gui_screen_pregame_lobby_matchmaking::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _string_id_nightmap)
    {
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    }
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}

void c_gui_screen_pregame_lobby_matchmaking::initialize()
{
    c_gui_screen_pregame_lobby::initialize();
    set_initial_focus(_pregame_lobby_string_lobby_list, 3);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-matchmaking-hopper", this, parse_xml_lobby_matchmaking_hopper));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-matchmaking-hopper-population", this, parse_xml_lobby_matchmaking_hopper_population));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-matchmaking-total-population", this, parse_xml_lobby_matchmaking_total_population));
}

bool c_gui_screen_pregame_lobby_matchmaking::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    if (dialog_result_message->get_dialog_name() == 0x30020)
    {
        if (dialog_result_message->get_dialog_result() == 0)
            user_interface_networking_set_session_advertisement(saved_film_play());
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_dialog_result(dialog_result_message);
    return handled;
}

const s_matchmaking_single_hopper_statistics *
c_gui_screen_pregame_lobby_matchmaking::get_hopper_statistics(void)
{
    const s_matchmaking_single_hopper_statistics *result = 0;
    unsigned short hopper_id = this->hopper_id;
    if (hopper_id != 0xffff) {
        unsigned short current_hopper = network_squad_session_get_hopper();
        if (hopper_id == current_hopper)
            result = &m_hopper_statistics;
    }
    return result;
}

static __forceinline bool matchmaking_is_lobby_in_live_mode()
{
    long advertisement = user_interface_networking_get_session_advertisement();
    return (advertisement == 2) | (advertisement == 1) | (advertisement == 0);
}
bool parse_xml_lobby_matchmaking_hopper(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_static_wchar_string<1024> string;
    c_gui_screen_pregame_lobby_matchmaking *lobby = (c_gui_screen_pregame_lobby_matchmaking *)context;
    long status = ::user_interface_matchmaking_hopper_catalog_load_status();
    if (!matchmaking_is_lobby_in_live_mode() || status == 0)
        success = lobby->get_string_by_string_id(0x103dd, &string);
    else if (status == 1)
        success = lobby->get_string_by_string_id(0x103de, &string);
    else if (status == 3)
        success = lobby->get_string_by_string_id(0x103df, &string);
    else
    {
        s_hopper_information info;
        if (::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &info))
        {
            string.set((const wchar_t *)info.name);
            success = true;
        }
        else
            success = lobby->get_string_by_string_id(0x103de, &string);
    }
    if (success)
        string.copy_to(buffer, buffer_length);
    return success;
}

bool parse_xml_lobby_matchmaking_hopper_population(void *context, wchar_t *buffer, long buffer_length)
{
    const s_matchmaking_single_hopper_statistics *statistics =
        static_cast<c_gui_screen_pregame_lobby_matchmaking *>(context)->get_hopper_statistics();
    long population;
    if (statistics)
    {
        population = statistics->hopper_population;
        if (population < 0)
            population = 0;
    }
    else
    {
        population = 0;
    }

    c_static_wchar_string<256> population_string;
    population_string.print(L"%d", population);
    population_string.copy_to(buffer, buffer_length);
    return true;
}

bool parse_xml_lobby_matchmaking_total_population(void *context, wchar_t *buffer, long buffer_length)
{
    const s_matchmaking_single_hopper_statistics *statistics =
        static_cast<c_gui_screen_pregame_lobby_matchmaking *>(context)->get_hopper_statistics();
    long population;
    if (statistics)
    {
        population = statistics->total_population;
        if (population < 0)
            population = 0;
    }
    else
    {
        population = 0;
    }

    c_static_wchar_string<256> population_string;
    population_string.print(L"%d", population);
    population_string.copy_to(buffer, buffer_length);
    return true;
}

// @address 0x44674 ??_7?$c_http_stored_buffer_downloader@$0JKB@@@6B@
// @address 0x4c6390 ??0?$c_http_blf_simple_downloader@Us_dynamic_matchmaking_hopper_statistics@@@@QAA@XZ
template<> c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics>::c_http_blf_simple_downloader()
{
    m_data = 0;
    m_data_size = 0;
    m_last_retry_count = m_downloader.get_retry_count();
}


static __forceinline void gui_screen_pregame_lobby_matchmaking_set_child_visible(c_gui_widget *screen, e_gui_widget_type type, long name, int visible)
{
    c_gui_widget *child = screen->get_child_widget(type, name);
    if (child)
    {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x4c5a18: the int goes straight to r4 (`mr r4,rN`) at all three set_visible calls;
        // visible != 0 adds cntlzw/rlwinm/xori before each.
        union { void (c_gui_widget::*mem)(bool); void (*fn)(c_gui_widget *, int); } set_visible_alias;
        set_visible_alias.mem = &c_gui_widget::set_visible;
        set_visible_alias.fn(child, visible);
#else
        child->set_visible(visible != 0);
#endif
    }
}

template <typename t_blf_type>
class c_http_blf_simple_downloader;
struct s_dynamic_matchmaking_hopper_statistics;

// @address 0x4c58b0 ?update@c_gui_screen_pregame_lobby_matchmaking@@UAAXK@Z
void c_gui_screen_pregame_lobby_matchmaking::update(unsigned long current_milliseconds)
{
    c_gui_screen_pregame_lobby::update(current_milliseconds);
    if (!user_interface_squad_exists())
        return;
    if (user_interface_get_session_game_mode() != 2)
        return;
    c_gui_widget *visible_widget = this->get_child_widget(_gui_bitmap_widget, 0x1034a);
    c_gui_widget *count_widget = this->get_child_widget(_gui_bitmap_widget, 0x273);
    unsigned short current_hopper = network_squad_session_get_hopper();
    if (current_hopper != 0xffff) {
        s_hopper_information visibility;
        if (::user_interface_matchmaking_get_hopper_information(current_hopper, &visibility) != 0) {
            if (visible_widget)
                visible_widget->set_visible(visibility.ranked);
            if (count_widget)
                ((c_gui_bitmap_widget *)count_widget)->set_sprite_frame(visibility.number);
        }
    }
    if (matchmaking_is_lobby_in_live_mode()
        && user_interface_matchmaking_hopper_catalog_load_status() == 2
        && hopper_catalog_state != 2) {
        const s_game_preferences_mode_settings *preferences
            = game_preferences_get_mode_settings();
        s_game_preferences_snapshot snapshot;
        snapshot.type = 1;
        snapshot.color = preferences->matchmaking_settings;
        g_pregame_setup_manager.set_pending_setup(&snapshot);
        hopper_catalog_state = 2;
    }
    update_population();
    update_nightmap();
}
// @address 0x4c5a18 ?update_population@c_gui_screen_pregame_lobby_matchmaking@@AAAXXZ
void c_gui_screen_pregame_lobby_matchmaking::update_population()
{
    // match: 0x4c5a18: retain the accepted 92-byte result slot (canonical output is 84 bytes).
    struct s_population_hopper_result {
        s_hopper_information information;
        char padding[8];
    } hopper_information;
    int hopper_loaded = (unsigned char)(user_interface_matchmaking_hopper_catalog_load_status() == 2 &&
        ::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &hopper_information.information));
    int total_population_valid = 0;
    int hopper_population_valid = 0;
    const s_matchmaking_single_hopper_statistics *statistics = 0;

    user_interface_matchmaking_update_hopper_statistics_for_screen(
        &m_hopper_statistics_downloader,
        &hopper_id,
        &m_hopper_statistics);
    statistics = get_hopper_statistics();
    total_population_valid = (unsigned char)(statistics && statistics->total_population > 0);
    hopper_population_valid = (unsigned char)(statistics && statistics->hopper_population > 0);

    int no_hopper_visible = (unsigned char)hopper_loaded && !(unsigned char)hopper_population_valid;
    gui_screen_pregame_lobby_matchmaking_set_child_visible((c_gui_widget *)this, _gui_text_widget, 0x10345, no_hopper_visible);
    int hopper_visible = (unsigned char)hopper_loaded && (unsigned char)hopper_population_valid;
    gui_screen_pregame_lobby_matchmaking_set_child_visible((c_gui_widget *)this, _gui_text_widget, 0x10301, hopper_visible);
    gui_screen_pregame_lobby_matchmaking_set_child_visible((c_gui_widget *)this, _gui_text_widget, 0x10302, total_population_valid);
}


// @rdata 0x820452b4..0x820452d8
// @address 0x452b4 k_matchmaking_nightmap_url_path
extern "C" {
const char k_matchmaking_nightmap_url_path[36] = "dynamic_matchmaking_nightmap.jpg";
}
// @rdata-end

// @address 0x4c5b68 ?update_nightmap@c_gui_screen_pregame_lobby_matchmaking@@AAAXXZ
void c_gui_screen_pregame_lobby_matchmaking::update_nightmap()
{
    if (!network_session_interface_has_gamestate_bonus())
        return;

    const char *buffer = 0;
    long buffer_size = 0;
    bool changed = false;
    if (m_nightmap_version != network_session_interface_get_online_url_version())
    {
        changed = true;
        m_nightmap_version = network_session_interface_get_online_url_version();
    }
    if (changed)
    {
        s_network_http_buffer_url url;
        url.name[0] = 0;
        url.service_type = 0;
        url.cache_mode = 0;
        url.lifetime_seconds = 600;
        url.queue_type = 0;
        online_url_set_base_path((c_online_url_string *)&url);
        ((c_static_string<256> *)url.name)->append_print(k_matchmaking_nightmap_url_path);
        url.cache_mode = 2;
        url.queue_type = 1;
        http_buffer_downloader_set_url(&m_nightmap_downloader, &url);
    }
    if (m_nightmap_downloader.get_data(&buffer, &buffer_size) == 2 && buffer && buffer_size > 0)
    {
        if (!m_nightmap_loaded)
        {
            c_gui_widget *bitmap_widget = this->get_child_widget(_gui_bitmap_widget, _string_id_nightmap);
            m_nightmap_loaded = true;
            if (bitmap_widget)
            {
                s_runtime_core_widget_definition *definition = bitmap_widget->get_core_definition();
                if (!(definition->flags & (1 << 3)))
                {
                    long width = (long)(definition->authored_bounds.x1 - definition->authored_bounds.x0);
                    long height = (long)(definition->authored_bounds.y1 - definition->authored_bounds.y0);
                    if (width > 0 && height > 0)
                    {
                        gui_custom_bitmap_widget_load_bitmap_from_buffer((c_gui_custom_bitmap_widget *)bitmap_widget, true, buffer, buffer_size, 0);
                        c_gui_widget *child1 = this->get_child_widget(_gui_bitmap_widget, 0x105b9);
                        if (child1)
                            child1->set_visible(false);
                        c_gui_widget *child2 = this->get_child_widget(_gui_bitmap_widget, 0x105ba);
                        if (child2)
                            child2->set_visible(false);
                    }
                }
            }
        }
    }
}

// @address 0x4c5dc8 ?handle_controller_input_message@c_gui_screen_pregame_lobby_matchmaking@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_lobby_matchmaking::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool in_countdown = ::user_interface_squad_in_or_after_countdown();
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_button_x)
    {
        if (in_countdown)
        {
            ::user_interface_squad_stop_countdown_timer(message->get_controller(), -1, false);
        }
        else
        {
            void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
            if (memory)
            {
                long layer = m_name;
                e_controller_index controller = message->get_controller();
                c_load_screen_message *load_screen_message = new (memory) c_load_screen_message(0x100c1, controller, get_render_window(), layer);
                if (load_screen_message)
                {
                    load_screen_message->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(load_screen_message);
                }
            }
        }
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_controller_input_message(message);
    return handled;
}

// 0x4c5ea8
bool c_gui_screen_pregame_lobby_matchmaking::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _pregame_lobby_string_lobby_list)
    {
        long selected = -1;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), 0x10011, &selected))
        {
            e_gui_selected_item_type type = _gui_selected_item_none;
            if (selected == 0x273)
            {
                if (matchmaking_is_lobby_in_live_mode())
                {
                    if (user_interface_matchmaking_hopper_catalog_load_status() == 2)
                        type = _gui_selected_item_hopper;
                }
                else
                {
                    void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                    if (memory)
                    {
                        long layer = m_name;
                        e_controller_index controller = message->get_controller();
                        c_load_dialog_screen_message *dialog = new (memory)
                            c_load_dialog_screen_message(controller, get_render_window(), layer, 0x30020, layer);
                        if (dialog)
                            ::user_interface_messaging_post(dialog);
                    }
                }
            }
            if (type != _gui_selected_item_none)
            {
                c_load_pregame_selection_screen_message *load = (c_load_pregame_selection_screen_message *)
                    user_interface_malloc_tracked(sizeof(c_load_pregame_selection_screen_message), 0, 0);
                if (load)
                {
                    long layer = m_name;
                    e_controller_index controller = message->get_controller();
                    e_window_index window = get_render_window();
                    new (load) c_load_pregame_selection_screen_message(controller, window, layer, type);
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


