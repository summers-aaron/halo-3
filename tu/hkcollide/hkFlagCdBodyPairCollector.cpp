// OBJ: from the records 0x63abe0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x63abe0 ?add_proxy_pair@phys_flag_proxy_pair_collector@@MAAXABVphys_collision_proxy@@0@Z
#include "phys/phys_base_types.h"
#include "phys/phys_flag_proxy_pair_collector.h"
void phys_flag_proxy_pair_collector::add_proxy_pair(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b)
{
    m_early_out = true;
}

