#ifndef GEOMETRY_RENDER_GEOMETRY_PART_TYPES_H
#define GEOMETRY_RENDER_GEOMETRY_PART_TYPES_H
#include "cseries/cseries.h"
#include <stddef.h>
struct s_render_part
{
    short render_method_index;
    short transparent_sorting_index;
    unsigned short index_start;
    unsigned short index_count;
    unsigned short subpart_start;
    unsigned short subpart_count;
    unsigned char part_type;
    unsigned char flags[3];
};
typedef char render_geometry_part_types_layout_1[(sizeof(s_render_part) == 0x10) ? 1 : -1];
typedef char render_geometry_part_types_layout_2[(offsetof(s_render_part, flags) == 0xd) ? 1 : -1];
typedef char render_geometry_part_types_layout_3[(offsetof(s_render_part, index_count) == 6) ? 1 : -1];
#endif
