#ifndef AI_PATHFINDING_SECTOR_TYPES_H
#define AI_PATHFINDING_SECTOR_TYPES_H

#include "memory/data_types.h"

// A pathfinding sector entry: flags plus head indices into the hint and link lists.
struct pathfinding_sector
{
    short flags;
    short first_hint_index;
    long first_link_index;
};

#endif // AI_PATHFINDING_SECTOR_TYPES_H
