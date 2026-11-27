#ifndef AI_ACTOR_MOVING_GIANT_SCARAB_H
#define AI_ACTOR_MOVING_GIANT_SCARAB_H

#include "ai/actor_moving_types.h"

// Functions defined in tu/ailibxbox/actor_moving_giant_scarab.cpp that other units call.

union real_vector3d;

e_movement_result actor_move_calculate_giant_scarab_movement( long actor_index, long object_index, real_vector3d *movement,
    real_vector3d *desired, real_vector3d *result, short iteration);

#endif // AI_ACTOR_MOVING_GIANT_SCARAB_H
