#ifndef PHYSICS_HAVOK_COLLISION_SURFACE_SHAPE_H
#define PHYSICS_HAVOK_COLLISION_SURFACE_SHAPE_H

#include "phys/phys_collide.h"
#include "cseries/cseries_wrapped.h"
#include <stddef.h>

struct collision_bsp;
struct real_matrix4x3;
class c_global_material_type;
enum e_soft_surface_sink_class { _soft_surface_sink_class_default = 0, k_soft_surface_sink_class_count = 6 };

// Leaf convex shape wrapping a single collision surface.
class c_collision_surface_shape : public phys_convex_shape {
public:
    c_collision_surface_shape(unsigned long user_data, long structure_bsp_index, const collision_bsp *bsp, float scale,
        bool collidable, bool unpooled, long instance_index, long surface_index, const real_matrix4x3 *matrix,
        e_soft_surface_sink_class sink_class, float sink_scale);
    virtual __forceinline ~c_collision_surface_shape() {}
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual int get_collision_sphere_count_impl() const;
    virtual const phys_sphere *get_collision_spheres_impl(phys_sphere *spheres) const;
    virtual void get_first_vertex(phys_vec4 &vertex) const;
    virtual void get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &vertex) const;
    virtual void vertex_ids_to_vertices_impl(const unsigned short *ids, int count, phys_hull_vertex *vertices) const;
    c_global_material_type get_global_material_type() const;
    __forceinline unsigned char get_collision_surface_flags() const { return m_surface_flags; }
    __forceinline long get_surface_index() const { return m_surface_index; }

    long m_surface_index;
    short m_structure_bsp_index;
    char m_vertex_count;
    char m_best_plane_calculation_vertex_index;
    short m_material_index;
    unsigned char m_surface_flags;
    c_flags_no_init<long, unsigned char, 2> m_flags;
    long m_instance_index;
    float m_scale;
    phys_vec4 m_vertices[8];
};

typedef char collision_surface_shape_size[(sizeof(void *) != 4 || ((sizeof(c_collision_surface_shape) == 0xb0) )) ? 1 : -1];
typedef char collision_surface_shape_flags_offset[(sizeof(void *) != 4 || ((offsetof(c_collision_surface_shape, m_flags) == 0x1f) )) ? 1 : -1];
typedef char collision_surface_shape_vertices_offset[(sizeof(void *) != 4 || ((offsetof(c_collision_surface_shape, m_vertices) == 0x30) )) ? 1 : -1];

#endif // PHYSICS_HAVOK_COLLISION_SURFACE_SHAPE_H
