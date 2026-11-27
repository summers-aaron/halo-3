// hkdynamics/hkEntityEntityBroadPhaseListener.obj: from the records 0x644390 0x65ab38 0x65ab58 0x65abc8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x65ab38 ??0phys_entity_pair_broadphase_listener@@QAA@PAVphys_world@@@Z
// @address 0x65ab58 ?add_collision_pair@phys_entity_pair_broadphase_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z
// @address 0x65abc8 ?remove_collision_pair@phys_entity_pair_broadphase_listener@@UAAXAAVphys_typed_broadphase_pair@@@Z
#include "phys/phys_dynamics.h"
#include "phys/phys_pair_agent_machine.h"
// @address 0x644390 ??_Gphys_entity_pair_broadphase_listener@@UAAPAXI@Z

#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_broadphase_listener.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_collision_input.h"

#include "phys/phys_entity_pair_broadphase_listener.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_statistics_collector.h"

phys_entity_pair_broadphase_listener::phys_entity_pair_broadphase_listener(phys_world *world)
    : m_world(world)
{
}

void phys_entity_pair_broadphase_listener::add_collision_pair(phys_typed_broadphase_pair &pair)
{
    phys_linked_collider *pair_collider_a = static_cast<phys_linked_collider *>(pair.get_element_a()->get_collider_owner());
    phys_linked_collider *pair_collider_b = static_cast<phys_linked_collider *>(pair.get_element_b()->get_collider_owner());
    phys_process_collision_input *input = m_world->get_collision_input_rw();
    phys_collider_quality quality_a = pair_collider_a->get_quality_type();
    phys_collider_quality quality_b = pair_collider_b->get_quality_type();
    signed char quality_index = input->m_agent_dispatcher->get_collision_quality_idx(quality_a, quality_b);
    if (quality_index == 0)
        return;
    phys_collision_quality_info *quality_info = input->m_agent_dispatcher->get_collision_quality_info(quality_index);
    input->m_create_pred_agents = quality_info->m_use_ccd;
    phys_world_agent_util::add_pair_agent(pair_collider_a, pair_collider_b, *input);
}

void phys_entity_pair_broadphase_listener::remove_collision_pair(phys_typed_broadphase_pair &pair)
{
    phys_linked_collider *pair_collider_a = static_cast<phys_linked_collider *>(pair.get_element_a()->get_collider_owner());
    phys_linked_collider *pair_collider_b = static_cast<phys_linked_collider *>(pair.get_element_b()->get_collider_owner());
    phys_pair_agent_entry *entry = phys_pair_agent_find(pair_collider_a, pair_collider_b);
    if (entry)
        phys_world_agent_util::remove_agent(entry);
}

