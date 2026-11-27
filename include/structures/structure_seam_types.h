#ifndef STRUCTURES_STRUCTURE_SEAM_TYPES_H
#define STRUCTURES_STRUCTURE_SEAM_TYPES_H

#include "math/real_math_types.h"
#include "memory/static_arrays.h"

struct s_seam_cluster_mapping
{
    union
    {
        long geometry[3];
        real_point3d position;
    };
    signed char cluster_indices[2];
    signed char cluster_mapping_indices[2];
};

struct s_seam_mapping
{
    signed char structure_indices[2];
    short cluster_count;
    s_seam_cluster_mapping clusters[32];
};

struct s_structure_seam_globals
{
    c_static_flags<512> bsp_seam_flags[16];
    s_seam_mapping mappings[64];
    c_static_flags<64> active_seam_flags;
    long connected_structure_bsp_mask;
};

#endif // STRUCTURES_STRUCTURE_SEAM_TYPES_H
