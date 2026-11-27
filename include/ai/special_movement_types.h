#ifndef AI_SPECIAL_MOVEMENT_TYPES_H
#define AI_SPECIAL_MOVEMENT_TYPES_H

class c_recent_obstacle;

// What an actor may do to get past obstacles, filled by actor_special_movement_new (actor_moving.cpp) from the
// character's movement block and style, and read by the obstacle and path code.
struct special_movement
{
    unsigned long hint_types;                     // allowed hint-type bits (March: "k_hint_type_count < 32")
    unsigned long movement_hints;                 // character movement block movement hints
    long jump_heights;                            // (1 << jump height) - 1 (March: k_jump_height_count)
    bool infinite_jump;
    short obstacle_ignore_size;
    short obstacle_smashable_size;
    short obstacle_leap_minimum_size;
    short obstacle_leap_maximum_size;
    c_recent_obstacle *recent_obstacles;          // &actor->recent_obstacles
};

typedef char special_movement_size[(sizeof(void *) != 4 || sizeof(special_movement) == 0x1c) ? 1 : -1];

#endif // AI_SPECIAL_MOVEMENT_TYPES_H
