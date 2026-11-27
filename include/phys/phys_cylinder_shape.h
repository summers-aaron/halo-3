#ifndef INCLUDE_PHYS_CYLINDER_SHAPE_H
#define INCLUDE_PHYS_CYLINDER_SHAPE_H

#include "phys/phys_collide.h"

class phys_cylinder_shape : public phys_convex_shape {
public:
    phys_cylinder_shape(phys_fixup_tag flag);
    phys_cylinder_shape(const phys_vec4 &capsule_vertex_a, const phys_vec4 &capsule_vertex_b, float cylinder_radius, float padding_radius);
    static void set_virtual_side_segments(int segment_count);
    void preset_perpendicular_vec();
    virtual __forceinline ~phys_cylinder_shape() {}
    float get_cylinder_radius() const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const class phys_sphere *get_collision_spheres_impl(phys_sphere *sphere_buffer) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    void set_cylinder_radius(float radius);
    virtual void get_first_vertex(phys_vec4 &vertex) const;
    virtual void get_shape_centre_impl(phys_vec4 &shape_centre) const;
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const;
    virtual void vertex_ids_to_vertices_impl(const unsigned short *ids, int id_count, phys_hull_vertex *vertices_out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const;
    virtual void get_aabb_impl(const class phys_transform &local_to_world, float tolerance, class phys_aabb &out) const;
    __forceinline void decode_vertex_id(unsigned short code, phys_vec4 &result) const;
    static __forceinline void *operator new(size_t, void *memory) { return memory; }
    __forceinline const phys_vec4 &get_vertex(int index) const { return m_vertices[index]; }

    float m_cylinder_radius;
    float m_cyl_heightfield_radius_factor;
    phys_vec4 m_vertices[2];
    phys_vec4 m_perp_axis1;
    phys_vec4 m_perp_axis2;
private:
    static float s_virtualTesselationParameter;
    static float s_virtualTesselationParameterInv;
    static float s_intRoundUpThreshold;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_cylinder_size[(sizeof(void *) != 4 || sizeof(phys_cylinder_shape) == 0x60) ? 1 : -1];
typedef char phys_cylinder_verts_offset[(sizeof(void *) != 4 || offsetof(phys_cylinder_shape, m_vertices) == 0x20) ? 1 : -1];
#endif

#endif
