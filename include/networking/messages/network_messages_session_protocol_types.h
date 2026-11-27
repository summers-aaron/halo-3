#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PROTOCOL_TYPES_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PROTOCOL_TYPES_H

#include "networking/messages/network_messages_session_protocol.h"
#include "networking/logic/logic_matchmaking_desirability.h"

struct s_network_message_peer_connect {
    unsigned short protocol;
    // Filler ahead of session_id; encode/decode carry protocol, session-id and join-nonce only.
    unsigned short pad;
    s_transport_secure_identifier session_id;
    unsigned __int64 join_nonce;
};
#include "networking/messages/network_message_join_abort_types.h"
struct s_network_message_peer_reestablish {
    s_transport_secure_identifier session_id;
};
struct s_network_message_election_refuse {
    s_transport_secure_identifier session_id;
    long refusal_reason;
    bool alternate_host_valid;
    s_transport_secure_address alternate_host_address;
};
class c_network_message_peer_connect {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};
class c_network_message_join_abort {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};
class c_network_message_peer_reestablish {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};
class c_network_message_election_refuse {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_join_refuse {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

struct s_network_message_session_disband {
    s_transport_secure_identifier session_id;
};
class c_network_message_session_disband {
public:
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};
struct s_network_message_election {
    s_transport_secure_identifier session_id;
    s_transport_secure_address host_address;
    long peer_count;
    s_transport_secure_address peer_addresses[16];
    long host_update_number;
    long peer_update_numbers[16];
};
class c_network_message_election {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};
#include "networking/session/network_session.h"
struct s_network_message_join_request {
    unsigned short protocol;
    long minimum_network_version;
    long current_network_version;
    s_transport_secure_identifier session_id;
    unsigned __int64 join_nonce;
    unsigned __int64 join_party_nonce;
    long joining_peer_count;
    s_network_session_join_request_peer joining_peers[16];
    long joining_player_count;
    s_network_session_join_request_player joining_players[16];
    bool join_from_recruiting;
    long payload_type;
    s_matchmaking_search_party_properties search_properties;
};
class c_network_message_join_request {
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char network_message_peer_connect_size[(sizeof(s_network_message_peer_connect) == 24) ? 1 : -1];
typedef char network_message_join_request_size[(sizeof(s_network_message_join_request) == 0x890) ? 1 : -1];
typedef char network_message_peer_connect_session_id_offset[(offsetof(s_network_message_peer_connect, session_id) == 4) ? 1 : -1];
typedef char network_message_peer_connect_join_nonce_offset[(offsetof(s_network_message_peer_connect, join_nonce) == 0x10) ? 1 : -1];
typedef char network_message_join_request_join_nonce_offset[(offsetof(s_network_message_join_request, join_nonce) == 0x18) ? 1 : -1];
typedef char network_message_join_request_payload_type_offset[(offsetof(s_network_message_join_request, payload_type) == 0x7b4) ? 1 : -1];
#endif

#endif
