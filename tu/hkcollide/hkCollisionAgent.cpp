// hkcollide/hkCollisionAgent.obj: from the records 0x6273f8 0x627400 0x627408 0x627410 0x627418
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x6273f8 ?invalidate_sweep_info@phys_agent@@UAAXAAUphys_collision_input@@@Z
// @address 0x627400 ?warp_time@phys_agent@@UAAXMMAAUphys_collision_input@@@Z
// @address 0x627408 ?bridge_remove_point@phys_agent@@UAAXG@Z
// @address 0x627410 ?commit_potential@phys_agent@@UAAXG@Z
// @address 0x627418 ?mark_zombie_entry@phys_agent@@UAAXG@Z

#include "phys/phys_agent.h"

void phys_agent::invalidate_sweep_info(phys_collision_input &input)
{
}

void phys_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
}

void phys_agent::bridge_remove_point(unsigned short bounding_contact_point_id)
{
}

void phys_agent::commit_potential(unsigned short bounding_contact_point_id)
{
}

void phys_agent::mark_zombie_entry(unsigned short bounding_contact_point_id)
{
}

