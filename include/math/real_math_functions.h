#ifndef MATH_REAL_MATH_FUNCTIONS_H
#define MATH_REAL_MATH_FUNCTIONS_H

#include "math/real_math_types.h"

// Verified C++-mangled out-of-line functions in real_math.obj (and COMDATs):
float normalize2d(real_vector2d *vector);
float normalize3d(real_vector3d *vector);
real_vector3d *vector3d_from_euler_angles2d(real_vector3d *vector, const real_euler_angles2d *angles);
real_vector3d *generate_up_vector3d(const real_vector3d *forward, real_vector3d *up);
// real_math.cpp emits the external definition. Firing-position evaluation uses the inline form.
#if defined(REAL_MATH_DEFINING_TU) || defined(REAL_MATH_INLINE_ROTATION)
#if defined(REAL_MATH_INLINE_ROTATION)
static inline
#endif
real_vector3d *rotate_vector_about_axis(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
    float scale = (axis->i * vector->i + axis->j * vector->j + axis->k * vector->k) * (1.0f - cosine);
    // Save all three cross-product components before overwriting the input vector.
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_j = vector->k * axis->i - vector->i * axis->k;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = axis->i * scale + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    vector->k = axis->k * scale + vector->k * cosine - cross_k * sine;
    return vector;
}
#else
real_vector3d *rotate_vector_about_axis(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine);
#endif
bool sphere_intersects_cone3d(const real_point3d *point, float radius, const real_point3d *cone_point,
    const real_vector3d *cone_direction, float cone_height, float cone_sine, float cone_cosine);
bool point_in_rectangle3d(const real_point3d *point, const real_rectangle3d *bounds);
bool fast_vector_intersects_sphere(const real_point3d *origin, const real_vector3d *vector, const real_point3d *center, float radius);
float angle_between_normals3d(const real_vector3d *a, const real_vector3d *b);
long quantize_real(float value, float min_value, float max_value, long size_in_bits, bool exact_midpoint, bool exact_endpoints);
float dequantize_real(long quantized, float min_value, float max_value, long size_in_bits, bool exact_midpoint);
float signed_angle_between_vectors2d(const real_vector2d *a, const real_vector2d *b);
float angle_between_vectors3d(const real_vector3d *a, const real_vector3d *b);
void real_vector3d_build_axes_from_vectors(const real_vector3d *forward_vector, const real_vector3d *up_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up);
real_euler_angles2d *euler_angles2d_from_vector3d(real_euler_angles2d *angles, const real_vector3d *vector);
float point_to_line_distance_squared3d(const real_point3d *point, const real_point3d *origin, const real_vector3d *vector);
float signed_angle_between_normals3d(const real_vector3d *a, const real_vector3d *b, const real_vector3d *axis);
float normalize3d_with_default(real_vector3d *v, const real_vector3d *fallback);
void angular_accelerate_to_position(real_vector3d *direction, const real_vector3d *target, float dt,
    real_vector3d *angular_velocity, float maximum_velocity, float maximum_acceleration);
void quaternion_to_angle_and_vector(const real_quaternion *quaternion, float *angle, real_vector3d *vector);
real_vector3d *perpendicular3d(const real_vector3d *vector, real_vector3d *result);
void quaternion_from_unit_vectors(real_quaternion *result, const real_vector3d *u, const real_vector3d *v);
void quaternions_multiply(const real_quaternion *a, const real_quaternion *b, real_quaternion *result);
bool pin_normal_to_cone3d(const real_vector3d *normal, const real_vector3d *cone_axis, float sine, float cosine, real_vector3d *result);
void real_vector3d_build_axes_from_forward_and_left(const real_vector3d *forward_vector, const real_vector3d *left_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up);
void quaternions_interpolate(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result);
void quaternions_interpolate_long_and_normalize(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result);
void quaternion_normalize(real_quaternion *quaternion);
void component_vectors_from_normal3d(const real_vector3d *vector, const real_vector3d *normal, real_vector3d *parallel, real_vector3d *perpendicular);
bool valid_real_normal3d(const real_vector3d *vector);
bool valid_real_vector3d_axes2(const real_vector3d *forward, const real_vector3d *up);
long plane3d_clip_polygon(const real_plane3d *plane, bool keep_positive, float tolerance, long point_count,
    const real_point3d *points, long maximum_count, real_point3d *clipped_points);
real_rectangle2d *real_rectangle2d_enclose_points(real_rectangle2d *bounds, long count, const real_point2d *const points);
long rectangle3d_build_edges(const real_rectangle3d *bounds, long maximum_edge_count, real_point3d edges[][2]);
float angle_between_vectors2d(const real_vector2d *a, const real_vector2d *b);
void real_vector3d_build_axes_from_vectors_using_up(const real_vector3d *forward_vector, const real_vector3d *up_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up);
void fast_normals_interpolate(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *result);
void normals_interpolate(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *out);
void quaternions_slerp_interpolate(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result);
void quaternion_transform_point(const real_quaternion *q, const real_point3d *p, real_point3d *result);
void orientations_multiply(const real_orientation *a, const real_orientation *b, real_orientation *result);
void orientation_inverse(const real_orientation *orientation, real_orientation *result);
void vectors3d_from_euler_angles3d(real_vector3d *forward, real_vector3d *up, const real_euler_angles3d *angles);
void vectors3d_from_euler_angles2d(real_vector3d *forward, real_vector3d *up, const real_euler_angles2d *angles);
real_vector3d *vector3d_from_angle(real_vector3d *vector, float angle);
bool point_in_pill2d(const real_point2d *point, const real_point2d *origin, const real_vector2d *vector, float radius);
unsigned char quantize_real_to_byte_lower_bound(float min_value, float max_value, float test);
void quantize_real_point3d(const real_point3d *point, const real_rectangle3d *bounds, long bits, long_point3d *quantized);
void dequantize_real_point3d(const long_point3d *quantized, const real_rectangle3d *bounds, long bits, real_point3d *point);
real_rectangle3d *real_rectangle3d_enclose_rectangle(real_rectangle3d *bounds, const real_rectangle3d *rectangle);
real_rectangle3d *real_rectangle3d_enclose_point(real_rectangle3d *rectangle, const real_point3d *point);
void quaternion_from_euler_angles(real_quaternion *quaternion, const real_euler_angles3d *euler);
real_point3d *project_point2d(const real_point2d *point, const real_plane3d *plane, short projection, bool sign, real_point3d *result);
real_vector3d *fast_normalize3d(real_vector3d *vector);
bool valid_real_point3d(const real_point3d *point);
void make_valid_real_euler_angles2d(real_euler_angles2d *angles);

// Functions defined in tu/blamlibxbox/real_math.cpp that other units call.

struct real_matrix4x3;

extern "C" void orientation_interpolate(const real_orientation *a, const real_orientation *b, float t, real_orientation *result);
extern "C" bool real_update_smooth_value(float *value, float *velocity, float target, float dt,
    float acceleration, float max_speed, float minimum, float maximum, bool wrap);
float vector_to_line_distance_squared3d(const real_point3d *point0, const real_vector3d *vector0, const real_point3d *point1, const real_vector3d *vector1);
extern "C" float real_compute_point_triangle_distance(const real_point3d *point, const real_point3d *triangle, real_point3d *closest_point);
void matrix4x3_inverse_kinematics_adjust_matrices_new(real_matrix4x3 *target, real_matrix4x3 *root, real_matrix4x3 *joint, real_matrix4x3 *end);
extern "C" void vectors_interpolate_toward(const real_vector3d *a, const real_vector3d *b, float fraction, real_vector3d *out);

// Data defined in tu/blamlibxbox/real_math.cpp that other units use.
extern const real_matrix4x3 *global_identity4x3;

void interpolate_scalar(float *value, float target, float rate);


bool vector_intersects_pill3d(const real_point3d *point0, const real_vector3d *vector0, const real_point3d *point1, const real_vector3d *vector1, float radius);
#endif // MATH_REAL_MATH_FUNCTIONS_H
