#ifndef NETWORKING_LOGIC_NETWORK_JOIN_QUEUE_ENTRY_TYPES_H
#define NETWORKING_LOGIC_NETWORK_JOIN_QUEUE_ENTRY_TYPES_H

#include "networking/transport/transport_address.h"
#include "networking/session/network_session_join_request_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include <stddef.h>

typedef s_network_session_join_request_peer s_networking_join_queue_peer;

// Queue entries retain the complete request following the sender's address.
#pragma pack(push, 8)
struct __declspec(align(8)) s_networking_join_queue_entry
{
    transport_address address;
    unsigned __int64 join_nonce;
    unsigned __int64 join_party_nonce;
    long joining_peer_count;
    s_network_session_join_request_peer joining_peers[16];
    long joining_player_count;
    s_network_session_join_request_player joining_players[16];
    bool join_from_recruiting;
    long payload_type;
    s_matchmaking_search_party_properties search_properties;
    unsigned long time_received;
    unsigned long time_last_acknowledged;
    long desirability;
};
#pragma pack(pop)

typedef char networking_join_entry_size[(sizeof(s_networking_join_queue_entry) == 0x8a0) ? 1 : -1];
typedef char networking_join_entry_nonce[(offsetof(s_networking_join_queue_entry, join_nonce) == 0x18) ? 1 : -1];
typedef char networking_join_entry_peers[(offsetof(s_networking_join_queue_entry, joining_peers) == 0x2c) ? 1 : -1];
typedef char networking_join_entry_players[(offsetof(s_networking_join_queue_entry, joining_players) == 0x3b0) ? 1 : -1];
typedef char networking_join_entry_properties[(offsetof(s_networking_join_queue_entry, search_properties) == 0x7b8) ? 1 : -1];
typedef char networking_join_entry_received[(offsetof(s_networking_join_queue_entry, time_received) == 0x890) ? 1 : -1];

#endif
