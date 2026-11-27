#ifndef MATH_REAL_MATH_GLOBALS_H
#define MATH_REAL_MATH_GLOBALS_H

#include "math/real_math_types.h"

struct real_matrix4x3;

// Global constant pointers owned by real_math.obj.
extern const real_point2d *global_origin2d;
extern const real_vector2d *global_zero_vector2d;
extern const real_vector2d *global_forward2d;
extern const real_vector2d *global_left2d;
extern const real_point3d *global_origin3d;
extern const real_point3d *global_z_axis3d;
extern const real_vector3d *global_zero_vector3d;
extern const real_vector3d *global_forward3d;
extern const real_vector3d *global_left3d;
extern const real_vector3d *global_up3d;
extern const real_vector3d *global_backward3d;
extern const real_vector3d *global_right3d;
extern const real_vector3d *global_down3d;
extern const real_euler_angles2d *global_zero_angles2d;
extern const real_euler_angles3d *global_zero_angles3d;
extern const real_quaternion *global_identity_quaternion;
extern const real_orientation *global_identity_orientation;
extern const real_matrix4x3 *global_identity4x3;
extern const real_rectangle2d *global_null_rectangle2d;
extern const real_rectangle3d *global_null_rectangle3d;

extern const short global_projection3d_mappings[3][2][3];
extern const short global_projection3d_inverse_mappings[3][2][3];

extern "C" float k_two_pi_global;

#endif // MATH_REAL_MATH_GLOBALS_H
