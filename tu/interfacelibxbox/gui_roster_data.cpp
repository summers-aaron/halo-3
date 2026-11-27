// interfacelibxbox/gui_roster_data.obj: from the records 0x4e8f78 0x4e9028 0x4e9160 0x4e9328 0x4e9388 0x4e93f0 0x4e9488 0x4e96d0 0x4e9898 0x4e9a48 0x4e9c18 0x4e9ce8 0x4ea078 0x4ea1d8 0x4ea2f0 0x4ea3d0 0x4ea578
// @compiler xdk6130
// @flags /TP /O2 /Oc

// 14 ranks by 4 grades of the rank icon element handles

#include "cseries/maximum.h"
#include "interface/user_interface_matchmaking_progress.h"
#include <stddef.h>
#include "memory/static_arrays.h"
#include <stdlib.h>
#include "interface/gui_data.h"
#include <string.h>
#include "interface/user_interface.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/voice_xenon.h"
#include "interface/gui_roster_data.h"
#include "interface/user_interface_utilities.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "networking/session/network_session_types.h"
#include "saved_games/content_catalogue.h"
#include "networking/voice_processing_mode_types.h"
#include "interface/user_interface_text.h"
#include "networking/online/voice_interface_types.h"
#include "networking/online/voice_xenon_inline.h"
#include "text/text_group.h"
#include "game/player_configuration.h"
#include "interface/user_interface_shared_globals.h"
#include "networking/session/network_session_membership_types.h"
#include "game/player_identifier_types.h"
#include "interface/controller_inline.h"
#include "networking/session/network_session.h"
#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_searching.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"

class c_gui_selected_item;

struct s_network_session_peer;

class IXAudioVoice;

class IXAudioSourceVoice;

class c_network_session;

enum e_session_game_start_status;

enum e_session_game_start_error;

typedef char gui_roster_configuration_offset[(offsetof(c_gui_roster_data::s_player_row, configuration) == 0x18) ? 1 : -1];

typedef char gui_roster_team_offset[(offsetof(c_gui_roster_data::s_player_row, team_index) == 0x12c) ? 1 : -1];

typedef char gui_roster_flags_offset[(offsetof(c_gui_roster_data::s_player_row, flags) == 0x15c) ? 1 : -1];

typedef char gui_roster_row_size[(sizeof(c_gui_roster_data::s_player_row) == 0x168) ? 1 : -1];

typedef char gui_roster_voice_settings_offset[(sizeof(void *) != 4 || ((offsetof(s_voice_player_state, settings) == 0x20) )) ? 1 : -1];

typedef char gui_roster_speaker_size[(sizeof(void *) != 4 || ((sizeof(s_voice_player_speaker_state) == 8) )) ? 1 : -1];

// @address 0x42d298 ?is_busy@c_gui_data@@UAA_NXZ
// @address 0x1c1258 ?add_element@c_gui_data@@UAAJXZ
// @address 0x4e9020 ?get_current_item_count_internal@c_gui_roster_data@@MAAJXZ
// @address 0x4e8f78 ??0c_gui_roster_data@@IAA@W4e_controller_index@@@Z
c_gui_roster_data::c_gui_roster_data(e_controller_index controller_index) :
    m_wanted_player_slot_count(0),
    m_party_player_slot_count(0),
    m_player_count(0),
    m_controller_index(controller_index),
    m_player_list_update_paused(false)
{
    memset(m_players, 0, sizeof(m_players));
}

// @address 0x56468 ??_7c_gui_squad_roster_data@@6B@
// @address 0x4c0ee0 ??_Gc_gui_squad_roster_data@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_squad_roster_data@@UAAPAXI@Z
c_gui_squad_roster_data::c_gui_squad_roster_data(e_controller_index controller_index) :
    c_gui_roster_data(controller_index)
{
}

// @address 0x4e9028 ?add_player_internal@c_gui_roster_data@@IAAPAUs_player_row@1@W4e_player_row_type@1@JPBUs_player_identifier@@PBUs_player_configuration@@W4e_controller_index@@W4e_voice_talking_state@1@J_N@Z
c_gui_roster_data::s_player_row *c_gui_roster_data::add_player_internal(
    e_player_row_type row_type, long player_index, const s_player_identifier *identifier,
    const s_player_configuration *configuration, e_controller_index controller,
    e_voice_talking_state voice, long flags, bool use_team_overrides) {
    s_player_row *result = 0;
    bool space_available = m_player_count < 16;
    if (space_available) {
        s_player_row *row = &m_players[m_player_count];
        memset(row, 0, sizeof(*row));
        row->type = row_type;
        row->player_index = player_index;
        row->team_index = 0;
        row->game_team_index = 0;
        if (identifier) {
            row->identifier = *identifier;
            row->identifier_valid = true;
        }
        if (configuration) {
            row->configuration = *configuration;
            row->configuration_valid = true;
            if (use_team_overrides) {
                row->team_index = configuration->host.hopper_stats_valid ? configuration->host.hopper_skill_display : -1;
                row->game_team_index = configuration->host.global_stats_valid ? configuration->host.global_experience : -1;
            }
        }
        row->controller_index = controller;
        row->voice = voice;
        row->state = 0;
        row->flags = flags;
        ++m_player_count;
        result = row;
    }
    return result;
}

// @address 0x4e9160 ?get_column_names@c_gui_roster_data@@UAAXQAJPAJ@Z
void c_gui_roster_data::get_column_names(long *const column_names, long *column_count) {
    *column_count = 0;
    column_names[*column_count] = _roster_data_column_type;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_player_index;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_team_index;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_base_color;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_controller;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_voice;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_button_x;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_name;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_service_tag;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_press_a_to_join;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_looking_for_player;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_player_found;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_rank;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_skill;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_state;
    ++*column_count;
    column_names[*column_count] = _roster_data_column_disabled;
    ++*column_count;
}

// @address 0x4e9328 ?get_player_identifier@c_gui_roster_data@@UAA_NJPAUs_player_identifier@@@Z
bool c_gui_roster_data::get_player_identifier(long element_handle, s_player_identifier *identifier) {
    bool result = false;
    if (element_handle >= 0 && element_handle < m_player_count) {
        s_player_row *row = &m_players[element_handle];
        if (row->identifier_valid) {
            *identifier = row->identifier;
            result = true;
        }
    }
    return result;
}

// @address 0x4e9388 ?get_player_configuration@c_gui_roster_data@@UAA_NJPAUs_player_configuration@@@Z
bool c_gui_roster_data::get_player_configuration(long element_handle, s_player_configuration *configuration) {
    bool result = false;
    if (element_handle >= 0 && element_handle < m_player_count) {
        s_player_row *row = &m_players[element_handle];
        if (row->configuration_valid) {
            *configuration = row->configuration;
            result = true;
        }
    }
    return result;
}

// @address 0x4e93f0 ?get_player_appearance@c_gui_roster_data@@UAA_NJPAUs_player_appearance@@@Z
bool c_gui_roster_data::get_player_appearance(long element_handle, s_player_appearance *appearance) {
    s_player_configuration configuration;
    if (get_player_configuration(element_handle, &configuration)) {
        *appearance = configuration.client.appearance;
        return true;
    }
    return false;
}

static __forceinline long gui_roster_data_get_player_rank_image_index(unsigned long rank, unsigned long grade) {
    long image_index = -1;
    if (rank <= 13 && grade <= 3) {
        image_index = g_experience_rank_icon_element_handles[rank][grade];
    }
    return image_index;
}

// @address 0x4e9488 ?get_integer_value@c_gui_roster_data@@UAA_NJJPAJ@Z
bool c_gui_roster_data::get_integer_value(long element_handle, long value_name, long *value) {
    bool result = false;
    if (element_handle >= 0 && element_handle < m_player_count) {
        s_player_row *row = &m_players[element_handle];
        switch (value_name) {
        case _roster_data_column_player_index:
            *value = row->player_index;
            result = true;
            break;
        case _roster_data_column_team_index:
            *value = row->configuration.host.team_index;
            result = true;
            break;
        case _roster_data_column_controller:
            *value = row->controller_index;
            result = true;
            break;
        case _roster_data_column_base_color:
            *value = row->configuration.client.appearance.change_colors[0];
            result = true;
            break;
        case _roster_data_column_voice:
            *value = row->voice;
            result = true;
            break;
        case _roster_data_column_type:
            *value = row->type;
            result = true;
            break;
        case _roster_data_column_disabled:
            *value = row->flags;
            result = true;
            break;
        case _roster_data_column_rank: {
            long rank_image_index = -1;
            if (row->configuration.host.global_stats_valid) {
                rank_image_index = gui_roster_data_get_player_rank_image_index(row->configuration.host.global_rank, row->configuration.host.global_grade);
            }
            *value = rank_image_index;
            result = true;
            break;
        }
        case _roster_data_column_skill:
            *value = row->team_index;
            result = true;
            break;
        case _roster_data_column_state:
            *value = row->state;
            result = true;
            break;
        case _roster_data_column_bungienet_user:
            *value = row->configuration.client.bungienet_user;
            result = true;
            break;
        }
    }
    return result;
}

static __forceinline bool gui_roster_data_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    string->m_string[0] = 0;
    if (string_id != -1) {
        const s_user_interface_shared_globals *globals = ((const s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
        if (globals != 0) {
            long string_list = globals->global_strings.index;
            if (string_list != -1)
                result = text_group_get_string(string_list, string_id, string);
        }
    }
    return result;
}

// @address 0x4e96d0 ?get_text_value@c_gui_roster_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_roster_data::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_player_count) {
        s_player_row *row = &m_players[element_handle];
        if (value_name <= _roster_data_column_looking_for_player) {
            if (value_name != _roster_data_column_looking_for_player) {
                if (value_name != _roster_data_column_press_a_to_join) {
                    if (value_name == _roster_data_column_name && row->type == _player_row_player) {
                        value->set(row->configuration.host.name);
                        result = true;
                    }
                }
                else if (row->type == 3) {
                    gui_roster_data_global_string_get(_roster_data_column_press_a_to_join, value);
                    result = true;
                }
            }
            else if (row->type == 2) {
                gui_roster_data_global_string_get(_roster_data_column_looking_for_player, value);
                result = true;
            }
        }
        else {
            if ((unsigned long)(value_name - _roster_data_column_player_found) != 0) {
                if ((unsigned long)(value_name - _roster_data_column_player_found) == 8 && row->type == _player_row_player) {
                unsigned char model_choice = row->configuration.client.appearance.player_model_choice;
                c_static_wchar_string<1024> rank_name;
                if (model_choice == 1)
                    result = gui_roster_data_global_string_get(198, &rank_name);
                else
                    result = gui_roster_data_global_string_get(197, &rank_name);
                if (result) {
                    c_static_wchar_string<256> service_tag;
                    service_tag.set(row->configuration.client.appearance.service_tag);
                    value->print(L"%s - %s", rank_name.m_string, service_tag.m_string);
                    result = true;
                }
                }
            }
            else if (row->type == 1) {
                gui_roster_data_global_string_get(_roster_data_column_player_found, value);
                result = true;
            }
        }
    }
    return result;
}

static __forceinline void gui_roster_data_add_slot(c_gui_roster_data *roster, int type)
{
    int n = roster->m_player_count;
    unsigned char ok;
    c_gui_roster_data::s_player_row *s;
    ok = (n < 16) ? 1 : 0;
    if (ok) {
        s = &roster->m_players[n];
        memset(s, 0, sizeof(*s));
        s->type = (c_gui_roster_data::e_player_row_type)type;
        s->player_index = -1;
        s->team_index = 0;
        s->game_team_index = 0;
        s->controller_index = _controller_none;
        s->voice = c_gui_roster_data::_voice_not_talking;
        s->state = 0;
        s->flags = 0;
        roster->m_player_count = roster->m_player_count + 1;
    }
}

// @address 0x4e9a48 ?update_matchmaking_slots@c_gui_roster_data@@IAAXXZ
void c_gui_roster_data::update_matchmaking_slots(void)
{
    s_life_cycle_matchmaking_progress progress;
    int lo, hi, kind;
    int a, b;
    unsigned int i;
    if (!network_life_cycle_in_matchmaking()) return;
    network_life_cycle_get_matchmaking_progress(&progress);
    g_user_interface_matchmaking_progress = progress;
    lo = progress.squad_player_count;
    hi = (progress.group_player_count > lo) ? progress.group_player_count : lo;
    kind = progress.type;
    a = 0;
    b = 0;
    switch (kind) {
    case 0:
        a = m_wanted_player_slot_count;
        b = m_party_player_slot_count;
        break;
    case 1: case 2:
        a = progress.search_criteria.want_player_count - hi;
        b = hi - lo;
        break;
    case 3:
        a = 0;
        b = hi - lo;
        break;
    }
    m_wanted_player_slot_count = a;
    m_party_player_slot_count = b;
    {
        for (i = 0; (int)i < b; i++) gui_roster_data_add_slot(this, 1);
    }
    {
        for (i = 0; (int)i < a; i++) gui_roster_data_add_slot(this, 2);
    }
}

extern "C" {
// @address 0x4e9c18 gui_roster_data_merge_duplicate_rows
void gui_roster_data_merge_duplicate_rows(c_gui_roster_data *roster)
{
    c_gui_roster_data::s_player_row *leader;
    int match;
    long index;
    c_gui_roster_data::s_player_row *current;
    unsigned char same_id;

    leader = 0;
    for (index = 0; index < roster->m_player_count; ++index)
    {
        current = &roster->m_players[index];
        if (leader != 0)
        {
            match = 0;
            if (current->type == 0 && leader->type == 0)
            {
                same_id = (current->party_nonce == leader->party_nonce) ? 1 : 0;
                match = (int)same_id;
                if (current->show_teams)
                {
                    match = (current->configuration.host.team_index == leader->configuration.host.team_index) & match;
                }
            }
            if ((unsigned char)match != 0)
            {
                current->state = 0;
                leader->state++;
                continue;
            }
        }
        leader = current;
        current->state = 1;
    }
}
}

// @address 0x4ea078 gui_roster_data_update_row_session_data
extern "C" void gui_roster_data_update_row_session_data(c_gui_roster_data::s_player_row *row)
{
    int squad_leader_index;
    int row_player_index;
    int saved_leader_index;
    int truncated_row_index;
    const s_player_configuration *leader_player_configuration;
    int session_peer_index;
    int session_sequence_number;
    int leader_team_index;
    squad_leader_index = ::user_interface_squad_get_leader_player_index();
    row_player_index = row->player_index;
    saved_leader_index = squad_leader_index;
    session_peer_index = -1;
    truncated_row_index = (unsigned short)row_player_index;
    {
        const s_network_session_player *players;
        unsigned long player_mask;
        if (network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, 0, &player_mask, &players))
        {
            if ((1 << truncated_row_index) & player_mask)
                session_peer_index = players[truncated_row_index].peer_index;
        }
    }
    row->peer_index = session_peer_index;
    session_sequence_number = -1;
    {
        unsigned long player_mask = 0;
        const s_network_session_player *players = 0;
        if (network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, 0, &player_mask, &players))
        {
            if ((1 << row_player_index) & player_mask)
                session_sequence_number = players[row_player_index].player_sequence_number;
        }
    }
    row->player_sequence_number = session_sequence_number;
    row->is_squad_leader = false;
    row->leader_team_index = -1;
    if (saved_leader_index != -1)
    {
        leader_player_configuration = ::user_interface_squad_get_player_data(saved_leader_index);
        if (leader_player_configuration != 0)
            leader_team_index = leader_player_configuration->host.team_index;
        else
            leader_team_index = -1;
        row->leader_team_index = leader_team_index;
        row->is_squad_leader = (saved_leader_index - row_player_index) == 0;
    }
    row->show_teams = ::user_interface_networking_should_show_teams();
}

extern "C" {

// @address 0x4ea2f0 gui_roster_data_add_joinable_controllers
void gui_roster_data_add_joinable_controllers(c_gui_roster_data *roster)
{
    if (!user_interface_interactive_session_is_squad())
        return;
    if (user_interface_squad_is_joinable() != _network_join_refuse_reason_none)
        return;

    for (long controller_index = 0; controller_index < 4; ++controller_index)
    {
        const c_controller_interface *controller = controller_get((e_controller_index)controller_index);
        if (!controller->is_attached())
            continue;
        if (!user_interface_squad_can_user_join(controller->user_index))
            continue;
        if (roster->m_player_count >= 16)
            continue;

        c_gui_roster_data::s_player_row *row = &roster->m_players[roster->m_player_count];
        memset(row, 0, sizeof(*row));
        row->type = c_gui_roster_data::_player_row_joinable_controller;
        row->player_index = -1;
        row->team_index = 0;
        row->game_team_index = 0;
        row->controller_index = (e_controller_index)controller_index;
        row->voice = c_gui_roster_data::_voice_not_talking;
        row->state = 0;
        row->flags = 0;
        ++roster->m_player_count;
    }
}
}

static __forceinline bool gui_roster_data_compare_long(long a, long b, long *result)
{
    bool equal = true;
    if (a < b)
    {
        *result = -1;
        equal = false;
    }
    else if (a > b)
    {
        *result = 1;
        equal = false;
    }
    return equal;
}

static __forceinline bool gui_roster_data_compare_long_descending(long a, long b, long *result)
{
    bool equal = true;
    if (b < a)
    {
        *result = -1;
        equal = false;
    }
    else if (b > a)
    {
        *result = 1;
        equal = false;
    }
    return equal;
}

extern "C" {
// @address 0x4ea3d0 gui_roster_data_compare_squad_rows
long gui_roster_data_compare_squad_rows(const c_gui_roster_data::s_player_row *a, const c_gui_roster_data::s_player_row *b)
{
    long result = 0;
    bool equal = gui_roster_data_compare_long(a->type, b->type, &result);
    if (equal && a->type != 0)
    {
        result = 0;
        equal = false;
    }
    if (equal)
    {
        if (a->is_squad_leader && !b->is_squad_leader)
        {
            result = -1;
            equal = false;
        }
        else if (b->is_squad_leader)
        {
            result = 1;
            equal = false;
        }
    }
    if (a->show_teams)
    {
        if (equal)
        {
            if (a->configuration.host.team_index == a->leader_team_index && b->configuration.host.team_index != b->leader_team_index)
            {
                result = -1;
                equal = false;
            }
            else if (a->configuration.host.team_index != a->leader_team_index && b->configuration.host.team_index == b->leader_team_index)
            {
                result = 1;
                equal = false;
            }
        }
        if (equal)
            equal = gui_roster_data_compare_long(a->configuration.host.team_index, b->configuration.host.team_index, &result);
    }
    if (equal)
        equal = gui_roster_data_compare_long(a->player_sequence_number, b->player_sequence_number, &result);
    if (equal)
        equal = gui_roster_data_compare_long_descending(a->peer_index, b->peer_index, &result);
    if (equal)
        return wcsncmp(a->configuration.host.player_name, b->configuration.host.player_name, 16);
    return result;
}
}

extern "C" {
// @address 0x4ea578 gui_roster_data_compare_group_rows
long gui_roster_data_compare_group_rows(const c_gui_roster_data::s_player_row *a, const c_gui_roster_data::s_player_row *b)
{
    long result = 0;
    bool equal = true;
    gui_roster_data_compare_long(a->type, b->type, &result);
    if (a->type != 0)
    {
        result = 0;
        equal = false;
    }
    if (a->show_teams && equal)
        equal = gui_roster_data_compare_long(a->configuration.host.team_index, b->configuration.host.team_index, &result);
    if (equal)
    {
        equal = gui_roster_data_compare_long(a->party_skill, b->party_skill, &result);
        result = -result;
    }
    if (equal)
    {
        equal = gui_roster_data_compare_long(a->team_index, b->team_index, &result);
        result = -result;
    }
    if (equal)
    {
        equal = gui_roster_data_compare_long(a->party_experience, b->party_experience, &result);
        result = -result;
    }
    if (equal)
    {
        equal = gui_roster_data_compare_long(a->game_team_index, b->game_team_index, &result);
        result = -result;
    }
    if (equal)
        equal = gui_roster_data_compare_long((long)a->party_nonce, (long)b->party_nonce, &result);
    if (equal)
        return wcsncmp(a->configuration.host.player_name, b->configuration.host.player_name, 16);
    return result;
}
}

// --- voice interface globals, declared exactly as the accepted unit
// tu/networkinglibxbox/voice_xenon.cpp declares them (same mangled symbol).

static __forceinline long gui_roster_data_voice_get_session(void) {
    long session = 0;
    if (g_voice_interface_globals.initialized) session = g_voice_interface_globals.matchmaking_state;
    return session;
}
static __forceinline bool gui_roster_data_voice_player_away_in_private_chat(long player_index) {
    bool away = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long flags = voice_get_player_flags(player_index);
        away = (flags & 8) != 0;
    }
    return away;
}
// @address 0x4e9898 ?get_voice_for_player@c_gui_roster_data@@SA?AW4e_voice_talking_state@1@W4e_controller_index@@J@Z
c_gui_roster_data::e_voice_talking_state c_gui_roster_data::get_voice_for_player(e_controller_index controller_index, long player_index)
{
    e_voice_talking_state voice = _voice_not_talking;
    if (gui_roster_data_voice_get_session() == 1 || gui_roster_data_voice_get_session() == 2) {
        bool communication = false;
        if (controller_index == 255) {
            if (voice_any_local_player_can_communicate(player_index)) communication = true;
        } else {
            if (controller_index != -1) {
                if (voice_controller_can_communicate(controller_index, player_index)) communication = true;
            }
        }
        if (communication) {
            if (gui_roster_data_voice_player_away_in_private_chat(player_index)) voice = _voice_private_chat;
            if (voice_player_has_voice(player_index)) {
                if (voice == _voice_not_talking)
                    voice = voice_player_is_talking(player_index) ? _voice_talking : _voice_available;
                if (controller_index == 255) {
                    if (voice_any_local_player_muting(player_index)) voice = _voice_muted;
                } else {
                    if (controller_index != -1) {
                        long local_player_index = voice_get_controller_player_index(controller_index);
                        if (local_player_index != -1 && voice_get_mute(local_player_index, player_index)) voice = _voice_muted;
                    }
                }
            }
        }
    }
    return voice;
}

static __forceinline bool gui_roster_data_rows_match(const c_gui_roster_data::s_player_row *row, const c_gui_roster_data::s_player_row *other)
{
    bool match = false;
    if (other->type == 0 && row->type == 0) {
        match = other->party_nonce == row->party_nonce;
        if (other->show_teams)
            match = match & (other->configuration.host.team_index == row->configuration.host.team_index);
    }
    return match;
}

// @address 0x4ea1d8 gui_roster_data_compute_team_maxima
extern "C" void gui_roster_data_compute_team_maxima(c_gui_roster_data *roster)
{
    for (long index = 0; index < roster->m_player_count; ++index) {
        c_gui_roster_data::s_player_row *row = &roster->m_players[index];
        c_gui_roster_data::e_player_row_type row_type = row->type;
        roster->m_players[index].party_skill = 0;
        if (row_type == 0) {
            for (long other_index = 0; other_index < roster->m_player_count; ++other_index) {
                c_gui_roster_data::s_player_row *other = &roster->m_players[other_index];
                if (gui_roster_data_rows_match(row, other)) {
                    roster->m_players[index].party_skill =
                        roster->m_players[index].party_skill > other->team_index ? roster->m_players[index].party_skill : other->team_index;
                    roster->m_players[index].party_experience = roster->m_players[index].party_experience > other->game_team_index
                        ? roster->m_players[index].party_experience
                        : other->game_team_index;
                }
            }
        }
    }
}

static __forceinline bool gui_roster_data_session_established(const c_network_session *session)
{
    return (unsigned long)session->m_local_state >= 4 &&
           (unsigned long)session->m_local_state < 10;
}

static __forceinline bool gui_roster_data_get_established_interactive_session(c_network_session **session)
{
    return user_interface_get_interactive_session(session) && gui_roster_data_session_established(*session);
}

static __forceinline bool gui_roster_data_interactive_session_is_squad_1(void)
{
    bool result = false;
    c_network_session *session = 0;
    if (user_interface_session_get_usable_player_xuid(&session))
        result = session->m_session_type == 1;
    return result;
}

static __forceinline bool gui_roster_data_interactive_session_is_squad_2(void)
{
    bool result = false;
    c_network_session *session = 0;
    if (gui_roster_data_get_established_interactive_session(&session))
        result = session->m_session_type == 1;
    return result;
}

// @address 0x4e9ce8 ?update@c_gui_squad_roster_data@@UAAXXZ
void c_gui_squad_roster_data::update()
{
    s_life_cycle_matchmaking_progress progress;
    if (network_life_cycle_in_matchmaking())
    {
        network_life_cycle_get_matchmaking_progress(&progress);
        g_user_interface_matchmaking_progress = progress;
    }

    bool pause = false;
    pause = pause || m_player_list_update_paused;
    pause = pause || (!user_interface_interactive_session_is_squad() && !user_interface_interactive_session_is_group());
    long progress_state = progress.type;
    pause = pause || (network_life_cycle_in_matchmaking() && progress_state == 0);
    // match: 0x4e9ce8: const reference binding preserves temporary lifetime
    const unsigned long &pause_value = (unsigned char)pause;
    if (pause_value != 0)
        return;

    unsigned long error_players = 0;
    if (network_life_cycle_in_matchmaking() && progress_state == 5 && progress.rematch_requested)
    {
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if (user_interface_session_is_local_peer_player(player_index))
            {
                user_interface_session_is_silver_or_gold_live(player_index);
                break;
            }
        }
    }

    m_player_count = 0;
    long game_start_status = (long)user_interface_get_session_game_start_status(0, &error_players);
    unsigned long rematch_party_players = progress.rematch_party_player_mask;
    unsigned long rematch_requested_players = progress.rematch_requested_player_mask;
    unsigned long rematch_team_players = progress.rematch_team_player_mask;
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        if (user_interface_session_is_player_valid(player_index))
        {
            long special_status = 0;
            e_controller_index controller_index = _controller_none;
            if (gui_roster_data_interactive_session_is_squad_1())
            {
                if (game_start_status == 7 && ((1 << player_index) & error_players) != 0)
                    special_status = 1;
            }
            else if (progress_state == 5)
            {
                if (((1 << player_index) & rematch_requested_players) != 0)
                    special_status = 3;
                else if (((1 << player_index) & rematch_team_players) != 0)
                    special_status = 2;
                else if (((1 << player_index) & rematch_party_players) != 0)
                    special_status = 4;
            }

            if (user_interface_session_is_local_peer_player(player_index))
                controller_index = (e_controller_index)user_interface_session_get_controller_index(player_index);

            c_gui_roster_data::s_player_row *row = add_player_internal(
                c_gui_roster_data::_player_row_player,
                player_index,
                user_interface_session_is_local_player(player_index),
                user_interface_session_get_player_data(player_index),
                controller_index,
                c_gui_roster_data::get_voice_for_player(m_controller_index, player_index),
                special_status,
                user_interface_networking_has_found_game());
            if (row != 0)
            {
                if (user_interface_interactive_session_is_squad())
                {
                    gui_roster_data_update_row_session_data(row);
                }
                else
                {
                    row->party_nonce = user_interface_session_is_silver_or_gold_live(row->player_index);
                    row->show_teams = user_interface_networking_should_show_teams();
                }
            }
        }
    }

    gui_roster_data_add_joinable_controllers(this);
    update_matchmaking_slots();
    int (*sort_proc)(const void *, const void *);
    if (gui_roster_data_interactive_session_is_squad_2())
    {
        sort_proc = (int (*)(const void *, const void *))gui_roster_data_compare_squad_rows;
    }
    else
    {
        gui_roster_data_compute_team_maxima(this);
        sort_proc = (int (*)(const void *, const void *))gui_roster_data_compare_group_rows;
    }
    qsort(m_players, m_player_count, sizeof(c_gui_roster_data::s_player_row), sort_proc);
    gui_roster_data_merge_duplicate_rows(this);
}

