#ifndef NETWORKING_LOGIC_LOGIC_SESSION_TRACKER_UNSUITABLE_REASON_TYPES_H
#define NETWORKING_LOGIC_LOGIC_SESSION_TRACKER_UNSUITABLE_REASON_TYPES_H

enum e_network_session_tracker_session_unsuitable_reason
{
    _session_unsuitable_none = 0,
    _session_unsuitable_qos_failed = 1,
    _session_unsuitable_qos_unavailable = 2,
    _session_unsuitable_qos_result_failed = 3,
    _session_unsuitable_qos_empty_data_block = 4,
    _session_unsuitable_qos_decode_failed = 5,
    _session_unsuitable_qos_unexpected_payload_type = 6,
    _session_unsuitable_qos_timed_out = 7,
    _session_unsuitable_qos_version_incompatible = 8,
    _session_previously_unsuitable = 9,
    _session_unsuitable_matchmaking_rejected = 10,
    _session_unsuitable_session_data_unavailable = 11
};

#endif // NETWORKING_LOGIC_LOGIC_SESSION_TRACKER_UNSUITABLE_REASON_TYPES_H
