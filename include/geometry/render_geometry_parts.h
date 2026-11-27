#ifndef GEOMETRY_RENDER_GEOMETRY_PARTS_H
#define GEOMETRY_RENDER_GEOMETRY_PARTS_H
#include <stddef.h>

struct s_part
{
    union { short material_index; short entry; };
    short transparent_sorting_index;
    union { unsigned short index_start; unsigned short start; };
    union { unsigned short index_count; unsigned short count; };
    union { short first_subpart; short first_range; };
    union { short subpart_count; short range_count; };
    unsigned char type;
    unsigned char specialized_render;
    unsigned short vertex_count;
};
typedef char render_geometry_part_size[(sizeof(s_part) == 16) ? 1 : -1];

struct s_subpart
{
    unsigned short index_start;
    unsigned short index_count;
    unsigned short part_index;
    unsigned short vertex_count;
};

typedef char render_geometry_subpart_size[(sizeof(s_subpart) == 8) ? 1 : -1];
#endif
