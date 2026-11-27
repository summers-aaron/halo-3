#ifndef AI_PATH_OBSTACLE_TYPES_H
#define AI_PATH_OBSTACLE_TYPES_H

#include "math/real_math_types.h"

// The obstacle disc list built by path_obstacles.cpp (obstacles_new / obstacles_add_disc / obstacles_recompute).
// Field names from the March 2007 asserts in ai\path_obstacles.cpp ("obstacles->disc_count",
// "obstacles->obstacle_count", "disc->obstacle_index"). The list holds up to 256 discs.
// Location dispute: Bungie kept these in ai/path_obstacles.h; they live here until the units that still carry a
// private copy and include path_obstacles.h (actor_firing_position, actor_moving_ground, behavior)
// switch to them.

enum
{
    MAXIMUM_DISC_COUNT = 256
};

struct obstacle_disc
{
    mutable short flags;                         // path_new marks goal-penetrating discs through a const list
    short obstacle_index;                         // NONE until obstacles_recompute groups overlapping discs
    long object_index;
    real_point2d center;
    float radius;
};

struct obstacles
{
    short obstacle_count;
    short disc_count;
    short flag1_count;                            // discs added with flag bit 0
    short flag8_count;                            // discs added with flag bit 3
    short unused;                                 // cleared by obstacles_new, never read (March add_disc touches +0..+6 only)
    obstacle_disc discs[MAXIMUM_DISC_COUNT];
};

typedef char obstacle_disc_size[(sizeof(obstacle_disc) == 0x14) ? 1 : -1];
typedef char obstacles_size[(sizeof(obstacles) == 0xc + 0x14 * MAXIMUM_DISC_COUNT) ? 1 : -1];

#endif // AI_PATH_OBSTACLE_TYPES_H
