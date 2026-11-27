#ifndef NETWORKING_ONLINE_ONLINE_PRESENCE_ACCESSORS_H
#define NETWORKING_ONLINE_ONLINE_PRESENCE_ACCESSORS_H

#include "main/levels_globals.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session_interface_types.h"

static __forceinline s_life_cycle_globals &online_presence_life_cycle_globals()
{
    return g_network_life_cycle_globals;
}
static __forceinline s_player_slot &online_presence_user_slot(long index)
{
    return g_network_session_interface_local_users[index];
}
static __forceinline s_data_array *online_presence_multiplayer_levels()
{
    return g_level_globals.multiplayer_levels;
}

#endif
