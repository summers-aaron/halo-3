#ifndef AI_ACTOR_BEHAVIOR_QUERIES_H
#define AI_ACTOR_BEHAVIOR_QUERIES_H

struct firing_position;
union real_vector3d;

extern "C" short actor_get_available_firing_positions(long actor_index, firing_position *positions, long maximum_count, void *context);
extern "C" bool actor_moving_can_move_along_vector(
    long actor_index, real_vector3d *vector, float distance, float minimum_distance, bool *blocked, float *fraction);

#endif
