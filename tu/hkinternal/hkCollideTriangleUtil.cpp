// hkinternal/hkCollideTriangleUtil.obj: from the records 0x692af0 0x692c90 0x692d20 0x692df8 0x692eb8 0x693000 0x6933a0
// @compiler xdk5632
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include <math.h>
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide_triangle_util.h"

struct __vector4;

namespace phys_math {  }
#include "phys/phys_algorithm.h"


class phys_vec4_util {
public:
    static __forceinline void __cdecl dot3_four_pairs(const phys_vec4 &a0, const phys_vec4 &b0, const phys_vec4 &a1, const phys_vec4 &b1,
        const phys_vec4 &a2, const phys_vec4 &b2, const phys_vec4 &a3, const phys_vec4 &b3, phys_vec4 &dot_products_out)
    {
        dot_products_out.m_simd_quad = __vrlimi(__vmsum3fp(a0.m_simd_quad, b0.m_simd_quad), __vmsum3fp(a1.m_simd_quad, b1.m_simd_quad), 4, 0);
        dot_products_out.m_simd_quad = __vrlimi(dot_products_out.m_simd_quad, __vmsum3fp(a2.m_simd_quad, b2.m_simd_quad), 2, 0);
        dot_products_out.m_simd_quad = __vrlimi(dot_products_out.m_simd_quad, __vmsum3fp(a3.m_simd_quad, b3.m_simd_quad), 1, 0);
    }
};



// @address 0x692c90 ?closest_point_on_segment@phys_collide_triangle_util@@SAHABVphys_vec4@@00AAUpoint_segment_result@1@@Z
int phys_collide_triangle_util::closest_point_on_segment(const phys_vec4 &A, const phys_vec4 &B, const phys_vec4 &B2, point_segment_result &result)
{
    __vector4 origin = B.m_simd_quad;
    __vector4 edge = __vsubfp(B2.m_simd_quad, origin);
    float proj_dists[8];
    __stvewx(__vmsum3fp(edge, __vsubfp(A.m_simd_quad, origin)), (void *)&proj_dists[0], 0);
    __stvewx(__vmsum3fp(edge, edge), (void *)&proj_dists[1], 0);
    float s2 = proj_dists[0];
    if (!(s2 > 0.0f)) {
        result.m_edge_point.m_simd_quad = origin;
        return 8;
    }
    float d2 = proj_dists[1];
    if (!(s2 < d2)) {
        result.m_edge_point.m_simd_quad = __vaddfp(origin, edge);
        return 4;
    }
    proj_dists[1] = s2 / d2;
    result.m_edge_point.m_simd_quad = __vmaddfp(edge, __vspltw(__lvlx((void *)&proj_dists[1], 0), 0), origin);
    return 0;
}

// @address 0x692eb8 ?compute_triangle_plane_dirs@phys_collide_triangle_util@@SAXPBVphys_vec4@@ABUpoint_tri_distance_cache@1@AAVphys_transform@@AAV2@@Z
void phys_collide_triangle_util::compute_triangle_plane_dirs(
    const phys_vec4 *tri_corners, const point_tri_distance_cache &cache, phys_transform &plane_equations_out, phys_vec4 &normal_out)
{
    phys_vec4 E0; E0.set_sub4(tri_corners[2], tri_corners[1]);
    phys_vec4 E1; E1.set_sub4(tri_corners[0], tri_corners[2]);
    phys_vec4 E2; E2.set_sub4(tri_corners[1], tri_corners[0]);


    phys_vec4 tri_normal;
    tri_normal.set_cross(E0, E1);
    tri_normal.mul4(phys_simd_scalar(cache.m_inv_normal_len));
    normal_out = tri_normal;

    E0.mul4(phys_simd_scalar(cache.m_inv_edge_len[0]));
    E1.mul4(phys_simd_scalar(cache.m_inv_edge_len[1]));
    E2.mul4(phys_simd_scalar(cache.m_inv_edge_len[2]));


    plane_equations_out.get_column(0).set_cross(E0, tri_normal);
    plane_equations_out.get_column(1).set_cross(E1, tri_normal);
    plane_equations_out.get_column(2).set_cross(E2, tri_normal);

    phys_algorithm::swap(plane_equations_out.get_column(1)(0), plane_equations_out.get_column(0)(1));
    phys_algorithm::swap(plane_equations_out.get_column(2)(0), plane_equations_out.get_column(0)(2));
    phys_algorithm::swap(plane_equations_out.get_column(2)(1), plane_equations_out.get_column(1)(2));

    plane_equations_out.get_column(0)(3) = normal_out(0);
    plane_equations_out.get_column(1)(3) = normal_out(1);
    plane_equations_out.get_column(2)(3) = normal_out(2);

    plane_equations_out.get_column(3).set_zero4();
    plane_equations_out.get_column(3)(0) = -(cache.m_normal_len * cache.m_inv_edge_len[0]);
}

// @address 0x692df8 ?setup_point_triangle_distance_cache@phys_collide_triangle_util@@SAXPBVphys_vec4@@AAUpoint_tri_distance_cache@1@@Z
void phys_collide_triangle_util::setup_point_triangle_distance_cache(const phys_vec4 *triangle, point_tri_distance_cache &cache)
{
    phys_vec4 E0;
    phys_vec4 E1;
    phys_vec4 E2;
    E0.set_sub4(triangle[2], triangle[1]);
    E1.set_sub4(triangle[0], triangle[2]);
    E2.set_sub4(triangle[1], triangle[0]);
    phys_vec4 tri_normal;
    tri_normal.set_cross(E0, E1);
    phys_vec4 dot_products;
    phys_vec4_util::dot3_four_pairs(E0, E0, E1, E1, E2, E2, tri_normal, tri_normal, dot_products);
    phys_vec4 inv_sqrt_dots;
    inv_sqrt_dots.set_sqrt_inverse4(dot_products);
    cache.m_inv_edge_len[0] = inv_sqrt_dots(0);
    cache.m_inv_edge_len[1] = inv_sqrt_dots(1);
    cache.m_inv_edge_len[2] = inv_sqrt_dots(2);
    cache.m_inv_normal_len = inv_sqrt_dots(3);
    cache.m_normal_len = dot_products(3) * inv_sqrt_dots(3);
}

// @address 0x692d20 ?setup_closest_point_triangle_cache@phys_collide_triangle_util@@SAXPBVphys_vec4@@AAUclosest_point_tri_cache@1@@Z
void phys_collide_triangle_util::setup_closest_point_triangle_cache(const phys_vec4 *triangle, closest_point_tri_cache &cache)
{
    phys_vec4 Q; Q.set_sub4(triangle[0], triangle[1]);
    phys_vec4 R; R.set_sub4(triangle[2], triangle[1]);
    const float q_len_sq = Q.length_sq_3d();
    const float r_len_sq = R.length_sq_3d();
    const float q_dot_r = R.dot3(Q);
    float qq_times_rr = q_len_sq * r_len_sq;
    float qr_squared = q_dot_r * q_dot_r;
    float det_qr = qq_times_rr - qr_squared;
    float contact_inv_det = 1.0f / det_qr;
    cache.m_values[0] = q_len_sq * contact_inv_det;
    cache.m_values[1] = r_len_sq * contact_inv_det;
    cache.m_values[2] = q_dot_r * contact_inv_det;
    phys_vec4 cross; cross.set_cross(Q, R);
    float len = cross.length_3d();
    cache.m_values[3] = 1.0f / len;
}

// @address 0x6933a0 ?compute_barycentric_coords@phys_collide_triangle_util@@SAXABVphys_vec4@@000QAV?$phys_padded@M@@@Z
void phys_collide_triangle_util::compute_barycentric_coords(
    const phys_vec4 &pos, const phys_vec4 &t0, const phys_vec4 &t1, const phys_vec4 &t2, phys_padded<float> *const result)
{
    phys_vec4 Q; Q.set_sub4(t0, t1);
    phys_vec4 R;
    R.set_sub4(t2, t1);
    const float q_len_sq = Q.length_sq_3d();
    const float r_len_sq = R.length_sq_3d();
    const float q_dot_r = R.dot3(Q);
    float qq_times_rr = q_len_sq * r_len_sq;
    float qr_squared = q_dot_r * q_dot_r;
    const float capsule_determinant = qq_times_rr - qr_squared;
    if (capsule_determinant > 0.0f) {
        const float contact_inv_det = 1.0f / capsule_determinant;
        phys_vec4 ap_vec; ap_vec.set_sub4(t1, pos);
        const float ap_dot_q = ap_vec.dot3(Q);
        const float ap_dot_r = ap_vec.dot3(R);
        const float u = ap_dot_r * q_dot_r - r_len_sq * ap_dot_q;
        const float v = ap_dot_q * q_dot_r - q_len_sq * ap_dot_r;
        result[0] = contact_inv_det * u;
        result[1] = contact_inv_det * (capsule_determinant - u - v);
        result[2] = contact_inv_det * v;
    } else {
        phys_vec4 S; S.set_sub4(t0, t2);
        const float s_len_sq = S.length_sq_3d();
        if (q_len_sq >= r_len_sq) {
            if (s_len_sq >= q_len_sq) {
                result[1] = 0.0f;
                if (s_len_sq > 0.0f) {
                    phys_vec4 d; d.set_sub4(pos, t2);
                    result[0] = static_cast<float>(d.dot3(S)) / s_len_sq;
                    result[2] = 1.0f - result[0];
                } else {
                    result[0] = 0.0f;
                    result[2] = 0.0f;
                }
            } else {
                phys_vec4 d; d.set_sub4(pos, t1);
                result[0] = static_cast<float>(d.dot3(Q)) / q_len_sq;
                result[1] = 1.0f - result[0];
                result[2] = 0.0f;
            }
        } else {
            phys_vec4 P; P = pos;
            if (s_len_sq >= r_len_sq) {
                phys_vec4 d; d.set_sub4(P, t2);
                result[0] = static_cast<float>(d.dot3(S)) / s_len_sq;
                result[1] = 0.0f;
                result[2] = 1.0f - result[0];
            } else {
                phys_vec4 d; d.set_sub4(P, t1);
                const float d_dot_r = d.dot3(R);
                result[0] = 0.0f;
                result[2] = d_dot_r / r_len_sq;
                result[1] = 1.0f - result[2];
            }
        }
    }
}

// @address 0x692af0 ?closest_segment_pair@phys_collide_triangle_util@@SAHABVphys_vec4@@000AAUsegment_segment_result@1@@Z
int phys_collide_triangle_util::closest_segment_pair(
    const phys_vec4 &A, const phys_vec4 &dA, const phys_vec4 &B, const phys_vec4 &dB, segment_segment_result &result)
{
    phys_vec4 d12; d12.set_sub4(B, A);

    const float R = dA.dot3(dB);
    const float S1 = dA.dot3(d12);
    const float S2 = dB.dot3(d12);

    const float D1 = dA.length_sq_3d();
    const float D2 = dB.length_sq_3d();

    float t, u;
    int return_value;

    const float box_denominator = phys_math::fabs(D1 * D2 - R * R);

    {
        t = S1 * D2 - S2 * R;

        if (t * box_denominator >= box_denominator * box_denominator)
        {
            t = 1.0f;
            return_value = 1;
        }
        else if (t <= 0.0f)
        {
            t = 0.0f;
            return_value = 2;
        }
        else
        {
            const float float_epsilon = (D1 * D2 + R * R) * 9.5367431640625e-7f;
            if (box_denominator > float_epsilon)
            {
                t /= box_denominator;
                return_value = 0;
            }
            else
            {
                t = 1.0f;
                return_value = 1;
            }
        }
    }

    do
    {
        u = t * R - S2;

        if (u >= D2)
        {
            u = 1.0f;
            return_value = 4;
        }
        else if (u <= 0.0f)
        {
            u = 0.0f;
            return_value = 8;
        }
        else
        {
            u /= D2;
            break;
        }

        t = u * R + S1;

        if (t <= 0.0f)
        {
            t = 0.0f;
            return_value |= 2;
        }
        else if (t >= D1)
        {
            t = 1.0f;
            return_value |= 1;
        }
        else
        {
            t /= D1;
        }
    } while (false);

    phys_vec4 &closest_point_a = result.m_closest_point_a;
    closest_point_a.set_add_mul4(A, dA, phys_simd_scalar(t));
    phys_vec4 closest_point_b;
    closest_point_b.set_add_mul4(B, dB, phys_simd_scalar(u));
    result.m_closest_a_minus_b.set_sub4(closest_point_a, closest_point_b);
    result.m_dist_sq = result.m_closest_a_minus_b.length_sq_3d();
    return return_value;
}

const __vector4 phys_simd_vec_half = { 0.5f, 0.5f, 0.5f, 0.5f };
const signed char phys_collide_triangle_util::mask_to_index[] = { -1, -1, 2, -1, 1, -1, -8, -1, 0, -1, -7, -1, -6, -1, -1, -1 };
const signed char phys_collide_triangle_util::vertex_to_edge_lut[] = { 2, 0, 1, 2, 0 };

// @address 0x693000 ?closest_point_on_triangle@phys_collide_triangle_util@@SA?AW4closest_point_tri_status@1@ABVphys_vec4@@PBV3@ABUclosest_point_tri_cache@1@AAUclosest_point_tri_result@1@@Z
phys_collide_triangle_util::closest_point_tri_status phys_collide_triangle_util::closest_point_on_triangle(
    const phys_vec4 &position, const phys_vec4 *tri_corners, const closest_point_tri_cache &cache, closest_point_tri_result &result)
{
    phys_vec4 rel_pos; rel_pos.set_sub4(tri_corners[1], position);
    phys_vec4 Q; Q.set_sub4(tri_corners[0], tri_corners[1]);
    float sq = rel_pos.dot3(Q);
    phys_vec4 R; R.set_sub4(tri_corners[2], tri_corners[1]);
    float sr = rel_pos.dot3(R);

    float q = sr * cache.m_values[2] - cache.m_values[1] * sq;
    float r = sq * cache.m_values[2] - cache.m_values[0] * sr;

    const float rel_eps = 0.001f;
    phys_vec4 proj_dists;
    proj_dists(0) = q + rel_eps;
    proj_dists(1) = (1.0f + rel_eps) - q - r;
    proj_dists(2) = r + rel_eps;
    proj_dists(3) = rel_eps;

    phys_vec4_cmp mask = phys_vec4::get_zero().compare_less4(proj_dists);

    if (mask.all_are_set(phys_vec4_cmp::CMP_MASK_XYZ))
    {
        result.triangle_hit_direction.set_cross(Q, R);
        result.triangle_hit_direction.mul4(cache.m_values[3]);
        result.distance = result.triangle_hit_direction.dot3(rel_pos);
        if (result.distance > 0.0f)
        {
            result.triangle_hit_direction.set_neg4(result.triangle_hit_direction);
        }
        else
        {
            result.distance = -result.distance;
        }
        return TRI_HIT_FACE;
    }

    int index = mask_to_index[mask.get_mask(phys_vec4_cmp::CMP_MASK_XYZ)];
    if (index < 0)
    {
        index += 8;
        point_segment_result seg_result;
        closest_point_on_segment(position, tri_corners[vertex_to_edge_lut[index + 2]], tri_corners[vertex_to_edge_lut[index]], seg_result);
        result.triangle_hit_direction.set_sub4(position, seg_result.m_edge_point);
        result.distance = result.triangle_hit_direction.normalize_xyz_with_length();
    }
    else
    {
        point_segment_result seg_result_a;
        closest_point_on_segment(position, tri_corners[index], tri_corners[vertex_to_edge_lut[index + 2]], seg_result_a);
        point_segment_result seg_result_b;
        closest_point_on_segment(position, tri_corners[vertex_to_edge_lut[index]], tri_corners[index], seg_result_b);

        phys_vec4 t0; t0.set_sub4(position, seg_result_a.m_edge_point);
        phys_vec4 t1; t1.set_sub4(position, seg_result_b.m_edge_point);
        const float dist_sq_a = t0.length_sq_3d();
        const float dist_sq_b = t1.length_sq_3d();

        if (dist_sq_a < dist_sq_b)
        {
            float inv = 1.0f / __fsqrts(dist_sq_a);
            result.distance = dist_sq_a * inv;
            result.triangle_hit_direction.set_mul4(inv, t0);
        }
        else
        {
            float inv = 1.0f / __fsqrts(dist_sq_b);
            result.distance = dist_sq_b * inv;
            result.triangle_hit_direction.set_mul4(inv, t1);
        }
    }
    return TRI_HIT_EDGE;
}
