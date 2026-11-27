#ifndef MATH_MATRIX_MATH_FUNCTIONS_H
#define MATH_MATRIX_MATH_FUNCTIONS_H

#include "math/real_math_types.h"
#include "math/matrix_math_types.h"

extern const real_matrix4x3 *global_identity4x3;

void matrix4x3_identity(real_matrix4x3 *matrix);

// Verified C++-mangled out-of-line functions in matrix_math.obj:
real_matrix3x3 *matrix3x3_rotation_from_quaternion(real_matrix3x3 *matrix, const real_quaternion *quaternion);
real_quaternion *matrix3x3_rotation_to_quaternion(const real_matrix3x3 *matrix, real_quaternion *quaternion);
real_matrix3x3 *matrix3x3_multiply(const real_matrix3x3 *a, const real_matrix3x3 *b, real_matrix3x3 *out);
real_matrix3x3 *matrix3x3_transpose(const real_matrix3x3 *matrix, real_matrix3x3 *result);
real_vector3d *matrix3x3_transform_vector(const real_matrix3x3 *matrix, const real_vector3d *vector, real_vector3d *result);
real_matrix3x3 *matrix3x3_from_axis_and_angle(real_matrix3x3 *matrix, const real_vector3d *axis, float sine, float cosine);
bool matrix3x3_normalize(real_matrix3x3 *matrix3x3);
real_matrix3x3 *matrix3x3_inverse(const real_matrix3x3 *matrix3x3, float determinant, real_matrix3x3 *result);
real_vector3d *matrix3x3_inverse_transform_vector(const real_matrix3x3 *matrix, const real_vector3d *vector, real_vector3d *result);
bool valid_real_vector3d_axes3(const real_vector3d *a, const real_vector3d *b, const real_vector3d *c);

void matrix4x3_from_point_and_quaternion(real_matrix4x3 *matrix, const real_point3d *point, const real_quaternion *quaternion);
void matrix4x3_multiply(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix4x3 *result);
real_point3d *matrix4x3_inverse_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result);
real_vector3d *matrix4x3_inverse_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result);
real_point3d *matrix4x3_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result);

// Local inline body for inlined call sites, named apart from the out-of-line function above:
// this build emits bl for cross-unit calls, so inlined sites need this copy.
static __forceinline real_point3d *inl_matrix4x3_transform_point(
    const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    const float &scale = matrix->scale;
    float x = point->x;
    float y = point->y;
    float z = point->z;
    x *= scale;
    y *= scale;
    z *= scale;
    result->x = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i + matrix->position.x;
    result->y = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j + matrix->position.y;
    result->z = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k + matrix->position.z;
    return result;
}
real_plane3d *matrix4x3_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result);
void matrix4x3_to_orientation(const real_matrix4x3 *matrix, real_orientation *orientation);
void matrix4x3_from_orientation(real_matrix4x3 *matrix, const real_orientation *orientation);
real_vector3d *matrix4x3_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result);
static __forceinline real_vector3d *matrix4x3_transform_normal(const real_matrix4x3 *matrix, const real_vector3d *normal, real_vector3d *result)
{
    float i = normal->i;
    float j = normal->j;
    float k = normal->k;
    result->i = i * matrix->forward.i + j * matrix->left.i + k * matrix->up.i;
    result->j = i * matrix->forward.j + j * matrix->left.j + k * matrix->up.j;
    result->k = i * matrix->forward.k + j * matrix->left.k + k * matrix->up.k;
    return result;
}
void matrix4x3_rotation_from_vectors(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up);
inline void matrix4x3_from_point_and_vectors(real_matrix4x3 *matrix, const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = *point;
}
void matrix4x3_inverse(const real_matrix4x3 *matrix, real_matrix4x3 *result);
void matrix4x3_from_point_and_nonaxial_forward_and_left(real_matrix4x3 *matrix, const real_point3d *point,
    const real_vector3d *forward, const real_vector3d *left);
void matrix4x3_from_point_and_nonaxial_vectors(real_matrix4x3 *matrix, const real_point3d *point, const real_vector3d *forward, const real_vector3d *up);
void matrix4x3_rotation_from_angles(real_matrix4x3 *matrix, float x_angle, float y_angle, float z_angle);
void matrix4x3_rotation_from_axis_and_angle(real_matrix4x3 *matrix, const real_vector3d *axis, float sine, float cosine);
void matrix4x3_transform_points(const real_matrix4x3 *matrix, long point_count, const real_point3d points[], real_point3d transformed_points[]);
void matrix4x3_rotation_between_vectors(real_matrix4x3 *matrix, const real_vector3d *from, const real_vector3d *to);
void matrix4x3_from_point_and_nonaxial_vectors_using_up(real_matrix4x3 *matrix, const real_point3d *point,
    const real_vector3d *forward, const real_vector3d *up);
real_plane3d *matrix4x3_inverse_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result);

// Functions defined in tu/blamlibxbox/matrix_math.cpp that other units call.

struct real_matrix3x4;

extern "C" void matrix4x3_compute_product(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix4x3 *result);
extern "C" void matrix4x3_rotation_to_angles(const real_quaternion *quaternion, real_euler_angles3d *angles);
extern "C" void matrix4x3_multiply_to_matrix3x4(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix3x4 *result);
extern "C" void matrix3x3_rotation_from_angles(real_matrix3x3 *matrix, float yaw, float pitch, float roll);
extern "C" void matrix4x3_multiply_by_orientation(const real_matrix4x3 *matrix, const real_orientation *orientation, real_matrix4x3 *result);

#endif // MATH_MATRIX_MATH_FUNCTIONS_H
