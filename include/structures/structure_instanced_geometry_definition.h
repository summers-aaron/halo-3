#ifndef STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_DEFINITION_H
#define STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_DEFINITION_H
#include <stddef.h>
#include "physics/collision_bsp_types.h"

// Original Halo 3 resource element (0xb8), before the MCC surface-plane block.
struct structure_instanced_geometry_definition
{
    long checksum;
    real_point3d bounding_sphere_offset;
    float bounding_sphere_radius;
    collision_bsp bsp;
    s_tag_block render_bsp;
    s_tag_block collision_mopp_codes;
    s_tag_block breakable_surfaces;
    s_tag_block surface_planes;
    s_tag_block surface_to_triangle_mapping;
    short mesh_index;
    short compression_index;
    float global_lightmap_resolution_scale;
};
typedef char structure_instance_definition_size[(sizeof(structure_instanced_geometry_definition) == 0xb8) ? 1 : -1];
typedef char structure_instance_definition_bsp_offset[(offsetof(structure_instanced_geometry_definition, bsp) == 0x14) ? 1 : -1];
#endif
