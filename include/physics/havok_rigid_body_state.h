#ifndef PHYSICS_HAVOK_RIGID_BODY_STATE_H
#define PHYSICS_HAVOK_RIGID_BODY_STATE_H

#include "cseries/cseries.h"
#include "math/matrix_math_types.h"

// Layout used by c_havok_component::save_rigid_body_state and restore_rigid_body_state.
struct s_havok_component_rigid_body_state
{
    c_static_flags<64> valid_rigid_bodies;
    real_matrix4x3 transforms[64];
    real_vector3d linear_velocities[64];
    real_vector3d angular_velocities[64];
};

typedef char havok_rigid_body_state_size[(sizeof(s_havok_component_rigid_body_state) == 4872) ? 1 : -1];

#endif
