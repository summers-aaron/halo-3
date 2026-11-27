#ifndef VISIBILITY_VISIBILITY_REGION_TYPES_H
#define VISIBILITY_VISIBILITY_REGION_TYPES_H

#include "visibility/visibility_projection_types.h"
#include "objects/object_reference_types.h"
#include "memory/static_arrays.h"

// Region layout shared by portal traversal and structure collection.
struct visibility_cluster
{
    union { struct { signed char bsp_index; unsigned char cluster_index; }; s_cluster_reference cluster_reference; };
    short volume_count[6];
    short first_volume[6];
};

struct s_visibility_region
{
    short projection_count;
    visibility_projection projections[6];
    short cluster_count;
    visibility_cluster clusters[128];
    long cluster_indices[128];
    short volume_count;
    visibility_volume volumes[512];
};

struct s_visibility_region_build : s_visibility_region
{
    s_cluster_reference sphere_cluster_reference;
    long option0;
    long option1;
    long flags;
    short original_projection_count;
    real_point3d sphere_center;
    float sphere_radius;
    long collection_type;
    long collection_shape;
    long marker_index;
    unsigned long visible_cluster_bits[16][8];
    c_static_array<c_static_array<char, 256>, 16> visible_cluster_region;
};

typedef char layout_check_1[(sizeof(visibility_cluster) == 0x1a) ? 1 : -1];
typedef char layout_check_2[(sizeof(s_visibility_region) == 0x2fc40) ? 1 : -1];
typedef char layout_check_3[(offsetof(s_visibility_region, clusters) == 0xd32) ? 1 : -1];
typedef char layout_check_4[(offsetof(s_visibility_region, volumes) == 0x1c40) ? 1 : -1];
typedef char layout_check_5[(offsetof(s_visibility_region_build, flags) == 0x2fc4c) ? 1 : -1];
typedef char layout_check_6[(offsetof(s_visibility_region_build, sphere_center) == 0x2fc54) ? 1 : -1];
typedef char layout_check_7[(offsetof(s_visibility_region_build, visible_cluster_bits) == 0x2fc70) ? 1 : -1];

#endif
