#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_ACCESS_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_ACCESS_H

#include "networking/session/network_session_parameters_base.h"
struct s_owner;
struct s_parameter_base;


static __forceinline bool network_session_parameter_set_allowed(s_owner *parameter)
{
    return ((c_network_session_parameter_base const *)parameter)->set_allowed();
}

static __forceinline bool network_session_parameter_change_request_allowed(s_parameter_base *parameter)
{
    return ((c_network_session_parameter_base const *)parameter)->change_request_allowed();
}

#endif
