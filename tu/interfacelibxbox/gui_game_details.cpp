// interfacelibxbox/gui_game_details.obj: from the records 0x95140 0x499198 0x4991a8 0x4991f8 0x499410 0x499448 0x499740 0x499be8 0x499cb8 0x49a2e0 0x49a3e8 0x49a680 0x49a710 0x49a970 0x49abc0 0x49ac40 0x49ac80 0x49acf8 0x49ad40 0x49ada8 0x49add8 0x49af58 0x49b068 0x49b070 0x49b190 0x49b1c0 0x49b270 0x49b2e0 0x49b320 0x49b360 0x49b440 0x49b470 0x49b528 0x49b558 0x4f3250
#include "interface/user_interface_networking.h"
#include "networking/online/online_cached_user.h"
#include "networking/logic/network_search_types.h"
// @flags /TP /O2 /Oc
#include "interface/gui_screens/game_details/load_game_details_screen_message.h"
#include "interface/gui_render_data_size_types.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_roster_list_externs.h"
#include <new>
#include "interface/gui_screens/player_select/player_select_screen_message.h"
#include "interface/gui_roster_data.h"
#include "networking/logic/network_squad_join_details.h"
#include "cseries/cseries_unicode.h"
#include "tag_files/tag_types.h"
#include <stddef.h>
#include "main/levels_map_datum_types.h"
#include "main/levels_globals.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "interface/c_gui_bitmap_widget.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "shell/shell.h"
#include "memory/static_arrays.h"
// @address 0x95140 ?copy_narrow_to_wide@@YAXPBDPAFHH@Z
// @address 0x4991a8 ?get_string_id_value@c_gui_game_details_menu_datasource@@UAA_NJJPAJ@Z
// @address 0x499410 ?get_column_names@c_gui_game_details_info_datasource@@UAAXQAJPAJ@Z
// @address 0x499448 ?get_string_id_value@c_gui_game_details_info_datasource@@UAA_NJJPAJ@Z
// @address 0x499740 ?show_only_necessary_fields@c_gui_game_details_info_datasource@@QAAXPBUs_network_squad_status_data@@W4e_controller_index@@J@Z
// @address 0x49acf8 ?parse_party_player_count@@YA_NPAXPA_WJ@Z
// @address 0x49ad40 ?parse_party_player_max@@YA_NPAXPA_WJ@Z
// @address 0x49b2e0 parse_first_detail_number
// @address 0x49b320 parse_second_detail_number
// @address 0x49b360 parse_campaign_name
// @address 0x49b270 parse_time_remaining
// @address 0x49b068 parse_map_name_alias
// @address 0x49a680 ?create_list_widget@c_gui_game_details@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
#include "interface/user_interface_messages.h"
#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/user_interface_utilities.h"
#include "networking/online/online_xenon.h"
#include "interface/gui_screens/game_details/gui_game_details.h"
#include "interface/gui_screens/game_details/gui_game_details_string_ids.h"
#include "tag_files/string_ids.h"
#include "interface/gui_render_data_size_types.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/ui_allocation_marker_types.h"
#include "cache/cache_files.h"
#include "interface/c_gui_widget.h"
#include "interface/user_interface_allocation.h"
#include "networking/transport/transport_security_types.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_types.h"
#include "networking/session/network_session_class.h"
#include "memory/data_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/gui_widget.h"
#include "interface/gui_widget_enums.h"
#include "game/player_identifier_types.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_widget.h"
#include "game/player_configuration.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/gui_screens/game_options/gui_screen_game_options.h"

#include "interface/gui_game_details_externs.h"
#include "interface/user_interface_squad_session.h"
#include "text/text_group.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_runtime_constants.h"
// @address 0x11401e8 g_controller_globals
// @address 0x96e058 ?g_level_globals@@3Us_levels_globals@@A
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"

// @address 0x563f0 ??_7c_gui_static_squad_roster_data@@6B@
// @address 0x4c0ee0 ??_Gc_gui_static_squad_roster_data@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_static_squad_roster_data@@UAAPAXI@Z
// @address 0x5bec98 ?update@c_gui_static_squad_roster_data@@UAAXXZ
class c_gui_static_squad_roster_data : public c_gui_roster_data
{
public:
    c_gui_static_squad_roster_data() : c_gui_roster_data(k_no_controller) {}
    virtual void update() {}
};

static __forceinline void add_name(long *names, long *count, long name)
{
    names[*count] = name;
    (*count)++;
}

void copy_narrow_to_wide(const char *source, short *destination, int maximum_count, int unused)
{
    const char *read = source;
    short *wide = destination;
    int count = maximum_count;
    if (count <= 0)
        return;
    do {
        unsigned char character = *read;
        if (count == 1)
            *wide = 0;
        else if (character <= 127)
            *wide = (signed char)character;
        else
            *wide = 0x25a1;
        if (*read == 0)
            return;
        --count;
        ++wide;
        ++read;
    } while (count > 0);
}

// @address 0x499198 ?get_render_data_size@c_gui_game_details@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_game_details::get_render_data_size()
{
  return k_render_data_size_large;
}

bool c_gui_game_details_menu_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    switch (element_handle)
    {
    case 0:
        *value = _game_details_string_option_join;
        result = true;
        break;
    case 1:
        *value = _game_details_string_option_invite;
        result = true;
        break;
    case 2:
        *value = _game_details_string_option_cantinvite;
        result = true;
        break;
    default:
        *value = _game_details_string_option_message;
        result = true;
        break;
    }
    return result;
}

void c_gui_game_details_menu_datasource::show_only_necessary_fields(const s_network_squad_status_data *config, e_controller_index controller, long player_index)
{

    bool join = false;
    bool invite = false;
    bool profile = false;
    bool online = ::online_local_xuid_is_online_enabled()
        && online_cached_user_is_silver_or_gold(controller) && online_cached_user_is_online_enabled(controller);
    const s_network_squad_status_player *player = &config->details.players[player_index];
    bool local_player = ::user_interface_session_player_get_player_index(&player->identifier) != -1;
    bool live_player = player->configuration.client.online && player->configuration.client.silver;
    if (!local_player && user_interface_remote_squad_is_joinable(config))
        join = true;
    if (live_player && online) {
        if (::user_interface_squad_exists() && !local_player && ::user_interface_squad_is_joinable() == 0) {
            long maximum_player_count = ::network_squad_session_get_maximum_player_count();
            if (::user_interface_squad_get_player_count() < maximum_player_count && ::network_squad_session_get_session_class() == 2 &&
                user_interface_squad_get_join_mode() != 3)
                invite = true;
        }
        profile = true;
    }
    clear_disabled_elements();
    if (!join)
        set_disabled_element(_game_details_string_name, _game_details_string_option_join);
    if (!invite) {
        set_disabled_element(_game_details_string_name, _game_details_string_option_invite);
        set_disabled_element(_game_details_string_name, _game_details_string_option_cantinvite);
    } else {
        set_disabled_element(_game_details_string_name, _game_details_string_option_cantinvite);
    }
    if (!profile)
        set_disabled_element(_game_details_string_name, _game_details_string_option_message);
}

// @address 0x4b4128 ?get_column_names@c_gui_game_details_menu_datasource@@UAAXQAJPAJ@Z
void c_gui_game_details_menu_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    add_name(names, count, _game_details_string_name);
}

// @address 0x3fc550 ?get_current_item_count_internal@c_gui_game_details_menu_datasource@@MAAJXZ
long c_gui_game_details_menu_datasource::get_current_item_count_internal()
{
    return 4;
}

// @address 0x42e678 ?get_current_item_count_internal@c_gui_game_details_info_datasource@@MAAJXZ
long c_gui_game_details_info_datasource::get_current_item_count_internal()
{
    return 15;
}

void c_gui_game_details_info_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    add_name(names, count, _game_details_string_name);
    add_name(names, count, _game_details_string_value);
}

bool c_gui_game_details_info_datasource::get_string_id_value(long type, long b, long *out)
{
    bool result = false;
    switch (type) {
    case _game_details_info_row_party_leader:
        if (b == _game_details_string_name)
            *out = _game_details_string_party_leader_heading;
        else
            *out = _game_details_string_party_leader_info;
        result = true;
        break;
    case _game_details_info_row_mode:
        if (b == _game_details_string_name)
            *out = _game_details_string_mode_heading;
        else
            *out = _game_details_string_mode_info;
        result = true;
        break;
    case _game_details_info_row_status:
        if (b == _game_details_string_name)
            *out = _game_details_string_status_heading;
        else
            *out = _game_details_string_status_info;
        result = true;
        break;
    case _game_details_info_row_level:
        if (b == _game_details_string_name)
            *out = _game_details_string_level_heading;
        else
            *out = _game_details_string_level_info;
        result = true;
        break;
    case _game_details_info_row_difficulty:
        if (b == _game_details_string_name)
            *out = _game_details_string_difficulty_heading;
        else
            *out = _game_details_string_difficulty_info;
        result = true;
        break;
    case _game_details_info_row_film:
        if (b == _game_details_string_name)
            *out = _game_details_string_film_heading;
        else
            *out = _game_details_string_film_info;
        result = true;
        break;
    case _game_details_info_row_playlist:
        if (b == _game_details_string_name)
            *out = _game_details_string_playlist_heading;
        else
            *out = _game_details_string_playlist_info;
        result = true;
        break;
    case _game_details_info_row_time_remaining:
        if (b == _game_details_string_name)
            *out = _game_details_string_time_remaining_heading;
        else
            *out = _game_details_string_time_remaining_info;
        result = true;
        break;
    case _game_details_info_row_score_to_win:
        if (b == _game_details_string_name)
            *out = _game_details_string_score_to_win_heading;
        else
            *out = _game_details_string_score_to_win_info;
        result = true;
        break;
    case _game_details_info_row_leading_score:
        if (b == _game_details_string_name)
            *out = _game_details_string_leading_score_heading;
        else
            *out = _game_details_string_leading_score_info;
        result = true;
        break;
    case _game_details_info_row_game:
        if (b == _game_details_string_name)
            *out = _game_details_string_game_heading;
        else
            *out = _game_details_string_game_info;
        result = true;
        break;
    case _game_details_info_row_game_variant:
        if (b == _game_details_string_name)
            *out = _game_details_string_game_variant_heading;
        else
            *out = _game_details_string_game_variant_info;
        result = true;
        break;
    case _game_details_info_row_map:
        if (b == _game_details_string_name)
            *out = _game_details_string_map_heading;
        else
            *out = _game_details_string_map_info;
        result = true;
        break;
    case _game_details_info_row_map_variant:
        if (b == _game_details_string_name)
            *out = _game_details_string_map_variant_heading;
        else
            *out = _game_details_string_map_variant_info;
        result = true;
        break;
    case _game_details_info_row_teams:
        if (b == _game_details_string_name)
            *out = _game_details_string_teams_heading;
        else
            *out = _game_details_string_teams_info;
        result = true;
        break;
    }
    return result;
}

void c_gui_game_details_info_datasource::show_only_necessary_fields(const s_network_squad_status_data *config, e_controller_index, long)
{
  unsigned char flags[k_game_details_info_row_count];
  short kind;
  memset(flags, 0, sizeof(flags));
  kind = config->details.kind;
  switch (kind) {
  case 0:
    flags[_game_details_info_row_level] = 1;
    flags[_game_details_info_row_difficulty] = 1;
    break;
  case 1:
    if (config->details.variant == 3) {
      flags[_game_details_info_row_score_to_win] = 1;
      flags[_game_details_info_row_leading_score] = 1;
      flags[_game_details_info_row_time_remaining] = 1;
    }
    flags[_game_details_info_row_playlist] = 1;
    break;
  case 2:
    if (config->details.variant == 3) {
      flags[_game_details_info_row_score_to_win] = 1;
      flags[_game_details_info_row_leading_score] = 1;
      flags[_game_details_info_row_time_remaining] = 1;
      flags[_game_details_info_row_teams] = 1;
    }
    flags[_game_details_info_row_game] = 1;
    flags[_game_details_info_row_game_variant] = 1;
    flags[_game_details_info_row_map] = 1;
    flags[_game_details_info_row_map_variant] = 1;
    break;
  case 3:
    flags[_game_details_info_row_map] = 1;
    flags[_game_details_info_row_map_variant] = 1;
    break;
  case 4:
    flags[_game_details_info_row_film] = 1;
    break;
  }
  clear_disabled_elements();
  if (!flags[_game_details_info_row_level]) {
    set_disabled_element(_game_details_string_name, _game_details_string_level_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_level_info);
  }
  if (!flags[_game_details_info_row_difficulty]) {
    set_disabled_element(_game_details_string_name, _game_details_string_difficulty_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_difficulty_info);
  }
  if (!flags[_game_details_info_row_film]) {
    set_disabled_element(_game_details_string_name, _game_details_string_film_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_film_info);
  }
  if (!flags[_game_details_info_row_playlist]) {
    set_disabled_element(_game_details_string_name, _game_details_string_playlist_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_playlist_info);
  }
  if (!flags[_game_details_info_row_time_remaining]) {
    set_disabled_element(_game_details_string_name, _game_details_string_time_remaining_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_time_remaining_info);
  }
  if (!flags[_game_details_info_row_score_to_win]) {
    set_disabled_element(_game_details_string_name, _game_details_string_score_to_win_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_score_to_win_info);
  }
  if (!flags[_game_details_info_row_leading_score]) {
    set_disabled_element(_game_details_string_name, _game_details_string_leading_score_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_leading_score_info);
  }
  if (!flags[_game_details_info_row_game]) {
    set_disabled_element(_game_details_string_name, _game_details_string_game_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_game_info);
  }
  if (!flags[_game_details_info_row_game_variant]) {
    set_disabled_element(_game_details_string_name, _game_details_string_game_variant_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_game_variant_info);
  }
  if (!flags[_game_details_info_row_map]) {
    set_disabled_element(_game_details_string_name, _game_details_string_map_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_map_info);
  }
  if (!flags[_game_details_info_row_map_variant]) {
    set_disabled_element(_game_details_string_name, _game_details_string_map_variant_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_map_variant_info);
  }
  if (!flags[_game_details_info_row_teams]) {
    set_disabled_element(_game_details_string_name, _game_details_string_teams_heading);
    set_disabled_element(_game_details_string_value, _game_details_string_teams_info);
  }
}


// @address 0x499be8 ?apply_initial_state@c_load_game_details_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_game_details_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
  c_gui_game_details *game_details = static_cast<c_gui_game_details *>(screen);
  game_details->m_initial_state = m_initial_state;
  memcpy(&game_details->m_game_details, m_game_details, sizeof(game_details->m_game_details));
}

// 0x499cb8
// Short tag strings are zero-padded to 4 bytes.
// @address 0x4fff0 k_game_details_tag_selected_player_name
// @address 0x5000c k_game_details_tag_squad_join_status
// @address 0x5002c k_game_details_tag_party_player_count
// @address 0x50048 k_game_details_tag_party_player_max
// @address 0x50060 k_game_details_tag_first_player_name
// @address 0x50088 k_game_details_tag_game_mode_name
// @address 0x500a0 k_game_details_tag_game_status_text
// @address 0x500bc k_game_details_tag_map_name_alias
// @address 0x500d4 k_magic_string_difficulty
// @address 0x500f8 k_game_details_tag_variant_name
// @address 0x50110 k_game_details_tag_description_text
// @address 0x50130 k_game_details_tag_time_remaining
// @address 0x5015c k_game_details_tag_first_detail_number
// @address 0x50184 k_game_details_tag_second_detail_number
// @address 0x501ac k_game_details_tag_campaign_name
// @address 0x501c4 k_game_details_tag_campaign_detail_text
// @address 0x501ec k_magic_string_map_name
// @address 0x50200 k_game_details_tag_variant_short_name
// @address 0x50224 k_game_details_tag_team_count
// @rdata 0x8204fff0..0x8205023c
extern "C" {
const wchar_t k_game_details_tag_selected_player_name[] = {
    '<', 'p', 'l', 'a', 'y', 'e', 'r', '-', 'n', 'a', 'm', 'e', 0, 0
};
const wchar_t k_game_details_tag_squad_join_status[] = {
    '<', 'p', 'a', 'r', 't', 'y', '-', 'j', 'o', 'i', 'n', 'a', 'b', 'l', 'e', 0
};
const wchar_t k_game_details_tag_party_player_count[] = {
    '<', 'p', 'l', 'a', 'y', 'e', 'r', '-', 'c', 'o', 'u', 'n', 't', 0
};
const wchar_t k_game_details_tag_party_player_max[] = {
    '<', 'p', 'l', 'a', 'y', 'e', 'r', '-', 'm', 'a', 'x', 0
};
const wchar_t k_game_details_tag_first_player_name[] = {
    '<', 'p', 'a', 'r', 't', 'y', '-', 'l', 'e', 'a', 'd', 'e', 'r', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_game_mode_name[] = {
    '<', 'm', 'o', 'd', 'e', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_game_status_text[] = {
    '<', 's', 't', 'a', 't', 'u', 's', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_map_name_alias[] = {
    '<', 'l', 'e', 'v', 'e', 'l', '-', 'i', 'n', 'f', 'o', 0
};
const wchar_t k_magic_string_difficulty[] = {
    '<', 'd', 'i', 'f', 'f', 'i', 'c', 'u', 'l', 't', 'y', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_variant_name[] = {
    '<', 'f', 'i', 'l', 'm', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_description_text[] = {
    '<', 'p', 'l', 'a', 'y', 'l', 'i', 's', 't', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_time_remaining[] = {
    '<', 't', 'i', 'm', 'e', '-', 'r', 'e', 'm', 'a', 'i', 'n', 'i', 'n', 'g', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_first_detail_number[] = {
    '<', 's', 'c', 'o', 'r', 'e', '-', 't', 'o', '-', 'w', 'i', 'n', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_second_detail_number[] = {
    '<', 'l', 'e', 'a', 'd', 'i', 'n', 'g', '-', 's', 'c', 'o', 'r', 'e', '-', 'i', 'n', 'f', 'o', 0
};
const wchar_t k_game_details_tag_campaign_name[] = {
    '<', 'g', 'a', 'm', 'e', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_game_details_tag_campaign_detail_text[] = {
    '<', 'g', 'a', 'm', 'e', '-', 'v', 'a', 'r', 'i', 'a', 'n', 't', '-', 'i', 'n', 'f', 'o', 0, 0
};
const wchar_t k_magic_string_map_name[] = {
    '<', 'm', 'a', 'p', '-', 'i', 'n', 'f', 'o', 0
};
const wchar_t k_game_details_tag_variant_short_name[] = {
    '<', 'm', 'a', 'p', '-', 'v', 'a', 'r', 'i', 'a', 'n', 't', '-', 'i', 'n', 'f', 'o', 0
};
const wchar_t k_game_details_tag_team_count[] = {
    '<', 't', 'e', 'a', 'm', 's', '-', 'i', 'n', 'f', 'o', 0
};
}
// @rdata-end
// @address 0x499cb8 ?initialize@c_gui_game_details@@UAAXXZ
void c_gui_game_details::initialize()
{
    c_gui_screen_widget::initialize();
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_selected_player_name, this, (bool (*)(void *, wchar_t *, long))parse_selected_player_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_squad_join_status, this, (bool (*)(void *, wchar_t *, long))parse_squad_join_status));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_party_player_count, this, parse_party_player_count));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_party_player_max, this, parse_party_player_max));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_first_player_name, this, (bool (*)(void *, wchar_t *, long))parse_first_player_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_game_mode_name, this, (bool (*)(void *, wchar_t *, long))parse_game_mode_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_game_status_text, this, (bool (*)(void *, wchar_t *, long))parse_game_status_text));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_map_name_alias, this, (bool (*)(void *, wchar_t *, long))parse_map_name_alias));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_difficulty, this, (bool (*)(void *, wchar_t *, long))parse_difficulty_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_variant_name, this, (bool (*)(void *, wchar_t *, long))parse_variant_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_description_text, this, (bool (*)(void *, wchar_t *, long))parse_description_text));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_time_remaining, this, parse_time_remaining));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_first_detail_number, this, (bool (*)(void *, wchar_t *, long))parse_first_detail_number));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_second_detail_number, this, (bool (*)(void *, wchar_t *, long))parse_second_detail_number));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_campaign_name, this, (bool (*)(void *, wchar_t *, long))parse_campaign_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_campaign_detail_text, this, (bool (*)(void *, wchar_t *, long))parse_campaign_detail_text));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_map_name, this, (bool (*)(void *, wchar_t *, long))parse_map_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_game_details_tag_variant_short_name, this, (bool (*)(void *, wchar_t *, long))parse_variant_short_name));
    this->add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_game_details_tag_team_count, this, (bool (*)(void *, wchar_t *, long))parse_team_count));
}

// @address 0x49a2e0 ?update_render_state@c_gui_game_details@@UAAXK@Z
void c_gui_game_details::update_render_state(unsigned long current_milliseconds)
{
  c_gui_widget::update_render_state(current_milliseconds);
  s_network_squad_status_data_game_details *view = &m_game_details.details;
  c_gui_widget *widget = get_child_widget(_gui_bitmap_widget, _game_details_string_connection_quality);
  s_player_configuration *entry = &view->players[m_initial_state].configuration;
  if (widget) {
    if (view->session_class == 2) {
      short frame_count = 0;
      c_gui_bitmap_widget *frame = static_cast<c_gui_bitmap_widget *>(widget);
      bitmap_group *definition = (bitmap_group *)tag_get('bitm', frame->m_definition.bitmap_definition_index);
      if (definition) {
        long count = definition->sequences.count ? ((bitmap_sequence *)definition->sequences.address)->sprites.count : definition->bitmaps.count;
        frame_count = (short)count;
      }
      short *minimum = &view->average_rating;
      short maximum = (short)(frame_count - 1);
      if (*minimum > maximum)
        minimum = &maximum;
      frame->m_bitmap_override = *minimum;
    } else {
      widget->set_visible(false);
    }
  }
  if (entry) {
    ::tint_widget_to_change_color(
        get_child_widget(_gui_bitmap_widget, _game_details_string_base_color),
        entry->client.appearance.primary_color,
        false);
  }
}


// @address 0x49abc0 ?get_emblem_info_for_widget@c_gui_game_details@@UAA_NPAVc_gui_widget@@PAUs_emblem_info@@@Z
bool c_gui_game_details::get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *player_id)
{
  bool result = false;
  if (widget->m_name == _game_details_string_party_leader_emblem) {
    long index = m_initial_state;
    s_network_squad_status_data_game_details *view = &m_game_details.details;
    if (index >= 0 && index < view->player_count) {
      result = true;
      *player_id = view->players[index].configuration.client.appearance.emblem;
    }
  }
  if (result) {
    return result;
  }
  return false;
}


// @address 0x49ac40 parse_selected_player_name
extern "C" bool parse_selected_player_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  ustrnzcpy(destination, static_cast<c_gui_game_details *>(screen)->m_game_details.details.players[
      static_cast<c_gui_game_details *>(screen)->m_initial_state].configuration.host.player_name, count);
  return true;
}

// @address 0x49ac80 parse_squad_join_status
extern "C" bool parse_squad_join_status(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  c_static_wchar_string<1024> text;
  text.m_string[0] = 0;
  long string_id = user_interface_remote_squad_is_joinable(&static_cast<c_gui_game_details *>(screen)->m_game_details)
      ? _game_details_string_joinable_yes
      : _game_details_string_joinable_no;
  screen->get_string_by_string_id(string_id, &text);
  ustrnzcpy(destination, text.m_string, count);
  return true;
}

// 0x49acf8
bool parse_party_player_count(void *screen, wchar_t *buffer, long size)
{
  const s_network_squad_status_data_game_details *details = &static_cast<c_gui_game_details *>(screen)->m_game_details.details;
  short count = details->slot_count_d + details->slot_count_c;
  usnzprintf(buffer, size, game_engine_number_format(), count);
  return true;
}

// 0x49ad40
bool parse_party_player_max(void *screen, wchar_t *buffer, long size)
{
  const s_network_squad_status_data_game_details *details = &static_cast<c_gui_game_details *>(screen)->m_game_details.details;
  short player_count = details->slot_count_d + details->slot_count_c;
  short maximum_player_count = player_count + details->slot_count_b + details->slot_count_a;
  usnzprintf(buffer, size, game_engine_number_format(), maximum_player_count);
  return true;
}


namespace {
static __forceinline bool gui_game_details_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    string->m_string[0] = 0;
    if (string_id != -1) {
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals) {
            long string_list_index = globals->global_strings.index;
            if (string_list_index != -1) {
                text_group_get_string(string_list_index, string_id, string);
                result = true;
            }
        }
    }
    return result;
}

}

// @address 0x49ada8 parse_first_player_name
extern "C" bool parse_first_player_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  ustrnzcpy(destination, static_cast<c_gui_game_details *>(screen)->m_game_details.details.players[0].configuration.host.player_name, count);
  return true;
}

// @address 0x49add8 parse_game_mode_name
extern "C" bool parse_game_mode_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  bool success = false;
  c_static_wchar_string<1024> text;
  switch (static_cast<c_gui_game_details *>(screen)->m_game_details.details.kind) {
  case 0:
    gui_game_details_global_string_get(_game_details_string_campaign, &text);
    break;
  case 2:
    gui_game_details_global_string_get(_game_details_string_custom_game, &text);
    break;
  case 1:
    gui_game_details_global_string_get(_game_details_string_matchmaking, &text);
    break;
  case 3:
    gui_game_details_global_string_get(_game_details_string_mapeditor, &text);
    break;
  case 4:
    gui_game_details_global_string_get(_game_details_string_theater, &text);
    break;
  default:
    gui_game_details_global_string_get(_string_id_unknown, &text);
    break;
  }
  if (text.length() > 0) {
    ustrnzcpy(destination, text.get_string(), count);
    success = true;
  }
  return success;
}

// @address 0x49af58 parse_game_status_text
extern "C" bool parse_game_status_text(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  c_static_wchar_string<1024> text;
  bool success = false;
  switch (static_cast<short>(static_cast<c_gui_game_details *>(screen)->m_game_details.details.variant)) {
  case 0:
    screen->get_string_by_string_id(_game_details_string_status_info_leaving, &text);
    break;
  case 2:
    screen->get_string_by_string_id(_game_details_string_status_info_in_pregame_lobby, &text);
    break;
  case 1:
    screen->get_string_by_string_id(_game_details_string_status_info_matchmaking, &text);
    break;
  case 3:
    switch (static_cast<c_gui_game_details *>(screen)->m_game_details.details.kind) {
    case 3:
      screen->get_string_by_string_id(_game_details_string_status_info_editing_map, &text);
      break;
    case 4:
      screen->get_string_by_string_id(_game_details_string_status_info_watching_a_film, &text);
      break;
    default:
      screen->get_string_by_string_id(_game_details_string_status_info_game_in_progress, &text);
      break;
    }
    break;
  case 4:
    screen->get_string_by_string_id(_game_details_string_status_info_in_postgame_lobby, &text);
    break;
  }
  if (text.length() > 0) {
    ustrnzcpy(destination, text.get_string(), count);
    success = true;
  }
  return success;
}

// 0x49b068
extern "C" bool parse_map_name_alias(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  return parse_map_name(screen, destination, count);
}

// @address 0x49b070 parse_difficulty_name
extern "C" bool parse_difficulty_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  c_static_wchar_string<1024> text;
  bool success = false;
  switch (static_cast<c_gui_game_details *>(screen)->m_game_details.details.campaign_difficulty) {
  case 0:
    gui_game_details_global_string_get(281, &text);
    break;
  case 1:
    gui_game_details_global_string_get(173, &text);
    break;
  case 2:
    gui_game_details_global_string_get(282, &text);
    break;
  case 3:
    gui_game_details_global_string_get(283, &text);
    break;
  default:
    gui_game_details_global_string_get(173, &text);
    break;
  }
  if (text.length() > 0) {
    ustrnzcpy(destination, text.get_string(), count);
    success = true;
  }
  return success;
}

// @address 0x49b190 parse_variant_name
extern "C" bool parse_variant_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  ustrnzcpy(destination, static_cast<c_gui_game_details *>(screen)->m_game_details.details.film_name, count);
  return true;
}

static __forceinline unsigned int gui_game_details_ustrnlen(const wchar_t *text, long capacity)
{
  unsigned int length;
  for (length = 0; length < (unsigned int)capacity && text[length] != 0; ++length) {}
  return length;
}

// @address 0x49b1c0 parse_description_text
extern "C" bool parse_description_text(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  copy_narrow_to_wide(static_cast<c_gui_game_details *>(screen)->m_game_details.details.hopper_name,
      (short *)destination, count, 0);
  if (gui_game_details_ustrnlen(destination, count) == 0) {
    c_static_wchar_string<1024> text;
    screen->get_string_by_string_id(_game_details_string_playlist_unknown, &text);
    if (text.length() > 0) {
      ustrnzcpy(destination, text.get_string(), count);
    }
  }
  return true;
}

// 0x49b270
bool parse_time_remaining(void *screen, wchar_t *buffer, long size)
{
  c_static_wchar_string<1024> text;
  bool success = false;
  if (user_interface_get_timespan_string(
      static_cast<c_gui_game_details *>(screen)->m_game_details.details.timer_seconds,
      1, 0, &text)) {
    text.copy_to(buffer, size);
    success = true;
  }
  return success;
}

// 0x49b2e0
extern "C" bool parse_first_detail_number(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  usnzprintf(destination, count, game_engine_number_format(), static_cast<c_gui_game_details *>(screen)->m_game_details.details.score_to_win_round);
  return true;
}

// 0x49b320
extern "C" bool parse_second_detail_number(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  usnzprintf(destination, count, game_engine_number_format(), static_cast<c_gui_game_details *>(screen)->m_game_details.details.best_player_score);
  return true;
}

#if !defined(_XBOX) || defined(__clang__)
// The Xbox image places this table at 0x33614 (see gui_screen_game_options.cpp); other builds define its contents.
const long k_game_engine_type_string_ids[11] = {
    _string_id_unknown, 0xe9, 0xeb, 0xec, 0xed, 0xf2, 0xef, 0xee, 0xf0, 0xea, 0xf1
};
#endif

// 0x49b360
extern "C" bool parse_campaign_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  c_static_wchar_string<1024> text;
  bool success = false;
  if (static_cast<c_gui_game_details *>(screen)->m_game_details.details.campaign_id != -1) {
    gui_game_details_global_string_get(_game_details_string_coop, &text);
  } else if (static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type >= 1
      && static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type <= 10) {
    long string_id = (unsigned long)static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type <= 10
        ? k_game_engine_type_string_ids[static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type]
        : _string_id_unknown;
    user_interface_global_string_get(string_id, (unsigned short *)text.m_string);
  } else {
    text.m_string[0] = 0;
  }
  if (text.length() > 0) {
    ustrnzcpy(destination, text.get_string(), count);
    success = true;
  }
  return success;
}

// @address 0x49b440 parse_campaign_detail_text
extern "C" bool parse_campaign_detail_text(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  ustrnzcpy(destination, static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_variant_name, count);
  return true;
}

// @address 0x49b470 parse_map_name
extern "C" bool parse_map_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  bool result = false;
  long campaign_id = static_cast<c_gui_game_details *>(screen)->m_game_details.details.campaign_id;
  levels_map_datum level;
  c_static_wchar_string<1024> text;
  if (campaign_id != -1) {
    long map_id = static_cast<c_gui_game_details *>(screen)->m_game_details.details.map_id;
    if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, &level))
      text.set(level.display_name);
  } else if (static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type >= 1
      && static_cast<c_gui_game_details *>(screen)->m_game_details.details.game_engine_type <= 10) {
    long map_id = static_cast<c_gui_game_details *>(screen)->m_game_details.details.map_id;
    if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, &level))
      text.set(level.display_name);
  }
  if (text.length() > 0) {
    ustrnzcpy(destination, text.get_string(), count);
    result = true;
  }
  return result;
}

// @address 0x49b528 parse_variant_short_name
extern "C" bool parse_variant_short_name(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  ustrnzcpy(destination, static_cast<c_gui_game_details *>(screen)->m_game_details.details.map_variant_name, count);
  return true;
}

// @address 0x49b558 parse_team_count
extern "C" bool parse_team_count(c_gui_screen_widget *screen, wchar_t *destination, long count)
{
  c_static_wchar_string<1024> text;
  bool success = false;
  if (static_cast<c_gui_game_details *>(screen)->m_game_details.details.teams_enabled == 0) {
    gui_game_details_global_string_get(_game_details_string_teams_info_none, &text);
    if (text.length() > 0) {
      ustrnzcpy(destination, text.get_string(), count);
      success = true;
    }
  } else {
    usnzprintf(destination, count, game_engine_number_format(), static_cast<c_gui_game_details *>(screen)->m_game_details.details.maximum_team_count);
    success = true;
  }
  return success;
}

// @address 0x4f3250 parse_lobby_delaying_player


extern "C" bool parse_lobby_delaying_player(void *, wchar_t *buffer, long size)
{
  bool success = false;
  if (::user_interface_squad_exists()) {
    long index = network_squad_session_get_countdown_delayed_culprit();
    if (index != -1) {
      const s_player_configuration *entry = ::user_interface_squad_get_player_data(index);
      if (entry) {
        ustrnzcpy(buffer, entry->host.player_name, size);
        success = true;
      }
    }
  }
  return success;
}

// 0x49a680 c_gui_game_details::create_list_widget
c_gui_list_widget *c_gui_game_details::create_list_widget(const s_list_widget_block *definition)
{
    if (definition->definition.core.name == _game_details_string_roster) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_roster_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_roster_list_widget(false, false);
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_list_widget;
    }
    return 0;
}

// @address 0x49a3e8 ?initialize_datasource@c_gui_game_details@@UAAXXZ
void c_gui_game_details::initialize_datasource()
{
  c_gui_screen_widget::initialize_datasource();
  c_gui_static_squad_roster_data *roster = (c_gui_static_squad_roster_data *)user_interface_malloc_tracked(sizeof(c_gui_static_squad_roster_data), 0, 0);
  if (roster) {
    roster = new (roster) c_gui_static_squad_roster_data;
    if (roster->initialize(_game_details_string_roster)) {
      const s_network_squad_status_data_game_details *details = &m_game_details.details;
      for (long i = 0; i < details->player_count; i++) {
        s_player_configuration *configuration = &m_configurations[i];
        memcpy(configuration, &details->players[i].configuration, sizeof(*configuration));
        roster
            ->add_player_internal(c_gui_roster_data::_player_row_player, -1, &details->players[i].identifier,
                (const s_player_configuration *)configuration, k_no_controller, c_gui_roster_data::_voice_not_talking, 0, false);
      }
      add_datasource(roster);
    } else {
      roster->~c_gui_static_squad_roster_data();
      user_interface_free(roster);
    }
  }
  c_gui_game_details_menu_datasource *menu =
      (c_gui_game_details_menu_datasource *)user_interface_malloc_tracked(sizeof(c_gui_game_details_menu_datasource), 0, 0);
  if (menu) {
    menu = new (menu) c_gui_game_details_menu_datasource;
    if (menu->initialize(_game_details_string_game_details_menu)) {
      const s_network_squad_status_data *data = &m_game_details;
      add_datasource(menu);
      menu->show_only_necessary_fields(data, get_any_responding_controller(), m_initial_state);
    } else {
      menu->~c_gui_game_details_menu_datasource();
      user_interface_free(menu);
    }
  }
  c_gui_game_details_info_datasource *info =
      (c_gui_game_details_info_datasource *)user_interface_malloc_tracked(sizeof(c_gui_game_details_info_datasource), 0, 0);
  if (info) {
    info = new (info) c_gui_game_details_info_datasource;
    if (info->initialize(_game_details_string_game_details_info)) {
      const s_network_squad_status_data *data = &m_game_details;
      add_datasource(info);
      info->show_only_necessary_fields(data, get_any_responding_controller(), m_initial_state);
    } else {
      info->~c_gui_game_details_info_datasource();
      user_interface_free(info);
    }
  }
}

// @address 0x49a710 ?handle_controller_input_message@c_gui_game_details@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_game_details::handle_controller_input_message(const c_controller_input_message *input) {
    bool handled = false;
    c_gui_widget *focused = get_focused_widget();
    c_gui_list_widget *parent = focused && focused->get_type() == _gui_list_item_widget ? gui_widget_get_parent_list_widget(focused) : 0;
    c_gui_list_widget *menu = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _game_details_string_game_details_menu);
    c_gui_list_widget *players = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _game_details_string_roster);
    const s_network_squad_status_data_game_details *details = &m_game_details.details;
    switch (input->get_event_type()) {
    case _event_type_tab_left:
        if (parent == players) {
            transfer_focus_to_list(
                    menu, (menu)->get_focused_element_handle(), true, false);
            handled = true;
        }
        break;
    case _event_type_tab_right:
        if (parent == menu && details->session_class == 2) {
            transfer_focus_to_list(
                    players, (players)->get_focused_element_handle(), true, false);
            handled = true;
        }
        break;
    case _event_type_button_press:
        if (input->get_component() == 0 && parent == players) {
            void *controller = &g_controller_globals.controllers[input->get_controller()];
            if (controller) {
                const s_player_identifier *identifier = &details->players[(players)->get_focused_element_handle()].identifier;
                const s_player_configuration *configuration = &details->players[(players)->get_focused_element_handle()].configuration;
                s_player_identifier local_identifier;
                ((const void *)((c_controller_interface const *)controller)->get_player_identifier(&local_identifier));
                if (identifier && configuration) {
                    void *storage = ::user_interface_malloc_tracked(sizeof(c_load_player_select_screen_message), (char const *)(0), 0);
                    if (storage) {
                        long name = tls_game_globals()->options.game_mode == 3
                            ? _game_details_string_player_select
                            : _game_details_string_scoreboard_player_select;
                        long screen_name = get_name();
                        long controller_index = input->get_controller();
                        long render_window = get_render_window();
                        c_load_player_select_screen_message *message = new (storage) c_load_player_select_screen_message(name,
                            (e_controller_index)controller_index, (e_window_index)render_window, screen_name,
                            &local_identifier, identifier, configuration);
                        handled = true;
                        message->set_parent_screen_index(get_screen_index());
                        ::user_interface_messaging_post(message);
                    }
                }
            }
        }
        break;
    }
    if (!(unsigned char)handled)
        return c_gui_screen_widget::handle_controller_input_message(input);
    return handled;
}
// @address 0x4797f8 ?user_interface_join_remote_session@@YAX_NW4e_network_session_class@@PBUs_transport_secure_identifier@@PBUs_transport_secure_address@@PBUs_transport_secure_key@@@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x49a970 ?handle_list_item_chosen@c_gui_game_details@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_game_details::handle_list_item_chosen(const c_controller_input_message *input, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    long action = -1;
    long selected_player = m_initial_state;
    const s_network_squad_status_player *player = &m_game_details.details.players[selected_player];
    unsigned __int64 xuid = player->configuration.client.xuid;
    if (list_name == _game_details_string_game_details_menu) {
        if (data->get_string_id_value(item->get_element_handle(), _game_details_string_options_list, &action)) {
            switch (action) {
            case _game_details_string_option_join: {
                const s_network_squad_status_data_game_details *details = &m_game_details.details;
                if (details->player_count < 16 && details->session_class != 0) {
                    user_interface_join_remote_session(false, (e_network_session_class)details->session_class,
                        &details->session_id, &details->secure_address,
                        &details->session_key);
                    transition_out_with_transition_type(0, _screen_transition_normal);
                } else {
                    ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                }
                handled = 1;
                break;
            }
            case _game_details_string_option_invite:
                if (!m_invite_task.busy()) {
                    c_invite_task *task = &m_invite_task;
                    task->reset();
                    m_invite_task.m_controller_index = input->get_controller();
                    m_invite_task.m_target_xuid = xuid;
                    m_invite_task.m_message_text[0] = 0;
                    overlapped_task_start_internal(&m_invite_task, "..\\interface\\gui_screens\\game_details\\gui_game_details.cpp", 681);
                    data->set_disabled_element(_game_details_string_name, _game_details_string_option_invite);
                    data->enable_element(_game_details_string_name, _game_details_string_option_cantinvite);
                    g_gui_error_manager.post_error_with_custom_message(0x2007e, 0, 0, k_any_controller, false, false);
                }
                handled = 1;
                break;
            case _game_details_string_option_message: {
                long controller = input->get_controller();
                unsigned __int64 target = xuid;
                if (g_controller_globals.controllers[controller].is_signed_in_to_live())
                    XShowMessageComposeUI(controller, &target, 1, 0);
                handled = 1;
                break;
            }
            }
        }
    }
    bool result = false;
    if ((unsigned char)handled) result = handled;
    return result;
}
