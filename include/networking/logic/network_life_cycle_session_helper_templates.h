#ifndef NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_SESSION_HELPER_TEMPLATES_H
#define NETWORKING_LOGIC_NETWORK_LIFE_CYCLE_SESSION_HELPER_TEMPLATES_H

#include "networking/logic/life_cycle/life_cycle_globals_types.h"

template<typename t_session>
static __forceinline bool network_life_cycle_in_squad_session(t_session **session)
{
  bool result = false;
  if (network_life_cycle_globals_get()->initialized) {
    t_session *current = (t_session *)network_life_cycle_globals_get()->manager.m_active_squad_session;
    if (!current->disconnected()) {
      if (session)
        *session = current;
      result = true;
    }
  }
  return result;
}
#endif
