#ifndef PHYSICS_HAVOK_ENVIRONMENT_SURFACE_SHAPE_H
#define PHYSICS_HAVOK_ENVIRONMENT_SURFACE_SHAPE_H

#include "phys/phys_collide.h"
#include "physics/havok_runtime_pool.h"

// User data of every c_environment_surface_shape (set by its constructor).
const unsigned long k_environment_surface_shape_user_data = 123456789;
// User data of the shape at the root of an environment rigid body (spelling as in the original source).
const unsigned long k_environement_body_user_data = 0xcabcabb0;

class c_environment_surface_shape : public phys_compound_shape {
public:
    c_environment_surface_shape(long structure_bsp_index, long runtime_pool_index);
    virtual ~c_environment_surface_shape() {}
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    virtual int get_child_shape_count() const;
    virtual unsigned int get_first_key() const;
    virtual unsigned int get_next_key(unsigned int old_key) const;
    virtual const phys_shape *get_child_shape(unsigned int key, char (&buffer)[512]) const;
    __forceinline bool is_key_alive(unsigned int key) const
    {
        return havok_runtime_pool_is_surface_enabled(m_structure_bsp_index, m_runtime_pool_index, 0, key);
    }
    phys_vec4 m_aabb_center;
    phys_vec4 m_aabb_half_extents;
    long m_structure_bsp_index;
    long m_runtime_pool_index;
};

#endif
