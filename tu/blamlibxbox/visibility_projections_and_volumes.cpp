// blamlibxbox/visibility_projections_and_volumes.obj: from the records 0x154188 0x156c38 0x17cd90
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "math/real_math_bounds_storage.h"
// @address 0x7c5d34 k_null_rectangle3d_storage
// @address 0x154188 ?visibility_volume_build@@YA_NPBUvisibility_projection@@JPBTreal_rectangle2d@@PAUvisibility_volume@@@Z
#include <xtl.h>
#include <vectorintrinsics.h>
#include <xboxmath.h>
#include "visibility/visibility_projections_and_volumes.h"
#include "math/matrix_math.h"
#include "math/matrix_math_constants.h"
#include "math/real_math.h"


#include "visibility/visibility_projection_types.h"
#include "visibility/visibility_region_types.h"

// @address 0x32938 visibility_projections_and_volumes_minus_one
#include "math/real_constant_pool_access.h"
#include "math/real_math_functions.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"

#if !defined(_XBOX) || defined(__clang__)
#include "math/real_constant_pool.h"
#include "math/real_math_constant_pool.h"

const float k_real_constant_pool_zero[] = { 0.0f };
const float k_real_constants_minus_one[] = { -1.0f };
const float k_real_zero_storage = 0.0f;
#endif

static __forceinline void visibility_projections_and_volumes_set_point(real_point3d *point, const float &x, float y, float z)
{
    point->x = x;
    point->y = y;
    point->z = z;
}
static __forceinline float visibility_projections_and_volumes_first_normal(real_vector3d *normal, float i, float k)
{
    normal->i = i;
    normal->k = k;
    float zero = real_constant_pool_zero();
    normal->j = zero;
    return zero;
}
static __forceinline void visibility_projections_and_volumes_difference(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
}
static __forceinline void visibility_projections_and_volumes_enclose(real_rectangle3d *bounds, long count, const real_point3d *points)
{
    for (long i = 0; i < count; ++i)
        real_rectangle3d_enclose_point(bounds, &points[i]);
}
static __forceinline real_rectangle2d *visibility_projections_and_volumes_scale(const real_rectangle2d *rectangle, float scale, real_rectangle2d *result)
{
    result->x0 = scale * rectangle->x0;
    result->x1 = scale * rectangle->x1;
    result->y0 = scale * rectangle->y0;
    result->y1 = scale * rectangle->y1;
    return result;
}
static __forceinline real_vector3d *visibility_projections_and_volumes_scale_vector(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->i = scale * vector->i;
    result->j = scale * vector->j;
    result->k = scale * vector->k;
    return result;
}
static __forceinline real_point3d *visibility_projections_and_volumes_plane_point(const real_vector3d *normal, float distance, real_point3d *point)
{
    point->x = distance * normal->i;
    point->y = distance * normal->j;
    point->z = distance * normal->k;
    return point;
}
static __forceinline real_point3d *visibility_projections_and_volumes_plane_point_result(
    const real_vector3d *normal, float distance, real_point3d *const &point)
{
    point->x = distance * normal->i;
    point->y = distance * normal->j;
    point->z = distance * normal->k;
    return point;
}
static __forceinline float visibility_projections_and_volumes_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline float visibility_projections_and_volumes_point_dot(const real_point3d *point, const real_vector3d *normal)
{
    return point->y * normal->j + (point->x * normal->i + point->z * normal->k);
}
static __forceinline __vector4 visibility_projections_and_volumes_vector_set(float x, float y, float z, float w)
{
    __vector4 xyzw = __lvlx(&x, 0);
    __vector4 y_vector = __lvlx(&y, 0);
    __vector4 zw = __lvlx(&z, 0);
    __vector4 w_vector = __lvlx(&w, 0);
    xyzw = __vrlimi(xyzw, y_vector, 4, 3);
    zw = __vrlimi(zw, w_vector, 4, 3);
    return __vrlimi(xyzw, zw, 3, 2);
}
static __forceinline __vector4 visibility_projections_and_volumes_pack(const real_plane3d *plane)
{
    const real_vector3d *const &n = &plane->normal;
    real_vector3d scaled;
    visibility_projections_and_volumes_scale_vector(n, plane->distance, &scaled);
    return XMVectorSet(n->i, n->j, n->k, -(scaled.j * n->j + (scaled.i * n->i + scaled.k * n->k)));
}
bool visibility_volume_build(const visibility_projection *projection, long index, const real_rectangle2d *rectangle, visibility_volume *volume)
{
    real_rectangle2d &volume_rectangle = volume->rectangle;
    bool result = false;
    if (!(rectangle->x0 >= rectangle->x1) && !(rectangle->y0 >= rectangle->y1))
    {
        volume->rectangle = *rectangle;
        volume->index = (short)index;
        real_rectangle2d scaled;
        visibility_projections_and_volumes_scale(&volume->rectangle, projection->far_distance, &scaled);
        visibility_projections_and_volumes_set_point(&volume->points[1], scaled.x0, scaled.y0, -projection->far_distance);
        visibility_projections_and_volumes_set_point(&volume->points[3], scaled.x1, scaled.y0, -projection->far_distance);
        visibility_projections_and_volumes_set_point(&volume->points[0], scaled.x0, scaled.y1, -projection->far_distance);
        visibility_projections_and_volumes_set_point(&volume->points[2], scaled.x1, scaled.y1, -projection->far_distance);
        matrix4x3_transform_points(&projection->basis_to_world, 4, volume->points, volume->points);
        volume->points[4] = projection->basis_to_world.position;
        volume->bounds = k_null_rectangle3d_storage;
        visibility_projections_and_volumes_enclose(&volume->bounds, 5, volume->points);
        float minus_one = visibility_projections_and_volumes_minus_one;
        float zero = visibility_projections_and_volumes_first_normal(&volume->planes[0].normal, minus_one, -volume_rectangle.x0);
        normalize3d(&volume->planes[0].normal);
        volume->planes[0].distance = zero;
        matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[0], &volume->planes[0]);
        float one = real_constant_pool_one();
        set_real_vector3d(&volume->planes[1].normal, one, zero, volume_rectangle.x1);
        normalize3d(&volume->planes[1].normal);
        volume->planes[1].distance = zero;
        matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[1], &volume->planes[1]);
        set_real_vector3d(&volume->planes[2].normal, zero, minus_one, -volume_rectangle.y0);
        normalize3d(&volume->planes[2].normal);
        volume->planes[2].distance = zero;
        matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[2], &volume->planes[2]);
        set_real_vector3d(&volume->planes[3].normal, zero, one, volume_rectangle.y1);
        normalize3d(&volume->planes[3].normal);
        volume->planes[3].distance = zero;
        matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[3], &volume->planes[3]);
        if (projection->near_plane_valid)
        {
            volume->planes[4].normal.i = -projection->near_plane.normal.i;
            volume->planes[4].normal.j = -projection->near_plane.normal.j;
            volume->planes[4].normal.k = -projection->near_plane.normal.k;
            volume->planes[4].distance = -projection->near_plane.distance;
        }
        else
        {
            set_real_vector3d(&volume->planes[4].normal, zero, zero, one);
            volume->planes[4].distance = zero;
            matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[4], &volume->planes[4]);
        }
        set_real_vector3d(&volume->planes[5].normal, zero, zero, minus_one);
        volume->planes[5].distance = projection->far_distance;
        matrix4x3_transform_plane(&projection->basis_to_world, &volume->planes[5], &volume->planes[5]);
        visibility_projections_and_volumes_difference(&volume->points[4], &volume->points[0], &volume->directions[0]);
        visibility_projections_and_volumes_difference(&volume->points[4], &volume->points[1], &volume->directions[1]);
        visibility_projections_and_volumes_difference(&volume->points[4], &volume->points[2], &volume->directions[2]);
        visibility_projections_and_volumes_difference(&volume->points[4], &volume->points[3], &volume->directions[3]);
        for (long plane_index = 0; plane_index < 6; ++plane_index)
        {
            const real_plane3d *plane = &volume->planes[plane_index];
            const real_vector3d *normal = &(volume->planes[plane_index]).normal;
            real_point3d plane_point;
            visibility_projections_and_volumes_plane_point_result(normal, (volume->planes[plane_index]).distance, &plane_point);
            __vector4 packed = visibility_projections_and_volumes_vector_set(volume->planes[plane_index].normal.i,
                volume->planes[plane_index].normal.j, volume->planes[plane_index].normal.k,
                -visibility_projections_and_volumes_point_dot(&plane_point, normal));
            volume->vector_planes[plane_index] = packed;
        }
        result = true;
    }
    return result;
}

static __forceinline const visibility_cluster *visibility_projections_and_volumes_get_cluster(const s_visibility_region *region, long index)
{
    return &region->clusters[index];
}
// @address 0x156c38 ?visibility_region_test_sphere@@YA_NPBUs_visibility_region@@JPBTreal_point3d@@MPA_N@Z
bool visibility_region_test_sphere(const s_visibility_region *region, long cluster_index, const real_point3d *center, float radius, bool *inside_out)
{
    const visibility_cluster *cluster = visibility_projections_and_volumes_get_cluster(region, cluster_index);
    bool result = false;
    bool inside = false;
    for (long group = 0; group < region->projection_count; ++group)
    {
        for (long index = 0; index < cluster->volume_count[group] && !inside; ++index)
        {
            long absolute_index = cluster->first_volume[group] + index;
            const visibility_volume *volume = &region->volumes[absolute_index];
            __dcbt(0, &region->volumes[absolute_index + 1]);
            long test = visibility_volume_test_sphere_planes(volume->vector_planes, center, radius);
            result |= test != 0;
            inside |= test == 2;
        }
    }
    *inside_out = inside;
    return result;
}

// NOTE (lane ms-sym-5582): the 15 data aliases at 0x8202251c/0x82032938 owned by this obj
// cannot be defined here. A second extern at 0x2251c grows .text and breaks global layout
// (even one: +0x10, pdata/file-static cascade); a global -1.0f re-optimizes 0x154188 and
// unpools 0x17cd90's -1.0f load to a private pool; DAT_8202251c needs its two conflicting
// header declarations unified first. The bytes stay image-owned; each alias needs wpl overlap
// support or a TU of its own. See the lane REPORT for the check/probe evidence.
#if defined(_XBOX) && !defined(__clang__)
namespace visibility_projections_and_volumes_constants
{
// @rdata 0x82032938
static const float visibility_projections_and_volumes_minus_one = -1.0f;
}
#else
// this unit, rasterizer_states and visibility_portal_traversal read the extern "C" one from
// visibility/visibility_projections_and_volumes.h; the namespace-scope static above is a different object
const float visibility_projections_and_volumes_minus_one = -1.0f;
#endif
// @rdata 0x8202251c
const float visibility_projections_and_volumes_zero = 0.0f;
static __forceinline real_plane3d *visibility_projections_and_volumes_plane(real_plane3d *plane, const real_point3d *point, const real_vector3d *normal)
{
 plane->normal = *normal;
 plane->distance = (point->z * plane->normal.k + point->y * plane->normal.j) + point->x * plane->normal.i;
 return plane;
}
static __forceinline real_vector3d *visibility_projections_and_volumes_negate_vector(const real_vector3d *vector, real_vector3d *result)
{
 result->i = -vector->i;
 result->j = -vector->j;
 result->k = -vector->k;
 return result;
}
static __forceinline real_plane3d *visibility_projections_and_volumes_negate_plane(const real_plane3d *source, real_plane3d *plane)
{
 plane->normal.i = -source->normal.i;
 plane->normal.j = -source->normal.j;
 plane->normal.k = -source->normal.k;
 plane->distance = -source->distance;
 return plane;
}
static __forceinline float visibility_projections_and_volumes_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
 return plane->normal.i * point->x + plane->normal.j * point->y + plane->normal.k * point->z - plane->distance;
}
static __forceinline float visibility_projections_and_volumes_distance_to_point(const real_plane3d *plane, const real_point3d *point)
{
 return visibility_projections_and_volumes_point_dot(point, &plane->normal) - plane->distance;
}
static __forceinline float visibility_projections_and_volumes_intersect(
    const real_point3d *point, const real_vector3d *direction, const real_plane3d *plane, float neg_one, float zero)
{
 float denominator = visibility_projections_and_volumes_dot(&plane->normal, direction);
 return denominator != zero ? visibility_projections_and_volumes_distance_to_point(plane, point) * (neg_one / denominator) : zero;
}
static __forceinline real_point3d *visibility_projections_and_volumes_point_on_line(
    const real_point3d *point, const real_vector3d *direction, float t, real_point3d *result)
{
 result->x = t * direction->i + point->x;
 result->y = t * direction->j + point->y;
 result->z = t * direction->k + point->z;
 return result;
}
static __forceinline real_point3d *visibility_projections_and_volumes_line_plane_intersection(
    const real_point3d *point, const real_vector3d *direction, const real_plane3d *plane, real_point3d *result, float neg_one, float zero)
{
 float t = visibility_projections_and_volumes_intersect(point, direction, plane, neg_one, zero);
 return visibility_projections_and_volumes_point_on_line(point, direction, t, result);
}
static __forceinline real_point3d *visibility_projections_and_volumes_transform(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
 float x = point->x;
 float y = point->y;
 float z = point->z;
 x *= matrix->scale; y *= matrix->scale; z *= matrix->scale;
 result->x = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i + matrix->position.x;
 result->y = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j + matrix->position.y;
 result->z = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k + matrix->position.z;
 return result;
}
static __forceinline void visibility_projections_and_volumes_enclose_projected(real_rectangle3d *bounds, const real_point3d *point)
{
 if (bounds->x0 > point->x) bounds->x0 = point->x;
 if (bounds->x1 < point->x) bounds->x1 = point->x;
 if (bounds->y0 > point->y) bounds->y0 = point->y;
 if (bounds->y1 < point->y) bounds->y1 = point->y;
 if (bounds->z0 > point->z) bounds->z0 = point->z;
 if (bounds->z1 < point->z) bounds->z1 = point->z;
}
// @address 0x17cd90 ?visibility_project_connected_points_clipped@@YA_NPBUvisibility_projection@@MPBTreal_point3d@@JPAT2@PAFJPATreal_rectangle3d@@_N55@Z
bool visibility_project_connected_points_clipped(const visibility_projection *projection, float margin, const real_point3d *world_points,
    long world_point_count, real_point3d *points_out, short *count_out, long maximum_points, real_rectangle3d *bounds, bool clip_near,
    bool clip_far, bool unused)
{
 // Project the 24 connected world points against the near plane without point output.
 world_point_count = 24;
 points_out = NULL;
 count_out = NULL;
 clip_near = true;
 clip_far = false;
 bool empty = true;
 clip_far &= projection->far_plane_valid || projection->far_plane_reversed;
 *bounds = k_null_rectangle3d_storage;
 unsigned char rejected[16];
 memset(rejected, 0, sizeof(rejected));
 float near_distance = projection->volume.distance + margin >= 0.000015258799976669252f ? projection->volume.distance + margin : 0.000015258799976669252f;
 real_plane3d near_plane;
 visibility_projections_and_volumes_plane(&near_plane, &projection->volume.points[4], &projection->basis_to_world.rotation.up);
 visibility_projections_and_volumes_negate_plane(&near_plane, &near_plane);
 near_plane.distance += near_distance;
 real_vector3d far_normal;
 visibility_projections_and_volumes_negate_vector(&projection->basis_to_world.rotation.up, &far_normal);
 real_plane3d far_plane;
 const real_point3d *const &far_point = &projection->volume.points[0];
 visibility_projections_and_volumes_plane(&far_plane, far_point, &far_normal);
 visibility_projections_and_volumes_negate_plane(&far_plane, &far_plane);
 real_point3d points[32];
 memcpy(points, world_points, world_point_count * sizeof(real_point3d));
 const float neg_one = -1.0f;
 const float zero = 0.0f;
 for (long plane_index = 0; plane_index < 2; ++plane_index) {
  if (plane_index == 0 && !clip_near) continue;
  if (plane_index == 1 && !clip_far) continue;
  const real_plane3d *plane = plane_index == 0 ? &near_plane : &far_plane;
  bool sides[32];
  for (long i = 0; i < world_point_count; ++i)
   sides[i] = visibility_projections_and_volumes_plane_distance(plane, &points[i]) > -0.0001f;
  for (long edge = 0; edge < world_point_count / 2; ++edge) {
   long index_zero = edge * 2;
   long index_one = index_zero + 1;
   rejected[edge] |= !sides[index_zero] && !sides[index_one];
   if (!rejected[edge] && sides[index_zero] != sides[index_one]) {
    real_vector3d direction;
    visibility_projections_and_volumes_difference(&points[index_zero], &points[index_one], &direction);
    real_point3d intersection;
    visibility_projections_and_volumes_line_plane_intersection(&points[index_zero], &direction, plane, &intersection, neg_one, zero);
    if (!sides[index_zero]) points[index_zero] = intersection;
    else points[index_one] = intersection;
   }
  }
 }
 long clipped_point_count = 0;
 for (long edge = 0; edge < world_point_count / 2; ++edge) {
  if (!rejected[edge]) {
        real_rectangle3d * bounds_blk = bounds;
   for (long endpoint = 0; endpoint < 2; ++endpoint) {
    long index = edge * 2 + endpoint;
    real_point3d view;
    visibility_projections_and_volumes_transform(&projection->world_to_basis, &points[index], &view);
    float depth = view.z > -0.000015258799976669252f ? -0.000015258799976669252f : view.z;
    float scale = neg_one / depth;
    real_point3d projected;
    projected.x = view.x * scale;
    projected.y = view.y * scale;
    projected.z = view.z;
    if (points_out) points_out[clipped_point_count] = projected;
    empty = false;
    visibility_projections_and_volumes_enclose_projected(bounds_blk, &projected);
    ++clipped_point_count;
   }
  }
 }
 if (count_out) *count_out = (short)clipped_point_count;
 return !empty;
}
