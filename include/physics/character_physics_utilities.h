#ifndef PHYSICS_CHARACTER_PHYSICS_UTILITIES_H
#define PHYSICS_CHARACTER_PHYSICS_UTILITIES_H

// Functions defined in tu/blamlibxbox/character_physics_utilities.cpp that other units call.

union real_vector3d;
struct s_character_physics_flying_definition;

// @address 0x7c5d0c k_flying_angular_velocity_threshold
extern "C" { extern float k_flying_angular_velocity_threshold; }

bool character_physics_flying_update_turning(long object_index, const s_character_physics_flying_definition *definition,
	const real_vector3d *vector, const real_vector3d *vector2, float p4, float *p5);

#endif // PHYSICS_CHARACTER_PHYSICS_UTILITIES_H
