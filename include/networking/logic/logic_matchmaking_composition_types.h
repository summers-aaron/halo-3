#ifndef NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_TYPES_H
#define NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_TYPES_H

enum e_network_session_matchmaking_composition_build_result
{
    _network_session_matchmaking_composition_build_result_none = 0,
    _network_session_matchmaking_composition_build_result_invalid_session = 1,
    _network_session_matchmaking_composition_build_result_no_hopper = 2,
    _network_session_matchmaking_composition_build_result_hopper_mismatch = 3,
    _network_session_matchmaking_composition_build_result_hoppers_not_loaded = 4,
    _network_session_matchmaking_composition_build_result_player_not_ready = 5,
    _network_session_matchmaking_composition_build_result_player_busy = 6,
    _network_session_matchmaking_composition_build_result_no_local_player_in_state = 7,
    _network_session_matchmaking_composition_build_result_too_many_players = 8,
    _network_session_matchmaking_composition_build_result_invalid_party_composition = 9,
    _network_session_matchmaking_composition_build_result_guests_not_allowed = 10,
    _network_session_matchmaking_composition_build_result_cant_pick_teams = 11,
    _network_session_matchmaking_composition_build_result_cant_even_out_teams = 12,
    _network_session_matchmaking_composition_build_result_big_party_no_rabble_yet = 13,
    _network_session_matchmaking_composition_build_result_big_party_teams_exceed_imbalance = 14,
    _network_session_matchmaking_composition_build_result_big_party_count_exceeds_teams = 15,
    _network_session_matchmaking_composition_build_result_no_signed_in_local_players = 16,
    _network_session_matchmaking_composition_build_result_skill_spread_too_large = 17,
    _network_session_matchmaking_composition_build_result_success = 18
};

#endif // NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_TYPES_H
