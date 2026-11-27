#ifndef PHYSICS_COLLISION_INSTANCED_GEOMETRY_H
#define PHYSICS_COLLISION_INSTANCED_GEOMETRY_H

#include "physics/collision_types.h"
#include "physics/collision_bsp_test_context.h"
#include "phys/phys_collide.h"

// Collects the instanced geometry a ray cast through a cluster's MOPP shape hits and tests each instance.
class c_instanced_geometry_ray_hit_collector : public phys_ray_hit_collector {
public:
    __forceinline c_instanced_geometry_ray_hit_collector(const s_collision_bsp_test_context *context,
        unsigned long flags, unsigned long bsp_flags, const real_point3d *point, const real_vector3d *vector,
        collision_result *result)
    {
        m_flags = flags;
        m_bsp_flags = bsp_flags;
        m_point = point;
        m_vector = vector;
        m_context = context;
        m_hit = false;
        m_result = result;
        m_early_out_hit_frac = result->fraction;
    }
    virtual ~c_instanced_geometry_ray_hit_collector() {}

protected:
    virtual void add_ray_hit(const phys_collision_proxy &body, const phys_ray_collector_output &hit_info);

public:
    unsigned long m_flags;
    unsigned long m_bsp_flags;
    const real_point3d *m_point;
    const real_vector3d *m_vector;
    const s_collision_bsp_test_context *m_context;
    bool m_hit;
    collision_result *m_result;
};
#if !defined(_WIN64) && !defined(__LP64__)
typedef char c_instanced_geometry_ray_hit_collector_result_offset[
    (offsetof(c_instanced_geometry_ray_hit_collector, m_result) == 0x20) ? 1 : -1];
#endif

#endif
