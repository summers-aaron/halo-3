#ifndef AI_PATHFINDING_PILL_RESULT_H
#define AI_PATHFINDING_PILL_RESULT_H
#include "ai/sector_ref.h"
struct pathfinding_test_pill2d_result
{
    float distance;
    c_sector_ref sector;
    long link_index;
    short structure_bsp_index;
    short disc_index;
    short obstacle_index;
    bool left_side_blocked;
    bool right_side_blocked;
};
typedef char pathfinding_pill_result_size[(sizeof(pathfinding_test_pill2d_result) == 0x14) ? 1 : -1];
#endif
