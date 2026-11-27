#ifndef PHYSICS_PHYSICS_CONSTANTS_GLOBALS_H
#define PHYSICS_PHYSICS_CONSTANTS_GLOBALS_H
#include "physics/physics_constants_tls.h"

#include "physics/physics_constants_types.h"

// The thread's physics constants (March g_physics_constants, the pointer __tls_set_g_physics_constants_allocator
// sets). March reads them through global_physics_constants_get and the global_*_get accessors of
// physics_constants.cpp.

// March name of the thread-local physics constants pointer.

inline const s_physics_constants *global_physics_constants_get()
{
    return g_physics_constants;
}

inline float global_gravity_get()
{
    return g_physics_constants->gravity;
}

inline float global_water_density_get()
{
    return g_physics_constants->water_density;
}

inline float global_air_density_get()
{
    return g_physics_constants->air_density;
}

#endif // PHYSICS_PHYSICS_CONSTANTS_GLOBALS_H
