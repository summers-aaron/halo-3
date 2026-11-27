#ifndef INCLUDE_PHYS_COLLECTORS_H
#define INCLUDE_PHYS_COLLECTORS_H

// Havok 4.5 collision query collector bases (Physics/Collide/Agent/Query), as the game ships them:
// Bungie added add_phantom_callback_shape as the third virtual of hkCdPointCollector.

#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory.h"

class phys_collision_proxy;
struct phys_hit_point;

class phys_hit_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_hit_collector);

    __forceinline phys_hit_collector() { reset(); }
    virtual __forceinline ~phys_hit_collector() {}
    virtual void add_hit(const phys_hit_point &point) = 0;
    virtual __forceinline void add_volume_callback_shape(const phys_collision_proxy &body) {}

    __forceinline void reset() { m_early_out_dist = 3.40282e+38f; }
    __forceinline float get_early_out_distance() const { return m_early_out_dist; }

protected:
    float m_early_out_dist;
};

class phys_proxy_pair_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_proxy_pair_collector);

    __forceinline phys_proxy_pair_collector() { reset(); }
    virtual __forceinline ~phys_proxy_pair_collector() {}
    virtual void add_proxy_pair(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b) = 0;

    __forceinline void reset() { m_early_out = false; }
    __forceinline phys_bool get_early_out() const { return m_early_out; }

protected:
    phys_bool m_early_out;
};

#endif // INCLUDE_HAVOK_HK_COLLECTORS_H
