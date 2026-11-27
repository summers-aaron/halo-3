#ifndef PHYSICS_POINT_PHYSICS_H
#define PHYSICS_POINT_PHYSICS_H

#include <stddef.h>
#include "physics/point_physics_globals.h"

// Functions defined in tu/blamlibxbox/point_physics.cpp that other units call.

class c_global_material_type;
union real_point3d;
union real_vector3d;
struct s_location;

enum e_point_physics_definition_flags
{
    _point_physics_flamethrower_particle_collision_bit = 0,
    _point_physics_collides_with_structures_bit,
    _point_physics_collides_with_water_surface_bit,
    _point_physics_uses_simple_wind_bit,
    _point_physics_uses_damped_wind_bit,
    _point_physics_no_gravity_bit,

    k_point_physics_definition_flags_count
};

// The 'pphy' tag. Point update uses flags, the runtime mass, density (with the air density, the
// gravity damping), air friction (the current blend) and surface friction / elasticity (the bounce); the flag
// bits are the ones it tests (structures, water surface, no gravity). Names follow the Halo 1 pphy layout (64 bytes,
// the same layout); runtime_inverse_density and water_friction are not used by the update.
struct point_physics_definition
{
    unsigned long flags;
    float runtime_mass_over_radius_cubed;
    float runtime_inverse_density;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    float density;
    float air_friction;
    float water_friction;
    float surface_friction;
    float elasticity;
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char point_physics_definition_density_offset[(offsetof(point_physics_definition, density) == 0x20) ? 1 : -1];
typedef char point_physics_definition_elasticity_offset[(offsetof(point_physics_definition, elasticity) == 0x30) ? 1 : -1];
#endif

unsigned long point_physics_update(long, const point_physics_definition *definition, s_location *location, short,
    real_point3d *out_point, real_vector3d *in_point, const real_vector3d *, real_vector3d *,
    c_global_material_type *, float, float scale);

#endif // PHYSICS_POINT_PHYSICS_H
