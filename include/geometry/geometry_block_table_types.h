#ifndef GEOMETRY_GEOMETRY_BLOCK_TABLE_TYPES_H
#define GEOMETRY_GEOMETRY_BLOCK_TABLE_TYPES_H

#include "geometry/geometry_block_types.h"

// Per-thread table front: the resource page array plus the value count; blocks aliases the value array.
struct s_geometry_block_table {
    long resource_page_count;
    __int64 *resource_pages;
    long resource_value_count;
    s_geometry_block **blocks;
};

#endif // GEOMETRY_GEOMETRY_BLOCK_TABLE_TYPES_H
