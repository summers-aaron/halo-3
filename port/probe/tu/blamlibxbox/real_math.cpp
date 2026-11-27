// blamlibxbox/real_math.obj: from the records 0x995c8 0x9aed0 0x9b278 0x9b308 0x9c2f8 0x9e400 0x9f098 0xa00b0 0xa2b80 0xa31c8 0xbb210 0xbb2a8 0xbbcf0 0xc1a90 0xd1230 0xd8608 0xd93f8 0xed2d8 0xf26b0 0xf7d68 0xf82a8 0xfe1f0 0x10aa60 0x10ae98 0x10fc30 0x10fcb0 0x10fd50 0x11a078 0x127d60 0x143bb0 0x155718 0x156290 0x17ca00 0x18ecd8 0x19c938 0x19ca28 0x19cc18 0x19cca8 0x19cde0 0x19ce70 0x19d018 0x19d0c8 0x19d1f8 0x19d2e8 0x19d360 0x19d3e0 0x19d448 0x19d4e0 0x19d880 0x19dd48 0x19dec0 0x19dfe8 0x19e068 0x19e0f0 0x19e158 0x19e328 0x19e430 0x19e818 0x19e8c8
// @flags /TP /O2 /Oc
// @address 0x9b278 ?rotate_vector_about_axis@@YAPATreal_vector3d@@PAT1@PBT1@MM@Z
// @address 0x9f098 ?fast_vector_intersects_sphere@@YA_NPBTreal_point3d@@PBTreal_vector3d@@0M@Z
// @address 0xbb210 ?signed_angle_between_vectors2d@@YAMPBTreal_vector2d@@0@Z
// @address 0xd1230 ?point_to_line_distance_squared3d@@YAMPBTreal_point3d@@0PBTreal_vector3d@@@Z
// @address 0xf82a8 ?quaternion_from_unit_vectors@@YAXPAUreal_quaternion@@PBTreal_vector3d@@1@Z
// @address 0x10fc30 orientation_interpolate
// @address 0x11a078 real_update_smooth_value
// @address 0x17ca00 ?rectangle3d_build_edges@@YAJPBTreal_rectangle3d@@JQAY01Treal_point3d@@@Z
// @address 0x18ecd8 vectors_interpolate_toward
// @address 0x19ca28 ?real_vector3d_build_axes_from_vectors_using_up@@YAXPBTreal_vector3d@@0PAT1@11@Z
// @address 0x19cca8 ?normals_interpolate@@YAXPBTreal_vector3d@@0MPAT1@@Z
// @address 0x19cde0 ?quaternions_interpolate_long_and_normalize@@YAXPBUreal_quaternion@@0MPAU1@@Z
// @address 0x19ce70 ?quaternions_slerp_interpolate@@YAXPBUreal_quaternion@@0MPAU1@@Z
// @address 0x19d448 ?point_in_pill2d@@YA_NPBTreal_point2d@@0PBTreal_vector2d@@M@Z
// @address 0x19dd48 planes_try_and_get_intersection_point
// @address 0x19dec0 planes_try_and_get_intersection_line
#include "math/real_math_globals.h"
#include "math/real_math_constants.h"
#include "math/point_math.h"
#include "math/real_math_bounds_storage.h"
#define REAL_MATH_DEFINING_TU
#include "math/real_math_interpolate.h"
#include "math/matrix_math.h"
#include "memory/data.h"
#include "effects/particle_emitter.h"
#include "physics/collision_test_flags.h"

#include "math/real_math_trigonometry.h"
#include "effects/particle_emitter_definitions.h"
#include "effects/particle_location_matrix.h"
#include "effects/particle_array_globals.h"

// One zero-filled region viewed under every zero-valued global type.
union u_zero_storage
{
    float f[6];
    real_point2d point2d;
    real_vector2d vector2d;
    real_point3d point3d;
    real_vector3d vector3d;
    real_euler_angles2d angles2d;
    real_euler_angles3d angles3d;
};
typedef char zero_storage_size[(sizeof(u_zero_storage) == 24) ? 1 : -1];
// The identity matrix plus typed 2d/point views of its rows for the axis globals.
union u_identity_matrix_storage
{
    real_matrix4x3 matrix;
    struct {
        float scale;
        union {
            real_vector3d forward;
            real_point3d forward_point;
            struct { real_vector2d forward2d; float forward_k; };
        };
        union {
            real_vector3d left;
            struct { real_vector2d left2d; float left_k; };
        };
        real_vector3d up;
        real_point3d position;
    } views;
};
typedef char identity_matrix_storage_size[(sizeof(u_identity_matrix_storage) == sizeof(real_matrix4x3)) ? 1 : -1];

static __forceinline float real_math_spawn_interpolate_x(float a, float inverse, float b, float fraction)
{
    return a * inverse + b * fraction;
}
// match: 0x1440c8: __restrict on result schedules the point loads ahead of the lerp stores
template<class t_point> void real_math_interpolate_points(const t_point *a, const t_point *b, float fraction, t_point *__restrict result)
{
    float inverse = 1.0f - fraction;
    const float &ax = +a->x;
    result->x = ax * inverse + b->x * fraction;
    result->y = a->y * inverse + b->y * fraction;
    result->z = a->z * inverse + b->z * fraction;
}
// match: 0x1440c8: __restrict on result schedules the point loads ahead of the lerp stores (as declared)
template void real_math_interpolate_points<real_point3d>(const real_point3d *, const real_point3d *, float, real_point3d *__restrict);

extern "C" {
// @address 0x221fc k_real_constant_pool
// @address 0x221fc k_real_constant_pool_one
// @address 0x2251c k_real_zero_storage
// @address 0x2251c k_real_constant_pool_zero
// @address 0x74b810 k_two_pi_global
float k_two_pi_global = 6.28318530717958647692f;
// @address 0x74b814 k_pi_global
float k_pi_global = 3.14159265358979323846f;
}

extern "C" {

// @rdata 0x8203aa90..0x8203ab78
// @address 0x3aa90 k_real_math_zero_storage
static const u_zero_storage k_real_math_zero_storage = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
// @address 0x3aaa8 k_real_math_identity_orientation_storage
#if defined(_XBOX) && !defined(__clang__)
static const real_orientation k_real_math_identity_orientation_storage = { { { 0.0f, 0.0f, 0.0f }, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f };
#else // tmp/probe (GAME-CHANGES F46): animation_definitions, objects and giants read it (animation_definition_storage.h)
extern "C" const real_orientation k_real_math_identity_orientation_storage = { { { 0.0f, 0.0f, 0.0f }, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f };
#endif
// @address 0x3aac8 k_real_math_identity_matrix_storage
static const u_identity_matrix_storage k_real_math_identity_matrix_storage =
    { 1.0f, { { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} } }, {0.0f, 0.0f, 0.0f} };
// @address 0x3aafc k_negative_identity4x3
#if defined(_XBOX) && !defined(__clang__)
static const real_matrix4x3 k_negative_identity4x3 = { 1.0f, { { {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} } }, {0.0f, 0.0f, 0.0f} };
#else // tmp/probe (GAME-CHANGES F46): atmosphere and vehicle_type_alien_fighter read it (real_math_constant_storage.h)
extern "C" const real_matrix4x3 k_negative_identity4x3 = { 1.0f, { { {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} } }, {0.0f, 0.0f, 0.0f} };
#endif
extern "C++" {
const short global_projection3d_mappings[3][2][3] =
{
    {{2, 1, 0}, {1, 2, 0}},
    {{0, 2, 1}, {2, 0, 1}},
    {{1, 0, 2}, {0, 1, 2}},
};

/* global_projection3d_inverse_mappings (owned by real_math.obj, data at 0x8203ab54): the inverse permutations */
const short global_projection3d_inverse_mappings[3][2][3] =
{
    {{2, 1, 0}, {2, 0, 1}},
    {{0, 2, 1}, {1, 2, 0}},
    {{1, 0, 2}, {0, 1, 2}},
};
}
// @rdata-end
// @address 0x217c k_real_three_fortieths_storage
const float k_real_three_fortieths_storage = 0.075f;
}

// The empty bounds: each axis holds (+FLT_MAX, -FLT_MAX), so any enclosed point shrinks it.
// @address 0x7c5d34 k_null_rectangle3d_storage
real_rectangle3d k_null_rectangle3d_storage =
{
    { 3.402823466e+38f, -3.402823466e+38f,
      3.402823466e+38f, -3.402823466e+38f,
      3.402823466e+38f, -3.402823466e+38f }
};

const real_point2d *global_origin2d = &k_real_math_zero_storage.point2d;
const real_vector2d *global_zero_vector2d = &k_real_math_zero_storage.vector2d;
const real_vector2d *global_forward2d = &k_real_math_identity_matrix_storage.views.forward2d;
const real_vector2d *global_left2d = &k_real_math_identity_matrix_storage.views.left2d;
const real_point3d *global_origin3d = &k_real_math_zero_storage.point3d;
const real_point3d *global_z_axis3d = &k_real_math_identity_matrix_storage.views.forward_point;
const real_vector3d *global_zero_vector3d = &k_real_math_zero_storage.vector3d;
const real_vector3d *global_forward3d = &k_real_math_identity_matrix_storage.matrix.forward;
const real_vector3d *global_left3d = &k_real_math_identity_matrix_storage.matrix.left;
const real_vector3d *global_up3d = &k_real_math_identity_matrix_storage.matrix.up;
const real_vector3d *global_backward3d = &k_negative_identity4x3.forward;
const real_vector3d *global_right3d = &k_negative_identity4x3.left;
const real_vector3d *global_down3d = &k_negative_identity4x3.up;
const real_euler_angles2d *global_zero_angles2d = &k_real_math_zero_storage.angles2d;
const real_euler_angles3d *global_zero_angles3d = &k_real_math_zero_storage.angles3d;
const real_quaternion *global_identity_quaternion = &k_real_math_identity_orientation_storage.rotation;
const real_matrix4x3 *global_identity4x3 = &k_real_math_identity_matrix_storage.matrix;
const real_orientation *global_identity_orientation = &k_real_math_identity_orientation_storage;
const real_rectangle2d *global_null_rectangle2d = (const real_rectangle2d *)&k_null_rectangle3d_storage;
const real_rectangle3d *global_null_rectangle3d = &k_null_rectangle3d_storage;

// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// match: 0xbbcf0: inline lets real_vector3d_build_axes_from_vectors inline the body; out of line it calls and saves r29-r31/fr30-fr31
#if defined(_XBOX) && !defined(__clang__)
inline float normalize3d(real_vector3d *vector)
#else
float normalize3d(real_vector3d *vector)
#endif
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabsf(length) >= k_real_epsilon) {
        float inverse = k_real_one / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
        return length;
    }
    return k_real_zero;
}



__forceinline real_vector3d *fast_normalize3d_inline(real_vector3d *v)
{
    float magnitude_squared = magnitude_squared3d(v);
    if (magnitude_squared != 0.0f)
    {
        float inverse = 1.0f / sqrtf(magnitude_squared);
        v->i *= inverse;
        v->j *= inverse;
        v->k *= inverse;
    }
    return v;
}

// Clamp the input to the domain of acos.
static __forceinline float real_math_arccosine_clamped(float x) { return (float)acos((double)PIN(x, -1.0f, 1.0f)); }


// @address 0x995c8 ?vector3d_from_euler_angles2d@@YAPATreal_vector3d@@PAT1@PBTreal_euler_angles2d@@@Z
real_vector3d *vector3d_from_euler_angles2d(real_vector3d *vector, const real_euler_angles2d *angles)
{
    float pitch_cosine = cosine(angles->pitch);
    vector->i = cosine(angles->yaw) * pitch_cosine;
    vector->j = sine(angles->yaw) * pitch_cosine;
    vector->k = sine(angles->pitch);
    return vector;
}

// @address 0x9aed0 ?generate_up_vector3d@@YAPATreal_vector3d@@PBT1@PAT1@@Z
real_vector3d *generate_up_vector3d(const real_vector3d *forward, real_vector3d *up)
{
    real_vector3d left;
    left.i = forward->j;
    left.j = -forward->i;
    left.k = 0.0f;
    float forward_i = forward->i;
    if (normalize3d(&left) == 0.0f)
        left = k_real_math_identity_matrix_storage.matrix.forward;
    float up_k = forward->j * left.i - forward_i * left.j;
    float up_j = forward_i * left.k - forward->k * left.i;
    float up_i = forward->k * left.j - forward->j * left.k;
    up->i = up_i;
    up->k = up_k;
    up->j = up_j;
    return up;
}

// @address 0x9c2f8 ?sphere_intersects_cone3d@@YA_NPBTreal_point3d@@M0PBTreal_vector3d@@MMM@Z
bool sphere_intersects_cone3d(const real_point3d *point, float radius, const real_point3d *cone_point, const real_vector3d *cone_direction,
    float cone_height, float cone_sine, float cone_cosine)
{
    real_vector3d vector;
    vector_from_points3d(cone_point, point, &vector);
    float distance_along = dot_product3d(cone_direction, &vector);
    if (distance_along < -radius || distance_along > radius + cone_height)
        return false;
    if ((radius * cone_sine * 2.0f + distance_along) * distance_along + radius * radius >= magnitude_squared3d(&vector) * cone_cosine * cone_cosine)
        return true;
    return false;
}

bool point_in_rectangle3d(const real_point3d *point, const real_rectangle3d *bounds)
{
    return point->x >= bounds->x0 && point->x <= bounds->x1 &&
           point->y >= bounds->y0 && point->y <= bounds->y1 &&
           point->z >= bounds->z0 && point->z <= bounds->z1;
}

// a ray (origin, vector) against a sphere (center, radius): true when the origin is inside or the ray hits
bool fast_vector_intersects_sphere(const real_point3d *origin, const real_vector3d *vector, const real_point3d *center, float radius)
{
    real_vector3d delta;
    delta.i = origin->x - center->x;
    delta.j = origin->y - center->y;
    delta.k = origin->z - center->z;
    float c = delta.i * delta.i + (delta.j * delta.j + delta.k * delta.k) - radius * radius;
    if (c < 0.0f)
        return true;
    real_vector3d ray = *vector;
    float b = dot_product3d(&ray, &delta);
    if (b >= 0.0f)
        return false;
    float a = ray.i * ray.i + (ray.j * ray.j + ray.k * ray.k);
    float discriminant = b * b - a * c;
    if (discriminant <= 0.0f)
        return false;
    float t = -a - b;
    if (t < 0.0f)
        return true;
    return t * t < discriminant;
}

/* angle_between_normals3d: the angle between two unit vectors; identical vectors give 0 */
float angle_between_normals3d(const real_vector3d *a, const real_vector3d *b)
{
    if (a->words[0] == b->words[0] && a->words[1] == b->words[1] && a->words[2] == b->words[2])
        return 0.0f;
    return real_math_arccosine_clamped(PIN(dot_product3d(a, b), -1.0f, 1.0f));
}

// the integer that stands for value in [min_value, max_value] when it is quantized to size_in_bits bits
long quantize_real(float value, float min_value, float max_value, long size_in_bits, bool exact_midpoint, bool exact_endpoints)
{
    long step_count = (1 << size_in_bits) - 1;
    if (exact_midpoint)
        step_count -= step_count % 2;
    float step = (max_value - min_value) / (float)step_count;
    long quantized = (long)((value - min_value) / step + 0.5f);
    if (exact_endpoints)
    {
        if (quantized == 0 && value != min_value)
            quantized = 1;
        else if (quantized == step_count && value != max_value)
            quantized--;
    }
    return quantized;
}

// the value a quantized integer of size_in_bits bits stands for in [min_value, max_value]
float dequantize_real(long quantized, float min_value, float max_value, long size_in_bits, bool exact_midpoint)
{
    int step_count = (1 << size_in_bits) - 1;
    if (exact_midpoint)
        step_count -= step_count % 2;
    if (quantized == 0)
        return min_value;
    if (quantized >= step_count)
        return max_value;
    return (float)(((double)(step_count - quantized) * min_value + (double)quantized * max_value) / (double)step_count);
}

// Recover the angle from cos(2a) = 2 cos^2(a) - 1.
float angle_between_vectors3d(const real_vector3d *a, const real_vector3d *b)
{
    float magnitude_product = magnitude_squared3d(a) * magnitude_squared3d(b);
    if (magnitude_product != 0.0f)
    {
        float dot = dot_product3d(a, b);
        float cosine2 = 2.0f * (dot * dot / magnitude_product) - 1.0f;
        float result = 0.5f * real_math_arccosine_clamped(real_pin(cosine2, -1.0f, 1.0f));
        if (dot < 0.0f)
            result = k_pi_global - result;
        return result;
    }
    return 0.0f;
}

/* the axes from a forward and an up vector; the forward axis falls back to global_forward3d, and a forward that runs along
   the up axis (or its negation) takes global_forward3d as its stand-in up vector for the cross product */
void real_vector3d_build_axes_from_vectors(const real_vector3d *forward_vector, const real_vector3d *up_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up)
{
    *out_forward = *forward_vector;
    if (normalize3d(out_forward) == 0.0f)
        *out_forward = *global_forward3d;
    cross_product3d(up_vector, out_forward, out_left);
    if (normalize3d(out_left) == 0.0f)
    {
        real_vector3d up;
        if (fabsf(out_forward->i - global_up3d->i) < k_real_epsilon &&
            fabsf(out_forward->j - global_up3d->j) < k_real_epsilon &&
            fabsf(out_forward->k - global_up3d->k) < k_real_epsilon ||
            fabsf(out_forward->i - global_down3d->i) < k_real_epsilon &&
                fabsf(out_forward->j - global_down3d->j) < k_real_epsilon &&
                fabsf(out_forward->k - global_down3d->k) < k_real_epsilon)
            up = *global_forward3d;
        else
            up = *global_up3d;
        cross_product3d(&up, out_forward, out_left);
        normalize3d(out_left);
    }
    cross_product3d(out_forward, out_left, out_up);
    normalize3d(out_up);
}

// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z
real_euler_angles2d *euler_angles2d_from_vector3d(real_euler_angles2d *angles, const real_vector3d *vector)
{
    angles->yaw = arctangent(vector->j, vector->i);
    angles->pitch = arctangent(vector->k, sqrtf(vector->j * vector->j + vector->i * vector->i));
    return angles;
}

float point_to_line_distance_squared3d(const real_point3d *point, const real_point3d *origin, const real_vector3d *vector)
{
    float result;
    if (magnitude_squared3d(vector) > k_real_epsilon)
    {
        real_vector3d delta;
        vector_from_points3d(origin, point, &delta);
        float t = dot_product3d(vector, &delta) / magnitude_squared3d(vector);
        if (t <= k_real_zero)
            t = k_real_zero;
        else if (t >= k_real_one)
            t = k_real_one;
        point_from_line3d(&delta, vector, -t, &delta);
        result = magnitude_squared3d(&delta);
    }
    else
    {
        result = distance_squared3d(point, origin);
    }
    return result;
}

// @address 0xd8608 ?signed_angle_between_normals3d@@YAMPBTreal_vector3d@@00@Z
float signed_angle_between_normals3d(const real_vector3d *a, const real_vector3d *b, const real_vector3d *axis)
{
    float angle = angle_between_normals3d(a, b);
    real_vector3d cross;
    cross_product3d(a, b, &cross);
    if (dot_product3d(&cross, axis) < 0.0f)
        angle = -angle;
    return angle;
}

// normalize3d_with_default: scales the vector to unit length and returns its old length;
// when too short, replaces it with the fallback vector and returns 0.
float normalize3d_with_default(real_vector3d *v, const real_vector3d *fallback)
{
    float length = sqrtf(v->i * v->i + v->j * v->j + v->k * v->k);
    if (fabsf(length) >= k_real_epsilon) {
        float inverse = k_real_one / length;
        v->i *= inverse;
        v->j *= inverse;
        v->k *= inverse;
    }
    else {
        *v = *fallback;
        length = k_real_zero;
    }
    return length;
}

// @address 0xf26b0 ?quaternion_to_angle_and_vector@@YAXPBUreal_quaternion@@PAMPATreal_vector3d@@@Z
void quaternion_to_angle_and_vector(const real_quaternion *quaternion, float *angle, real_vector3d *vector)
{
    *vector = quaternion->v;
    *angle = 2.0f * arctangent(normalize3d(vector), quaternion->w);
    if (*angle > k_pi_global)
    {
        // match: 0xf26b0: __restrict on v removes a vector reload across the negate
        real_vector3d *__restrict v = vector;
        negate_vector3d(v, v);
        *angle = k_two_pi_global - *angle;
    }
}

// @address 0xf7d68 ?perpendicular3d@@YAPATreal_vector3d@@PBT1@PAT1@@Z
real_vector3d *perpendicular3d(const real_vector3d *vector, real_vector3d *result)
{
    float abs_i = fabsf(vector->i);
    float abs_j = fabsf(vector->j);
    float abs_k = fabsf(vector->k);
    if (abs_i <= abs_j && abs_i <= abs_k)
    {
        result->i = 0.0f;
        result->j = vector->k;
        result->k = -vector->j;
    }
    else if (abs_j <= abs_k)
    {
        result->i = -vector->k;
        result->j = 0.0f;
        result->k = vector->i;
    }
    else
    {
        result->i = vector->j;
        result->j = -vector->i;
        result->k = 0.0f;
    }
    return result;
}

/* quaternions_multiply */
void quaternions_multiply(const real_quaternion *a, const real_quaternion *b, real_quaternion *result)
{
    real_quaternion temp;
    if (a == result) {
        temp = *a;
        a = &temp;
    }
    if (b == result) {
        temp = *b;
        b = &temp;
    }
    result->v.i = a->w * b->v.i + b->w * a->v.i + a->v.j * b->v.k - a->v.k * b->v.j;
    result->v.j = a->w * b->v.j + b->w * a->v.j + a->v.k * b->v.i - a->v.i * b->v.k;
    result->v.k = a->w * b->v.k + b->w * a->v.k + a->v.i * b->v.j - a->v.j * b->v.i;
    result->w = a->w * b->w - a->v.i * b->v.i - a->v.j * b->v.j - a->v.k * b->v.k;
}

/* pin_normal_to_cone3d: a normal outside the cone (its cosine to the axis below cosine) is turned about the axis
   cross normal, so it lies on the cone's surface; returns whether it was pinned */
bool pin_normal_to_cone3d(const real_vector3d *normal, const real_vector3d *cone_axis, float sine, float cosine, real_vector3d *result)
{
    if (dot_product3d(normal, cone_axis) >= cosine)
    {
        *result = *normal;
        return false;
    }
    real_vector3d rotation_axis;
    cross_product3d(cone_axis, normal, &rotation_axis);
    if (normalize3d(&rotation_axis) == 0.0f)
    {
        perpendicular3d(cone_axis, &rotation_axis);
        normalize3d(&rotation_axis);
    }
    *result = *cone_axis;
    rotate_vector_about_axis(result, &rotation_axis, sine, cosine);
    return true;
}

// @address 0x10ae98 ?real_vector3d_build_axes_from_forward_and_left@@YAXPBTreal_vector3d@@0PAT1@11@Z
void real_vector3d_build_axes_from_forward_and_left(const real_vector3d *forward_vector, const real_vector3d *left_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up)
{
    *out_forward = *forward_vector;
    if (normalize3d(out_forward) == 0.0f)
        *out_forward = *global_forward3d;
    cross_product3d(out_forward, left_vector, out_up);
    if (normalize3d(out_up) == 0.0f)
    {
        const real_vector3d *forward = out_forward;
        real_vector3d left;
        if (fabsf(forward->i - global_up3d->i) < k_real_epsilon &&
            fabsf(forward->j - global_up3d->j) < k_real_epsilon &&
            fabsf(forward->k - global_up3d->k) < k_real_epsilon ||
            fabsf(forward->i - global_down3d->i) < k_real_epsilon &&
                fabsf(forward->j - global_down3d->j) < k_real_epsilon &&
                fabsf(forward->k - global_down3d->k) < k_real_epsilon)
            cross_product3d(global_right3d, forward, &left);
        else
            cross_product3d(global_up3d, forward, &left);
        normalize3d(&left);
        real_vector3d up;
        up.k = forward->i * left.j - forward->j * left.i;
        up.j = forward->k * left.i - forward->i * left.k;
        up.i = forward->j * left.k - forward->k * left.j;
        out_up->i = up.i;
        out_up->j = up.j;
        out_up->k = up.k;
        normalize3d(out_up);
    }
    cross_product3d(out_up, out_forward, out_left);
    normalize3d(out_left);
}

/* orientations_interpolate: the rotations interpolate and normalize, the translations and scales interpolate */

static __forceinline float real_math_lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

extern "C" void orientation_interpolate(const real_orientation *a, const real_orientation *b, float t, real_orientation *result)
{
    quaternions_interpolate(&a->rotation, &b->rotation, t, &result->rotation);
    quaternion_normalize(&result->rotation);
    result->translation.x = real_math_lerp(a->translation.x, b->translation.x, t);
    result->translation.y = real_math_lerp(a->translation.y, b->translation.y, t);
    result->translation.z = real_math_lerp(a->translation.z, b->translation.z, t);
    result->scale = real_math_lerp(a->scale, b->scale, t);
}

void quaternions_interpolate(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result)
{
    float s = 1.0f - t;
    float dot = dot_product3d(&a->v, &b->v) + a->w * b->w;
    if (dot < 0.0f)
        t = -t;
    result->v.i = s * a->v.i + t * b->v.i;
    result->v.j = s * a->v.j + t * b->v.j;
    result->v.k = s * a->v.k + t * b->v.k;
    result->w = s * a->w + t * b->w;
}

void quaternion_normalize(real_quaternion *quaternion)
{
    float magnitude_squared = quaternion->v.i * quaternion->v.i + quaternion->v.j * quaternion->v.j
        + quaternion->v.k * quaternion->v.k + quaternion->w * quaternion->w;
#if defined(_XBOX) && !defined(__clang__)
    float zero = k_real_constant_pool_one[200];
#else
    float zero = 0.0f;
#endif
    if (magnitude_squared > zero)
    {
#if defined(_XBOX) && !defined(__clang__)
        float inverse = k_real_constant_pool_one[0] / sqrtf(magnitude_squared);
#else
        float inverse = 1.0f / sqrtf(magnitude_squared);
#endif
        quaternion->v.i *= inverse;
        quaternion->v.j *= inverse;
        quaternion->v.k *= inverse;
        quaternion->w *= inverse;
    }
    else
    {
#if defined(_XBOX) && !defined(__clang__)
        float one = k_real_constant_pool_one[0];
#else
        float one = 1.0f;
#endif
        quaternion->v.i = zero;
        quaternion->v.j = zero;
        quaternion->v.k = zero;
        quaternion->w = one;
    }
}

#define real_math_pin_range(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

extern "C" bool real_update_smooth_value(float *value, float *velocity, float target, float dt, float acceleration, float max_speed,
                             float minimum, float maximum, bool wrap)
{
    float max_speed_squared = max_speed * max_speed;
    float v = *velocity;
    bool snapped = false;
    float p = *value;
    float delta = target - p;
    if (wrap) {
        float half_range = (1.0f - minimum) * 0.5f;
        if (delta > half_range)
            delta -= half_range * 2.0f;
        else if (delta < -half_range)
            delta += half_range * 2.0f;
    }
    // match: 0x11a078: retain the acceleration product evaluation order.
    float max_delta = (acceleration * (dt * dt) > dt * max_speed) ? dt * max_speed : dt * dt * acceleration;
    float new_velocity;
    float new_value;
    if (fabsf(delta - v * dt) <= max_delta) {
        new_velocity = 0.0f;
        new_value = real_math_pin_range(target, minimum, 1.0f);
        snapped = true;
    } else {
        float speed = max_speed;
        float stopping = fabsf(delta) * acceleration * 2.0f;
        if (stopping < max_speed_squared)
            speed = sqrtf(stopping);
        if (delta < 0.0f)
            speed = -speed;
        float desired_acceleration = (speed - v) / dt;
        if (fabsf(desired_acceleration) > acceleration) {
            long sign;
            if (desired_acceleration != 0.0f)
                sign = desired_acceleration < 0.0f ? -1 : 1;
            else
                sign = 0;
            desired_acceleration = (float)sign * acceleration;
        }
        float delta_velocity = desired_acceleration * dt;
        new_velocity = delta_velocity + v;
        new_value = (delta_velocity * 0.5f + new_velocity) * dt + p;
        if (wrap) {
            if (new_value < minimum)
                new_value = (1.0f - minimum) + new_value;
            else if (new_value > 1.0f)
                new_value = new_value - (1.0f - minimum);
        }
        new_value = real_math_pin_range(new_value, minimum, 1.0f);
    }
    *velocity = new_velocity;
    *value = new_value;
    return snapped;
}

// @address 0x127d60 ?component_vectors_from_normal3d@@YAXPBTreal_vector3d@@0PAT1@1@Z
void component_vectors_from_normal3d(const real_vector3d *vector, const real_vector3d *normal,
                                   real_vector3d *parallel, real_vector3d *perpendicular)
{
    real_vector3d temporary;
    float projection = dot_product3d(vector, normal);
    if (!parallel)
        parallel = &temporary;
    scale_vector3d(normal, projection, parallel);
    if (perpendicular)
        subtract_vectors3d(vector, parallel, perpendicular);
}

/* Extends 2D bounds (x0, x1, y0, y1) to cover an array of points. */
real_rectangle2d *real_rectangle2d_enclose_points(real_rectangle2d *bounds, long count, const real_point2d *const points)
{
    int i;
    const real_point2d *point;
    for (i = 0; i < count; i++) {
        point = &points[i];
        if (bounds->x0 > point->x)
            bounds->x0 = point->x;
        if (bounds->x1 < point->x)
            bounds->x1 = point->x;
        if (bounds->y0 > point->y)
            bounds->y0 = point->y;
        if (bounds->y1 < point->y)
            bounds->y1 = point->y;
    }
    return bounds;
}

// Recover the angle from cos(2a) = 2 cos^2(a) - 1.
float angle_between_vectors2d(const real_vector2d *a, const real_vector2d *b)
{
    float magnitude_product = magnitude_squared2d(a) * magnitude_squared2d(b);
    if (magnitude_product != 0.0f)
    {
        float dot = dot_product2d(a, b);
        float cosine2 = 2.0f * (dot * dot / magnitude_product) - 1.0f;
        float result = 0.5f * real_math_arccosine_clamped(real_pin(cosine2, -1.0f, 1.0f));
        if (dot < 0.0f)
            result = k_pi_global - result;
        return result;
    }
    return 0.0f;
}

/* the axes from a forward and an up vector; the up axis falls back to global_up3d, and a forward that runs along the up axis
   (or its negation) takes global_up3d as its stand-in for the cross product */
void real_vector3d_build_axes_from_vectors_using_up(const real_vector3d *forward_vector, const real_vector3d *up_vector,
    real_vector3d *out_forward, real_vector3d *out_left, real_vector3d *out_up)
{
    *out_up = *up_vector;
    if (normalize3d(out_up) == 0.0f)
        *out_up = *global_up3d;
    cross_product3d(out_up, forward_vector, out_left);
    if (normalize3d(out_left) == 0.0f)
    {
        real_vector3d forward;
        if (fabsf(out_up->i - global_forward3d->i) < k_real_epsilon &&
            fabsf(out_up->j - global_forward3d->j) < k_real_epsilon &&
            fabsf(out_up->k - global_forward3d->k) < k_real_epsilon ||
            fabsf(out_up->i - global_backward3d->i) < k_real_epsilon &&
                fabsf(out_up->j - global_backward3d->j) < k_real_epsilon &&
                fabsf(out_up->k - global_backward3d->k) < k_real_epsilon)
            forward = *global_up3d;
        else
            forward = *global_forward3d;
        cross_product3d(out_up, &forward, out_left);
        normalize3d(out_left);
    }
    cross_product3d(out_left, out_up, out_forward);
    normalize3d(out_forward);
}


void fast_normals_interpolate(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *result)
{
    float s = 1.0f - t;
    result->i = a->i * s + b->i * t;
    result->j = a->j * s + b->j * t;
    result->k = a->k * s + b->k * t;
    fast_normalize3d_inline(result);
}

// the point a fraction t of the way along the arc from a to b (the nearer end is copied, the rotation is about the arc's axis)
void normals_interpolate(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *out)
{
    float angle = angle_between_vectors3d(a, b);
    real_vector3d axis;
    if (t <= 0.5f)
    {
        angle = angle * t;
        cross_product3d(a, b, &axis);
        *out = *a;
    }
    else
    {
        angle = (1.0f - t) * angle;
        cross_product3d(b, a, &axis);
        *out = *b;
    }
    if (normalize3d(&axis) != 0.0f)
    {
        float cosine_angle = cosine(angle);
        float sine_angle = sine(angle);
        rotate_vector_about_axis(out, &axis, sine_angle, cosine_angle);
    }
}


// match: 0x19cde0: __restrict on result pairs the quaternion loads ahead of the blend stores
__forceinline void quaternions_interpolate_long(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *__restrict result)
{
    float s = 1.0f - t;
    if (a->v.i * b->v.i + a->v.j * b->v.j + a->v.k * b->v.k + a->w * b->w > 0.0f)
        t = -t;
    result->v.i = s * a->v.i + t * b->v.i;
    result->v.j = s * a->v.j + t * b->v.j;
    result->v.k = s * a->v.k + t * b->v.k;
    result->w = s * a->w + t * b->w;
}

// match: 0x19cde0: restricted output preserves input loads before stores.
void quaternions_interpolate_long_and_normalize(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result)
{
    // match: 0x19cde0: __restrict on out keeps the input loads ahead of the normalize stores
    real_quaternion *__restrict out = result;
    quaternions_interpolate_long(a, b, t, out);
    quaternion_normalize(out);
}

/* quaternion_transform_point */
void quaternion_transform_point(const real_quaternion *q, const real_point3d *p, real_point3d *result)
{
    float w_scale = 2.0f * q->w * q->w - 1.0f;
    float cross_scale = 2.0f * q->w;
    float dot = q->v.i * p->x + q->v.j * p->y + q->v.k * p->z;
    float dot_scale = 2.0f * dot;
    float cross_i = q->v.j * p->z - q->v.k * p->y;
    float cross_j = q->v.k * p->x - q->v.i * p->z;
    float cross_k = q->v.i * p->y - q->v.j * p->x;
    result->x = w_scale * p->x + (dot_scale * q->v.i + cross_scale * cross_i);
    result->y = cross_scale * cross_j + (dot_scale * q->v.j + w_scale * p->y);
    result->z = cross_scale * cross_k + (dot_scale * q->v.k + w_scale * p->z);
}

/* orientations_multiply */
void orientations_multiply(const real_orientation *a, const real_orientation *b, real_orientation *result)
{
    real_orientation temp;
    if (a == result)
    {
        temp = *a;
        a = &temp;
    }
    if (b == result)
    {
        temp = *b;
        b = &temp;
    }
    quaternions_multiply(&a->rotation, &b->rotation, &result->rotation);
    quaternion_transform_point(&a->rotation, &b->translation, &result->translation);
    if (a->scale != 1.0f)
    {
        scale_point3d(&result->translation, a->scale, &result->translation);
        result->scale = b->scale * a->scale;
    }
    else
        result->scale = b->scale;
    add_points3d(&a->translation, &result->translation, &result->translation);
}

/* orientation_inverse */
void orientation_inverse(const real_orientation *orientation, real_orientation *result)
{
    result->rotation = orientation->rotation;
    result->rotation.w *= -1.0f;
    quaternion_transform_point(&result->rotation, &orientation->translation, &result->translation);
    float inverse_scale = 1.0f;
    if (orientation->scale == 0.0f)
        inverse_scale = 10000.0f;
    else if (orientation->scale != 1.0f)
        inverse_scale = 1.0f / orientation->scale;
    scale_point3d(&result->translation, inverse_scale, &result->translation);
    result->scale = inverse_scale;
    result->translation.x *= -1.0f;
    result->translation.y *= -1.0f;
    result->translation.z *= -1.0f;
}

// Copy the forward and up axes without the position.
static __forceinline void real_math_matrix4x3_forward_and_up(const real_matrix4x3 *matrix, real_vector3d *forward, real_vector3d *up)
{
    *forward = matrix->forward;
    *up = matrix->up;
}

// @address 0x19d2e8 ?vectors3d_from_euler_angles3d@@YAXPATreal_vector3d@@0PBTreal_euler_angles3d@@@Z
void vectors3d_from_euler_angles3d(real_vector3d *forward, real_vector3d *up, const real_euler_angles3d *angles)
{
    real_matrix4x3 matrix;
    matrix4x3_rotation_from_angles(&matrix, angles->yaw, angles->pitch, angles->roll);
    real_math_matrix4x3_forward_and_up(&matrix, forward, up);
}

// @address 0x19d360 ?vectors3d_from_euler_angles2d@@YAXPATreal_vector3d@@0PBTreal_euler_angles2d@@@Z
void vectors3d_from_euler_angles2d(real_vector3d *forward, real_vector3d *up, const real_euler_angles2d *angles)
{
    real_matrix4x3 matrix;
    matrix4x3_rotation_from_angles(&matrix, angles->yaw, angles->pitch, 0.0f);
    real_math_matrix4x3_forward_and_up(&matrix, forward, up);
}

real_vector3d *vector3d_from_angle(real_vector3d *vector, float angle)
{
    float j = (float)sin((double)angle);
    float i = (float)cos((double)angle);
    vector->j = j;
    vector->k = k_real_zero_storage;
    vector->i = i;
    return vector;
}

// 0x19d448 (2D point within radius of a clamped segment)
bool point_in_pill2d(const real_point2d *point, const real_point2d *origin, const real_vector2d *vector, float radius)
{
    real_vector2d delta;
    vector_from_points2d(origin, point, &delta);
    float length_squared = magnitude_squared2d(vector);
    if (length_squared > k_real_epsilon)
    {
        float t = dot_product2d(vector, &delta) / length_squared;
        if (t <= k_real_zero)
            t = k_real_zero;
        else if (t >= k_real_one)
            t = k_real_one;
        point_from_line2d(&delta, vector, -t, &delta);
    }
    return magnitude_squared2d(&delta) <= radius * radius;
}

// the byte in [0, 255] that stands for the largest dequantized value not above test (min_value 0, max_value 1 at every call)
static __forceinline float real_math_dequantize_byte_to_real(float min_value, float max_value, unsigned char value)
{
    if (value == 255)
        return max_value;
    return min_value + (max_value - min_value) * ((float)value / 255.0f);
}

__forceinline real_vector3d *real_math_add_scaled_vector3d(real_vector3d *out, const real_vector3d *v, float scale)
{
    out->i = v->i * scale + out->i;
    out->j = v->j * scale + out->j;
    out->k = v->k * scale + out->k;
    return out;
}
static __forceinline void real_math_distance_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float j = a->k * b->i - a->i * b->k;
    float k = a->i * b->j - a->j * b->i;
    float i = a->j * b->k - a->k * b->j;
    result->i = i; result->j = j; result->k = k;
}
// @address 0x19d4e0 ?vector_to_line_distance_squared3d@@YAMPBTreal_point3d@@PBTreal_vector3d@@01@Z
float vector_to_line_distance_squared3d(const real_point3d *point0, const real_vector3d *vector0, const real_point3d *point1, const real_vector3d *vector1)
{
    real_vector3d difference;
    real_vector3d cross;
    real_point3d closest0, closest1;
    vector_from_points3d(point0, point1, &difference);
    real_math_distance_cross(vector0, vector1, &cross);
    float cross_squared = magnitude_squared3d(&cross);
    float t0, t1;
    if (!(fabsf(cross_squared) < k_real_epsilon))
    {
        scale_vector3d(&cross, 1.0f / cross_squared, &cross);
        t0 = triple_product3d(&difference, vector1, &cross);
        t1 = triple_product3d(&difference, vector0, &cross);
        bool outside0 = t0 < 0.0f || t0 > 1.0f;
        bool outside1 = t1 < 0.0f || t1 > 1.0f;
        if (outside0 || outside1)
        {
            float d0 = 3.402823466e+38f;
            float d1 = 3.402823466e+38f;
            if (outside0)
            {
                point_from_line3d(point0, vector0, t0 < 0.0f ? 0.0f : 1.0f, &closest0);
                d0 = point_to_line_distance_squared3d(&closest0, point1, vector1);
            }
            if (outside1)
            {
                point_from_line3d(point1, vector1, t1 < 0.0f ? 0.0f : 1.0f, &closest1);
                d1 = point_to_line_distance_squared3d(&closest1, point0, vector0);
            }
            return d0 > d1 ? d1 : d0;
        }
    }
    else
    {
        float dot = dot_product3d(vector0, vector1);
        float length_squared = magnitude_squared3d(vector0);
        if (length_squared > k_real_epsilon)
        {
            float inverse = 1.0f / length_squared;
            float start = dot_product3d(vector0, &difference) * inverse;
            float end = inverse * dot + start;
            t0 = (PIN(start, 0.0f, 1.0f) + PIN(end, 0.0f, 1.0f)) * 0.5f;
        }
        else
            t0 = 0.0f;
        length_squared = magnitude_squared3d(vector1);
        t1 = 0.0f;
        if (length_squared > k_real_epsilon)
        {
            float inverse = 1.0f / length_squared;
            float start = -(dot_product3d(vector1, &difference) * inverse);
            float end = inverse * dot + start;
            t1 = (PIN(start, 0.0f, 1.0f) + PIN(end, 0.0f, 1.0f)) * 0.5f;
        }
    }
    point_from_line3d(point0, vector0, t0, &closest0);
    point_from_line3d(point1, vector1, t1, &closest1);
    return distance_squared3d(&closest0, &closest1);
}

static __forceinline float real_math_triangle_parameters(float edge0_dot_edge0, float edge0_dot_edge1, float edge1_dot_edge1,
    float diff_dot_edge0, float diff_dot_edge1, float *s, float *t)
{
    *s = edge0_dot_edge1 * diff_dot_edge1 - edge1_dot_edge1 * diff_dot_edge0;
    *t = edge0_dot_edge1 * diff_dot_edge0 - edge0_dot_edge0 * diff_dot_edge1;
    return (edge0_dot_edge1 * diff_dot_edge0 - edge0_dot_edge0 * diff_dot_edge1)
        + (edge0_dot_edge1 * diff_dot_edge1 - edge1_dot_edge1 * diff_dot_edge0);
}

// @address 0x19d880 real_compute_point_triangle_distance
extern "C" float real_compute_point_triangle_distance(const real_point3d *point, const real_point3d *triangle, real_point3d *closest_point)
{
    real_vector3d edge0;
    real_vector3d edge1;
    real_vector3d difference;
    vector_from_points3d(&triangle[0], &triangle[1], &edge0);
    vector_from_points3d(&triangle[0], &triangle[2], &edge1);
    vector_from_points3d(point, &triangle[0], &difference);

    float edge0_dot_edge0 = dot_product3d(&edge0, &edge0);
    float edge0_dot_edge1 = dot_product3d(&edge0, &edge1);
    float edge1_dot_edge1 = dot_product3d(&edge1, &edge1);
    float diff_dot_edge0 = dot_product3d(&difference, &edge0);
    float diff_dot_edge1 = dot_product3d(&difference, &edge1);
    float determinant = edge0_dot_edge0 * edge1_dot_edge1 - edge0_dot_edge1 * edge0_dot_edge1;
    float s;
    float t;
    float sum = real_math_triangle_parameters(edge0_dot_edge0, edge0_dot_edge1, edge1_dot_edge1, diff_dot_edge0, diff_dot_edge1, &s, &t);

    real_point3d closest;
    if (sum <= determinant)
    {
        if (s < 0.0f)
        {
            if (t < 0.0f)
            {
                // region 4
                if (diff_dot_edge0 < 0.0f)
                {
                    if (-diff_dot_edge0 >= edge0_dot_edge0)
                    {
                        closest = triangle[1];
                    }
                    else
                    {
                        if (edge0_dot_edge0 > 1e-6f)
                            s = -(diff_dot_edge0 / edge0_dot_edge0);
                        else
                            s = 0.0f;
                        point_from_line3d(&triangle[0], &edge0, s, &closest);
                    }
                }
                else
                {
                    if (diff_dot_edge1 >= 0.0f)
                    {
                        closest = triangle[0];
                    }
                    else if (-diff_dot_edge1 >= edge1_dot_edge1)
                    {
                        closest = triangle[2];
                    }
                    else
                    {
                        if (edge1_dot_edge1 > 1e-6f)
                            t = -(diff_dot_edge1 / edge1_dot_edge1);
                        else
                            t = 0.0f;
                        point_from_line3d(&triangle[0], &edge1, t, &closest);
                    }
                }
            }
            else
            {
                // region 3
                if (diff_dot_edge1 >= 0.0f)
                {
                    closest = triangle[0];
                }
                else if (-diff_dot_edge1 >= edge1_dot_edge1)
                {
                    closest = triangle[2];
                }
                else
                {
                    if (edge1_dot_edge1 > 1e-6f)
                        t = -(diff_dot_edge1 / edge1_dot_edge1);
                    else
                        t = 0.0f;
                    point_from_line3d(&triangle[0], &edge1, t, &closest);
                }
            }
        }
        else if (t < 0.0f)
        {
            // region 5
            if (diff_dot_edge0 >= 0.0f)
            {
                closest = triangle[0];
            }
            else if (-diff_dot_edge0 >= edge0_dot_edge0)
            {
                closest = triangle[1];
            }
            else
            {
                if (edge0_dot_edge0 > 1e-6f)
                    s = -(diff_dot_edge0 / edge0_dot_edge0);
                else
                    s = 0.0f;
                point_from_line3d(&triangle[0], &edge0, s, &closest);
            }
        }
        else
        {
            // region 0
            float inverse;
            if (fabsf(determinant) > 1e-6f)
                inverse = 1.0f / determinant;
            else
                inverse = 0.0f;
            s *= inverse;
            t *= inverse;
            closest.x = edge0.i * s + edge1.i * t + triangle[0].x;
            closest.y = edge0.j * s + edge1.j * t + triangle[0].y;
            closest.z = edge0.k * s + edge1.k * t + triangle[0].z;
        }
    }
    else if (s < 0.0f)
    {
        // region 2 (edge0_vertex2_dot is edge0 . (triangle[2] - point), and likewise for edge1)
        float edge0_vertex2_dot = edge0_dot_edge1 + diff_dot_edge0;
        float edge1_vertex2_dot = edge1_dot_edge1 + diff_dot_edge1;
        if (edge1_vertex2_dot > edge0_vertex2_dot)
        {
            float numerator = edge1_vertex2_dot - edge0_vertex2_dot;
            float denominator = edge0_dot_edge0 - 2.0f * edge0_dot_edge1 + edge1_dot_edge1;
            if (numerator >= denominator || denominator < 1e-6f)
            {
                closest = triangle[1];
            }
            else
            {
                s = numerator / denominator;
                t = 1.0f - s;
                closest.x = edge0.i * s + edge1.i * t + triangle[0].x;
                closest.y = edge0.j * s + edge1.j * t + triangle[0].y;
                closest.z = edge0.k * s + edge1.k * t + triangle[0].z;
            }
        }
        else if (edge1_vertex2_dot <= 0.0f)
        {
            closest = triangle[2];
        }
        else if (diff_dot_edge1 >= 0.0f)
        {
            closest = triangle[0];
        }
        else
        {
            if (edge1_dot_edge1 > 1e-6f)
                t = -(diff_dot_edge1 / edge1_dot_edge1);
            else
                t = 0.0f;
            point_from_line3d(&triangle[0], &edge1, t, &closest);
        }
    }
    else if (t < 0.0f)
    {
        // region 6 (edge1_vertex1_dot is edge1 . (triangle[1] - point), and likewise for edge0)
        float edge1_vertex1_dot = edge0_dot_edge1 + diff_dot_edge1;
        float edge0_vertex1_dot = edge0_dot_edge0 + diff_dot_edge0;
        if (edge0_vertex1_dot > edge1_vertex1_dot)
        {
            float numerator = edge0_vertex1_dot - edge1_vertex1_dot;
            float denominator = edge0_dot_edge0 - 2.0f * edge0_dot_edge1 + edge1_dot_edge1;
            if (numerator >= denominator || denominator < 1e-6f)
            {
                closest = triangle[2];
            }
            else
            {
                t = numerator / denominator;
                s = 1.0f - t;
                closest.x = edge0.i * s + edge1.i * t + triangle[0].x;
                closest.y = edge0.j * s + edge1.j * t + triangle[0].y;
                closest.z = edge0.k * s + edge1.k * t + triangle[0].z;
            }
        }
        else if (edge0_vertex1_dot <= 0.0f)
        {
            closest = triangle[1];
        }
        else if (diff_dot_edge0 >= 0.0f)
        {
            closest = triangle[0];
        }
        else
        {
            if (edge0_dot_edge0 > 1e-6f)
                s = -(diff_dot_edge0 / edge0_dot_edge0);
            else
                s = 0.0f;
            point_from_line3d(&triangle[0], &edge0, s, &closest);
        }
    }
    else
    {
        // region 1
        float numerator = edge1_dot_edge1 + diff_dot_edge1 - edge0_dot_edge1 - diff_dot_edge0;
        if (numerator <= 0.0f)
        {
            closest = triangle[2];
        }
        else
        {
            float denominator = edge0_dot_edge0 - 2.0f * edge0_dot_edge1 + edge1_dot_edge1;
            if (numerator >= denominator || denominator < 1e-6f)
            {
                closest = triangle[1];
            }
            else
            {
                s = numerator / denominator;
                t = 1.0f - s;
                closest.x = edge0.i * s + edge1.i * t + triangle[0].x;
                closest.y = edge0.j * s + edge1.j * t + triangle[0].y;
                closest.z = edge0.k * s + edge1.k * t + triangle[0].z;
            }
        }
    }

    float distance = sqrtf(distance_squared3d(point, &closest));
    if (closest_point)
        *closest_point = closest;
    return distance;
}

// the point where three planes meet, from the determinant of their normals (false when two are nearly parallel)
extern "C" bool planes_try_and_get_intersection_point(const real_plane3d *p0, const real_plane3d *p1, const real_plane3d *p2, real_vector3d *out)
{
    real_vector3d c;
    float determinant = dot_product3d(cross_product3d(&p0->n, &p1->n, &c), &p2->n);
    if (fabsf(determinant) >= k_real_epsilon)
    {
        float d0 = p0->d;
        scale_vector3d(cross_product3d(&p1->n, &p2->n, &c), d0, out);
        float d1 = p1->d;
        real_math_add_scaled_vector3d(out, cross_product3d(&p2->n, &p0->n, &c), d1);
        c.i = p0->n.j * p1->n.k - p0->n.k * p1->n.j;
        float d2 = p2->d;
        c.j = p0->n.k * p1->n.i - p0->n.i * p1->n.k;
        c.k = p0->n.i * p1->n.j - p0->n.j * p1->n.i;
        real_math_add_scaled_vector3d(out, &c, d2);
        float inverse = 1.0f / determinant;
        scale_vector3d(out, inverse, out);
        return true;
    }
    return false;
}

/* the line where two planes meet: direction is the cross product of their normals, point is found from their offsets */
static __forceinline void real_math_scaled_vector3d(const real_vector3d *v, float s, real_vector3d *result)
{
    result->i = v->i * s;
    result->j = v->j * s;
    result->k = v->k * s;
}

static __forceinline void real_math_add_scaled_vector3d(const real_vector3d *v, const real_vector3d *offset, float s, real_vector3d *result)
{
    result->i = v->i + offset->i * s;
    result->j = v->j + offset->j * s;
    result->k = v->k + offset->k * s;
}

extern "C" bool planes_try_and_get_intersection_line(const real_plane3d *p0, const real_plane3d *p1, real_vector3d *point, real_vector3d *direction)
{
    real_vector3d t;
    cross_product3d(&p0->n, &p1->n, direction);
    float length_squared = magnitude_squared3d(direction);
    if (fabsf(length_squared) >= k_real_epsilon)
    {
        float inverse = k_real_one / length_squared;
        cross_product3d(&p1->n, direction, &t);
        real_math_scaled_vector3d(&t, p0->d, point);
        cross_product3d(direction, &p0->n, &t);
        real_math_add_scaled_vector3d(point, &t, p1->d, point);
        real_math_scaled_vector3d(point, inverse, point);
        return true;
    }
    return false;
}

// @address 0x19dfe8 ?quantize_real_to_byte_lower_bound@@YAEMMM@Z
byte quantize_real_to_byte_lower_bound(float min_value, float max_value, float test)
{
    // match: 0x19dfe8: value range assumption guides branch lowering
    __assume(min_value == 0.0f);
    // match: 0x19dfe8: value range assumption guides branch lowering
    __assume(max_value == 1.0f);
    byte value = 0;
    if (max_value - min_value > k_real_epsilon)
    {
        value = (byte)((test - min_value) / (max_value - min_value) * 255.0f);
        while (value != 0 && test < real_math_dequantize_byte_to_real(min_value, max_value, value))
            value--;
    }
    return value;
}

// quantize_real_point3d: each axis is clamped to its bounds, then quantized to bits bits
void quantize_real_point3d(const real_point3d *point, const real_rectangle3d *bounds, long bits, long_point3d *quantized)
{
    for (int i = 0; i < 3; i++)
    {
        float value = PIN(point->n[i], bounds->n[i].lower, bounds->n[i].upper);
        quantized->n[i] = quantize_real(value, bounds->n[i].lower, bounds->n[i].upper, bits, false, false);
    }
}

void dequantize_real_point3d(const long_point3d *quantized, const real_rectangle3d *bounds, long bits, real_point3d *point)
{
    for (int i = 0; i < 3; i++)
        point->n[i] = dequantize_real(quantized->n[i], bounds->n[i].lower, bounds->n[i].upper, bits, false);
}

/* real_rectangle3d_enclose_rectangle: MIN(a, b) is ((a) > (b) ? (b) : (a)) and MAX(a, b) is ((a) > (b) ? (a) : (b)) here */
real_rectangle3d *real_rectangle3d_enclose_rectangle(real_rectangle3d *bounds, const real_rectangle3d *rectangle)
{
    bounds->x0 = bounds->x0 > rectangle->x0 ? rectangle->x0 : bounds->x0;
    bounds->x1 = bounds->x1 > rectangle->x1 ? bounds->x1 : rectangle->x1;
    bounds->y0 = bounds->y0 > rectangle->y0 ? rectangle->y0 : bounds->y0;
    bounds->y1 = bounds->y1 > rectangle->y1 ? bounds->y1 : rectangle->y1;
    bounds->z0 = bounds->z0 > rectangle->z0 ? rectangle->z0 : bounds->z0;
    bounds->z1 = bounds->z1 > rectangle->z1 ? bounds->z1 : rectangle->z1;
    return bounds;
}

// Convert the yaw, roll, and pitch half angles.
void quaternion_from_euler_angles(real_quaternion *quaternion, const real_euler_angles3d *euler)
{
    float cos_yaw = cosine(euler->yaw * 0.5f);
    float sin_yaw = sine(euler->yaw * 0.5f);
    float cos_roll = cosine(euler->roll * 0.5f);
    float sin_roll = sine(euler->roll * 0.5f);
    float cos_pitch = cosine(euler->pitch * 0.5f);
    float sin_pitch = sine(euler->pitch * 0.5f);
    float cos_roll_cos_yaw = cos_roll * cos_yaw;
    float sin_roll_sin_yaw = sin_roll * sin_yaw;
    quaternion->w = sin_roll_sin_yaw * sin_pitch + cos_roll_cos_yaw * cos_pitch;
    quaternion->v.i = cos_roll_cos_yaw * sin_pitch - sin_roll_sin_yaw * cos_pitch;
    quaternion->v.j = sin_pitch * cos_roll * sin_yaw + cos_pitch * sin_roll * cos_yaw;
    quaternion->v.k = cos_pitch * cos_roll * sin_yaw - sin_pitch * sin_roll * cos_yaw;
}

static __forceinline float real_math_ik_minimum_distance(const float &maximum, const float &minimum)
{
    return maximum > minimum ? minimum : maximum;
}

// @address 0x19e430 ?matrix4x3_inverse_kinematics_adjust_matrices_new@@YAXPAUreal_matrix4x3@@000@Z
void matrix4x3_inverse_kinematics_adjust_matrices_new(real_matrix4x3 *target, real_matrix4x3 *root, real_matrix4x3 *joint, real_matrix4x3 *end)
{
    real_vector3d upper_direction;
    real_vector3d lower_direction;
    vector_from_points3d(&root->position, &joint->position, &upper_direction);
    vector_from_points3d(&joint->position, &end->position, &lower_direction);
    float upper_length = normalize3d(&upper_direction);
    float lower_length = normalize3d(&lower_direction);
    if (upper_length > k_real_epsilon && lower_length > k_real_epsilon)
    {
        real_vector3d end_direction;
        vector_from_points3d(&root->position, &end->position, &end_direction);
        normalize3d(&end_direction);

        real_vector3d target_direction;
        vector_from_points3d(&root->position, &target->position, &target_direction);
        float target_distance = normalize3d(&target_direction);

        float difference = (float)fabs(upper_length - lower_length);
        float maximum_distance = (lower_length + upper_length) * 0.98f;
        float minimum_distance = maximum_distance > difference * 1.02f ? difference * 1.02f : maximum_distance;
        target_distance = PIN(target_distance, minimum_distance, maximum_distance);

        real_point3d end_position;
        point_from_line3d(&root->position, &target_direction, target_distance, &end_position);

        real_matrix4x3 joint_matrix;
        matrix4x3_from_point_and_nonaxial_forward_and_left(&joint_matrix, &root->position, &end_direction, &upper_direction);

        real_matrix4x3 root_matrix;
        matrix4x3_identity(&root_matrix);
        root_matrix.position = joint->position;
        matrix4x3_from_point_and_nonaxial_forward_and_left(&root_matrix, &joint->position, &lower_direction, &upper_direction);
        matrix4x3_from_point_and_nonaxial_vectors(&root_matrix, &root->position, &target_direction, &joint_matrix.up);

        float cosine_angle = (lower_length * lower_length - target_distance * target_distance - upper_length * upper_length) /
            (target_distance * upper_length * -2.0f);
        float sine_squared = MAX(1.0f - cosine_angle * cosine_angle, k_real_epsilon * k_real_epsilon);

        real_point3d local_joint_position;
        local_joint_position.x = cosine_angle * upper_length;
        local_joint_position.y = sqrtf(sine_squared) * upper_length;
        local_joint_position.z = 0.0f;

        real_point3d joint_position;
        matrix4x3_transform_point(&root_matrix, &local_joint_position, &joint_position);

        real_vector3d joint_forward;
        vector_from_points3d(&joint_position, &end_position, &joint_forward);
        matrix4x3_from_point_and_nonaxial_vectors(&joint_matrix, &joint_position, &joint_forward, &root->up);
        real_vector3d root_forward;
        vector_from_points3d(&root_matrix.position, &joint_position, &root_forward);
        matrix4x3_from_point_and_nonaxial_vectors(&root_matrix, &root->position, &root_forward, &root->up);

        root->rotation = root_matrix.rotation;
        root->position = root_matrix.position;
        joint->rotation = joint_matrix.rotation;
        joint->position = joint_matrix.position;
        end->rotation = target->rotation;
        end->position = end_position;
    }
}


// match: 0x28c640: inline keeps collision_get_unobstructed_point at 1812 bytes; out of line it links 8 bytes short
#if defined(_XBOX) && !defined(__clang__)
inline real_point3d *project_point2d(const real_point2d *point, const real_plane3d *plane, short projection, bool sign, real_point3d *result)
#else
real_point3d *project_point2d(const real_point2d *point, const real_plane3d *plane, short projection, bool sign, real_point3d *result)
#endif
{
    short x_axis = global_projection3d_mappings[projection][sign][0];
    short y_axis = global_projection3d_mappings[projection][sign][1];
    result->n[x_axis] = point->x;
    result->n[y_axis] = point->y;
    float z;
    if (fabsf(plane->n.n[projection] - 0.0f) < 0.0001f)
        z = 0.0f;
    else
        z = (plane->d - plane->n.n[x_axis] * point->x - plane->n.n[y_axis] * point->y) / plane->n.n[projection];
    result->n[projection] = z;
    return result;
}

// @address 0x19e8c8 ?fast_normalize3d@@YAPATreal_vector3d@@PAT1@@Z
// match: 0x2b1ef8: inline keeps collision_point_in_cylinder's pdata length; out of line .text links 0x40 bytes short from here on
#if defined(_XBOX) && !defined(__clang__)
inline real_vector3d *fast_normalize3d(real_vector3d *vector)
#else
real_vector3d *fast_normalize3d(real_vector3d *vector)
#endif
{
    float magnitude_squared = magnitude_squared3d(vector);
    if (magnitude_squared != 0.0f)
        scale_vector3d(vector, reciprocal_square_root(magnitude_squared), vector);
    return vector;
}

static __forceinline long rectangle3d_build_vertices(const real_rectangle3d *bounds, long maximum_vertex_count, real_point3d *vertices)
{
    vertices[0].x = bounds->x0; vertices[0].y = bounds->y0; vertices[0].z = bounds->z0;
    vertices[1].x = bounds->x1; vertices[1].y = bounds->y0; vertices[1].z = bounds->z0;
    vertices[2].x = bounds->x0; vertices[2].y = bounds->y1; vertices[2].z = bounds->z0;
    vertices[3].x = bounds->x1; vertices[3].y = bounds->y1; vertices[3].z = bounds->z0;
    vertices[4].x = bounds->x0; vertices[4].y = bounds->y0; vertices[4].z = bounds->z1;
    vertices[5].x = bounds->x1; vertices[5].y = bounds->y0; vertices[5].z = bounds->z1;
    vertices[6].x = bounds->x0; vertices[6].y = bounds->y1; vertices[6].z = bounds->z1;
    vertices[7].x = bounds->x1; vertices[7].y = bounds->y1; vertices[7].z = bounds->z1;
    return 8;
}

long rectangle3d_build_edges(const real_rectangle3d *bounds, long maximum_edge_count, real_point3d edges[][2])
{
    long line_vertex_indices[12][2] = {
        {0, 2}, {2, 3}, {3, 1}, {1, 0}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {4, 5}, {5, 7}, {7, 6}, {6, 4} };
    real_point3d vertices[8];
    rectangle3d_build_vertices(bounds, 8, vertices);
    for (long edge_index = 0; edge_index < 12; edge_index++)
    {
        long first_vertex = line_vertex_indices[edge_index][0];
        long second_vertex = line_vertex_indices[edge_index][1];
        edges[edge_index][0] = vertices[first_vertex];
        edges[edge_index][1] = vertices[second_vertex];
    }
    return 12;
}

/* quaternions_slerp_interpolate */
void quaternions_slerp_interpolate(const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result)
{
    float s = 1.0f - t;
    float cosine = a->v.i * b->v.i + a->v.j * b->v.j + a->v.k * b->v.k + a->w * b->w;
    if (cosine > 1.0f)
        cosine = 1.0f;
    else if (cosine < -1.0f)
        cosine = -1.0f;
    float angle = (float)acos(cosine);
    float sine = (float)sin(angle);
    float scale_a, scale_b;
    if (fabsf(sine) >= 0.0001f)
    {
        float inverse = 1.0f / sine;
        scale_b = (float)sin(angle * t) * inverse;
        scale_a = (float)sin(angle * s) * inverse;
        if (cosine < 0.0f)
            scale_b = -scale_b;
    }
    else
    {
        scale_b = PIN((float)(int)(t + 0.5f), 0.0f, 1.0f);
        scale_a = 1.0f - scale_b;
    }
    result->v.i = a->v.i * scale_a + b->v.i * scale_b;
    result->v.j = a->v.j * scale_a + b->v.j * scale_b;
    result->v.k = a->v.k * scale_a + b->v.k * scale_b;
    result->w = a->w * scale_a + b->w * scale_b;
}


float signed_angle_between_vectors2d(const real_vector2d *a, const real_vector2d *b)
{
    float angle = real_math_arccosine_clamped(real_pin(dot_product2d(a, b), -1.0f, 1.0f));
    if (cross_product2d(a, b) < 0.0f)
        angle = -angle;
    return angle;
}

void quaternion_from_unit_vectors(real_quaternion *result, const real_vector3d *u, const real_vector3d *v)
{
    float dot = dot_product3d(u, v);
    float s = sqrtf(MAX((dot + k_real_one) * k_real_two, k_real_zero));
    if (s >= 0.001f) {
        float inverse = k_real_one / s;
        cross_product3d(u, v, &result->v);
        result->w = s * k_real_half;
        scale_vector3d(&result->v, inverse, &result->v);
    } else {
        perpendicular3d(u, &result->v);
        result->w = k_real_zero;
    }
}

// rotate a by (angle(a,b) * fraction) about the a x b axis.
static __forceinline void real_math_rotate_about_unit_axis(const real_vector3d *axis, float cosine_angle, float sine_angle, real_vector3d *vector)
{
    float cross_y = vector->k * axis->i - vector->i * axis->k;
    float cross_z = vector->i * axis->j - vector->j * axis->i;
    float scale = (vector->j * axis->j + (vector->i * axis->i + vector->k * axis->k)) * (1.0f - cosine_angle);
    float y = vector->j * cosine_angle + scale * axis->j;
    vector->i = vector->i * cosine_angle + scale * axis->i - (vector->j * axis->k - vector->k * axis->j) * sine_angle;
    vector->j = y - cross_y * sine_angle;
    vector->k = vector->k * cosine_angle + scale * axis->k - cross_z * sine_angle;
}
extern "C" void vectors_interpolate_toward(const real_vector3d *a, const real_vector3d *b, float fraction, real_vector3d *out)
{
    float angle = angle_between_vectors3d(a, b) * fraction;
    real_vector3d axis;
    cross_product3d(a, b, &axis);
    *out = *a;
    normalize3d(&axis);
    float cosine_angle = (float)cos(angle);
    float sine_angle = (float)sin(angle);
    real_math_rotate_about_unit_axis(&axis, cosine_angle, sine_angle, out);
}

static __forceinline float real_math_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return point->x * plane->n.i + point->y * plane->n.j + point->z * plane->n.k - plane->d;
}

static __forceinline void real_math_clip_point_on_line(const real_point3d *a, const real_vector3d *difference, float fraction, real_point3d *result)
{
    result->x = a->x + difference->i * fraction;
    result->y = a->y + difference->j * fraction;
    result->z = a->z + difference->k * fraction;
}

static __forceinline void real_math_intersection_point(const real_point3d *a, const real_point3d *b, float distance_a, float distance_b, real_point3d *result)
{
    real_vector3d difference;
    vector_from_points3d(a, b, &difference);
    float fraction = distance_a / (distance_a - distance_b);
    real_math_clip_point_on_line(a, &difference, fraction, result);
}

// @address 0x155718 ?plane3d_clip_polygon@@YAJPBUreal_plane3d@@_NMJPBTreal_point3d@@JPAT2@@Z
long plane3d_clip_polygon(const real_plane3d *plane, bool keep_positive, float tolerance, long point_count, const real_point3d *points,
    long maximum_count, real_point3d *clipped_points)
{
    long clipped_count = 0;
    if (point_count > 0)
    {
        bool keep = true;
        float side_tolerance = 0.0078125f;
        float zero = 0.0f;
        if (!keep)
            side_tolerance = -side_tolerance;
        const real_point3d *previous = &points[point_count - 1];
        float previous_distance = real_math_plane_distance(plane, previous) - side_tolerance;
        bool previous_positive = previous_distance >= zero;
        for (long index = 0; index < point_count; ++index)
        {
            const real_point3d *point = &points[index];
            float distance = real_math_plane_distance(plane, point) - side_tolerance;
            bool positive = distance >= 0.0f;
            if (positive != previous_positive)
            {
                if (clipped_count >= 128)
                    break;
                real_math_intersection_point(previous, point, previous_distance, distance, &clipped_points[clipped_count++]);
            }
            if (positive == keep)
            {
                if (clipped_count >= 128)
                    break;
                clipped_points[clipped_count++] = *point;
            }
            previous_distance = distance;
            previous = point;
            previous_positive = positive;
        }
    }
    return clipped_count;
}

// @address 0xed2d8 ?angular_accelerate_to_position@@YAXPATreal_vector3d@@PBT1@M0MM@Z
void angular_accelerate_to_position(real_vector3d *direction, const real_vector3d *target, float dt, real_vector3d *angular_velocity,
    float maximum_velocity, float maximum_acceleration)
{
    if (!(maximum_acceleration > 0.0f) && !(maximum_velocity > 0.0f))
    {
        *angular_velocity = *global_zero_vector3d;
        *direction = *target;
    }
    else
    {
        bool arrived = false;
        float dot_cosine = dot_product3d(direction, target);
        if (dot_cosine < -1.0f)
            dot_cosine = -1.0f;
        else if (dot_cosine > 1.0f)
            dot_cosine = 1.0f;
        float stopping_velocity_squared = real_math_arccosine_clamped(dot_cosine) * maximum_acceleration * 2.0f;
        if (!(stopping_velocity_squared >= maximum_velocity * maximum_velocity))
            maximum_velocity = sqrtf(stopping_velocity_squared);
        real_vector3d axis;
        cross_product3d(direction, target, &axis);
        normalize3d(&axis);
        scale_vector3d(&axis, maximum_velocity, &axis);
        real_vector3d change;
        subtract_vectors3d(&axis, angular_velocity, &change);
        float change_squared = magnitude_squared3d(&change);
        if (!(change_squared >= (dt * maximum_acceleration) * (dt * maximum_acceleration) * 5.0f))
        {
            if (!(maximum_velocity >= dt * maximum_acceleration))
            {
                arrived = true;
                *angular_velocity = *global_zero_vector3d;
                *direction = *target;
            }
            else
                *angular_velocity = axis;
        }
        else
            point_from_line3d(angular_velocity, &change, dt * maximum_acceleration / sqrtf(change_squared), angular_velocity);
        if (!arrived)
        {
            axis = *angular_velocity;
            float angle = normalize3d(&axis) * dt;
            if (angle != 0.0f)
            {
                float cosine_angle = cosine(angle);
                float sine_angle = sine(angle);
                normalize3d(rotate_vector_about_axis(direction, &axis, sine_angle, cosine_angle));
            }
        }
    }
}

// Unplaced caller (dropped by /OPT:REF): c1xx /GL emits header-inline COMDAT definitions only when ODR-used in the TU.
real_vector3d *real_math_emit_header_inlines(real_vector3d *vector)
{
    (void)&project_point2d;
    return fast_normalize3d(vector);
}


// @address 0x11df78c g_particle_location_data_array

class c_particle_system;

__forceinline float c_particle_emitter::calc_matrix(
    const c_particle_emitter_definition *definition, const c_particle_system *system, const c_particle_state_list *states)
{
    const real_matrix4x3 *parent_orientation = datum_get<c_particle_location>(g_particle_location_data_array, m_location_index)->get_matrix();
    float scale = 1.0f;
    definition->evaluate_emitter_offset_if_not_constant_over_time(states, &m_property_values.emitter_offset);
    definition->evaluate_emitter_direction_if_not_constant_over_time(states, &m_property_values.emitter_direction);
    real_vector3d direction = m_property_values.emitter_direction;
    normalize3d(&direction);
    float yaw = (float)atan2(direction.j, direction.i);
    float pitch = (float)asin(direction.k);
    m_matrix = *parent_orientation;
    m_matrix.position.x *= scale;
    m_matrix.position.y *= scale;
    m_matrix.position.z *= scale;
    if (fabsf(pitch - 0.0f) >= k_real_epsilon || fabsf(yaw - 0.0f) >= k_real_epsilon)
    {
        real_matrix3x3 rotation;
        matrix3x3_rotation_from_angles(&rotation, yaw, pitch, 0.0f);
        matrix3x3_multiply(&m_matrix.rotation, &rotation, &m_matrix.rotation);
    }
    real_vector3d offset;
    matrix3x3_transform_vector(&m_matrix.rotation, &m_property_values.emitter_offset_vector, &offset);
    m_matrix.position.x += offset.i;
    m_matrix.position.y += offset.j;
    m_matrix.position.z += offset.k;
    set_matrix_initialized(true);
    return scale;
}
// @address 0x143bb0 particle_emitter_compute_matrix
extern "C" float particle_emitter_compute_matrix(c_particle_emitter *emitter, const c_particle_emitter_definition *definition,
    const c_particle_system *system, const c_particle_state_list *states)
{
    return emitter->calc_matrix(definition, system, states);
}

static inline s_collision_test_flags real_math_test_flags_new(unsigned long collision_flags, unsigned long object_flags)
{
    s_collision_test_flags result = { collision_flags, object_flags };
    return result;
}

// @address 0x7088e0 ??__Ek_biped_ground_collision_flags@@YAXXZ
s_collision_test_flags k_biped_ground_collision_flags = real_math_test_flags_new(0x849, 1);
// @address 0x708900 ??__Ek_biped_structure_collision_flags@@YAXXZ
s_collision_test_flags k_biped_structure_collision_flags = real_math_test_flags_new(0x801, 0);
// @address 0x708920 ??__Ek_biped_footstep_collision_flags@@YAXXZ
s_collision_test_flags k_biped_footstep_collision_flags = k_biped_ground_collision_flags;
// @address 0x708938 ??__Ek_placement_test_collision_flags@@YAXXZ
s_collision_test_flags k_placement_test_collision_flags = real_math_test_flags_new(0x14809, 0x81);
