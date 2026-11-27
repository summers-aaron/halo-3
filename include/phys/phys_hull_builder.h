#ifndef INCLUDE_PHYS_HULL_BUILDER_H
#define INCLUDE_PHYS_HULL_BUILDER_H

#include "phys/phys_geom_edge.h"
#include "phys/phys_hull_tolerances.h"

template <typename T> class phys_array;
class phys_aabb;
class phys_geom_hull;
class phys_text_display;
struct phys_geometry;

struct phys_hull_config {
    float m_atom_max_angle;
    float m_line_length_weight;
    int m_max_iters;
};

#include "phys/phys_hull_mode.h"
class phys_hull_builder {
public:
    struct weighted_neighbour { phys_geom_edge *m_hull_edge; float m_neighbour_weight; };
    struct weighted_line {
        phys_geom_edge *m_edge_a;
        phys_geom_edge *m_edge_b;
        weighted_line *m_next;
        unsigned int m_last_vertex;
        float m_neighbour_weight;
        __forceinline phys_bool edges_and_vertex_equal(const weighted_line &other)
        {
            return phys_bool(m_edge_a == other.m_edge_a && m_edge_b == other.m_edge_b && m_last_vertex == other.m_last_vertex);
        }
        __forceinline phys_bool verts_and_vertex_equal(const weighted_line &other)
        {
            return phys_bool(m_edge_a->m_edge_vertex == other.m_edge_a->m_edge_vertex && m_edge_b->m_edge_vertex == other.m_edge_b->m_edge_vertex
                && m_last_vertex == other.m_last_vertex);
        }
    };
    struct wrapping_line {
        phys_geom_edge *m_edge_a;
        phys_geom_edge *m_edge_b;
        unsigned short m_left_vertex;
        unsigned short m_right_vertex;
        unsigned short m_left_next_edge_vertex;
        unsigned short m_right_next_edge_vertex;
    };
    class plane_and_points {
    public:
        phys_vec4 m_hull_plane;
        phys_geom_edge *m_hull_edges[3];
        void sort();
        static phys_bool find_edge_pair(phys_geom_edge *base, phys_geom_edge *first_edge_a, phys_geom_edge *first_edge_b, phys_geom_edge *second_a,
            phys_geom_edge *second_b, plane_and_points &first, plane_and_points &second);
    };
    static float get_pseudo_angle(float cos_theta, float hull_sin_theta);
    static void draw_plane(const phys_vec4 &a, const phys_vec4 &b, int c);
    static void convert_to_unit_cube(phys_array<phys_vec4> &vertices, phys_vec4 &scale, phys_vec4 &offset);
    static void convert_from_unit_cube(phys_array<phys_vec4> &vertices, phys_vec4 &scale, phys_vec4 &offset);
    static void make_bevel_plane(const phys_vec4 &bevel_plane_normal, const phys_vec4 &vertex0, const phys_vec4 &vertex1,
        const phys_vec4 &vertex2, phys_array<phys_vec4> &plane_equations_out);
    static void find_common_tangent(phys_geom_hull &left_hull, phys_geom_hull &right_hull, weighted_line &line, phys_vec4 &plane);
    static void weld_x_sorted_vertices(float tolerance, phys_array<phys_vec4> &vertices, int &hull_vertex_count);
    static void remove_flagged_vertices(phys_array<phys_vec4> &vertices);
    static void remove_unused_vertices(phys_geom_hull &geom_hull, phys_array<phys_vec4> &vertices);
    static void validate_neighbours(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *vertices,
        const phys_vec4 &plane, unsigned short vertex, weighted_line *line, const phys_vec4 *first_hull_vertices,
        const phys_vec4 *second_vertices, phys_array<weighted_neighbour> &left_neighbours, phys_array<weighted_neighbour> &right_neighbours);
    static void remove_coplanar_neighbours(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull, const phys_vec4 &plane,
        const phys_geom_edge *edge, const phys_vec4 *first_hull_vertices, const phys_vec4 *second_vertices, phys_array<weighted_neighbour> &neighbour_list);
    static void find_weighted_neighbours(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull, const phys_vec4 &plane, unsigned short vertex,
        const phys_geom_edge *edge, const phys_vec4 *first_hull_vertices, const phys_vec4 *second_vertices, phys_array<weighted_neighbour> &neighbour_list);
    static void build_convex_hull(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *vertices,
        int hull_vertex_count, phys_geom_hull &geom_hull, phys_array<phys_vec4> &vertices_out);
    static phys_bool vec_less_merge_coords(phys_vec4 &first, phys_vec4 &second);
    static void get_handle_aabb(const phys_array<phys_vec4> &vertices, phys_aabb &aabb);
    static void print_hull_edges(phys_geom_hull &geom_hull);
    static void append_reindex_edges(phys_geom_hull &source, phys_bool flag, phys_geom_hull &destination, phys_array<unsigned short> &remap);
    static void build_display_geometry(phys_geom_hull &geom_hull, phys_geometry &geometry);
    static phys_bool is_edge_check_valid(phys_geom_hull &geom_hull, phys_geom_edge *start_edge, int flag);
    static phys_bool is_valid_wrapping(phys_geom_hull &geom_hull, phys_geom_edge *other_base_edge, phys_bool first, phys_array<wrapping_line> &hull_wrapping);
    static void find_wrapping(phys_geom_hull &first_hull, phys_geom_hull &second_hull, const phys_array<weighted_line> &lines,
        phys_array<wrapping_line> &hull_wrapping);
    static void stitch_hulls(phys_geom_hull &left_hull, phys_geom_hull &right_hull, phys_array<wrapping_line> &hull_wrapping, phys_geom_hull &out_hull);
public:
  static void draw(phys_geom_hull &geom_hull, int, class phys_text_display *);
  static void get_plane_equation_z_axis(class phys_vec4 const &p0, class phys_vec4 const &p1, class phys_vec4 &p2);
public:
  static float get_angle_between_vertex_and_plane(const struct phys_hull_tolerances &hull_tol, const class phys_vec4 &v, const class phys_vec4 &n,
      const class phys_vec4 &p1, const class phys_vec4 &p2);
public:
  static void compute_new_neighbours(class phys_vec4 const *p0, struct phys_hull_tolerances const &p1,
      float p2, class phys_vec4 const &p3, class phys_geom_edge const *p4, class phys_vec4 const *p5,
      class phys_vec4 const *p6, class phys_array<struct phys_hull_builder::weighted_neighbour> &p7);
    static phys_bool build_plane_equations(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull, const phys_array<phys_vec4> &used_vertices,
        phys_vec4 &plane, phys_bool &planar, phys_array<phys_vec4> &plane_equations_out, phys_array<plane_and_points> &planes);
    static phys_bool build_plane_equations(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &geom_hull,
        const phys_array<phys_vec4> &used_vertices, phys_array<phys_vec4> &plane_equations_out);
    static void generate_hull_from_planar_points(const phys_vec4 &plane, const phys_vec4 *verts, int hull_vertex_count,
        phys_array<phys_vec4> &used_vertices_out, phys_array<phys_vec4> &plane_vertices_out);
    static void remove_collinear_vertices(phys_array<phys_vec4> &verts, float tolerance);
    static void generate_convex_hull(const phys_vec4 *verts, int hull_vertex_count, phys_geom_hull &out_hull,
        phys_array<phys_vec4> &used_vertices_out, phys_hull_mode mode);
    static void post_filter_vertices(phys_geom_hull &geom_hull, int hull_start_vertex, int hull_end_vertex,
        const phys_hull_tolerances &hull_tolerances, phys_bool &changed);
    static void build_convex_sub_hull(const phys_hull_tolerances &hull_tolerances, phys_array<phys_vec4> &used_vertices,
        int hull_start_vertex, int hull_end_vertex, phys_geom_hull &geom_hull);
public:
  static void add_wrapping_lines(struct phys_hull_config const &p0, struct phys_hull_builder::weighted_line *p1,
      class phys_array<struct phys_hull_builder::weighted_neighbour> &p2, class phys_array<struct phys_hull_builder::weighted_neighbour> &p3,
      class phys_array<struct phys_hull_builder::weighted_line> &p4);
    static phys_result merge_hulls(const phys_hull_tolerances &hull_tolerances, phys_geom_hull &left_hull, phys_geom_hull &right_hull,
        phys_geom_hull &out_hull);
    static phys_bool is_single_line(float tolerance, phys_geom_hull &hull_a, phys_geom_hull &hull_b, phys_geom_hull &out_hull);
    static void approx_hull_from_kdop(const phys_vec4 *verts, int hull_vertex_count,
        phys_array<phys_vec4> &plane_equations_out, phys_array<phys_vec4> &used_vertices_out);
};

#include "phys/phys_algorithm_sort.h"

namespace phys_algorithm {
    template <>
    void quick_sort_recursive<phys_hull_builder::weighted_line,
        phys_bool (*)(const phys_hull_builder::weighted_line &, const phys_hull_builder::weighted_line &)>(
        phys_hull_builder::weighted_line *, int, int,
        phys_bool (*)(const phys_hull_builder::weighted_line &, const phys_hull_builder::weighted_line &));
}

#endif

