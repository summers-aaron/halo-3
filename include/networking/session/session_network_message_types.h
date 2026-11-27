#ifndef NETWORKING_SESSION_SESSION_NETWORK_MESSAGE_TYPES_H
#define NETWORKING_SESSION_SESSION_NETWORK_MESSAGE_TYPES_H

enum e_session_network_message
{
    _session_network_message_guide_opened = 0,
    _session_network_message_guide_closed = 1,
    _session_network_message_transport_startup = 2,
    _session_network_message_transport_shutdown = 3,
    _session_network_message_online_connected = 4,
    _session_network_message_online_duplicate_logon = 5,
    _session_network_message_online_disconnected = 6,
    k_session_network_message_count
};

#endif // NETWORKING_SESSION_SESSION_NETWORK_MESSAGE_TYPES_H
