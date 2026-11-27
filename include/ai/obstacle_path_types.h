#ifndef AI_OBSTACLE_PATH_TYPES_H
#define AI_OBSTACLE_PATH_TYPES_H
#include <stddef.h>
#include "ai/path_obstacle_types.h"
#include "ai/sector_ref.h"
// Full 0x38-byte search node, matching path_obstacle_avoidance.cpp's avoidance_step.
struct obstacle_node
{
    real_point3d point;
    c_sector_ref sector;
    real_vector2d direction;
    float distance;
    short obstacle_index;
    short disc_index;
    bool side;
    bool turning_point;
    short children[2];
    float cost;
    union { short previous; short parent; };
    short a, b, c;
    bool visited;
};
struct obstacle_path
{
    float radius;
    bool flag;
    const obstacles *obstacles_ref;
    real_point3d goal;
    c_sector_ref goal_sector;
    short goal_obstacle;
    short last_node;
    short closest_step;
    float closest_distance;
    short projection_axis;
    bool projection_sign;
    bool complete;
    bool skip_flagged;
    short step_count;
    obstacle_node nodes[64];
    short heap_count;
    short heap[64];
    union { unsigned short partial; short partial_step; };
};
typedef char obstacle_node_size[(sizeof(obstacle_node) == 0x38) ? 1 : -1];
typedef char obstacle_path_nodes_offset[(sizeof(void *) != 4 || ((offsetof(obstacle_path, nodes) == 0x30))) ? 1 : -1];
typedef char obstacle_path_size[(sizeof(void *) != 4 || ((sizeof(obstacle_path) == 0xeb4))) ? 1 : -1];
#endif
