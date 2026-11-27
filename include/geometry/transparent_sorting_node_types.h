#ifndef GEOMETRY_TRANSPARENT_SORTING_NODE_TYPES_H
#define GEOMETRY_TRANSPARENT_SORTING_NODE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

// render_geometry::part_sorting_position; layout shared with render_structure.cpp.
struct s_transparent_sorting_node
{
    real_plane3d plane;
    real_point3d position;
    float radius;
    unsigned char data_0020[16];
};

typedef char transparent_sorting_node_size[(sizeof(s_transparent_sorting_node) == 0x30) ? 1 : -1];
typedef char transparent_sorting_node_position_offset[(offsetof(s_transparent_sorting_node, position) == 0x10) ? 1 : -1];

#endif
