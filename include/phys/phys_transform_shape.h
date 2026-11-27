#ifndef INCLUDE_PHYS_TRANSFORM_SHAPE_H
#define INCLUDE_PHYS_TRANSFORM_SHAPE_H
#include "phys/phys_collide.h"
class phys_transform_shape : public phys_shape {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_COLLISION_INFO);
    __forceinline phys_transform_shape(phys_fixup_tag flag) : phys_shape(flag), m_child_container(flag) {
        m_type = PHYS_SHAPE_TRANSFORM;
    }
    phys_transform_shape(const phys_shape *bounding_child_shape, const phys_transform &transform);
    virtual __forceinline ~phys_transform_shape() {}
    virtual float get_max_projection(const phys_vec4 &direction) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual const phys_shape_container *get_shape_container() const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    __forceinline const phys_shape *get_child_shape() const { return m_child_container.get_single_child(); }
    __forceinline const phys_transform &get_transform() const { return m_body_transform; }
    __forceinline void set_transform(const phys_transform &transform);
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    phys_single_shape_container m_child_container;
    phys_quat m_rotation;
    phys_transform m_body_transform;
};
#endif
