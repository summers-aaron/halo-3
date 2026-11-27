#ifndef AI_ACTOR_MOVING_FLYING_VEHICLE_OBSTACLE_AVOIDANCE_H
#define AI_ACTOR_MOVING_FLYING_VEHICLE_OBSTACLE_AVOIDANCE_H

// Functions defined in tu/ailibxbox/actor_moving_flying_vehicle_obstacle_avoidance.cpp that other units call.

union real_vector3d;

extern "C" bool actor_moving_flying_vehicle_compute_avoidance(long actor_index, long object_index, real_vector3d *desired_vector,
	const real_vector3d *current_vector, real_vector3d *avoidance_vector, float *throttle, float *urgency, bool use_object_axes);

#endif // AI_ACTOR_MOVING_FLYING_VEHICLE_OBSTACLE_AVOIDANCE_H
