// hkcollide/hkAllCdPointCollector.obj: from the records 0x626008 0x6260f0 0x626138
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x626008 ?add_hit@phys_all_hits_collector@@MAAXABUphys_hit_point@@@Z
// @address 0x6260f0 ?sort_hits@phys_all_hits_collector@@QAAXXZ
// @address 0x626138 ??$quick_sort_recursive@Uphys_root_hit_point@@V?$less@Uphys_root_hit_point@@@phys_algorithm@@@phys_algorithm@@YAXPAUphys_root_hit_point@@HHV?$less@Uphys_root_hit_point@@@0@@Z
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_collide.h"
#include "phys/phys_algorithm_sort.h"

void phys_all_hits_collector::add_hit(const phys_hit_point &event)
{
    phys_root_hit_point &hit = m_hit_list.expand_one();
    hit.m_hit_contact = event.m_hit_contact;
    hit.m_root_collider_a = event.m_proxy_a.get_root_collider();
    hit.m_shape_key_a = event.m_proxy_a.get_shape_key();
    hit.m_root_collider_b = event.m_proxy_b.get_root_collider();
    hit.m_shape_key_b = event.m_proxy_b.get_shape_key();
}

void phys_all_hits_collector::sort_hits()
{
    phys_algorithm::quick_sort(m_hit_list.begin(), m_hit_list.getSize());
}
