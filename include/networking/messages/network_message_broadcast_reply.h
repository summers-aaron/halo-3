#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_BROADCAST_REPLY_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_BROADCAST_REPLY_H

#include <stddef.h>
#include "networking/logic/network_search_types.h"

// Broadcast reply carrying the 5064-byte squad status payload.
struct s_network_message_broadcast_reply
{
    unsigned short protocol;
    unsigned __int64 search_nonce;
    s_network_squad_status_data status;
};

typedef char network_message_broadcast_reply_status_offset[(offsetof(s_network_message_broadcast_reply, status) == 16) ? 1 : -1];
typedef char network_message_broadcast_reply_size[(sizeof(s_network_message_broadcast_reply) == 5080) ? 1 : -1];

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_BROADCAST_REPLY_H
