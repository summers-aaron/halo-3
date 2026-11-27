#ifndef NETWORKING_LOGIC_NETWORK_JOIN_DEPENDENCIES_H
#define NETWORKING_LOGIC_NETWORK_JOIN_DEPENDENCIES_H
#include "networking/logic/life_cycle/life_cycle_tracker_view.h"
#include "networking/online/online_presence_accessors.h"
#include "saved_games/content_catalogue_types.h"
#include "interface/controller_inline.h"
#include "networking/online/managed_session_status_flags.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_assemble_match.h"
typedef c_flags_no_init<e_managed_session_status_flags, unsigned short, 14> network_join_managed_session_status;
extern "C" network_join_managed_session_status *managed_session_get_status(network_join_managed_session_status *, long);
static __forceinline s_levels_globals &network_join_levels_globals()
{
    return g_level_globals;
}
#endif
