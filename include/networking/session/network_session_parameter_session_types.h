#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SESSION_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SESSION_TYPES_H

enum e_network_session_mode;

// Session-mode and session-size payloads; the codecs and setters in
// network_session_parameters_session.cpp read and write these words.
struct s_network_session_parameter_session_mode
{
    long sequence;
    e_network_session_mode mode;
    unsigned long time;
};

struct s_network_session_parameter_session_size
{
    long maximum_peer_count;
    long maximum_player_count;
};

typedef char network_session_mode_payload_size[(sizeof(s_network_session_parameter_session_mode) == 12) ? 1 : -1];
typedef char network_session_size_payload_size[(sizeof(s_network_session_parameter_session_size) == 8) ? 1 : -1];

#endif
