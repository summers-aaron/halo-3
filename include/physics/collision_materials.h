#ifndef PHYSICS_COLLISION_MATERIALS_H
#define PHYSICS_COLLISION_MATERIALS_H

#include "game/materials.h"
#include "tag_files/tag_types.h"
#include <stddef.h>

// Collision material entry: render method, global material type, and surface mapping indices.
// TagTool ScenarioStructureBsp.CollisionMaterial corroborates the remaining fields.
struct s_collision_material
{
    s_tag_reference render_method;
    c_global_material_type global_material;
    short conveyor_surface_index;
    short seam_mapping_index;
    unsigned short flags;
};

typedef char collision_material_size[(sizeof(s_collision_material) == 0x18) ? 1 : -1];
typedef char collision_material_global_offset[(offsetof(s_collision_material, global_material) == 0x10) ? 1 : -1];

union real_point3d;

// Material queries defined in tu/blamlibxbox/collisions.cpp.
extern "C" c_global_material_type collision_get_bsp_surface_material(long structure_bsp_index,
    const real_point3d *position, long surface_index, long instance_index);

struct collision_bsp;
extern "C" c_global_material_type collision_get_surface_material(long structure_bsp_index,
    const collision_bsp *structure_bsp_tag, const real_point3d *position, long surface_index, long instance_index);

#endif // PHYSICS_COLLISION_MATERIALS_H
