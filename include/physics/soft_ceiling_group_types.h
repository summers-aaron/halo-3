#ifndef PHYSICS_SOFT_CEILING_GROUP_TYPES_H
#define PHYSICS_SOFT_CEILING_GROUP_TYPES_H

#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "physics/soft_ceiling_triangle_types.h"

struct s_soft_ceiling_group {
    long name;
    unsigned short type;
    short pad6;
    long triangle_count;
    TAG_POINTER(s_soft_ceiling_triangle) triangles;
    long pad10;
};

#endif // PHYSICS_SOFT_CEILING_GROUP_TYPES_H
