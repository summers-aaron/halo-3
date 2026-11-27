#ifndef NETWORKING_SESSION_NETWORK_SESSION_INTERFACE_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_INTERFACE_TYPES_H

#include <stddef.h>
#include "game/player_configuration.h"

#include "networking/session/network_quality.h"

enum e_network_interface_user_state
{
    _network_session_interface_user_state_none = 0,
    _network_session_interface_user_state_exists = 1,
    _network_session_interface_user_state_zombie = 2,
    _network_session_interface_user_state_error_first = 3,
    _network_session_interface_user_state_error_online_permissions = _network_session_interface_user_state_error_first,
    _network_session_interface_user_state_error_user_content_not_permitted = 4,
    _network_session_interface_user_state_error_generic = 5,
    _network_session_interface_user_state_error_game_in_matchmaking = 6,
    _network_session_interface_user_state_error_game_in_theater = 7,
    _network_session_interface_user_state_error_game_in_campaign = 8,
    _network_session_interface_user_state_error_game_full = 9,
    _network_session_interface_user_state_error_no_split_screen_on_alpha = 10,
    _network_session_interface_user_state_error_everybody_signed_out = 11,
    _network_session_interface_user_state_error_not_signed_in = 12,
    _network_session_interface_user_state_error_confirm_temporary = 13
};

struct s_network_session_interface_peer_properties {
    bool initialized;
    wchar_t name[16];
    wchar_t description[32];
    bool quality_valid;
    s_network_quality_data quality;
    long nat_flags_a;
    long nat_flags_b;
    unsigned long flags;
    unsigned short hopper_identifier;
    long hopper_error;
    bool disconnected;
    long map_id;
    long map_status;
    long map_progress;
    unsigned short map_hopper_identifier;
    char spare_after_map_hopper_identifier[6];
};
typedef char session_interface_peer_size[(sizeof(s_network_session_interface_peer_properties) == 0xb0) ? 1 : -1];

struct s_player_slot : s_network_session_interface_peer_properties {
    long state;
    unsigned char key[8];
    char spare_after_key[0x100 - 0xbc];
    unsigned __int64 xuid;
    char spare_after_xuid;
    bool online_enabled;
    char spare_after_online_enabled[0x188 - 0x10a];
};

typedef char player_slot_size[(sizeof(s_player_slot) == 0x188) ? 1 : -1];
typedef char player_slot_off_name[(offsetof(s_player_slot, name) == 0x2) ? 1 : -1];
typedef char player_slot_off_description[(offsetof(s_player_slot, description) == 0x22) ? 1 : -1];
typedef char player_slot_off_quality[(offsetof(s_player_slot, quality) == 0x64) ? 1 : -1];
typedef char player_slot_off_flags[(offsetof(s_player_slot, flags) == 0x8c) ? 1 : -1];
typedef char player_slot_off_state[(offsetof(s_player_slot, state) == 0xb0) ? 1 : -1];
typedef char player_slot_off_key[(offsetof(s_player_slot, key) == 0xb4) ? 1 : -1];
typedef char player_slot_off_xuid[(offsetof(s_player_slot, xuid) == 0x100) ? 1 : -1];
typedef char player_slot_off_online_enabled[(offsetof(s_player_slot, online_enabled) == 0x109) ? 1 : -1];

extern "C" s_player_slot g_network_session_interface_local_users[];

// User records start at +0xb0 in the interface globals (the peer header overlaps
// the first record). This is distinct from the peer-based s_player_slot above.
struct s_network_session_interface_user_record
{
    long state;
    unsigned char key[8];
    long controller_index;
    s_player_configuration configuration;
    long change_count;
    char text[64];
    unsigned long voice_settings;
    long desired_team_session_id;
    long desired_team_index;
    long update_time[3];
    long remove_time[3];
};
typedef char session_interface_user_size[(sizeof(s_network_session_interface_user_record) == 0x188) ? 1 : -1];
typedef char session_interface_user_configuration_offset[(offsetof(s_network_session_interface_user_record, configuration) == 0x10) ? 1 : -1];
typedef char session_interface_user_text_offset[(offsetof(s_network_session_interface_user_record, text) == 0x124) ? 1 : -1];
#endif // NETWORKING_SESSION_NETWORK_SESSION_INTERFACE_TYPES_H
