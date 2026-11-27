#ifndef STRUCTURES_STRUCTURE_BSP_SEAM_TYPES_H
#define STRUCTURES_STRUCTURE_BSP_SEAM_TYPES_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct s_structure_seam_cluster_mapping
{
    long cluster_index;
    real_point3d position;
};

struct s_structure_seam
{
    char identifier[16];
    c_typed_tag_block<long> edge_indices;
    c_typed_tag_block<s_structure_seam_cluster_mapping> clusters;
};

struct s_structure_seam_edge_mapping
{
    short seam_index;
    short seam_edge_index;
};

typedef char s_structure_seam_cluster_mapping_size[(sizeof(s_structure_seam_cluster_mapping) == 0x10) ? 1 : -1];
typedef char s_structure_seam_size[(sizeof(s_structure_seam) == 0x28) ? 1 : -1];
typedef char s_structure_seam_edge_mapping_size[(sizeof(s_structure_seam_edge_mapping) == 4) ? 1 : -1];

#endif // STRUCTURES_STRUCTURE_BSP_SEAM_TYPES_H
