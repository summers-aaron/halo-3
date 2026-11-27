// hkinternal/hkPlaneEquationUtil.obj: from the records 0x6aac60 0x6aace8 0x6aae40 0x6ab030 0x6ab228 0x6ab490 0x6ab7b8 0x6abee8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// match: 0x6ab7b8: hkVector4 arrays require trivial construction.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"
// @address 0x6aac60 ?compute_intersection_line@phys_plane_equation_util@@SAXABV?$phys_array@Vphys_vec4@@@@HHHAAVphys_vec4@@@Z
// @address 0x6abee8 ??$quick_sort_recursive@Vphys_vec4@@P6A?AVphys_bool@@AAV1@0@Z@phys_algorithm@@YAXPAVphys_vec4@@HHP6A?AVphys_bool@@AAV1@1@Z@Z
#include "cseries/crt_xdk.h"
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_base.h"
#include "phys/phys_local_array.h"
#include "phys/phys_hull_builder.h"

#include "phys/phys_algorithm.h"

#include "phys/phys_algorithm_sort.h"

template void phys_algorithm::quick_sort_recursive<phys_vec4, phys_bool (*)(phys_vec4 &, phys_vec4 &)>(phys_vec4 *, int, int,
    phys_bool (*)(phys_vec4 &, phys_vec4 &));

#include "phys/phys_thread_memory.h"

#include "phys/phys_scalar_types.h"

#include "phys/phys_plane_equation_util.h"


// @address 0x6aace8 ?find_tri_plane_intersection@phys_plane_equation_util@@SA?AW4phys_result@@ABV?$phys_array@Vphys_vec4@@@@HHHAAUplane_intersection@1@@Z
phys_result phys_plane_equation_util::find_tri_plane_intersection(
    const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index, int plane_b_index, int plane_c_index, plane_intersection &out_intersect_point)
{
    phys_mat3 m;
    phys_vec4 &r0 = m.get_column(0);
    phys_vec4 &r1 = m.get_column(1);
    phys_vec4 &r2 = m.get_column(2);

    r0.set_cross(hull_plane_equations[plane_b_index], hull_plane_equations[plane_c_index]);
    r1.set_cross(hull_plane_equations[plane_c_index], hull_plane_equations[plane_a_index]);
    r2.set_cross(hull_plane_equations[plane_a_index], hull_plane_equations[plane_b_index]);

    const float determinant = r0.dot3(hull_plane_equations[plane_a_index]);
    if (phys_math::fabs(determinant) < 1.1920929e-07f)
    {
        return PHYS_FAILURE;
    }

    phys_vec4 offset;
    offset.set(-hull_plane_equations[plane_a_index](3), -hull_plane_equations[plane_b_index](3), -hull_plane_equations[plane_c_index](3), 0.0f);

    out_intersect_point.m_location.set_mul_mat3_vec3(m, offset);
    phys_simd_scalar inv_determinant(1.0f / determinant);
    out_intersect_point.m_location.mul4(inv_determinant);

    out_intersect_point.m_plane_a = plane_a_index;
    out_intersect_point.m_plane_b = plane_b_index;
    out_intersect_point.m_plane_c = plane_c_index;
    out_intersect_point.sort();

    return PHYS_SUCCESS;
}

// @address 0x6aae40 ?find_planar_intersections@phys_plane_equation_util@@SA?AW4phys_result@@ABVphys_vec4@@ABV?$phys_array@Vphys_vec4@@@@HHAAUplane_intersection@1@2@Z
phys_result phys_plane_equation_util::find_planar_intersections(const phys_vec4 &intersect_line_dir, const phys_array<phys_vec4> &hull_plane_equations,
    int plane_a_index, int plane_b_index, plane_intersection &upper_point_out, plane_intersection &lower_point_out)
{
    upper_point_out.m_plane_a = -1;
    upper_point_out.m_plane_b = -1;
    upper_point_out.m_plane_c = -1;
    lower_point_out.m_plane_a = -1;
    lower_point_out.m_plane_b = -1;
    lower_point_out.m_plane_c = -1;

    bool found_vertex = false;

    for (int i = 0; i < hull_plane_equations.getSize(); i++)
    {
        if ((i != plane_a_index) && (i != plane_b_index))
        {
            plane_intersection point;
            if (find_tri_plane_intersection(hull_plane_equations, plane_a_index, plane_b_index, i, point) == PHYS_SUCCESS)
            {
                if (!found_vertex)
                {
                    lower_point_out.m_location.set_add_mul4(point.m_location, intersect_line_dir, phys_simd_scalar(-100000.0f));
                    upper_point_out.m_location.set_neg4(lower_point_out.m_location);
                    found_vertex = true;
                }

                phys_vec4 lower_diff;
                lower_diff.set_sub4(lower_point_out.m_location, point.m_location);
                if (hull_plane_equations[i].dot3(lower_diff) > -1.1920929e-07f)
                {
                    lower_point_out = point;
                }

                phys_vec4 upper_diff;
                upper_diff.set_sub4(upper_point_out.m_location, point.m_location);
                if (hull_plane_equations[i].dot3(upper_diff) > -1.1920929e-07f)
                {
                    upper_point_out = point;
                }
            }
        }
    }

    if (upper_point_out.m_plane_a == -1)
    {
        if (lower_point_out.m_plane_a != -1)
        {
            upper_point_out.m_plane_a = lower_point_out.m_plane_a;
            upper_point_out.m_plane_b = lower_point_out.m_plane_b;
            upper_point_out.m_plane_c = lower_point_out.m_plane_c;
        }
        else
        {
            return PHYS_FAILURE;
        }
    }

    if (lower_point_out.m_plane_a == -1)
    {
        if (upper_point_out.m_plane_a != -1)
        {
            lower_point_out.m_plane_a = upper_point_out.m_plane_a;
            lower_point_out.m_plane_b = upper_point_out.m_plane_b;
            lower_point_out.m_plane_c = upper_point_out.m_plane_c;
        }
        else
        {
            return PHYS_FAILURE;
        }
    }

    return PHYS_SUCCESS;
}

// @address 0x6ab030 ?is_next_intersection@phys_plane_equation_util@@SA?AVphys_bool@@ABVphys_vec4@@ABV?$phys_array@Vphys_vec4@@@@HHAAV3@AAV?$phys_array@Uplane_intersection@phys_plane_equation_util@@@@@Z
phys_bool phys_plane_equation_util::is_next_intersection(const phys_vec4 &intersect_line_dir, const phys_array<phys_vec4> &hull_plane_equations,
    int plane_a_index, int plane_b_index, phys_vec4 &vertex, phys_array<plane_intersection> &capsule_points_out)
{
    bool found_vertex = false;
    int closest_plane_idx = -1;
    phys_real manifold_min_distance = 3.40282e+38f;

    for (int i = 0; i < hull_plane_equations.getSize(); i++)
    {
        if ((i != plane_a_index) && (i != plane_b_index))
        {
            const phys_real dot = hull_plane_equations[i].dot3(intersect_line_dir);
            phys_vec4 cross;
            cross.set_cross(hull_plane_equations[i], intersect_line_dir);
            const phys_real dir_length_sq = cross.length_sq_3d();

            if (dot > 1e-08f)
            {
                vertex(3) = 1.0f;
                phys_real distance = static_cast<float>(-hull_plane_equations[i].dot4_simd(vertex)) / phys_math::fabs(dot);
                const phys_real tolerance = 0.0001f;

                if (distance < manifold_min_distance + tolerance)
                {
                    plane_intersection point;
                    if (find_tri_plane_intersection(hull_plane_equations, plane_a_index, plane_b_index, i, point) == PHYS_SUCCESS)
                    {
                        found_vertex = true;
                        if (phys_math::fabs(distance - manifold_min_distance) < tolerance)
                        {
                            capsule_points_out.array_push_back(point);
                        }
                        else
                        {
                            capsule_points_out.clear();
                            capsule_points_out.array_push_back(point);
                            manifold_min_distance = distance;
                            closest_plane_idx = i;
                        }
                    }
                }
            }
        }
    }

    return found_vertex;
}

// @address 0x6ab228 ?walk_along_edge@phys_plane_equation_util@@SAXAAUvisited_edge@1@ABV?$phys_array@Vphys_vec4@@@@AAV?$phys_array@Uvisited_edge@phys_plane_equation_util@@@@AAV3@@Z
void phys_plane_equation_util::walk_along_edge(
    visited_edge &v, const phys_array<phys_vec4> &hull_plane_equations, phys_array<visited_edge> &visited_edges, phys_array<phys_vec4> &vertex_cloud_out)
{
    if (visited_edges.array_index_of(v) == -1)
    {
        visited_edges.array_push_back(v);

        phys_array<plane_intersection> points;
        if (is_next_intersection(v.m_intersect_line_dir, hull_plane_equations, v.m_plane_a, v.m_plane_b, v.m_from_point->m_location, points))
        {
            vertex_cloud_out.array_push_back(points[0].m_location);

            for (int i = 0; i < points.getSize(); i++)
            {
                visited_edge edge1;
                edge1.m_plane_a = points[i].m_plane_a;
                edge1.m_plane_b = points[i].m_plane_c;
                edge1.m_from_point = &points[i];
                compute_intersection_line(hull_plane_equations, points[i].m_plane_a, points[i].m_plane_c, points[i].m_plane_b, edge1.m_intersect_line_dir);
                walk_along_edge(edge1, hull_plane_equations, visited_edges, vertex_cloud_out);

                visited_edge edge2;
                edge2.m_plane_a = points[i].m_plane_b;
                edge2.m_plane_b = points[i].m_plane_c;
                edge2.m_from_point = &points[i];
                compute_intersection_line(hull_plane_equations, points[i].m_plane_b, points[i].m_plane_c, points[i].m_plane_a, edge2.m_intersect_line_dir);
                walk_along_edge(edge2, hull_plane_equations, visited_edges, vertex_cloud_out);
            }
        }
    }
}

// @address 0x6ab490 ?get_starting_point@phys_plane_equation_util@@SA?AW4phys_result@@ABV?$phys_array@Vphys_vec4@@@@AAH1AAUplane_intersection@1@2@Z
phys_result phys_plane_equation_util::get_starting_point(const phys_array<phys_vec4> &hull_plane_equations, int &near_parallel_plane_lo,
    int &near_parallel_plane_hi, plane_intersection &lower_point, plane_intersection &upper_point)
{
    bool found_hull_vertex = false;
    phys_inline_array<phys_plane_eqn_index_pair, 32> checked_pairs(0);

    while (!found_hull_vertex)
    {
        phys_plane_eqn_index_pair best_plane_pair;
        best_plane_pair.m_first = -1;
        best_plane_pair.m_second = -1;
        phys_real max_dot = -3.40282e+38f;

        for (int i = 0; i < hull_plane_equations.getSize(); i++)
        {
            for (int j = i + 1; j < hull_plane_equations.getSize(); j++)
            {
                const phys_real dot = hull_plane_equations[i].dot3(hull_plane_equations[j]);
                if ((dot > max_dot) && (dot < 0.9f))
                {
                    phys_plane_eqn_index_pair pair;
                    pair.m_first = i;
                    pair.m_second = j;
                    if (checked_pairs.array_index_of(pair) == -1)
                    {
                        max_dot = dot;
                        near_parallel_plane_lo = i;
                        near_parallel_plane_hi = j;
                        best_plane_pair.m_first = i;
                        best_plane_pair.m_second = j;
                    }
                }
            }
        }

        if (best_plane_pair.m_first == -1)
        {
            return PHYS_FAILURE;
        }

        checked_pairs.array_push_back(best_plane_pair);

        phys_vec4 intersect_line_dir;
        intersect_line_dir.set_cross(hull_plane_equations[near_parallel_plane_lo], hull_plane_equations[near_parallel_plane_hi]);

        if (find_planar_intersections(intersect_line_dir, hull_plane_equations, near_parallel_plane_lo, near_parallel_plane_hi, upper_point,
                lower_point) == PHYS_FAILURE)
        {
            return PHYS_FAILURE;
        }

        phys_vec4 diff;
        diff.set_sub4(upper_point.m_location, lower_point.m_location);
        if (intersect_line_dir.dot3(diff) > -1.1920929e-07f)
        {
            found_hull_vertex = true;
        }
    }

    return PHYS_SUCCESS;
}

inline void phys_plane_equation_util::compute_intersection_line(
    const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index, int plane_b_index, int plane_c_index, phys_vec4 &intersect_line_dir)
{
    intersect_line_dir.set_cross(hull_plane_equations[plane_a_index], hull_plane_equations[plane_b_index]);
    if (intersect_line_dir.dot4_xyz1(hull_plane_equations[plane_c_index]) > 0.0f)
        intersect_line_dir.set_neg4(intersect_line_dir);
}

// @address 0x6ab7b8 ?hull_verts_from_planes@phys_plane_equation_util@@SA?AW4phys_result@@ABV?$phys_array@Vphys_vec4@@@@AAV3@@Z
phys_result phys_plane_equation_util::hull_verts_from_planes(const phys_array<phys_vec4> &plane_eqns_in, phys_array<phys_vec4> &vertex_cloud_out)
{
    phys_local_array<phys_vec4> hull_plane_equations(plane_eqns_in.getSize());
    hull_plane_equations.set_size_unchecked(plane_eqns_in.getSize());
    for (int i = 0; i < hull_plane_equations.getSize(); i++)
    {
        hull_plane_equations[i] = plane_eqns_in[i];
    }
    phys_algorithm::quick_sort(hull_plane_equations.begin(), hull_plane_equations.getSize(), phys_hull_builder::vec_less_merge_coords);

    int near_parallel_planes[2];
    plane_intersection upper_point;
    plane_intersection lower_point;

    if (get_starting_point(hull_plane_equations, near_parallel_planes[0], near_parallel_planes[1], lower_point, upper_point) == PHYS_FAILURE)
    {
        return PHYS_FAILURE;
    }

    {
        phys_inline_array<visited_edge, 128> visited_edges;

        vertex_cloud_out.array_push_back(upper_point.m_location);
        vertex_cloud_out.array_push_back(lower_point.m_location);

        visited_edge first_hull_edge;
        first_hull_edge.m_plane_a = upper_point.m_plane_a;
        first_hull_edge.m_plane_b = upper_point.m_plane_b;
        visited_edges.push_back_unchecked(first_hull_edge);

        visited_edge edge1;
        edge1.m_plane_a = lower_point.m_plane_a;
        edge1.m_plane_b = lower_point.m_plane_c;
        edge1.m_from_point = &lower_point;
        compute_intersection_line(
            hull_plane_equations, lower_point.m_plane_a, lower_point.m_plane_c, lower_point.m_plane_b, edge1.m_intersect_line_dir);
        walk_along_edge(edge1, hull_plane_equations, visited_edges, vertex_cloud_out);

        visited_edge edge2;
        edge2.m_plane_a = lower_point.m_plane_b;
        edge2.m_plane_b = lower_point.m_plane_c;
        edge2.m_from_point = &lower_point;
        compute_intersection_line(
            hull_plane_equations, lower_point.m_plane_b, lower_point.m_plane_c, lower_point.m_plane_a, edge2.m_intersect_line_dir);
        walk_along_edge(edge2, hull_plane_equations, visited_edges, vertex_cloud_out);

        visited_edge third_edge;
        third_edge.m_plane_a = upper_point.m_plane_a;
        third_edge.m_plane_b = upper_point.m_plane_c;
        third_edge.m_from_point = &upper_point;
        compute_intersection_line(
            hull_plane_equations, upper_point.m_plane_a, upper_point.m_plane_c, upper_point.m_plane_b, third_edge.m_intersect_line_dir);
        walk_along_edge(third_edge, hull_plane_equations, visited_edges, vertex_cloud_out);

        visited_edge fourth_edge;
        fourth_edge.m_plane_a = upper_point.m_plane_b;
        fourth_edge.m_plane_b = upper_point.m_plane_c;
        fourth_edge.m_from_point = &upper_point;
        compute_intersection_line(
            hull_plane_equations, upper_point.m_plane_b, upper_point.m_plane_c, upper_point.m_plane_a, fourth_edge.m_intersect_line_dir);
        walk_along_edge(fourth_edge, hull_plane_equations, visited_edges, vertex_cloud_out);
    }

    {
        phys_vec4 *src = vertex_cloud_out.begin();
        phys_vec4 *dst = vertex_cloud_out.begin();
        const phys_real tolerance = 1e-5f;
        for (int i = vertex_cloud_out.getSize() - 1; i >= 0; i--)
        {
            phys_bool keep = true;
            for (int j = 0; j < hull_plane_equations.getSize(); j++)
            {
                const phys_real distance = hull_plane_equations[j].dot4_xyz1(*src);
                if (distance > tolerance)
                {
                    keep = false;
                    break;
                }
            }
            if (keep)
            {
                *(dst++) = *src;
            }
            src++;
        }
        vertex_cloud_out.set_array_size(int(dst - vertex_cloud_out.begin()));
    }

    phys_algorithm::quick_sort(vertex_cloud_out.begin(), vertex_cloud_out.getSize(), phys_hull_builder::vec_less_merge_coords);
    {
        int hull_vertex_count;
        phys_hull_builder::weld_x_sorted_vertices(2e-5f, vertex_cloud_out, hull_vertex_count);
    }

    return PHYS_SUCCESS;
}
