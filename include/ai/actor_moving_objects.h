#ifndef AI_ACTOR_MOVING_OBJECTS_H
#define AI_ACTOR_MOVING_OBJECTS_H

// Functions defined in tu/ailibxbox/actor_moving_objects.cpp that other units call.

class c_recent_obstacle;
union real_vector3d;

c_recent_obstacle *actor_find_recent_obstacle(long actor_index, long object_index);
void ignored_obstacle_register(long actor_index, long object_index, short unused, bool success);
bool actor_moving_handle_object_hint(long actor_index, long object_index, const short *hint_flags, real_vector3d *avoidance_vector, real_vector3d *throttle);

#endif // AI_ACTOR_MOVING_OBJECTS_H
