#ifndef AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_FLYING_H
#define AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_FLYING_H

#include "ai/actor_moving_types.h"

// Functions defined in tu/ailibxbox/actor_moving_vehicle_directional_flying.cpp that other units call.

union real_vector3d;

e_movement_result actor_move_calculate_directional_flying_movement(long actor_index, long object_index,
    real_vector3d *movement, real_vector3d *desired_facing, real_vector3d *desired_throttle, short iteration);

#endif // AI_ACTOR_MOVING_VEHICLE_DIRECTIONAL_FLYING_H
