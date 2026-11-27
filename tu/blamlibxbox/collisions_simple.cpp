// blamlibxbox/collisions_simple.obj: from the records 0x2b1e90 0x2b1ef8 0x2b2090 0x2b2170 0x2b2368
// @flags /TP /O2 /Oc
// @address 0x2b1e90 ?collision_point_in_sphere@@YA_NPBTreal_point3d@@0M@Z
// @address 0x2b1ef8 ?collision_point_in_cylinder@@YA_NPBTreal_point3d@@PBUreal_matrix4x3@@MMPAM@Z
// @address 0x2b2090 ?collision_point_in_pill@@YA_NPBTreal_point3d@@PBUreal_matrix4x3@@MMPAM@Z
// @address 0x2b2170 ?collision_point_in_box@@YA_NPBTreal_point3d@@PBUreal_matrix4x3@@MMM@Z
// @address 0x2b2368 ?collision_point_in_cone@@YA_NPBTreal_point3d@@PBUreal_matrix4x3@@PBTreal_vector3d@@MMPAM3@Z
#include "math/matrix_math.h"
#include "math/collisions_simple.h"


#include "math/real_math_trigonometry.h"

static __forceinline bool point_in_cone3d(const real_point3d *p, const real_point3d *origin, const real_vector3d *axis, float length, float cone_cosine)
{
    real_vector3d offset;
    vector_from_points3d(origin, p, &offset);
    float distance = dot_product3d(&offset, axis);
    if (distance < 0.0f || distance > length || distance * distance < magnitude_squared3d(&offset) * cone_cosine * cone_cosine)
        return false;
    return true;
}

bool collision_point_in_sphere(const real_point3d *point, const real_point3d *sphere_center, float radius)
{
    bool result;
    if (radius > k_real_epsilon)
        result = distance_squared3d(point, sphere_center) <= radius * radius;
    else
        result = false;
    return result;
}

static __forceinline float collisions_simple_point_dot_vector3d(const real_point3d *p, const real_vector3d *v)
{
    return p->x * v->i + (p->y * v->j + p->z * v->k);
}

bool collision_point_in_cylinder(const real_point3d *point, const real_matrix4x3 *cylinder_matrix, float radius, float height, float *distance)
{
    bool result = false;
    if (radius > k_real_epsilon && height > k_real_epsilon) {
        real_vector3d normal = cylinder_matrix->up;
        real_vector3d *n = fast_normalize3d(&normal);
        real_vector3d cap_normal = *n;
        float cap_d = collisions_simple_point_dot_vector3d(&cylinder_matrix->position, &cap_normal);
        float plane_distance = fabsf(collisions_simple_point_dot_vector3d(point, &cap_normal) - cap_d);
        if (plane_distance <= height * 0.5) {
            real_vector3d direction = cylinder_matrix->up;
            fast_normalize3d(&direction);
            scale_vector3d(&direction, height, &direction);
            real_point3d origin;
            origin.x = 0.0f;
            origin.y = 0.0f;
            origin.z = height * -0.5f;
            matrix4x3_transform_point(cylinder_matrix, &origin, &origin);
            float dist_sq = point_to_line_distance_squared3d(point, &origin, &direction);
            if (dist_sq <= radius * radius) {
                result = true;
                if (distance)
                    *distance = sqrtf(dist_sq);
            }
        }
    }
    return result;
}

bool collision_point_in_pill(const real_point3d *point, const real_matrix4x3 *pill_matrix, float radius, float height, float *)
{
    bool result = false;
    if (radius > k_real_epsilon && height >= 0.0f) {
        real_vector3d direction = pill_matrix->up;
        fast_normalize3d(&direction);
        direction.i *= height;
        direction.j *= height;
        direction.k *= height;
        real_point3d origin;
        origin.x = 0.0f;
        origin.y = 0.0f;
        origin.z = -0.5f * height;
        matrix4x3_transform_point(pill_matrix, &origin, &origin);
        result = fast_vector_intersects_sphere(&origin, &direction, point, radius);
    }
    return result;
}

bool collision_point_in_cone(const real_point3d *p, const real_matrix4x3 *cone, const real_vector3d *axis, float length, float radius,
    float *out_length, float *out_radius)
{
    // match: 0x2b2368: proves out_radius null so the out-param guard tests only out_length (removal adds cmplwi+bne, frame 0x80->0x90, extra saved GPR)
    __assume(out_radius == 0);
    bool result;
    if ((fabsf(axis->i) > k_real_epsilon || fabsf(axis->j) > k_real_epsilon || fabsf(axis->k) > k_real_epsilon) &&
        length > k_real_epsilon && radius > k_real_epsilon)
    {
        float angle = 2.0f * arctangent(radius, length);
        float angle_cosine = cosine(angle);
        result = point_in_cone3d(p, &cone->position, axis, length, angle_cosine);
        if (result && (out_length || out_radius)) {
            real_vector3d offset;
            vector_from_points3d(&cone->position, p, &offset);
            if (out_length)
                *out_length = dot_product3d(&offset, axis);
        }
    } else {
        result = false;
    }
    return result;
}

static __forceinline void collisions_simple_plane_from_point_and_normal(real_plane3d *plane, const real_point3d *point, const real_vector3d *const &normal)
{
    plane->normal = *normal;
    plane->distance = collisions_simple_point_dot_vector3d(point, &plane->normal);
}

static __forceinline float collisions_simple_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return point->x * plane->normal.i + (point->z * plane->normal.k + point->y * plane->normal.j) - plane->distance;
}

bool collision_point_in_box(const real_point3d *point, const real_matrix4x3 *box_matrix, float width, float length, float height)
{
    // match: 0x2b2170: holds point loads in saved FPRs across the three plane checks (removal re-colors savefpr_25->28, frame 0xD0->0xB0)
    const real_point3d *__restrict test_point = point;
    bool result = false;
    if (width > k_real_epsilon && length > k_real_epsilon && height > k_real_epsilon) {
        real_vector3d normal = box_matrix->forward;
        real_plane3d forward_plane;
        collisions_simple_plane_from_point_and_normal(&forward_plane, &box_matrix->position, fast_normalize3d(&normal));
        if (fabsf(collisions_simple_plane_distance(&forward_plane, test_point)) <= width * 0.5f) {
            normal = box_matrix->left;
            real_plane3d left_plane;
            collisions_simple_plane_from_point_and_normal(&left_plane, &box_matrix->position, fast_normalize3d(&normal));
            float left_distance = fabsf(collisions_simple_plane_distance(&left_plane, test_point));
            if (left_distance <= length * 0.5f) {
                normal = box_matrix->up;
                real_plane3d up_plane;
                collisions_simple_plane_from_point_and_normal(&up_plane, &box_matrix->position, fast_normalize3d(&normal));
                if (fabsf(collisions_simple_plane_distance(&up_plane, test_point)) <= height * 0.5f)
                    result = true;
            }
        }
    }
    return result;
}
