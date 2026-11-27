#ifndef MATH_REAL_MATH_H
#define MATH_REAL_MATH_H

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"

const float k_real_epsilon = 0.0001f;
const float k_real_tiny_epsilon = 0.000001f;
const float k_real_zero = 0.0f;
const float k_real_one = 1.0f;
const float k_real_half = 0.5f;
const float k_real_two = 2.0f;
const float k_pi = 3.14159265358979323846f;
const float k_two_pi = 6.28318530717958647692f;
const float k_half_pi = 1.57079632679489661923f;
const float k_degrees_to_radians = 0.017453292519943295769f;
const float k_radians_to_degrees = 57.2957795130823208768f;

#ifndef k_valid_real_epsilon
#define k_valid_real_epsilon 0.001f
#endif
#ifndef VALID_REAL
#define VALID_REAL(x) (((*(const long *)&(x)) & 0x7f800000) != 0x7f800000)
#endif
#ifndef VALID_REALCMP
#define VALID_REALCMP(a, b) (VALID_REAL((a) - (b)) && fabsf((a) - (b)) < k_valid_real_epsilon)
#endif

#include "math/real_math_types.h"

#include "math/real_math_globals.h"

// match: 0x512328: normalize2d writes only through vector; callers keep loaded values in registers across the call.
__declspec(noalias) float normalize2d(real_vector2d *vector);

#ifndef REAL_MATH_DEFINING_TU
// match: 0x300158: normalize3d writes only through vector; callers keep loaded values in registers across the call.
__declspec(noalias) float normalize3d(real_vector3d *vector);
#endif

static __forceinline float square_root(float x) {
    return sqrt(x);
}

static __forceinline float reciprocal_square_root(float x) {
    return 1.0f / square_root(x);
}

static __forceinline bool valid_real(float x) {
    union { float value; long bits; } representation;
    representation.value = x;
    return (representation.bits & 0x7f800000) != 0x7f800000;
}

static __forceinline bool valid_realcmp(float a, float b) {
    float difference = a - b;
    return valid_real(difference) && fabs(difference) < k_valid_real_epsilon;
}

static __forceinline float interpolate_linear(float start_value, float end_value, float interpolation_factor) {
    return start_value + (end_value - start_value) * interpolation_factor;
}

static __forceinline real_point2d *set_real_point2d(real_point2d *point, float x, float y) {
    point->x = x;
    point->y = y;
    return point;
}

static __forceinline real_point3d *set_real_point3d(real_point3d *point, float x, float y, float z) {
    point->x = x;
    point->y = y;
    point->z = z;
    return point;
}

static __forceinline real_vector2d *set_real_vector2d(real_vector2d *vector, float i, float j) {
    vector->i = i;
    vector->j = j;
    return vector;
}

static __forceinline real_vector3d *set_real_vector3d(real_vector3d *vector, float i, float j, float k) {
    vector->i = i;
    vector->j = j;
    vector->k = k;
    return vector;
}

static __forceinline float dot_product2d(const real_vector2d *a, const real_vector2d *b) {
    return a->i * b->i + a->j * b->j;
}

static __forceinline float dot_product2d(const real_vector2d *a, const real_point2d *b) {
    return a->i * b->x + a->j * b->y;
}

static __forceinline float cross_product2d(const real_vector2d *a, const real_vector2d *b) {
    return a->i * b->j - a->j * b->i;
}

static __forceinline float magnitude_squared2d(const real_vector2d *a) {
    return a->i * a->i + a->j * a->j;
}

static __forceinline float magnitude2d(const real_vector2d *a) {
    return square_root(magnitude_squared2d(a));
}

static __forceinline real_vector2d *scale_vector2d(const real_vector2d *vector, float scale, real_vector2d *result) {
    result->i = scale * vector->i;
    result->j = scale * vector->j;
    return result;
}

static __forceinline real_vector2d *add_vectors2d(const real_vector2d *a, const real_vector2d *b, real_vector2d *result) {
    result->i = a->i + b->i;
    result->j = a->j + b->j;
    return result;
}

static __forceinline real_vector2d *negate_vector2d(const real_vector2d *a, real_vector2d *result) {
    result->i = -a->i;
    result->j = -a->j;
    return result;
}

static __forceinline real_vector2d *subtract_vectors2d(const real_vector2d *a, const real_vector2d *b, real_vector2d *result) {
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    return result;
}

static __forceinline real_vector2d *vector_from_points2d(const real_point2d *a, const real_point2d *b, real_vector2d *result) {
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    return result;
}

static __forceinline float distance_squared2d(const real_point2d *a, const real_point2d *b) {
    real_vector2d vector;
    return magnitude_squared2d(vector_from_points2d(a, b, &vector));
}

static __forceinline float distance2d(const real_point2d *a, const real_point2d *b) {
    return square_root(distance_squared2d(a, b));
}

static __forceinline real_point2d *point_from_line2d(const real_point2d *point, const real_vector2d *vector, float t, real_point2d *result) {
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    return result;
}

static __forceinline real_vector2d *point_from_line2d(const real_vector2d *point, const real_vector2d *vector, float t, real_vector2d *result) {
    result->i = point->i + vector->i * t;
    result->j = point->j + vector->j * t;
    return result;
}

static __forceinline float dot_product3d(const real_vector3d *a, const real_vector3d *b) {
    return a->i * b->i + a->j * b->j + a->k * b->k;
}

static __forceinline float dot_product3d(const real_point3d *a, const real_vector3d *b) {
    return a->x * b->i + a->y * b->j + a->z * b->k;
}

static __forceinline float dot_product3d(const real_vector3d *a, const real_point3d *b) {
    return a->i * b->x + a->j * b->y + a->k * b->z;
}

static __forceinline float magnitude_squared3d(const real_vector3d *a) {
    return a->i * a->i + a->j * a->j + a->k * a->k;
}

static __forceinline float magnitude3d(const real_vector3d *a) {
    return square_root(magnitude_squared3d(a));
}

static __forceinline real_vector3d *cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = a->j * b->k - a->k * b->j;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline float cross_product_magnitude3d(const real_vector3d *a, const real_vector3d *b) {
    real_vector3d cross;
    return magnitude3d(cross_product3d(a, b, &cross));
}

static __forceinline float triple_product3d(const real_vector3d *a, const real_vector3d *b, const real_vector3d *c) {
    real_vector3d cross;
    return dot_product3d(cross_product3d(a, b, &cross), c);
}

static __forceinline real_vector3d *scale_vector3d(const real_vector3d *vector, float scale, real_vector3d *result) {
    result->i = scale * vector->i;
    result->j = scale * vector->j;
    result->k = scale * vector->k;
    return result;
}

static __forceinline real_vector3d *add_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    result->i = a->i + b->i;
    result->j = a->j + b->j;
    result->k = a->k + b->k;
    return result;
}

static __forceinline real_vector3d *add_vectors3d(const real_vector3d *a, const real_point3d *b, real_vector3d *result) {
    result->i = a->i + b->x;
    result->j = a->j + b->y;
    result->k = a->k + b->z;
    return result;
}

static __forceinline real_vector3d *subtract_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
    return result;
}

static __forceinline real_vector3d *subtract_vectors3d(const real_vector3d *a, const real_point3d *b, real_vector3d *result) {
    result->i = a->i - b->x;
    result->j = a->j - b->y;
    result->k = a->k - b->z;
    return result;
}

static __forceinline real_vector3d *multiply_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result) {
    result->i = a->i * b->i;
    result->j = a->j * b->j;
    result->k = a->k * b->k;
    return result;
}

static __forceinline real_vector3d *negate_vector3d(const real_vector3d *a, real_vector3d *result) {
    result->i = -a->i;
    result->j = -a->j;
    result->k = -a->k;
    return result;
}

static __forceinline real_vector3d *vector_from_points3d(const real_point3d *a, const real_point3d *b, real_vector3d *result) {
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
    return result;
}

static __forceinline float distance_squared3d(const real_point3d *a, const real_point3d *b) {
    real_vector3d vector;
    return magnitude_squared3d(vector_from_points3d(a, b, &vector));
}

static __forceinline float distance3d(const real_point3d *a, const real_point3d *b) {
    return square_root(distance_squared3d(a, b));
}

static __forceinline real_rectangle2d *set_real_rectangle2d(
    real_rectangle2d *rectangle, real x0, real x1, real y0, real y1)
{
    rectangle->x0 = x0;
    rectangle->x1 = x1;
    rectangle->y0 = y0;
    rectangle->y1 = y1;
    return rectangle;
}

static __forceinline real_point3d *point_from_line3d(const real_point3d *point, const real_vector3d *vector, float t, real_point3d *result) {
    result->x = point->x + vector->i * t;
    result->y = point->y + vector->j * t;
    result->z = point->z + vector->k * t;
    return result;
}

static __forceinline real_vector3d *point_from_line3d(const real_vector3d *point, const real_vector3d *vector, float t, real_vector3d *result) {
    result->i = point->i + vector->i * t;
    result->j = point->j + vector->j * t;
    result->k = point->k + vector->k * t;
    return result;
}

static __forceinline real_point2d *midpoint2d(const real_point2d *a, const real_point2d *b, real_point2d *result) {
    result->x = 0.5f * (a->x + b->x);
    result->y = 0.5f * (a->y + b->y);
    return result;
}

static __forceinline real_point3d *midpoint3d(const real_point3d *a, const real_point3d *b, real_point3d *result) {
    result->x = 0.5f * (a->x + b->x);
    result->y = 0.5f * (a->y + b->y);
    result->z = 0.5f * (a->z + b->z);
    return result;
}

#include "math/real_math_limit.h"

static __forceinline float dot_product4d(const real_vector4d *a, const real_vector4d *b) {
    return a->i * b->i + a->j * b->j + a->k * b->k + a->l * b->l;
}

static __forceinline float magnitude_squared4d(const real_vector4d *a) {
    return a->i * a->i + a->j * a->j + a->k * a->k + a->l * a->l;
}

static __forceinline float magnitude4d(const real_vector4d *a) {
    return square_root(magnitude_squared4d(a));
}

static __forceinline float plane2d_distance_to_point(const real_plane2d *plane, const real_point2d *point) {
    return point->x * plane->n.i + point->y * plane->n.j - plane->d;
}

// Collision queries inline this calculation. The transparent renderer emits the external definition used by BSP traversal.
#ifdef REAL_MATH_PLANE_DISTANCE_LINKAGE
REAL_MATH_PLANE_DISTANCE_LINKAGE float plane3d_distance_to_point(const real_plane3d *plane, const real_point3d *point) {
    return dot_product3d(point, &plane->n) - plane->d;
}
#else
float plane3d_distance_to_point(const real_plane3d *plane, const real_point3d *point);
#endif

static __forceinline real_plane3d *plane3d_from_point_and_normal(real_plane3d *plane, const real_point3d *point, const real_vector3d *normal) {
    plane->n = *normal;
    plane->d = dot_product3d(point, &plane->n);
    return plane;
}

static __forceinline real_plane3d *plane3d_negate(const real_plane3d *plane, real_plane3d *result) {
    negate_vector3d(&plane->n, &result->n);
    result->d = -plane->d;
    return result;
}

// Defined out of line in projectiles.cpp.
float vector_intersect_plane3d(const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane);

static __forceinline bool point_in_sphere(const real_point3d *point, const real_point3d *center, float radius) {
    return distance_squared3d(point, center) <= radius * radius;
}

static __forceinline bool point_in_circle(const real_point2d *point, const real_point2d *center, float radius) {
    return distance_squared2d(point, center) <= radius * radius;
}

bool valid_real_vector3d(const real_vector3d *vector);

static __forceinline float real_pin(float x, float lo, float hi) {
    return PIN(x, lo, hi);
}

#include "math/real_math_functions.h"

#endif // MATH_REAL_MATH_H
