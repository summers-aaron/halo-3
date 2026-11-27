#ifndef INCLUDE_PHYS_CLOSEST_HIT_COLLECTOR_H
#define INCLUDE_PHYS_CLOSEST_HIT_COLLECTOR_H

// hkClosestCdPointCollector, defined in tu/hkcollide/hkClosestCdPointCollector.cpp.

#include "phys/phys_collectors.h"
#include "phys/phys_collide.h"

class phys_closest_hit_collector : public phys_hit_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_closest_hit_collector);
    // March 0x834a2ca0, 0x834a2ce8, 0x834a2df8, 0x834a2e40.
    __forceinline phys_closest_hit_collector() { reset(); }
    virtual __forceinline ~phys_closest_hit_collector() {}
    __forceinline void reset() {
        m_closest_hit.m_root_collider_a = 0;
        m_closest_hit.m_hit_contact.contact_set_distance(3.40282e+38f);
        phys_hit_collector::reset();
    }
    __forceinline phys_bool has_hit() const { return m_closest_hit.m_root_collider_a != 0; }
    __forceinline const phys_contact_point &get_hit_contact() const { return m_closest_hit.m_hit_contact; }
protected:
    virtual void add_hit(const phys_hit_point &point);
    phys_root_hit_point m_closest_hit;
};

#endif // HAVOK_HK_CLOSEST_CD_POINT_COLLECTOR_H
