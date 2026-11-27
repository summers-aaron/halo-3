#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_JOIN_ABORT_TYPES_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_JOIN_ABORT_TYPES_H

#include "networking/transport/transport_security_types.h"
#include <stddef.h>

// The session protocol writes each field as 64 bits; the handler passes the
// identifier to get_session and the nonce by value to join_abort.
struct s_network_message_join_abort
{
    s_transport_secure_identifier session_id;
    unsigned __int64 join_nonce;
};

typedef char network_message_join_abort_nonce_offset[(offsetof(s_network_message_join_abort, join_nonce) == 8) ? 1 : -1];
typedef char network_message_join_abort_size[(sizeof(s_network_message_join_abort) == 16) ? 1 : -1];

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_JOIN_ABORT_TYPES_H
