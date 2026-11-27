#ifndef AI_SECTOR_ENVIRONMENT_OBJECTS_H
#define AI_SECTOR_ENVIRONMENT_OBJECTS_H

#include <stddef.h>

#include "math/real_math_types.h"

class c_pathfinding_object
{
public:
    unsigned short type;
    unsigned short flags;
    unsigned char location_valid;
    unsigned short index;
    unsigned short pad8;
    unsigned short structure_bsp_index;
    long object_index;
    long unknown16;
    long unknown20;
    unsigned char unknown24;
    real_rectangle3d bounds;
    long unknown52;
    long unknown56;
    long unknown60;
    long unknown64;
};

typedef char pathfinding_object_size[(sizeof(c_pathfinding_object) == 0x44) ? 1 : -1];
typedef char pathfinding_object_index_offset[(offsetof(c_pathfinding_object, object_index) == 0x0c) ? 1 : -1];
typedef char pathfinding_object_location_valid_offset[(offsetof(c_pathfinding_object, location_valid) == 4) ? 1 : -1];
typedef char pathfinding_object_bsp_index_offset[(offsetof(c_pathfinding_object, structure_bsp_index) == 10) ? 1 : -1];
typedef char pathfinding_object_bounds_offset[(offsetof(c_pathfinding_object, bounds) == 28) ? 1 : -1];

struct scenario;

long find_pathfinding_objects(scenario *scenario, short, c_pathfinding_object *pathfinding_object, long, bool);
short pathfinding_object_get_object_ref_index(long object_index, short node_index);

#endif // AI_SECTOR_ENVIRONMENT_OBJECTS_H
