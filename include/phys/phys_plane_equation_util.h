#ifndef INCLUDE_PHYS_PLANE_EQUATION_UTIL_H
#define INCLUDE_PHYS_PLANE_EQUATION_UTIL_H

#include "phys/phys_base.h"
#include "phys/phys_algorithm.h"

// Builds the vertices of a convex hull from its plane equations (hkPlaneEquationUtil.obj).
class phys_plane_equation_util {
public:
    struct plane_intersection {
        void sort()
        {
            if (m_plane_a > m_plane_b) phys_algorithm::swap(m_plane_a, m_plane_b);
        }
        phys_vec4 m_location;
        int m_plane_a;
        int m_plane_b;
        int m_plane_c;
    };
    struct phys_plane_eqn_index_pair {
        phys_bool operator==(const phys_plane_eqn_index_pair &other)
        {
            return ((m_first == other.m_first &&
                     m_second == other.m_second));
        }
        int m_first;
        int m_second;
    };
    struct visited_edge {
        phys_bool operator==(const visited_edge &other)
        {
            return (((m_plane_a == other.m_plane_a) &&
                     (m_plane_b == other.m_plane_b)) ||
                    ((m_plane_b == other.m_plane_a) &&
                     (m_plane_a == other.m_plane_b)));
        }
        int m_plane_a;
        int m_plane_b;
        plane_intersection *m_from_point;
        phys_vec4 m_intersect_line_dir;
    };
    static phys_result find_tri_plane_intersection(const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index, int plane_b_index,
        int plane_c_index, plane_intersection &out_intersect_point);
    static phys_result find_planar_intersections(const phys_vec4 &intersect_line_dir, const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index,
        int plane_b_index, plane_intersection &upper_point_out, plane_intersection &lower_point_out);
    static phys_bool is_next_intersection(const phys_vec4 &intersect_line_dir, const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index,
        int plane_b_index, phys_vec4 &vertex, phys_array<plane_intersection> &capsule_points_out);
    static void walk_along_edge(visited_edge &v, const phys_array<phys_vec4> &hull_plane_equations,
        phys_array<visited_edge> &visited_edges, phys_array<phys_vec4> &vertex_cloud_out);
    static phys_result get_starting_point(const phys_array<phys_vec4> &hull_plane_equations, int &near_parallel_plane_lo, int &near_parallel_plane_hi,
        plane_intersection &lower_point, plane_intersection &upper_point);
    static void compute_intersection_line(
        const phys_array<phys_vec4> &hull_plane_equations, int plane_a_index, int plane_b_index, int plane_c_index, phys_vec4 &intersect_line_dir);
    static phys_result hull_verts_from_planes(const phys_array<phys_vec4> &hull_plane_equations, phys_array<phys_vec4> &vertex_cloud_out);
};

#endif
