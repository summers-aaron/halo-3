#ifndef STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_TYPES_H
#define STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_TYPES_H

#include "math/matrix_math_types.h"
#include "physics/collision_bsp_types.h"
#include "tag_files/tag_types.h"

struct s_structure_instanced_geometry_resource {
    unsigned long checksum;
    real_point3d bounding_sphere_offset;
    float bounding_sphere_radius;
    collision_bsp collision_info;
    s_tag_block render_bsp;
    s_tag_block collision_mopp_codes;
    s_tag_block breakable_surface_sets;
    s_tag_block surface_to_triangle_mapping;
    s_tag_block mopp_reorder_table;
    short mesh_index;
    short compression_index;
    float global_lightmap_resolution_scale;
};

#endif
