#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_SET_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_SET_H

#include "game/game_types.h"
#include "networking/session/network_session_parameter_game_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/session/network_game_simulation_protocol_types.h"

template <>
bool c_generic_network_session_parameter<s_network_session_parameter_map>::set(const s_network_session_parameter_map *parameter);

// Out-of-line setter specializations owned by network_life_cycle and network_session_interface.
template <>
bool c_generic_network_session_parameter<bool>::set(const bool *parameter);
template <>
bool c_generic_network_session_parameter<e_campaign_difficulty_level>::set(const e_campaign_difficulty_level *parameter);

template <>
bool c_generic_network_session_parameter<bool>::request_change(const bool *parameter);
template <>
bool c_generic_network_session_parameter<short>::set(const short *parameter);

// Out-of-line change requests owned by network_session_interface.
template <>
bool c_generic_network_session_parameter<s_network_session_parameter_map>::request_change(const s_network_session_parameter_map *parameter);
template <>
bool c_generic_network_session_parameter<e_network_game_simulation_protocol>::request_change(
    const e_network_game_simulation_protocol *parameter);
template <>
bool c_generic_network_session_parameter<short>::request_change(const short *parameter);

template <>
bool c_generic_network_session_parameter<s_replicated_life_cycle_matchmaking_progress>::set(const s_replicated_life_cycle_matchmaking_progress *parameter);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_GENERIC_SET_H
