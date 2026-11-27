#ifndef PHYSICS_HAVOK_WORLD_CONSTRAINT_H
#define PHYSICS_HAVOK_WORLD_CONSTRAINT_H

#include "physics/havok_monitor_storage.h"

static __forceinline void havok_world_add_constraint(phys_constraint *constraint)
{
    if (!g_havok_monitor_globals.inside_simulation_step)
        g_havok_world->add_world_constraint(constraint);
}

#endif
