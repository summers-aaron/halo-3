// hkcollide/hkConvexVerticesShape.obj: from the records 0x1c13e8 0x623c98 0x623cb0 0x623cb8 0x623cc8 0x623d00 0x623ec0 0x624080 0x624288 0x624298 0x6243b0 0x624498 0x624950 0x624970 0x624a58 0x624a80 0x624a90 0x624bb8
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x624298 ?trace_ray_impl@phys_convex_hull_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x623c98 ?destroy_loaded_convex_hull_shape@@YAXPAX@Z
// @address 0x623d00 ?get_collision_spheres_impl@phys_convex_hull_shape@@UBAPBVphys_sphere@@PAV2@@Z
// @address 0x624288 ?get_shape_centre_impl@phys_convex_hull_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x623cb0 ?get_collision_sphere_count_impl@phys_convex_hull_shape@@UBAHXZ
// @address 0x623cb8 ?fetch_vec_from_array@@YAABVphys_vec4@@PBV1@H@Z
// @address 0x623cc8 ?get_plane_equations@phys_convex_hull_shape@@QBAABV?$phys_array@Vphys_vec4@@@@XZ
// @address 0x624950 ?get_aabb_impl@phys_convex_hull_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x624a58 ?fixup_loaded_convex_hull_shape@@YAXPAX@Z
// @address 0x624a80 ?get_convex_hull_shape_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_collide.h"
#include "phys/phys_aabb_util.h"
#include "phys/phys_statistics_collector.h"


#include "phys/phys_convex_hull_shape.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_thread_memory.h"

static __forceinline float plane_point_dist(const phys_vec4 &plane, const phys_vec4 &point)
{
    float distance;
    __stvewx(__vaddfp(__vspltw(plane.m_simd_quad, 3), __vmsum3fp(plane.m_simd_quad, point.m_simd_quad)), &distance, 0);
    return distance;
}




extern "C" {
}

int phys_convex_hull_shape::get_collision_sphere_count_impl() const
{
    return m_hull_vertex_count;
}

const phys_vec4 &fetch_vec_from_array(const phys_vec4 *vectors, int index)
{
    return vectors[index];
}

const phys_array<phys_vec4> &phys_convex_hull_shape::get_plane_equations() const
{
    return m_plane_equations;
}

const phys_sphere *phys_convex_hull_shape::get_collision_spheres_impl(phys_sphere *spheres) const
{
    phys_sphere *sphere = spheres;
    const four_vecs *vertices = m_rotated_vertices.m_data;
    int remaining = m_hull_vertex_count - 1;
    for (; remaining >= 3; remaining -= 4) {
        (sphere++)->get_position_and_radius().set(vertices->m_x(0), vertices->m_y(0), vertices->m_soa_z(0), m_radius);
        (sphere++)->get_position_and_radius().set(vertices->m_x(1), vertices->m_y(1), vertices->m_soa_z(1), m_radius);
        (sphere++)->get_position_and_radius().set(vertices->m_x(2), vertices->m_y(2), vertices->m_soa_z(2), m_radius);
        (sphere++)->get_position_and_radius().set(vertices->m_x(3), vertices->m_y(3), vertices->m_soa_z(3), m_radius);
        vertices++;
    }
    if (remaining >= 0)
        (sphere++)->get_position_and_radius().set(vertices->m_x(0), vertices->m_y(0), vertices->m_soa_z(0), m_radius);
    if (remaining >= 1)
        (sphere++)->get_position_and_radius().set(vertices->m_x(1), vertices->m_y(1), vertices->m_soa_z(1), m_radius);
    if (remaining >= 2)
        sphere->get_position_and_radius().set(vertices->m_x(2), vertices->m_y(2), vertices->m_soa_z(2), m_radius);
    return spheres;
}

void phys_convex_hull_shape::get_shape_centre_impl(phys_vec4 &shape_centre) const
{
    shape_centre.m_simd_quad = m_hull_aabb_center.m_simd_quad;
}

phys_bool phys_convex_hull_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    phys_vec4 best_face_normal;
    best_face_normal.set_zero4();
    float exit_frac = results.m_hit_frac;
    float enter_frac = -1.0f;
    for (int i = m_plane_equations.m_size - 1; i >= 0; i--) {
        const phys_vec4 &plane = m_plane_equations.m_data[i];
        float dist_from = plane_point_dist(plane, input.m_ray_from);
        float dist_to = plane_point_dist(plane, input.m_ray_to);
        if (dist_from >= 0.0f) {
            if (dist_to >= 0.0f)
                return false;
            float fraction = dist_from / (dist_from - dist_to);
            if (enter_frac <= fraction) {
                enter_frac = fraction;
                best_face_normal = plane;
            }
        } else {
            if (dist_to < 0.0f)
                continue;
            float fraction = dist_from / (dist_from - dist_to);
            exit_frac = phys_math::min_of_two(exit_frac, fraction);
        }
        if (exit_frac < enter_frac)
            return false;
    }
    if (enter_frac < 0.0f)
        return false;
    results.m_hit_frac = enter_frac;
    results.m_normal = best_face_normal;
    results.set_cached_key((unsigned int)-1);
    return true;
}

// @address 0x6243b0 ?get_original_vertices@phys_convex_hull_shape@@QBAXAAV?$phys_array@Vphys_vec4@@@@@Z
void phys_convex_hull_shape::get_original_vertices(phys_array<phys_vec4> &vertices) const
{
    vertices.set_array_size(m_hull_vertex_count);
    for (int i = 0; i < m_hull_vertex_count; ++i) {
        int block = i / 4;
        int lane = i & 3;
        vertices[i].set(m_rotated_vertices[block].m_x(lane), m_rotated_vertices[block].m_y(lane),
            m_rotated_vertices[block].m_soa_z(lane), 0.0f);
    }
}

void phys_convex_hull_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    phys_aabb_util::compute_bounds(transform, m_aabb_half_extents, m_hull_aabb_center, m_radius + tolerance, out);
}

// @address 0x624970 ?gather_memory_stats@phys_convex_hull_shape@@UBAXPAVphys_statistics_collector@@@Z
void phys_convex_hull_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("CvxVerts", phys_statistics_collector::stat_category_shared, this);
    collector->add_array_stats("Verts", phys_statistics_collector::stat_category_shared, m_rotated_vertices);
    collector->add_array_stats("Verts", phys_statistics_collector::stat_category_shared, m_plane_equations);
    collector->end_object();
}


void destroy_loaded_convex_hull_shape(void *object)
{
    static_cast<phys_convex_hull_shape *>(object)->~phys_convex_hull_shape();
}

void fixup_loaded_convex_hull_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_convex_hull_shape(flag);
}

const void *get_convex_hull_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_convex_hull_shape)]; } buffer;
    new (buffer.storage) phys_convex_hull_shape(flag);
    return *(const void **)buffer.storage;
}

// @address 0x623ec0 ?get_support_vertex_impl@phys_convex_hull_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
void phys_convex_hull_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    const four_vecs *fv = m_rotated_vertices.m_data;
    const float neg_real_max = -3.40282e+38f;
    static __vector4 neg_max_quad = { neg_real_max, neg_real_max, neg_real_max, neg_real_max };
    static const __vector4 first_lane_indices = { 0.5f, 1.5f, 2.5f, 3.5f };
    static const __vector4 index_step = { 4.0f, 4.0f, 4.0f, 4.0f };
    phys_vec4 best_dots;
    best_dots.m_simd_quad = neg_max_quad;
    phys_vec4 cur_indices;
    cur_indices.m_simd_quad = first_lane_indices;
    phys_vec4 best_indices;

    int max_vertex_cnt = m_rotated_vertices.m_size << 2;
    __vector4 d0 = __vspltw(direction.m_simd_quad, 0);
    __vector4 d1 = __vspltw(direction.m_simd_quad, 1);
    __vector4 d2 = __vspltw(direction.m_simd_quad, 2);
    const four_vecs *p = fv;
    for (int i = 0; i < max_vertex_cnt; p++, i += 4) {
        __vector4 x = __vmulfp(d0, p->m_x.m_simd_quad);
        __vector4 y = __vmulfp(d1, p->m_y.m_simd_quad);
        __vector4 cur_dots = __vaddfp(x, y);
        __vector4 z = __vmulfp(d2, p->m_soa_z.m_simd_quad);
        cur_dots = __vaddfp(cur_dots, z);
        __vector4 comp = __vcmpgtfp(cur_dots, best_dots.m_simd_quad);
        best_dots.m_simd_quad = __vsel(best_dots.m_simd_quad, cur_dots, comp);
        best_indices.m_simd_quad = __vsel(best_indices.m_simd_quad, cur_indices.m_simd_quad, comp);
        cur_indices.m_simd_quad = __vaddfp(cur_indices.m_simd_quad, index_step);
    }

    int best_of_01 = best_dots(0) > best_dots(1) ? 0 : 1;
    int best_of_23 = best_dots(2) > best_dots(3) ? 2 : 3;
    int best_lane_idx = best_dots(best_of_01) > best_dots(best_of_23) ? best_of_01 : best_of_23;

    int vertex_id = (int)best_indices(best_lane_idx);
    const four_vecs &f = fv[(unsigned int)vertex_id >> 2];
    int a = vertex_id & 3;
    supporting_vertex_out(0) = f.m_x(a);
    supporting_vertex_out(1) = f.m_y(a);
    supporting_vertex_out(2) = f.m_soa_z(a);
    supporting_vertex_out.set_int24_w(vertex_id);
}

// @address 0x624080 ?vertex_ids_to_vertices_impl@phys_convex_hull_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
void phys_convex_hull_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int id_count, phys_hull_vertex *vertices_out) const
{
    for (int i = id_count - 1; i >= 0; i--) {
        int vertex_id = ids[0];
        const four_vecs &f = m_rotated_vertices.m_data[vertex_id >> 2];
        int a = vertex_id & 3;
        phys_vec4 &v = vertices_out[0];
        v(0) = f.m_x(a);
        v(1) = f.m_y(a);
        v(2) = f.m_soa_z(a);
        v.set_int24_w(vertex_id);
        vertices_out++;
        ids++;
    }
}





// @address 0x624498 ?copy_vertex_data@phys_convex_hull_shape@@QAAXPBMHH@Z
void phys_convex_hull_shape::copy_vertex_data(const float *vertex_in, int byte_stride, int hull_vertex_count)
{
    m_hull_vertex_count = hull_vertex_count;
    int padded_cnt = (hull_vertex_count + 3) & ~3;
    int need = padded_cnt / 4;
    m_rotated_vertices.set_array_size(need);

    const float *v = vertex_in;
    int i;
    for (i = 0; i < hull_vertex_count; i++) {
        int block = i / 4;
        int lane = i & 3;
        m_rotated_vertices.m_data[block].m_x(lane) = v[0];
        m_rotated_vertices.m_data[block].m_y(lane) = v[1];
        m_rotated_vertices.m_data[block].m_soa_z(lane) = v[2];
        v = (const float *)((const char *)v + byte_stride);
    }
    v = (const float *)((const char *)v - byte_stride);
    for (; i < padded_cnt; i++) {
        int block = i >> 2;
        int lane = i & 3;
        m_rotated_vertices.m_data[block].m_x(lane) = v[0];
        m_rotated_vertices.m_data[block].m_y(lane) = v[1];
        m_rotated_vertices.m_data[block].m_soa_z(lane) = v[2];
    }

    phys_aabb aabb;
    phys_aabb_util::compute_bounds(vertex_in, hull_vertex_count, byte_stride, aabb);
    m_hull_aabb_center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
    m_hull_aabb_center.mul4(phys_simd_scalar(0.5f));
    m_aabb_half_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    m_aabb_half_extents.mul4(phys_simd_scalar(0.5f));
}

// @address 0x1c13e8 ??_Gphys_convex_hull_shape@@UAAPAXI@Z
// @address 0x1c1240 ?compute_dma_size@phys_convex_hull_shape@@UBAHH@Z
// @address 0x623cd0 ?get_first_vertex@phys_convex_hull_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x61acb8 ?get_max_projection@phys_convex_shape@@UBAMABVphys_vec4@@@Z
// @address 0x61ada8 ?trace_ray_with_collector@phys_convex_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x42d298 ?get_shape_container@phys_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x61acb0 ?weld_contact_point_impl@phys_convex_shape@@MBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBV1@32@Z
// @address 0x624a90 ??0phys_convex_hull_shape@@QAA@Uphys_strided_vertices@@ABV?$phys_array@Vphys_vec4@@@@M@Z
phys_convex_hull_shape::phys_convex_hull_shape(phys_strided_vertices vertex_in, const phys_array<phys_vec4> &hull_plane_equations, float radius)
    : phys_convex_shape(PHYS_SHAPE_CONVEX_HULL, radius)
{
    m_plane_equations = hull_plane_equations;
    copy_vertex_data(vertex_in.m_vertices, vertex_in.m_vertex_stride_bytes, vertex_in.m_hull_vertex_count);
}

// @address 0x624bb8 ??0phys_convex_hull_shape@@QAA@PAUfour_vecs@0@HPAVphys_vec4@@HABVphys_aabb@@M@Z
phys_convex_hull_shape::phys_convex_hull_shape(four_vecs *hull_rotated_vertices, int hull_vertex_count, phys_vec4 *planes,
    int hull_plane_count, const phys_aabb &aabb, float radius)
    : phys_convex_shape(PHYS_SHAPE_CONVEX_HULL, radius),
      m_rotated_vertices(hull_rotated_vertices, ((hull_vertex_count + 3) & ~3) / 4, ((hull_vertex_count + 3) & ~3) / 4),
      m_hull_vertex_count(hull_vertex_count),
      m_plane_equations(planes, hull_plane_count, hull_plane_count)
{
    m_aabb_half_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    m_aabb_half_extents.mul4(0.5f);
    m_hull_aabb_center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
    m_hull_aabb_center.mul4(0.5f);
}

// The first vertex is lane 0 of the transposed block; scalar stores keep the exact instruction
// sequence where the vectorized set() would route through a stack temporary.
void phys_convex_hull_shape::get_first_vertex(phys_vec4 &vertex) const
{
    vertex(0) = m_rotated_vertices[0].m_x(0);
    vertex(1) = m_rotated_vertices[0].m_y(0);
    vertex(2) = m_rotated_vertices[0].m_soa_z(0);
    vertex(3) = 0.0f;
}
