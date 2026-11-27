#ifndef NETWORKING_SESSION_NETWORK_SESSION_PEER_PROPERTIES_STATUS_FLAGS_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PEER_PROPERTIES_STATUS_FLAGS_TYPES_H

enum e_network_session_peer_properties_status_flags
{
    _network_session_peer_properties_status_flags_none = 0,
    _network_session_peer_properties_status_squad_ready_bit = 1,
    _network_session_peer_properties_status_arbitration_failed_bit = 3,
    _network_session_peer_properties_status_arbitration_validated_bit = 4,
    _network_session_peer_properties_status_repeated_play_complete_bit = 5,
    _network_session_peer_properties_status_start_match_complete_bit = 6,
    _network_session_peer_properties_status_start_match_failed_bit = 7,
    _network_session_peer_properties_status_initial_stats_written_bit = 8,
    _network_session_peer_properties_status_initial_stats_write_failed_bit = 9,
    _network_session_peer_properties_status_match_stats_written_bit = 10,
    _network_session_peer_properties_status_leader_idle_controller_bit = 13,
    k_network_session_peer_properties_status_flags_count = 14,

    _network_session_peer_properties_status_match_ready_for_next_match_bit = 14
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_PEER_PROPERTIES_STATUS_FLAGS_TYPES_H
