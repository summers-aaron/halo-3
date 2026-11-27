#ifndef VISIBILITY_VISIBILITY_PROJECTION_TYPES_H
#define VISIBILITY_VISIBILITY_PROJECTION_TYPES_H

#include <stddef.h>
#include <vectorintrinsics.h>
#include "math/matrix_math_types.h"

struct visibility_volume
{
    __vector4 vector_planes[6];
    short index;
    short cluster_index;
    long portal_flags;
    short portal_side;
    real_rectangle2d rectangle;
    float distance;
    real_point3d points[5];
    real_rectangle3d bounds;
    real_plane3d planes[6];
    real_vector3d directions[4];
};

struct visibility_projection
{
    bool mirrored;
    real_matrix4x3 world_to_basis;
    real_matrix4x3 basis_to_world;
    bool near_plane_valid;
    float near_clip;
    real_plane3d near_plane;
    bool far_plane_valid;
    bool far_plane_reversed;
    float far_distance;
    bool volume_valid;
    visibility_volume volume;
    long polygon_count;
    real_point2d polygon[5];
};

typedef char visibility_volume_size[(sizeof(visibility_volume) == 0x170) ? 1 : -1];
typedef char visibility_volume_cluster_offset[(offsetof(visibility_volume, cluster_index) == 0x62) ? 1 : -1];
typedef char visibility_volume_flags_offset[(offsetof(visibility_volume, portal_flags) == 0x64) ? 1 : -1];
typedef char visibility_volume_rectangle_offset[(offsetof(visibility_volume, rectangle) == 0x6c) ? 1 : -1];
typedef char visibility_volume_distance_offset[(offsetof(visibility_volume, distance) == 0x7c) ? 1 : -1];
typedef char visibility_volume_points_offset[(offsetof(visibility_volume, points) == 0x80) ? 1 : -1];
typedef char visibility_volume_planes_offset[(offsetof(visibility_volume, planes) == 0xd4) ? 1 : -1];
typedef char visibility_projection_size[(sizeof(visibility_projection) == 0x230) ? 1 : -1];
typedef char visibility_projection_far_valid_offset[(offsetof(visibility_projection, far_plane_valid) == 0x84) ? 1 : -1];
typedef char visibility_projection_far_reversed_offset[(offsetof(visibility_projection, far_plane_reversed) == 0x85) ? 1 : -1];
typedef char visibility_projection_volume_offset[(offsetof(visibility_projection, volume) == 0x90) ? 1 : -1];
typedef char visibility_projection_polygon_count_offset[(offsetof(visibility_projection, polygon_count) == 0x200) ? 1 : -1];

#endif // VISIBILITY_VISIBILITY_PROJECTION_TYPES_H
