#ifndef INCLUDE_PHYS_CONVEX_HULL_SHAPE_H
#define INCLUDE_PHYS_CONVEX_HULL_SHAPE_H

#include "phys/phys_collide.h"
#include "phys/phys_strided_vertices.h"

class phys_convex_hull_shape : public phys_convex_shape {
public:
    virtual ~phys_convex_hull_shape();
    struct four_vecs {
        phys_vec4 m_x;
        phys_vec4 m_y;
        phys_vec4 m_soa_z;
    };

    phys_convex_hull_shape(phys_strided_vertices vertex_in, const phys_array<phys_vec4> &hull_plane_equations, float radius);
    phys_convex_hull_shape(four_vecs *hull_rotated_vertices, int hull_vertex_count, phys_vec4 *planes, int hull_plane_count,
        const phys_aabb &aabb, float radius);
    __forceinline phys_convex_hull_shape(phys_fixup_tag flag)
        : phys_convex_shape(flag), m_rotated_vertices(flag), m_plane_equations(flag) { m_type = PHYS_SHAPE_CONVEX_HULL; }
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int compute_dma_size(int max_available_size) const { return sizeof(*this); }
    virtual int get_collision_sphere_count_impl() const;
    const phys_array<phys_vec4> &get_plane_equations() const;
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *spheres) const;
    virtual void get_first_vertex(phys_vec4 &v) const;
    virtual void get_shape_centre_impl(phys_vec4 &shape_centre) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void vertex_ids_to_vertices_impl(const unsigned short *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    void copy_vertex_data(const float *vertex_in, int byte_stride, int hull_vertex_count);
    void get_original_vertices(phys_array<phys_vec4> &vertices) const;

    phys_vec4 m_aabb_half_extents;
    phys_vec4 m_hull_aabb_center;
    phys_array<four_vecs> m_rotated_vertices;
    int m_hull_vertex_count;
    phys_array<phys_vec4> m_plane_equations;
};

typedef char phys_convex_hull_size_four_vecs[(sizeof(phys_convex_hull_shape::four_vecs) == 48) ? 1 : -1];
typedef char phys_convex_hull_size[(sizeof(void *) != 4 || ((sizeof(phys_convex_hull_shape) == 96) )) ? 1 : -1];
typedef char phys_convex_hull_offset_aabb_half_extents[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_aabb_half_extents) == 32))) ? 1 : -1];
typedef char phys_convex_hull_offset_aabb_centre[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_hull_aabb_center) == 48))) ? 1 : -1];
typedef char phys_convex_hull_offset_rotated_verts[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_rotated_vertices) == 64))) ? 1 : -1];
typedef char phys_convex_hull_offset_num_verts[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_hull_vertex_count) == 76) )) ? 1 : -1];
typedef char phys_convex_hull_offset_plane_equations[(sizeof(void *) != 4 || ((offsetof(phys_convex_hull_shape, m_plane_equations) == 80) )) ? 1 : -1];

#endif
