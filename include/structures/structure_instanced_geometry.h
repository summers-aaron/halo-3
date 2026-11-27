#ifndef STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_H
#define STRUCTURES_STRUCTURE_INSTANCED_GEOMETRY_H

#include "cseries/cseries.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

struct s_collision_bsp_physics_definition;

// Instance placements store the transform and world bounds;
// render_structure supplies the flags and cubemap; visibility supplies the fade limits.
struct structure_instanced_geometry_instance
{
    real_matrix4x3 transform;
    short definition_index;
    unsigned short flags;
    short lightmap_texcoords_index;
    // Filler between the lightmap texcoords index and the world bounds; no in-tree reader uses these bytes.
    char bounding_sphere_padding[6];
    real_point3d world_bounding_sphere_center;
    float world_bounding_sphere_radius;
    unsigned long checksum;
    string_id name;
    short pathfinding_policy;
    short lightmap_policy;
    float lightmap_resolution_scale;
    union
    {
        c_typed_tag_block<s_collision_bsp_physics_definition> physics;
        struct
        {
            long physics_count;
            TAG_POINTER(struct havok_runtime_pool_instance_collision) collision;
            long physics_definition;
        };
    };
    short fade_start;
    short fade_end;
    short cubemap_index;
    // Trailing filler after the cubemap index; no in-tree reader uses these bytes.
    char instance_tail_padding[6];
};

typedef char structure_instance_lightmap_texcoords_offset[(offsetof(structure_instanced_geometry_instance, lightmap_texcoords_index) == 0x38) ? 1 : -1];
typedef char structure_instance_lightmap_policy_offset[(offsetof(structure_instanced_geometry_instance, lightmap_policy) == 0x5a) ? 1 : -1];
typedef char structure_instance_physics_offset[(offsetof(structure_instanced_geometry_instance, physics) == 0x60) ? 1 : -1];

typedef char structure_instance_collision_offset[(offsetof(structure_instanced_geometry_instance, collision) == 0x64) ? 1 : -1];

typedef char structure_instance_layout_1[(sizeof(structure_instanced_geometry_instance) == 0x78) ? 1 : -1];
typedef char structure_instance_layout_2[(offsetof(structure_instanced_geometry_instance, definition_index) == 0x34) ? 1 : -1];
typedef char structure_instance_layout_3[(offsetof(structure_instanced_geometry_instance, world_bounding_sphere_center) == 0x40) ? 1 : -1];
typedef char structure_instance_layout_4[(offsetof(structure_instanced_geometry_instance, world_bounding_sphere_radius) == 0x4c) ? 1 : -1];
typedef char structure_instance_checksum_offset[(offsetof(structure_instanced_geometry_instance, checksum) == 0x50) ? 1 : -1];
typedef char structure_instance_name_offset[(offsetof(structure_instanced_geometry_instance, name) == 0x54) ? 1 : -1];
typedef char structure_instance_pathfinding_policy_offset[(offsetof(structure_instanced_geometry_instance, pathfinding_policy) == 0x58) ? 1 : -1];
typedef char structure_instance_lightmap_resolution_scale_offset[(offsetof(structure_instanced_geometry_instance, lightmap_resolution_scale) == 0x5c) ? 1 : -1];
typedef char structure_instance_layout_5[(offsetof(structure_instanced_geometry_instance, fade_start) == 0x6c) ? 1 : -1];
typedef char structure_instance_layout_6[(offsetof(structure_instanced_geometry_instance, cubemap_index) == 0x70) ? 1 : -1];

#endif
