// hkcollide/hkSimpleClosestContactCollector.obj: from the records 0x63abf0
// @flags /TP /O2 /Oc
#include "phys/phys_collectors.h"
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
// @address 0x63abf0 ?add_hit@phys_simple_closest_collector@@MAAXABUphys_hit_point@@@Z
#include <vectorintrinsics.h>

#include "phys/phys_simple_closest_collector.h"

void phys_simple_closest_collector::add_hit(const phys_hit_point &point)
{
    if (!m_has_hit || point.m_hit_contact.get_distance() < m_closest_hit_point.get_distance()) {
        m_has_hit = 1;
        m_closest_hit_point = point.m_hit_contact;
        m_early_out_dist = point.m_hit_contact.get_distance();
    }
}
