#ifndef NETWORKING_SESSION_NETWORK_SESSION_JOIN_REQUEST_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_JOIN_REQUEST_TYPES_H

#include "game/player_types.h"
#include "networking/transport/transport_security.h"
#include <stddef.h>

// The join request a peer sends to a session's host (s_network_message_join_request::join_request; March encode
// asserts "message->join_request.joining_peer_count"; member names from that encoder's stream labels). No assert names
// its header: kept beside the session that holds it in m_local_state_data (location dispute). 0x878 bytes.
struct s_network_session_join_request_peer                  // 0x38
{
    s_transport_secure_address address;                     // "joining-peer-address"
    long network_version;                                   // "joining-network-version-number"
    long player_indices[4];                                 // "user-player-index" per controller; NONE: "joining-peer-player-valid" false
};
struct s_network_session_join_request_player                // 0x40
{
    s_player_identifier player_identifier;                  // "joining-peer-player-id"
    unsigned __int64 player_xuid;                           // "joining-peer-player-xuids"
    bool is_silver_or_gold_live;                            // "joining-peer-is-silver-or-gold-live"
    bool is_online_enabled;                                 // "joining-peer-is-online-enabled"
    bool is_guest;                                          // network_session.cpp copies the player's client.guest
    bool is_bot;                                            // network_session.cpp copies the player's client.bot
    s_queried_player_global_statistics global_statistics;
    s_queried_player_hopper_statistics hopper_statistics;
};
struct s_network_session_join_request_payload               // 0xdc (March type name: join_remote_session's argument)
{
    long payload_type;                                      // "payload-type"
    char payload_data[0xdc - 4];                            // the payload_type's data (raw until typed)
};
struct s_network_session_join_request
{
    unsigned __int64 join_nonce;                            // "join-nonce"
    unsigned __int64 join_party_nonce;                      // "join-party-nonce"
    long joining_peer_count;                                // March assert
    s_network_session_join_request_peer joining_peers[16];
    long joining_player_count;                              // "joining-player-count"
    s_network_session_join_request_player joining_players[16];
    bool join_from_recruiting;                              // "join-from-recruiting"
    s_network_session_join_request_payload payload;
};
typedef char network_session_join_request_size_check[(sizeof(s_network_session_join_request) == 0x878) ? 1 : -1];

typedef char network_session_join_peer_size[(sizeof(s_network_session_join_request_peer) == 0x38) ? 1 : -1];
typedef char network_session_join_player_size[(sizeof(s_network_session_join_request_player) == 0x40) ? 1 : -1];
typedef char network_session_join_payload_size[(sizeof(s_network_session_join_request_payload) == 0xdc) ? 1 : -1];
typedef char network_session_join_peers_offset[(offsetof(s_network_session_join_request, joining_peers) == 0x14) ? 1 : -1];
typedef char network_session_join_players_offset[(offsetof(s_network_session_join_request, joining_players) == 0x398) ? 1 : -1];
typedef char network_session_join_payload_offset[(offsetof(s_network_session_join_request, payload) == 0x79c) ? 1 : -1];

#endif
