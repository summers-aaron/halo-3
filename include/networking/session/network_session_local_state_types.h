#ifndef NETWORKING_SESSION_NETWORK_SESSION_LOCAL_STATE_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_LOCAL_STATE_TYPES_H

#include <stddef.h>
#include "networking/session/network_session_join_request_types.h"
#include "networking/session/network_session_state_types.h"
#include "networking/transport/transport_address.h"

enum e_transport_platform;
enum e_peer_reestablish_state;

// m_local_state_data: the data of the session's local state, one member per state that has any (March asserts name
// the members: "m_local_state_data.peer_joining.join_nonce", "...peer_join_abort.join_usable_address",
// "host_handoff.host_leaving", "host_reestablish.reestablish_host_leaving"). Field sources: the March 2007 bodies of
// change_local_state_* / idle_* and the bodies in
// network_session_state.cpp and network_session.cpp. Type names are not attested.
struct s_network_session_state_peer_creating                // state 1
{
    e_transport_platform transport_platform;
    bool unknown;                                           // (change_local_state_peer_creating's bool)
    unsigned __int64 join_nonce;
    s_network_session_join_request join_request;
};

struct s_network_session_state_peer_joining                 // state 2
{
    long observer_channel_index;                            // March: observer_channel_secure_connection_established(owner, this)
    s_transport_secure_address join_address;                // (March: change_local_state_peer_joining's parameter names)
    transport_address join_usable_address;                  // March assert
    bool use_join_nonce;                                    // peer-connect (join_nonce) instead of a join request
    unsigned __int64 join_nonce;                            // March assert
    s_network_session_join_request join_request;            // March assert ".join_request.join_nonce"
    unsigned long join_start_timestamp;                     // network_time_get() in change_local_state_peer_joining
    unsigned long join_holding_in_queue_timestamp;          // NONE; set by host_connection_refused(holding_in_queue); idle times
                                                            //       out from it instead of 0x8c0; no request is sent while set
    unsigned long join_secure_connection_timestamp;         // March assert; set once the secure connection is up
    long join_request_count;                                // incremented for each request sent
    unsigned long join_request_timestamp;                   // last request sent
};

struct s_network_session_state_peer_join_abort              // state 3 (March memset 120)
{
    s_transport_secure_address join_address;                // copied from peer_joining.join_address
    transport_address join_usable_address;                  // March assert
    s_transport_secure_address local_address;               // transport_secure_address_get_safe()
    unsigned __int64 join_nonce;                            // March assert; use_join_nonce ? join_nonce : join_request.join_nonce
    unsigned long join_start_timestamp;                     // peer_joining.join_start_timestamp
    unsigned long join_abort_start_timestamp;               // network_time_get() (idle_peer_join_abort's timeout base)
    unsigned long join_abort_request_timestamp;             // last abort sent (check_to_send_peer_join_abort)
};

struct s_network_session_state_peer_leaving                 // state 5
{
    e_peer_reestablish_state peer_reestablish_state;
    unsigned long leave_start_timestamp;
    unsigned long leave_request_timestamp;
};

struct s_network_session_election_state                 // state 10
{
    unsigned long start_time, decision_time;
    long local_peer_index;
    s_transport_secure_address host_address;
    long peer_count;
    s_transport_secure_address peers[16];
    unsigned long decision_mask;
    unsigned long peer_votes[16];
    unsigned long local_mask, host_mask;
    unsigned long peer_times[16];
};

union s_network_session_local_state_data
{
    s_network_session_state_peer_creating peer_creating;
    s_network_session_state_peer_joining peer_joining;
    s_network_session_state_peer_join_abort peer_join_abort;
    s_network_session_state_peer_leaving peer_leaving;
    s_network_session_state_host_handoff host_handoff;
    s_network_session_state_host_reestablish host_reestablish;
    s_network_session_election_state election;           // network_session_message.cpp's view disagrees at 0x00 and 0x14
};
typedef char network_session_local_state_data_size_check[(sizeof(s_network_session_local_state_data) == 0x8d8) ? 1 : -1];
#define NETWORK_SESSION_STATE_OFFSET_CHECK(type, field, offset) typedef char type##_##field##_offset[(offsetof(type, field) == (offset)) ? 1 : -1]
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_join_request, joining_player_count, 0x394);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_join_request, join_from_recruiting, 0x798);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_join_request, payload, 0x79c);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_creating, join_request, 0x10);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_joining, join_usable_address, 0x28);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_joining, join_nonce, 0x40);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_joining, join_start_timestamp, 0x8c0);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_joining, join_request_timestamp, 0x8d0);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_join_abort, join_usable_address, 0x24);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_join_abort, join_nonce, 0x60);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_peer_join_abort, join_abort_request_timestamp, 0x70);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_host_handoff, start_time, 0x14);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_host_handoff, handoff_initiated, 0x1e);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_host_handoff, handoff_transition_time, 0x2c);
NETWORK_SESSION_STATE_OFFSET_CHECK(s_network_session_state_host_reestablish, left_peer_mask, 0x1c);
#undef NETWORK_SESSION_STATE_OFFSET_CHECK

#endif
