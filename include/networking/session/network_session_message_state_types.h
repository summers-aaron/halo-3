#ifndef NETWORKING_SESSION_NETWORK_SESSION_MESSAGE_STATE_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_MESSAGE_STATE_TYPES_H
#include <stddef.h>
#include "networking/transport/transport_platform_types.h"
#include "networking/transport/transport_security_types.h"
// The two election message layouts below differ
// in the first word; do not merge mode and start_time without resolving that dispute.
struct s_network_session_message_election
{
    unsigned long start_time;
    unsigned long decision_time;
    long local_peer_index;
    s_transport_secure_address host_address;
    long peer_count;
    s_transport_secure_address peers[16];
    unsigned long decision;
    long peer_slot[16];
    unsigned long participation_mask;
    unsigned long refusal_mask;
};

struct s_network_session_message_election_state
{
    long mode;
    unsigned long decision_time;
    long local_peer;
    unsigned long host_address_prefix[2];
    unsigned long handoff_mask;
    unsigned long host_address_suffix[6];
    long peer_count;
    s_transport_secure_address peers[16];
    unsigned long decision;
    unsigned long connectivity[16];
    unsigned long agreement;
    unsigned long refused;
    unsigned long received_time[16];
};


typedef char message_election_peers_offset[(offsetof(s_network_session_message_election, peers) == 0x34) ? 1 : -1];
typedef char message_election_connectivity_offset[(offsetof(s_network_session_message_election_state, connectivity) == 0x278) ? 1 : -1];
struct s_network_session_message_reestablish
{
    s_transport_secure_identifier session_id;
    s_transport_session_description description;
    e_transport_platform platform;
};
#endif
