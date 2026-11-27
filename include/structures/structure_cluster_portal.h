#ifndef STRUCTURES_STRUCTURE_CLUSTER_PORTAL_H
#define STRUCTURES_STRUCTURE_CLUSTER_PORTAL_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct cluster_portal
{
    short cluster_indices[2];
    long plane_index;
    real_point3d centroid;
    float bounding_radius;
    long flags;
    c_typed_tag_block<real_point3d> vertices;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char cluster_portal_size[(sizeof(cluster_portal) == 0x28) ? 1 : -1];
typedef char cluster_portal_vertices_offset[(offsetof(cluster_portal, vertices) == 0x1c) ? 1 : -1];
#endif

#endif
