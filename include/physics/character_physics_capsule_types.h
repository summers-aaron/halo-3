#ifndef PHYSICS_CHARACTER_PHYSICS_CAPSULE_TYPES_H
#define PHYSICS_CHARACTER_PHYSICS_CAPSULE_TYPES_H

#include <stddef.h>
#include "physics/character_physics_shapes.h"
#include "phys/phys_collide.h"

// Capsule tag block: properties followed by the runtime capsule shape.
struct s_character_physics_capsule
{
    s_character_physics_shape_properties properties;
    phys_capsule_shape shape;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char character_physics_capsule_shape_offset[(offsetof(s_character_physics_capsule, shape) == 0x20) ? 1 : -1];
typedef char character_physics_capsule_size[(sizeof(void *) != 4 || ((sizeof(s_character_physics_capsule) == 0x60))) ? 1 : -1];
#endif

#endif
