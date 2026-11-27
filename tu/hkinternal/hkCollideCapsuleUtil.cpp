// hkinternal/hkCollideCapsuleUtil.obj: from the records 0x693588 0x693b20 0x693c68 0x693eb8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// match: 0x693588, 0x693eb8: omit constructors to preserve vector expression evaluation.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_collide_capsule_util.h"
#include "phys/phys_math.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_vec4_util.h"

#include "phys/phys_algorithm.h"
#define PHYS_REAL_EPSILON 1.192092896e-07f

// @address 0x693588 phys_capsule_util_manifold_caps_vs_caps
extern "C" void phys_capsule_util_manifold_caps_vs_caps(
    const phys_vec4 *caps_a, float caps_a_radius, const phys_vec4 *caps_b_points, float caps_b_radius, phys_contact_point *capsule_points_out)
{
    const phys_vec4 *caps_b = caps_b_points;
    phys_vec4 dA; dA.set_sub4(caps_a[1], caps_a[0]);
    phys_vec4 dB; dB.set_sub4(caps_b[1], caps_b[0]);

    float R = dA.dot3(dB);
    phys_vec4 caps_a_points[2];
    if (R < 0.0f)
    {
        R = -R;
        dA.set_neg4(dA);
        caps_a_points[0] = caps_a[1];
        caps_a_points[1] = caps_a[0];
    }
    else
    {
        caps_a_points[0] = caps_a[0];
        caps_a_points[1] = caps_a[1];
    }

    const phys_vec4 &A = caps_a_points[0];
    const phys_vec4 &B = caps_b[0];

    float box_denominator;
    float D1;
    float D2;
    float t;
    float u;
    float S1;
    float S2;

    int return_value;
    {
        phys_vec4 d12; d12.set_sub4(B, A);

        S1 = dA.dot3(d12);
        S2 = dB.dot3(d12);

        D1 = dA.length_sq_3d();
        D2 = dB.length_sq_3d();

        box_denominator = phys_math::fabs(D1 * D2 - R * R);
        float float_epsilon = (phys_math::fabs(D1 * D2) + R * R) * (PHYS_REAL_EPSILON * 8.0f);

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
                if (box_denominator > float_epsilon)
                {
                    t = t / box_denominator;
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
                u = u / D2;
                break;
            }
        }

        {
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
                t = t / D1;
            }
        }
        } while (false);
    }

    phys_vec4 a; a.set_add_mul4(A, dA, t);
    phys_vec4 b; b.set_add_mul4(B, dB, u);
    phys_vec4 a_minus_b; a_minus_b.set_sub4(a, b);

    {
        phys_contact_point &capsule_closest_point = capsule_points_out[0];

        const float capsule_radius_sum = caps_a_radius + caps_b_radius;
        const float ref_dist = phys_math::max_of_two(PHYS_REAL_EPSILON, capsule_radius_sum + capsule_closest_point.get_distance());

        const float bvh_dist_sq = a_minus_b.length_sq_3d();
        if (bvh_dist_sq < ref_dist * ref_dist)
        {
            capsule_closest_point.get_separating_normal() = a_minus_b;
            float dist;
            if (bvh_dist_sq < PHYS_REAL_EPSILON)
            {
                dist = 0.0f;
                {
                    // match: 0x693588: perpendicular through the float overload of fabs; widening to double changes codegen.
                    phys_vec4 &simplex_separating_normal = capsule_closest_point.get_separating_normal();
                    int min = 0;
                    int ok_flag1 = 1;
                    int ok_flag2 = 2;
                    float a0 = ::fabs(dA(0));
                    float a1 = ::fabs(dA(1));
                    float a2 = ::fabs(dA(2));
                    if (a1 < a0) { ok_flag1 = 0; min = 1; a0 = a1; }
                    if (a2 < a0) { ok_flag2 = min; min = 2; }
                    simplex_separating_normal.set_zero4();
                    simplex_separating_normal(ok_flag1) = dA(ok_flag2);
                    simplex_separating_normal(ok_flag2) = -dA(ok_flag1);
                }
                capsule_closest_point.get_separating_normal().normalize_xyz();
            }
            else
            {
                float inv_dist = phys_math::inv_sqrt(bvh_dist_sq);
                dist = inv_dist * bvh_dist_sq;
                capsule_closest_point.get_separating_normal().mul4(inv_dist);
            }
            capsule_closest_point.get_contact_position().set_add_mul4(a, capsule_closest_point.get_contact_normal(), caps_b_radius - dist);
            capsule_closest_point.contact_set_distance(dist - capsule_radius_sum);
        }
        else
        {
            float inv_dist = phys_math::inv_sqrt(bvh_dist_sq);
            capsule_closest_point.get_separating_normal() = a_minus_b;
            float dist = inv_dist * bvh_dist_sq;
            capsule_closest_point.get_separating_normal().mul4(inv_dist);
            capsule_closest_point.contact_set_distance(dist - capsule_radius_sum);
            return;
        }
    }

    {
        const float parallel_tol = 0.2f;
        if (box_denominator > parallel_tol * D1 * D2)
        {
            return;
        }
    }

    float inv_d1_d2 = 1.0f / (D1 * D2);
    float inv_d1 = D2 * inv_d1_d2;
    float inv_d2 = D1 * inv_d1_d2;

    {
        phys_contact_point *point = capsule_points_out + 1;
        phys_vec4 closest_on_a = caps_a_points[0];
        phys_vec4 closest_on_b = caps_b[0];
        float param_a_on_b = -S2;
        float param_b_on_a = S1;
        int i = 0;
        int mask = 10;
        while (1)
        {
            if ((return_value & mask) == 0)
            {
                do
                {
                {
                    if (param_a_on_b > 0.0f)
                    {
                        if (param_a_on_b > D2)
                        {
                            break;
                        }

                        if (param_b_on_a > 0.0f)
                        {
                            if (param_b_on_a > D1)
                            {
                                break;
                            }

                            if (param_a_on_b * param_a_on_b * inv_d2 > param_b_on_a * param_b_on_a * inv_d1)
                            {
                                closest_on_a.add_scaled4(param_b_on_a * inv_d1, dA);
                            }
                            else
                            {
                                closest_on_b.add_scaled4(param_a_on_b * inv_d2, dB);
                            }
                        }
                        else
                        {
                            closest_on_b.add_scaled4(param_a_on_b * inv_d2, dB);
                        }
                    }
                    else
                    {
                        if (param_b_on_a > 0.0f)
                        {
                            if (param_b_on_a > D1)
                            {
                                break;
                            }
                            closest_on_a.add_scaled4(param_b_on_a * inv_d1, dA);
                        }
                    }
                }

                phys_vec4 a_minus_b; a_minus_b.set_sub4(closest_on_a, closest_on_b);
                phys_contact_point &capsule_closest_point = *point;

                const float capsule_radius_sum = caps_a_radius + caps_b_radius;
                const float ref_dist = capsule_radius_sum + capsule_closest_point.get_distance();

                const float bvh_dist_sq = a_minus_b.length_sq_3d();
                if (bvh_dist_sq < ref_dist * ref_dist)
                {
                    const float dist = phys_math::sqrt(bvh_dist_sq);
                    capsule_closest_point.get_separating_normal() = a_minus_b;
                    if (bvh_dist_sq > PHYS_REAL_EPSILON)
                    {
                        capsule_closest_point.get_separating_normal().normalize_xyz();
                    }
                    else
                    {
                        capsule_closest_point.get_separating_normal() = capsule_points_out->get_separating_normal();
                    }

                    capsule_closest_point.get_contact_position().set_add_mul4(closest_on_a, capsule_closest_point.get_contact_normal(), caps_b_radius - dist);
                    capsule_closest_point.contact_set_distance(dist - capsule_radius_sum);
                }
                } while (false);
            }
            if (i == 1)
            {
                break;
            }

            closest_on_a = caps_a_points[1];
            closest_on_b = caps_b[1];
            param_a_on_b = D2 - param_a_on_b - R;
            param_b_on_a = D1 - param_b_on_a - R;
            dA.set_neg4(dA);
            dB.set_neg4(dB);
            point++;
            i++;
            mask = 5;
        }
    }
}

// @address 0x693b20 ?phys_capsule_util_tri_plane_dirs@@YAXPBVphys_vec4@@ABUpoint_tri_distance_cache@phys_collide_triangle_util@@AAVphys_transform@@AAV1@@Z
void phys_capsule_util_tri_plane_dirs(const phys_vec4 *tri_corners, const phys_collide_triangle_util::point_tri_distance_cache &cache,
    phys_transform &plane_equations_out, phys_vec4 &normal_out)
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

    phys_algorithm::swap(plane_equations_out(0, 1), plane_equations_out(1, 0));
    phys_algorithm::swap(plane_equations_out(0, 2), plane_equations_out(2, 0));
    phys_algorithm::swap(plane_equations_out(1, 2), plane_equations_out(2, 1));

    plane_equations_out(3, 0) = normal_out(0);
    plane_equations_out(3, 1) = normal_out(1);
    plane_equations_out(3, 2) = normal_out(2);

    plane_equations_out.get_column(3).set_zero4();
    plane_equations_out(0, 3) = -(cache.m_normal_len * cache.m_inv_edge_len[0]);
}

// @address 0x693c68 phys_capsule_util_closest_caps_vs_caps
extern "C" phys_result phys_capsule_util_closest_caps_vs_caps(
    const phys_vec4 *caps_a, float caps_a_radius, const phys_vec4 *caps_b, float caps_b_radius, float capsule_tri_tolerance,
        phys_contact_point &capsule_closest_point)
{
    phys_vec4 dA; dA.set_sub4(caps_a[1], caps_a[0]);
    phys_vec4 dB; dB.set_sub4(caps_b[1], caps_b[0]);

    phys_collide_triangle_util::segment_segment_result result;
    int line_seg_result = phys_collide_triangle_util::closest_segment_pair(caps_a[0], dA, caps_b[0], dB, result);

    float capsule_radius_sum = caps_a_radius + caps_b_radius;
    float ref_dist = capsule_tri_tolerance + capsule_radius_sum;
    if (result.m_dist_sq > ref_dist * ref_dist)
    {
        return PHYS_FAILURE;
    }

    float dist = __fsqrts(result.m_dist_sq);

    while (1)
    {
        if (line_seg_result)
        {
            if (result.m_dist_sq > 0.0f)
            {
                capsule_closest_point.get_separating_normal() = result.m_closest_a_minus_b;
                break;
            }
        }
        else
        {
            phys_vec4 alt_normal; alt_normal.set_cross(dA, dB);
            float alt_normal_len_sq = alt_normal.length_sq_3d();
            if (alt_normal_len_sq > 1.1920929e-07f)
            {
                float dot = alt_normal.dot3(result.m_closest_a_minus_b);
                if (dot < 0.0f)
                {
                    alt_normal.set_neg4(alt_normal);
                }
                capsule_closest_point.get_separating_normal() = alt_normal;
                break;
            }
        }
        {
            // match: 0x693c68: perpendicular through the float overload of fabs; widening to double changes codegen.
            phys_vec4 &simplex_separating_normal = capsule_closest_point.get_separating_normal();
            int min = 0;
            int ok_flag1 = 1;
            int ok_flag2 = 2;
            float a0 = ::fabs(dA(0));
            float a1 = ::fabs(dA(1));
            float a2 = ::fabs(dA(2));
            if (a1 < a0) { ok_flag1 = 0; min = 1; a0 = a1; }
            if (a2 < a0) { ok_flag2 = min; min = 2; }
            simplex_separating_normal.set_zero4();
            simplex_separating_normal(ok_flag1) = dA(ok_flag2);
            simplex_separating_normal(ok_flag2) = -dA(ok_flag1);
        }
        break;
    }

    capsule_closest_point.get_separating_normal().normalize_xyz();
    capsule_closest_point.get_contact_position().set_add_mul4(result.m_closest_point_a, capsule_closest_point.get_contact_normal(), caps_b_radius - dist);
    capsule_closest_point.contact_set_distance(dist - capsule_radius_sum);
    return PHYS_SUCCESS;
}

static __forceinline void phys_capsule_util_tri_plane_dirs_inline(const phys_vec4 *tri_corners,
    const phys_collide_triangle_util::point_tri_distance_cache &cache, phys_transform &plane_equations_out, phys_vec4 &normal_out)
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
    phys_algorithm::swap(plane_equations_out(0, 1), plane_equations_out(1, 0));
    phys_algorithm::swap(plane_equations_out(0, 2), plane_equations_out(2, 0));
    phys_algorithm::swap(plane_equations_out(1, 2), plane_equations_out(2, 1));
    plane_equations_out(3, 0) = normal_out(0);
    plane_equations_out(3, 1) = normal_out(1);
    plane_equations_out(3, 2) = normal_out(2);
    plane_equations_out.get_column(3).set_zero4();
    plane_equations_out(0, 3) = -(cache.m_normal_len * cache.m_inv_edge_len[0]);
}

// @address 0x693eb8 phys_capsule_util_caps_vs_tri
extern "C" void phys_capsule_util_caps_vs_tri(const phys_vec4 *caps_a_points, float caps_a_radius, const phys_vec4 *tri_vertices, float triangle_b_radius,
    const phys_collide_triangle_util::point_tri_distance_cache &cache, float capsule_tri_tolerance, int search_manifold, phys_contact_point *points)
{
    phys_transform plane_dirs;
    phys_vec4 normal;
    phys_capsule_util_tri_plane_dirs_inline(tri_vertices, cache, plane_dirs, normal);
    phys_vec4 distances[2];
    distances[0].set_sub4(caps_a_points[0], tri_vertices[0]);
    distances[1].set_sub4(caps_a_points[1], tri_vertices[0]);
    phys_vec4_util::transform_points4(plane_dirs, distances, 2, distances);
    const float capsule_radius_sum = caps_a_radius + triangle_b_radius;
    const float check_radius = capsule_radius_sum + capsule_tri_tolerance;
    {
        const float d = 3.40282e38f;
        points[0].contact_set_distance(d);
        points[1].contact_set_distance(d);
        points[2].contact_set_distance(d);
    }
    {
        phys_vec4 check_radius_vec; check_radius_vec.flags_set_all(check_radius);
        // Test both endpoints against the same triangle plane.
        phys_vec4_cmp both_outside = check_radius_vec.compare_less4(distances[0]);
        both_outside.set_and(both_outside, check_radius_vec.compare_less4(distances[1]));
        if (both_outside.any_is_set() || (distances[0](3) < -check_radius && distances[1](3) < -check_radius)) { return; }
    }
    int mask[2];
    mask[0] = distances[0].compare_less_zero4().get_mask();
    mask[1] = distances[1].compare_less_zero4().get_mask();
    if ((mask[0] ^ mask[1]) & phys_vec4_cmp::CMP_MASK_W)
    {
        float f = distances[0](3) / (distances[0](3) - distances[1](3));
        phys_vec4 colliding_dist; colliding_dist.set_lerp4(distances[0], distances[1], f);
        if (colliding_dist.all_less_than3(phys_vec4::get_zero()))
        {
            phys_contact_point *point = &points[1];
            point->contact_set_distance(-3.40282e38f);
            for (int i = 0; i < 2; i++)
            {
                if ((mask[i] & phys_vec4_cmp::CMP_MASK_XYZ) == phys_vec4_cmp::CMP_MASK_XYZ)
                {
                    // match: 0x693eb8: the float overload of fabs (March hkMath::fabs calls fabsf), see above
                    const float dist = -::fabs(distances[i](3)) - capsule_radius_sum;
                    if (dist > point->get_distance())
                    {
                        if (mask[i] & phys_vec4_cmp::CMP_MASK_W)
                        {
                            point->get_separating_normal() = normal;
                            point->get_contact_position().set_add_mul4(caps_a_points[i], normal, triangle_b_radius - distances[i](3));
                        }
                        else
                        {
                            point->get_separating_normal().set_neg4(normal);
                            point->get_contact_position().set_add_mul4(caps_a_points[i], normal, -triangle_b_radius - distances[i](3));
                        }
                        point->contact_set_distance(dist);
                    }
                }
            }
            phys_vec4 dir; dir.set_sub4(distances[1], distances[0]);
            for (int e = 0; e < 3; e++)
            {
                float dir_w_comp = dir(3);
                float dir_edge = dir(e);
                float len_sq = dir_w_comp * dir_w_comp + dir_edge * dir_edge;
                float inv_len_sq = 1.0f / len_sq;
                float dist = dir_edge * distances[0](3) - dir_w_comp * distances[0](e);
                float bvh_dist_sq = dist * dist * inv_len_sq;
                float contact_dist = point->get_distance() + capsule_radius_sum;
                if (bvh_dist_sq < contact_dist * contact_dist)
                {
                    float project = dir_edge * distances[0](e) + dir_w_comp * distances[0](3);
                    project *= -inv_len_sq;
                    if (project > -.0001f && project < 1.0001f)
                    {
                        phys_vec4 tri_edge_dir; plane_dirs.get_rotation().get_row(e, tri_edge_dir);
                        if (dir_w_comp < 0.0f) { dir_w_comp = -dir_w_comp; dir_edge = -dir_edge; }
                        tri_edge_dir.mul4(dir_w_comp);
                        point->get_separating_normal().set_add_mul4(tri_edge_dir, normal, -dir_edge);
                        point->get_separating_normal().normalize_xyz();
                        phys_vec4 pos; pos.set_lerp4(caps_a_points[0], caps_a_points[1], project);
                        float d = -phys_math::sqrt(bvh_dist_sq);
                        point->get_contact_position().set_add_mul4(pos, point->get_contact_normal(), triangle_b_radius - d);
                        point->contact_set_distance(d - capsule_radius_sum);
                    }
                }
            }
            return;
        }
    }
    int i = 0;
    phys_vec4 caps_a_seg_dir; caps_a_seg_dir.set_sub4(caps_a_points[1], caps_a_points[0]);
    phys_contact_point *point = points;
    bool check_single = false;
    if ((phys_vec4_cmp::CMP_MASK_XYZ & (mask[0] | mask[1])) != phys_vec4_cmp::CMP_MASK_XYZ)
    {
        if (search_manifold)
        {
            int combined_mask = mask[0] | mask[1];
            float d = capsule_tri_tolerance;
            points[0].contact_set_distance(d);
            points[1].contact_set_distance(d);
            points[2].contact_set_distance(d);
            phys_contact_point prev_contact;
            prev_contact.get_separating_normal()(3) = 3.40282e38f;
            phys_vec4 pB[2];
            int mask_j_bit = phys_vec4_cmp::CMP_MASK_X;
            for (int j = 0; j < 3; mask_j_bit >>= 1, j++)
            {
                if (mask_j_bit & combined_mask) { continue; }
                pB[0] = tri_vertices[phys_collide_triangle_util::next_mod3(j)];
                pB[1] = tri_vertices[phys_collide_triangle_util::get_prev_modulo3(j)];
                phys_capsule_util_manifold_caps_vs_caps(caps_a_points, caps_a_radius, pB, triangle_b_radius, points);
                if (points[0].get_separating_normal()(3) > prev_contact.get_separating_normal()(3)) { points[0] = prev_contact; }
                else { prev_contact = points[0]; }
            }
            return;
        }
        else { check_single = true; goto check_single_second_point; }
    }
    for (; i < 2; point++, i++)
    {
        if ((mask[i] & phys_vec4_cmp::CMP_MASK_XYZ) == phys_vec4_cmp::CMP_MASK_XYZ)
        {
            if (mask[i] & phys_vec4_cmp::CMP_MASK_W)
            {
                point->get_contact_position().set_add_mul4(caps_a_points[i], normal, -triangle_b_radius - distances[i](3));
                point->get_separating_normal().set_neg4(normal);
                point->contact_set_distance(-distances[i](3) - capsule_radius_sum);
            }
            else
            {
                point->get_contact_position().set_add_mul4(caps_a_points[i], normal, triangle_b_radius - distances[i](3));
                point->get_separating_normal() = normal;
                point->contact_set_distance(distances[i](3) - capsule_radius_sum);
            }
        }
        else
        {
check_single_second_point:
            int m = phys_vec4_cmp::CMP_MASK_X;
            for (int e = 0; e < 3; m >>= 1, e++)
            {
                if (!check_single) { if (mask[i] & m) { continue; } }
                else { if (mask[0] & mask[1] & m) { continue; } }
                int n = phys_collide_triangle_util::next_mod3(e);
                int nn = phys_collide_triangle_util::get_prev_modulo3(e);
                const phys_vec4 &P = tri_vertices[n];
                phys_vec4 edge; edge.set_sub4(tri_vertices[nn], P);
                phys_collide_triangle_util::segment_segment_result result;
                int which_points = phys_collide_triangle_util::closest_segment_pair(caps_a_points[0], caps_a_seg_dir, P, edge, result);
                const float ref_dist = point->get_distance() + capsule_radius_sum;
                if (result.m_dist_sq < ref_dist * ref_dist)
                {
                    if (which_points & (1 << i))
                    {
                        if (check_single) { i = 1; }
                        else { continue; }
                    }
                    phys_vec4 capsule_edge_normal;
                    if (which_points == 0) { capsule_edge_normal.set_cross(caps_a_seg_dir, edge); }
                    else
                    {
                        const float eps_squared = PHYS_REAL_EPSILON * PHYS_REAL_EPSILON;
                        if (result.m_dist_sq <= eps_squared)
                        {
                            plane_dirs.get_rotation().get_row(e, result.m_closest_a_minus_b);
                            capsule_edge_normal.set_cross(caps_a_seg_dir, edge);
                            if (capsule_edge_normal.length_sq_3d() <= eps_squared) { capsule_edge_normal = result.m_closest_a_minus_b; }
                        }
                        else { capsule_edge_normal = result.m_closest_a_minus_b; }
                    }
                    capsule_edge_normal.normalize_xyz();
                    float dist = capsule_edge_normal.dot3(result.m_closest_a_minus_b);
                    if (dist < 0.0f) { dist = -dist; capsule_edge_normal.set_neg4(capsule_edge_normal); }
                    point->get_contact_position().set_add_mul4(result.m_closest_point_a, capsule_edge_normal, triangle_b_radius - dist);
                    point->get_separating_normal() = capsule_edge_normal;
                    point->contact_set_distance(dist - capsule_radius_sum);
                    continue;
                }
            }
        }
        if (check_single) { return; }
    }
}
