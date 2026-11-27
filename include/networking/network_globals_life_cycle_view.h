#ifndef NETWORKING_NETWORK_GLOBALS_LIFE_CYCLE_VIEW_H
#define NETWORKING_NETWORK_GLOBALS_LIFE_CYCLE_VIEW_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

// Initialization/update prefix of the lifecycle globals. This view avoids the
// full lifecycle header, whose online-search task types collide in this owner.
struct network_globals_life_cycle_view
{
    bool initialized;
    __declspec(align(8)) c_life_cycle_state_manager manager;
};

#endif
