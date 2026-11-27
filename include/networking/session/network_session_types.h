#ifndef NETWORKING_SESSION_NETWORK_SESSION_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_TYPES_H

// Leaf header: enums of c_network_session (network_session.h still forward-declares them) for the units that hold
// private copies of the class or of its parameters and cannot include network_session.h or
// network_session_parameters.h yet. network_session_parameters.h includes it. Fold into network_session.h once every
// includer of network_session.h has dropped its own copy of these enums (life_cycle_handler_matchmaking_prepare_map.cpp
// still defines e_network_session_mode).

// c_network_session::m_local_state. March: the state name table at 0x83ce028c ("none", "peer-creating", ...,
// "election") and network_session.h's assert "m_local_state>=0 && m_local_state<k_network_session_state_count".
enum e_network_session_type
{
    _network_session_type_none = 0,
    _network_session_type_squad,
    _network_session_type_group
};

enum e_network_session_state
{
    _network_session_state_none = 0,
    _network_session_state_peer_creating,
    _network_session_state_peer_joining,
    _network_session_state_peer_join_abort,
    _network_session_state_peer_established,
    _network_session_state_peer_leaving,
    _network_session_state_host_established,
    _network_session_state_host_disband,
    _network_session_state_host_handoff,
    _network_session_state_host_reestablish,
    _network_session_state_election,

    k_network_session_state_count
};

// c_network_session::session_mode() and the session-mode parameter. March: the mode name table at 0x83ce02bc
// ("none", "idle", "setup", "in-game", ..., "matchmaking-choosing-game") and the assert
// "current_mode>=0 && current_mode<k_network_session_mode_count".
enum e_network_session_mode
{
    _network_session_mode_none = 0,
    _network_session_mode_idle,
    _network_session_mode_setup,
    _network_session_mode_in_game,
    _network_session_mode_end_game,
    _network_session_mode_post_game,
    _network_session_mode_matchmaking_start,
    _network_session_mode_matchmaking_searching,
    _network_session_mode_matchmaking_gathering,
    _network_session_mode_matchmaking_slave,
    _network_session_mode_matchmaking_disbanding,       // 10
    _network_session_mode_matchmaking_arbitrating,
    _network_session_mode_matchmaking_choosing_game,

    k_network_session_mode_count                        // 13
};

// c_network_session::create_host_session / join_remote_session flags (March: c_flags<e_session_creation_flags,
// unsigned char, 4>). Enumerator names are network_join.cpp's (no March name; network_join sets local and squad
// for the squad session, group and matchmaking for the group session).
enum e_session_creation_flags
{
    _session_creation_flag_local = 0,
    _session_creation_flag_squad,
    _session_creation_flag_matchmaking,
    _session_creation_flag_group,

    k_session_creation_flag_count
};

// Why a host refuses a join (c_network_session::acknowledge_join_request, can_accept_player_join_request; March
// asserts name _network_join_refuse_reason_none and _player_add_pending). Names: March's name table at 0x83ce1890
// (from "no-reason-given" for _none through "holding-in-queue"). Gaps are left open;
// only the values the code
// proves are given: session-disbanded 9 and session-booted 10 (network_join.cpp), aborted 14 and abort-ignored 15
// (handle_join_abort; March 11/12), host-time-out 23 and holding-in-queue 27 (network_join.cpp; March 20/24),
// peer-version-too-low 25 and host-version-too-low 26 (handle_join_request's version checks; March 22/23).
enum e_network_join_refuse_reason
{
    _network_join_refuse_reason_none = 0,
    _network_join_refuse_reason_tried_to_join_self = 1,  // not in March (its peer-joining code sets no reason);
                                                         // change_local_state_peer_joining: the host address is our own
    _network_join_refuse_reason_could_not_connect = 2,   // not in March; idle_peer_joining: no secure connection in time
    _network_join_refuse_reason_join_timed_out = 3,      // not in March; idle_peer_joining: no initial updates in time
    _network_join_refuse_reason_squad_full = 7,         // no March name (not in its table); user_interface_session.cpp
    _network_join_refuse_reason_session_disbanded = 9,
    _network_join_refuse_reason_session_booted = 10,
    _network_join_refuse_reason_address_invalid = 11,   // March name table entry 8 "address-invalid", shifted by 3 like its neighbours
    _network_join_refuse_reason_address_failed = 12,    // March name table entry 9 "address-failed", shifted by 3 like its neighbours
    _network_join_refuse_reason_too_many_observers = 13, // March name table entry 10 "too-many-observers", shifted by 3 like its neighbours
    _network_join_refuse_reason_aborted = 14,
    _network_join_refuse_reason_abort_ignored = 15,
    _network_join_refuse_reason_wrong_payload_type = 16, // March name table entry 13 "wrong-payload-type", shifted by 3 like its neighbours
    _network_join_refuse_reason_no_reservation = 17,    // March name table entry 14 "no-reservation", shifted by 3 like its neighbours
    _network_join_refuse_reason_in_matchmaking = 18,     // network_squad_session_is_joinable; March 15, table string in-matchmaking
    _network_join_refuse_reason_player_count_zero = 19,      // March name table entry 16 "player-count-zero", shifted by 3 like its neighbours
    _network_join_refuse_reason_player_not_online_enabled = 20, // March name table entry 17 "player-not-online-enabled", shifted by 3 like its neighbours
    _network_join_refuse_reason_player_add_pending = 21,        // March name table entry 18 "player-add-pending", shifted by 3 like its neighbours
    _network_join_refuse_reason_player_add_failed = 22,         // March name table entry 19 "player-add-failed", shifted by 3 like its neighbours
    _network_join_refuse_reason_host_time_out = 23,
    _network_join_refuse_reason_rejected_by_host = 24,   // March name table entry 21 "rejected-by-host", shifted by 3 like its neighbours
    _network_join_refuse_reason_peer_version_too_low = 25,
    _network_join_refuse_reason_host_version_too_low = 26,
    _network_join_refuse_reason_holding_in_queue = 27,
    _network_join_refuse_reason_still_connected = 28,   // March name table entry 25 "still-connected", shifted by 3 like its neighbours
    _network_join_refuse_reason_local_host = 29,        // March name table entry 26 "local-host", shifted by 3 like its neighbours
    _network_join_refuse_reason_alternate_host = 30,    // March name table entry 27 "alternate-host", shifted by 3 like its neighbours
};

#endif // NETWORKING_SESSION_NETWORK_SESSION_TYPES_H
