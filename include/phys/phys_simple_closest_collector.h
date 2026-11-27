#ifndef INCLUDE_PHYS_SIMPLE_CLOSEST_COLLECTOR_H
#define INCLUDE_PHYS_SIMPLE_CLOSEST_COLLECTOR_H

// hkSimpleClosestContactCollector, defined in tu/hkcollide/hkSimpleClosestContactCollector.cpp. The hit flag is read
// as one byte.

#include "phys/phys_collectors.h"
#include "phys/phys_collide.h"

class phys_simple_closest_collector : public phys_hit_collector {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_AGENT, phys_simple_closest_collector);
    // March 0x834a3060, 0x834a30b0, 0x834a31c0, 0x834aecd0.
    __forceinline phys_simple_closest_collector() { reset(); }
    virtual __forceinline ~phys_simple_closest_collector() {}
    __forceinline void reset() {
        m_has_hit = false;
        m_closest_hit_point.contact_set_distance(3.40282e+38f);
        phys_hit_collector::reset();
    }
    __forceinline phys_bool has_hit() const { return m_has_hit; }
    __forceinline const phys_contact_point &get_hit_contact() const { return m_closest_hit_point; }
protected:
    virtual void add_hit(const phys_hit_point &point);
    phys_bool m_has_hit;
    phys_contact_point m_closest_hit_point;
};

#endif // HAVOK_HK_SIMPLE_CLOSEST_CONTACT_COLLECTOR_H
