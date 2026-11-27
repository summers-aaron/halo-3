#ifndef NETWORKING_SUITABLE_MATCHMAKING_SESSION_TYPES_H
#define NETWORKING_SUITABLE_MATCHMAKING_SESSION_TYPES_H

#include "networking/logic/network_search_types.h"
#include "networking/transport/transport_qos_types.h"

struct s_suitable_matchmaking_session {
    char name[16];
    s_transport_session_description description;
    s_transport_qos_result qos_status;
    s_network_squad_status_data qos_data;
    long quality;
};

typedef char suitable_session_qos_offset[(sizeof(void *) != 4 || ((offsetof(s_suitable_matchmaking_session, qos_data) == 0x70))) ? 1 : -1];
typedef char suitable_session_size[(sizeof(void *) != 4 || ((sizeof(s_suitable_matchmaking_session) == 0x1440))) ? 1 : -1];

#endif // NETWORKING_SUITABLE_MATCHMAKING_SESSION_TYPES_H
