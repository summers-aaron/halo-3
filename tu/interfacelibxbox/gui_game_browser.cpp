// interfacelibxbox/gui_game_browser.obj: from the records 0x4d0ed0 0x4d0ed8 0x4d1068 0x4d11b0 0x4d12e8 0x4d1418 0x4d1548 0x4d1670 0x4d1900 0x4d1960 0x4d1ad0 0x4d1cb8 0x4d2290 0x4d2330 0x4d2348 0x4d2470 0x4d24e8 0x4d25c0 0x4d2738 0x4d2900 0x4d2a58 0x4d2c18 0x4d2c98
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/gui_widget_types.h"
#include "networking/session/network_session_class.h"
#include "interface/user_interface_sound_effect_types.h"
#include <stdlib.h>
#include <stddef.h>
#include "memory/static_arrays.h"
#include "tag_files/string_ids.h"
#include "main/levels_map_datum_types.h"
#include "main/levels_globals.h"
#include "interface/user_interface_allocation.h"
#include "cseries/placement_new.h"
#include "interface/gui_screens/game_details/load_game_details_screen_message.h"
// @address 0x4d2c98 ?parse_game_browser_title@@YA_NPAXPA_WJ@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x54af4 ??_7c_load_game_browser_screen_message@@6B@
// @address 0x4c4bb8 ??_Gc_load_game_browser_screen_message@@UAAPAXI@Z
// @address 0x4c4bb8 ??_Ec_load_game_browser_screen_message@@UAAPAXI@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_memory_allocation.h"
// @address 0x7c1b88 g_user_interface_allocation
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "interface/user_interface_network_search.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_utilities.h"
#include "networking/logic/network_search.h"
#include "networking/transport/transport_security.h"
#include "networking/logic/network_search_types.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_messages.h"
#include "game/player_types.h"
#include "interface/c_gui_widget.h"

#include "interface/gui_game_browser_externs.h"
// @address 0x58f740 _wcsnicmp
#include "interface/user_interface_shared_globals.h"
struct s_user_interface_shared_globals;
class c_message;
enum e_user_interface_sound_effect;
class c_gui_list_widget;
template<long k_maximum_count> class c_static_wchar_string;

#include "interface/gui_data.h"
struct s_game_list_data {
    long squad_index;
};
class c_games_datasource : public c_gui_ordered_data {
public:
    __forceinline c_games_datasource() : m_browser_game_count(0), m_game_count(0) {}
    virtual void update();
    virtual bool get_integer_value(long index, long column, long *value);
    virtual bool get_string_id_value(long index, long column, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_player_appearance(long element_handle, s_player_appearance *appearance);
protected:
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual long get_current_item_count_internal();
    static int connectivity_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    static int player_count_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    static int name_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    static int mode_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    static int game_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    static int map_sort_proc(const s_game_list_data *a, const s_game_list_data *b);
    s_game_list_data m_game_indices[25];
    long m_browser_game_count;
    long m_game_count;
};


class c_font_cache;
class c_gui_text_widget;
class c_gui_bitmap_widget;
class c_gui_group_widget;
class c_gui_button_key_widget;
class c_gui_list_widget;
class c_gui_list_item_widget;
class c_gui_model_widget;
class c_controller_input_message;
struct s_runtime_core_widget_definition;
struct s_runtime_text_widget_definition;
struct s_runtime_bitmap_widget_definition;
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_list_widget_block;
struct s_list_item_widget_block;
struct s_model_widget_block;
struct s_animation_transform;
union real_rectangle2d;
union real_argb_color;
union rectangle2d;
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_list_item_widget.h"
class c_game_tag_parser;
#include "interface/gui_screens/game_browser/gui_game_browser.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
class c_deletable;
struct s_available_session;
// @address 0x50278 ??_7c_load_game_details_screen_message@@6B@

#include "networking/logic/network_squad_join_details.h"
struct s_game_browser_join_status
{
    short protocol;
    short flags;
    long type;
    long version;
    long build;
    long state;
    long header_extension;
    s_network_squad_join_details details;
};

#include "networking/logic/network_search_globals.h"





enum e_game_browser_column
{
    _game_browser_column_compatible = 252,
    _game_browser_column_slot_count_a = 0x10335,
    _game_browser_column_slot_count_b = 0x10336,
    _game_browser_column_slot_count_c = 0x10337,
    _game_browser_column_slot_count_d = 0x10338,
    _game_browser_column_player_count = 0x10339,
    _game_browser_column_available_slots = 0x1033a,
    _game_browser_column_connectivity = 0x1033b,
    _game_browser_column_mode = 0x10072,
    _game_browser_column_name = 0x29c,
    _game_browser_column_map = 0x270,
    _game_browser_column_game = 0x10342,
    _game_browser_column_player_count_text = 0x10343,
    _game_browser_column_join_mode = 0x10344,
};

enum e_browser_list_sort_column
{
    _browser_list_sort_column_name = 0,
    _browser_list_sort_column_mode,
    _browser_list_sort_column_game,
    _browser_list_sort_column_map,
    _browser_list_sort_column_player_count,
    _browser_list_sort_column_connectivity
};


// @address 0x4d0ed0 ?get_current_item_count_internal@c_games_datasource@@MAAJXZ
long c_games_datasource::get_current_item_count_internal()
{
    return m_game_count;
}

void c_games_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _game_browser_column_slot_count_a;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_slot_count_b;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_slot_count_c;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_slot_count_d;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_player_count;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_available_slots;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_connectivity;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_compatible;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_mode;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_name;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_game;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_map;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_player_count_text;
    ++*column_count;
    column_names[*column_count] = _game_browser_column_join_mode;
    ++*column_count;
}

bool c_games_datasource::get_element(long element_handle, void *element, long element_size)
{
    bool result;
    if (element_handle >= 0 && element_handle < m_game_count && element_size == sizeof(long)) {
        memcpy(element, &m_game_indices[element_handle], sizeof(long));
        result = true;
    } else {
        result = false;
    }
    return result;
}

c_load_game_browser_screen_message::c_load_game_browser_screen_message(long screen_name, e_controller_index controller,
    e_window_index window, long layered_position, long squad_search_flags, e_browser_type browser_type)
    : c_load_screen_message(screen_name, controller, window, layered_position)
{
    m_squad_search_flags = squad_search_flags;
    m_browser_type = browser_type;
}

void c_load_game_browser_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_gui_game_browser *browser = static_cast<c_gui_game_browser *>(screen);
    browser->set_squad_search_flags(m_squad_search_flags);
    browser->set_type(m_browser_type);
}

struct s_transport_secure_identifier;
struct s_transport_secure_address;
struct s_transport_secure_key;



// @address 0x4d2290 ?get_player_appearance@c_games_datasource@@UAA_NJPAUs_player_appearance@@@Z
bool c_games_datasource::get_player_appearance(long element_handle, s_player_appearance *appearance)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_game_count) {
        const s_game_list_data *element = &m_game_indices[element_handle];
        const s_network_squad_status_data *status =
            user_interface_available_squad_get_status(element->squad_index);
        if (status && status->details.player_count > 0 && player_appearance_valid((const char *)&status->details.players[0].configuration.client.appearance)) {
            memcpy(appearance, &status->details.players[0].configuration.client.appearance, sizeof(status->details.players[0].configuration.client.appearance));
            result = true;
        }
    }
    return result;
}

// @address 0x4d1068 ?name_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::name_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second = user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            result = _wcsnicmp(first->details.session_name, second->details.session_name, 32);
            if (result == 0) {
                if (first->details.start_mode > second->details.start_mode)
                    result = -1;
                else if (first->details.start_mode < second->details.start_mode)
                    result = 1;
                else
                    result = 0;
            }
        }
    }
    return result;
}

// @address 0x4d11b0 ?mode_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::mode_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second = user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            if (first->details.start_mode > second->details.start_mode)
                result = -1;
            else if (first->details.start_mode < second->details.start_mode)
                result = 1;
            else if (first->details.game_engine_type > second->details.game_engine_type)
                result = -1;
            else if (first->details.game_engine_type < second->details.game_engine_type)
                result = 1;
            else
                result = 0;
        }
    }
    return result;
}

// @address 0x4d12e8 ?game_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::game_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second = user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            if (first->details.game_engine_type > second->details.game_engine_type)
                result = -1;
            else if (first->details.game_engine_type < second->details.game_engine_type)
                result = 1;
            else if (first->details.map_id > second->details.map_id)
                result = -1;
            else if (first->details.map_id < second->details.map_id)
                result = 1;
            else
                result = 0;
        }
    }
    return result;
}

// @address 0x4d1418 ?map_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::map_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second = user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            if (first->details.map_id > second->details.map_id)
                result = -1;
            else if (first->details.map_id < second->details.map_id)
                result = 1;
            else if (first->details.game_engine_type > second->details.game_engine_type)
                result = -1;
            else if (first->details.game_engine_type < second->details.game_engine_type)
                result = 1;
            else
                result = 0;
        }
    }
    return result;
}

// @address 0x4d1548 ?player_count_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::player_count_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second =
            user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            result = 0;
            if (result == 0) {
                if (first->details.player_count > second->details.player_count)
                    result = -1;
                else if (first->details.player_count < second->details.player_count)
                    result = 1;
                else
                    result = 0;
            }
        }
    }
    return result;
}

// @address 0x4d1548 ?connectivity_sort_proc@c_games_datasource@@KAHPBUs_game_list_data@@0@Z
int c_games_datasource::connectivity_sort_proc(const s_game_list_data *a, const s_game_list_data *b)
{
    int result = 0;
    if (a && b && a != b) {
        const s_network_squad_status_data *first = user_interface_available_squad_get_status(a->squad_index);
        const s_network_squad_status_data *second =
            user_interface_available_squad_get_status(b->squad_index);
        unsigned char first_compatible = first && user_interface_networking_build_is_compatible(first->type, first->version, first->build);
        unsigned char second_compatible = second && user_interface_networking_build_is_compatible(second->type, second->version, second->build);
        if (first_compatible && !second_compatible) {
            result = -1;
        } else if (!first_compatible && second_compatible) {
            result = 1;
        } else if (first && second) {
            result = 0;
            if (result == 0) {
                if (first->details.player_count > second->details.player_count)
                    result = -1;
                else if (first->details.player_count < second->details.player_count)
                    result = 1;
                else
                    result = 0;
            }
        }
    }
    return result;
}
bool c_games_datasource::get_integer_value(long index, long column, long *value)
{
    bool result = false;
    if (index >= 0 && index < m_game_count) {
        const s_game_list_data *element = &m_game_indices[index];
        const s_network_squad_status_data *status =
            user_interface_available_squad_get_status(element->squad_index);
        switch (column) {
        case _game_browser_column_slot_count_a:
            *value = status ? (short)status->details.slot_count_a : 0;
            result = true;
            break;
        case _game_browser_column_slot_count_b:
            *value = status ? status->details.slot_count_b : 0;
            result = true;
            break;
        case _game_browser_column_slot_count_c:
            *value = status ? status->details.slot_count_c : 0;
            result = true;
            break;
        case _game_browser_column_slot_count_d:
            *value = status ? status->details.slot_count_d : 0;
            result = true;
            break;
        case _game_browser_column_player_count:
            *value = status ? status->details.player_count : 0;
            result = true;
            break;
        case _game_browser_column_available_slots:
            *value = status ? status->details.slot_count_b + (short)status->details.slot_count_a : 0;
            result = true;
            break;
        case _game_browser_column_connectivity:
            *value = 0;
            result = true;
            break;
        case _game_browser_column_compatible:
            {
                unsigned char compatible = status ? user_interface_networking_build_is_compatible(status->type, status->version, status->build) : false;
                *value = compatible;
                result = true;
            }
            break;
        }
    }
    return result;
}

bool c_games_datasource::get_string_id_value(long index, long column, long *value)
{
    bool result = false;
    if (index >= 0 && index < m_game_count) {
        const s_game_list_data *element = &m_game_indices[index];
        const s_network_squad_status_data *status =
            user_interface_available_squad_get_status(element->squad_index);
        switch (column) {
        case _game_browser_column_mode:
            if (status) {
                *value = 249;
                switch (status->details.kind) {
                case 0:
                    *value = 0x10074;
                    break;
                case 2:
                    if (status->details.campaign_id != -1)
                        *value = 0x10074;
                    else if (status->details.game_engine_type >= 1 && status->details.game_engine_type <= 10)
                        *value = 0x10077;
                    break;
                case 1:
                    *value = 0x10073;
                    break;
                case 3:
                    *value = 0x10078;
                    break;
                case 4:
                    *value = 0x10079;
                    break;
                default:
                    switch (status->details.start_mode) {
                    case 1:
                        if (status->details.campaign_id != -1)
                            *value = 0x10074;
                        else if (status->details.game_engine_type >= 1 && status->details.game_engine_type <= 10)
                            *value = 0x10077;
                        break;
                    case 2:
                        *value = 0x10073;
                        break;
                    case 3:
                        *value = 0x10213;
                        break;
                    default:
                        *value = 0;
                        break;
                    }
                    break;
                }
                result = true;
            }
            break;
        }
    }
    return result;
}

// @address 0x96e058 ?g_level_globals@@3Us_levels_globals@@A

static __forceinline void gui_game_browser_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    string->m_string[0] = 0;
    const s_user_interface_shared_globals *globals = ((const s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
    if (globals && globals->global_strings.index != -1)
        text_group_get_string(globals->global_strings.index, string_id, string);
}

// @address 0x4d1cb8 ?get_text_value@c_games_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_games_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_game_count) {
        const s_game_list_data *element = &m_game_indices[element_handle];
        const s_network_squad_status_data *status =
            user_interface_available_squad_get_status(element->squad_index);
        switch (value_name) {
        case _game_browser_column_name:
            if (status) {
                value->set(status->details.session_name);
                result = true;
            }
            break;
        case _game_browser_column_map:
            {
                levels_map_datum level;
                long map;
                if ((status->details.campaign_id != -1 && (map = status->details.map_id, levels_try_and_get_datum(g_level_globals.map_levels, map, &level)))
                    || (status->details.game_engine_type >= 1 && status->details.game_engine_type <= 10 && (map = status->details.map_id,
                        levels_try_and_get_datum(g_level_globals.multiplayer_levels, map, &level))))
                    {
                    value->set(level.display_name);
                    result = true;
                }
            }
            break;
        case _game_browser_column_game:
            if (status->details.campaign_id != -1) {
                gui_game_browser_global_string_get(0x10075, value);
                result = true;
            } else if (status->details.game_engine_type >= 1 && status->details.game_engine_type <= 10) {
                switch (status->details.game_engine_type) {
                case 1:
                    gui_game_browser_global_string_get(233, value);
                    result = true;
                    break;
                case 2:
                    gui_game_browser_global_string_get(235, value);
                    result = true;
                    break;
                case 3:
                    gui_game_browser_global_string_get(236, value);
                    result = true;
                    break;
                case 4:
                    gui_game_browser_global_string_get(237, value);
                    result = true;
                    break;
                case 5:
                    gui_game_browser_global_string_get(242, value);
                    result = true;
                    break;
                case 6:
                    gui_game_browser_global_string_get(239, value);
                    result = true;
                    break;
                case 7:
                    gui_game_browser_global_string_get(238, value);
                    result = true;
                    break;
                case 8:
                    gui_game_browser_global_string_get(240, value);
                    result = true;
                    break;
                case 9:
                    gui_game_browser_global_string_get(234, value);
                    result = true;
                    break;
                case _game_engine_infection:
                    gui_game_browser_global_string_get(241, value);
                    result = true;
                    break;
                default:
                    gui_game_browser_global_string_get(249, value);
                    result = true;
                    break;
                }
            }
            break;
        case _game_browser_column_player_count_text:
            if (status) {
                short total_a = status->details.slot_count_d + status->details.slot_count_c;
                short total_b = total_a + status->details.slot_count_b + (short)status->details.slot_count_a;
                value->print(L"%d/%d", total_a, total_b);
                result = true;
            }
            break;
        case _game_browser_column_join_mode:
            if (status) {
                switch (status->details.join_mode) {
                case 0:
                    gui_game_browser_global_string_get(0x10b, value);
                    break;
                case 1:
                    gui_game_browser_global_string_get(0x10c, value);
                    break;
                case 2:
                    gui_game_browser_global_string_get(0x10d, value);
                    break;
                case 3:
                    gui_game_browser_global_string_get(0x10e, value);
                    break;
                default:
                    gui_game_browser_global_string_get(249, value);
                    break;
                }
                result = true;
            }
            break;
        }
    }
    return result;
}




// @address 0x80a2e4 g_network_search_globals
// @address 0x8c4948 g_user_interface_network_search_started
static __forceinline void gui_game_browser_available_squads_dispose()
{
    if (g_user_interface_network_search_started != 0) {
        g_network_search_globals.reference_count = g_network_search_globals.reference_count - 1;
        if (g_network_search_globals.reference_count <= 0) {
            network_search_end();
            g_network_search_globals.maximum = 0;
            g_network_search_globals.allocator = 0;
        }
        g_user_interface_network_search_started = 0;
    }
}


static __forceinline long gui_game_browser_available_squads_first()
{
    long result = -1;
    if (g_user_interface_network_search_started) {
        long count = g_network_search_globals.count;
        for (long i = 0; i < count; i++) {
            if (network_search_session(i)) {
                result = i;
                break;
            }
        }
    }
    return result;
}
static __forceinline bool gui_game_browser_available_squad_exists(long squad_index)
{
    bool result = false;
    if (g_user_interface_network_search_started && network_search_session(squad_index))
        result = true;
    return result;
}
static __forceinline bool gui_game_browser_available_squad_is_local_box(long squad_index)
{
    s_transport_secure_address local_address;
    bool have_address = transport_secure_address_get(&local_address);
    const s_network_squad_status_data *status = user_interface_available_squad_get_status(squad_index);
    return have_address && status && transport_secure_address_compare(&((const s_network_squad_status_data *)status)->details.secure_address, &local_address);
}

// @address 0x4d1670 ?update@c_games_datasource@@UAAXXZ
void c_games_datasource::update()
{
    m_game_count = 0;
    for (long squad_index = gui_game_browser_available_squads_first(); squad_index != -1; squad_index = user_interface_available_squads_iterate(squad_index))
    {
        if (m_game_count >= sizeof(m_game_indices) / sizeof(m_game_indices[0]))
            continue;
        if (!gui_game_browser_available_squad_exists(squad_index))
            continue;
        const s_network_squad_status_data *status = user_interface_available_squad_get_status(squad_index);
        s_available_session *session = network_search_session(squad_index);
        bool local_box = gui_game_browser_available_squad_is_local_box(squad_index);
        bool joinable = status && user_interface_remote_squad_is_joinable(status);
        bool contains_local_peer = status && user_interface_remote_squad_contains_local_peer(status);
        if (session && session->joinable && !local_box && !contains_local_peer)
            m_game_indices[m_game_count++].squad_index = squad_index;
    }
    if (m_game_count > 0)
    {
        int (*sort_proc)(const void *, const void *);
        switch ((e_browser_list_sort_column)m_browser_game_count)
        {
        case _browser_list_sort_column_name:
            sort_proc = (int (*)(const void *, const void *))name_sort_proc;
            break;
        case _browser_list_sort_column_mode:
            sort_proc = (int (*)(const void *, const void *))mode_sort_proc;
            break;
        case _browser_list_sort_column_game:
            sort_proc = (int (*)(const void *, const void *))game_sort_proc;
            break;
        case _browser_list_sort_column_map:
            sort_proc = (int (*)(const void *, const void *))map_sort_proc;
            break;
        case _browser_list_sort_column_player_count:
            sort_proc = (int (*)(const void *, const void *))player_count_sort_proc;
            break;
        case _browser_list_sort_column_connectivity:
            sort_proc = (int (*)(const void *, const void *))connectivity_sort_proc;
            break;
        default:
            sort_proc = (int (*)(const void *, const void *))name_sort_proc;
            break;
        }
        qsort(m_game_indices, m_game_count, sizeof(long), sort_proc);
    }
}


// @address 0x4d2470 ?dispose@c_gui_game_browser@@UAAXXZ
void c_gui_game_browser::dispose()
{
    c_gui_screen_widget::dispose();
    gui_game_browser_available_squads_dispose();
}



static __forceinline void gui_game_browser_squads_initialize(e_controller_index controller_index, long search_flags, unsigned char upcoming)
{
    s_available_squad_globals *g = &g_network_search_globals;
    if (g->reference_count <= 0) {
        g->maximum = 25;
        g->flags = search_flags;
        g->active = upcoming != 0;
        g->allocator = &g_user_interface_allocation;
    }
    bool started = network_search_begin(controller_index);
    if (started)
        ++g->reference_count;
    g_user_interface_network_search_started = started;
}

static __forceinline void gui_game_browser_squads_set_active(e_controller_index controller_index)
{
    if (g_user_interface_network_search_started)
        network_search_begin(controller_index);
}

// @address 0x4d2348 ?initialize@c_gui_game_browser@@UAAXXZ
void c_gui_game_browser::initialize()
{
    e_controller_index controller_index = get_any_responding_controller();
    unsigned char upcoming;
    unsigned long type = get_type();
    if (type >= 1) {
        if (type == 1 || type < 3)
            upcoming = 1;
        else
            upcoming = 0;
    } else {
        upcoming = 0;
    }
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (static_cast<const char *>(0), 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<browser-title" /* L"<browser-title" */, this, parse_game_browser_title));
    gui_game_browser_squads_initialize(controller_index, get_squad_search_flags(), upcoming);
    gui_game_browser_squads_set_active(controller_index);
}

// @address 0x4d24e8 ?initialize_datasource@c_gui_game_browser@@UAAXXZ
void c_gui_game_browser::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    c_gui_data *datasource = new (static_cast<const char *>(0), 0, _ui_allocation_marker_dummy) c_games_datasource;
    if (datasource) {
        if (datasource->initialize(_string_id_game_browser))
            add_datasource(datasource);
        else
            ui_track_delete(datasource);
    }
}



// @address 0x4d25c0 ?update@c_gui_game_browser@@UAAXK@Z
void c_gui_game_browser::update(unsigned long current_milliseconds)
{
    c_gui_data *datasource = 0;
    for (long index = 0; index < m_datasource_count; ++index)
    {
        if (m_datasource[index]->m_name == _string_id_game_browser)
        {
            datasource = m_datasource[index];
            break;
        }
    }

    c_gui_widget *list = get_child_widget(_gui_list_widget, _string_id_game_browser);
    if (datasource && list)
    {
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget);
             item;
             item = gui_widget_get_next_list_item(item, true))
        {
            c_gui_widget *first = item->get_child_widget(_gui_bitmap_widget, _player_select_string_base_color);
            c_gui_widget *second = item->get_child_widget(_gui_bitmap_widget, _player_select_string_base_color_hilite);
            long row = item->get_element_handle();
            long value = 0;
            if (first && second && datasource->get_integer_value(row, _string_id_enabled, &value))
            {
                first->set_visible(value != 0);
                second->set_visible(value != 0);
            }
        }
    }

    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4d2738 ?update_render_state@c_gui_game_browser@@UAAXK@Z
void c_gui_game_browser::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    long count = m_datasource_count;
    c_gui_data *games = 0;
    long index = 0;
    long wanted = _string_id_game_browser;
    if (count > 0) {
        c_gui_data **slot = m_datasource;
        for (; index < count; ++index, ++slot) {
            if ((*slot)->m_name == wanted) {
                games = m_datasource[index];
                break;
            }
        }
    }
    c_gui_widget *list = get_child_widget(_gui_list_widget, wanted);
    if (games != 0 && list != 0) {
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, 5); item != 0; item = gui_widget_get_next_list_item(item, true)) {
            c_gui_widget *first_tint = item->get_child_widget(_gui_bitmap_widget, 0x10127);
            c_gui_widget *second_tint = item->get_child_widget(_gui_bitmap_widget, 0x103b8);
            long element_handle = item->get_element_handle();
            long squad_index;
            s_player_appearance appearance;
            if (games->get_element(element_handle, &squad_index, sizeof(squad_index))) {
                const s_network_squad_status_data *status = user_interface_available_squad_get_status(squad_index);
                if (status != 0 && games->get_player_appearance(element_handle, &appearance)) {
                    bool team_flag;
                    if (status->details.teams_enabled != 0) {
                        team_flag = true;
                        if (status->details.kind == 4)
                            team_flag = false;
                    } else {
                        team_flag = false;
                    }
                    long color;
                    if (team_flag && status->details.player_count > 0)
                        color = status->details.players[0].configuration.host.team_index;
                    else
                        color = appearance.primary_color;
                    tint_widget_to_change_color(first_tint, color, team_flag);
                    tint_widget_to_change_color(second_tint, color, team_flag);
                }
            }
        }
    }
}

bool parse_game_browser_title(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_game_browser *game_browser = (c_gui_game_browser *)context;
    c_static_wchar_string<1024> title;
    bool success;
    switch (game_browser->get_type())
    {
    case 0:
        success = game_browser->get_string_by_string_id(0x1033c, &title);
        if (success == true)
            title.copy_to(buffer, buffer_length);
        break;
    case 1:
        success = game_browser->get_string_by_string_id(0x1033d, &title);
        if (success == true)
            title.copy_to(buffer, buffer_length);
        break;
    case 2:
        success = game_browser->get_string_by_string_id(0x1033e, &title);
        if (success == true)
            title.copy_to(buffer, buffer_length);
        break;
    default:
        success = false;
        break;
    }
    return success;
}

// @address 0x4d2900 ?handle_list_item_chosen@c_gui_game_browser@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_game_browser::handle_list_item_chosen(
    const c_controller_input_message *message, long name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (name == _string_id_game_browser) {
        long element_handle = item->get_element_handle();
        if (element_handle != -1) {
            long squad_index;
            if (data->get_element(element_handle, &squad_index, sizeof(squad_index))) {
                if (squad_index != -1) {
                    bool valid = false;
                    if (g_user_interface_network_search_started != 0) {
                        if (network_search_session(squad_index) != 0)
                            valid = true;
                    }
                    if (valid) {
                        const s_game_browser_join_status *status =
                            (const s_game_browser_join_status *)user_interface_available_squad_get_status(squad_index);
                        if (status != 0) {
                            // Keep the session join details together.
                            const s_network_squad_join_details *details = &status->details;
                            if (details->player_count < 16) {
                                if (details->session_class != 0) {
                                    user_interface_join_remote_session(false, (e_network_session_class)details->session_class,
                                        &details->session_id,
                                        &details->secure_address,
                                        &details->session_key);
                                    transition_out_with_transition_type(0, _screen_transition_normal);
                                } else {
                                    ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                                }
                            } else {
                                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                            }
                        }
                    }
                }
            }
        }
        handled = true;
    }
    if ((unsigned char)handled == 0)
        handled = false;
    return handled;
}

// @address 0x4d2a58 ?show_list_item_details@c_gui_game_browser@@QAAXW4e_controller_index@@@Z
void c_gui_game_browser::show_list_item_details(e_controller_index controller)
{
    if (this->m_suppress_focus == 0) {
        c_gui_widget *focused = this->m_focused_widget;
        if (focused != 0) {
            c_gui_widget *list = gui_widget_get_parent_list_widget(focused);
            if (focused->m_type != _gui_list_item_widget)
                focused = 0;
            if (list != 0 && focused != 0) {
                if (focused->get_enabled()) {
                    c_gui_data *data = ((c_gui_list_widget *)list)->get_data();
                    if (data != 0 && list->m_name == _string_id_game_browser) {
                        long element_handle = focused->get_element_handle();
                        if (element_handle != -1) {
                            long squad_index;
                            if (data->get_element(element_handle, &squad_index, sizeof(squad_index))) {
                                if (squad_index != -1) {
                                    bool valid = false;
                                    if (g_user_interface_network_search_started != 0) {
                                        if (network_search_session(squad_index) != 0)
                                            valid = true;
                                    }
                                    if (valid) {
                                        const s_network_squad_status_data *status = user_interface_available_squad_get_status(squad_index);
                                        if (status != 0) {
                                            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_game_details_screen_message), (char const *)(0), 0);
                                            if (memory != 0) {
                                                long screen_name = this->m_name;
                                                e_window_index window = this->get_render_window();
                                                c_load_game_details_screen_message *message = new (memory) c_load_game_details_screen_message(
                                                    _string_id_game_details, controller, window, screen_name, 0, status);
                                                message->m_parent_screen_index = this->m_screen_index;
                                                ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// @address 0x4d2c18 ?handle_controller_input_message@c_gui_game_browser@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_game_browser::handle_controller_input_message(const c_controller_input_message *message)
{
    if (message->m_event_type == 13) {
        long datasource_index;
        if (get_data(_string_id_game_browser, &datasource_index) != 0) {
            if (message->m_component == 2)
                show_list_item_details(message->m_controller);
        }
    }
    return c_gui_screen_widget::handle_controller_input_message(message);
}
