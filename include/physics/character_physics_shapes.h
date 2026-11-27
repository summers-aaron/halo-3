#ifndef PHYSICS_CHARACTER_PHYSICS_SHAPES_H
#define PHYSICS_CHARACTER_PHYSICS_SHAPES_H

// Serialized shape properties shared by character sphere and capsule blocks.
// Layout agrees with s_character_physics_dead_sphere in character_physics_shape_types.h.
struct s_character_physics_shape_properties
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
typedef char character_physics_shape_properties_size[(sizeof(s_character_physics_shape_properties) == 0x20) ? 1 : -1];

#endif
