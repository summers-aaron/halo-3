#ifndef PHYSICS_CHARACTER_PHYSICS_GROUND_HELPERS_H
#define PHYSICS_CHARACTER_PHYSICS_GROUND_HELPERS_H
#include "math/real_math_types.h"
class c_havok_component;
bool calculate_localized_physics_space_linear_velocity(long havok_component_index, long rigid_body_index,
    const real_point3d *point, const real_matrix4x3 *matrix, real_matrix4x3 *out_matrix, real_vector3d *origin,
    // match: 0x25a478: declaration must mangle identically to the restrict-qualified definition
    real_vector3d *__restrict linear_velocity, real_vector3d *linear_velocity_delta, real_matrix3x3 *rotation);
extern "C" unsigned char havok_component_has_active_entity(c_havok_component *component);
#endif
