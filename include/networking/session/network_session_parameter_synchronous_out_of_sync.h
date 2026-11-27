#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SYNCHRONOUS_OUT_OF_SYNC_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SYNCHRONOUS_OUT_OF_SYNC_H

#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session_parameter_payload_types.h"

template <>
bool c_generic_network_session_parameter<s_network_session_parameter_synchronous_out_of_sync>::set(
    const s_network_session_parameter_synchronous_out_of_sync *parameter);

template <>
inline bool c_generic_network_session_parameter<s_network_session_parameter_synchronous_out_of_sync>::request_change(
    const s_network_session_parameter_synchronous_out_of_sync *parameter)
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

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_SYNCHRONOUS_OUT_OF_SYNC_H
