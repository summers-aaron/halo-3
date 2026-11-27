#ifndef PHYSICS_HAVOK_WORLD_SHAPE_PENETRATION_COLLECTOR_H
#define PHYSICS_HAVOK_WORLD_SHAPE_PENETRATION_COLLECTOR_H

#include "phys/phys_collectors.h"

// The world-shape query's local c_penetration_collector in the March build.
class c_world_shape_penetration_collector : public phys_proxy_pair_collector {
public:
    __forceinline c_world_shape_penetration_collector(long component_index)
        : found_collision(false), component_index(component_index) {}
    virtual ~c_world_shape_penetration_collector() {}
    virtual void add_proxy_pair(const phys_collision_proxy &body_a, const phys_collision_proxy &body_b);

    bool found_collision;
    int component_index;
};
typedef char world_shape_penetration_collector_size[(sizeof(void *) != 4 || ((sizeof(c_world_shape_penetration_collector) == 16))) ? 1 : -1];
typedef char world_shape_penetration_collector_collision_offset[
    (sizeof(void *) != 4 || ((offsetof(c_world_shape_penetration_collector, found_collision) == 8))) ? 1 : -1];

#endif
