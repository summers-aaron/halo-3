#ifndef PHYSICS_HAVOK_ENVIRONMENT_MOPP_BV_TREE_SHAPE_H
#define PHYSICS_HAVOK_ENVIRONMENT_MOPP_BV_TREE_SHAPE_H

#include "physics/havok_environment_surface_shape.h"

class c_environment_mopp_bv_tree_shape : public phys_bvh_tree_shape {
public:
    // March 0x82bbc7e0: the shape holds a reference on its environment surface.
    __forceinline c_environment_mopp_bv_tree_shape(const c_environment_surface_shape *environment_surface_shape, const phys_bvh_bytecode *code)
        : phys_bvh_tree_shape(environment_surface_shape, code)
    {
        m_environment_surface_shape = environment_surface_shape;
        m_environment_surface_shape->add_object_reference();
    }
    virtual ~c_environment_mopp_bv_tree_shape();
    const c_environment_surface_shape *m_environment_surface_shape;
    void query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &keys) const;
    virtual void shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned int> &keys) const;
    virtual void query_obb(const phys_transform &transform, const phys_vec4 &half_extents, float tolerance, phys_array<unsigned int> &keys) const;
    virtual void query_aabb(const phys_aabb &aabb, phys_array<unsigned int> &keys) const;

    // March 0x82bbd420: the derived environment surface pointer is at offset 28.
    __forceinline const c_environment_surface_shape *get_environment_surface_shape() const
    {
        return m_environment_surface_shape;
    }
};

#endif // PHYSICS_HAVOK_ENVIRONMENT_MOPP_BV_TREE_SHAPE_H
