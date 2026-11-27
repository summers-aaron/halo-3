#ifndef INCLUDE_PHYS_WINGED_EDGE_H
#define INCLUDE_PHYS_WINGED_EDGE_H

#include "phys/phys_collide.h"

#define PHYS_WINGED_EDGE_TRIANGLE_LIMIT 100

class phys_winged_edge {
public:
    enum winged_edge_status {
        WINGED_EDGE_OK = 0,
        WINGED_EDGE_FAILED = 2
    };
    struct hull_edge;
    struct geometry_triangle;
    struct Vertex {
        phys_vec4 m_edge_vertex;
        phys_hull_vertex m_vertex_a; phys_hull_vertex m_vertex_b;
        phys_padded<hull_edge *> m_hull_edge;
    };
    struct hull_edge {
        Vertex *m_edge_vertex;
        hull_edge *m_next;
        hull_edge *m_twin_edge;
        geometry_triangle *m_edge_triangle;
    };
    struct geometry_triangle {
        enum triangle_state {
            HULL_TRI_UNKNOWN = 0,
            HULL_TRI_SHOWN = 1,
            HULL_TRI_HIDDEN = 2,
            HULL_TRI_VISITED = 3,
            HULL_TRI_FAILED = 4
        };

        phys_vec4 m_hull_plane;
        hull_edge m_hull_edges[3];               // stride 0x10
        phys_real m_plane_distance;
        int m_state;

        void compute_triangle_data();

        __forceinline phys_result compute_triangle_data_checked(const phys_vec4 &v0, const phys_vec4 &v1, const phys_vec4 &v2)
        {
            phys_vec4 e0;
            e0.set_sub4(v0, v1);
            phys_vec4 e1;
            e1.set_sub4(v1, v2);
            phys_vec4 normal;
            normal.set_cross(e0, e1);
            if (normal.length_sq_3d() == 0.0f) {
                return PHYS_FAILURE;
            }
            normal.normalize_xyz();
            m_plane_distance = normal.dot3(v0);
            m_hull_plane = normal;
            return PHYS_SUCCESS;
        }

        __forceinline const phys_real distance_to_triangle(const phys_vec4 &point) const
        {
            phys_vec4 separation;
            separation.set_sub4(point, m_hull_edges[0].m_edge_vertex->m_edge_vertex);
            return m_hull_plane.dot3(separation);
        }

        const triangle_state classify_and_update_tolerance(const phys_vec4 &point, phys_padded<float> &min_tolerance, phys_padded<float> &max_tolerance) const;
    };
protected:
    void reset_triangle_and_vertex_states();
    public:
    void init(const phys_hull_vertex *vertices_a, const phys_hull_vertex *vertices_b, const phys_vec4 *positions);
    __forceinline phys_bool has_free_vertex() { return m_hull_vertex_count < 55; }
    __forceinline Vertex *allocate_vertex() { Vertex *vertex = &m_vertices[m_hull_vertex_count];
        m_hull_vertex_count = m_hull_vertex_count + 1; vertex->m_hull_edge = 0; return vertex; }
    geometry_triangle *find_tri_on_nearest_plane();
    __forceinline int free_triangle_count() { return m_free_triangle_count - m_triangle_count + PHYS_WINGED_EDGE_TRIANGLE_LIMIT; }
protected:
    void allocate_triangles(phys_array<geometry_triangle *> &new_triangles, int hull_triangle_count);
    void add_tetrahedron();
    winged_edge_status find_visible_triangles(geometry_triangle *triangle, const phys_vec4 &point, int &hull_edge_count, hull_edge *&last_edge);
    __forceinline winged_edge_status begin_polytope_expansion(phys_array<geometry_triangle *> &new_triangles, int chain_length,
        hull_edge *edge, Vertex *&vertex);
public:
    winged_edge_status patch_boundary_to_view(int hull_edge_count, hull_edge *edge, Vertex *vertex);
public:
    winged_edge_status find_visible_boundary_edges(geometry_triangle *triangle, const phys_vec4 &point, int &hull_edge_count, hull_edge *&last_edge);
    geometry_triangle *find_triangle_nearest_origin(geometry_triangle *start_triangle);
protected:

    phys_padded<float> m_min_tolerance;
    phys_padded<float> m_max_tolerance;
    phys_padded<int> m_hull_vertex_count;
    phys_padded<int> m_free_triangle_count;
    phys_padded<int> m_triangle_count;
    Vertex m_vertices[55];             // stride 0x40
    geometry_triangle *m_free_triangles[PHYS_WINGED_EDGE_TRIANGLE_LIMIT];
    geometry_triangle m_triangle_list[PHYS_WINGED_EDGE_TRIANGLE_LIMIT];           // stride 0x50 (m_state at 0xfb4)
public:
  static void simplex_code_begin();
  static void simplex_code_end();
  static void find_two_closest_multi_tris(phys_winged_edge &a, phys_winged_edge &b, phys_vec4 &out,
      phys_winged_edge::geometry_triangle *&p3, phys_winged_edge::geometry_triangle *&p4);
};

typedef char havok_size_hkWingedEdge_Vertex[(sizeof(phys_winged_edge::Vertex) == 0x40) ? 1 : -1];
typedef char havok_size_hkWingedEdge_Triangle[(sizeof(void *) != 4 || ((sizeof(phys_winged_edge::geometry_triangle) == 0x50) )) ? 1 : -1];
typedef char havok_size_hkWingedEdge[(sizeof(void *) != 4 || ((sizeof(phys_winged_edge) == 0x2eb0) )) ? 1 : -1];

#endif
