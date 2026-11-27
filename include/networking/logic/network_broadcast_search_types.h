#ifndef INCLUDE_NETWORKING_LOGIC_NETWORK_BROADCAST_SEARCH_TYPES_H
#define INCLUDE_NETWORKING_LOGIC_NETWORK_BROADCAST_SEARCH_TYPES_H

#include "networking/logic/network_search_types.h"
class c_network_link;
class c_network_message_gateway;
struct s_network_broadcast_search_globals {
    bool initialized;
    c_network_link *link;
    c_network_message_gateway *message_gateway;
    bool searching;
    bool changed;
    unsigned long last_search_time;
    unsigned __int64 search_nonce;
    long session_count;
    s_available_session *sessions;
};
extern "C" s_network_broadcast_search_globals g_broadcast_search_globals;
#endif
