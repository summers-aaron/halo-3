#ifndef INCLUDE_PHYS_SIMPLEX_MANIFOLD_H
#define INCLUDE_PHYS_SIMPLEX_MANIFOLD_H

#include "phys/phys_collide.h"
#include <stddef.h>

struct phys_simplex_manifold {
    struct manifold_contact {
        unsigned char m_dim_a;
        unsigned char m_dim_b;
        unsigned short m_handle_id;
        union {
            unsigned char m_vert_ids[4];
            unsigned int m_all_verts;
        };
    };
    typedef unsigned short manifold_vertex_id;
    enum { MANIFOLD_POINT_MAX = 4, MANIFOLD_VERTEX_MAX = MANIFOLD_POINT_MAX * 4 };

    union {
        unsigned int m_header;
        struct {
            unsigned char m_vert_count_a;
            unsigned char m_vert_count_b;
            unsigned char m_contact_point_count;
        };
    };
    manifold_contact m_process_contact_points[MANIFOLD_POINT_MAX];
    manifold_vertex_id m_vertex_storage[MANIFOLD_VERTEX_MAX];

    __forceinline phys_simplex_manifold() { init(); }
    __forceinline void init() { m_header = 0; }
    __forceinline int get_total_size_in_bytes() {
        return 4 + m_contact_point_count * sizeof(manifold_contact)
            + (m_vert_count_a + m_vert_count_b) * sizeof(manifold_vertex_id);
    }
    __forceinline manifold_vertex_id *get_vertex_ids() {
        return reinterpret_cast<manifold_vertex_id *>(&m_process_contact_points[m_contact_point_count]);
    }
    __forceinline const manifold_vertex_id *get_vertex_ids() const {
        return reinterpret_cast<const manifold_vertex_id *>(&m_process_contact_points[m_contact_point_count]);
    }
    __forceinline manifold_vertex_id get_vertex_id(const manifold_contact &cp, int vertex_nr) const {
        return *reinterpret_cast<const manifold_vertex_id *>(reinterpret_cast<const char *>(get_vertex_ids())
            + static_cast<unsigned char>(cp.m_vert_ids[vertex_nr] >> 3));
    }
};

typedef char phys_simplex_manifold_point_size[(sizeof(phys_simplex_manifold::manifold_contact) == 8) ? 1 : -1];
typedef char phys_simplex_manifold_points_offset[(offsetof(phys_simplex_manifold, m_process_contact_points) == 4) ? 1 : -1];
typedef char phys_simplex_manifold_size[(sizeof(phys_simplex_manifold) == 68) ? 1 : -1];

struct phys_simplex_manifold_work {
    // March 0x83567cc8/0x83567cf8: packed manifold vertex offsets are byte offsets.
    __forceinline const phys_vec4 &get_vertex_a(unsigned int vertex_id_x16) const {
        return *phys_add_byte_offset_const(&m_vertices[0], vertex_id_x16 * (sizeof(phys_hull_vertex) >> 4));
    }
    __forceinline const phys_vec4 &get_vertex_b(unsigned int vertex_id_x16) const {
        return *phys_add_byte_offset_const(&m_vertices[0], vertex_id_x16 * (sizeof(phys_hull_vertex) >> 4));
    }

    phys_hull_vertex m_vertices[16];
    phys_vec4 m_master_normal;
    float m_radius_a;
    float m_radius_b;
    float m_keep_contact;
    float m_radius_sum_sq;
};

typedef char phys_simplex_manifold_work_size[(sizeof(phys_simplex_manifold_work) == 288) ? 1 : -1];

#endif
