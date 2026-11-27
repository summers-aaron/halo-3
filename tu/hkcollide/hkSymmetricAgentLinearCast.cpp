// hkcollide/hkSymmetricAgentLinearCast.obj: from the records 0x627420 0x6274b8 0x627550
// @compiler xdk6130
// @flags /TP /O1
// @address 0x627550 ?add_proxy_pair@phys_symmetric_flip_pair_collector@@UAAXABVphys_collision_proxy@@0@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include "phys/phys_collide.h"
// @address 0x627420 ?add_hit@phys_symmetric_flip_collector@@UAAXABUphys_hit_point@@@Z
#include <ppcintrinsics.h>
#include "phys/phys_symmetric_agent_linear_cast.h"
void phys_symmetric_flip_pair_collector::add_proxy_pair(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b)
{
    m_collector.add_proxy_pair(agent_body_b, agent_body_a);
    m_early_out = m_collector.get_early_out();
}
void phys_symmetric_flip_collector::add_hit(const phys_hit_point &point)
{
    phys_hit_point flipped(point.m_proxy_b, point.m_proxy_a);
    flipped.m_hit_contact.set_flipped_contact(point.m_hit_contact);
    m_collector.add_hit(flipped);
    m_early_out_dist = m_collector.get_early_out_distance();
}

// @address 0x6274b8 ?add_hit@phys_symmetric_flip_cast_collector@@UAAXABUphys_hit_point@@@Z
void phys_symmetric_flip_cast_collector::add_hit(const phys_hit_point &point)
{
    phys_hit_point flipped(point.m_proxy_b, point.m_proxy_a);
    flipped.m_hit_contact.get_contact_position().set_add_mul4(point.m_hit_contact.get_contact_position(), m_path, point.m_hit_contact.get_distance_simd());
    flipped.m_hit_contact.get_separating_normal().set_neg3(point.m_hit_contact.get_separating_normal());
    m_collector.add_hit(flipped);
    m_early_out_dist = m_collector.get_early_out_distance();
}

