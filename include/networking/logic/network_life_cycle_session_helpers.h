#ifndef NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_SESSION_HELPERS_H
#define NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_SESSION_HELPERS_H
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session.h"

#include "networking/logic/network_life_cycle_session_helper_templates.h"

static __forceinline bool network_life_cycle_in_squad_session(c_network_session **session)
{
  return network_life_cycle_in_squad_session<c_network_session>(session);
}
#endif
