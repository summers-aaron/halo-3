// hkinternal/hkGeomConvexHullBuilder.obj: from the records 0x6ac038 0x6ac078 0x6ac080 0x6ac3e8 0x6ac408 0x6ac428 0x6ac520 0x6ac528 0x6ac688 0x6ac708 0x6ac778 0x6ac7c0 0x6ac808 0x6ac9d8 0x6acbf8 0x6ace28 0x6acf58 0x6ad468 0x6ad5c8 0x6ad780 0x6ad9c0 0x6add70 0x6ae030 0x6ae7d8 0x6aecc8 0x6aefb0 0x6af768 0x6b0428 0x6b05a8 0x6b07b8 0x6b08a0 0x6b0930 0x6b0bb8 0x6b1130 0x6b12e8 0x6b1430 0x6b1598 0x6b1a50 0x6b1ba8 0x6b1d60 0x6b26e8 0x6b2c18 0x6b2e08 0x6b31f8 0x6b3348 0x6b3428
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x6ac808 ?weld_x_sorted_vertices@phys_hull_builder@@SAXMAAV?$phys_array@Vphys_vec4@@@@AAH@Z
// @address 0x6ae030 ?generate_hull_from_planar_points@phys_hull_builder@@SAXABVphys_vec4@@PBV2@HAAV?$phys_array@Vphys_vec4@@@@2@Z
// @address 0x6ae7d8 ?approx_hull_from_kdop@phys_hull_builder@@SAXPBVphys_vec4@@HAAV?$phys_array@Vphys_vec4@@@@1@Z
// @address 0x6aefb0 ?post_filter_vertices@phys_hull_builder@@SAXAAVphys_geom_hull@@HHABUphys_hull_tolerances@@AAVphys_bool@@@Z
// @address 0x6af768 ?build_plane_equations@phys_hull_builder@@SA?AVphys_bool@@ABUphys_hull_tolerances@@AAVphys_geom_hull@@ABV?$phys_array@Vphys_vec4@@@@AAVphys_vec4@@AAV2@AAV5@AAV?$phys_array@Vplane_and_points@phys_hull_builder@@@@@Z
// @address 0x6b12e8 ?validate_neighbours@phys_hull_builder@@SAXABUphys_hull_tolerances@@PBVphys_vec4@@ABV3@GPAUweighted_line@1@11AAV?$phys_array@Uweighted_neighbour@phys_hull_builder@@@@4@Z
// @address 0x6b1a50 ?find_weighted_neighbours@phys_hull_builder@@SAXABUphys_hull_tolerances@@AAVphys_geom_hull@@ABVphys_vec4@@GPBVphys_geom_edge@@PBV4@4AAV?$phys_array@Uweighted_neighbour@phys_hull_builder@@@@@Z
// @address 0x6b26e8 ?build_convex_sub_hull@phys_hull_builder@@SAXABUphys_hull_tolerances@@AAV?$phys_array@Vphys_vec4@@@@HHAAVphys_geom_hull@@@Z
// @address 0x6b2c18 ?build_convex_hull@phys_hull_builder@@SAXABUphys_hull_tolerances@@PBVphys_vec4@@HAAVphys_geom_hull@@AAV?$phys_array@Vphys_vec4@@@@@Z
#include "phys/phys_base_types.h"
#include "physics/havok_memory.h"
#include "phys/phys_math.h"
#include "phys/phys_strided_vertices.h"
#include "phys/phys_string.h"
#include "phys/phys_memory_class.h"
// @address 0x6ac038 ?get_pseudo_angle@phys_hull_builder@@SAMMM@Z
// @address 0x6ac078 ?draw_plane@phys_hull_builder@@SAXABVphys_vec4@@0H@Z
// @address 0x6ac080 ?find_edge_pair@plane_and_points@phys_hull_builder@@SA?AVphys_bool@@PAVphys_geom_edge@@0000AAV12@1@Z
// @address 0x6ac3e8 ?weighted_neighbour_less@@YA?AVphys_bool@@ABUweighted_neighbour@phys_hull_builder@@0@Z
// @address 0x6ac408 ?weighted_line_less@@YA?AVphys_bool@@ABUweighted_line@phys_hull_builder@@0@Z
// @address 0x6ac428 ?get_handle_aabb@phys_hull_builder@@SAXABV?$phys_array@Vphys_vec4@@@@AAVphys_aabb@@@Z
// @address 0x6ac520 ?print_hull_edges@phys_hull_builder@@SAXAAVphys_geom_hull@@@Z
// @address 0x6ac528 ?convert_from_unit_cube@phys_hull_builder@@SAXAAV?$phys_array@Vphys_vec4@@@@AAVphys_vec4@@1@Z
// @address 0x6ac688 ?sort@plane_and_points@phys_hull_builder@@QAAXXZ
// @address 0x6ac778 ?vec4_add@@YAXABVphys_vec4@@0AAV1@@Z
// @address 0x6ac7c0 ?vec4_sub@@YAXABVphys_vec4@@0AAV1@@Z
// @address 0x6ac9d8 ?make_bevel_plane@phys_hull_builder@@SAXABVphys_vec4@@000AAV?$phys_array@Vphys_vec4@@@@@Z
// @address 0x6acbf8 ?convert_to_unit_cube@phys_hull_builder@@SAXAAV?$phys_array@Vphys_vec4@@@@AAVphys_vec4@@1@Z
// @address 0x6aecc8 ?remove_collinear_vertices@phys_hull_builder@@SAXAAV?$phys_array@Vphys_vec4@@@@M@Z
// @address 0x6b0428 ?append_reindex_edges@phys_hull_builder@@SAXAAVphys_geom_hull@@Vphys_bool@@0AAV?$phys_array@G@@@Z
// @address 0x6b07b8 ?build_display_geometry@phys_hull_builder@@SAXAAVphys_geom_hull@@AAUphys_geometry@@@Z
// @address 0x6b1430 ?is_edge_check_valid@phys_hull_builder@@SA?AVphys_bool@@AAVphys_geom_hull@@PAVphys_geom_edge@@H@Z
// @address 0x6b1598 ?is_valid_wrapping@phys_hull_builder@@SA?AVphys_bool@@AAVphys_geom_hull@@PAVphys_geom_edge@@V2@AAV?$phys_array@Uwrapping_line@phys_hull_builder@@@@@Z
// @address 0x6b1ba8 ?find_wrapping@phys_hull_builder@@SAXAAVphys_geom_hull@@0ABV?$phys_array@Uweighted_line@phys_hull_builder@@@@AAV?$phys_array@Uwrapping_line@phys_hull_builder@@@@@Z
// @address 0x6b31f8 ??$quick_sort_recursive@Uweighted_neighbour@phys_hull_builder@@P6A?AVphys_bool@@ABU12@0@Z@phys_algorithm@@YAXPAUweighted_neighbour@phys_hull_builder@@HHP6A?AVphys_bool@@ABU12@1@Z@Z
// @address 0x6b3348 ??4?$phys_inline_array@Vphys_geom_edge@@$0IA@@@QAAAAV?$phys_array@Vphys_geom_edge@@@@ABV0@@Z
// @address 0x6ac708 ?vec_less_merge_coords@phys_hull_builder@@SA?AVphys_bool@@AAVphys_vec4@@0@Z
// @address 0x6b08a0 ?build_plane_equations@phys_hull_builder@@SA?AVphys_bool@@ABUphys_hull_tolerances@@AAVphys_geom_hull@@ABV?$phys_array@Vphys_vec4@@@@AAV5@@Z
// @address 0x6b1130 ?remove_coplanar_neighbours@phys_hull_builder@@SAXABUphys_hull_tolerances@@AAVphys_geom_hull@@ABVphys_vec4@@PBVphys_geom_edge@@PBV4@4AAV?$phys_array@Uweighted_neighbour@phys_hull_builder@@@@@Z
// @address 0x6b3428 ??_G?$phys_inline_array@Uweighted_line@phys_hull_builder@@$0BA@@@QAAPAXI@Z
#include "phys/phys_base.h"
#include "phys/phys_local_array.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_geom_edge.h"
#include "phys/phys_geom_hull.h"
#include "phys/phys_geometry.h"
// match: 0x6ae7d8: retain the implicit destructor's omitted vtable store in this unit.
class __declspec(novtable) phys_convex_hull_shape;
#include "phys/phys_convex_hull_shape.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_algorithm_sort.h"
#include "phys/phys_hull_tolerances.h"
#include "phys/phys_hull_builder.h"
#include "phys/phys_hull_tester.h"
// match: 0x6ae7d8: retain the empty destructor body here so stack-shape cleanup remains inline.
inline phys_convex_hull_shape::~phys_convex_hull_shape()
{
}


namespace {
    template <typename T> __forceinline void swap_values(T &a, T &b) { T value = a; a = b; b = value; }
}

// 0x6ac080
phys_bool phys_hull_builder::plane_and_points::find_edge_pair(phys_geom_edge *base, phys_geom_edge *first_edge_a,
    phys_geom_edge *first_edge_b, phys_geom_edge *second_a,
    phys_geom_edge *second_b, plane_and_points &first, plane_and_points &second)
{
    phys_bool first_mirror_pair = first_edge_a != first_edge_b && first_edge_a->m_edge_vertex == first_edge_b->m_edge_vertex &&
        first_edge_a->get_mirror_edge(base)->m_edge_vertex != first_edge_b->get_mirror_edge(base)->m_edge_vertex;
    phys_bool second_mirror_pair = second_a != second_b && second_a->m_edge_vertex == second_b->m_edge_vertex &&
        second_a->get_mirror_edge(base)->m_edge_vertex != second_b->get_mirror_edge(base)->m_edge_vertex;
    phys_bool result = first_mirror_pair && second_mirror_pair;
    if (result) {
        phys_bool second_dir_compatible = true;
        second_dir_compatible = second_dir_compatible && first_edge_a != second_b && first_edge_a->m_edge_vertex == second_b->m_edge_vertex
            ? first_edge_a->get_mirror_edge(base)->m_edge_vertex != second_b->get_mirror_edge(base)->m_edge_vertex
            : true;
        second_dir_compatible = second_dir_compatible && second_a != second_b && second_a->m_edge_vertex == second_b->m_edge_vertex
            ? second_a->get_mirror_edge(base)->m_edge_vertex != second_b->get_mirror_edge(base)->m_edge_vertex
            : true;
        second_dir_compatible = second_dir_compatible && first_edge_a->get_mirror_edge(base)->m_edge_vertex == second_b->m_edge_vertex &&
                second_b->get_mirror_edge(base)->m_edge_vertex == first_edge_a->m_edge_vertex
            ? first_edge_a->get_mirror_edge(base) == second_b
            : true;
        second_dir_compatible = second_dir_compatible && second_a->get_mirror_edge(base)->m_edge_vertex == second_b->m_edge_vertex &&
                second_b->get_mirror_edge(base)->m_edge_vertex == second_a->m_edge_vertex
            ? second_a->get_mirror_edge(base) == second_b
            : true;
        phys_bool first_dir_compatible = true;
        first_dir_compatible = first_dir_compatible && first_edge_a != first_edge_b && first_edge_a->m_edge_vertex == first_edge_b->m_edge_vertex
            ? first_edge_a->get_mirror_edge(base)->m_edge_vertex != first_edge_b->get_mirror_edge(base)->m_edge_vertex
            : true;
        first_dir_compatible = first_dir_compatible && second_a != first_edge_b && second_a->m_edge_vertex == first_edge_b->m_edge_vertex
            ? second_a->get_mirror_edge(base)->m_edge_vertex != first_edge_b->get_mirror_edge(base)->m_edge_vertex
            : true;
        first_dir_compatible = first_dir_compatible && first_edge_a->get_mirror_edge(base)->m_edge_vertex == first_edge_b->m_edge_vertex &&
                first_edge_b->get_mirror_edge(base)->m_edge_vertex == first_edge_a->m_edge_vertex
            ? first_edge_a->get_mirror_edge(base) == first_edge_b
            : true;
        first_dir_compatible = first_dir_compatible && second_a->get_mirror_edge(base)->m_edge_vertex == first_edge_b->m_edge_vertex &&
                first_edge_b->get_mirror_edge(base)->m_edge_vertex == second_a->m_edge_vertex
            ? second_a->get_mirror_edge(base) == first_edge_b
            : true;
        result = second_dir_compatible && first_dir_compatible;
    }
    return result;
}

// 0x6ac3e8
phys_bool weighted_neighbour_less(const phys_hull_builder::weighted_neighbour &first, const phys_hull_builder::weighted_neighbour &second)
{
    return phys_bool(first.m_neighbour_weight < second.m_neighbour_weight);
}
// 0x6ac408
phys_bool weighted_line_less(const phys_hull_builder::weighted_line &first, const phys_hull_builder::weighted_line &second)
{
    return phys_bool(first.m_neighbour_weight < second.m_neighbour_weight);
}
// 0x6ac428
void phys_hull_builder::get_handle_aabb(const phys_array<phys_vec4> &vertices, phys_aabb &aabb)
{
    if (vertices.getSize() > 0) {
        aabb.m_min_limit = vertices[0];
        aabb.m_max_limit = vertices[0];
        for (int index = 0; index < vertices.getSize(); ++index) {
            aabb.m_min_limit(0) = phys_math::min_of_two(aabb.m_min_limit(0), vertices[index](0));
            aabb.m_max_limit(0) = phys_math::max_of_two(aabb.m_max_limit(0), vertices[index](0));
            aabb.m_min_limit(1) = phys_math::min_of_two(aabb.m_min_limit(1), vertices[index](1));
            aabb.m_max_limit(1) = phys_math::max_of_two(aabb.m_max_limit(1), vertices[index](1));
            aabb.m_min_limit(2) = phys_math::min_of_two(aabb.m_min_limit(2), vertices[index](2));
            aabb.m_max_limit(2) = phys_math::max_of_two(aabb.m_max_limit(2), vertices[index](2));
        }
    }
}
// 0x6ac520
void phys_hull_builder::print_hull_edges(phys_geom_hull &geom_hull)
{
    for (int index = 0; index < geom_hull.m_hull_edges.getSize(); ++index) { }
}

// 0x6ac038
float phys_hull_builder::get_pseudo_angle(float cos_theta, float hull_sin_theta)
{
    if (cos_theta < 0.0f)
        return 2.0f - hull_sin_theta;
    if (hull_sin_theta > 0.0f)
        return hull_sin_theta;
    return 4.0f - hull_sin_theta;
}
// 0x6ac078
void phys_hull_builder::draw_plane(const phys_vec4 &a, const phys_vec4 &b, int c)
{
}

void phys_hull_builder::convert_from_unit_cube(phys_array<phys_vec4> &vertices, phys_vec4 &extents, phys_vec4 &offset)
{
    phys_mat3 scale;
    scale.set_diagonal(extents(0), extents(1), extents(2));
    phys_mat3 vertex_mat3;
    for (int index = 0; index < vertices.getSize(); ++index) {
        vertex_mat3.set_rows(vertices[index], phys_vec4::get_zero(), phys_vec4::get_zero());
        vertex_mat3.mul(scale);
        vertex_mat3.get_row(0, vertices[index]);
        vertices[index].add4(offset);
    }
}

// 0x6ac688
void phys_hull_builder::plane_and_points::sort()
{
    if (m_hull_edges[0]->m_edge_vertex > m_hull_edges[1]->m_edge_vertex) swap_values(m_hull_edges[0], m_hull_edges[1]);
    if (m_hull_edges[1]->m_edge_vertex > m_hull_edges[2]->m_edge_vertex) swap_values(m_hull_edges[1], m_hull_edges[2]);
    if (m_hull_edges[0]->m_edge_vertex > m_hull_edges[1]->m_edge_vertex) swap_values(m_hull_edges[0], m_hull_edges[1]);
}

phys_bool phys_hull_builder::vec_less_merge_coords(phys_vec4 &first, phys_vec4 &second)
{
    const float *a = (const float *)&first.m_simd_quad;
    const float *b = (const float *)&second.m_simd_quad;
    if (a[0] < b[0]) return phys_bool(true);
    if (a[0] == b[0] && a[1] < b[1]) return phys_bool(true);
    return phys_bool(a[0] == b[0] && a[1] == b[1] && a[2] < b[2]);
}

// 0x6ac778
void vec4_add(const phys_vec4 &first, const phys_vec4 &second, phys_vec4 &result)
{
    result(0) = first(0) + second(0);
    result(1) = first(1) + second(1);
    result(2) = first(2) + second(2);
    result(3) = first(3) + second(3);
}
// 0x6ac7c0
inline void vec4_sub(const phys_vec4 &first, const phys_vec4 &second, phys_vec4 &result)
{
    result(0) = first(0) - second(0);
    result(1) = first(1) - second(1);
    result(2) = first(2) - second(2);
    result(3) = first(3) - second(3);
}
// 0x6ac808
void phys_hull_builder::weld_x_sorted_vertices(float tolerance, phys_array<phys_vec4> &vertices, int &hull_vertex_count)
{
    phys_vec4 *source = vertices.begin();
    phys_vec4 *destination = vertices.begin();
    for (int remaining = vertices.getSize() - 1; remaining >= 0; --remaining) {
        phys_vec4 *neighbour_vertex;
        for (neighbour_vertex = destination - 1; ; --neighbour_vertex) {
            if (neighbour_vertex < vertices.begin() || (*neighbour_vertex)(0) < (*source)(0) - 0.01f) {
                *destination++ = *source;
                break;
            }
            phys_vec4 difference;
            vec4_sub(*neighbour_vertex, *source, difference);
            if (difference.length_sq_3d() < tolerance) {
                for (int next = remaining - 1; next >= 0; --next) {
                    phys_vec4 next_diff;
                    vec4_sub(*neighbour_vertex, source[1], next_diff);
                    if (next_diff.length_sq_3d() < tolerance) {
                        ++source;
                        --remaining;
                    } else break;
                }
                break;
            }
        }
        ++source;
    }
    hull_vertex_count = int(destination - vertices.begin());
    vertices.set_array_size(hull_vertex_count);
}


// @address 0x6ad468 ?remove_flagged_vertices@phys_hull_builder@@SAXAAV?$phys_array@Vphys_vec4@@@@@Z
void phys_hull_builder::remove_flagged_vertices(phys_array<phys_vec4> &vertices)
{
    phys_vec4 *source = vertices.begin();
    phys_vec4 *destination = vertices.begin();
    for (int index = vertices.getSize() - 1; index >= 0; --index) {
        if ((*source)(3) == 0.0f) {
            phys_vec4 *target = destination;
            destination = destination + 1;
            *target = *source;
        }
        source = source + 1;
    }
    vertices.set_array_size(int(destination - vertices.begin()));
}

// 0x6b0428
void phys_hull_builder::append_reindex_edges(phys_geom_hull &source, phys_bool flag, phys_geom_hull &destination, phys_array<unsigned short> &remap)
{
    remap.set_array_size(source.m_hull_edges.getSize());
    int array_old_size = destination.m_hull_edges.getSize();
    for (int index = 0; index < source.m_hull_edges.getSize(); ++index) {
        phys_geom_edge &edge = source.m_hull_edges[index];
        if (edge.m_bvh_info != 1 && edge.m_bvh_info != 2) {
            remap[index] = (unsigned short)-1;
        } else {
            remap[index] = (unsigned short)destination.m_hull_edges.getSize();
            destination.m_hull_edges.array_push_back(edge);
        }
    }
    for (int index = array_old_size; index < destination.m_hull_edges.getSize(); ++index) {
        phys_geom_edge &edge = destination.m_hull_edges[index];
        edge.m_mirror_edge = remap[edge.m_mirror_edge];
        edge.m_next = remap[edge.m_next];
    }
}
// 0x6b07b8
void phys_hull_builder::build_display_geometry(phys_geom_hull &geom_hull, phys_geometry &geometry)
{
    phys_geom_edge *base = geom_hull.m_hull_edges.begin();
    for (int index = 0; index < geom_hull.m_hull_edges.getSize(); ++index) {
        phys_geom_edge edge = geom_hull.m_hull_edges[index];
        int next_idx = edge.m_next;
        int third_edge_idx = edge.get_next_edge(base)->m_next;
        if (index < next_idx && index < third_edge_idx) {
            int edge_first_vertex = edge.m_edge_vertex;
            int edge_second_vertex = edge.get_next_edge(base)->m_edge_vertex;
            int third_vertex_idx = edge.get_next_edge(base)->get_next_edge(base)->m_edge_vertex;
            phys_geometry::geometry_triangle *triangle = geometry.m_triangle_list.expand_by(1);
            triangle->set(edge_first_vertex, third_vertex_idx, edge_second_vertex);
        }
    }
}

phys_bool phys_hull_builder::build_plane_equations(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull,
    const phys_array<phys_vec4> &used_vertices, phys_array<phys_vec4> &plane_equations_out)
{
    phys_bool is_planar;
    phys_array<plane_and_points> planes;
    phys_vec4 plane;
    build_plane_equations(hull_tolerances, geom_hull, used_vertices, plane, is_planar, plane_equations_out, planes);
    return is_planar;
}

void phys_hull_builder::remove_coplanar_neighbours(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull,
    const phys_vec4 &plane, const phys_geom_edge *edge, const phys_vec4 *first_hull_vertices, const phys_vec4 *second_vertices,
    phys_array<weighted_neighbour> &neighbour_list)
{
    phys_vec4 *vertices = geom_hull.m_vertex_base;
    float tolerance = hull_tolerances.m_coplanar_vertex_tol;
    if (neighbour_list.m_size > 1) {
        weighted_neighbour *data = neighbour_list.m_data;
        if (data[1].m_neighbour_weight - data[0].m_neighbour_weight < tolerance) {
            phys_inline_array<weighted_neighbour, 64> coplanar;
            int i = 0;
            float weight = data[0].m_neighbour_weight;
            if (neighbour_list.m_size > 0) {
                do {
                    weighted_neighbour *neighbour_vertex = &neighbour_list.m_data[i];
                    if (neighbour_vertex->m_neighbour_weight - weight > tolerance) break;
                    if (coplanar.m_size == coplanar.get_sector_capacity()) phys_array_util::grow_array_storage(&coplanar, sizeof(weighted_neighbour));
                    coplanar.m_data[coplanar.m_size] = *neighbour_vertex;
                    i++;
                    coplanar.m_size++;
                } while (i < neighbour_list.m_size);
            }
            compute_new_neighbours(vertices, hull_tolerances, weight, plane, edge, first_hull_vertices, second_vertices, coplanar);
            neighbour_list.m_data[0] = coplanar.m_data[0];
            neighbour_list.m_data[0].m_neighbour_weight = weight;
            int capacity = neighbour_list.m_capacity_and_flags & 0x3fffffff;
            if (capacity < 1) {
                int array_new_capacity = capacity << 1;
                if (array_new_capacity <= 1) array_new_capacity = 1;
                phys_array_util::reserve_array_storage(&neighbour_list, array_new_capacity, sizeof(weighted_neighbour));
            }
            neighbour_list.m_size = 1;
        }
    }
}

// 0x6b12e8
void phys_hull_builder::validate_neighbours(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *vertices,
    const phys_vec4 &plane, unsigned short vertex, weighted_line *line, const phys_vec4 *first_hull_vertices, const phys_vec4 *second_vertices,
    phys_array<weighted_neighbour> &left_neighbours, phys_array<weighted_neighbour> &right_neighbours)
{
    if (left_neighbours.getSize() == 1 && right_neighbours.getSize() == 1) {
        phys_geom_edge *edge = line->m_edge_a;
        if (line->m_next && vertex != 0xffff && line->m_next->m_edge_b->m_edge_vertex == vertex) edge = line->m_edge_b;
        float tolerance = hull_tolerances.m_coplanar_vertex_tol;
        if (fabsf(left_neighbours[0].m_neighbour_weight - right_neighbours[0].m_neighbour_weight) < tolerance) {
            phys_inline_array<weighted_neighbour, 2> coplanar_neighbors;
            float weight = right_neighbours[0].m_neighbour_weight;
            coplanar_neighbors.array_push_back(left_neighbours[0]);
            coplanar_neighbors.array_push_back(right_neighbours[0]);
            compute_new_neighbours(vertices, hull_tolerances, weight, plane, edge, first_hull_vertices, second_vertices, coplanar_neighbors);
            if (coplanar_neighbors[0].m_hull_edge == left_neighbours[0].m_hull_edge) {
                left_neighbours[0].m_neighbour_weight *= 0.5f;
            } else {
                right_neighbours[0].m_neighbour_weight *= 0.5f;
            }
        }
    }
}

// 0x6b1430
phys_bool phys_hull_builder::is_edge_check_valid(phys_geom_hull &geom_hull, phys_geom_edge *start_edge, int flag)
{
    phys_geom_edge *base = geom_hull.m_hull_edges.begin();
    phys_inline_array<phys_geom_edge *, 128> pending;
    pending.m_data[pending.m_size++] = start_edge;
    while (pending.getSize() != 0) {
        phys_geom_edge *start = pending.end()[-1];
        pending.drop_back();
        phys_geom_edge *edge = start;
        do {
            edge = edge->get_next_edge(base);
            switch (edge->m_bvh_info) {
            case 1:
            case 3:
                if (edge->m_bvh_info != flag) return phys_bool(false);
                break;
            case 2:
                if (flag == 3) continue;
                break;
            }
            edge->m_bvh_info = (unsigned short)flag;
        } while (edge != start);
        phys_geom_edge *neighbour_vertex = start;
        do {
            neighbour_vertex = neighbour_vertex->get_next_edge(base);
            phys_geom_edge *mirror = neighbour_vertex->get_mirror_edge(base);
            if (mirror->m_bvh_info == 0) pending.array_push_back(mirror);
        } while (neighbour_vertex != start);
    }
    return phys_bool(true);
}
// 0x6b1598
phys_bool phys_hull_builder::is_valid_wrapping(phys_geom_hull &geom_hull, phys_geom_edge *other_base_edge, phys_bool first,
    phys_array<wrapping_line> &hull_wrapping)
{
    phys_geom_edge *base = geom_hull.m_hull_edges.begin();
    for (int index = 0; index < geom_hull.m_hull_edges.getSize(); ++index) geom_hull.m_hull_edges[index].m_bvh_info = 0;
    int change_cnt = 0;
    phys_geom_edge *previous = first ? hull_wrapping[0].m_edge_a : hull_wrapping[0].m_edge_b;
    for (int index = hull_wrapping.getSize() - 1; index >= 0; --index) {
        phys_geom_edge *edge = first ? hull_wrapping[index].m_edge_a : hull_wrapping[index].m_edge_b;
        if (edge == previous) continue;
        previous = edge;
        ++change_cnt;
        phys_geom_edge *outside = first ? edge->get_mirror_edge(base) : edge;
        if (outside->m_bvh_info == 3) return phys_bool(false);
        outside->m_bvh_info = 3;
    }
    if (change_cnt == 0) return phys_bool(true);
    phys_geom_edge *prev_inside_edge = first ? hull_wrapping[0].m_edge_a : hull_wrapping[0].m_edge_b;
    for (int index = hull_wrapping.getSize() - 1; index >= 0; --index) {
        phys_geom_edge *edge = first ? hull_wrapping[index].m_edge_a : hull_wrapping[index].m_edge_b;
        if (edge == prev_inside_edge) continue;
        prev_inside_edge = edge;
        phys_geom_edge *inside = first ? edge : edge->get_mirror_edge(base);
        switch (inside->m_bvh_info) {
        case 0: inside->m_bvh_info = 1; break;
        case 1: return phys_bool(false);
        case 3: inside->m_bvh_info = 2; break;
        }
    }
    phys_bool result = true;
    phys_geom_edge *prev_flood_edge = first ? hull_wrapping[0].m_edge_a : hull_wrapping[0].m_edge_b;
    for (int index = hull_wrapping.getSize() - 1; index >= 0; --index) {
        phys_geom_edge *edge = first ? hull_wrapping[index].m_edge_a : hull_wrapping[index].m_edge_b;
        if (edge != prev_flood_edge) {
            prev_flood_edge = edge;
            phys_geom_edge *inside = first ? edge : edge->get_mirror_edge(base);
            if (inside->m_bvh_info == 1) result = result && is_edge_check_valid(geom_hull, inside, 1);
            phys_geom_edge *outside = inside->get_mirror_edge(base);
            if (outside->m_bvh_info == 3) result = result && is_edge_check_valid(geom_hull, outside, 3);
        }
    }
    phys_geom_edge *prev_shared_edge = first ? hull_wrapping[0].m_edge_a : hull_wrapping[0].m_edge_b;
    for (int index = hull_wrapping.getSize() - 1; index >= 0; --index) {
        phys_geom_edge *edge = first ? hull_wrapping[index].m_edge_a : hull_wrapping[index].m_edge_b;
        if (edge != prev_shared_edge) {
            prev_shared_edge = edge;
            phys_geom_edge *inside = first ? edge : edge->get_mirror_edge(base);
            if (inside->m_bvh_info == 2) {
                phys_bool all_shared = true;
                phys_geom_edge *prev_test_edge = first ? hull_wrapping[0].m_edge_a : hull_wrapping[0].m_edge_b;
                for (int candidate_idx = hull_wrapping.getSize() - 1; candidate_idx >= 0; --candidate_idx) {
                    phys_geom_edge *candidate_edge = first ? hull_wrapping[candidate_idx].m_edge_a : hull_wrapping[candidate_idx].m_edge_b;
                    if (candidate_edge == prev_test_edge) continue;
                    prev_test_edge = candidate_edge;
                    if (candidate_edge->m_bvh_info != 2) { all_shared = false; break; }
                }
                phys_bool has_unmarked = false;
                for (int candidate_idx = 0; candidate_idx < geom_hull.m_hull_edges.getSize(); ++candidate_idx) {
                    if (geom_hull.m_hull_edges[candidate_idx].m_bvh_info == 0) { has_unmarked = true; break; }
                }
                if (all_shared && has_unmarked) result = result && is_edge_check_valid(geom_hull, inside, 3);
            }
        }
    }
    for (int index = 0; index < geom_hull.m_hull_edges.getSize(); ++index) result = result && geom_hull.m_hull_edges[index].m_bvh_info != 0;
    return result;
}
// 0x6b1a50
void phys_hull_builder::find_weighted_neighbours(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull, const phys_vec4 &plane,
    unsigned short vertex, const phys_geom_edge *edge, const phys_vec4 *first_hull_vertices, const phys_vec4 *second_vertices,
    phys_array<weighted_neighbour> &neighbour_list)
{
    if (geom_hull.m_hull_edges.getSize() == 1) return;
    phys_geom_edge *base = geom_hull.m_hull_edges.begin();
    phys_vec4 *vertices = geom_hull.m_vertex_base;
    phys_geom_edge *start = edge->get_mirror_edge(base);
    phys_geom_edge *current = start;
    do {
        const phys_vec4 *point = &vertices[current->m_edge_vertex];
        float weight;
        if (current->m_edge_vertex == vertex) weight = 4.0f;
        else weight = get_angle_between_vertex_and_plane(hull_tolerances, *point, plane, *first_hull_vertices, *second_vertices);
        weighted_neighbour *neighbour_vertex = neighbour_list.expand_by(1);
        neighbour_vertex->m_hull_edge = current;
        neighbour_vertex->m_neighbour_weight = weight;
        current = current->get_next_edge(base)->get_mirror_edge(base);
    } while (current != start);
    phys_algorithm::quick_sort(neighbour_list.begin(), neighbour_list.getSize(), weighted_neighbour_less);
    remove_coplanar_neighbours(hull_tolerances, geom_hull, plane, edge, first_hull_vertices, second_vertices, neighbour_list);
}

// 0x6b1ba8
void phys_hull_builder::find_wrapping(phys_geom_hull &first_hull, phys_geom_hull &second_hull, const phys_array<weighted_line> &lines,
    phys_array<wrapping_line> &hull_wrapping)
{
    for (int index = 0; index < lines.getSize(); ++index) {
        hull_wrapping.clear();
        const weighted_line &line = lines[index];
        wrapping_line *initial = hull_wrapping.expand_by(1);
        initial->m_edge_a = line.m_edge_a;
        initial->m_edge_b = line.m_edge_b;
        for (weighted_line *next = line.m_next; next; next = next->m_next) {
            if (next->edges_and_vertex_equal(line)) {
                phys_bool first_valid = is_valid_wrapping(first_hull, second_hull.m_hull_edges.begin(), true, hull_wrapping);
                phys_bool second_valid = is_valid_wrapping(second_hull, first_hull.m_hull_edges.begin(), false, hull_wrapping);
                if (first_valid && second_valid) return;
            }
            wrapping_line *new_line = hull_wrapping.expand_by(1);
            new_line->m_edge_a = next->m_edge_a;
            new_line->m_edge_b = next->m_edge_b;
        }
    }
    hull_wrapping.clear();
}
// @address 0x6b0bb8 ?stitch_hulls@phys_hull_builder@@SAXAAVphys_geom_hull@@0AAV?$phys_array@Uwrapping_line@phys_hull_builder@@@@0@Z
void phys_hull_builder::stitch_hulls(phys_geom_hull &left_hull, phys_geom_hull &right_hull, phys_array<wrapping_line> &hull_wrapping, phys_geom_hull &out_hull)
{
    out_hull.m_hull_edges.clear();
    phys_local_array<unsigned short> left_reindex(left_hull.m_hull_edges.getSize());
    append_reindex_edges(left_hull, true, out_hull, left_reindex);
    phys_local_array<unsigned short> right_reindex(right_hull.m_hull_edges.getSize());
    append_reindex_edges(right_hull, false, out_hull, right_reindex);
    out_hull.m_hull_edges.reserve_exactly(out_hull.m_hull_edges.getSize() + hull_wrapping.getSize() * 3);
    for (int i = 0; i < hull_wrapping.getSize(); i++) {
        wrapping_line &w = hull_wrapping[i];
        w.m_left_vertex = w.m_edge_a->m_edge_vertex;
        w.m_left_next_edge_vertex = w.m_edge_a->get_next_edge(left_hull.m_hull_edges.begin())->m_edge_vertex;
        w.m_right_next_edge_vertex = w.m_edge_b->m_edge_vertex;
        int left_idx = (int)(w.m_edge_a - left_hull.m_hull_edges.begin());
        phys_geom_edge *right_edge_base = right_hull.m_hull_edges.begin();
        int right_idx = (int)(w.m_edge_b->get_mirror_edge(right_edge_base) - right_edge_base);
        unsigned int new_left_idx = left_reindex[left_idx];
        w.m_edge_a = (new_left_idx == 0xffff) ? 0 : &out_hull.m_hull_edges[new_left_idx];
        unsigned int new_right_idx = right_reindex[right_idx];
        w.m_edge_b = (new_right_idx == 0xffff) ? 0 : &out_hull.m_hull_edges[new_right_idx];
    }
    phys_geom_edge *base = out_hull.m_hull_edges.begin();
    wrapping_line *prev_wrap_line = &hull_wrapping[0];
    phys_geom_edge *last_edge = 0;
    unsigned int last_idx = 0xffff;
    int first_axis_idx = out_hull.m_hull_edges.getSize();
    for (int i = hull_wrapping.getSize() - 1; i >= 0; i--) {
        wrapping_line &w = hull_wrapping[i];
        int newIndex = out_hull.m_hull_edges.getSize();
        phys_geom_edge *new_edge = out_hull.m_hull_edges.expand_by_unchecked(1);
        int new_idx_b = out_hull.m_hull_edges.getSize();
        phys_geom_edge *new_edge_b = out_hull.m_hull_edges.expand_by_unchecked(1);
        if (w.m_edge_a == prev_wrap_line->m_edge_a) {
            phys_bool existing(w.m_edge_b->m_bvh_info == 2);
            int mirror_idx = existing ? w.m_edge_b->m_mirror_edge : out_hull.m_hull_edges.getSize();
            phys_geom_edge *mirror = existing ? w.m_edge_b->get_mirror_edge(base) : out_hull.m_hull_edges.expand_by_unchecked(1);
            new_edge->m_edge_vertex = w.m_edge_b->m_edge_vertex;
            new_edge->m_next = (unsigned short)new_idx_b;
            new_edge->m_mirror_edge = (unsigned short)last_idx;
            if (last_edge)
                last_edge->m_mirror_edge = (unsigned short)newIndex;
            mirror->m_edge_vertex = w.m_right_next_edge_vertex;
            mirror->m_next = (unsigned short)newIndex;
            mirror->m_mirror_edge = (unsigned short)(w.m_edge_b - base);
            w.m_edge_b->m_mirror_edge = (unsigned short)mirror_idx;
            new_edge_b->m_edge_vertex = w.m_left_vertex;
            new_edge_b->m_next = (unsigned short)mirror_idx;
        } else {
            phys_bool existing(w.m_edge_a->m_bvh_info == 2);
            int mirror_idx = existing ? w.m_edge_a->m_mirror_edge : out_hull.m_hull_edges.getSize();
            phys_geom_edge *mirror = existing ? w.m_edge_a->get_mirror_edge(base) : out_hull.m_hull_edges.expand_by_unchecked(1);
            new_edge->m_edge_vertex = w.m_right_next_edge_vertex;
            new_edge->m_next = (unsigned short)mirror_idx;
            new_edge->m_mirror_edge = (unsigned short)last_idx;
            if (last_edge)
                last_edge->m_mirror_edge = (unsigned short)newIndex;
            mirror->m_edge_vertex = w.m_left_next_edge_vertex;
            mirror->m_next = (unsigned short)new_idx_b;
            mirror->m_mirror_edge = (unsigned short)(w.m_edge_a - base);
            w.m_edge_a->m_mirror_edge = (unsigned short)mirror_idx;
            new_edge_b->m_edge_vertex = w.m_edge_a->m_edge_vertex;
            new_edge_b->m_next = (unsigned short)newIndex;
        }
        last_idx = new_idx_b;
        last_edge = new_edge_b;
        prev_wrap_line = &w;
    }
    last_edge->m_mirror_edge = (unsigned short)first_axis_idx;
    base[first_axis_idx].m_mirror_edge = (unsigned short)last_idx;
}

// @address 0x6b1d60 ?merge_hulls@phys_hull_builder@@SA?AW4phys_result@@ABUphys_hull_tolerances@@AAVphys_geom_hull@@11@Z
phys_result phys_hull_builder::merge_hulls(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &left_hull,
    phys_geom_hull &right_hull, phys_geom_hull &out_hull)
{
    out_hull.m_vertex_base = left_hull.m_vertex_base;
    if (is_single_line(hull_tolerances.m_degenerate_tol, left_hull, right_hull, out_hull))
        return PHYS_SUCCESS;

    const phys_vec4 *vertex_base_ptr = left_hull.m_vertex_base;
    phys_inline_array<weighted_line, 1> initial_lines;
    weighted_line *initial_line = initial_lines.expand_by_unchecked(1);
    phys_vec4 initial_plane;
    find_common_tangent(left_hull, right_hull, *initial_line, initial_plane);
    phys_vec4 last_plane = initial_plane;

    phys_inline_array<phys_inline_array<weighted_line, 16> *, 64> allocated_lines;
    int max_iterations = left_hull.m_hull_edges.getSize() + right_hull.m_hull_edges.getSize() + 2;
    phys_hull_config config;
    config.m_atom_max_angle = hull_tolerances.m_max_angle;
    config.m_max_iters = max_iterations * 3;
    phys_inline_array<wrapping_line, 128> hull_wrapping;
    phys_array<weighted_line> *current_lines = &initial_lines;

    for (int i = 0, iteration = 0; i < config.m_max_iters; iteration++)
    {
        if (iteration > max_iterations)
        {
            config.m_atom_max_angle *= 1.1f;
            if (config.m_atom_max_angle > 1.0f)
                max_iterations++;
        }

        phys_inline_array<weighted_line, 16> *new_lines = phys_alloc_stack<phys_inline_array<weighted_line, 16> >(1);
        new (new_lines) phys_inline_array<weighted_line, 16>();
        allocated_lines.array_push_back(new_lines);

        for (int j = 0; j < current_lines->getSize(); j++)
        {
            weighted_line &line = (*current_lines)[j];
            phys_inline_array<weighted_neighbour, 64> left_neighbours;
            phys_inline_array<weighted_neighbour, 64> right_neighbours;
            const phys_vec4 *edge_first_vertex = line.m_edge_a->get_vertex(vertex_base_ptr);
            const phys_vec4 *edge_second_vertex = line.m_edge_b->get_vertex(vertex_base_ptr);
            phys_vec4 tangent_plane;
            if (line.m_next)
            {
                const phys_vec4 *prev_first = line.m_next->m_edge_a->get_vertex(vertex_base_ptr);
                const phys_vec4 *prev_second = line.m_next->m_edge_b->get_vertex(vertex_base_ptr);
                const phys_vec4 *new_vertex = (prev_first == edge_first_vertex) ? edge_second_vertex : edge_first_vertex;
                phys_vec4 edge;
                vec4_sub(*prev_second, *prev_first, edge);
                edge.normalize_xyz();
                phys_vec4 to_new_vertex;
                vec4_sub(*new_vertex, *prev_first, to_new_vertex);
                to_new_vertex.normalize_xyz();
                tangent_plane.set_cross(edge, to_new_vertex);
                const float plane_eqn_min_len = 1e-6f;
                if (tangent_plane.length_sq_3d() < plane_eqn_min_len)
                {
                    vec4_sub(*new_vertex, *prev_second, to_new_vertex);
                    to_new_vertex.normalize_xyz();
                    tangent_plane.set_cross(edge, to_new_vertex);
                    if (tangent_plane.length_sq_3d() < plane_eqn_min_len)
                    {
                        phys_vec4 sum;
                        vec4_add(edge, to_new_vertex, sum);
                        phys_vec4 diff;
                        vec4_sub(edge, to_new_vertex, diff);
                        if (diff.length_sq_3d() < plane_eqn_min_len || sum.length_sq_3d() < plane_eqn_min_len)
                        {
                            tangent_plane = last_plane;
                        }
                        else
                        {
                            sum.normalize_xyz();
                            diff.normalize_xyz();
                            tangent_plane.set_cross(diff, sum);
                        }
                    }
                }
                tangent_plane.normalize_xyz();
            }
            else
            {
                tangent_plane = initial_plane;
            }
            last_plane = tangent_plane;

            find_weighted_neighbours(hull_tolerances, left_hull, tangent_plane, (unsigned short)line.m_last_vertex, line.m_edge_a,
                edge_first_vertex, edge_second_vertex, left_neighbours);
            find_weighted_neighbours(hull_tolerances, right_hull, tangent_plane, (unsigned short)line.m_last_vertex, line.m_edge_b, edge_first_vertex,
                edge_second_vertex, right_neighbours);
            validate_neighbours(hull_tolerances, right_hull.m_vertex_base, tangent_plane, (unsigned short)line.m_last_vertex, &line, edge_first_vertex,
                edge_second_vertex, left_neighbours, right_neighbours);
            add_wrapping_lines(config, &line, left_neighbours, right_neighbours, *new_lines);
        }

        current_lines = new_lines;
        phys_algorithm::quick_sort(current_lines->begin(), current_lines->getSize(), weighted_line_less);
        find_wrapping(left_hull, right_hull, *current_lines, hull_wrapping);
        if (hull_wrapping.getSize() != 0)
            break;
    }

    for (int k = allocated_lines.getSize() - 1; k >= 0; k--)
    {
        allocated_lines[k]->~phys_inline_array<weighted_line, 16>();
        phys_free_stack(allocated_lines[k]);
    }
    allocated_lines.clear();

    if (hull_wrapping.getSize() != 0)
    {
        stitch_hulls(left_hull, right_hull, hull_wrapping, out_hull);
        return PHYS_SUCCESS;
    }
    return PHYS_FAILURE;
}
// 0x6b2c18
void phys_hull_builder::build_convex_hull(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *vertices, int hull_vertex_count,
    phys_geom_hull &geom_hull, phys_array<phys_vec4> &vertices_out)
{
    vertices_out.clear();
    for (int index = 0; index < hull_vertex_count; ++index) vertices_out.array_push_back(vertices[index]);
    phys_vec4 scale;
    phys_vec4 offset;
    if (hull_tolerances.m_convert_to_unit_cube) convert_to_unit_cube(vertices_out, scale, offset);
    if (vertices_out.getSize() > 1) phys_algorithm::quick_sort_recursive(vertices_out.begin(), 0, vertices_out.getSize() - 1, vec_less_merge_coords);
    int welded_vert_cnt;
    weld_x_sorted_vertices(hull_tolerances.m_edge_weld_tol, vertices_out, welded_vert_cnt);
    if (hull_tolerances.m_accurate_but_slow && welded_vert_cnt < 300) remove_collinear_vertices(vertices_out, 0.001f);
    phys_bool changed = true;
    phys_bool has_retried = false;
    const float unflagged_value = 0.0f;
    while (!has_retried || changed) {
        has_retried = !changed;
        for (int index = 0; index < vertices_out.getSize(); ++index) vertices_out[index](3) = unflagged_value;
        geom_hull.m_hull_edges.clear();
        build_convex_sub_hull(hull_tolerances, vertices_out, 0, vertices_out.getSize() - 1, geom_hull);
        remove_unused_vertices(geom_hull, vertices_out);
        remove_flagged_vertices(vertices_out);
        post_filter_vertices(geom_hull, 0, vertices_out.getSize() - 1, hull_tolerances, changed);
        remove_flagged_vertices(vertices_out);
    }
    if (hull_tolerances.m_convert_to_unit_cube) convert_from_unit_cube(vertices_out, scale, offset);
}

// 0x6b31f8
namespace phys_algorithm {
    template void quick_sort_recursive<phys_hull_builder::weighted_neighbour,
        phys_bool (*)(const phys_hull_builder::weighted_neighbour &, const phys_hull_builder::weighted_neighbour &)>(
        phys_hull_builder::weighted_neighbour *, int, int,
        phys_bool (*)(const phys_hull_builder::weighted_neighbour &, const phys_hull_builder::weighted_neighbour &));
}

// @address 0x6b0930 ?remove_unused_vertices@phys_hull_builder@@SAXAAVphys_geom_hull@@AAV?$phys_array@Vphys_vec4@@@@@Z
void phys_hull_builder::remove_unused_vertices(phys_geom_hull &geom_hull, phys_array<phys_vec4> &vertices)
{
    for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
        vertices[geom_hull.m_hull_edges[i].m_edge_vertex](3) = 2.0f;
    {
        phys_local_array<int> local(vertices.getSize());
        local.set_size_unchecked(vertices.getSize());
        int used_cnt = 0;
        for (int i = 0; i < vertices.getSize(); i++) {
            if (vertices[i](3) == 2.0f) {
                vertices[used_cnt] = vertices[i];
                local[i] = used_cnt;
                used_cnt = used_cnt + 1;
            } else {
                local[i] = -1;
            }
        }
        vertices.set_array_size(used_cnt);
        for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
            geom_hull.m_hull_edges[i].m_edge_vertex = local[geom_hull.m_hull_edges[i].m_edge_vertex];
    }
    for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
        vertices[geom_hull.m_hull_edges[i].m_edge_vertex](3) = 0.0f;
}

// @address 0x6ace28 ?draw@phys_hull_builder@@SAXAAVphys_geom_hull@@HPAVphys_text_display@@@Z
void phys_hull_builder::draw(phys_geom_hull &geom_hull, int, class phys_text_display *text_display)
{
    phys_text_display **pd = &text_display; (void)pd;
    char *buf = (char *)phys_thread_memory::get_instance_ptr()->allocate(128, (phys_memory_class)20);
    for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
    {
        phys_geom_edge &e = geom_hull.m_hull_edges[i];
        phys_geom_edge *next = e.get_next_edge(geom_hull.m_hull_edges.begin());
        phys_geom_edge *next_next_edge = next->get_next_edge(geom_hull.m_hull_edges.begin());
        const phys_vec4 *v0 = &geom_hull.m_vertex_base[e.m_edge_vertex];
        const phys_vec4 *v1 = &geom_hull.m_vertex_base[next->m_edge_vertex];
        const phys_vec4 *v2 = &geom_hull.m_vertex_base[next_next_edge->m_edge_vertex];
        phys_vec4 dir;
        vec4_sub(*v1, *v0, dir);
        float s = 0.3f;
        dir.m_simd_quad = __vmulfp(dir.m_simd_quad, __vspltw(__lvlx(&s, 0), 0));
        phys_vec4 pos;
        float t = 0.1f;
        pos.m_simd_quad = __vmaddfp(__vsubfp(v2->m_simd_quad, v0->m_simd_quad), __vspltw(__lvlx(&t, 0), 0), v0->m_simd_quad);
        phys_string::sprintf(buf, "%d(%d)", i, geom_hull.m_hull_edges[i].m_next);
    }
    phys_thread_memory::get_instance_ptr()->deallocate(buf);
}


// @address 0x6acf58 ?is_single_line@phys_hull_builder@@SA?AVphys_bool@@MAAVphys_geom_hull@@00@Z
phys_bool phys_hull_builder::is_single_line(float tolerance, phys_geom_hull &hull_a, phys_geom_hull &hull_b, phys_geom_hull &out_hull)
{
    const phys_vec4 *verts = hull_a.m_vertex_base;
    if (hull_a.m_hull_edges.getSize() < 3 && hull_b.m_hull_edges.getSize() < 3)
    {
        int a0 = 0;
        int a1 = 0;
        int b0 = 0;
        int b1 = 0;
        int hull_vert_count = 3;
        if (hull_a.m_hull_edges.getSize() == 2)
        {
            a0 = hull_a.m_hull_edges[0].m_edge_vertex;
            a1 = hull_a.m_hull_edges[0].get_mirror_edge(hull_a.m_hull_edges.begin())->m_edge_vertex;
            if (a1 < a0) phys_algorithm::swap(a0, a1);
            b0 = hull_b.m_hull_edges[0].m_edge_vertex;
            if (hull_b.m_hull_edges.getSize() == 2)
            {
                b1 = hull_b.m_hull_edges[0].get_mirror_edge(hull_b.m_hull_edges.begin())->m_edge_vertex;
                if (b1 < b0) phys_algorithm::swap(b1, b0);
                hull_vert_count = 4;
            }
        }
        else if (hull_b.m_hull_edges.getSize() == 2)
        {
            a0 = hull_a.m_hull_edges[0].m_edge_vertex;
            a1 = hull_b.m_hull_edges[0].get_mirror_edge(hull_b.m_hull_edges.begin())->m_edge_vertex;
            b0 = hull_b.m_hull_edges[0].m_edge_vertex;
            if (a1 < a0) phys_algorithm::swap(a0, a1);
        }
        else
        {
            out_hull.init_from_edge(hull_a.m_hull_edges[0].m_edge_vertex, hull_b.m_hull_edges[0].m_edge_vertex);
            return true;
        }

        phys_vec4 e0;
        vec4_sub(verts[a1], verts[a0], e0);
        float len_sq0 = e0.length_sq_3d();
        phys_vec4 e1;
        vec4_sub(verts[b0], verts[a1], e1);
        float len_sq1 = e1.length_sq_3d();
        phys_vec4 e2;
        vec4_sub(verts[a0], verts[b0], e2);
        float length_sq_quad = e2.length_sq_3d();
        e0.normalize_xyz();
        e1.normalize_xyz();
        e2.normalize_xyz();
        phys_vec4 diff;
        vec4_sub(e1, e0, diff);
        if (diff.length_sq_3d() < tolerance)
        {
            if (hull_vert_count == 4)
            {
                phys_vec4 e3;
                vec4_sub(verts[b1], verts[a1], e3);
                float len_sq3 = e3.length_sq_3d();
                e3.normalize_xyz();
                phys_vec4 diff_c;
                vec4_sub(e1, e3, diff_c);
                if (diff_c.length_sq_3d() < tolerance)
                {
                    if (len_sq3 > len_sq1)
                    {
                        b0 = b1;
                        e1 = e3;
                        vec4_sub(verts[a0], verts[b1], e2);
                        length_sq_quad = e2.length_sq_3d();
                    }
                }
                else
                {
                    return false;
                }
            }
            float max_edge_len = phys_math::max_of_two(len_sq0, phys_math::max_of_two(len_sq1, length_sq_quad));
            if (len_sq0 == max_edge_len)
            {
                out_hull.init_from_edge(a0, a1);
            }
            else if (len_sq1 == max_edge_len)
            {
                out_hull.init_from_edge(a1, b0);
            }
            else if (length_sq_quad == max_edge_len)
            {
                out_hull.init_from_edge(b0, a0);
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}
// @address 0x6add70 ?get_angle_between_vertex_and_plane@phys_hull_builder@@SAMABUphys_hull_tolerances@@ABVphys_vec4@@111@Z
float phys_hull_builder::get_angle_between_vertex_and_plane(const struct phys_hull_tolerances &hull_tol, const class phys_vec4 &vertex,
    const class phys_vec4 &normal, const class phys_vec4 &p1, const class phys_vec4 &p2)
{
    phys_vec4 to_vertex_dir;
    vec4_sub(vertex, p1, to_vertex_dir);
    if ((float)to_vertex_dir.length_sq_3d() < hull_tol.m_degenerate_tol)
    {
        return 0.0f;
    }
    to_vertex_dir.normalize_xyz();
    float x = -(float)normal.dot3(to_vertex_dir);
    phys_vec4 edge;
    vec4_sub(p1, p2, edge);
    phys_vec4 perp;
    perp.set_cross(edge, normal);
    perp.normalize_xyz();
    float y = perp.dot3(to_vertex_dir);
    float tolerance = hull_tol.m_degenerate_tol;
    if (x * x + y * y < tolerance)
    {
        return 1.70141e+38f;
    }
    float float_epsilon = 1e-6f;
    float eps_sq = 1e-7f;
    float angle_tol = hull_tol.m_hull_tol3;
    float angle;
    if ((float)fabs(x) < float_epsilon && (float)fabs(y) < eps_sq)
    {
        angle = (y < 0.0f) ? 4.0f : 0.0f;
    }
    else if ((float)fabs(x) > (float)fabs(y))
    {
        angle = 2.0f - y / x;
        angle += (x < -float_epsilon) ? 4.0f : 0.0f;
    }
    else
    {
        angle = x / y;
        if (y < float_epsilon - 1.0f && (float)fabs(x) < float_epsilon)
        {
            angle = 4.0f;
        }
        else
        {
            angle += (y < -float_epsilon) ? 4.0f : 0.0f;
        }
        if ((float)fabs(angle) < angle_tol)
        {
            angle = 0.0f;
        }
        else
        {
            angle += (y > float_epsilon && (float)fabs(x) < eps_sq) ? 8.0f : 0.0f;
        }
    }
    if (angle < -angle_tol)
    {
        angle += 8.0f;
    }
    if (angle > 8.0f)
    {
        angle = 0.0f;
    }
    return angle;
}
// @address 0x6b05a8 ?compute_new_neighbours@phys_hull_builder@@SAXPBVphys_vec4@@ABUphys_hull_tolerances@@MABV2@PBVphys_geom_edge@@00AAV?$phys_array@Uweighted_neighbour@phys_hull_builder@@@@@Z
void phys_hull_builder::compute_new_neighbours(class phys_vec4 const *verts, struct phys_hull_tolerances const &hull_tol, float p2,
    class phys_vec4 const &vec, class phys_geom_edge const *edge, class phys_vec4 const *a, class phys_vec4 const *b,
    class phys_array<struct phys_hull_builder::weighted_neighbour> &out)
{
    float tolerance = hull_tol.m_coplanar_vertex_tol;
    const phys_vec4 *v = &verts[edge->m_edge_vertex];
    phys_vec4 normal = vec;
    phys_vec4 d;
    vec4_sub(*a, *b, d);
    if (p2 < tolerance || p2 > 8.0f - tolerance)
    {
        normal.set_cross(normal, d);
        normal.normalize_xyz();
    }
    else if ((float)fabs(p2 - 4.0f) < tolerance)
    {
        normal.set_cross(d, normal);
        normal.normalize_xyz();
    }
    d.set_cross(normal, d);
    d.add4(*v);
    phys_vec4 p1;
    if (v == a) p1 = d;
    else p1 = *v;
    phys_vec4 plane_to_vert;
    if (v == a) plane_to_vert = *v;
    else plane_to_vert = d;
    for (int i = 0; i < out.getSize(); i++)
    {
        const phys_vec4 *vv = &verts[out[i].m_hull_edge->m_edge_vertex];
        float weight = get_angle_between_vertex_and_plane(hull_tol, *vv, normal, p1, plane_to_vert);
        out[i].m_neighbour_weight = weight;
    }
    phys_algorithm::quick_sort(out.begin(), out.getSize(), weighted_neighbour_less);
}

// @address 0x6b2e08 ?generate_convex_hull@phys_hull_builder@@SAXPBVphys_vec4@@HAAVphys_geom_hull@@AAV?$phys_array@Vphys_vec4@@@@W4phys_hull_mode@@@Z
void phys_hull_builder::generate_convex_hull(
    const phys_vec4 *verts, int hull_vertex_count, phys_geom_hull &out_hull, phys_array<phys_vec4> &used_vertices_out, phys_hull_mode mode)
{
    phys_hull_tolerances hull_tolerances;
    hull_tolerances.m_post_filter = false;
    hull_tolerances.m_degenerate_tol = 4e-6f;
    hull_tolerances.m_accurate_but_slow = (mode == PHYS_HULL_MODE_ACCURATE_BUT_SLOW);
    hull_tolerances.m_convert_to_unit_cube = false;

    phys_array<phys_vec4> sorted_verts;
    for (int i = 0; i < hull_vertex_count; i++) sorted_verts.array_push_back(verts[i]);
    phys_algorithm::quick_sort(sorted_verts.begin(), sorted_verts.getSize(), vec_less_merge_coords);
    int welded_vert_cnt;
    weld_x_sorted_vertices(hull_tolerances.m_edge_weld_tol, sorted_verts, welded_vert_cnt);

    phys_array<phys_vec4> hull_plane_equations;
    phys_bool is_valid_aabb(false);
    if (!is_valid_aabb) {
        used_vertices_out.clear();
        build_convex_hull(hull_tolerances, sorted_verts.begin(), sorted_verts.getSize(), out_hull, used_vertices_out);
        is_valid_aabb = phys_hull_tester::is_valid_hull(hull_tolerances, sorted_verts.begin(), sorted_verts.getSize(), out_hull, used_vertices_out);
    }

    if (!is_valid_aabb && hull_tolerances.m_accurate_but_slow) {
        hull_plane_equations.clear();
        phys_bool planar(false);
        phys_array<plane_and_points> planes;
        phys_vec4 plane;
        build_plane_equations(hull_tolerances, out_hull, used_vertices_out, plane, planar, hull_plane_equations, planes);
        if (planar) {
            used_vertices_out.clear();
            remove_collinear_vertices(sorted_verts, 0.001f);
            phys_array<phys_vec4> plane_vertices;
            generate_hull_from_planar_points(plane, sorted_verts.begin(), sorted_verts.getSize(), used_vertices_out, plane_vertices);
            int used_cnt = used_vertices_out.getSize();
            is_valid_aabb = phys_hull_tester::is_valid_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
        }
    }

    if (hull_tolerances.m_accurate_but_slow) return;

    hull_tolerances.m_convert_to_unit_cube = true;
    if (!is_valid_aabb) {
        hull_tolerances.m_post_filter = true;
        build_convex_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
        is_valid_aabb = phys_hull_tester::is_valid_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
        if (!is_valid_aabb) {
            hull_tolerances.m_degenerate_tol = 2e-7f;
            build_convex_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
            is_valid_aabb = phys_hull_tester::is_valid_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
            if (!is_valid_aabb) {
                hull_tolerances.m_hull_tol3 = 1e-6f;
                build_convex_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
                is_valid_aabb = phys_hull_tester::is_valid_hull(hull_tolerances, verts, hull_vertex_count, out_hull, used_vertices_out);
            }
        }
    }
}

// @address 0x6ad5c8 ?get_plane_equation_z_axis@phys_hull_builder@@SAXABVphys_vec4@@0AAV2@@Z
void phys_hull_builder::get_plane_equation_z_axis(class phys_vec4 const &p0, class phys_vec4 const &p1, class phys_vec4 &plane_out)
{
    const float float_epsilon = 1e-06f;
    phys_vec4 edge_a_to_b;
    vec4_sub(p0, p1, edge_a_to_b);
    if (fabsf(p0(0) - p1(0)) < float_epsilon && fabsf(p0(1) - p1(1)) < float_epsilon)
    {
        phys_vec4 x; x.set(1.0f, 0.0f, 0.0f, 0.0f);
        plane_out.set_cross(edge_a_to_b, x);
        plane_out.normalize_xyz();
    }
    else
    {
        edge_a_to_b.normalize_xyz();
        phys_vec4 y; y.set(0.0f, 1.0f, 0.0f, 0.0f);
        plane_out.set_cross(edge_a_to_b, y);
        if ((float)plane_out.length_sq_3d() < 1.1920929e-07f)
        {
            phys_vec4 z; z.set(0.0f, 0.0f, 1.0f, 0.0f);
            plane_out.set_cross(edge_a_to_b, z);
        }
        plane_out.normalize_xyz();
    }
}

// @address 0x6ad780 ?find_common_tangent@phys_hull_builder@@SAXAAVphys_geom_hull@@0AAUweighted_line@1@AAVphys_vec4@@@Z
void phys_hull_builder::find_common_tangent(phys_geom_hull &left_hull, phys_geom_hull &right_hull, weighted_line &line, phys_vec4 &plane)
{
    const phys_vec4 *base = left_hull.m_vertex_base;
    line.m_edge_a = &left_hull.m_hull_edges[0];
    line.m_edge_b = &right_hull.m_hull_edges[0];
    line.m_next = 0;
    line.m_neighbour_weight = 0.0f;
    line.m_last_vertex = ~0u;
    const phys_vec4 *left_vert = line.m_edge_a->get_vertex(base);
    const phys_vec4 *right_vert = line.m_edge_b->get_vertex(base);
    get_plane_equation_z_axis(*left_vert, *right_vert, plane);
    phys_bool changed;
    int max_iters = (left_hull.m_hull_edges.getSize() + right_hull.m_hull_edges.getSize()) * 2;
    for (int iter = 0; iter < max_iters; iter++) {
        changed = false;
        for (int i = left_hull.m_hull_edges.getSize() - 1; i >= 0; i--) {
            phys_geom_edge &edge = left_hull.m_hull_edges[i];
            const phys_vec4 *v = edge.get_vertex(base);
            phys_vec4 diff;
            vec4_sub(*v, *left_vert, diff);
            float dist = plane.dot3(diff);
            if (dist > 1e-7f) {
                line.m_edge_a = &edge;
                left_vert = v;
                get_plane_equation_z_axis(*left_vert, *right_vert, plane);
                changed = true;
            }
        }
        for (int j = right_hull.m_hull_edges.getSize() - 1; j >= 0; j--) {
            phys_geom_edge &edge = right_hull.m_hull_edges[j];
            const phys_vec4 *v = edge.get_vertex(base);
            phys_vec4 diff;
            vec4_sub(*v, *right_vert, diff);
            float dist = plane.dot3(diff);
            if (dist > 1e-7f) {
                line.m_edge_b = &edge;
                right_vert = v;
                get_plane_equation_z_axis(*left_vert, *right_vert, plane);
                changed = true;
            }
        }
        if (!changed) break;
        get_plane_equation_z_axis(*left_vert, *right_vert, plane);
    }
}

// 0x6ac9d8
void phys_hull_builder::make_bevel_plane(const phys_vec4 &bevel_plane_normal, const phys_vec4 &vertex0, const phys_vec4 &vertex1,
    const phys_vec4 &vertex2, phys_array<phys_vec4> &plane_equations_out)
{
    phys_vec4 *plane = plane_equations_out.expand_by(1);
    phys_vec4 edge;
    vec4_sub(vertex0, vertex1, edge);
    plane->set_cross(bevel_plane_normal, edge);
    phys_vec4 dir;
    vec4_sub(vertex2, vertex1, dir);
    float d = plane->dot3(dir);
    if (d > 1e-6f) {
        plane->set_neg4(*plane);
    }
    if ((float)plane->length_sq_3d() > 1e-4f) {
        plane->normalize_xyz();
        (*plane)(3) = -(float)plane->dot3(vertex0);
        phys_vec4 mid;
        vec4_sub(vertex0, vertex1, mid);
        mid.set_mul4(0.5f, mid);
        mid.add4(vertex1);
    } else {
        plane_equations_out.set_array_size(plane_equations_out.getSize() - 1);
    }
}

// 0x6acbf8
void phys_hull_builder::convert_to_unit_cube(phys_array<phys_vec4> &used_vertices_out, phys_vec4 &extents, phys_vec4 &hull_aabb_center)
{
    phys_aabb aabb;
    get_handle_aabb(used_vertices_out, aabb);
    vec4_add(aabb.m_max_limit, aabb.m_min_limit, hull_aabb_center);
    hull_aabb_center.mul4(0.5f);
    vec4_sub(aabb.m_max_limit, aabb.m_min_limit, extents);
    phys_mat3 scale;
    scale.set_diagonal(
        extents(0) > 1.1920929e-7f ? 1.0f / extents(0) : 1.0f,
        extents(1) > 1.1920929e-7f ? 1.0f / extents(1) : 1.0f,
        extents(2) > 1.1920929e-7f ? 1.0f / extents(2) : 1.0f);
    phys_mat3 vertex_mat3;
    for (int i = 0; i < used_vertices_out.getSize(); i++) {
        phys_vec4 centered;
        vec4_sub(used_vertices_out[i], hull_aabb_center, centered);
        vertex_mat3.set_rows(centered, phys_vec4::get_zero(), phys_vec4::get_zero());
        vertex_mat3.mul(scale);
        vertex_mat3.get_row(0, used_vertices_out[i]);
    }
}

// 0x6aecc8
void phys_hull_builder::remove_collinear_vertices(phys_array<phys_vec4> &vertices, float tolerance)
{
    for (int i = 0; i < vertices.getSize(); i++) {
        vertices[i](3) = 0.0f;
    }
    for (int a = 0; a < vertices.getSize(); a++) {
        for (int b = a + 1; b < vertices.getSize(); b++) {
            for (int c = b + 1; c < vertices.getSize(); c++) {
                if (vertices[a](3) == 1.0f || vertices[b](3) == 1.0f || vertices[c](3) == 1.0f) continue;
                phys_vec4 ab;
                ab.set_sub4(vertices[a], vertices[b]);
                ab.normalize_xyz();
                phys_vec4 b_minus_a;
                b_minus_a.set_neg4(ab);
                phys_vec4 ac;
                ac.set_sub4(vertices[a], vertices[c]);
                ac.normalize_xyz();
                phys_vec4 ca;
                ca.set_neg4(ac);
                phys_vec4 cb;
                cb.set_sub4(vertices[c], vertices[b]);
                cb.normalize_xyz();
                phys_vec4 bc;
                bc.set_neg4(cb);
                phys_vec4 cross;
                cross.set_cross(ab, ac);
                float normal_len_sq = cross.length_sq_3d();
                if (normal_len_sq < tolerance && (float)ab.dot3(ac) < 0.0f) {
                    vertices[a](3) = 1.0f;
                    continue;
                }
                cross.set_cross(cb, ca);
                normal_len_sq = cross.length_sq_3d();
                if (normal_len_sq < tolerance && (float)cb.dot3(ca) < 0.0f) {
                    vertices[c](3) = 1.0f;
                    continue;
                }
                cross.set_cross(b_minus_a, bc);
                normal_len_sq = cross.length_sq_3d();
                if (normal_len_sq < tolerance && (float)b_minus_a.dot3(bc) < 0.0f) {
                    vertices[b](3) = 1.0f;
                    continue;
                }
            }
        }
    }
    remove_flagged_vertices(vertices);
}

// @address 0x6ad9c0 ?add_wrapping_lines@phys_hull_builder@@SAXABUphys_hull_config@@PAUweighted_line@1@AAV?$phys_array@Uweighted_neighbour@phys_hull_builder@@@@2AAV?$phys_array@Uweighted_line@phys_hull_builder@@@@@Z
void phys_hull_builder::add_wrapping_lines(struct phys_hull_config const &config,
    struct phys_hull_builder::weighted_line *line, class phys_array<struct phys_hull_builder::weighted_neighbour> &left_neighbours,
    class phys_array<struct phys_hull_builder::weighted_neighbour> &right_neighbours,
    class phys_array<struct phys_hull_builder::weighted_line> &lines)
{
    float min_weight;
    if (left_neighbours.getSize() == 0)
        min_weight = right_neighbours[0].m_neighbour_weight;
    else if (right_neighbours.getSize() == 0)
        min_weight = left_neighbours[0].m_neighbour_weight;
    else
        min_weight = phys_math::min_of_two(left_neighbours[0].m_neighbour_weight, right_neighbours[0].m_neighbour_weight);

    {
        phys_geom_edge *right_edge = line->m_edge_b;
        for (int i = 0; i < left_neighbours.getSize(); i++)
        {
            float weight = left_neighbours[i].m_neighbour_weight - min_weight;
            if (weight > config.m_atom_max_angle)
                break;

            weighted_line new_line;
            new_line.m_neighbour_weight = weight + line->m_neighbour_weight;
            new_line.m_edge_a = left_neighbours[i].m_hull_edge;
            new_line.m_edge_b = right_edge;
            new_line.m_next = line;
            new_line.m_last_vertex = line->m_edge_a->m_edge_vertex;

            phys_bool found = false;
            for (int j = 0; j < lines.getSize(); j++)
            {
                if (lines[j].verts_and_vertex_equal(new_line))
                {
                    if (lines[j].m_neighbour_weight < new_line.m_neighbour_weight)
                        lines[j] = new_line;
                    found = true;
                    break;
                }
            }
            if (!found)
                lines.array_push_back(new_line);
        }
    }
    {
        phys_geom_edge *left_edge = line->m_edge_a;
        for (int i = 0; i < right_neighbours.getSize(); i++)
        {
            float weight = right_neighbours[i].m_neighbour_weight - min_weight;
            if (weight > config.m_atom_max_angle)
                break;

            weighted_line new_line;
            new_line.m_neighbour_weight = weight + line->m_neighbour_weight;
            new_line.m_edge_a = left_edge;
            new_line.m_edge_b = right_neighbours[i].m_hull_edge;
            new_line.m_next = line;
            new_line.m_last_vertex = line->m_edge_b->m_edge_vertex;

            phys_bool found = false;
            for (int j = 0; j < lines.getSize(); j++)
            {
                if (lines[j].verts_and_vertex_equal(new_line))
                {
                    if (lines[j].m_neighbour_weight < new_line.m_neighbour_weight)
                        lines[j] = new_line;
                    found = true;
                    break;
                }
            }
            if (!found)
                lines.array_push_back(new_line);
        }
    }
}

// @address 0x6ae030
void phys_hull_builder::generate_hull_from_planar_points(const phys_vec4 &plane_eqn, const phys_vec4 *input_vertices, int vert_cnt,
    phys_array<phys_vec4> &used_vertices, phys_array<phys_vec4> &plane_equations_out)
{
    plane_equations_out.clear();
    phys_vec4 neg_plane;
    neg_plane.set_neg4(plane_eqn);
    plane_equations_out.array_push_back(plane_eqn);
    plane_equations_out.array_push_back(neg_plane);

    phys_array<phys_bool> used_vertex_flags(vert_cnt, phys_bool(false));
    int hull_start_vertex = -1;

    phys_vec4 perp;
    if (phys_math::fabs(1.0f - phys_math::fabs(plane_eqn(2))) < 1e-6f)
    {
        phys_vec4 plane_x_axis;
        plane_x_axis.set(1.0f, 0.0f, 0.0f, 0.0f);
        perp.set_cross(plane_x_axis, plane_eqn);
    }
    else
    {
        phys_vec4 plane_z_axis;
        plane_z_axis.set(0.0f, 0.0f, 1.0f, 0.0f);
        perp.set_cross(plane_eqn, plane_z_axis);
    }

    float max_dist_vec = -3.40282e+38f;
    for (int i = 0; i < vert_cnt; i++)
    {
        float dist = input_vertices[i].dot3(perp);
        if (dist > max_dist_vec)
        {
            max_dist_vec = dist;
            hull_start_vertex = i;
        }
    }

    used_vertex_flags[hull_start_vertex] = true;
    used_vertices.array_push_back(input_vertices[hull_start_vertex]);

    phys_vec4 direction;
    direction.set_cross(perp, plane_eqn);
    direction.add4(perp);

    int walk_cur_vertex = hull_start_vertex;
    int prev_vertex = hull_start_vertex;
    int prev_wrap_vertex = -1;
    int wrap_next_vertex = -1;
    phys_bool finished(false);
    while (!finished)
    {
        float best_sin = -2.0f;
        direction.normalize_xyz();
        phys_vec4 target;
        target.set_add4(input_vertices[walk_cur_vertex], direction);
        target(3) = 1.0f;
        float wrap_min_angle = 3.40282e+38f;
        for (int j = 0; j < vert_cnt; j++)
        {
            if (j != walk_cur_vertex)
            {
                phys_vec4 edge;
                edge.set_sub4(input_vertices[j], input_vertices[walk_cur_vertex]);
                phys_vec4 cross;
                cross.set_cross(direction, edge);
                float dot = direction.dot3(edge);
                float edge_len = edge.length_3d();
                float cross_len = cross.length_3d();
                float edge_weld_sin_angle = cross_len / edge_len;
                float weld_cos_angle = dot / edge_len;
                if (cross.dot3(plane_eqn) > 0.0f)
                {
                    edge_weld_sin_angle *= -1.0f;
                }
                float angle = get_pseudo_angle(weld_cos_angle, edge_weld_sin_angle);
                if (angle < wrap_min_angle)
                {
                    wrap_min_angle = angle;
                    wrap_next_vertex = j;
                    best_sin = edge_weld_sin_angle;
                }
            }
        }

        if (walk_cur_vertex != hull_start_vertex)
        {
            phys_vec4 e0;
            e0.set_sub4(input_vertices[walk_cur_vertex], input_vertices[prev_vertex]);
            phys_vec4 e1;
            e1.set_sub4(input_vertices[walk_cur_vertex], input_vertices[wrap_next_vertex]);
            // match: 0x6ae030: direct m_data[m_size++] preserves the plane.mul4 store before normalize3;
            // hk_base.h expandOne keeps the named reference for 0x67e120.
            if (plane_equations_out.m_size == plane_equations_out.get_sector_capacity())
            {
                phys_array_util::grow_array_storage(&plane_equations_out, sizeof(phys_vec4));
            }
            phys_vec4 &plane = plane_equations_out.m_data[plane_equations_out.m_size++];
            phys_vec4 normal;
            if (prev_vertex != wrap_next_vertex)
            {
                normal.set_cross(e0, e1);
            }
            else
            {
                normal = plane_eqn;
            }
            plane.set_cross(e0, normal);
            float sign = (plane.dot3(e1) < 1e-6f) ? -1.0f : 1.0f;
            plane.mul4(sign);
            plane.normalize_xyz();
            plane(3) = -plane.dot3(input_vertices[walk_cur_vertex]);
        }
        else
        {
            prev_wrap_vertex = wrap_next_vertex;
        }

        if (used_vertex_flags[wrap_next_vertex])
        {
            finished = true;
        }
        else
        {
            used_vertex_flags[wrap_next_vertex] = true;
            used_vertices.array_push_back(input_vertices[wrap_next_vertex]);
            phys_vec4 wrap_new_dir;
            wrap_new_dir.set_sub4(input_vertices[wrap_next_vertex], input_vertices[walk_cur_vertex]);
            wrap_new_dir.normalize_xyz();
            if (best_sin < 0.0001f)
            {
                direction = wrap_new_dir;
            }
            else
            {
                direction.set_add4(wrap_new_dir, direction);
            }
            prev_vertex = walk_cur_vertex;
            walk_cur_vertex = wrap_next_vertex;
        }
    }

    phys_vec4 e0;
    e0.set_sub4(input_vertices[wrap_next_vertex], input_vertices[walk_cur_vertex]);
    phys_vec4 e1;
    e1.set_sub4(input_vertices[wrap_next_vertex], input_vertices[prev_wrap_vertex]);
    phys_vec4 &plane = plane_equations_out.expand_one();
    plane.set_cross(e0, plane_equations_out[0]);
    float sign = (plane.dot3(e1) < 1e-6f) ? -1.0f : 1.0f;
    plane.mul4(sign);
    plane.normalize_xyz();
    plane(3) = -plane.dot3(input_vertices[wrap_next_vertex]);
}

// @address 0x6ae7d8
void phys_hull_builder::approx_hull_from_kdop(
    const phys_vec4 *verts, int hull_vertex_count, phys_array<phys_vec4> &plane_equations_out, phys_array<phys_vec4> &used_vertices_out)
{
    phys_vec4 sample_dirs[13];
    sample_dirs[0].set(1.0f, 0.0f, 0.0f, 0.0f);
    sample_dirs[1].set(0.0f, 1.0f, 0.0f, 0.0f);
    sample_dirs[2].set(0.0f, 0.0f, 1.0f, 0.0f);
    sample_dirs[3].set(0.0f, 1.0f, 1.0f, 0.0f);
    sample_dirs[4].set(1.0f, 0.0f, 1.0f, 0.0f);
    sample_dirs[5].set(1.0f, 1.0f, 0.0f, 0.0f);
    sample_dirs[6].set(0.0f, 1.0f, -1.0f, 0.0f);
    sample_dirs[7].set(1.0f, 0.0f, -1.0f, 0.0f);
    sample_dirs[8].set(1.0f, -1.0f, 0.0f, 0.0f);
    sample_dirs[9].set(1.0f, 1.0f, 1.0f, 0.0f);
    sample_dirs[10].set(1.0f, 1.0f, -1.0f, 0.0f);
    sample_dirs[11].set(1.0f, -1.0f, -1.0f, 0.0f);
    sample_dirs[12].set(1.0f, -1.0f, 1.0f, 0.0f);
    int i;
    for (i = 0; i < 13; i++)
    {
        sample_dirs[i].normalize_xyz();
    }

    phys_array<phys_vec4> hull_plane_equations;
    phys_strided_vertices sv;
    sv.m_hull_vertex_count = hull_vertex_count;
    sv.m_vertex_stride_bytes = sizeof(phys_vec4);
    sv.m_vertices = &verts[0](0);
    phys_convex_hull_shape shape(sv, hull_plane_equations, phys_convex_shape_default_radius);

    for (i = 0; i < 13; i++)
    {
        phys_vec4 dir = sample_dirs[i];
        phys_hull_vertex v;
        shape.get_support_vertex(dir, v);
        used_vertices_out.array_push_back(v);
        dir(3) = -float(dir.dot3(v));
        plane_equations_out.array_push_back(dir);

        dir.set_neg4(sample_dirs[i]);
        shape.get_support_vertex(dir, v);
        used_vertices_out.array_push_back(v);
        dir(3) = -float(dir.dot3(v));
        plane_equations_out.array_push_back(dir);
    }
}

// @address 0x6aefb0
void phys_hull_builder::post_filter_vertices(
    phys_geom_hull &geom_hull, int hull_start_vertex, int hull_end_vertex, const phys_hull_tolerances &hull_tolerances, phys_bool &changed)
{
    changed = false;
    if (hull_tolerances.m_post_filter == false)
        return;

    phys_geom_edge *edge_base = geom_hull.m_hull_edges.begin();
    phys_vec4 *vertex_base_ptr = geom_hull.m_vertex_base;

    {
        for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
            geom_hull.m_hull_edges[i].m_bvh_info = 0;
    }

    {
        for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
        {
            if (geom_hull.m_hull_edges[i].m_bvh_info != 1)
            {
                phys_geom_edge &edge = geom_hull.m_hull_edges[i];
                phys_geom_edge *mirror = edge.get_mirror_edge(edge_base);
                edge.m_bvh_info = 1;
                mirror->m_bvh_info = 1;
                int start_vertex_idx = edge.m_edge_vertex;
                int end_vertex_idx = mirror->m_edge_vertex;
                const phys_vec4 &start = vertex_base_ptr[start_vertex_idx];
                const phys_vec4 &end = vertex_base_ptr[end_vertex_idx];
                phys_vec4 dir;
                vec4_sub(end, start, dir);

                phys_array<int> collinear_ids;
                collinear_ids.clear();
                int sweep_min_index = start_vertex_idx;
                int sweep_max_index = end_vertex_idx;
                float min_dot = 1e-6f;
                float max_dot = dir.dot3(dir);

                if (end_vertex_idx != start_vertex_idx && start(3) == 0.0f && end(3) == 0.0f)
                {
                    for (int j = hull_start_vertex; j < hull_end_vertex + 1; j++)
                    {
                        const phys_vec4 &v = vertex_base_ptr[j];
                        if (start_vertex_idx != j && end_vertex_idx != j && v(3) == 0.0f)
                        {
                            phys_vec4 diff;
                            vec4_sub(v, start, diff);
                            float d = dir.dot3(diff);
                            phys_vec4 cross;
                            cross.set_cross(dir, diff);
                            if (cross.length_sq_3d() < hull_tolerances.m_degenerate_tol)
                            {
                                if (d < min_dot)
                                {
                                    collinear_ids.array_push_back(sweep_min_index);
                                    sweep_min_index = j;
                                    min_dot = d;
                                }
                                else if (d > max_dot)
                                {
                                    collinear_ids.array_push_back(sweep_max_index);
                                    sweep_max_index = j;
                                    max_dot = d;
                                }
                                else
                                {
                                    collinear_ids.array_push_back(j);
                                }
                            }
                        }
                    }
                }

                for (int k = 0; k < collinear_ids.getSize(); k++)
                {
                    if (collinear_ids[k] != sweep_min_index && collinear_ids[k] != sweep_max_index)
                    {
                        vertex_base_ptr[collinear_ids[k]](3) = 1.0f;
                        changed = true;
                    }
                }
            }
        }
    }

    {
        for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
            geom_hull.m_hull_edges[i].m_bvh_info = 0;
    }

    if (hull_end_vertex - hull_start_vertex > 2 && geom_hull.m_hull_edges.getSize() > 2)
    {
        for (int i = hull_start_vertex; i < hull_end_vertex + 1; i++)
        {
            if (vertex_base_ptr[i](3) == 0.0f)
            {
                phys_array<phys_vec4> normals;
                for (int e = 0; e < geom_hull.m_hull_edges.getSize(); e++)
                {
                    if (geom_hull.m_hull_edges[e].m_bvh_info != 1 && geom_hull.m_hull_edges[e].m_edge_vertex == i)
                    {
                        geom_hull.m_hull_edges[e].m_bvh_info = 1;
                        phys_geom_edge *first = geom_hull.m_hull_edges[e].get_mirror_edge(edge_base);
                        phys_geom_edge *current = first;
                        do
                        {
                            current->get_next_edge(edge_base)->m_bvh_info = 1;
                            const phys_vec4 *a = current->get_vertex(vertex_base_ptr);
                            const phys_vec4 *b = current->get_next_edge(edge_base)->get_vertex(vertex_base_ptr);
                            const phys_vec4 *c = current->get_next_edge(edge_base)->get_next_edge(edge_base)->get_vertex(vertex_base_ptr);
                            phys_vec4 e0;
                            phys_vec4 e1;
                            vec4_sub(*a, *b, e0);
                            vec4_sub(*c, *b, e1);
                            phys_vec4 *normal = normals.expand_by(1);
                            normal->set_cross(e0, e1);
                            normal->normalize_xyz();
                            current = current->get_next_edge(edge_base)->get_mirror_edge(edge_base);
                        } while (current != first);
                    }
                }

                if (normals.getSize() > 0)
                {
                    phys_vec4 average;
                    average.set_zero4();
                    int k;
                    for (k = 0; k < normals.getSize(); k++)
                        average.add4(normals[k]);
                    if (average.length_sq_3d() > hull_tolerances.m_coplanar_vertex_tol)
                    {
                        average.normalize_xyz();
                        phys_bool found(false);
                        for (k = 0; k < normals.getSize(); k++)
                        {
                            float d = normals[k].dot3(average);
                            if (d < 1.0f - hull_tolerances.m_coplanar_vertex_tol)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            vertex_base_ptr[i](3) = 1.0f;
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

// @address 0x6b26e8
void phys_hull_builder::build_convex_sub_hull(
    const phys_hull_tolerances &hull_tolerances, phys_array<phys_vec4> &used_vertices, int hull_start_vertex, int hull_end_vertex, phys_geom_hull &geom_hull)
{
    int hull_vertex_count = 0;
    for (int i = hull_start_vertex; i < hull_end_vertex + 1; i++)
        hull_vertex_count += (used_vertices[i](3) == 0.0f) ? 1 : 0;

    if (hull_vertex_count > 3)
    {
        int mid_vertex = (hull_end_vertex + hull_start_vertex) / 2;
        phys_geom_hull merge_left_hull;
        merge_left_hull.m_vertex_base = used_vertices.begin();
        phys_geom_hull merge_right_hull;
        merge_right_hull.m_vertex_base = used_vertices.begin();

        phys_bool changed(true);
        while (changed)
        {
            while (changed)
            {
                merge_left_hull.m_hull_edges.clear();
                build_convex_sub_hull(hull_tolerances, used_vertices, hull_start_vertex, mid_vertex, merge_left_hull);
                post_filter_vertices(merge_left_hull, hull_start_vertex, mid_vertex, hull_tolerances, changed);
            }
            changed = true;
            while (changed)
            {
                merge_right_hull.m_hull_edges.clear();
                build_convex_sub_hull(hull_tolerances, used_vertices, mid_vertex + 1, hull_end_vertex, merge_right_hull);
                post_filter_vertices(merge_right_hull, mid_vertex + 1, hull_end_vertex, hull_tolerances, changed);
            }
            phys_bool left_changed(true);
            post_filter_vertices(merge_left_hull, hull_start_vertex, hull_end_vertex, hull_tolerances, left_changed);
            phys_bool right_changed(true);
            post_filter_vertices(merge_right_hull, hull_start_vertex, hull_end_vertex, hull_tolerances, right_changed);
            changed = left_changed || right_changed;
        }

        if (merge_left_hull.m_hull_edges.getSize() == 0)
        {
            for (int i = 0; i < merge_right_hull.m_hull_edges.getSize(); i++)
                geom_hull.m_hull_edges.array_push_back(merge_right_hull.m_hull_edges[i]);
        }
        else if (merge_right_hull.m_hull_edges.getSize() == 0)
        {
            for (int i = 0; i < merge_left_hull.m_hull_edges.getSize(); i++)
                geom_hull.m_hull_edges.array_push_back(merge_left_hull.m_hull_edges[i]);
        }
        else
        {
            merge_hulls(hull_tolerances, merge_left_hull, merge_right_hull, geom_hull);
        }

        if (geom_hull.m_hull_edges.getSize() == 0)
            geom_hull.m_hull_edges = merge_left_hull.m_hull_edges;
    }
    else
    {
        geom_hull.m_vertex_base = used_vertices.begin();
        phys_inline_array<int, 3> vertices(0);
        for (int i = hull_start_vertex; i < hull_end_vertex + 1; i++)
        {
            if (used_vertices[i](3) == 0.0f)
                vertices.array_push_back(i);
        }
        switch (hull_vertex_count)
        {
        case 0:
            break;
        case 1:
            geom_hull.init_from_vertex(vertices[0]);
            break;
        case 2:
            geom_hull.init_from_edge(vertices[0], vertices[1]);
            break;
        default:
            geom_hull.init_from_triangle(vertices[0], vertices[1], vertices[2]);
            break;
        }
    }
}

// @address 0x6af768
phys_bool phys_hull_builder::build_plane_equations(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull,
    const phys_array<phys_vec4> &used_vertices, phys_vec4 &plane, phys_bool &planar, phys_array<phys_vec4> &plane_equations_out,
    phys_array<plane_and_points> &planes)
{
    phys_geom_edge *edge_base = geom_hull.m_hull_edges.begin();
    phys_vec4 *vertex_base_ptr = geom_hull.m_vertex_base;

    for (int i = 0; i < geom_hull.m_hull_edges.getSize(); i++)
    {
        phys_geom_edge edge = geom_hull.m_hull_edges[i];
        int next = edge.m_next;
        int next_next_edge = edge.get_next_edge(edge_base)->m_next;
        if (i < next && i < next_next_edge)
        {
            phys_vec4 *new_plane_eqn = plane_equations_out.expand_by(1);
            phys_vec4 v0 = *edge.get_vertex(geom_hull.m_vertex_base);
            phys_vec4 v1 = *edge_base[next].get_vertex(geom_hull.m_vertex_base);
            phys_vec4 v2 = *edge_base[next_next_edge].get_vertex(geom_hull.m_vertex_base);
            phys_vec4 e0;
            vec4_sub(v0, v1, e0);
            e0.normalize_xyz();
            phys_vec4 e1;
            vec4_sub(v2, v1, e1);
            e1.normalize_xyz();
            new_plane_eqn->set_cross(e0, e1);
            if (new_plane_eqn->length_sq_3d() < hull_tolerances.m_degenerate_tol)
            {
                plane_equations_out.set_array_size(plane_equations_out.getSize() - 1);
            }
            else
            {
                new_plane_eqn->normalize_xyz();
                (*new_plane_eqn)(3) = -float(new_plane_eqn->dot3(v0));
                phys_vec4 a;
                vec4_sub(v1, v0, a);
                a.set_mul4(0.5f, a);
                phys_vec4 b;
                vec4_sub(v2, v0, b);
                b.set_mul4(0.5f, b);
                phys_vec4 center;
                vec4_add(a, b, center);
                vec4_add(center, v0, center);
                plane_and_points *pp = planes.expand_by(1);
                pp->m_hull_plane = *new_plane_eqn;
                pp->m_hull_edges[0] = &geom_hull.m_hull_edges[i];
                pp->m_hull_edges[1] = &edge_base[next];
                pp->m_hull_edges[2] = &edge_base[next_next_edge];
                pp->sort();
            }
        }
    }

    planar = false;
    for (int i = 0; i < planes.getSize(); i++)
    {
        for (int j = i + 1; j < planes.getSize(); j++)
        {
            plane_and_points p1 = planes[i];
            plane_and_points p2 = planes[j];
            phys_vec4 sum;
            vec4_add(p1.m_hull_plane, p2.m_hull_plane, sum);
            if (sum.dot4_simd(sum) < hull_tolerances.m_opposite_normal_tol)
            {
                planar = true;
                p1.m_hull_plane(0) = -p2.m_hull_plane(0);
                p1.m_hull_plane(1) = -p2.m_hull_plane(1);
                p1.m_hull_plane(2) = -p2.m_hull_plane(2);
                if (plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[0], p2.m_hull_edges[0], p1.m_hull_edges[1], p2.m_hull_edges[1], p1, p2) ||
                    plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[0], p2.m_hull_edges[0], p1.m_hull_edges[1], p2.m_hull_edges[2], p1, p2))
                {
                    planar = true;
                    make_bevel_plane(p1.m_hull_plane, vertex_base_ptr[p1.m_hull_edges[0]->m_edge_vertex], vertex_base_ptr[p1.m_hull_edges[1]->m_edge_vertex],
                        vertex_base_ptr[p1.m_hull_edges[2]->m_edge_vertex], plane_equations_out);
                }
                if (plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[0], p2.m_hull_edges[0], p1.m_hull_edges[2], p2.m_hull_edges[1], p1, p2) ||
                    plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[0], p2.m_hull_edges[0], p1.m_hull_edges[2], p2.m_hull_edges[2], p1, p2))
                {
                    planar = true;
                    make_bevel_plane(p1.m_hull_plane, vertex_base_ptr[p1.m_hull_edges[0]->m_edge_vertex], vertex_base_ptr[p1.m_hull_edges[2]->m_edge_vertex],
                        vertex_base_ptr[p1.m_hull_edges[1]->m_edge_vertex], plane_equations_out);
                }
                if (plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[1], p2.m_hull_edges[0], p1.m_hull_edges[2], p2.m_hull_edges[1], p1, p2) ||
                    plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[1], p2.m_hull_edges[0], p1.m_hull_edges[2], p2.m_hull_edges[2], p1, p2) ||
                    plane_and_points::find_edge_pair(edge_base, p1.m_hull_edges[1], p2.m_hull_edges[1], p1.m_hull_edges[2], p2.m_hull_edges[2], p1, p2))
                {
                    planar = true;
                    make_bevel_plane(p1.m_hull_plane, vertex_base_ptr[p1.m_hull_edges[1]->m_edge_vertex], vertex_base_ptr[p1.m_hull_edges[2]->m_edge_vertex],
                        vertex_base_ptr[p1.m_hull_edges[0]->m_edge_vertex], plane_equations_out);
                }
            }
        }
    }

    if (planar)
    {
        phys_vec4 e0;
        e0.set_sub4(used_vertices[0], used_vertices[1]);
        phys_vec4 e1;
        e1.set_sub4(used_vertices[0], used_vertices[2]);
        plane.set_cross(e0, e1);
        plane.normalize_xyz();
        plane(3) = -plane.dot3(used_vertices[0]);
    }

    {
        phys_algorithm::quick_sort(plane_equations_out.begin(), plane_equations_out.getSize(), vec_less_merge_coords);
        int hull_plane_count;
        weld_x_sorted_vertices(hull_tolerances.m_coplanar_plane_tol, plane_equations_out, hull_plane_count);
    }

    if (plane_equations_out.getSize() < 2)
    {
        phys_vec4 v0;
        phys_vec4 v1;
        if (geom_hull.m_hull_edges.getSize() == 1)
        {
            v0 = *geom_hull.m_hull_edges[0].get_vertex(geom_hull.m_vertex_base);
            phys_vec4 offset;
            offset.set(1.0f, 0.0f, 0.0f, 0.0f);
            v1.set_add4(v0, offset);
        }
        else
        {
            v0 = *geom_hull.m_hull_edges[0].get_vertex(geom_hull.m_vertex_base);
            v1 = *geom_hull.m_hull_edges[0].get_mirror_edge(geom_hull.m_hull_edges.begin())->get_vertex(geom_hull.m_vertex_base);
        }
        phys_vec4 dir;
        dir.set_sub4(v0, v1);
        float min_dot = 3.40282e38f;
        phys_vec4 best;
        for (int i = 0; i < 3; i++)
        {
            phys_vec4 axis;
            axis.set(0.0f, 0.0f, 0.0f, 0.0f);
            axis(i) = 1.0f;
            float d = phys_math::fabs(dir.dot3(axis));
            if (d < min_dot)
            {
                min_dot = d;
                best = axis;
            }
        }
        int old_plane_cnt = plane_equations_out.getSize();
        plane_equations_out.set_array_size(old_plane_cnt + 6);
        phys_vec4 &p0 = plane_equations_out[old_plane_cnt];
        p0.set_cross(dir, best);
        p0.normalize_xyz();
        p0(3) = -p0.dot3(v0);
        phys_vec4 &p1 = plane_equations_out[old_plane_cnt + 1];
        p1.set_cross(dir, p0);
        p1.normalize_xyz();
        p1(3) = -p1.dot3(v0);
        phys_vec4 &p2 = plane_equations_out[old_plane_cnt + 2];
        p2.set_neg4(p0);
        p2(3) = -p2.dot3(v0);
        phys_vec4 &p3 = plane_equations_out[old_plane_cnt + 3];
        p3.set_neg4(p1);
        p3(3) = -p3.dot3(v0);
        phys_vec4 &p4 = plane_equations_out[old_plane_cnt + 4];
        p4 = dir;
        p4.normalize_xyz();
        p4(3) = -p4.dot3(v0);
        phys_vec4 &p5 = plane_equations_out[old_plane_cnt + 5];
        p5.set_neg4(p4);
        p5.normalize_xyz();
        p5(3) = (geom_hull.m_hull_edges.getSize() == 1) ? -p5.dot3(v0) : -p5.dot3(v1);
    }

    if (planar && hull_tolerances.m_accurate_but_slow)
    {
        plane_equations_out.clear();
        phys_array<phys_vec4> plane_vertices;
        generate_hull_from_planar_points(plane, used_vertices.begin(), used_vertices.getSize(), plane_vertices, plane_equations_out);
    }

    {
        phys_algorithm::quick_sort(plane_equations_out.begin(), plane_equations_out.getSize(), vec_less_merge_coords);
        int plane_cnt_out;
        weld_x_sorted_vertices(hull_tolerances.m_coplanar_plane_tol, plane_equations_out, plane_cnt_out);
    }
    return true;
}

