// interfacelibxbox/gui_screen_matchmaking_advanced_options.obj: from the records 0x4cad88 0x4caef8 0x4caff8 0x4cb120 0x4cb200 0x4cb4c0 0x4cb5d8 0x4cb660 0x4cb6d8 0x4cb790 0x4cb858
// @flags /TP /O2 /J /Oc
#include "interface/gui_widget_types.h"
#include "cseries/language.h"
#include <stddef.h>
#include "memory/static_arrays.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/logic/network_life_cycle.h"
#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/hopper_statistics_types.h"
#include "interface/user_interface_allocation.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_widget.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/gui_screen_matchmaking_advanced_options.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/matchmaking_advanced_options_string_ids.h"
#include "game/player_types.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/matchmaking_advanced_options_parser.h"
#include "interface/user_interface_matchmaking_statistics.h"
#include "interface/hopper_information_types.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/online/online_url.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "interface/gui_custom_bitmap_widget.h"
#include <new>
#include "interface/c_gui_list_item_widget.h"
#include "interface/user_interface_messages.h"
#include "cseries/crt_xdk.h"


struct s_hopper_information;

enum { k_histogram_reveal_animation = 30 };

static bool parse_xml_hopper_name(void *context, wchar_t *buffer, long count);
static bool parse_xml_search_preference(void *context, wchar_t *buffer, long count);
static bool parse_xml_hopper_population(void *context, wchar_t *buffer, long count);

// @address 0x4cad88 ?initialize@c_gui_screen_matchmaking_advanced_options@@UAAXXZ
void c_gui_screen_matchmaking_advanced_options::initialize()
{
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<hopper-name", this, parse_xml_hopper_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<hopper-population", this, parse_xml_hopper_population));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<search-preference", this, parse_xml_search_preference));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<search-preference-description", this, parse_xml_search_preference_description));
    c_gui_screen_widget::initialize();
}

// @address 0x4cb660 ?parse_xml_hopper_name@@YA_NPAXPA_WJ@Z
static bool parse_xml_hopper_name(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    s_hopper_information info;
    if (::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &info))
    {
        wchar_t name[256];
        name[0] = 0;
        ((c_static_wchar_string<256> *)name)->set((const wchar_t *)info.name);
        unsigned long size = count;
        if (size > 256)
            size = 256;
        ustrnzcpy(buffer, name, size);
        success = true;
    }
    return success;
}

// @address 0x4cb790 ?parse_xml_search_preference@@YA_NPAXPA_WJ@Z
static bool parse_xml_search_preference(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    wchar_t text[1024];
    text[0] = 0;
    long string_id;
    switch (user_interface_matchmaking_squad_get_search_preference())
    {
    case 0: string_id = _matchmaking_advanced_options_string_search_preference_none; break;
    case 1: string_id = _matchmaking_advanced_options_string_search_preference_skill; break;
    case 2: string_id = _matchmaking_advanced_options_string_search_preference_connection; break;
    case 3: string_id = _matchmaking_advanced_options_string_search_preference_language; break;
    // match: 0x4cb790: hint omits the switch range check (cmplwi r3,3 + bgt) and the string_id stack spill (frame stays 0x880)
    default: __assume(0);
    }
    c_gui_screen_widget *source = (c_gui_screen_widget *)context;
    if (source->get_string_by_string_id(string_id, (c_static_wchar_string<1024> *)text))
    {
        unsigned long size = count;
        if (size > 1024)
            size = 1024;
        ustrnzcpy(buffer, text, size);
        success = true;
    }
    return success;
}

// @address 0x4cb858 ?parse_xml_search_preference_description@@YA_NPAXPA_WJ@Z
bool parse_xml_search_preference_description(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    wchar_t text[1024];
    text[0] = 0;
    long string_id;
    switch (user_interface_matchmaking_squad_get_search_preference())
    {
    case 0: string_id = _matchmaking_advanced_options_string_search_preference_none_description; break;
    case 1: string_id = _matchmaking_advanced_options_string_search_preference_skill_description; break;
    case 2: string_id = _matchmaking_advanced_options_string_search_preference_connection_description; break;
    case 3: string_id = _matchmaking_advanced_options_string_search_preference_language_description; break;
    // match: 0x4cb858: hint omits the switch range check (cmplwi r3,3 + bgt) and the string_id stack spill (frame stays 0x880)
    default: __assume(0);
    }
    c_gui_screen_widget *source = (c_gui_screen_widget *)context;
    if (source->get_string_by_string_id(string_id, (c_static_wchar_string<1024> *)text))
    {
        unsigned long size = count;
        if (size > 1024)
            size = 1024;
        ustrnzcpy(buffer, text, size);
        success = true;
    }
    return success;
}

// @address 0x4caef8 ?post_initialize@c_gui_screen_matchmaking_advanced_options@@UAAXXZ
void c_gui_screen_matchmaking_advanced_options::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _matchmaking_advanced_options_string_search_preference);
    c_gui_data *data = list != 0 ? list->get_data() : 0;
    update_description();
    if (list != 0 && data != 0) {
        for (long element_handle = data->get_first_element_handle(); element_handle != NONE; element_handle = data->get_next_element_handle(element_handle)) {
            long search_preference;
            if (data->get_integer_value(element_handle, 0x10a, &search_preference)) {
                if (::user_interface_matchmaking_squad_get_search_preference() == search_preference)
                    transfer_focus_to_list(list, element_handle, 0, 0);
            }
        }
    }
}

// @address 0x4a8e80 ?dispose@c_gui_screen_matchmaking_advanced_options@@UAAXXZ
void c_gui_screen_matchmaking_advanced_options::dispose()
{
    c_gui_screen_widget::dispose();
}

// @address 0x4caff8 ?update@c_gui_screen_matchmaking_advanced_options@@UAAXK@Z
void c_gui_screen_matchmaking_advanced_options::update(unsigned long current_milliseconds)
{
    bool enabled = ::network_life_cycle_squad_local_peer_is_leader();
    c_gui_widget *widget = get_child_widget(_gui_list_widget, _matchmaking_advanced_options_string_search_preference);
    if (widget)
        widget->set_enabled(enabled);
    update_description();
    c_gui_list_widget *list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _matchmaking_advanced_options_string_search_preference);
    c_gui_data *data = list ? list->get_data() : 0;
    long description = NONE;
    if (list && data && data->get_string_id_value(list->get_focused_element_handle(), 670, &description))
        set_text_widget_string_id(670, description);
    else
        set_text_widget_string_id(670, 0x103e4);
    update_histogram();
    ::user_interface_matchmaking_update_hopper_statistics_for_screen(&m_statistics_downloader, &m_histogram_hopper, &m_hopper_statistics);
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4cb120 ?update_description@c_gui_screen_matchmaking_advanced_options@@AAAXXZ
void c_gui_screen_matchmaking_advanced_options::update_description()
{
    c_gui_data **entry;
    int index = 0;
    s_hopper_information buf;
    if (0 < m_datasource_count)
    {
        entry = m_datasource;
        do
        {
            if ((*entry)->get_name() == _matchmaking_advanced_options_string_search_preference)
            {
                c_gui_data *data = m_datasource[index];
                if (!data)
                    return;
                unsigned short hopper = network_squad_session_get_hopper();
                data->clear_disabled_elements();
                if (::user_interface_matchmaking_get_hopper_information(hopper, &buf) && buf.ranked)
                    return;
                data->set_disabled_element(0x29c, _matchmaking_advanced_options_string_search_preference_skill);
                return;
            }
            index = index + 1;
            entry = entry + 1;
        } while (index < m_datasource_count);
    }
}

// ---- record 0x4cb4c0 ----

// @address 0x4cb4c0 ?handle_list_item_chosen@c_gui_screen_matchmaking_advanced_options@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_matchmaking_advanced_options::handle_list_item_chosen(
    const c_controller_input_message *event, long message, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool result = false;
    if (message == _matchmaking_advanced_options_string_search_preference) {
        if ((unsigned char)::network_life_cycle_squad_local_peer_is_leader() != 0) {
            if (this->get_child_widget(_gui_list_widget, _matchmaking_advanced_options_string_search_preference) != 0) {
                long selection = -1;
                if (data->get_integer_value(item->get_element_handle(), 266, &selection)) {
                    if ((unsigned int)selection <= 3)
                        user_interface_matchmaking_squad_set_search_preference(selection);
                }
                transition_out_with_transition_type(0, _screen_transition_normal);
            }
        } else {
            g_gui_error_manager.post_error_with_custom_message(131143, 0, 0, event->get_controller(), false, true);
        }
        result = true;
    }
    if (!result)
        result = c_gui_screen_widget::handle_list_item_chosen(event, message, item, data);
    return result;
}

// ---- record 0x4cb200 ----


// @address 0x4cb200 ?update_histogram@c_gui_screen_matchmaking_advanced_options@@AAAXXZ
void c_gui_screen_matchmaking_advanced_options::update_histogram()
{
    unsigned short hopper = network_squad_session_get_hopper();
    c_gui_custom_bitmap_widget *histogram = (c_gui_custom_bitmap_widget *)this->get_child_widget(
        _gui_bitmap_widget, _matchmaking_advanced_options_string_statistics);
    if (!histogram)
        return;
    if (m_histogram_hopper != 0xffff && m_histogram_hopper != hopper)
    {
        m_histogram_downloader.reset();
        m_histogram_hopper = hopper;
    }
    if (hopper != 0xffff)
    {
        const char *buffer = 0;
        long buffer_size = 0;
        s_network_http_buffer_url url;
        url.name[0] = 0;
        url.service_type = 0;
        url.cache_mode = 0;
        url.lifetime_seconds = 600;
        url.queue_type = 0;
        long team_index = 0;
        long player_index = user_interface_squad_get_player_at_local_controller(this->get_any_responding_controller());
        if (player_index != -1)
        {
            s_hopper_information hopper_information;
            if (::user_interface_matchmaking_get_hopper_information(hopper, &hopper_information) && hopper_information.ranked)
            {
                const s_player_configuration *player = user_interface_squad_get_player_data(player_index);
                if (player && player->host.team_override)
                    team_index = player->host.hopper_skill_display;
            }
        }
        unsigned short url_hopper = network_squad_session_get_hopper();
        online_url_set_base_path((c_online_url_string *)&url);
        long language = g_current_language;
        if (language == -1)
        {
            language = get_e_language_from_xbox_language(XTLGetLanguage());
            g_current_language = (e_language)language;
        }
        ((c_static_string<256> *)url.name)->append_print("%05u/%s/dynamic_matchmaking_histogram_%02d.jpg", url_hopper,
            get_language_suffix((e_language)language, false), team_index);
        url.cache_mode = 2;
        url.queue_type = 1;
        http_buffer_downloader_set_url(&m_histogram_downloader, &url);
        if (m_histogram_downloader.get_data(&buffer, &buffer_size) == 2 && buffer && buffer_size > 0)
        {
            if (!m_histogram_loaded)
            {
                m_histogram_loaded = true;
                gui_custom_bitmap_widget_load_bitmap_from_buffer(histogram, true, buffer, buffer_size, 0);
                ((c_gui_widget *)histogram)->start_animation_at_time((e_animation_state)k_histogram_reveal_animation, -1, true);
            }
        }
        else
        {
            m_histogram_loaded = false;
        }
        set_child_visible(_gui_group_widget, 0x100b3, m_histogram_downloader.get_download_status() == 1 || histogram->bitmap_index != -1);
        long status = m_histogram_downloader.get_download_status();
        set_child_visible(_gui_bitmap_widget, 0x1003a, status == 3 || status == 4);
    }
    else
    {
        set_child_visible(_gui_group_widget, 0x100b3, false);
        set_child_visible(_gui_bitmap_widget, 0x1003a, false);
    }
}

// ---- record 0x4cb6d8 ----

// @address 0x4cb6d8 ?parse_xml_hopper_population@@YA_NPAXPA_WJ@Z
static bool parse_xml_hopper_population(void *context, wchar_t *buffer, long count)
{
    long population;
    const s_matchmaking_single_hopper_statistics *statistics =
        ((c_gui_screen_matchmaking_advanced_options *)context)->get_hopper_statistics();
    if (statistics && statistics->hopper_population >= 0)
        population = statistics->hopper_population;
    else
        population = 0;

    c_static_wchar_string<256> string;
    string.print(L"%d", population);
    string.copy_to(buffer, count);
    return true;
}

// ---- record 0x4cb5d8 ----
// @address 0x4cb5d8 ?create_bitmap_widget@c_gui_screen_matchmaking_advanced_options@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_screen_matchmaking_advanced_options::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _matchmaking_advanced_options_string_statistics)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
        if (memory != 0)
            return new (memory) c_gui_custom_bitmap_widget();
    }
    else
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
        if (memory != 0)
            return new (memory) c_gui_bitmap_widget();
    }
    return 0;
}

typedef char advanced_options_hopper_number_offset[(offsetof(s_hopper_information, number) == 0x4c) ? 1 : -1];
typedef char advanced_options_hopper_size[(sizeof(s_hopper_information) == 0x54) ? 1 : -1];
