#ifndef AI_PATHFINDING_DATA_H
#define AI_PATHFINDING_DATA_H

#include <stddef.h>

#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

// Element of pathfinding_data::instanced_geometry_refs: the runtime instance
// slot an instanced-geometry instance maps to, plus an unread trailing word.
struct s_pathfinding_instanced_geometry_ref
{
    unsigned short index;
    unsigned short : 16;
};

typedef char pathfinding_instanced_geometry_ref_size[(sizeof(s_pathfinding_instanced_geometry_ref) == 4) ? 1 : -1];

// Shared pathfinding tag data. Only the fields observed by current users
// are named; the final tag size and the intervening bytes remain unresolved.
struct pathfinding_data
{
    s_tag_block sectors;
    s_tag_block links;
    s_tag_block refs;
    s_tag_block bsp2d_nodes;
    c_typed_tag_block<real_point3d> vertices;
    s_tag_block object_refs;
    s_tag_block hints;
    s_tag_block instanced_geometry_refs;
    // March pathfinding_data_valid (82e98028) compares this word against
    // structure_bsp->import_info_checksum: the import stamp this pathfinding was generated from.
    long import_info_checksum;
    // Bytes 0x64-0x70 are read by no March pathfinding_data consumer and no in-tree user.
    int : 32;
    int : 32;
    int : 32;
    s_tag_block bsp2d_roots;
    // pathfinding_find_seam_link indexes a block of link-index tag blocks here.
    s_tag_block seam_link_indices;
    s_tag_block seams;
};

typedef char pathfinding_data_links_offset[(offsetof(pathfinding_data, links) == 0xc) ? 1 : -1];
typedef char pathfinding_data_bsp2d_nodes_offset[(offsetof(pathfinding_data, bsp2d_nodes) == 0x24) ? 1 : -1];
typedef char pathfinding_data_vertices_offset[(offsetof(pathfinding_data, vertices) == 0x30) ? 1 : -1];
typedef char pathfinding_data_hints_offset[(offsetof(pathfinding_data, hints) == 0x48) ? 1 : -1];
typedef char pathfinding_data_instance_refs_offset[(offsetof(pathfinding_data, instanced_geometry_refs) == 0x54) ? 1 : -1];
typedef char pathfinding_data_bsp2d_roots_offset[(offsetof(pathfinding_data, bsp2d_roots) == 0x70) ? 1 : -1];
typedef char pathfinding_data_seams_offset[(offsetof(pathfinding_data, seams) == 0x88) ? 1 : -1];
typedef char pathfinding_data_seam_link_indices_offset[(offsetof(pathfinding_data, seam_link_indices) == 0x7c) ? 1 : -1];

#endif // AI_PATHFINDING_DATA_H
