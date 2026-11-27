#ifndef PHYSICS_COLLISION_BSP_QUERY_TYPES_H
#define PHYSICS_COLLISION_BSP_QUERY_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct collision_bsp_test_vector_result
{
    float fraction;
    const real_plane3d *plane;
    long leaf_index;
    long surface_index;
    long plane_index;
    unsigned char flags;
    unsigned char breakable_surface_bit;
    unsigned short material;
    long leaf_count;
    long leaf_indices[256];
    unsigned char breakable_surface;
};

struct collision_bsp_test_sphere_result
{
    long surface_count;
    long surface_indices[256];
    long surface_planes[256];
    long edge_count;
    long edge_indices[256];
    long edge_planes[256];
    long vertex_count;
    long vertex_indices[256];
    long vertex_planes[256];
    long leaf_count;
    long leaf_indices[256];
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_vector_result_size[(sizeof(collision_bsp_test_vector_result) == 0x420) ? 1 : -1];
typedef char collision_vector_result_leaf_offset[(offsetof(collision_bsp_test_vector_result, leaf_index) == 8) ? 1 : -1];
typedef char collision_vector_result_plane_offset[(offsetof(collision_bsp_test_vector_result, plane_index) == 0x10) ? 1 : -1];
typedef char collision_vector_result_flags_offset[(offsetof(collision_bsp_test_vector_result, flags) == 0x14) ? 1 : -1];
typedef char collision_vector_result_breakable_offset[(offsetof(collision_bsp_test_vector_result, breakable_surface) == 0x41c) ? 1 : -1];
#endif
typedef char collision_sphere_result_size[(sizeof(collision_bsp_test_sphere_result) == 0x1c10) ? 1 : -1];
typedef char collision_sphere_result_edges_offset[(offsetof(collision_bsp_test_sphere_result, edge_count) == 0x804) ? 1 : -1];
typedef char collision_sphere_result_vertices_offset[(offsetof(collision_bsp_test_sphere_result, vertex_count) == 0x1008) ? 1 : -1];
typedef char collision_sphere_result_leaves_offset[(offsetof(collision_bsp_test_sphere_result, leaf_count) == 0x180c) ? 1 : -1];

#endif // PHYSICS_COLLISION_BSP_QUERY_TYPES_H
