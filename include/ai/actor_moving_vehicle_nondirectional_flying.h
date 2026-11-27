#ifndef AI_ACTOR_MOVING_VEHICLE_NONDIRECTIONAL_FLYING_H
#define AI_ACTOR_MOVING_VEHICLE_NONDIRECTIONAL_FLYING_H

#include "ai/actor_moving_types.h"

// Functions defined in tu/ailibxbox/actor_moving_vehicle_nondirectional_flying.cpp that other units call.

union real_vector3d;

void steering_control(long actor_index, long object_index, real_vector3d *input, float scale, real_vector3d *forward, real_vector3d *result);
e_movement_result actor_move_calculate_nondirectional_flying_movement(long actor_index, long object_index, const real_vector3d *movement,
    real_vector3d *desired_facing, real_vector3d *desired_throttle, short iteration);

#endif // AI_ACTOR_MOVING_VEHICLE_NONDIRECTIONAL_FLYING_H
