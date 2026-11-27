#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_GAME_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_GAME_TYPES_H

#include "game/game_types.h"
#include "networking/session/network_session_parameter_game_start_status_types.h"
#include <stddef.h>

// Map payload read by begin_load_map and the map parameter codecs.
struct s_network_session_parameter_map
{
    e_campaign_id campaign_id;
    e_map_id map_id;
    char scenario_path[128];
    char campaign_players[120];
};
typedef char network_session_parameter_map_size_check[(sizeof(s_network_session_parameter_map) == 256) ? 1 : -1];
typedef char network_session_parameter_map_path_offset[(offsetof(s_network_session_parameter_map, scenario_path) == 8) ? 1 : -1];
typedef char network_session_parameter_map_players_offset[(offsetof(s_network_session_parameter_map, campaign_players) == 136) ? 1 : -1];

#endif
