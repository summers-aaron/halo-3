#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_INITIAL_PARTICIPANTS_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_INITIAL_PARTICIPANTS_TYPES_H

#include "game/game_options.h"
#include <stddef.h>

struct s_network_session_parameter_initial_participants
{
    bool valid;
    game_machine_options machine_options;
    game_player_options player_options[16];
};
typedef char network_session_parameter_initial_participants_size[(sizeof(s_network_session_parameter_initial_participants) == 4848) ? 1 : -1];
typedef char network_session_parameter_initial_participants_machines[(offsetof(s_network_session_parameter_initial_participants,
    machine_options) == 4) ? 1 : -1];
typedef char network_session_parameter_initial_participants_players[(offsetof(s_network_session_parameter_initial_participants,
    player_options) == 112) ? 1 : -1];

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_INITIAL_PARTICIPANTS_TYPES_H
