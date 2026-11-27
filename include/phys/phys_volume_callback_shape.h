#ifndef INCLUDE_PHYS_VOLUME_CALLBACK_SHAPE_H
#define INCLUDE_PHYS_VOLUME_CALLBACK_SHAPE_H

#include "phys/phys_collide.h"

class phys_volume_callback_shape : public phys_shape {
public:
    phys_volume_callback_shape() : phys_shape(PHYS_SHAPE_VOLUME_CALLBACK) {}
    phys_volume_callback_shape(phys_fixup_tag flag) : phys_shape(flag) { m_type = PHYS_SHAPE_VOLUME_CALLBACK; }
    virtual void volume_enter_event(const phys_collider *phantom, const phys_collider *other, const phys_collision_input &input) = 0;
    virtual void volume_leave_event(const phys_collider *phantom, const phys_collider *other) = 0;
    virtual phys_bool trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const;
    virtual void trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const;
    virtual void get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &aabb) const;
    virtual void gather_memory_stats(phys_statistics_collector *collector) const;
};

#endif
