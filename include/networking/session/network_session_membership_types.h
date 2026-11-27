#ifndef NETWORKING_SESSION_NETWORK_SESSION_MEMBERSHIP_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_MEMBERSHIP_TYPES_H

#include "game/player_types.h"
#include <stddef.h>

// Membership player shared by the session, memory pool, and simulation watcher.
struct s_network_session_player
{
    long desired_configuration_version;            // March set_player_properties' second argument
    s_player_identifier identifier;                // March get_player_identifier
    long peer_index;                               // March is_player_established
    long peer_user_index;
    long player_sequence_number;
    bool left_game;
    long controller_index;                         // March set_player_properties
    s_player_configuration configuration;          // March get_player_data
    unsigned long voice_settings;                  // March set_player_properties' last argument
};

typedef char network_session_player_size[(sizeof(s_network_session_player) == 0x138) ? 1 : -1];
typedef char network_session_player_identifier_offset[(offsetof(s_network_session_player, identifier) == 4) ? 1 : -1];
typedef char network_session_player_peer_offset[(offsetof(s_network_session_player, peer_index) == 0xc) ? 1 : -1];
typedef char network_session_player_controller_offset[(offsetof(s_network_session_player, controller_index) == 0x1c) ? 1 : -1];
typedef char network_session_player_configuration_offset[(offsetof(s_network_session_player, configuration) == 0x20) ? 1 : -1];
typedef char network_session_player_voice_offset[(offsetof(s_network_session_player, voice_settings) == 0x130) ? 1 : -1];

#endif
