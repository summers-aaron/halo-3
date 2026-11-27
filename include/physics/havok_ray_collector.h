#ifndef PHYSICS_HAVOK_RAY_COLLECTOR_H
#define PHYSICS_HAVOK_RAY_COLLECTOR_H

#include "phys/phys_collide.h"
#include "physics/havok_ray_test_types.h"
#include <float.h>

const float k_real_max = FLT_MAX;

class c_collector : public phys_ray_hit_collector {
public:
    s_havok_ray_test *m_ray_test;
    long m_context;
    long m_hit_index;
    bool m_hit_found;
    __forceinline c_collector(s_havok_ray_test *ray_test, long context, long hit_index)
    {
        m_ray_test = ray_test;
        m_context = context;
        m_hit_index = hit_index;
        m_ray_test->fraction = k_real_max;
        m_hit_found = false;
    }
    virtual ~c_collector() {}
    virtual void add_ray_hit(const phys_collision_proxy &, const phys_ray_collector_output &);
};

#endif
