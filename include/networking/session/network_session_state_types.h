#ifndef NETWORKING_SESSION_NETWORK_SESSION_STATE_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_STATE_TYPES_H

// Host-state payloads for handoff and reestablishment.
struct s_network_session_state_host_handoff
{
    bool host_leaving;                                      // March assert
    unsigned long peer_consideration_mask;
    long handoff_mode;
    bool stabilized;
    long unknown;                   // zeroed with the state; newer than the March 2007 44-byte state; no reader is attested
    unsigned long start_time;
    long handoff_peer_index;
    bool option1;                                           // change_local_state_host_handoff arguments
    bool option2;
    bool handoff_initiated;
    unsigned long handoff_initiated_time;
    unsigned long acknowledged_peer_mask;                   // replies OR in peer bits
    bool handoff_transition_sent;
    unsigned long handoff_transition_time;
};

struct s_network_session_state_host_reestablish
{
    long phase;
    bool reestablish_host_leaving;                          // March assert
    bool use_election;
    long original_host_peer_index;
    unsigned long start_time;
    long reestablishment_mode;
    unsigned long requested_peer_mask;
    unsigned long acknowledged_peer_mask;
    unsigned long left_peer_mask;                           // peers leaving during reestablishment
};

typedef char network_session_host_handoff_size[(sizeof(s_network_session_state_host_handoff) == 48) ? 1 : -1];
typedef char network_session_host_reestablish_size[(sizeof(s_network_session_state_host_reestablish) == 32) ? 1 : -1];

#endif
