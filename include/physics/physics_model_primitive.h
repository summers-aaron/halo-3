#ifndef PHYSICS_PHYSICS_MODEL_PRIMITIVE_H
#define PHYSICS_PHYSICS_MODEL_PRIMITIVE_H

#include <stddef.h>

// March hkShape_get_primitive (82a67720): the shared serialized prefix is
// exactly 32 bytes before its shape. The collector reads phantom_index at +30.
// Character sphere/capsule definitions retain the same prefix.
struct s_physics_model_primitive
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
};
typedef char physics_model_primitive_size[(sizeof(s_physics_model_primitive) == 0x20) ? 1 : -1];
typedef char physics_model_primitive_phantom_offset[(offsetof(s_physics_model_primitive, phantom_index) == 30) ? 1 : -1];

#endif
