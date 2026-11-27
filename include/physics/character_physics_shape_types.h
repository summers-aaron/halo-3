#ifndef PHYSICS_CHARACTER_PHYSICS_SHAPE_TYPES_H
#define PHYSICS_CHARACTER_PHYSICS_SHAPE_TYPES_H

#include <stddef.h>
#include "phys/phys_collide.h"
#include "physics/character_physics_shapes.h"

struct s_character_physics_pill
{
    s_character_physics_shape_properties properties;
    phys_capsule_shape shape;
};
typedef char character_physics_pill_shape_offset[(offsetof(s_character_physics_pill, shape) == 0x20) ? 1 : -1];

// Dead-sphere shape: the serialized shape properties precede the
// sphere and its aligned convex-translate shape.
struct s_character_physics_dead_sphere
{
    long name;
    short material_index;
    short runtime_material_type;
    float relative_mass_scale;
    float friction;
    float restitution;
    float volume;
    float mass;
    short mass_distribution_index;
    signed char phantom_index;
    signed char proxy_collision_group;
    phys_sphere_shape shape;
    phys_convex_offset_shape transform;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char character_physics_dead_sphere_shape_offset[(sizeof(void *) != 4 || offsetof(s_character_physics_dead_sphere, shape) == 0x20) ? 1 : -1];
typedef char character_physics_dead_sphere_transform_offset[(sizeof(void *) != 4 || offsetof(s_character_physics_dead_sphere, transform) == 0x40) ? 1 : -1];
typedef char character_physics_dead_sphere_size[(sizeof(void *) != 4 || sizeof(s_character_physics_dead_sphere) == 0x70) ? 1 : -1];
#endif

// Live-sphere set: the serialized shape properties followed by the
// standing and crouching sphere shapes, each padded to 0x20.
struct s_character_physics_spheres
{
    s_character_physics_shape_properties properties;
    phys_sphere_shape standing_shape;
#if !defined(_WIN64) && !defined(__LP64__)
    char standing_padding[0x20 - sizeof(phys_sphere_shape)];
#endif
    phys_sphere_shape crouching_shape;
#if !defined(_WIN64) && !defined(__LP64__)
    char crouching_padding[0x20 - sizeof(phys_sphere_shape)];
#endif
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char character_physics_spheres_standing_offset[(sizeof(void *) != 4 || offsetof(s_character_physics_spheres, standing_shape) == 0x20) ? 1 : -1];
typedef char character_physics_spheres_crouching_offset[(sizeof(void *) != 4 || offsetof(s_character_physics_spheres, crouching_shape) == 0x40) ? 1 : -1];
typedef char character_physics_spheres_size[(sizeof(void *) != 4 || sizeof(s_character_physics_spheres) == 0x60) ? 1 : -1];
#endif

#endif
