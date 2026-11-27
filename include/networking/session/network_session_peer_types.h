#ifndef NETWORKING_SESSION_NETWORK_SESSION_PEER_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PEER_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "networking/transport/transport_security.h"

enum e_network_session_peer_state;
enum e_network_session_peer_properties_status_flags;

// s_network_session_peer_properties::connectivity (network_session_interface.cpp's names)
struct s_network_session_peer_connectivity
{
    unsigned short peer_connectivity_mask;
    unsigned short peer_probe_mask;
    long latency_minimum_msec;
    long latency_average_msec;
    long latency_maximum_msec;
};

// What a peer says about itself (set_peer_properties compares and copies it whole), 0xc0 bytes (March 0xa8).
struct s_network_session_peer_properties
{
    wchar_t name[16];                              // March set_peer_properties ustrcmp's it first
    wchar_t session_name[32];
    unsigned long map_content_mask;
    long map_id;
    long map_status;
    long map_progress;
    unsigned __int64 id;
    long hopper_identifier;                        // (multiplayer_game_hopper: compared with the hopper, 0xffff is none)
    long hopper_error;                             // (multiplayer_game_hopper: the peer's hopper error)
    bool has_gamestate_bonus;
    long upstream_bandwidth;
    long downstream_bandwidth;
    bool is_peer_flagged;
    long nat_type;
    unsigned long connection_quality;
    long connectivity_badness;
    long host_badness;
    s_network_session_peer_connectivity connectivity;
    long language;
    long determinism_version;
    long determinism_compatible_version;
    // March: c_flags<e_network_session_peer_properties_status_flags, unsigned long, 14> (its test/set/clear and
    // constructor initialize it); starts cleared for each peer
    c_flags<e_network_session_peer_properties_status_flags, unsigned long, 14> status_flags;
};

struct s_network_session_peer
{
    s_transport_secure_address secure_address;
    e_network_session_peer_state connection_state; // March get_peer_connection_state
    long peer_version;
    unsigned long join_start_time;                 // March get_creation_timestamp
    s_network_session_peer_properties properties;
    unsigned __int64 party_nonce;                  // March set_peer_party_nonce (before the join nonce)
    unsigned __int64 join_nonce;                   // March set_join_nonce / get_join_nonce
    long player_count;
    long user_player_indices[4];                   // March get_player_from_peer, per user, NONE if none
};


typedef char network_session_peer_properties_size[(sizeof(s_network_session_peer_properties) == 0xc0) ? 1 : -1];
typedef char network_session_peer_size[(sizeof(s_network_session_peer) == 0x118) ? 1 : -1];

#endif
