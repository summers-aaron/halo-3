#ifndef INCLUDE_PHYSICS_PHYSICS_MODEL_SHAPE_INTERNAL_H
#define INCLUDE_PHYSICS_PHYSICS_MODEL_SHAPE_INTERNAL_H
#include <stddef.h>
#include "phys/phys_collide.h"
#include "physics/havok_shape_fixup.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collision_proxy.h"
#include "physics/collision_bsp_physics_definitions.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_convex_hull_shape.h"

class c_sphere_shape : public phys_sphere_shape
{
public:
    c_sphere_shape(phys_fixup_tag flag) : phys_sphere_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, 0);
    }
    __forceinline c_sphere_shape(phys_fixup_tag flag, phys_ulong user_data) : phys_sphere_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    // match: 0x1c00b8: the tag-owned shape destructor is unreachable.
    virtual ~c_sphere_shape() { __assume(0); }
};

class c_capsule_shape : public phys_capsule_shape
{
public:
    c_capsule_shape(phys_fixup_tag flag) : phys_capsule_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, 0);
    }
    __forceinline c_capsule_shape(phys_fixup_tag flag, phys_ulong user_data) : phys_capsule_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    // match: 0x1c00a8: the tag-owned shape destructor is unreachable.
    virtual ~c_capsule_shape() { __assume(0); }
};

class c_multi_sphere_shape : public phys_multi_sphere_shape
{
public:
    c_multi_sphere_shape(phys_fixup_tag flag) : phys_multi_sphere_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, 0);
    }
    __forceinline c_multi_sphere_shape(phys_fixup_tag flag, phys_ulong) : phys_multi_sphere_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, 0);
    }
    // match: 0x1c0098: the tag-owned shape destructor is unreachable.
    virtual ~c_multi_sphere_shape() { __assume(0); }
};

class c_convex_vertices_shape : public phys_convex_hull_shape
{
public:
    __forceinline c_convex_vertices_shape(phys_fixup_tag flag, phys_ulong user_data,
        phys_convex_hull_shape::four_vecs *rotated_vertices, phys_vec4 *plane_equations) : phys_convex_hull_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
        m_rotated_vertices.m_data = rotated_vertices;
        m_plane_equations.m_data = plane_equations;
    }
    // match: 0x1c01a0: the tag-owned shape destructor is unreachable.
    virtual ~c_convex_vertices_shape() { __assume(0); }
};

class c_list_shape : public phys_list_shape
{
public:
    c_list_shape(phys_fixup_tag flag) : phys_list_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, 0);
    }
    __forceinline c_list_shape(phys_fixup_tag flag, phys_ulong user_data, phys_list_shape::child_shape_entry *child_info)
        : phys_list_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
        m_child_info.m_data = child_info;
    }
    // match: 0x1c01b0: the tag-owned shape destructor is unreachable.
    virtual ~c_list_shape() { __assume(0); }
};

class c_box_shape : public phys_box_shape
{
public:
    __forceinline c_box_shape(phys_fixup_tag flag, phys_ulong user_data) : phys_box_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    // match: 0x1c0180: the tag-owned shape destructor is unreachable.
    virtual ~c_box_shape() { __assume(0); }
};

class c_triangle_shape : public phys_triangle_shape
{
public:
    __forceinline c_triangle_shape(phys_fixup_tag flag, phys_ulong user_data) : phys_triangle_shape(flag)
    {
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    // match: 0x1c0190: the tag-owned shape destructor is unreachable.
    virtual ~c_triangle_shape() { __assume(0); }
};

struct c_mopp_bv_tree_shape : phys_bvh_tree_shape {
    float m_scale;
    __forceinline c_mopp_bv_tree_shape(phys_fixup_tag flag, phys_ulong user_data,
        phys_compound_shape *collection, phys_bvh_bytecode *code) : phys_bvh_tree_shape(flag)
    {
        m_bvh_code = code;
        new (&m_child_shape) phys_single_shape_container(flag, collection);
        initialize_tag_shape_reference_count_and_user_data(this, user_data);
    }
    __forceinline c_mopp_bv_tree_shape(const phys_compound_shape *collection, const phys_bvh_bytecode *code, float scale) :
        phys_bvh_tree_shape(collection, code)
    {
        m_scale = scale;
    }
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual void get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const;
    virtual void shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &hits) const;
    virtual void query_obb(const phys_transform &transform, const phys_vec4 &extent, float tolerance, phys_array<unsigned int> &hits) const;
    virtual void query_aabb(const phys_aabb &aabb, phys_array<unsigned int> &hits) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const;
};
class c_transform_shape : public phys_convex_offset_shape {
public:
    c_transform_shape(phys_fixup_tag flag, long data, phys_shape *shape);
    // match: 0x1c0170: the tag-owned shape destructor is unreachable.
    virtual ~c_transform_shape() { __assume(0); }
};

#endif
