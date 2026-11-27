#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_CALLBACKS_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_CALLBACKS_H

// Out-of-line payload callbacks defined by network_session_parameters.cpp.
#include "networking/session/network_session_parameter_initial_participants_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "saved_games/saved_film_description.h"

extern "C" s_network_session_parameter_initial_participants *network_session_parameters_clear_entries(
    s_network_session_parameter_initial_participants *participants);

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_CALLBACKS_H
