#ifndef AI_ACTOR_MOVING_GROUND_H
#define AI_ACTOR_MOVING_GROUND_H

#include "ai/actor_moving_types.h"

union real_vector3d;

e_movement_result actor_move_calculate_ground_movement(
    long actor_index, const real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration);
e_movement_result actor_move_calculate_climbing_movement(
    long actor_index, const real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration);
e_movement_result actor_move_calculate_directional_ground_movement(
    long actor_index, long unit_index, const real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration);
e_movement_result actor_move_calculate_nondirectional_ground_movement(
    long actor_index, long unit_index, const real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration);

#endif // AI_ACTOR_MOVING_GROUND_H
