#ifndef INCLUDE_PHYS_COLLIDE_TRIANGLE_UTIL_H
#define INCLUDE_PHYS_COLLIDE_TRIANGLE_UTIL_H
#include "phys/phys_math.h"
class phys_collide_triangle_util {
public:
    struct segment_segment_result {
        phys_vec4 m_closest_point_a;
        phys_vec4 m_closest_a_minus_b;
        phys_padded<float> m_dist_sq;
    };
    static int closest_segment_pair(const phys_vec4 &A, const phys_vec4 &dA, const phys_vec4 &B, const phys_vec4 &dB, segment_segment_result &result);
    struct point_segment_result {
        phys_vec4 m_edge_point;
    };
    struct closest_point_tri_cache {
        float m_values[4];
    };
    struct point_tri_distance_cache {
        float m_inv_edge_len[3];
        float m_inv_normal_len;
        float m_normal_len;
    };
    static int closest_point_on_segment(const phys_vec4 &A, const phys_vec4 &B, const phys_vec4 &B2, point_segment_result &result);
    static void compute_barycentric_coords(const phys_vec4 &pos, const phys_vec4 &t0, const phys_vec4 &t1, const phys_vec4 &t2,
        phys_padded<float> *const result);
    static void setup_point_triangle_distance_cache(const phys_vec4 *triangle, point_tri_distance_cache &cache);
    static void compute_triangle_plane_dirs(const phys_vec4 *tri_corners, const point_tri_distance_cache &cache,
        phys_transform &plane_equations_out, phys_vec4 &normal_out);
public:
  static void setup_closest_point_triangle_cache(const phys_vec4 *triangle, closest_point_tri_cache &cache);
    struct closest_point_tri_result {
        phys_vec4 triangle_hit_direction;
        phys_padded<float> distance;
    };
    enum closest_point_tri_status {
        TRI_HIT_FACE = 0,
        TRI_HIT_EDGE
    };
    static closest_point_tri_status closest_point_on_triangle(const phys_vec4 &position, const phys_vec4 *tri_corners,
        const closest_point_tri_cache &cache, closest_point_tri_result &result);
    static const signed char mask_to_index[];
    static const signed char vertex_to_edge_lut[];
    // March 2007 (0x8354a680, 0x8354a6a0): (i + 1) % 3 and (i + 2) % 3 through the lookup table.
    static __forceinline int next_mod3(int i) { return vertex_to_edge_lut[i + 2]; }
    static __forceinline int get_prev_modulo3(int i) { return vertex_to_edge_lut[i]; }
};
#endif
