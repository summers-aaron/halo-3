// hkcollide/hkClosestCdPointCollector.obj: from the records 0x63ab40
// @flags /TP /O2 /Oc
#include "phys/phys_collectors.h"
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
// @address 0x63ab40 ?add_hit@phys_closest_hit_collector@@MAAXABUphys_hit_point@@@Z
#include <vectorintrinsics.h>

#include "phys/phys_closest_hit_collector.h"


void phys_closest_hit_collector::add_hit(const phys_hit_point &point)
{
    if (m_closest_hit.m_root_collider_a == 0 || point.m_hit_contact.get_distance() < m_closest_hit.m_hit_contact.get_distance()) {
        m_closest_hit.m_hit_contact = point.m_hit_contact;
        m_closest_hit.m_root_collider_a = point.m_proxy_a.get_root_collider();
        m_closest_hit.m_shape_key_a = point.m_proxy_a.get_shape_key();
        m_closest_hit.m_root_collider_b = point.m_proxy_b.get_root_collider();
        m_closest_hit.m_shape_key_b = point.m_proxy_b.get_shape_key();
        m_early_out_dist = point.m_hit_contact.get_distance();
    }
}
