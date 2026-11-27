#ifndef MATH_MATRIX_MATH_NOALIAS_H
#define MATH_MATRIX_MATH_NOALIAS_H

struct real_matrix4x3;
union real_vector3d;

void matrix4x3_rotation_from_axis_and_angle(real_matrix4x3 *matrix, const real_vector3d *axis, float sine, float cosine);
extern "C" void matrix4x3_compute_product(const real_matrix4x3 *matrix1, const real_matrix4x3 *matrix2, real_matrix4x3 *result);
real_vector3d *matrix4x3_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result);

#endif // MATH_MATRIX_MATH_NOALIAS_H
