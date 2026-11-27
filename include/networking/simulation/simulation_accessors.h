#ifndef NETWORKING_SIMULATION_SIMULATION_ACCESSORS_H
#define NETWORKING_SIMULATION_SIMULATION_ACCESSORS_H

#include "simulation/simulation_globals.h"

// Callers read the simulation globals through
// these accessors; the debug-only world/initialization assertions are stripped.
__forceinline bool simulation_is_initialized()
{
    return g_simulation_globals.initialized;
}

__forceinline c_simulation_world *simulation_get_world()
{
    return g_simulation_globals.world;
}

__forceinline c_simulation_type_collection *simulation_get_type_collection()
{
    return g_simulation_globals.type_collection;
}

#endif
