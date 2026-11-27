#ifndef INCLUDE_PHYS_BOUNDING_SHAPE_H
#define INCLUDE_PHYS_BOUNDING_SHAPE_H

#include "phys/phys_collide.h"

// Pairs a bounding-volume shape with its child shape.
class phys_bounding_shape : public phys_shape {
public:
    phys_bounding_shape(const phys_shape *bounding_volume_shape, const phys_shape *bounding_child_shape);
    __forceinline phys_bounding_shape(phys_fixup_tag flag) : phys_shape(flag), m_child_container(flag)
    {
        m_type = PHYS_SHAPE_BOUNDING;
    }
    virtual ~phys_bounding_shape();
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
    virtual const phys_shape_container *get_shape_container() const;
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const;
    __forceinline const phys_shape *get_child_shape() const { return m_child_container.get_single_child(); }
    __forceinline const phys_shape *get_bounding_volume_shape() const { return m_bounding_shape; }
    const phys_shape *m_bounding_shape;
    phys_single_shape_container m_child_container;
};

typedef char phys_size_bounding_shape[(sizeof(void *) != 4 || ((sizeof(phys_bounding_shape) == 0x1c) )) ? 1 : -1];

#endif
