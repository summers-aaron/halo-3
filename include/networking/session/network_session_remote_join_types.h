#ifndef NETWORKING_SESSION_NETWORK_SESSION_REMOTE_JOIN_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_REMOTE_JOIN_TYPES_H

#include "networking/transport/transport_security_types.h"
#include "networking/session/network_session_class.h"
#include <stddef.h>

// The remote session join record is 104 bytes in total.
// The nonce sets the object's eight-byte alignment; security identifiers remain
// byte-aligned within the four-byte-packed record (identifier at +0x1c).
#pragma pack(push, 4)
struct __declspec(align(8)) s_network_session_remote_session_join_data
{
    long join_state;
    long join_to;
    long join_type;
    long join_from;
    unsigned __int64 nonce;
    long timestamp;
    s_transport_secure_identifier identifier;
    s_transport_secure_key key;
    s_transport_secure_address address;
    e_network_session_class session_class;
    long join_result;
    bool join_as_friend;
};
#pragma pack(pop)

typedef char network_remote_join_size[(sizeof(s_network_session_remote_session_join_data) == 0x68) ? 1 : -1];
typedef char network_remote_join_alignment[(__alignof(s_network_session_remote_session_join_data) == 8) ? 1 : -1];
typedef char network_remote_join_nonce[(offsetof(s_network_session_remote_session_join_data, nonce) == 0x10) ? 1 : -1];
typedef char network_remote_join_timestamp[(offsetof(s_network_session_remote_session_join_data, timestamp) == 0x18) ? 1 : -1];
typedef char network_remote_join_identifier[(offsetof(s_network_session_remote_session_join_data, identifier) == 0x1c) ? 1 : -1];
typedef char network_remote_join_key[(offsetof(s_network_session_remote_session_join_data, key) == 0x24) ? 1 : -1];
typedef char network_remote_join_address[(offsetof(s_network_session_remote_session_join_data, address) == 0x34) ? 1 : -1];
typedef char network_remote_join_session_class[(offsetof(s_network_session_remote_session_join_data, session_class) == 0x58) ? 1 : -1];
typedef char network_remote_join_result[(offsetof(s_network_session_remote_session_join_data, join_result) == 0x5c) ? 1 : -1];
typedef char network_remote_join_friend[(offsetof(s_network_session_remote_session_join_data, join_as_friend) == 0x60) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_REMOTE_JOIN_TYPES_H
