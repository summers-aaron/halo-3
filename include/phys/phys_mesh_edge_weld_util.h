#ifndef INCLUDE_PHYS_MESH_EDGE_WELD_UTIL_H
#define INCLUDE_PHYS_MESH_EDGE_WELD_UTIL_H
#include "phys/phys_math.h"
class phys_bvh_shape;
class phys_triangle_shape;

extern "C" float k_mesh_edge_weld_degenerate_tol;
class phys_mesh_edge_weld_util {
protected:
    static int build_singular_vertices(const phys_vec4 *vertices_a, const phys_vec4 *vertices_b, int edge, phys_vec4 *vertices_out);
    static int compute_edge_angle_bitcode(const phys_vec4 *vertices);
    static float compute_angle_from_vertices(const phys_vec4 *vertices, float &weld_cos_angle, float &edge_weld_sin_angle);
    static unsigned short modify_combined_edge_bits(unsigned short combined, int edge, int edge_bitcode);
    static void compute_triangle_edge_bitcode(const phys_bvh_shape *tree, const phys_triangle_shape *triangle, unsigned int key,
        int edge, short &edge_weld_info);
    static void compute_ccw_triangle_normal(const phys_vec4 &vertex0, const phys_vec4 &vertex1, const phys_vec4 &vertex2, phys_vec4 &normal);
public:
    static unsigned short compute_triangle_weld_info(unsigned int key, const phys_bvh_shape *tree);
};
#endif
