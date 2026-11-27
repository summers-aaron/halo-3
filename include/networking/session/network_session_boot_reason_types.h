#ifndef NETWORKING_SESSION_NETWORK_SESSION_BOOT_REASON_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_BOOT_REASON_TYPES_H

enum e_network_session_boot_reason
{
    _network_session_boot_reason_none = -1,
    _network_session_boot_reason_player_booted_player = 0,
    _network_session_boot_reason_in_game_join_timed_out = 1,
    _network_session_boot_reason_prepare_map_failed = 2,
    _network_session_boot_reason_stats_write_failed = 3,
    _network_session_boot_reason_leader_booted = 4,
    _network_session_boot_reason_in_game_join_aborted = 5,
    _network_session_boot_reason_recreating_session = 6,
    _network_session_boot_reason_invalid_join_nonce = 7,
    _network_session_boot_reason_post_match_left_arbitrated_session = 8,
    k_network_session_boot_reason_count
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_BOOT_REASON_TYPES_H
