// hkcollide/hkMeshWeldingUtility.obj: from the records 0x706828 0x706850 0x706978 0x7069e8 0x706b48 0x706be0 0x706dc8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x706828 ?modify_combined_edge_bits@phys_mesh_edge_weld_util@@KAGGHH@Z
// @address 0x706850 ?build_singular_vertices@phys_mesh_edge_weld_util@@KAHPBVphys_vec4@@0HPAV2@@Z
// @address 0x706b48 ?compute_edge_angle_bitcode@phys_mesh_edge_weld_util@@KAHPBVphys_vec4@@@Z
// @address 0x706dc8 ?compute_triangle_weld_info@phys_mesh_edge_weld_util@@SAGIPBVphys_bvh_shape@@@Z
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "phys/phys_math_types.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_triangle_util.h"
#include "phys/phys_triangle_shape.h"

class phys_shape_key_array;
typedef __declspec(align(16)) char phys_shape_buffer[512];
#include "phys/phys_mesh_edge_weld_util.h"
#include "phys/phys_thread_memory.h"

// @address 0x72c4d0 k_mesh_edge_weld_degenerate_tol
float k_mesh_edge_weld_degenerate_tol = 1e-7f;

unsigned short phys_mesh_edge_weld_util::modify_combined_edge_bits(unsigned short combined, int edge, int edge_bitcode)
{
    edge_bitcode <<= edge * 5;
    int mask = ~(31 << (edge * 5));
    combined = (unsigned short)(combined & mask);
    combined = (unsigned short)(combined | edge_bitcode);
    return combined;
}

int phys_mesh_edge_weld_util::build_singular_vertices(const phys_vec4 *vertices_a, const phys_vec4 *vertices_b, int edge, phys_vec4 *vertices_out)
{
    vertices_out[0] = vertices_a[edge % 3];
    vertices_out[1] = vertices_a[(edge + 1) % 3];
    vertices_out[2] = vertices_a[(edge + 2) % 3];
    int hull_vertex_count = 3;
    for (int i = 0; i < 3; ++i) {
        phys_bool duplicate = false;
        for (int j = 0; j < 2; ++j) {
            phys_vec4 difference;
            difference.set_sub4(vertices_out[j], vertices_b[i]);
            if (float(difference.length_sq_3d()) == 0.0f) {
                duplicate = true;
                break;
            }
        }
        if (duplicate == false) {
            phys_vec4 *vertex = &vertices_out[hull_vertex_count++];
            *vertex = vertices_b[i];
        }
    }
    return hull_vertex_count;
}

int phys_mesh_edge_weld_util::compute_edge_angle_bitcode(const phys_vec4 *vertices)
{
    float weld_cos_angle;
    float edge_weld_sin_angle;
    float angle = compute_angle_from_vertices(vertices, weld_cos_angle, edge_weld_sin_angle);
    int i;
    for (i = 0; i < 32; ++i) {
        float quantized_angle = -3.1415927f + 0.2026834f * float(i);
        if (angle <= quantized_angle) {
            if (angle < 0.0f && angle < quantized_angle) {
                --i;
            }
            break;
        }
    }
    return i;
}

unsigned short phys_mesh_edge_weld_util::compute_triangle_weld_info(unsigned int key, const phys_bvh_shape *tree)
{
    const phys_shape_container *container = tree->get_shape_container();
    phys_shape_buffer buffer;
    const phys_shape *shape = container->get_child_shape(key, buffer);
    const phys_triangle_shape *triangle = static_cast<const phys_triangle_shape *>(shape);
    if (phys_triangle_util::is_degenerate(triangle->get_vertex(0), triangle->get_vertex(1), triangle->get_vertex(2), k_mesh_edge_weld_degenerate_tol)) {
        return 0;
    } else {
        short edge_weld_info = 0;
        for (int edge = 0; edge < 3; ++edge) {
            compute_triangle_edge_bitcode(tree, triangle, key, edge, edge_weld_info);
        }
        return (unsigned short)edge_weld_info;
    }
}

// @address 0x706978 ?compute_ccw_triangle_normal@phys_mesh_edge_weld_util@@KAXABVphys_vec4@@00AAV2@@Z
void phys_mesh_edge_weld_util::compute_ccw_triangle_normal(const phys_vec4 &vertex0, const phys_vec4 &vertex1, const phys_vec4 &vertex2, phys_vec4 &normal)
{
    phys_vec4 edges[2];
    {
        edges[0].set_sub4(vertex1, vertex0);
        edges[1].set_sub4(vertex2, vertex1);
    }
    normal.set_cross(edges[0], edges[1]);
    normal.normalize_xyz();
}

// @address 0x7069e8 ?compute_angle_from_vertices@phys_mesh_edge_weld_util@@KAMPBVphys_vec4@@AAM1@Z
float phys_mesh_edge_weld_util::compute_angle_from_vertices(const phys_vec4 *vertices, float &sin_angle_out, float &cos_angle_out)
{
    phys_vec4 tri0_normal;
    phys_mesh_edge_weld_util::compute_ccw_triangle_normal(vertices[0], vertices[1], vertices[2], tri0_normal);

    phys_vec4 tri1_normal;
    phys_mesh_edge_weld_util::compute_ccw_triangle_normal(vertices[1], vertices[0], vertices[3], tri1_normal);

    phys_vec4 orth_normal_edge;
    {
        phys_vec4 edge_1_to_0;
        edge_1_to_0.set_sub4(vertices[1], vertices[0]);
        edge_1_to_0.normalize_xyzw();
        orth_normal_edge.set_cross(edge_1_to_0, tri0_normal);
    }

    sin_angle_out = tri1_normal.dot3(orth_normal_edge);
    cos_angle_out = tri1_normal.dot3(tri0_normal);
    return phys_math_atan2f_approx(sin_angle_out, cos_angle_out);
}

extern "C" {
}
// @address 0x706be0 ?compute_triangle_edge_bitcode@phys_mesh_edge_weld_util@@KAXPBVphys_bvh_shape@@PBVphys_triangle_shape@@IHAAF@Z
void phys_mesh_edge_weld_util::compute_triangle_edge_bitcode(
    const phys_bvh_shape *tree, const phys_triangle_shape *triangle, unsigned int key, int edge, short &edge_weld_info)
{
    phys_inline_array<unsigned int, 128> edge_adjacent_keys;
    const phys_vec4 *tri0_vertices = &triangle->get_vertex(0);
    {
        const float weld_query_radius = 0.0001f;
        phys_vec4 min; min.set(-weld_query_radius, -weld_query_radius, -weld_query_radius);
        phys_vec4 max; max.set(weld_query_radius, weld_query_radius, weld_query_radius);
        phys_vec4 edge_vertex = tri0_vertices[edge % 3];
        phys_aabb aabb;
        min.add4(edge_vertex);
        max.add4(edge_vertex);
        aabb.m_min_limit = min;
        aabb.m_max_limit = max;
        tree->query_aabb(aabb, edge_adjacent_keys);
    }

    for (int hit_list_idx = edge_adjacent_keys.getSize() - 1; hit_list_idx >= 0; hit_list_idx--)
    {
        unsigned int neighbor_tri_key = edge_adjacent_keys[hit_list_idx];
        if (neighbor_tri_key == key)
        {
            continue;
        }

        const phys_vec4 *tri1_vertices;
        {
            const phys_triangle_shape *adjacent_triangle;
            phys_shape_buffer buffer;
            {
                const phys_shape *adjacent_shape = tree->get_shape_container()->get_child_shape(neighbor_tri_key, buffer);
                if (adjacent_shape->get_type_id() != 5)
                {
                    continue;
                }
                adjacent_triangle = static_cast<const phys_triangle_shape *>(adjacent_shape);
            }
            tri1_vertices = &adjacent_triangle->get_vertex(0);
        }

        phys_vec4 singular_vertices[6];
        if (build_singular_vertices(tri0_vertices, tri1_vertices, edge, singular_vertices) == 4)
        {
            int edge_bitcode = compute_edge_angle_bitcode(singular_vertices);
            int shift = edge * 5;
            unsigned short combined = *(unsigned short *)&edge_weld_info;
            *(unsigned short *)&edge_weld_info = (unsigned short)((combined & ~(31 << shift)) | (edge_bitcode << shift));
        }
    }
}

