// hkcollide/hkCapsuleShape.obj: from the records 0x622468 0x622480 0x622488 0x6224e8 0x622548 0x622588 0x6225c0 0x6225d0 0x6225f8 0x622708 0x622788 0x622bb0 0x622bd8 0x622be8 0x622c38 0x622d50
// @flags /TP /O2 /Oc
// @address 0x622c38 ?get_aabb_impl@phys_capsule_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
#include "phys/phys_base_types.h"
// @address 0x622488 ?gather_memory_stats@phys_capsule_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x622548 ?vertex_ids_to_vertices_impl@phys_capsule_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x6225c0 ?get_first_vertex@phys_capsule_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x6225d0 ?get_collision_spheres_impl@phys_capsule_shape@@UBAPBVphys_sphere@@PAV2@@Z
// @address 0x622bb0 ?fixup_loaded_capsule_shape@@YAXPAX@Z
// @address 0x622bd8 ?get_capsule_shape_vtable@@YAPBXXZ
// @address 0x622be8 ??0phys_capsule_shape@@QAA@ABVphys_vec4@@0M@Z
// @address 0x61ada8 ?trace_ray_with_collector@phys_convex_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x61acb8 ?get_max_projection@phys_convex_shape@@UBAMABVphys_vec4@@@Z
// @address 0x6224e8 ?get_support_vertex_impl@phys_capsule_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x622588 ?get_shape_centre_impl@phys_capsule_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x622788 ?trace_ray_impl@phys_capsule_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collide.h"

#include "phys/phys_vec4_util.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_sphere_util.h"
#include "phys/phys_sphere.h"

// @address 0x14d20 ?phys_simd_vec_1111@@3U__vector4@@B
// @rdata 0x82014d20..0x82014d30
const __vector4 phys_simd_vec_1111 = { 1.0f, 1.0f, 1.0f, 1.0f };
// @rdata-end

// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z

void phys_capsule_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("CapsuleShape", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}


void phys_capsule_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &vertex) const
{
    phys_vec4 axis;
    axis.set_sub4(m_vertex_b, m_vertex_a);
    float projection = static_cast<float>(axis.dot3(direction));
    int id;
    if (projection < 0.0f) {
        vertex.phys_vec4::operator=(m_vertex_a);
        id = 0;
    } else {
        vertex.phys_vec4::operator=(m_vertex_b);
        id = 16;
    }
    vertex.set_int24_w(id);
}


void phys_capsule_shape::vertex_ids_to_vertices_impl(const unsigned short *vertex_ids, int count, phys_hull_vertex *vertices) const
{
    for (int i = count - 1; i >= 0; i--) {
        unsigned int capsule_vertex_offset = *vertex_ids++;
        vertices->phys_vec4::operator=(*phys_add_byte_offset_const(get_vertices(), capsule_vertex_offset));
        vertices->set_int24_w(capsule_vertex_offset);
        vertices++;
    }
}

void phys_capsule_shape::get_shape_centre_impl(phys_vec4 &shape_centre) const
{
    shape_centre.set_add4(m_vertex_a, m_vertex_b);
    shape_centre.mul4(phys_simd_scalar(0.5f));
}


void phys_capsule_shape::get_first_vertex(phys_vec4 &vertex) const
{
    vertex.m_simd_quad = m_vertex_b.m_simd_quad;
}

const phys_sphere *phys_capsule_shape::get_collision_spheres_impl(phys_sphere *spheres) const
{
    spheres[0].set_position_and_radius(m_vertex_a);
    spheres[1].set_position_and_radius(m_vertex_b);
    return spheres;
}
// @address 0x6225f8 ?closest_inf_segment_pair@phys_capsule_shape@@SAXABVphys_vec4@@000AAM11AAV2@2@Z
// @address 0x622708 ?closest_point_on_segment@phys_capsule_shape@@SAXABVphys_vec4@@00AAV2@@Z
void phys_capsule_shape::closest_inf_segment_pair(const phys_vec4 &seg_start_a, const phys_vec4 &dir_a, const phys_vec4 &seg_start_b,
    const phys_vec4 &seg_dir_b, float &distance_sq, float &fraction_a, float &fraction_b, phys_vec4 &closest_on_a, phys_vec4 &closest_on_b)
{
    phys_vec4 diff;
    diff.set_sub4(seg_start_b, seg_start_a);
    float dirs_dot = static_cast<float>(dir_a.dot3(seg_dir_b));
    float seg_proj_a = static_cast<float>(dir_a.dot3(diff));
    float seg_proj_b = static_cast<float>(seg_dir_b.dot3(diff));
    float len_sq_a = static_cast<float>(dir_a.dot3(dir_a));
    float len_sq_b = static_cast<float>(seg_dir_b.dot3(seg_dir_b));
    float determinant = len_sq_a * len_sq_b - dirs_dot * dirs_dot;
    float tolerance = (fabsf(len_sq_a * len_sq_b) + dirs_dot * dirs_dot) * 9.5367431640625e-7f;
    if (fabsf(determinant) > tolerance) {
        fraction_a = (seg_proj_a * len_sq_b - seg_proj_b * dirs_dot) / determinant;
    } else {
        fraction_a = 0.0f;
    }
    fraction_b = (fraction_a * dirs_dot - seg_proj_b) / len_sq_b;
    phys_vec4 simplex_point_a;
    simplex_point_a = seg_start_a;
    simplex_point_a.add_scaled4(phys_simd_scalar(fraction_a), dir_a);
    phys_vec4 capsule_point_b;
    capsule_point_b = seg_start_b;
    capsule_point_b.add_scaled4(phys_simd_scalar(fraction_b), seg_dir_b);
    phys_vec4 separation;
    separation.set_sub4(simplex_point_a, capsule_point_b);
    closest_on_a = simplex_point_a;
    closest_on_b = capsule_point_b;
    distance_sq = static_cast<float>(separation.length_sq_3d());
}

void phys_capsule_shape::closest_point_on_segment(const phys_vec4 &point, const phys_vec4 &start, const phys_vec4 &end, phys_vec4 &closest)
{
    phys_vec4 diff;
    diff.set_sub4(start, point);
    phys_vec4 direction;
    direction.set_sub4(end, start);
    float projection = static_cast<float>(direction.dot3(diff));
    float dir_length_sq = static_cast<float>(direction.dot3(direction));
    float fraction = -projection;
    if (fraction <= 0.0f) {
        closest = start;
    } else if (fraction >= dir_length_sq) {
        closest = end;
    } else {
        fraction /= dir_length_sq;
        closest.set_add_mul4(start, direction, phys_simd_scalar(fraction));
    }
}


void fixup_loaded_capsule_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_capsule_shape(flag);
}

const void *get_capsule_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_capsule_shape)]; } buffer;
    new (buffer.storage) phys_capsule_shape(flag);
    return *(const void **)buffer.storage;
}

phys_capsule_shape::phys_capsule_shape(const phys_vec4 &capsule_vertex_a, const phys_vec4 &capsule_vertex_b, float radius)
    : phys_convex_shape(PHYS_SHAPE_CAPSULE, radius)
{
    m_vertex_a = capsule_vertex_a;
    m_vertex_b = capsule_vertex_b;
    m_vertex_a(3) = radius;
    m_vertex_b(3) = radius;
    phys_vec4 diff;
    diff.set_sub4(capsule_vertex_b, capsule_vertex_a);
    // March's length test belongs to the stripped assert that the vertices differ.
}

void phys_capsule_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_vec4 tol_vec4;
    tol_vec4.set_all3(tolerance + m_radius);
    phys_vec4 obj[2];
    phys_vec4_util::transform_points(local_to_world, get_vertices(), 2, obj);
    out.m_min_limit.set_min4(obj[0], obj[1]);
    out.m_min_limit.sub4(tol_vec4);
    out.m_max_limit.set_max4(obj[0], obj[1]);
    out.m_max_limit.add4(tol_vec4);
}

// @address 0x622468 ?destroy_loaded_capsule_shape@@YAXPAX@Z
void destroy_loaded_capsule_shape(void *p)
{
    static_cast<phys_capsule_shape *>(p)->~phys_capsule_shape();
}

// @address 0x622480 ?get_collision_sphere_count_impl@phys_capsule_shape@@UBAHXZ
int phys_capsule_shape::get_collision_sphere_count_impl() const
{
    return 2;
}


// @address 0x622d50 ?trace_sphere_ray@phys_sphere_util@@SA?AVphys_bool@@MABUphys_ray_input@@AAUphys_ray_output@@@Z
#ifdef HALO3_64BIT
__attribute__((used))
#endif
inline phys_bool phys_sphere_util::trace_sphere_ray(float radius, const phys_ray_input &input, phys_ray_output &results)
{
    do
    {
        float radius_sq = radius * radius;
        phys_vec4 dir;
        dir.set_sub4(input.m_ray_to, input.m_ray_from);
        float B = static_cast<float>(dir.dot3(input.m_ray_from));
        if (B >= 0.0f) {
            break;
        }
        float A = static_cast<float>(dir.length_sq_3d());
        float offset;
        phys_vec4 mid_point;
        if (B * B > A * radius_sq * 100.0f) {
            if (A < radius_sq) {
                break;
            }
            offset = -B;
            mid_point.set_lerp4(input.m_ray_from, input.m_ray_to, offset / A);
            B = 0.0f;
        } else {
            offset = 0.0f;
            mid_point = input.m_ray_from;
        }
        float C = static_cast<float>(mid_point.length_sq_3d()) - radius_sq;
        float capsule_determinant = B * B - A * C;
        if (capsule_determinant <= 0.0f) {
            break;
        }
        const float sqrt_det = __fsqrts(capsule_determinant);
        float t2 = -B - sqrt_det;
        float t = t2 + offset;
        if (t >= A * results.m_hit_frac) {
            break;
        }
        if (t < 0.0f) {
            break;
        }
        t = t / A;
        results.m_hit_frac = t;
        results.m_normal.set_lerp4(input.m_ray_from, input.m_ray_to, t);
        results.m_normal.mul4(1.0f / radius);
        results.set_cached_key(-1);
        return true;
    } while (false);
    return false;

}

// @address 0x622788 ?trace_ray_impl@phys_capsule_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_capsule_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    do
    {
        {
            phys_vec4 res;
            closest_point_on_segment(input.m_ray_from, get_vertex(0), get_vertex(1), res);
            phys_vec4 join;
            join.set_sub4(input.m_ray_from, res);
            float segment_to_cylinder_dist = static_cast<float>(join.length_3d());
            if (segment_to_cylinder_dist < m_radius) {
                break;
            }
        }
        {
            float inf_inf_dist_sq = 3.40282e+38f;
            float t;
            float u;
            phys_vec4 p;
            phys_vec4 q;
            phys_vec4 dA;
            dA.set_sub4(input.m_ray_to, input.m_ray_from);
            phys_vec4 dB;
            dB.set_sub4(get_vertex(1), get_vertex(0));
            closest_inf_segment_pair(input.m_ray_from, dA, get_vertex(0), dB, inf_inf_dist_sq, t, u, p, q);
            if (inf_inf_dist_sq > m_radius * m_radius) {
                break;
            }

            float capsule_axis_length;
            phys_vec4 axis;
            float intersect_param;
            {
                axis = dB;
                const float axis_len_sq = static_cast<float>(axis.length_sq_3d());
                if (axis_len_sq > 1.1920929e-07f) {
                    capsule_axis_length = static_cast<float>(axis.normalize_xyz_with_length());
                } else {
                    capsule_axis_length = 0.0f;
                    axis.set_zero4();
                }
                phys_vec4 dir = dA;
                float component = static_cast<float>(dir.dot3(axis));
                phys_vec4 capsule_flat_direction;
                capsule_flat_direction.set_add_mul4(dir, axis, phys_simd_scalar(-component));
                float d = __fsqrts(m_radius * m_radius - inf_inf_dist_sq);
                if (static_cast<float>(capsule_flat_direction.length_sq_3d()) != 0.0f) {
                    d *= static_cast<float>(capsule_flat_direction.inv_length_3d());
                    intersect_param = t - d;
                } else {
                    intersect_param = -1.0f;
                }
            }
            if (intersect_param >= results.m_hit_frac) {
                break;
            }

            float capsule_point_height;
            float point_a_proj = static_cast<float>(get_vertex(0).dot3(axis));
            phys_vec4 intersect_point;
            intersect_point.set_lerp4(input.m_ray_from, input.m_ray_to, phys_simd_scalar(intersect_param));
            const float point_proj = static_cast<float>(intersect_point.dot3(axis));
            capsule_point_height = point_proj - point_a_proj;

            if (intersect_param >= 0.0f) {
                if (capsule_point_height > 0.0f && capsule_point_height < capsule_axis_length) {
                    phys_vec4 axis_proj_point;
                    axis_proj_point.set_lerp4(get_vertex(0), get_vertex(1), phys_simd_scalar(capsule_point_height / capsule_axis_length));
                    phys_vec4 normal;
                    normal.set_sub4(intersect_point, axis_proj_point);
                    normal.normalize_xyz();
                    results.m_normal = normal;
                    results.m_hit_frac = intersect_param;
                    results.m_extra_info = 2;
                    results.set_cached_key(-1);
                    return true;
                }
            }
            {
                float from_local_proj = static_cast<float>(input.m_ray_from.dot3(axis));
                float proj_param = from_local_proj - point_a_proj;
                phys_vec4 from_pt_proj_axis;
                from_pt_proj_axis.set_lerp4(get_vertex(0), get_vertex(1), phys_simd_scalar(proj_param / capsule_axis_length));
                phys_vec4 axis_to_ray_start;
                axis_to_ray_start.set_sub4(input.m_ray_from, from_pt_proj_axis);
                if (static_cast<float>(axis_to_ray_start.length_3d()) > m_radius && intersect_param < 0.0f) {
                    break;
                }
                phys_ray_input sub_input;
                int cap_idx = (capsule_point_height <= 0.0f) ? 0 : 1;
                sub_input.m_ray_from.set_sub4(input.m_ray_from, get_vertex(cap_idx));
                sub_input.m_ray_to.set_sub4(input.m_ray_to, get_vertex(cap_idx));
                if (phys_sphere_util::trace_sphere_ray(m_radius, sub_input, results)) {
                    results.m_extra_info = cap_idx;
                    return true;
                }
                return false;
            }
        }
    } while (false);
    return false;

}

