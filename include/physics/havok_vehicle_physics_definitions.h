#ifndef PHYSICS_HAVOK_VEHICLE_PHYSICS_DEFINITIONS_H
#define PHYSICS_HAVOK_VEHICLE_PHYSICS_DEFINITIONS_H

#include "phys/phys_multi_sphere_shape.h"
#include "math/real_math_types.h"
#include <stddef.h>

struct s_havok_vehicle_physics_shapes
{
    phys_list_shape list_shape;
    long shape_count;
    long flags;
    real_rectangle3d phantom_aabb;
    phys_multi_sphere_shape spheres[4];
    phys_list_shape::child_shape_entry child_infos[4];
};
typedef char havok_vehicle_physics_shapes_size[(sizeof(void *) != 4 || ((sizeof(s_havok_vehicle_physics_shapes) == 816))) ? 1 : -1];
typedef char havok_vehicle_physics_shapes_spheres_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_vehicle_physics_shapes, spheres) == 0x70))) ? 1 : -1];

// Functions defined in tu/blamlibxbox/havok_vehicle_physics_definitions.cpp that other units call.

union real_rectangle3d;
struct s_havok_vehicle_physics_definition;

bool havok_vehicle_physics_get_phantom_aabb(const s_havok_vehicle_physics_definition *definition, real_rectangle3d *aabb);
void havok_vehicle_physics_definition_fixup(s_havok_vehicle_physics_definition *definition);

#endif // PHYSICS_HAVOK_VEHICLE_PHYSICS_DEFINITIONS_H
