#ifndef INCLUDE_PHYS_SIMPLEX_TYPES_H
#define INCLUDE_PHYS_SIMPLEX_TYPES_H

#include <stddef.h>

#include "phys/phys_collide.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_simplex_status.h"
#include "phys/phys_simplex_out.h"

// March 0x82bf8288 and 0x82bf8428 establish the state and initialization.
class phys_simplex {
public:
    enum next_case { SIMPLEX_NEXT_BREAK, SIMPLEX_NEXT_PENETRATION, SIMPLEX_NEXT_EDGE_EDGE };
    enum
    {
        SIMPLEX_NONE_NONE = 0,
        SIMPLEX_POINT_POINT = 9,
        SIMPLEX_POINT_EDGE = 10,
        SIMPLEX_EDGE_POINT = 17,
        SIMPLEX_POINT_TRIANGLE = 11,
        SIMPLEX_TRIANGLE_POINT = 25,
        SIMPLEX_EDGE_TRIANGLE = 19,
        SIMPLEX_TRIANGLE_EDGE = 26,
        SIMPLEX_POINT_TETRA = 12,
        SIMPLEX_TETRA_POINT = 33,
        SIMPLEX_EDGE_EDGE = 18
    };
    enum support_kind { SIMPLEX_SUPPORT_NEG = -8, SIMPLEX_SUPPORT_NONE = 0, SIMPLEX_SUPPORT_POS = 8 };
    enum support_state { SIMPLEX_SUPPORT_CHANGED = 0, SIMPLEX_SKIP_SUPPORT_A = SIMPLEX_SUPPORT_POS, SIMPLEX_SKIP_SUPPORT_B = SIMPLEX_SUPPORT_NEG };
public:
    phys_padded<int> m_dim_a;
    phys_padded<int> m_dim_b;
    phys_padded<int> m_max_dim_a;
    phys_padded<int> m_max_dim_b;
    phys_padded<unsigned int> m_skip_penetration;
    phys_padded<int> m_feature_changed;
    phys_hull_vertex m_vertices_a[4];
    phys_vec4 m_dummy[4];
    phys_vec4 m_vertices_b_in_a[4];
    phys_hull_vertex m_vertices_b_in_b[4];
    phys_vec4 m_support_vec;
    phys_collide_triangle_util::segment_segment_result m_closest_line_result;
    phys_vec4 m_check_triangle_dots;
    phys_padded<int> m_last_dim_b;
    int m_support_state;
    void print();
    static void simplex_code_begin();
    static void simplex_code_end();
    void feature_to_closest_distance(const phys_vec4 &normal, phys_simplex_out &output);
    void exit_and_export_cache(phys_simplex_cache &cache) const;
    __forceinline phys_simplex() : m_skip_penetration(0) {}
    __forceinline void init(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b, const phys_simplex_cache &cache);
    phys_simplex_status get_closest_feature(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
        const phys_transform &b_to_a_transform, phys_vec4 &separating_normal_out);
    struct closest_point_input;
    static phys_result get_closest_point(const closest_point_input &input, phys_simplex_cache &cache, phys_vec4 &separating_normal_out,
        phys_vec4 &point_on_b_out);
    __forceinline void refresh_simplex_cache(phys_simplex_cache &cache);
    // The closest point on A for the current simplex: a vertex of A, a vertex of B moved back along the normal by the
    // distance in its w, or the closest point of the edge-edge case.
    __forceinline void get_closest_point_a_in_a(const phys_vec4 &simplex_separating_normal, phys_vec4 &point_in_a) {
        if (m_dim_a == 1) {
            point_in_a = m_vertices_a[0];
        } else if (m_dim_b == 1) {
            point_in_a = m_vertices_b_in_a[0];
            point_in_a.m_simd_quad = __vmaddfp(simplex_separating_normal.m_simd_quad, __vspltw(simplex_separating_normal.m_simd_quad,
                3), point_in_a.m_simd_quad);
        } else {
            point_in_a = m_closest_line_result.m_closest_point_a;
        }
    }
public:
    phys_bool trace_ray(const phys_convex_shape *shape, const phys_ray_input &input, phys_ray_output &output);
public:
    phys_bool sweep_cast(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b, const phys_rotation &rotation,
        const phys_ray_input &input, phys_ray_output &output);
protected:
    enum reduce_dim_result { SIMPLEX_REDUCE_OK, SIMPLEX_REDUCE_PENETRATION };
    __forceinline reduce_dim_result reduce_dimension();
    __forceinline void set_feature_change(int f) { m_feature_changed = f; }
    int check_triangle_boundaries(const phys_vec4 &a, phys_vec4 *three_vertices, support_kind support);
    next_case process_edge_triangle(phys_vec4 *capsule_vertex_a, phys_vec4 *capsule_vertex_b, phys_padded<int> &dim_a, phys_padded<int> &dim_b,
        phys_bool from_edge_edge, support_kind support_type);
    void reduce_dimension_ext();
    void handle_penetration(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b, const phys_transform &a_from_b);
};
typedef char phys_simplex_state_size[(sizeof(phys_simplex) == 384) ? 1 : -1];
typedef char phys_simplex_verts_a_offset[(offsetof(phys_simplex, m_vertices_a) == 32) ? 1 : -1];

struct phys_simplex::closest_point_input {
    phys_padded<const phys_transform *> m_a_to_b;
    phys_padded<const phys_transform *> m_transform_a;
    phys_padded<const phys_convex_shape *> m_shape_a;
    phys_padded<const phys_convex_shape *> m_shape_b;
    phys_padded<float> m_collision_tolerance;
};
__forceinline void phys_simplex::init(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
    const phys_simplex_cache &cache)
{
    m_dim_a = cache.m_dim_a;
    m_dim_b = cache.m_dim_b;
    m_max_dim_a = cache.m_max_dim_a;
    m_max_dim_b = cache.m_max_dim_b;

    set_feature_change(0);
    {
        penetration_shape_a->vertex_ids_to_vertices(&cache.m_vertices[0], m_dim_a, &m_vertices_a[0]);
        penetration_shape_b->vertex_ids_to_vertices(&cache.m_vertices[m_dim_a], m_dim_b, &m_vertices_b_in_b[0]);
    }
}
__forceinline void phys_simplex::refresh_simplex_cache(phys_simplex_cache &cache)
{
    if (!m_feature_changed)
    {
        return;
    }
    exit_and_export_cache(cache);
}


#endif
