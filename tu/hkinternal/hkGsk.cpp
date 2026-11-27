// hkinternal/hkGsk.obj: from the records 0x683cb8 0x683cc0 0x683cc8 0x683cd0 0x683e10 0x684050 0x684080 0x684148 0x684168 0x6842f8 0x684770 0x6849b0 0x684b08 0x684f90 0x6850e0 0x685158 0x685400 0x6860c0 0x6864d0 0x686610 0x6874d8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x683cb8 ?print@phys_simplex@@QAAXXZ
// @address 0x683cc0 ?simplex_code_begin@phys_simplex@@SAXXZ
// @address 0x683cc8 ?simplex_code_end@phys_simplex@@SAXXZ
// @address 0x6850e0 ?feature_to_closest_distance@phys_simplex@@QAAXABVphys_vec4@@AAUphys_simplex_out@@@Z
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"
#include "phys/phys_simplex.h"
#include "phys/phys_convex_penetration_util.h"
#include "phys/phys_simplex_types.h"
// @address 0x684050 ?phys_simplex_support_dist_sq@@YAMABVphys_vec4@@00@Z
// @address 0x684080 ?phys_simplex_recalc_point_tri@@YA?AVphys_simd_scalar@@ABVphys_vec4@@PAV2@0AAV2@@Z
// @address 0x684148 ?phys_simplex_recalc_point_edge@@YA?AVphys_simd_scalar@@PAVphys_vec4@@0ABV2@AAV2@@Z
// @address 0x684f90 ?exit_and_export_cache@phys_simplex@@QBAXAAVphys_simplex_cache@@@Z
#include <vectorintrinsics.h>
#include <math.h>


#include "phys/phys_scalar_types.h"

#include "phys/phys_simplex_status.h"
#include "phys/phys_collide.h"
#include "phys/phys_math.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_vec4_util.h"
// Writes the vertex ids of one shape's simplex (N of them) to the cache, largest id first.
#define sort3_vertices(N, vertex_list, sorted_dst0, sorted_dst1, sorted_dst2) \
    { \
        unsigned short a = (unsigned short)(vertex_list)[0].get_packed_id(); \
        if ((N) >= 2) { \
            int k = 0; \
            unsigned short b = (unsigned short)(vertex_list)[1].get_packed_id(); \
            unsigned short t; \
            if (b > a) { t = b; b = a; a = t; k = 1; } \
            if ((N) > 2) { \
                unsigned short c = (unsigned short)(vertex_list)[2].get_packed_id(); \
                if (c > a) { t = c; c = a; a = t; k++; } \
                if (k == 1) { t = c; c = b; b = t; } \
                sorted_dst2 = c; \
            } \
            sorted_dst1 = b; \
        } \
        sorted_dst0 = a; \
    }

void phys_simplex::print()
{
}

void phys_simplex::simplex_code_begin()
{
}

void phys_simplex::simplex_code_end()
{
}

__forceinline float phys_simplex_support_dist_sq(const phys_vec4 &normal, const phys_vec4 &a, const phys_vec4 &b)
{
    phys_vec4 d;
    d.set_sub4(a, b);
    float r;
    __stvewx(__vmsum3fp(d.m_simd_quad, normal.m_simd_quad), &r, 0);
    return fabsf(r) * r;
}

__forceinline phys_simd_scalar phys_simplex_recalc_point_tri(const phys_vec4 &a, phys_vec4 *b, const phys_vec4 &c, phys_vec4 &d)
{
    float dot;
    phys_vec4 e0;
    e0.set_sub4(b[2], b[1]);
    phys_vec4 e1;
    e1.set_sub4(b[0], b[2]);
    d.set_cross(e0, e1);
    phys_simd_scalar inv_len = d.inv_length_3d();
    __stvewx(__vmsum3fp(d.m_simd_quad, c.m_simd_quad), &dot, 0);
    if (dot < 0.0f) {
        d.set_neg4(d);
    }
    d.mul4(inv_len);
    phys_vec4 t;
    t.set_sub4(a, b[0]);
    return t.dot3(d);
}

phys_simd_scalar phys_simplex_recalc_point_edge(phys_vec4 *a, phys_vec4 *b, const phys_vec4 &normal, phys_vec4 &normal_out)
{
    phys_vec4 d;
    d.set_sub4(*a, *b);
    phys_vec4 n;
    n = normal;
    normal_out = n;
    return phys_simd_scalar(__vmsum3fp(n.m_simd_quad, d.m_simd_quad));
}

void phys_simplex::exit_and_export_cache(phys_simplex_cache &cache) const
{
    cache.m_dim_a = (unsigned char)m_dim_a;
    cache.m_dim_b = (unsigned char)m_dim_b;
    unsigned short *vertex_ids = cache.m_vertices;
    sort3_vertices(m_dim_a, m_vertices_a, vertex_ids[0], vertex_ids[1], vertex_ids[2])
    sort3_vertices(m_dim_b, m_vertices_b_in_b, vertex_ids[m_dim_a], vertex_ids[m_dim_a + 1], vertex_ids[m_dim_a + 2])
}

void phys_simplex::feature_to_closest_distance(const phys_vec4 &normal, phys_simplex_out &output)
{
    output.m_distance = normal(3);
    output.m_normal_in_a = normal;
    if (m_dim_a == 1) {
        output.m_point_on_a = m_vertices_a[0];
    } else if (m_dim_b == 1) {
        output.m_point_on_a.set_add_mul4(m_vertices_b_in_a[0], output.m_normal_in_a, normal.get_simd_at(3));
    } else {
        output.m_point_on_a = m_closest_line_result.m_closest_point_a;
    }
}


// @address 0x6864d0 ?trace_ray@phys_simplex@@QAA?AVphys_bool@@PBVphys_convex_shape@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_simplex::trace_ray(const phys_convex_shape *shape, const phys_ray_input &input, phys_ray_output &output)
{
    phys_rotation identity;
    identity.set_to_identity();
    phys_ray_output local_ray_output = output;
    phys_bool hit = sweep_cast(shape, 0, identity, input, local_ray_output);
    if (hit && local_ray_output.m_hit_frac != 0.0f) {
        // This build's hkShapeRayCastOutput assignment (March 0x8354a798) copies the normal, hit fraction, extra info
        // and the shape keys, not the two padding words the shared header declares after the extra info.
        output.m_normal = local_ray_output.m_normal;
        output.m_hit_frac = local_ray_output.m_hit_frac;
        output.m_extra_info = local_ray_output.m_extra_info;
        for (int i = 0; i < phys_ray_output::HIERARCHY_DEPTH_LIMIT; i++) {
            output.m_shape_keys[i] = local_ray_output.m_shape_keys[i];
        }
        output.m_shape_key_idx = local_ray_output.m_shape_key_idx;
        return phys_bool(true);
    }
    return phys_bool(false);
}

// @address 0x683cd0 ?find_closest_triangle_backup@@YAHABVphys_vec4@@PBV1@AAV1@M@Z
int find_closest_triangle_backup(const phys_vec4 &position, const phys_vec4 *triangle, phys_vec4 &edge_dists, float)
{
    float f0, f3, f2, f1;

    for (int i = 0; i < 3; i++)
    {
        int next_edge_idx = phys_collide_triangle_util::vertex_to_edge_lut[i + 2];
        int prev_index = phys_collide_triangle_util::vertex_to_edge_lut[i];
        if (edge_dists(next_edge_idx) >= 0.0f && edge_dists(prev_index) >= 0.0f)
        {
            (void)&f2;
            __vector4 v0 = triangle[i].m_simd_quad;
            __vector4 d1 = __vsubfp(triangle[next_edge_idx].m_simd_quad, v0);
            __vector4 d0 = __vsubfp(position.m_simd_quad, v0);
            __vector4 d2 = __vsubfp(triangle[prev_index].m_simd_quad, v0);
            __vector4 d3 = __vsubfp(triangle[3].m_simd_quad, v0);
            __stvewx(__vmsum3fp(d0, d1), &f0, 0);
            __stvewx(__vmsum3fp(d0, d2), &f1, 0);
            __stvewx(__vmsum3fp(d3, d1), &f2, 0);
            __stvewx(__vmsum3fp(d3, d2), &f3, 0);
            if (f3 * f0 < f1 * f2)
                edge_dists(prev_index) = -1.0f;
            else
                edge_dists(next_edge_idx) = -1.0f;
        }
    }
    if (edge_dists(0) > 0.0f) return 0;
    if (edge_dists(1) > 0.0f) return 1;
    int result = 2;
    if (!(edge_dists(2) > 0.0f)) result = -1;
    return result;
}


static __forceinline float phys_simplex_to_float(const phys_simd_scalar &s)
{
    float value;
    __stvewx(s.m_simd_scalar, &value, 0);
    return value;
}

static __forceinline void phys_simplex_dot3_3vs3(const phys_vec4 &a0, const phys_vec4 &b0, const phys_vec4 &a1, const phys_vec4 &b1,
    const phys_vec4 &a2, const phys_vec4 &b2, phys_vec4 &dot_products_out)
{
    dot_products_out(0) = phys_simplex_to_float(a0.dot3(b0));
    dot_products_out(1) = phys_simplex_to_float(a1.dot3(b1));
    dot_products_out(2) = phys_simplex_to_float(a2.dot3(b2));
    dot_products_out(3) = 0;
}


template <typename T>
static __forceinline void phys_simplex_swap(T &x, T &y)
{
    T t = x;
    x = y;
    y = t;
}

// @address 0x683e10 ?check_triangle_boundaries@phys_simplex@@IAAHABVphys_vec4@@PAV2@W4support_kind@1@@Z
int phys_simplex::check_triangle_boundaries(const phys_vec4 &a, phys_vec4 *three_vertices, support_kind support)
{
    phys_vec4 e0; e0.set_sub4(three_vertices[2], three_vertices[1]);
    phys_vec4 e1; e1.set_sub4(three_vertices[0], three_vertices[2]);
    phys_vec4 e2; e2.set_sub4(three_vertices[1], three_vertices[0]);
    phys_vec4 normal; normal.set_cross(e0, e1);

    phys_vec4 d1; d1.set_sub4(a, three_vertices[1]);
    phys_vec4 d2; d2.set_sub4(a, three_vertices[2]);
    phys_vec4 d0; d0.set_sub4(a, three_vertices[0]);

    phys_vec4 c0; c0.set_cross(d1, e0);
    phys_vec4 c1; c1.set_cross(d2, e1);
    phys_vec4 c2; c2.set_cross(d0, e2);

    phys_vec4 dot_products;
    phys_simplex_dot3_3vs3(c0, normal, c1, normal, c2, normal, dot_products);
    m_check_triangle_dots = dot_products;
    int mask = dot_products.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);

    if (support)
    {
        m_support_vec = normal;
        if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
        {
            phys_vec4 diff; diff.set_sub4(m_vertices_a[0], m_vertices_b_in_a[0]);
            if (phys_simplex_to_float(normal.dot3(diff)) < 0.0f)
            {
                m_support_vec.set_neg4(normal);
                phys_algorithm::swap(three_vertices[0], three_vertices[1]);
                phys_algorithm::swap(three_vertices[4], three_vertices[5]);
                phys_simplex_swap(m_check_triangle_dots(0), m_check_triangle_dots(1));
                set_feature_change(1);
            }
        }
    }
    return mask;
}

// @address 0x6849b0 ?handle_point_edge@@YAXPIAVphys_vec4@@0AAV?$phys_padded@H@@AAV1@W4support_kind@phys_simplex@@@Z
__forceinline void handle_point_edge(
    // match: 0x6849b0: retains support_out[0] in vr11 across the simplex_dim store
    phys_vec4 *__restrict support_out, phys_vec4 *__restrict verts, phys_padded<int> &simplex_dim, phys_vec4 &sep_out, phys_simplex::support_kind support_type)
{
    phys_vec4 ap;
    ap.set_sub4(verts[0], support_out[0]);
    phys_vec4 edge;
    edge.set_sub4(verts[1], verts[0]);
    phys_vec4 bp;
    bp.set_sub4(verts[1], support_out[0]);
    edge.normalize_xyz();
    phys_simd_scalar dot_a = ap.dot3(edge);
    int sign_b = (phys_simplex_to_float(bp.dot3(edge)) < 0.0f) ? 8 : 0;
    int sign_a = (phys_simplex_to_float(dot_a) < 0.0f) ? 8 : 0;
    if (sign_b != sign_a) {
        phys_vec4 n;
        n.set_cross(bp, ap);
        if (support_type == phys_simplex::SIMPLEX_SUPPORT_POS) {
            sep_out.set_cross(edge, n);
        } else {
            sep_out.set_cross(n, edge);
        }
        return;
    }
    if (sign_b) {
        verts[0] = verts[1];
        verts[4] = verts[5];
    }
    simplex_dim = simplex_dim - 1;
    if (support_type == phys_simplex::SIMPLEX_SUPPORT_POS) {
        sep_out.set_sub4(support_out[0], verts[0]);
    } else {
        sep_out.set_sub4(verts[0], support_out[0]);
    }
}


// @address 0x684168 ?phys_simplex_recalc_edge_edge@@YAXPAVphys_vec4@@0ABV1@AAV1@2@Z
void phys_simplex_recalc_edge_edge(
    class phys_vec4 *edge_a, class phys_vec4 *edge_b, class phys_vec4 const &ref, class phys_vec4 &capsule_point_out, class phys_vec4 &normal_out)
{
    phys_vec4 dir_a;
    dir_a.set_sub4(edge_a[1], edge_a[0]);
    phys_vec4 dir_b;
    dir_b.set_sub4(edge_b[1], edge_b[0]);
    phys_collide_triangle_util::segment_segment_result result;
    phys_collide_triangle_util::closest_segment_pair(edge_a[0], dir_a, edge_b[0], dir_b, result);

    normal_out.set_cross(dir_a, dir_b);

    phys_vec4 sq;
    sq.m_simd_quad = __vmaddfp(dir_a.m_simd_quad, dir_a.m_simd_quad, __vmulfp(dir_b.m_simd_quad, dir_b.m_simd_quad));
    float simplex_sum_sq;
    float cross_len_sq;
    __stvewx(__vmsum3fp(sq.m_simd_quad, phys_simd_vec_1111), &simplex_sum_sq, 0);
    __stvewx(__vmsum3fp(normal_out.m_simd_quad, normal_out.m_simd_quad), &cross_len_sq, 0);
    if (!(cross_len_sq * 0.001f <= simplex_sum_sq * simplex_sum_sq)) {
        float d;
        __stvewx(__vmsum3fp(normal_out.m_simd_quad, ref.m_simd_quad), &d, 0);
        if (!(d >= 0.0f)) normal_out.set_neg4(normal_out);
        union Q { __vector4 q; float v[4]; };
        Q temp;
        temp.q = __vmsum3fp(normal_out.m_simd_quad, normal_out.m_simd_quad);
        Q scale;
        scale.v[0] = 1.0f / sqrtf(temp.v[0]);
        scale.q = __vspltw(scale.q, 0);
        normal_out.m_simd_quad = __vmulfp(normal_out.m_simd_quad, scale.q);
    } else {
        normal_out = ref;
    }
    float w;
    phys_vec4 diff;
    phys_vec4 a0;
    a0 = edge_a[0];
    phys_vec4 b0;
    b0 = edge_b[0];
    diff.set_sub4(a0, b0);
    __vector4 dot = __vmsum3fp(diff.m_simd_quad, normal_out.m_simd_quad);
    capsule_point_out = result.m_closest_point_a;
    __stvewx(dot, &w, 0);
    normal_out(3) = w;
}

static __forceinline void phys_simplex_edge_edge_i(
    class phys_vec4 *edge_a, class phys_vec4 *edge_b, class phys_vec4 const &ref, class phys_vec4 &capsule_point_out, class phys_vec4 &normal_out)
{
    phys_vec4 dir_a;
    dir_a.set_sub4(edge_a[1], edge_a[0]);
    phys_vec4 dir_b;
    dir_b.set_sub4(edge_b[1], edge_b[0]);
    phys_collide_triangle_util::segment_segment_result result;
    phys_collide_triangle_util::closest_segment_pair(edge_a[0], dir_a, edge_b[0], dir_b, result);

    normal_out.set_cross(dir_a, dir_b);

    phys_vec4 sq;
    sq.m_simd_quad = __vmaddfp(dir_a.m_simd_quad, dir_a.m_simd_quad, __vmulfp(dir_b.m_simd_quad, dir_b.m_simd_quad));
    float simplex_sum_sq;
    float cross_len_sq;
    __stvewx(__vmsum3fp(sq.m_simd_quad, phys_simd_vec_1111), &simplex_sum_sq, 0);
    __stvewx(__vmsum3fp(normal_out.m_simd_quad, normal_out.m_simd_quad), &cross_len_sq, 0);
    if (!(cross_len_sq * 0.001f <= simplex_sum_sq * simplex_sum_sq)) {
        phys_vec4 &n = *reinterpret_cast<phys_vec4 *>(&normal_out(0));
        float d;
        __stvewx(__vmsum3fp(n.m_simd_quad, ref.m_simd_quad), &d, 0);
        if (!(d >= 0.0f)) n.set_neg4(n);
        union Q { __vector4 q; float v[4]; };
        Q temp;
        temp.q = __vmsum3fp(normal_out.m_simd_quad, normal_out.m_simd_quad);
        Q scale;
        scale.v[0] = 1.0f / sqrtf(temp.v[0]);
        scale.q = __vspltw(scale.q, 0);
        normal_out.m_simd_quad = __vmulfp(normal_out.m_simd_quad, scale.q);
    } else {
        normal_out = ref;
    }
    float w;
    phys_vec4 diff;
    phys_vec4 a0;
    a0 = edge_a[0];
    phys_vec4 b0;
    b0 = edge_b[0];
    diff.set_sub4(a0, b0);
    __vector4 dot = __vmsum3fp(diff.m_simd_quad, normal_out.m_simd_quad);
    capsule_point_out = result.m_closest_point_a;
    __stvewx(dot, &w, 0);
    normal_out(3) = w;
}


// @address 0x6842f8 phys_simplex_recalc_contact_internal
extern "C" void phys_simplex_recalc_contact_internal(
    phys_vec4 *vertices_a, phys_vec4 *vertices_b, int dim_a, int dim_b, const phys_vec4 &master_normal, phys_vec4 &simplex_point_a, phys_vec4 &support)
{
    int simplex_case = (dim_a << 3) | dim_b;
    switch (simplex_case)
    {
    case 0:
        break;
    case (1 << 3) + 1:
        {
            support.set_sub4(vertices_a[0], vertices_b[0]);
            float dist = phys_simplex_to_float(master_normal.dot3(support));
            float length_sq_quad = phys_simplex_to_float(support.length_sq_3d());
            float d;
            if (dist * 1000.0f < length_sq_quad)
            {
                d = phys_simplex_to_float(support.dot3(master_normal));
                support = master_normal;
            }
            else
            {
                d = phys_simplex_to_float(support.normalize_xyz_with_length());
            }
            support(3) = d;
            simplex_point_a = vertices_a[0];
            break;
        }
    case (1 << 3) + 2:
        {
            float d = phys_simplex_to_float(phys_simplex_recalc_point_edge(vertices_a, vertices_b, master_normal, support));
            support(3) = d;
            simplex_point_a = vertices_a[0];
            break;
        }
    case (2 << 3) + 1:
        {
            phys_simd_scalar d = phys_simplex_recalc_point_edge(vertices_b, vertices_a, master_normal, support);
            simplex_point_a = vertices_b[0];
            simplex_point_a.sub_mul4(d, support);
            support(3) = phys_simplex_to_float(-d);
            break;
        }
    case (3 << 3) + 1:
        {
            phys_vec4 *h = vertices_a;
            vertices_a = vertices_b;
            vertices_b = h;
        }
    case (1 << 3) + 3:
        {
            phys_simd_scalar d = phys_simplex_recalc_point_tri(vertices_a[0], vertices_b, master_normal, support);
            if (simplex_case == (1 << 3) + 3)
            {
                simplex_point_a = vertices_a[0];
                support(3) = phys_simplex_to_float(d);
            }
            else
            {
                simplex_point_a = vertices_a[0];
                simplex_point_a.sub_mul4(d, support);
                support(3) = phys_simplex_to_float(-d);
            }
            break;
        }
    case (2 << 3) + 2:
        {
            phys_simplex_edge_edge_i(vertices_a, vertices_b, master_normal, simplex_point_a, support);
            break;
        }
    default:
        break;
    }
}

// @address 0x684770 ?find_closest_triangle@@YAHABVphys_vec4@@PBV1@M@Z
int find_closest_triangle(const phys_vec4 &position, const phys_vec4 *vertices, float tolerance)
{
    phys_vec4 e0; e0.set_sub4(vertices[2], vertices[1]);
    phys_vec4 e2; e2.set_sub4(vertices[0], vertices[2]);
    phys_vec4 simplex_edge_ab; simplex_edge_ab.set_sub4(vertices[1], vertices[0]);
    phys_vec4 e1; e1.set_sub4(vertices[3], vertices[1]);
    phys_vec4 e3; e3.set_sub4(vertices[3], vertices[2]);
    phys_vec4 edge_ad; edge_ad.set_sub4(vertices[3], vertices[0]);
    phys_rotation normals;
    normals.get_column(0).set_cross(e0, e1);
    normals.get_column(1).set_cross(e2, e3);
    normals.get_column(2).set_cross(simplex_edge_ab, edge_ad);
    phys_vec4 rel_vec; rel_vec.set_sub4(position, vertices[3]);
    rel_vec.mul4(phys_simd_scalar(tolerance));
    phys_vec4 dist; dist.set_rotated_inverse_dir(normals, rel_vec);
    phys_vec4 abs_dist; abs_dist.set_abs4(dist);
    phys_vec4 signed_dist_sq; signed_dist_sq.m_simd_quad = __vmulfp(dist.m_simd_quad, abs_dist.m_simd_quad);

    float t[3];
    for (int i = 0; i < 3; i++)
    {
        float length_sq_quad = phys_simplex_to_float(normals.get_column(i).dot3(normals.get_column(i)));
        if (length_sq_quad != 0.0f)
        {
            t[i] = signed_dist_sq(i) / length_sq_quad;
        }
        else
        {
            t[i] = 3.40282e+38f;
        }
    }

    int best = 1;
    if (t[0] > t[1])
    {
        phys_simplex_swap(t[0], t[1]);
        best = 0;
    }
    if (t[1] > t[2])
    {
        phys_simplex_swap(t[1], t[2]);
        if (t[0] > t[1])
        {
            t[1] = t[0];
        }
    }
    else
    {
        best = 2;
    }

    if (!(t[2] >= 1.0000001e-06f))
    {
        return -1;
    }
    if (t[2] > t[1] * 1.1f)
    {
        return best;
    }
    phys_vec4 edge_dists = dist;
    return find_closest_triangle_backup(position, vertices, edge_dists, tolerance);
}


static __forceinline int phys_simplex_is_negative(float r0)
{
    return (r0 < 0) ? 8 : 0;
}





static __forceinline void phys_simplex_set_lerp4(phys_vec4 &out, const phys_vec4 &a, const phys_vec4 &b, phys_simd_scalar t)
{
    __vector4 x = __vspltw(t.get_quad(), 0);
    __vector4 s = __vsubfp(phys_simd_vec_1111, x);
    __vector4 temp = __vmulfp(s, a.m_simd_quad);
    out.m_simd_quad = __vmaddfp(x, b.m_simd_quad, temp);
}

// @address 0x684b08 ?process_edge_triangle@phys_simplex@@IAA?AW4next_case@1@PAVphys_vec4@@0AAV?$phys_padded@H@@1Vphys_bool@@W4support_kind@1@@Z
phys_simplex::next_case phys_simplex::process_edge_triangle(
    // match: 0x684b08: loads dim_a before the two vertex stores on the mask == 14 break path
    phys_vec4 *__restrict capsule_vertex_a,
    // match: 0x684b08: keeps the segment-pair path on its own return after decrementing dim_b
    phys_vec4 *__restrict capsule_vertex_b, phys_padded<int> &dim_a,
    phys_padded<int> &dim_b, phys_bool from_edge_edge, support_kind support_type)
{
    if (from_edge_edge)
    {
        phys_vec4 edge0; edge0.set_sub4(capsule_vertex_b[1], capsule_vertex_b[2]);
        phys_vec4 edge1; edge1.set_sub4(capsule_vertex_b[0], capsule_vertex_b[2]);
        phys_vec4 normal; normal.set_cross(edge0, edge1);
        phys_vec4 a0; a0.set_sub4(capsule_vertex_a[0], capsule_vertex_b[0]);
        phys_vec4 a1; a1.set_sub4(capsule_vertex_a[1], capsule_vertex_b[0]);
        phys_simd_scalar point_dist0 = normal.dot3(a0);
        phys_simd_scalar point_dist1 = normal.dot3(a1);
        int dist0_negative = phys_simplex_is_negative(point_dist0);
        int dist1_negative = phys_simplex_is_negative(point_dist1);
        if (dist0_negative != dist1_negative)
        {
            phys_simd_scalar t = point_dist0 / (point_dist0 - point_dist1);
            phys_vec4 p; phys_simplex_set_lerp4(p, capsule_vertex_a[0], capsule_vertex_a[1], t);
            int mask = check_triangle_boundaries(p, capsule_vertex_b, SIMPLEX_SUPPORT_NONE);
            if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
            {
                return SIMPLEX_NEXT_PENETRATION;
            }
        }
        else
        {
            if (fabsf(point_dist0) > fabsf(point_dist1))
            {
                phys_algorithm::swap(capsule_vertex_a[0], capsule_vertex_a[1]);
                phys_algorithm::swap(capsule_vertex_a[4], capsule_vertex_a[5]);
            }
            int mask = check_triangle_boundaries(capsule_vertex_a[0], capsule_vertex_b, SIMPLEX_SUPPORT_POS);
            if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
            {
                dim_a = dim_a - 1;
                return SIMPLEX_NEXT_BREAK;
            }
        }
        {
            phys_vec4 edge_a; edge_a.set_sub4(capsule_vertex_a[1], capsule_vertex_a[0]);
            phys_collide_triangle_util::segment_segment_result seg_result0;
            phys_collide_triangle_util::segment_segment_result seg_result1;
            phys_collide_triangle_util::closest_segment_pair(capsule_vertex_a[0], edge_a, capsule_vertex_b[2], edge0, seg_result0);
            phys_collide_triangle_util::closest_segment_pair(capsule_vertex_a[0], edge_a, capsule_vertex_b[2], edge1, seg_result1);
            int i = (seg_result0.m_dist_sq > seg_result1.m_dist_sq) ? 1 : 0;
            capsule_vertex_b[i] = capsule_vertex_b[2];
            capsule_vertex_b[i + 4] = capsule_vertex_b[6];
            dim_b = dim_b - 1;
            return SIMPLEX_NEXT_EDGE_EDGE;
        }
    }

    phys_vec4 dot_products = m_check_triangle_dots;
    phys_vec4 e1; e1.set_sub4(capsule_vertex_a[1], capsule_vertex_b[0]);
    phys_simd_scalar point_dist1 = m_support_vec.dot3(e1);
    phys_vec4 e0; e0.set_sub4(capsule_vertex_a[0], capsule_vertex_b[0]);
    phys_simd_scalar point_dist0 = m_support_vec.dot3(e0);
    if (phys_simplex_is_negative(point_dist0 * point_dist1))
    {
        phys_simd_scalar t = point_dist0 / (point_dist0 - point_dist1);
        phys_vec4 p; phys_simplex_set_lerp4(p, capsule_vertex_a[0], capsule_vertex_a[1], t);
        int point_mask = check_triangle_boundaries(p, capsule_vertex_b, SIMPLEX_SUPPORT_NONE);
        if (point_mask == phys_vec4_cmp::CMP_MASK_XYZ)
        {
            return SIMPLEX_NEXT_PENETRATION;
        }
    }
    int mask = check_triangle_boundaries(capsule_vertex_a[1], capsule_vertex_b, SIMPLEX_SUPPORT_NONE);
    if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
    {
        capsule_vertex_a[0] = capsule_vertex_a[1];
        capsule_vertex_a[4] = capsule_vertex_a[5];
        dim_a = dim_a - 1;
        m_support_state = support_type;
        return SIMPLEX_NEXT_BREAK;
    }

    int vertex_to_drop;
    if (mask == phys_vec4_cmp::CMP_MASK_YZ)
    {
        vertex_to_drop = 0;
    }
    else if (mask == phys_vec4_cmp::CMP_MASK_XZ)
    {
        vertex_to_drop = 1;
    }
    else if (mask == phys_vec4_cmp::CMP_MASK_XY)
    {
        vertex_to_drop = 2;
    }
    else if (mask == phys_vec4_cmp::CMP_MASK_X)
    {
        float d1 = m_check_triangle_dots(1) - dot_products(1);
        float d2 = m_check_triangle_dots(2) - dot_products(2);
        vertex_to_drop = (dot_products(2) * d1 > dot_products(1) * d2) ? 2 : 1;
    }
    else if (mask == phys_vec4_cmp::CMP_MASK_Y)
    {
        float d2 = m_check_triangle_dots(2) - dot_products(2);
        float d0 = m_check_triangle_dots(0) - dot_products(0);
        vertex_to_drop = (dot_products(0) * d2 > dot_products(2) * d0) ? 0 : 2;
    }
    else if (mask == phys_vec4_cmp::CMP_MASK_Z)
    {
        float d0 = m_check_triangle_dots(0) - dot_products(0);
        float d1 = m_check_triangle_dots(1) - dot_products(1);
        vertex_to_drop = (dot_products(1) * d0 > dot_products(0) * d1) ? 1 : 0;
    }
    else
    {
        vertex_to_drop = 0;
    }

    if (vertex_to_drop == 1)
    {
        capsule_vertex_b[1] = capsule_vertex_b[2];
        capsule_vertex_b[5] = capsule_vertex_b[6];
        dim_b = dim_b - 1;
        return SIMPLEX_NEXT_EDGE_EDGE;
    }
    if (vertex_to_drop == 0)
    {
        capsule_vertex_b[0] = capsule_vertex_b[2];
        capsule_vertex_b[4] = capsule_vertex_b[6];
        dim_b = dim_b - 1;
        return SIMPLEX_NEXT_EDGE_EDGE;
    }
    dim_b = dim_b - 1;
    return SIMPLEX_NEXT_EDGE_EDGE;
}

// @address 0x685158 ?reduce_dimension_ext@phys_simplex@@IAAXXZ
void phys_simplex::reduce_dimension_ext()
{
    if (m_dim_b == 3)
    {
        for (int i = 0; i < m_dim_a; i++)
        {
            int ret = check_triangle_boundaries(m_vertices_a[i], &m_vertices_b_in_a[0], SIMPLEX_SUPPORT_NONE);
            if (ret == phys_vec4_cmp::CMP_MASK_XYZ)
            {
                m_vertices_a[0] = m_vertices_a[i];
                m_dim_a = 1;
                return;
            }
        }
    }
    if (m_dim_a == 3)
    {
        for (int i = 0; i < m_dim_b; i++)
        {
            int ret = check_triangle_boundaries(m_vertices_b_in_a[i], &m_vertices_a[0], SIMPLEX_SUPPORT_NONE);
            if (ret == phys_vec4_cmp::CMP_MASK_XYZ)
            {
                m_vertices_b_in_b[0] = m_vertices_b_in_b[i];
                m_vertices_b_in_a[0] = m_vertices_b_in_a[i];
                m_dim_b = 1;
                return;
            }
        }
    }

    int max_bits_a = (m_dim_a == 3) ? 3 : 1;
    int max_bits_b = (m_dim_b == 3) ? 3 : 1;

    float best_dist = 3.40282e+38f;
    int best_a_idx = 0;
    int best_b_idx = 0;
    for (int a = 0; a < max_bits_a; a++)
    {
        for (int b = 0; b < max_bits_b; b++)
        {
            int a1 = phys_collide_triangle_util::vertex_to_edge_lut[a + 2];
            phys_vec4 edge_a; edge_a.set_sub4(m_vertices_a[a1], m_vertices_a[a]);
            int b1 = phys_collide_triangle_util::vertex_to_edge_lut[b + 2];
            phys_vec4 edge_b; edge_b.set_sub4(m_vertices_b_in_a[b1], m_vertices_b_in_a[b]);
            phys_collide_triangle_util::segment_segment_result result;
            phys_collide_triangle_util::closest_segment_pair(m_vertices_a[a], edge_a, m_vertices_b_in_a[b], edge_b, result);
            if (result.m_dist_sq < best_dist)
            {
                best_dist = result.m_dist_sq;
                best_a_idx = a;
                best_b_idx = b;
            }
        }
    }

    if (max_bits_a == 3)
    {
        m_dim_a = m_dim_a - 1;
        int i = phys_collide_triangle_util::vertex_to_edge_lut[best_a_idx];
        m_vertices_a[i] = m_vertices_a[m_dim_a];
    }
    if (max_bits_b == 3)
    {
        m_dim_b = m_dim_b - 1;
        int i = phys_collide_triangle_util::vertex_to_edge_lut[best_b_idx];
        m_vertices_b_in_b[i] = m_vertices_b_in_b[m_dim_b];
        m_vertices_b_in_a[i] = m_vertices_b_in_a[m_dim_b];
    }
}



// @address 0x6860c0 ?handle_penetration@phys_simplex@@IAAXPBVphys_convex_shape@@0ABVphys_transform@@@Z
void phys_simplex::handle_penetration(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
    const phys_transform &a_from_b)
{
    int simplex_dim = 4;
    if (m_dim_b >= 3)
    {
        if (m_dim_b > 3)
        {
            m_vertices_a[3] = m_vertices_a[0];
        }
        else if (m_dim_a > 1)
        {
            m_vertices_a[3] = m_vertices_a[1];
            m_vertices_b_in_b[3] = m_vertices_b_in_b[0];
            m_vertices_b_in_a[3] = m_vertices_b_in_a[0];
        }
        else
        {
            simplex_dim = 3;
        }
        m_vertices_a[1] = m_vertices_a[0];
        m_vertices_a[2] = m_vertices_a[0];
    }
    else if (m_dim_a >= 3)
    {
        if (m_dim_a > 3)
        {
            m_vertices_b_in_b[3] = m_vertices_b_in_b[0];
            m_vertices_b_in_a[3] = m_vertices_b_in_a[0];
        }
        else if (m_dim_b > 1)
        {
            m_vertices_a[3] = m_vertices_a[0];
            m_vertices_b_in_b[3] = m_vertices_b_in_b[1];
            m_vertices_b_in_a[3] = m_vertices_b_in_a[1];
        }
        else
        {
            simplex_dim = 3;
        }
        m_vertices_b_in_b[1] = m_vertices_b_in_b[0];
        m_vertices_b_in_a[1] = m_vertices_b_in_a[0];
        m_vertices_b_in_b[2] = m_vertices_b_in_b[0];
        m_vertices_b_in_a[2] = m_vertices_b_in_a[0];
    }
    else
    {
        simplex_dim = 1;
    }
    phys_vec4 simplex_verts[4];
    for (int i = 0; i < simplex_dim; i++)
    {
        simplex_verts[i].set_sub4(m_vertices_a[i], m_vertices_b_in_a[i]);
    }
    phys_simplex_out out;
    phys_compute_penetration_depth(penetration_shape_a, penetration_shape_b, a_from_b, 0.0001f, m_vertices_a, m_vertices_b_in_b,
        simplex_verts, &simplex_dim, out);
    if (simplex_dim == 1)
    {
        m_dim_a = 1;
        m_dim_b = 1;
    }
    else if (true)
    {
        m_dim_a = 3;
        m_dim_b = 3;
        if (m_vertices_a[1].get_packed_id() == m_vertices_a[2].get_packed_id())
        {
            m_dim_a = m_dim_a - 1;
        }
        else if (m_vertices_a[0].get_packed_id() == m_vertices_a[2].get_packed_id())
        {
            m_dim_a = m_dim_a - 1;
        }
        if (m_dim_a >= 2 && m_vertices_a[0].get_packed_id() == m_vertices_a[1].get_packed_id())
        {
            m_dim_a = m_dim_a - 1;
            m_vertices_a[0] = m_vertices_a[m_dim_a];
        }
        if (m_vertices_b_in_b[1].get_packed_id() == m_vertices_b_in_b[2].get_packed_id())
        {
            m_dim_b = m_dim_b - 1;
        }
        else if (m_vertices_b_in_b[0].get_packed_id() == m_vertices_b_in_b[2].get_packed_id())
        {
            m_dim_b = m_dim_b - 1;
        }
        if (m_dim_b >= 2 && m_vertices_b_in_b[0].get_packed_id() == m_vertices_b_in_b[1].get_packed_id())
        {
            m_dim_b = m_dim_b - 1;
            m_vertices_b_in_b[0] = m_vertices_b_in_b[m_dim_b];
        }
        phys_vec4_util::transform_points(a_from_b, m_vertices_b_in_b, m_dim_b, m_vertices_b_in_a);
        if (m_dim_a + m_dim_b > 4)
        {
            reduce_dimension_ext();
        }
        if (m_dim_a == 2 && m_dim_b == 2)
        {
            phys_vec4 dA; dA.set_sub4(m_vertices_a[1], m_vertices_a[0]);
            phys_vec4 dB; dB.set_sub4(m_vertices_b_in_a[1], m_vertices_b_in_a[0]);
            phys_collide_triangle_util::closest_segment_pair(m_vertices_a[0], dA, m_vertices_b_in_a[0], dB, m_closest_line_result);
        }
    }
    else
    {
        reduce_dimension();
    }
    m_support_vec = out.m_normal_in_a;
    set_feature_change(1);
}


__forceinline phys_simplex::reduce_dim_result phys_simplex::reduce_dimension()
{
    m_support_state = SIMPLEX_SUPPORT_CHANGED;
    while (1)
    {
        reduce_dim_result outcome = SIMPLEX_REDUCE_PENETRATION;
        support_kind support_type = SIMPLEX_SUPPORT_NEG;
        switch ((m_dim_a << 3) | m_dim_b)
        {
        case SIMPLEX_POINT_POINT:
            {
                m_support_vec.set_sub4(m_vertices_a[0], m_vertices_b_in_a[0]);
                outcome = SIMPLEX_REDUCE_OK;
                break;
            }

        case SIMPLEX_EDGE_TRIANGLE:
            support_type = SIMPLEX_SUPPORT_POS;
            // Fall through.
        case SIMPLEX_TRIANGLE_EDGE:
            {
                next_case next;
                if (support_type == SIMPLEX_SUPPORT_POS)
                {
                    next = process_edge_triangle(&m_vertices_a[0], &m_vertices_b_in_a[0], m_dim_a, m_dim_b, m_last_dim_b == 2, support_type);
                }
                else
                {
                    next = process_edge_triangle(&m_vertices_b_in_a[0], &m_vertices_a[0], m_dim_b, m_dim_a, m_last_dim_b == 2, support_type);
                }
                if (next == SIMPLEX_NEXT_PENETRATION)
                {
                    break;
                }
                if (next != SIMPLEX_NEXT_EDGE_EDGE)
                {
                    outcome = SIMPLEX_REDUCE_OK;
                    break;
                }
            }
            // Fall through.
        case SIMPLEX_EDGE_EDGE:
            {
                phys_vec4 dA; dA.set_sub4(m_vertices_a[1], m_vertices_a[0]);
                phys_vec4 dB; dB.set_sub4(m_vertices_b_in_a[1], m_vertices_b_in_a[0]);
                int result = phys_collide_triangle_util::closest_segment_pair(m_vertices_a[0], dA, m_vertices_b_in_a[0], dB, m_closest_line_result);
                if (result == 0)
                {
                    m_support_vec.set_cross(dA, dB);
                    phys_vec4 diff; diff.set_sub4(m_vertices_a[0], m_vertices_b_in_a[0]);
                    if (phys_simplex_to_float(m_support_vec.dot3(diff)) < 0.0f)
                    {
                        m_support_vec.set_neg4(m_support_vec);
                    }
                    outcome = SIMPLEX_REDUCE_OK;
                    break;
                }
                if (result & 1)
                {
                    m_vertices_a[0] = m_vertices_a[1];
                    m_dim_a = 1;
                }
                else if (result & 2)
                {
                    m_dim_a = 1;
                }
                if (result & 4)
                {
                    m_vertices_b_in_a[0] = m_vertices_b_in_a[1];
                    m_vertices_b_in_b[0] = m_vertices_b_in_b[1];
                    m_dim_b = 1;
                }
                else if (result & 8)
                {
                    m_dim_b = 1;
                }
                continue;
            }

        case SIMPLEX_POINT_TETRA:
            {
                int sweep_max_index = find_closest_triangle(m_vertices_a[0], &m_vertices_b_in_a[0], 1.0f);
                if (sweep_max_index < 0)
                {
                    break;
                }
                m_dim_b = m_dim_b - 1;
                m_vertices_b_in_a[sweep_max_index] = m_vertices_b_in_a[3];
                m_vertices_b_in_b[sweep_max_index] = m_vertices_b_in_b[3];
            }
            // Fall through.
        case SIMPLEX_POINT_TRIANGLE:
            {
                int mask = check_triangle_boundaries(m_vertices_a[0], &m_vertices_b_in_a[0], SIMPLEX_SUPPORT_POS);
                if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
                {
                    outcome = SIMPLEX_REDUCE_OK;
                    break;
                }
                int edge_case_idx = phys_collide_triangle_util::mask_to_index[mask];
                if (edge_case_idx >= 0)
                {
                    int i1 = phys_collide_triangle_util::vertex_to_edge_lut[edge_case_idx];
                    int i2 = phys_collide_triangle_util::vertex_to_edge_lut[edge_case_idx + 2];
                    phys_vec4 a; a.set_sub4(m_vertices_a[0], m_vertices_b_in_a[edge_case_idx]);
                    phys_vec4 b; b.set_sub4(m_vertices_b_in_a[i1], m_vertices_b_in_a[edge_case_idx]);
                    phys_vec4 c; c.set_sub4(m_vertices_b_in_a[i2], m_vertices_b_in_a[edge_case_idx]);
                    if (phys_simplex_to_float(a.dot3(b)) < 0.0f)
                    {
                        m_dim_b = 2;
                        m_vertices_b_in_a[i1] = m_vertices_b_in_a[2];
                        m_vertices_b_in_b[i1] = m_vertices_b_in_b[2];
                        if (phys_simplex_to_float(a.dot3(c)) < 0.0f)
                        {
                            if (i2 == m_dim_b)
                            {
                                i2 = i1;
                            }
                            m_dim_b = m_dim_b - 1;
                            m_vertices_b_in_a[i2] = m_vertices_b_in_a[m_dim_b];
                            m_vertices_b_in_b[i2] = m_vertices_b_in_b[m_dim_b];
                        }
                    }
                    else
                    {
                        m_dim_b = 2;
                        m_vertices_b_in_a[i2] = m_vertices_b_in_a[2];
                        m_vertices_b_in_b[i2] = m_vertices_b_in_b[2];
                    }
                    continue;
                }
                edge_case_idx = edge_case_idx + 8;
                if (edge_case_idx > 3)
                {
                    edge_case_idx = 1;
                }
                m_dim_b = m_dim_b - 1;
                m_vertices_b_in_a[edge_case_idx] = m_vertices_b_in_a[2];
                m_vertices_b_in_b[edge_case_idx] = m_vertices_b_in_b[2];
            }
            // Fall through.
        case SIMPLEX_POINT_EDGE:
            {
                handle_point_edge(&m_vertices_a[0], &m_vertices_b_in_a[0], m_dim_b, m_support_vec, SIMPLEX_SUPPORT_POS);
                outcome = SIMPLEX_REDUCE_OK;
                break;
            }

        case SIMPLEX_TETRA_POINT:
            {
                int sweep_max_index = find_closest_triangle(m_vertices_b_in_a[0], &m_vertices_a[0], -1.0f);
                if (sweep_max_index < 0)
                {
                    break;
                }
                m_dim_a = m_dim_a - 1;
                m_vertices_a[sweep_max_index] = m_vertices_a[3];
            }
            // Fall through.
        case SIMPLEX_TRIANGLE_POINT:
            {
                int mask = check_triangle_boundaries(m_vertices_b_in_a[0], &m_vertices_a[0], SIMPLEX_SUPPORT_NEG);
                if (mask == phys_vec4_cmp::CMP_MASK_XYZ)
                {
                    outcome = SIMPLEX_REDUCE_OK;
                    break;
                }
                int edge_case_idx = phys_collide_triangle_util::mask_to_index[mask];
                if (edge_case_idx >= 0)
                {
                    int i1 = phys_collide_triangle_util::vertex_to_edge_lut[edge_case_idx];
                    int i2 = phys_collide_triangle_util::vertex_to_edge_lut[edge_case_idx + 2];
                    phys_vec4 a; a.set_sub4(m_vertices_b_in_a[0], m_vertices_a[edge_case_idx]);
                    phys_vec4 b; b.set_sub4(m_vertices_a[i1], m_vertices_a[edge_case_idx]);
                    phys_vec4 c; c.set_sub4(m_vertices_a[i2], m_vertices_a[edge_case_idx]);
                    if (phys_simplex_to_float(a.dot3(b)) < 0.0f)
                    {
                        m_dim_a = 2;
                        m_vertices_a[i1] = m_vertices_a[2];
                        if (phys_simplex_to_float(a.dot3(c)) < 0.0f)
                        {
                            if (i2 == m_dim_a)
                            {
                                i2 = i1;
                            }
                            m_dim_a = m_dim_a - 1;
                            m_vertices_a[i2] = m_vertices_a[m_dim_a];
                        }
                    }
                    else
                    {
                        m_dim_a = 2;
                        m_vertices_a[i2] = m_vertices_a[m_dim_a];
                    }
                    continue;
                }
                edge_case_idx = edge_case_idx + 8;
                if (edge_case_idx > 3)
                {
                    edge_case_idx = 1;
                }
                m_dim_a = m_dim_a - 1;
                m_vertices_a[edge_case_idx] = m_vertices_a[2];
            }
            // Fall through.
        case SIMPLEX_EDGE_POINT:
            {
                handle_point_edge(&m_vertices_b_in_a[0], &m_vertices_a[0], m_dim_a, m_support_vec, SIMPLEX_SUPPORT_NEG);
                outcome = SIMPLEX_REDUCE_OK;
                break;
            }
        case SIMPLEX_NONE_NONE:
        default:
            break;
        }

        if (outcome == SIMPLEX_REDUCE_OK)
        {
            m_last_dim_b = m_dim_b;
            return SIMPLEX_REDUCE_OK;
        }
        return SIMPLEX_REDUCE_PENETRATION;
    }
}

// @address 0x686610 ?get_closest_feature@phys_simplex@@QAA?AW4phys_simplex_status@@PBVphys_convex_shape@@0ABVphys_transform@@AAVphys_vec4@@@Z
phys_simplex_status phys_simplex::get_closest_feature(const phys_convex_shape *penetration_shape_a,
    const phys_convex_shape *penetration_shape_b, const phys_transform &a_from_b, phys_vec4 &separating_normal_out)
{
    phys_vec4_util::transform_points(a_from_b, &m_vertices_b_in_b[0], m_dim_b, &m_vertices_b_in_a[0]);

    int iteration = 0;
    float epsilon = 1e-10f;
    while (1)
    {
        iteration++;
        epsilon *= 2.0f;
        int old_dim_sum = m_dim_a + m_dim_b;
        reduce_dim_result reduce_outcome = reduce_dimension();
        float support_len_sq = phys_simplex_to_float(m_support_vec.length_sq_3d());
        m_feature_changed = m_feature_changed | (m_dim_a + m_dim_b - old_dim_sum);
        phys_vec4 d; d.set_sub4(m_vertices_a[0], m_vertices_b_in_a[0]);
        float dist_sq_a = phys_simplex_to_float(d.dot3(m_support_vec));

        if (reduce_outcome != SIMPLEX_REDUCE_OK || fabsf(dist_sq_a) * dist_sq_a <= epsilon * support_len_sq || support_len_sq < epsilon)
        {
            if (!m_skip_penetration)
            {
                handle_penetration(penetration_shape_a, penetration_shape_b, a_from_b);
                break;
            }
            else
            {
                if (m_dim_a + m_dim_b > 4)
                {
                    if (m_dim_a > m_dim_b)
                    {
                        m_dim_a = 3;
                        m_dim_b = 1;
                    }
                    else
                    {
                        m_dim_a = 1;
                        m_dim_b = 3;
                    }
                }
                return PHYS_SIMPLEX_PENETRATING;
            }
        }

        float eps_support = epsilon * support_len_sq;
        if ((iteration & 1) && m_dim_a != m_max_dim_a && m_support_state != SIMPLEX_SKIP_SUPPORT_A)
        {
            phys_vec4 neg_support; neg_support.set_neg4(m_support_vec);
            penetration_shape_a->get_support_vertex(neg_support, m_vertices_a[m_dim_a]);
            if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[0]) > eps_support)
            {
                switch (m_dim_a)
                {
                case 3:
                    if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[2]) < eps_support) break;
                case 2:
                    if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[1]) < eps_support) break;
                default:
                    m_dim_a = m_dim_a + 1;
                    set_feature_change(1);
                    continue;
                }
            }
        }
        if (m_dim_b != m_max_dim_b && m_support_state != SIMPLEX_SKIP_SUPPORT_B)
        {
            phys_vec4 support_in_b; support_in_b.set_rotated_inverse_dir(a_from_b.get_rotation(), m_support_vec);
            penetration_shape_b->get_support_vertex(support_in_b, m_vertices_b_in_b[m_dim_b]);
            if (phys_simplex_support_dist_sq(support_in_b, m_vertices_b_in_b[m_dim_b], m_vertices_b_in_b[0]) > eps_support)
            {
                switch (m_dim_b)
                {
                case 3:
                    if (phys_simplex_support_dist_sq(support_in_b, m_vertices_b_in_b[m_dim_b], m_vertices_b_in_b[2]) < eps_support) break;
                case 2:
                    if (phys_simplex_support_dist_sq(support_in_b, m_vertices_b_in_b[m_dim_b], m_vertices_b_in_b[1]) < eps_support) break;
                default:
                    m_vertices_b_in_a[m_dim_b].set_transformed_pos(a_from_b, m_vertices_b_in_b[m_dim_b]);
                    m_dim_b = m_dim_b + 1;
                    set_feature_change(1);
                    continue;
                }
            }
        }
        if (!(iteration & 1) && m_dim_a != m_max_dim_a && m_support_state != SIMPLEX_SKIP_SUPPORT_A)
        {
            phys_vec4 neg_support; neg_support.set_neg4(m_support_vec);
            penetration_shape_a->get_support_vertex(neg_support, m_vertices_a[m_dim_a]);
            if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[0]) > eps_support)
            {
                switch (m_dim_a)
                {
                case 3:
                    if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[2]) < eps_support) break;
                case 2:
                    if (phys_simplex_support_dist_sq(neg_support, m_vertices_a[m_dim_a], m_vertices_a[1]) < eps_support) break;
                default:
                    m_dim_a = m_dim_a + 1;
                    set_feature_change(1);
                    continue;
                }
            }
        }
        break;
    }

    phys_vec4 diff; diff.set_sub4(m_vertices_a[0], m_vertices_b_in_a[0]);
    phys_vec4 support; support = m_support_vec;
    support.normalize_xyz();
    phys_vec4 dist; dist = diff.dot3(support).get_quad();
    m_support_vec.set_xyzw(support, dist);
    separating_normal_out.set_xyzw(support, dist);
    return PHYS_SIMPLEX_OK;
}

// @address 0x6874d8 ?get_closest_point@phys_simplex@@SA?AW4phys_result@@ABUclosest_point_input@1@AAVphys_simplex_cache@@AAVphys_vec4@@2@Z
phys_result phys_simplex::get_closest_point(const closest_point_input &input, phys_simplex_cache &cache,
    phys_vec4 &separating_normal_out, phys_vec4 &point_on_b_out)
{
    phys_simplex gsk;
    gsk.init(input.m_shape_a, input.m_shape_b, cache);

    phys_vec4 simplex_separating_normal;
    gsk.get_closest_feature(input.m_shape_a, input.m_shape_b, *input.m_a_to_b, simplex_separating_normal);
    gsk.refresh_simplex_cache(cache);

    float dist = simplex_separating_normal(3) - input.m_shape_a->get_convex_radius() - input.m_shape_b->get_convex_radius();
    separating_normal_out.set_rotated_dir(input.m_transform_a->get_rotation(), simplex_separating_normal);
    separating_normal_out(3) = dist;

    if (dist < input.m_collision_tolerance)
    {
        phys_vec4 point_on_b;
        if (gsk.m_dim_a == 1)
        {
            point_on_b.set_add_mul4(gsk.m_vertices_a[0], simplex_separating_normal, input.m_shape_b->get_convex_radius() - simplex_separating_normal(3));
        }
        else if (gsk.m_dim_b == 1)
        {
            point_on_b.set_add_mul4(gsk.m_vertices_b_in_a[0], simplex_separating_normal, input.m_shape_b->get_convex_radius());
        }
        else
        {
            point_on_b.set_add_mul4(gsk.m_closest_line_result.m_closest_point_a, simplex_separating_normal,
                input.m_shape_b->get_convex_radius() - simplex_separating_normal(3));
        }
        point_on_b_out.set_transformed_pos(*input.m_transform_a, point_on_b);
        return PHYS_SUCCESS;
    }
    return PHYS_FAILURE;
}


// @address 0x685400 ?sweep_cast@phys_simplex@@QAA?AVphys_bool@@PBVphys_convex_shape@@0ABVphys_rotation@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_simplex::sweep_cast(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
    const phys_rotation &a_from_b, const phys_ray_input &input,
    phys_ray_output &output)
{
    unsigned short id;
    phys_vec4 ray_dir; ray_dir.set_sub4(input.m_ray_to, input.m_ray_from);
    float float_epsilon = 0.001f;
    float radius = penetration_shape_b ? penetration_shape_a->get_convex_radius() +
        penetration_shape_b->get_convex_radius() : penetration_shape_a->get_convex_radius();
    float radius_sq = (radius + float_epsilon) * (radius + float_epsilon);
    float line_lambda = 0.0f;
    phys_vec4 normal; normal.set_zero4();
    phys_hull_vertex position = static_cast<const phys_hull_vertex &>(input.m_ray_from);
    {
        id = 0;
        phys_hull_vertex vertex;
        penetration_shape_a->vertex_ids_to_vertices(&id, 1, &vertex);
        m_support_vec.set_sub4(position, vertex);
    }
    phys_vec4 dir = m_support_vec;
    dir.normalize_xyz();

    m_dim_a = 0;
    m_dim_b = 1;
    phys_hull_vertex support_a; support_a.set_zero4();
    phys_hull_vertex support_b; support_b.set_zero4();
    float distance = 1e7f;
    int stalled_iter_cnt = 0;
    int iterations = 0;
    while (distance * distance > radius_sq)
    {
        iterations++;
        penetration_shape_a->get_support_vertex(m_support_vec, support_a);
        if (penetration_shape_b)
        {
            phys_vec4 neg_dir; neg_dir.set_neg4(m_support_vec);
            neg_dir.vec_set_rotated_inverse_dir(a_from_b, neg_dir);
            penetration_shape_b->get_support_vertex(neg_dir, support_b);
            phys_vec4 support_b_in_a; support_b_in_a.vec_set_rotated_dir(a_from_b, support_b);
            support_a.sub3_clobber_w(support_b_in_a);
        }
        phys_bool is_new_vertex = true;
        for (int i = 0; i < m_dim_a; i++)
        {
            if (m_vertices_a[i](0) == support_a(0) && m_vertices_a[i](1) == support_a(1) && m_vertices_a[i](2) == support_a(2))
            {
                is_new_vertex = false;
                break;
            }
        }
        if (is_new_vertex)
        {
            m_vertices_a[m_dim_a] = support_a;
            m_dim_a = m_dim_a + 1;
        }
        {
            phys_vec4 p; p.set_add_mul4(support_a, dir, phys_simd_scalar(radius));
            phys_vec4 w; w.set_sub4(position, p);
            float vw = phys_simplex_to_float(m_support_vec.dot3(w));
            if (vw > 0.0f)
            {
                float vr = phys_simplex_to_float(m_support_vec.dot3(ray_dir));
                if (vr >= 0.0f)
                {
                    return false;
                }
                line_lambda = line_lambda - vw / vr;
                if (line_lambda >= 1.0f)
                {
                    return false;
                }
                position.set_add_mul4(input.m_ray_from, ray_dir, phys_simd_scalar(line_lambda));
                normal = dir;
            }
        }
        phys_vec4 old_support = m_support_vec;
        m_vertices_b_in_b[0] = position;
        m_vertices_b_in_a[0] = m_vertices_b_in_b[0];
        reduce_dim_result reduce_outcome = reduce_dimension();
        if (reduce_outcome != SIMPLEX_REDUCE_OK)
        {
            break;
        }
        m_support_vec.set_mul4(phys_simd_scalar(-1.0f), m_support_vec);
        dir = m_support_vec;
        dir.normalize_xyz();
        for (int i = 0; i < m_dim_a; i++)
        {
            phys_vec4 diff; diff.set_sub4(m_vertices_b_in_a[0], m_vertices_a[0]);
            float dist = fabsf(phys_simplex_to_float(dir.dot3(diff)));
            if (dist < distance)
            {
                distance = dist;
            }
        }
        phys_vec4 delta; delta.set_sub4(m_support_vec, old_support);
        if (phys_simplex_to_float(delta.length_sq_3d()) < 1e-10f)
        {
            stalled_iter_cnt++;
            if (stalled_iter_cnt >= 3)
            {
                break;
            }
        }
    }
    if (line_lambda < output.m_hit_frac)
    {
        output.m_hit_frac = line_lambda;
        if (phys_simplex_to_float(normal.length_sq_3d()) > 0.0f)
        {
            output.m_normal = normal;
        }
        else
        {
            output.m_normal = normal;
        }
        return true;
    }
    return false;
}

