// hkinternal/hkWingedEdge.obj: from the records 0x6b3f90 0x6b3f98 0x6b3fa0 0x6b4030 0x6b41a0 0x6b4260 0x6b4490 0x6b45e0 0x6b4828 0x6b48b8 0x6b4b00 0x6b4b80
// @compiler xdk5632
// @flags /TP /O2
#include "phys/phys_base_types.h"
// @address 0x6b3f90 ?simplex_code_begin@phys_winged_edge@@SAXXZ
// @address 0x6b3f98 ?simplex_code_end@phys_winged_edge@@SAXXZ
// @address 0x6b4b80 ?reset_triangle_and_vertex_states@phys_winged_edge@@IAAXXZ
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_base.h"

typedef float phys_real;
typedef __vector4 phys_simd_vec;




namespace phys_winged_edge_math {
            }



#include "phys/phys_thread_memory.h"
#include "phys/phys_winged_edge.h"


struct phys_tetrahedron_edge {
    unsigned long m_vertex_offset;
    unsigned long m_triangle_offset;
    unsigned long m_edge_offset;
};

static __forceinline phys_thread_memory &phys_winged_edge_thread_memory()
{
    return *(phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
}



static const phys_real minimum_tolerance = 1e-7f;
static const phys_real edge_visibility_tolerance = 1e-5f;


static const phys_tetrahedron_edge tetrahedron_edges[12] = {
    { 0 * sizeof(phys_winged_edge::Vertex), 2 * sizeof(phys_winged_edge::geometry_triangle), 2 * sizeof(phys_winged_edge::hull_edge) },
    { 2 * sizeof(phys_winged_edge::Vertex), 3 * sizeof(phys_winged_edge::geometry_triangle), 0 * sizeof(phys_winged_edge::hull_edge) },
    { 1 * sizeof(phys_winged_edge::Vertex), 1 * sizeof(phys_winged_edge::geometry_triangle), 0 * sizeof(phys_winged_edge::hull_edge) },
    { 0 * sizeof(phys_winged_edge::Vertex), 0 * sizeof(phys_winged_edge::geometry_triangle), 2 * sizeof(phys_winged_edge::hull_edge) },
    { 1 * sizeof(phys_winged_edge::Vertex), 3 * sizeof(phys_winged_edge::geometry_triangle), 2 * sizeof(phys_winged_edge::hull_edge) },
    { 3 * sizeof(phys_winged_edge::Vertex), 2 * sizeof(phys_winged_edge::geometry_triangle), 0 * sizeof(phys_winged_edge::hull_edge) },
    { 0 * sizeof(phys_winged_edge::Vertex), 1 * sizeof(phys_winged_edge::geometry_triangle), 2 * sizeof(phys_winged_edge::hull_edge) },
    { 3 * sizeof(phys_winged_edge::Vertex), 3 * sizeof(phys_winged_edge::geometry_triangle), 1 * sizeof(phys_winged_edge::hull_edge) },
    { 2 * sizeof(phys_winged_edge::Vertex), 0 * sizeof(phys_winged_edge::geometry_triangle), 0 * sizeof(phys_winged_edge::hull_edge) },
    { 1 * sizeof(phys_winged_edge::Vertex), 0 * sizeof(phys_winged_edge::geometry_triangle), 1 * sizeof(phys_winged_edge::hull_edge) },
    { 2 * sizeof(phys_winged_edge::Vertex), 2 * sizeof(phys_winged_edge::geometry_triangle), 1 * sizeof(phys_winged_edge::hull_edge) },
    { 3 * sizeof(phys_winged_edge::Vertex), 1 * sizeof(phys_winged_edge::geometry_triangle), 1 * sizeof(phys_winged_edge::hull_edge) },
};


void phys_winged_edge::simplex_code_begin()
{
}

void phys_winged_edge::simplex_code_end()
{
}

inline void phys_winged_edge::reset_triangle_and_vertex_states()
{
    int index;
    for (index = 0; index < m_triangle_count; index++) {
        m_triangle_list[index].m_state = 0;
    }
    for (index = 0; index < m_hull_vertex_count; index++) {
        m_vertices[index].m_hull_edge = 0;
    }
}

// @address 0x6b4b00 ?classify_and_update_tolerance@geometry_triangle@phys_winged_edge@@QBA?BW4triangle_state@12@ABVphys_vec4@@AAV?$phys_padded@M@@1@Z
inline const phys_winged_edge::geometry_triangle::triangle_state phys_winged_edge::geometry_triangle::classify_and_update_tolerance(
    const phys_vec4 &point, phys_padded<float> &min_tolerance, phys_padded<float> &max_tolerance) const
        {
            const phys_real distance = distance_to_triangle(point);
            triangle_state state = HULL_TRI_SHOWN;

            if (distance > min_tolerance) {
                if (distance < max_tolerance) {
                    max_tolerance = distance;
                }
            } else {
                if (distance + minimum_tolerance > min_tolerance) {
                    min_tolerance = distance + minimum_tolerance;
                }
                state = HULL_TRI_HIDDEN;
            }

            if (min_tolerance >= max_tolerance) {
                state = HULL_TRI_FAILED;
            }
            return state;
        }

// @address 0x6b3fa0 ?compute_triangle_data@geometry_triangle@phys_winged_edge@@QAAXXZ
void phys_winged_edge::geometry_triangle::compute_triangle_data()
{
    const phys_vec4 &v0 = m_hull_edges[0].m_edge_vertex->m_edge_vertex;
    const phys_vec4 &v1 = m_hull_edges[1].m_edge_vertex->m_edge_vertex;
    const phys_vec4 &v2 = m_hull_edges[2].m_edge_vertex->m_edge_vertex;
    phys_vec4 e0;
    e0.set_sub4(v0, v1);
    phys_vec4 e1;
    e1.set_sub4(v1, v2);
    m_hull_plane.set_cross(e0, e1);
    m_hull_plane.normalize_xyz();
    m_plane_distance = m_hull_plane.dot3(m_hull_edges[0].m_edge_vertex->m_edge_vertex);
}

// @address 0x6b4030 ?find_visible_triangles@phys_winged_edge@@IAA?AW4winged_edge_status@1@PAUgeometry_triangle@1@ABVphys_vec4@@AAHAAPAUhull_edge@1@@Z
phys_winged_edge::winged_edge_status phys_winged_edge::find_visible_triangles(geometry_triangle *triangle, const phys_vec4 &point,
    int &hull_edge_count, hull_edge *&last_edge)
{
    winged_edge_status status = WINGED_EDGE_OK;

    for (int i = 0; i < 3; i++) {
        hull_edge *edge = triangle->m_hull_edges[i].m_twin_edge;
        geometry_triangle *neighbour_vertex = edge->m_edge_triangle;

        if (neighbour_vertex->m_state == geometry_triangle::HULL_TRI_UNKNOWN) {
            neighbour_vertex->m_state = neighbour_vertex->classify_and_update_tolerance(point, m_min_tolerance, m_max_tolerance);
            if (neighbour_vertex->m_state == geometry_triangle::HULL_TRI_SHOWN) {
                status = find_visible_triangles(neighbour_vertex, point, hull_edge_count, last_edge);
                if (status == WINGED_EDGE_FAILED) {
                    break;
                }
            } else if (neighbour_vertex->m_state == geometry_triangle::HULL_TRI_FAILED) {
                status = WINGED_EDGE_FAILED;
                break;
            }
        }

        if (neighbour_vertex->m_state == geometry_triangle::HULL_TRI_HIDDEN) {
            edge->m_edge_vertex->m_hull_edge = edge;
            hull_edge_count++;
            last_edge = edge;
        }
    }

    triangle->m_plane_distance = 3.40282e+38f;

    m_free_triangles[m_free_triangle_count] = triangle;
    m_free_triangle_count = m_free_triangle_count + 1;
    if (m_free_triangle_count >= PHYS_WINGED_EDGE_TRIANGLE_LIMIT) {
        status = WINGED_EDGE_FAILED;
    }
    return status;
}

// @address 0x6b41a0 ?allocate_triangles@phys_winged_edge@@IAAXAAV?$phys_array@PAUgeometry_triangle@phys_winged_edge@@@@H@Z
void phys_winged_edge::allocate_triangles(phys_array<geometry_triangle *> &new_triangles, int hull_triangle_count)
{
    int i = m_free_triangle_count - 1;
    while (hull_triangle_count != 0 && i >= 0) {
        new_triangles.push_back_unchecked(m_free_triangles[i]);
        m_free_triangle_count = m_free_triangle_count - 1;
        hull_triangle_count--;
        i--;
    }

    for (int j = 0; j < hull_triangle_count; j++) {
        geometry_triangle *triangle = &m_triangle_list[m_triangle_count];
        m_triangle_count = m_triangle_count + 1;
        new_triangles.push_back_unchecked(triangle);
    }
}


// @address 0x6b4260 ?add_tetrahedron@phys_winged_edge@@IAAXXZ
void phys_winged_edge::add_tetrahedron()
{
    m_triangle_count = 4;

    const phys_tetrahedron_edge *tet_edge_desc = tetrahedron_edges;
    geometry_triangle *end = m_triangle_list + m_triangle_count;
    for (geometry_triangle *triangle = m_triangle_list; triangle != end; triangle++) {
        triangle->m_state = geometry_triangle::HULL_TRI_UNKNOWN;
        triangle->m_hull_edges[0].m_next = &triangle->m_hull_edges[1];
        triangle->m_hull_edges[1].m_next = &triangle->m_hull_edges[2];
        triangle->m_hull_edges[2].m_next = &triangle->m_hull_edges[0];
        for (hull_edge *edge = triangle->m_hull_edges; edge < triangle->m_hull_edges + 3; edge++) {
            edge->m_edge_triangle = triangle;
            edge->m_edge_vertex = phys_add_byte_offset(m_vertices, tet_edge_desc->m_vertex_offset);
            geometry_triangle *twin_tri = phys_add_byte_offset(m_triangle_list, tet_edge_desc->m_triangle_offset);
            edge->m_twin_edge = phys_add_byte_offset(twin_tri->m_hull_edges, tet_edge_desc->m_edge_offset);
            tet_edge_desc++;
        }
    }

    m_triangle_list[3].compute_triangle_data();

    phys_vec4 separation;
    separation.set_sub4(m_vertices[0].m_edge_vertex, m_vertices[1].m_edge_vertex);
    if (separation.dot3(m_triangle_list[3].m_hull_plane) > 0.0f) {
        geometry_triangle &t0 = m_triangle_list[0];
        geometry_triangle &t1 = m_triangle_list[1];
        geometry_triangle &t2 = m_triangle_list[2];
        geometry_triangle &t3 = m_triangle_list[3];
        t0.m_hull_edges[0].m_edge_vertex = t1.m_hull_edges[0].m_edge_vertex = t2.m_hull_edges[0].m_edge_vertex = &m_vertices[1];
        t0.m_hull_edges[2].m_edge_vertex = t1.m_hull_edges[1].m_edge_vertex = t3.m_hull_edges[0].m_edge_vertex = &m_vertices[0];
    }

    geometry_triangle *triangle = m_triangle_list;
    for (int i = 3; i >= 0; i--) {
        geometry_triangle &current_tri = *triangle;
        current_tri.compute_triangle_data();
        triangle++;
    }
}

// @address 0x6b4490 ?find_triangle_nearest_origin@phys_winged_edge@@QAAPAUgeometry_triangle@1@PAU21@@Z
phys_winged_edge::geometry_triangle *phys_winged_edge::find_triangle_nearest_origin(geometry_triangle *start_triangle)
{
    reset_triangle_and_vertex_states();

    geometry_triangle *triangle = start_triangle;
    hull_edge *entry_edge = 0;
    phys_bool finished = false;
    phys_bool found_visited = false;

    while (!finished && !found_visited) {
        triangle->m_state = geometry_triangle::HULL_TRI_VISITED;
        int i;
        for (i = 0; i < 3; i++) {
            if (&triangle->m_hull_edges[i] == entry_edge) {
                continue;
            }
            const phys_vec4 &capsule_vertex_a = triangle->m_hull_edges[i].m_edge_vertex->m_edge_vertex;
            const phys_vec4 &capsule_vertex_b = triangle->m_hull_edges[i].m_next->m_edge_vertex->m_edge_vertex;
            phys_vec4 edge;
            edge.set_sub4(capsule_vertex_b, capsule_vertex_a);
            const phys_real edge_len_sq = edge.length_sq_3d();
            phys_vec4 winged_edge_normal;
            winged_edge_normal.set_cross(edge, triangle->m_hull_plane);
            const phys_real distance = winged_edge_normal.dot3(capsule_vertex_a);
            if (distance * phys_math::fabs(distance) < -edge_visibility_tolerance * edge_visibility_tolerance * edge_len_sq) {
                geometry_triangle *neighbour_vertex = triangle->m_hull_edges[i].m_twin_edge->m_edge_triangle;
                if (start_triangle->m_plane_distance > 0.0f && neighbour_vertex->m_plane_distance < start_triangle->m_plane_distance * 0.9f) {
                    continue;
                }
                entry_edge = triangle->m_hull_edges[i].m_twin_edge;
                triangle = neighbour_vertex;
                found_visited = (neighbour_vertex->m_state == geometry_triangle::HULL_TRI_VISITED);
                break;
            }
        }
        finished = (i == 3);
    }
    return triangle;
}
// @address 0x6b4828 ?find_visible_boundary_edges@phys_winged_edge@@QAA?AW4winged_edge_status@1@PAUgeometry_triangle@1@ABVphys_vec4@@AAHAAPAUhull_edge@1@@Z
phys_winged_edge::winged_edge_status phys_winged_edge::find_visible_boundary_edges(geometry_triangle *triangle, const phys_vec4 &point,
    int &hull_edge_count, hull_edge *&last_edge)
{
    last_edge = 0;
    hull_edge_count = 0;
    m_min_tolerance = minimum_tolerance;
    m_max_tolerance = 3.40282e+38f;
    triangle->m_state = triangle->classify_and_update_tolerance(point, m_min_tolerance, m_max_tolerance);
    if (triangle->m_state == geometry_triangle::HULL_TRI_SHOWN) {
        return find_visible_triangles(triangle, point, hull_edge_count, last_edge);
    }
    return WINGED_EDGE_FAILED;
}

// @address 0x6b45e0 ?find_two_closest_multi_tris@phys_winged_edge@@SAXAAV1@0AAVphys_vec4@@AAPAUgeometry_triangle@1@2@Z
void phys_winged_edge::find_two_closest_multi_tris(
    phys_winged_edge &edges_a, phys_winged_edge &edges_b, phys_vec4 &simplex_separating_normal, geometry_triangle *&best_tri_a, geometry_triangle *&best_tri_b)
{
    phys_real closest_dist = 3.40282e+38f;
    phys_real alpha = 0.0f;
    phys_real beta = 0.0f;

    for (int i = edges_a.m_triangle_count - 1; i >= 0; i--) {
        geometry_triangle *cur_tri_a = &edges_a.m_triangle_list[i];
        if (cur_tri_a->m_plane_distance * cur_tri_a->m_plane_distance > closest_dist) {
            continue;
        }
        for (int j = edges_b.m_triangle_count - 1; j >= 1; j--) {
            geometry_triangle *triangle_b = &edges_b.m_triangle_list[j];
            if (triangle_b->m_plane_distance * triangle_b->m_plane_distance > closest_dist) {
                continue;
            }
            const phys_real weld_cos_angle = cur_tri_a->m_hull_plane.dot3(triangle_b->m_hull_plane);
            if (weld_cos_angle < -0.5f) {
                continue;
            }
            const phys_real dist_sq_a = cur_tri_a->m_plane_distance;
            const phys_real dist_sq_b = triangle_b->m_plane_distance;
            const phys_real proj_a = dist_sq_a - dist_sq_b * weld_cos_angle;
            const phys_real proj_b = dist_sq_b - dist_sq_a * weld_cos_angle;
            phys_real weight_a;
            phys_real weight_b;
            phys_real distance;
            if (proj_a > 1.1920929e-07f) {
                if (proj_b > 1.1920929e-07f) {
                    phys_real perp;
                    phys_real along;
                    if (proj_a > proj_b) {
                        const phys_real ratio = proj_b / proj_a;
                        weight_a = 1.0f / (1.0f + ratio * weld_cos_angle);
                        weight_b = ratio * weight_a;
                        along = dist_sq_a / (1.0f + weld_cos_angle * ratio);
                        perp = ratio * along;
                    } else {
                        const phys_real ratio = proj_a / proj_b;
                        weight_b = 1.0f / (1.0f + ratio * weld_cos_angle);
                        weight_a = ratio * weight_b;
                        perp = dist_sq_b / (1.0f + weld_cos_angle * ratio);
                        along = ratio * perp;
                    }
                    along = along + perp * weld_cos_angle;
                    distance = along * along + (1.0f - weld_cos_angle * weld_cos_angle) * perp * perp;
                } else if (dist_sq_b > 0.0f) {
                    weight_a = 1.0f;
                    weight_b = 0.0f;
                    distance = dist_sq_a * dist_sq_a;
                } else {
                    weight_a = 0.5f;
                    weight_b = 0.5f;
                    distance = -weld_cos_angle * weld_cos_angle;
                }
            } else if (dist_sq_a > 1.1920929e-07f) {
                weight_a = 0.0f;
                weight_b = 1.0f;
                distance = dist_sq_b * dist_sq_b;
            } else {
                weight_a = 0.5f;
                weight_b = 0.5f;
                distance = -weld_cos_angle * weld_cos_angle;
            }
            if (distance < closest_dist) {
                alpha = weight_a;
                beta = weight_b;
                best_tri_a = cur_tri_a;
                best_tri_b = triangle_b;
                closest_dist = distance;
            }
        }
    }

    simplex_separating_normal.set_mul4(alpha, best_tri_a->m_hull_plane);
    simplex_separating_normal.add_scaled4(beta, best_tri_b->m_hull_plane);
    simplex_separating_normal.normalize_xyz();
    simplex_separating_normal(3) = phys_math::sqrt(phys_math::max_of_two(0.0f, closest_dist));
}

__forceinline phys_winged_edge::winged_edge_status phys_winged_edge::begin_polytope_expansion(
    phys_array<geometry_triangle *> &new_triangles, int chain_length, hull_edge *edge, Vertex *&vertex)
{
    hull_edge *first_hull_edge = edge;
    hull_edge *walk_cur_edge = edge;
    const int hull_triangle_count = new_triangles.getSize();
    for (int i = 0; i < hull_triangle_count; i++) {
        if (walk_cur_edge == 0) {
            return WINGED_EDGE_FAILED;
        }
        geometry_triangle *triangle = new_triangles[i];
        geometry_triangle *next_tri;
        if (i + 1 < hull_triangle_count) {
            next_tri = new_triangles[i + 1];
        } else {
            next_tri = new_triangles[0];
        }

        triangle->m_hull_edges[0].m_edge_triangle = triangle;
        triangle->m_hull_edges[0].m_next = &triangle->m_hull_edges[1];
        triangle->m_hull_edges[1].m_edge_triangle = triangle;
        triangle->m_hull_edges[1].m_next = &triangle->m_hull_edges[2];
        triangle->m_hull_edges[2].m_edge_triangle = triangle;
        triangle->m_hull_edges[2].m_next = &triangle->m_hull_edges[0];

        Vertex *v0 = walk_cur_edge->m_next->m_edge_vertex;
        Vertex *v1 = walk_cur_edge->m_edge_vertex;
        Vertex *v2 = vertex;
        triangle->m_hull_edges[0].m_edge_vertex = v0;
        triangle->m_hull_edges[1].m_edge_vertex = v1;
        triangle->m_hull_edges[2].m_edge_vertex = v2;

        walk_cur_edge->m_twin_edge = &triangle->m_hull_edges[0];
        triangle->m_hull_edges[0].m_twin_edge = walk_cur_edge;
        next_tri->m_hull_edges[1].m_twin_edge = &triangle->m_hull_edges[2];
        triangle->m_hull_edges[2].m_twin_edge = &next_tri->m_hull_edges[1];

        if (triangle->compute_triangle_data_checked(v0->m_edge_vertex, v1->m_edge_vertex, v2->m_edge_vertex) == PHYS_FAILURE) {
            return WINGED_EDGE_FAILED;
        }

        hull_edge *walk_next_edge = walk_cur_edge->m_next->m_edge_vertex->m_hull_edge;
        walk_cur_edge->m_next->m_edge_vertex->m_hull_edge = 0;
        walk_cur_edge = walk_next_edge;
    }

    reset_triangle_and_vertex_states();

    if (first_hull_edge != walk_cur_edge) {
        return WINGED_EDGE_FAILED;
    }
    return WINGED_EDGE_OK;
}

// @address 0x6b48b8 ?patch_boundary_to_view@phys_winged_edge@@QAA?AW4winged_edge_status@1@HPAUhull_edge@1@PAUVertex@1@@Z
phys_winged_edge::winged_edge_status phys_winged_edge::patch_boundary_to_view(int hull_edge_count, hull_edge *edge, Vertex *vertex)
{
    phys_inline_array<geometry_triangle *, PHYS_WINGED_EDGE_TRIANGLE_LIMIT> new_triangles;
    allocate_triangles(new_triangles, hull_edge_count);
    winged_edge_status status = begin_polytope_expansion(new_triangles, hull_edge_count, edge, vertex);
    return status;
}
