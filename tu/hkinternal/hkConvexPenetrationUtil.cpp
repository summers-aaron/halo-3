// hkinternal/hkConvexPenetrationUtil.obj: from the records 0x6a92e0 0x6a92e8 0x6a92f0 0x6a9328 0x6a9390 0x6a93f0 0x6a94e0 0x6a9620 0x6a97b8 0x6a98a0 0x6a9a30 0x6a9b70 0x6a9da8 0x6a9e70 0x6aa358 0x6aa590 0x6aa678 0x6aab10 0x6aaba0
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x6a92e0 ?simplex_code_begin@phys_convex_penetration_util@@SAXXZ
// @address 0x6a92e8 ?simplex_code_end@phys_convex_penetration_util@@SAXXZ
// @address 0x6a9328 ?add_point_to_simplex@phys_convex_penetration_util@@QAAXABUVertex@phys_winged_edge@@@Z
// @address 0x6a9620 ?convert_result_to_feature@phys_convex_penetration_util@@QAAXAAVphys_winged_edge@@PAUgeometry_triangle@2@AAUphys_simplex_out@@@Z
// @address 0x6a9a30 ?expand_simplex_1_to_2@phys_convex_penetration_util@@IAAXXZ
// @address 0x6a9da8 ?expand_simplex_3_to_4@phys_convex_penetration_util@@IAA?AW4phys_result@@ABVphys_vec4@@@Z
// @address 0x6aa590 ?phys_compute_penetration_depth@@YA?AW4phys_simplex_status@@PBVphys_convex_shape@@0ABVphys_transform@@MPAVphys_hull_vertex@@2PAVphys_vec4@@PAHAAUphys_simplex_out@@@Z
// @address 0x6aab10 ?init@phys_winged_edge@@QAAXPBVphys_hull_vertex@@0PBVphys_vec4@@@Z
// @address 0x6aaba0 ?find_tri_on_nearest_plane@phys_winged_edge@@QAAPAUgeometry_triangle@1@XZ
#include "phys/phys_base_types.h"
#include "phys/phys_convex_dist_sampling.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_convex_penetration_util.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <math.h>

struct __vector4;

#include "phys/phys_vec4_util.h"
#include "phys/phys_simplex_out.h"
#include "phys/phys_simplex_status.h"

#include "phys/phys_winged_edge.h"
#include "phys/phys_memory.h"
#include "phys/phys_thread_memory.h"

class phys_convex_penetration_util
{
public:
    static void simplex_code_begin();
    static void simplex_code_end();
    static void random_vec(int seed, float min, float max, phys_vec4 &vector);
    void add_point_to_simplex(const phys_winged_edge::Vertex &vertex);
    void get_simplex_point(const phys_vec4 &direction, phys_winged_edge::Vertex &vertex);
    void find_best_projection(const phys_vec4 &direction, const phys_vec4 &point, phys_winged_edge::Vertex &vertex, float &distance);
    phys_simplex_status compute_penetration_depth(phys_simplex_out &output);
    __forceinline void init(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
        const phys_transform &transform, float tolerance,
        phys_hull_vertex *vertices_a, phys_hull_vertex *vertices_b, phys_vec4 *simplex_verts, int *simplex_size)
    {
        m_body_transform = transform;
        m_shape_a = penetration_shape_a;
        m_shape_b = penetration_shape_b;
        m_box_tolerance.set_all3(tolerance);
        m_tolerance_sq = tolerance * tolerance;
        m_vertices_a = vertices_a;
        m_vertices_b = vertices_b;
        m_flag = 0;
        m_simplex_verts = simplex_verts;
        m_simplex_size = simplex_size;
    }
protected:
    phys_bool expand_simplex_0_to_1(phys_simplex_out &output);
    void expand_simplex_1_to_2();
    void expand_simplex_2_to_3();
    phys_result expand_simplex_3_to_4(const phys_vec4 &direction);
public:
    phys_result build_tetrahedron(phys_simplex_out &output, phys_simplex_status &status);
protected:
    phys_transform m_body_transform;
    phys_vec4 m_box_tolerance;
    float m_tolerance_sq;
    const phys_convex_shape *m_shape_a;
    const phys_convex_shape *m_shape_b;
    int m_flag;
    phys_hull_vertex *m_vertices_a;
    phys_hull_vertex *m_vertices_b;
    phys_vec4 *m_simplex_verts;
    int *m_simplex_size;
public:
  void convert_result_to_feature(class phys_winged_edge &p0, struct phys_winged_edge::geometry_triangle *p1, struct phys_simplex_out &p2);
    phys_result try_expand_polytope(phys_winged_edge &edge, phys_winged_edge::geometry_triangle *triangle,
        phys_winged_edge::Vertex *vertex, phys_simplex_status &status);
};

struct phys_multi_penetration_work
{
    phys_convex_penetration_util m_callback_util;
    phys_hull_vertex m_vertices_a[4];
    phys_hull_vertex m_vertices_b[4];
    phys_vec4 m_simplex_verts[4];
    int m_simplex_size;
    const phys_convex_shape *m_shape_b;
    phys_contact_point *m_contact_point;
    phys_winged_edge m_winged_edge;
    phys_winged_edge::geometry_triangle *m_edge_triangle;
};

void phys_convex_penetration_util::simplex_code_begin()
{
}

void phys_convex_penetration_util::simplex_code_end()
{
}

// Generate a repeatable unit-interval sample from the seed.
// @address 0x6a92f0 ?rand_unit@@YAMH@Z
float rand_unit(int seed)
{
    int value = (0x3039 - seed * 0x3e39b193) & 0x7fffffff;
    return (float)value * 4.656613e-10f;
}

// @address 0x6a9390 ?random_vec@phys_convex_penetration_util@@SAXHMMAAVphys_vec4@@@Z
void phys_convex_penetration_util::random_vec(int seed, float min, float max, phys_vec4 &vector)
{
    float range = max - min;
    float t0 = rand_unit(seed);
    vector(0) = (t0 + min) * range;
    float t1 = rand_unit(seed);
    vector(1) = (t1 + min) * range;
    float t2 = rand_unit(seed);
    vector(2) = (t2 + min) * range;
}

// @address 0x6a93f0 ?get_simplex_point@phys_convex_penetration_util@@QAAXABVphys_vec4@@AAUVertex@phys_winged_edge@@@Z
void phys_convex_penetration_util::get_simplex_point(const phys_vec4 &direction, phys_winged_edge::Vertex &vertex)
{
    m_shape_a->get_support_vertex_impl(direction, vertex.m_vertex_a);
    phys_vec4 neg_direction;
    neg_direction.set_neg4(direction);
    const __vector4 &neg_quad = neg_direction.m_simd_quad;
    const __vector4 &rot_column0 = m_body_transform.m_rotation.get_column(0).m_simd_quad;
    const __vector4 &rot_column1 = m_body_transform.m_rotation.get_column(1).m_simd_quad;
    const __vector4 &rot_column2 = m_body_transform.m_rotation.get_column(2).m_simd_quad;
    __vector4 x = __vmsum3fp(rot_column0, neg_quad);
    __vector4 y = __vmsum3fp(rot_column1, neg_quad);
    __vector4 z = __vmsum3fp(rot_column2, neg_quad);
    __vector4 zero = phys_simd_vec_0000;
    __vector4 yw = __vmrghw(y, zero);
    __vector4 xz = __vmrghw(x, z);
    phys_vec4 support_dir;
    support_dir.m_simd_quad = __vmrghw(xz, yw);
    m_shape_b->get_support_vertex_impl(support_dir, vertex.m_vertex_b);
    __vector4 sx = __vspltw(vertex.m_vertex_b.m_simd_quad, 0);
    __vector4 sy = __vspltw(vertex.m_vertex_b.m_simd_quad, 1);
    __vector4 sz = __vspltw(vertex.m_vertex_b.m_simd_quad, 2);
    __vector4 r0 = __vmulfp(m_body_transform.m_rotation.get_column(0).m_simd_quad, sx);
    __vector4 r1 = __vmaddfp(m_body_transform.m_rotation.get_column(1).m_simd_quad, sy, r0);
    __vector4 r2 = __vmaddfp(m_body_transform.m_rotation.get_column(2).m_simd_quad, sz, r1);
    phys_vec4 point;
    point.m_simd_quad = __vaddfp(r2, m_body_transform.m_translation.m_simd_quad);
    vertex.m_edge_vertex.set_sub4(vertex.m_vertex_a, point);
}

// @address 0x6a97b8 ?find_best_projection@phys_convex_penetration_util@@QAAXABVphys_vec4@@0AAUVertex@phys_winged_edge@@AAM@Z
void phys_convex_penetration_util::find_best_projection(const phys_vec4 &direction, const phys_vec4 &simplex_a,
    phys_winged_edge::Vertex &vertex, float &distance)
{
    phys_vec4 opposite_dir;
    opposite_dir.set_neg4(direction);
    phys_winged_edge::Vertex projected_vertex;
    get_simplex_point(direction, projected_vertex);
    get_simplex_point(opposite_dir, vertex);
    phys_vec4 projected_delta;
    projected_delta.set_sub4(projected_vertex.m_edge_vertex, simplex_a);
    phys_vec4 vertex_delta;
    vertex_delta.set_sub4(vertex.m_edge_vertex, simplex_a);
    float pen_projected_dist = projected_delta.dot3(direction);
    float vertex_dist = vertex_delta.dot3(opposite_dir);
    if (pen_projected_dist >= vertex_dist) {
        vertex.m_edge_vertex = projected_vertex.m_edge_vertex;
        vertex.m_vertex_a = projected_vertex.m_vertex_a;
        vertex.m_vertex_b = projected_vertex.m_vertex_b;
        vertex.m_hull_edge = projected_vertex.m_hull_edge;
        distance = pen_projected_dist;
        return;
    }
    distance = vertex_dist;
}

void phys_convex_penetration_util::add_point_to_simplex(const phys_winged_edge::Vertex &vertex)
{
    m_simplex_verts[*m_simplex_size] = vertex.m_edge_vertex;
    m_vertices_a[*m_simplex_size] = vertex.m_vertex_a;
    m_vertices_b[*m_simplex_size] = vertex.m_vertex_b;
    ++*m_simplex_size;
}

void phys_convex_penetration_util::expand_simplex_1_to_2()
{
    const phys_vec4 &direction = *m_simplex_verts;
    phys_winged_edge::Vertex forward_vertex;
    get_simplex_point(direction, forward_vertex);
    phys_vec4 forward_delta;
    forward_delta.set_sub4(forward_vertex.m_edge_vertex, *m_simplex_verts);
    float forward_dist_sq = forward_delta.length_sq_3d();
    phys_vec4 opposite_dir;
    opposite_dir.set_neg4(direction);
    phys_winged_edge::Vertex backward_vertex;
    get_simplex_point(opposite_dir, backward_vertex);
    phys_vec4 backward_delta;
    backward_delta.set_sub4(backward_vertex.m_edge_vertex, *m_simplex_verts);
    float backward_dist_sq = backward_delta.length_sq_3d();
    if (forward_dist_sq >= backward_dist_sq) {
        add_point_to_simplex(forward_vertex);
    } else {
        add_point_to_simplex(backward_vertex);
    }
}

phys_result phys_convex_penetration_util::expand_simplex_3_to_4(const phys_vec4 &direction)
{
    phys_winged_edge::Vertex vertex;
    float distance;
    find_best_projection(direction, *m_simplex_verts, vertex, distance);
    if (distance < m_box_tolerance(0)) {
        return PHYS_FAILURE;
    } else {
        add_point_to_simplex(vertex);
        return PHYS_SUCCESS;
    }
}

phys_simplex_status phys_compute_penetration_depth(const phys_convex_shape *penetration_shape_a,
    const phys_convex_shape *penetration_shape_b, const phys_transform &transform,
    float tolerance, phys_hull_vertex *vertices_a, phys_hull_vertex *vertices_b, phys_vec4 *simplex_verts, int *simplex_size, phys_simplex_out &output)
{
    phys_convex_penetration_util penetration_util;
    penetration_util.init(penetration_shape_a, penetration_shape_b, transform, tolerance, vertices_a, vertices_b, simplex_verts, simplex_size);
    phys_simplex_status result = penetration_util.compute_penetration_depth(output);
    if (result == PHYS_SIMPLEX_PD_UNSOLVABLE) {
        phys_convex_dist_by_sampling(penetration_shape_a, penetration_shape_b, transform, vertices_a, vertices_b, output);
        *simplex_size = 1;
        result = PHYS_SIMPLEX_PD_HEURISTIC_SAMPLING;
    }
    return result;
}

// @address 0x6a98a0 ?expand_simplex_0_to_1@phys_convex_penetration_util@@IAA?AVphys_bool@@AAUphys_simplex_out@@@Z
phys_bool phys_convex_penetration_util::expand_simplex_0_to_1(phys_simplex_out &output)
{
    float offset_dist_sq = m_body_transform.get_translation().length_sq_3d();
    phys_vec4 direction;
    direction.set_neg4(m_body_transform.get_translation());
    if (!(offset_dist_sq > m_tolerance_sq))
        direction = phys_transform::get_identity().get_column(1);
    phys_winged_edge::Vertex vertex;
    get_simplex_point(direction, vertex);
    add_point_to_simplex(vertex);
    if (!(m_simplex_verts->length_sq_3d() > m_tolerance_sq)) {
        output.m_distance = m_simplex_verts->length_3d();
        output.m_normal_in_a = direction;
        output.m_point_on_a = vertex.m_vertex_a;
        return phys_bool(true);
    }
    return phys_bool(false);
}

// @address 0x6a94e0 ?try_expand_polytope@phys_convex_penetration_util@@QAA?AW4phys_result@@AAVphys_winged_edge@@PAUgeometry_triangle@3@PAUVertex@3@AAW4phys_simplex_status@@@Z
phys_result phys_convex_penetration_util::try_expand_polytope(phys_winged_edge &edge, phys_winged_edge::geometry_triangle *triangle,
    phys_winged_edge::Vertex *vertex, phys_simplex_status &status)
{
    phys_rotation m;
    m.get_column(0).set_sub4(vertex->m_edge_vertex, triangle->m_hull_edges[0].m_edge_vertex->m_edge_vertex);
    m.get_column(1).set_sub4(vertex->m_edge_vertex, triangle->m_hull_edges[1].m_edge_vertex->m_edge_vertex);
    m.get_column(2).set_sub4(vertex->m_edge_vertex, triangle->m_hull_edges[2].m_edge_vertex->m_edge_vertex);

    phys_vec4 bary_coords;
    bary_coords.vec_set_rotated_inverse_dir(m, triangle->m_hull_plane);
    if (bary_coords.compare_less4(m_box_tolerance).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
    {
        status = PHYS_SIMPLEX_OK;
        return PHYS_FAILURE;
    }

    int hull_edge_count = 0;
    phys_winged_edge::hull_edge *edges;
    if (edge.find_visible_boundary_edges(triangle, vertex->m_edge_vertex, hull_edge_count, edges) != phys_winged_edge::WINGED_EDGE_OK)
    {
        status = PHYS_SIMPLEX_PD_UNSOLVABLE;
        return PHYS_FAILURE;
    }
    if (edge.free_triangle_count() < hull_edge_count)
    {
        status = PHYS_SIMPLEX_PD_OUT_OF_MEMORY;
        return PHYS_FAILURE;
    }
    if (edge.patch_boundary_to_view(hull_edge_count, edges, vertex) != phys_winged_edge::WINGED_EDGE_OK)
    {
        status = PHYS_SIMPLEX_PD_UNSOLVABLE;
        return PHYS_FAILURE;
    }
    return PHYS_SUCCESS;
}

// @address 0x6a9620?convertResultToFeature@hkConvexPenetrationUtil@@QAAXAAVhkWingedEdge@@PAUTriangle@2@AAUhkGskOut@@@Z
void phys_convex_penetration_util::convert_result_to_feature(phys_winged_edge &edge, phys_winged_edge::geometry_triangle *triangle, phys_simplex_out &output)
{
    phys_padded<float> bary_weights[3];
    int count = 0;
    do {
        m_simplex_verts[0] = triangle->m_hull_edges[0].m_edge_vertex->m_edge_vertex;
        m_simplex_verts[1] = triangle->m_hull_edges[1].m_edge_vertex->m_edge_vertex;
        m_simplex_verts[2] = triangle->m_hull_edges[2].m_edge_vertex->m_edge_vertex;
        phys_collide_triangle_util::compute_barycentric_coords(
            phys_vec4::get_zero(), m_simplex_verts[0], m_simplex_verts[1], m_simplex_verts[2], bary_weights);
        if (bary_weights[0] >= 0.0f && bary_weights[1] >= 0.0f && bary_weights[2] >= 0.0f)
            break;
        triangle = edge.find_triangle_nearest_origin(triangle);
        count++;
    } while (count < 10);

    for (int i = 0; i < 3; i++) {
        const phys_winged_edge::Vertex *vertex = triangle->m_hull_edges[i].m_edge_vertex;
        m_vertices_a[i] = vertex->m_vertex_a;
        m_vertices_b[i] = vertex->m_vertex_b;
    }
    *m_simplex_size = 3;
    __vector4 b0 = __vspltw(__lvlx((void *)&bary_weights[0], 0), 0);
    __vector4 b1 = __vspltw(__lvlx((void *)&bary_weights[1], 0), 0);
    __vector4 b2 = __vspltw(__lvlx((void *)&bary_weights[2], 0), 0);
    phys_vec4 p;
    p.m_simd_quad = __vmulfp(m_vertices_a[0].m_simd_quad, b0);
    p.m_simd_quad = __vmaddfp(m_vertices_a[1].m_simd_quad, b1, p.m_simd_quad);
    p.m_simd_quad = __vmaddfp(m_vertices_a[2].m_simd_quad, b2, p.m_simd_quad);
    output.m_point_on_a = p;
    output.m_distance = -triangle->m_plane_distance;
    output.m_normal_in_a.set_neg4(triangle->m_hull_plane);
}

// @address 0x6a9b70 ?expand_simplex_2_to_3@phys_convex_penetration_util@@IAAXXZ
void phys_convex_penetration_util::expand_simplex_2_to_3()
{
    phys_vec4 edge;
    edge.set_sub4(m_simplex_verts[0], m_simplex_verts[1]);
    phys_vec4 perpendicular;
    phys_vec4_util::compute_perpendicular_vec(edge, perpendicular);
    phys_vec4 cross;
    cross.set_cross(edge, perpendicular);
    perpendicular.normalize_xyz();
    cross.normalize_xyz();
    phys_winged_edge::Vertex capsule_vertex_a;
    float dist_a;
    find_best_projection(perpendicular, m_simplex_verts[0], capsule_vertex_a, dist_a);
    phys_winged_edge::Vertex capsule_vertex_b;
    float dist_b;
    find_best_projection(cross, m_simplex_verts[0], capsule_vertex_b, dist_b);
    if (dist_a > dist_b)
        add_point_to_simplex(capsule_vertex_a);
    else
        add_point_to_simplex(capsule_vertex_b);
}

// @address 0x6a9e70 ?build_tetrahedron@phys_convex_penetration_util@@QAA?AW4phys_result@@AAUphys_simplex_out@@AAW4phys_simplex_status@@@Z
phys_result phys_convex_penetration_util::build_tetrahedron(phys_simplex_out &output, phys_simplex_status &status)
{
    int last_case = -1;
    phys_vec4 normal;
    switch (*m_simplex_size)
    {
    case 0:
    simplex_case0:
        if (expand_simplex_0_to_1(output))
        {
            status = PHYS_SIMPLEX_OK;
            return PHYS_FAILURE;
        }
        last_case = 0;
    case 1:
    simplex_case1:
        if (m_simplex_verts[0].length_sq_3d() <= m_tolerance_sq)
        {
            if (last_case < 0)
            {
                (*m_simplex_size)--;
                goto simplex_case0;
            }
            status = PHYS_SIMPLEX_PD_UNSOLVABLE;
            return PHYS_FAILURE;
        }
        last_case = 1;
        expand_simplex_1_to_2();
    case 2:
    simplex_case2:
        {
            phys_vec4 d;
            d.set_sub4(m_simplex_verts[0], m_simplex_verts[1]);
            if (d.length_sq_3d() <= m_tolerance_sq)
            {
                if (last_case < 1)
                {
                    (*m_simplex_size)--;
                    goto simplex_case1;
                }
                else
                {
                    status = PHYS_SIMPLEX_PD_UNSOLVABLE;
                    return PHYS_FAILURE;
                }
            }
        }
        last_case = 2;
        expand_simplex_2_to_3();
    case 3:
    simplex_case3:
        {
            phys_vec4 a;
            a.set_sub4(m_simplex_verts[0], m_simplex_verts[1]);
            phys_vec4 b;
            b.set_sub4(m_simplex_verts[1], m_simplex_verts[2]);
            normal.set_cross(a, b);
            if (normal.length_sq_3d() == 0.0f)
            {
                if (last_case < 2)
                {
                    (*m_simplex_size)--;
                    goto simplex_case2;
                }
                else
                {
                    output.m_distance = 0.0f;
                    phys_vec4 d;
                    d.set_sub4(m_simplex_verts[0], m_simplex_verts[1]);
                    phys_vec4_util::compute_perpendicular_vec(d, output.m_normal_in_a);
                    output.m_normal_in_a.normalize_xyz();
                    phys_vec4 abs_dir_a;
                    abs_dir_a.set_abs4(m_simplex_verts[0]);
                    phys_vec4 abs_dir_b;
                    abs_dir_b.set_abs4(m_simplex_verts[1]);
                    float l0 = abs_dir_a.horizontal_add3();
                    float l1 = abs_dir_a.horizontal_add3();
                    float t = l0 / (l0 + l1 + 1.192092896e-07f);
                    output.m_point_on_a.set_lerp4(m_vertices_a[0], m_vertices_a[1], phys_simd_scalar(t));
                    status = PHYS_SIMPLEX_OK;
                    return PHYS_FAILURE;
                }
            }
        }
        last_case = 3;
        normal.normalize_xyz();
        if (expand_simplex_3_to_4(normal) == PHYS_FAILURE)
        {
            output.m_distance = 0.0f;
            output.m_normal_in_a = normal;
            phys_padded<float> lambda_weights[3];
            phys_collide_triangle_util::compute_barycentric_coords(
                phys_vec4::get_zero(), m_simplex_verts[0], m_simplex_verts[1], m_simplex_verts[2], lambda_weights);
            phys_vec4 p;
            p.set_mul4(phys_simd_scalar(lambda_weights[0].val()), m_vertices_a[0]);
            p.add_scaled4(phys_simd_scalar(lambda_weights[1].val()), m_vertices_a[1]);
            p.add_scaled4(phys_simd_scalar(lambda_weights[2].val()), m_vertices_a[2]);
            output.m_point_on_a = p;
            status = PHYS_SIMPLEX_OK;
            return PHYS_FAILURE;
        }
    case 4:
    simplex_case4:
        {
            phys_vec4 a;
            a.set_sub4(m_simplex_verts[0], m_simplex_verts[1]);
            phys_vec4 b;
            b.set_sub4(m_simplex_verts[1], m_simplex_verts[2]);
            normal.set_cross(a, b);
            phys_vec4 d;
            d.set_sub4(m_simplex_verts[3], m_simplex_verts[0]);
            float normal_len_sq = normal.length_sq_3d();
            float dist = d.dot3(normal);
            if (dist * dist <= m_tolerance_sq * normal_len_sq)
            {
                if (last_case < 3)
                {
                    (*m_simplex_size)--;
                    goto simplex_case3;
                }
                else
                {
                    status = PHYS_SIMPLEX_PD_UNSOLVABLE;
                    return PHYS_FAILURE;
                }
            }
        }
        last_case = 4;
        break;
    default:
        status = PHYS_SIMPLEX_PD_UNSOLVABLE;
        return PHYS_FAILURE;
    }
    status = PHYS_SIMPLEX_OK;
    return PHYS_SUCCESS;
}

// @address 0x6aa678 ?phys_compute_multi_penetration_depth@@YA?AW4phys_result@@ABVphys_transform@@PBVphys_convex_shape@@PAPBV3@H0PAPAVphys_contact_point@@@Z
phys_result phys_compute_multi_penetration_depth(const phys_transform &xform_a, const phys_convex_shape *penetration_shape_a,
    const phys_convex_shape **shape_b_list,
    int shape_b_count, const phys_transform &a_from_b, phys_contact_point **capsule_points_out)
{
    phys_local_buffer<phys_multi_penetration_work> work(shape_b_count);
    int j = 0;
    for (int i = 0; i < shape_b_count; i++)
    {
        phys_multi_penetration_work &w = work[j];
        w.m_shape_b = shape_b_list[i];
        w.m_contact_point = capsule_points_out[i];
        w.m_simplex_size = 0;
        w.m_callback_util.init(penetration_shape_a, shape_b_list[i], a_from_b, 0.0001f, w.m_vertices_a, w.m_vertices_b, w.m_simplex_verts, &w.m_simplex_size);
        phys_simplex_out output;
        phys_simplex_status status;
        if (w.m_callback_util.build_tetrahedron(output, status) == PHYS_FAILURE)
        {
            shape_b_count--;
            continue;
        }
        else
        {
            j++;
        }
        w.m_winged_edge.init(w.m_vertices_a, w.m_vertices_b, w.m_simplex_verts);
    }
    if (shape_b_count > 2)
    {
        shape_b_count = 2;
    }
    if (shape_b_count < 2)
    {
        return PHYS_FAILURE;
    }

    phys_simplex_status status[2];
    status[0] = PHYS_SIMPLEX_OK;
    status[1] = PHYS_SIMPLEX_OK;
    phys_vec4 normal;
    bool expanded;
    do
    {
        phys_winged_edge::find_two_closest_multi_tris(work[0].m_winged_edge, work[1].m_winged_edge, normal, work[0].m_edge_triangle, work[1].m_edge_triangle);
        expanded = false;
        for (int i = 0; i < shape_b_count; i++)
        {
            phys_multi_penetration_work &w = work[i];
            if (status[i] == PHYS_SIMPLEX_PD_OUT_OF_MEMORY || w.m_winged_edge.has_free_vertex())
            {
                continue;
            }
            phys_winged_edge::Vertex *vertex = w.m_winged_edge.allocate_vertex();
            w.m_callback_util.get_simplex_point(w.m_edge_triangle->m_hull_plane, *vertex);
            if (w.m_callback_util.try_expand_polytope(w.m_winged_edge, w.m_edge_triangle, vertex, status[i]) == PHYS_FAILURE)
            {
                if (status[i] == PHYS_SIMPLEX_PD_UNSOLVABLE)
                {
                    return PHYS_FAILURE;
                }
            }
            else
            {
                expanded = true;
            }
        }
    } while (expanded);

    phys_vec4 neg_normal;
    neg_normal.set_neg4(normal);
    float distance = -normal(3);
    float radius_a = penetration_shape_a->get_convex_radius();
    for (int i = 0; i < shape_b_count; i++)
    {
        phys_multi_penetration_work &w = work[i];
        phys_simplex_out output;
        w.m_callback_util.convert_result_to_feature(w.m_winged_edge, w.m_edge_triangle, output);
        w.m_contact_point->get_separating_normal().set_rotated_dir(xform_a.get_rotation(), neg_normal);
        phys_vec4 position;
        position.set_transformed_pos(xform_a, output.m_point_on_a);
        float radius_b = w.m_shape_b->get_convex_radius();
        w.m_contact_point->contact_set_position(position);
        w.m_contact_point->contact_set_distance(distance - radius_a - radius_b);
    }
    return PHYS_SUCCESS;
}

// @address 0x6aa358 ?compute_penetration_depth@phys_convex_penetration_util@@QAA?AW4phys_simplex_status@@AAUphys_simplex_out@@@Z
phys_simplex_status phys_convex_penetration_util::compute_penetration_depth(phys_simplex_out &output)
{
    while (true)
    {
        phys_simplex_status status = PHYS_SIMPLEX_OK;
        if (build_tetrahedron(output, status) == PHYS_FAILURE)
        {
            if (status != PHYS_SIMPLEX_PD_UNSOLVABLE)
            {
                return status;
            }
        }
        else
        {
            phys_winged_edge edge;
            edge.init(m_vertices_a, m_vertices_b, m_simplex_verts);
            phys_winged_edge::geometry_triangle *triangle;
            while (true)
            {
                triangle = edge.find_tri_on_nearest_plane();
                phys_winged_edge::Vertex *vertex = edge.allocate_vertex();
                get_simplex_point(triangle->m_hull_plane, *vertex);
                if (try_expand_polytope(edge, triangle, vertex, status) == PHYS_FAILURE)
                {
                    break;
                }
                if (!edge.has_free_vertex())
                {
                    status = PHYS_SIMPLEX_PD_OUT_OF_MEMORY;
                    break;
                }
            }
            if (status != PHYS_SIMPLEX_PD_UNSOLVABLE)
            {
                convert_result_to_feature(edge, triangle, output);
                return status;
            }
        }
        if (status == PHYS_SIMPLEX_PD_UNSOLVABLE)
        {
            m_flag++;
            if (m_flag == 1)
            {
                *m_simplex_size = 1;
            }
            else if (m_flag < 20)
            {
                *m_simplex_size = 0;
                phys_vec4 offset;
                random_vec(m_flag, -0.0001f, 0.0001f, offset);
                offset.add4(m_body_transform.get_translation());
                m_body_transform.set_translation(offset);
            }
            else
            {
                return status;
            }
        }
    }
}

void phys_winged_edge::init(const phys_hull_vertex *vertices_a, const phys_hull_vertex *vertices_b, const phys_vec4 *positions)
{
    m_hull_vertex_count = 4;
    Vertex *vertex = m_vertices;
    for (int i = 0; i < 4; i++) {
        vertex->m_vertex_a = *vertices_a++;
        vertex->m_vertex_b = *vertices_b++;
        vertex->m_edge_vertex = *positions++;
        vertex->m_hull_edge = 0;
        ++vertex;
    }
    add_tetrahedron();
    m_free_triangle_count = 0;
}

inline phys_winged_edge::geometry_triangle* phys_winged_edge::find_tri_on_nearest_plane()
{
    int remaining = m_triangle_count - 2;
    geometry_triangle* triangle = m_triangle_list;
    geometry_triangle* nearest_tri = triangle;
    float nearest_dist = triangle->m_plane_distance;
    for (int i = remaining; i >= 0; i--) {
        ++triangle;
        float distance = triangle->m_plane_distance;
        if (distance < nearest_dist) {
            nearest_dist = distance;
            nearest_tri = triangle;
        }
    }
    return nearest_tri;
}

