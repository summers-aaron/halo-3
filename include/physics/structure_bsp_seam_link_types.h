#ifndef PHYSICS_STRUCTURE_BSP_SEAM_LINK_TYPES_H
#define PHYSICS_STRUCTURE_BSP_SEAM_LINK_TYPES_H

#include <stddef.h>

#include "tag_files/tag_types.h"

// 0x18-byte collision-material row, mirroring s_collision_material: decals.cpp
// views collision_materials through this type (seam at +0x14).
struct s_structure_bsp_seam_link
{
    s_tag_reference render_method;
    short global_material_index;
    short conveyor_surface_index;
    short seam_index;
    unsigned short flags;
};

typedef char structure_bsp_seam_link_size[(sizeof(s_structure_bsp_seam_link) == 0x18) ? 1 : -1];
typedef char structure_bsp_seam_link_material_offset[(offsetof(s_structure_bsp_seam_link, global_material_index) == 0x10) ? 1 : -1];
typedef char structure_bsp_seam_link_seam_offset[(offsetof(s_structure_bsp_seam_link, seam_index) == 0x14) ? 1 : -1];

#endif // PHYSICS_STRUCTURE_BSP_SEAM_LINK_TYPES_H
