#ifndef INCLUDE_PHYS_HEIGHTFIELD_RAY_FORWARDER_H
#define INCLUDE_PHYS_HEIGHTFIELD_RAY_FORWARDER_H
#include "phys/phys_collide.h"
#include "phys/phys_collectors.h"

class phys_heightfield_ray_forwarder : public phys_ray_hit_collector {
public:
    __forceinline phys_heightfield_ray_forwarder(const phys_collision_proxy &heightfield_proxy, const phys_vec4 &path, phys_hit_collector &collector)
        : dir(path), agent_body_a(&heightfield_proxy), ray_collector(&collector) {}
    virtual void add_ray_hit(const phys_collision_proxy &body, const phys_ray_collector_output &hit);
    phys_vec4 from;
    float neg_scale;
    phys_vec4 dir;
    const phys_collision_proxy *agent_body_a;
    phys_hit_collector *ray_collector;
};
#endif
