#ifndef PHYSICS_PHYSICS_CONSTANTS_TLS_H
#define PHYSICS_PHYSICS_CONSTANTS_TLS_H

#include "physics/physics_constants_types.h"




// Thread-local physics constants for the current thread.
extern __declspec(thread) s_physics_constants *g_physics_constants;

#endif
