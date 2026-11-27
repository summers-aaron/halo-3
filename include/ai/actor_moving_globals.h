#ifndef AI_ACTOR_MOVING_GLOBALS_H
#define AI_ACTOR_MOVING_GLOBALS_H

#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "physics/collision_test_flags.h"

// Runtime state consumed by actor movement.
#include "ai/command_script_tls.h"
extern "C" {
extern float g_actor_moving_destination_direction_threshold;
}

#endif // AI_ACTOR_MOVING_GLOBALS_H
