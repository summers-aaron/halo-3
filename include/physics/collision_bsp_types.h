#ifndef PHYSICS_COLLISION_BSP_TYPES_H
#define PHYSICS_COLLISION_BSP_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

struct collision_bsp
{
    s_tag_block bsp3d_nodes;
    s_tag_block planes;
    s_tag_block leaves;
    s_tag_block bsp2d_references;
    s_tag_block bsp2d_nodes;
    s_tag_block surfaces;
    s_tag_block edges;
    s_tag_block vertices;
};

struct collision_surface
{
    short plane;
    unsigned short first_edge;
    short material;
    short breakable_surface_bit;
    short breakable_surface;
    unsigned char flags;
    unsigned char best_plane_calculation_vertex_index;
};

struct collision_edge
{
    unsigned short vertices[2];
    unsigned short edges[2];
    short surfaces[2];
};

struct collision_vertex
{
    real_point3d point;
    unsigned short first_edge;
    unsigned short sink;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_bsp_size[(sizeof(collision_bsp) == 0x60) ? 1 : -1];
typedef char collision_bsp_surfaces_offset[(offsetof(collision_bsp, surfaces) == 0x3c) ? 1 : -1];
typedef char collision_bsp_edges_offset[(offsetof(collision_bsp, edges) == 0x48) ? 1 : -1];
typedef char collision_bsp_vertices_offset[(offsetof(collision_bsp, vertices) == 0x54) ? 1 : -1];
#endif
typedef char collision_surface_size[(sizeof(collision_surface) == 12) ? 1 : -1];
typedef char collision_surface_breakable_bit_offset[(offsetof(collision_surface, breakable_surface_bit) == 6) ? 1 : -1];
typedef char collision_surface_breakable_offset[(offsetof(collision_surface, breakable_surface) == 8) ? 1 : -1];
typedef char collision_surface_flags_offset[(offsetof(collision_surface, flags) == 10) ? 1 : -1];
typedef char collision_surface_best_vertex_offset[(offsetof(collision_surface, best_plane_calculation_vertex_index) == 11) ? 1 : -1];
typedef char collision_edge_size[(sizeof(collision_edge) == 12) ? 1 : -1];
typedef char collision_vertex_size[(sizeof(collision_vertex) == 16) ? 1 : -1];

#endif // PHYSICS_COLLISION_BSP_TYPES_H
