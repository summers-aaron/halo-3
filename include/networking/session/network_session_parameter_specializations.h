#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SPECIALIZATIONS_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SPECIALIZATIONS_H

#include "game/game_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_payload_types.h"
#include "saved_games/saved_film_description.h"

// Explicit specializations defined by network_session_interface.cpp.
template<>
bool c_generic_network_session_parameter<e_campaign_difficulty_level>::set(const e_campaign_difficulty_level *parameter);

template<>
bool c_generic_network_session_parameter<short>::set(const short *parameter);

template<>
bool c_generic_network_session_parameter<s_network_session_privacy_mode>::set(const s_network_session_privacy_mode *parameter);

template<>
bool c_generic_network_session_parameter<s_saved_film_description>::set(const s_saved_film_description *parameter);

template<>
bool c_generic_network_session_parameter<bool>::request_change(const bool *parameter);

#include "networking/session/network_session_parameter_synchronous_out_of_sync.h"


template <>
inline bool c_generic_network_session_parameter<e_campaign_difficulty_level>::request_change(const e_campaign_difficulty_level *parameter)
{
    bool result = false;
    if (this->set_allowed())
        result = set(parameter);
    else if (this->change_request_allowed()) {
        m_requested_data = *parameter;
        set_desires_change_request();
        result = true;
    }
    return result;
}

template <>
inline bool c_generic_network_session_parameter<s_saved_film_description>::request_change(const s_saved_film_description *parameter)
{
    bool result = false;
    if (this->set_allowed())
        result = set(parameter);
    else if (this->change_request_allowed()) {
        memcpy(&m_requested_data, parameter, sizeof(m_requested_data));
        set_desires_change_request();
        result = true;
    }
    return result;
}

#endif
