#ifndef PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_INTERNALS_H
#define PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_INTERNALS_H

#include <stddef.h>
#include "phys/phys_base.h"
#include "physics/character_physics_shapes.h"
#include "tag_files/tag_groups.h"

#include "physics/physics_model_shape_internal.h"

struct sphere_definition
{
    s_character_physics_shape_properties properties;
    c_sphere_shape sphere;
#ifndef HALO3_64BIT
    char alignment_storage[12];
#endif
    c_transform_shape transform;
};

struct pill_definition
{
    s_character_physics_shape_properties properties;
    c_capsule_shape pill;
};

static __forceinline void *character_physics_definitions_get_element(const tag_block *block, long index, long size)
{
    return (char *)block->address + index * size;
}

#endif // PHYSICS_CHARACTER_PHYSICS_DEFINITIONS_INTERNALS_H
