#ifndef INCLUDE_PHYS_TRIANGLE_SHAPE_H
#define INCLUDE_PHYS_TRIANGLE_SHAPE_H

#include "phys/phys_collide.h"
#include "phys/phys_edge_weld_util.h"

struct s_triangle_vertex_id_table {
    int vertex[6];
    float sign[6];
    float weld_type_normal_flip[5];
};

extern "C" {
extern s_triangle_vertex_id_table k_phys_triangle_edge_weld_table;
}

// The extrusion flag is stored just before the three vertices.
// March 0x8354ad10 and 0x8348b450 supply the two accessors.
class phys_triangle_shape : public phys_convex_shape {
public:
    // March 2007: the welding info and type are set through their setters, then the extrusion is cleared.
    __forceinline phys_triangle_shape(float radius = phys_convex_shape_default_radius, unsigned short edge_weld_info = 0,
        phys_edge_weld_util::edge_weld_kind type = phys_edge_weld_util::EDGE_WELD_NONE)
        : phys_convex_shape(PHYS_SHAPE_TRIANGLE, radius)
    {
        set_edge_weld_info(edge_weld_info);
        set_edge_weld_type(type);
        m_extrusion_vec.set_zero4();
        m_extruded = 0;
    }
    __forceinline phys_triangle_shape(phys_fixup_tag flag) : phys_convex_shape(flag)
    {
        m_type = PHYS_SHAPE_TRIANGLE;
    }
    // Out-of-line overrides owned by hkTriangleShape.obj.
    virtual __forceinline ~phys_triangle_shape() {}
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual void get_shape_centre_impl(phys_vec4 &shape_centre) const;
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &vertex_out) const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual int weld_contact_point_impl(unsigned short *feature_points, unsigned char &feature_point_count,
        phys_vec4 &contact_point_world, const phys_transform *this_obj_transform, const phys_convex_shape *colliding_shape,
        const phys_transform *colliding_transform, phys_vec4 &separating_normal_in_out) const;
    virtual void get_first_vertex(phys_vec4 &vertex) const;
    virtual void vertex_ids_to_vertices_impl(const unsigned short *ids, int count, phys_hull_vertex *vertices) const;
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &aabb) const;
    unsigned short m_edge_weld_info;
    unsigned char m_edge_weld_type;
    unsigned char m_extruded;
    phys_vec4 m_vertex_a;
    phys_vec4 m_vertex_b;
    phys_vec4 m_vertex_c;
    phys_vec4 m_extrusion_vec;

    __forceinline const phys_vec4 *get_vertices() const { return &m_vertex_a; }
    __forceinline const phys_vec4 &get_vertex(int index) const { return (&m_vertex_a)[index]; }
    __forceinline bool is_extruded() const { return m_extruded != 0; }
    __forceinline unsigned short get_edge_weld_info() const { return m_edge_weld_info; }
    __forceinline void set_edge_weld_info(unsigned short info) { m_edge_weld_info = info; }
    __forceinline phys_edge_weld_util::edge_weld_kind get_edge_weld_type() const { return static_cast<phys_edge_weld_util::edge_weld_kind>(m_edge_weld_type); }
    __forceinline void set_edge_weld_type(phys_edge_weld_util::edge_weld_kind type) { m_edge_weld_type = static_cast<unsigned char>(type); }
    __forceinline void set_triangle_vertex(int index, const phys_vec4 &vertex) { (&m_vertex_a)[index] = vertex; }
    __forceinline const phys_vec4 &get_extrusion() const { return m_extrusion_vec; }
    __forceinline void set_extrusion_vec(const phys_vec4 &extrusion_vec)
    {
        m_extruded = extrusion_vec.length_sq_3d() > 0.0f;
        m_extrusion_vec = extrusion_vec;
    }
};

typedef char phys_triangle_shape_size[(sizeof(void *) != 4 || ((sizeof(phys_triangle_shape) == 0x60) )) ? 1 : -1];
typedef char phys_triangle_shape_extruded[(sizeof(void *) != 4 || ((offsetof(phys_triangle_shape, m_extruded) == 0x17) )) ? 1 : -1];
typedef char phys_triangle_shape_vertices[(sizeof(void *) != 4 || ((offsetof(phys_triangle_shape, m_vertex_a) == 0x20) )) ? 1 : -1];

#endif
