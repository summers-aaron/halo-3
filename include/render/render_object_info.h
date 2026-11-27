#ifndef RENDER_RENDER_OBJECT_INFO_H
#define RENDER_RENDER_OBJECT_INFO_H

#include <stddef.h>

struct s_shader_extern_info;

// Mesh submission and visibility share region permutations and sixteen region sort offsets.
struct s_render_object_info
{
    long mesh_count;
    long definition_index;
    long skinning_memory_designator;
    union
    {
        struct
        {
            unsigned char skinning_matrix_count;
            unsigned char mesh_indices[0x44 - 0x0d];
        };
        struct
        {
            unsigned char region_skinning_matrix_count;
            char region_permutations[16];
            char alignment1d;
            unsigned short region_zsort[16];
            short level_of_detail;
            short scenery_instance_index;
            char alignment42[2];
        };
    };
    unsigned long cinematic_clip_plane_mask;
    s_shader_extern_info *shader_extern_info;
    long object_index;
    unsigned long *data;
};

typedef char render_object_info_region_zsort_offset[(offsetof(s_render_object_info, region_zsort) == 0x1e) ? 1 : -1];

#endif
