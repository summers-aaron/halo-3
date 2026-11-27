#ifndef NETWORKING_SESSION_NETWORK_SESSION_PEER_STATE_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PEER_STATE_TYPES_H

enum e_network_session_peer_state
{
    _network_session_peer_state_none = 0,
    _network_session_peer_state_rejoining = 1,
    _network_session_peer_state_reserved = 2,
    _network_session_peer_state_disconnected = 3,
    _network_session_peer_state_connected = 4,
    _network_session_peer_state_joining = 5,
    _network_session_peer_state_joined = 6,
    _network_session_peer_state_waiting = 7,
    _network_session_peer_state_established = 8
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_PEER_STATE_TYPES_H
