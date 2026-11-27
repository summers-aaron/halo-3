#ifndef NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_GLOBALS_H
#define NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_GLOBALS_H

#include "networking/online/online_url_version.h"
#include "saved_games/saved_film_description.h"
#include "shell/utility_drive.h"

extern "C" {
extern long g_network_frame_rate_update_time;
extern bool g_network_online_connected;
extern bool g_network_squad_map_loading;
extern long g_network_session_current_map_id;
}

struct s_life_cycle_globals;
#include "networking/session/network_session_interface_types.h"

#include "networking/session/network_session_interface_globals_types.h"
extern "C" {
extern s_life_cycle_globals g_network_life_cycle_globals;
}

static __forceinline bool network_session_interface_has_gamestate_bonus()
{
    return g_utility_drive_mounted;
}

static __forceinline long network_session_interface_get_online_url_version()
{
    return g_online_url_version;
}

#endif // NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_GLOBALS_H
