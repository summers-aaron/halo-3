// hkcollide/hkAgent3Bridge.obj: from the records 0x627600 0x627618 0x627640 0x627658 0x627670 0x627688 0x6276a0 0x6276b8 0x627770 0x6277c0
// @flags /TP /O2 /Oc
// @address 0x627600 ?destroy@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x627640 ?invalidate_sweep_info@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXAAUphys_collision_input@@@Z
// @address 0x627658 ?warp_time@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXMMAAUphys_collision_input@@@Z
// @address 0x627670 ?bridge_remove_point@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x627688 ?commit_potential@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x6276a0 ?mark_zombie_entry@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x627770 ?process@phys_narrow_agent_bridge@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
// @address 0x6277c0 ?register_narrow_agent@phys_narrow_agent_bridge@@YAHPAVphys_dispatcher@@@Z
// @address 0x6276b8 ?create@phys_narrow_agent_bridge@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z

#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_collision_input.h"

#include "phys/phys_agent_entry.h"
#include "tag_files/tag_types.h"

#define PHYS_GET_AGENT_TYPE(D, A, B, P, OUT) do { \
    int phys_type_a_ = (A); int phys_type_b_ = (B); phys_bool phys_pred_ = (P); \
    const unsigned char *phys_agent_table_ = phys_pred_ ? &(D)->m_agent2_types_pred[0][0] : &(D)->m_agent2_types[0][0]; \
    (OUT) = phys_agent_table_[phys_type_a_ * 32 + phys_type_b_]; \
} while (0)
typedef void *(*phys_create_func)(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &, phys_contact_manager *);

#include "phys/phys_narrow_agent_bridge.h"
#include "phys/phys_collectors.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"

static __forceinline phys_agent *phys_bridge_get_agent(phys_agent_entry *entry)
{
    return *reinterpret_cast<TAG_POINTER(phys_agent) *>(entry + 1);
}

static __forceinline void phys_bridge_set_agent(phys_agent_entry *entry, phys_agent *agent)
{
    *reinterpret_cast<TAG_POINTER(phys_agent) *>(entry + 1) = agent;
}

void phys_narrow_agent_bridge::destroy(phys_agent_entry *entry, void *agent_data, phys_contact_manager *agent_contact_manager,
    phys_constraint_owner &owner, phys_dispatcher *dispatcher)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->cleanup(owner);
}

// @address 0x627618 ?update_agent_filter@phys_narrow_agent_bridge@@YAXPAUphys_agent_entry@@PAXAAVphys_collision_proxy@@2ABUphys_collision_input@@PAVphys_contact_manager@@AAVphys_constraint_owner@@@Z
void phys_narrow_agent_bridge::update_agent_filter(phys_agent_entry *entry, void *agent_data, phys_collision_proxy &agent_body_a,
    phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->update_compound_filter(agent_body_a, agent_body_b, input, owner);
}

void phys_narrow_agent_bridge::invalidate_sweep_info(phys_agent_entry *entry, void *agent_data, phys_collision_input &input)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->invalidate_sweep_info(input);
}

void phys_narrow_agent_bridge::warp_time(phys_agent_entry *entry, void *agent_data, float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->warp_time(agent_old_time, agent_new_time, input);
}

void phys_narrow_agent_bridge::bridge_remove_point(phys_agent_entry *entry, void *agent_data, unsigned short bounding_contact_point_id)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->bridge_remove_point(bounding_contact_point_id);
}

void phys_narrow_agent_bridge::commit_potential(phys_agent_entry *entry, void *agent_data, unsigned short bounding_contact_point_id)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->commit_potential(bounding_contact_point_id);
}

void phys_narrow_agent_bridge::mark_zombie_entry(phys_agent_entry *entry, void *agent_data, unsigned short bounding_contact_point_id)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->mark_zombie_entry(bounding_contact_point_id);
}

void *phys_narrow_agent_bridge::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data)
{
    phys_contact_manager *agent_contact_manager = input.m_contact_manager;
    const phys_process_collision_input *bridge_process_input = input.m_input;
    const phys_collision_proxy *agent_body_b = input.m_body_b;
    const phys_collision_proxy *agent_body_a = input.m_body_a;
    const phys_process_collision_input *dispatch_input = input.m_input;
    phys_dispatcher *dispatcher = (phys_dispatcher *)dispatch_input->m_agent_dispatcher.val();
    int shape_type_a = agent_body_a->m_proxy_shape->m_type;
    int shape_type_b = agent_body_b->m_proxy_shape->m_type;
    int index;
    PHYS_GET_AGENT_TYPE(dispatcher, shape_type_a, shape_type_b, bridge_process_input->m_create_pred_agents, index);
    phys_create_func create = (phys_create_func)dispatcher->m_agent2_funcs[index].m_create_fn;
    phys_bridge_set_agent(entry, (phys_agent *)create(*agent_body_a, *agent_body_b, *bridge_process_input, agent_contact_manager));
    entry->m_stream_command = 6;
    entry->m_contact_point_count = 255;
    return agent_data;
}

void *phys_narrow_agent_bridge::process(
    const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data, phys_vec4 *simplex_separating_normal,
        phys_process_collision_output &output)
{
    phys_agent *pair_agent_entry = phys_bridge_get_agent(entry);
    pair_agent_entry->process_collision(*input.m_body_a, *input.m_body_b, *input.m_input, output);
    return agent_data;
}

int phys_narrow_agent_bridge::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.commit_potential = commit_potential;
    dispatcher_agent_funcs.mark_zombie_entry = mark_zombie_entry;
    dispatcher_agent_funcs.bridge_remove_point = bridge_remove_point;
    dispatcher_agent_funcs.create = create;
    dispatcher_agent_funcs.process = process;
    dispatcher_agent_funcs.compute_sep_normal = 0;
    dispatcher_agent_funcs.cleanup = 0;
    dispatcher_agent_funcs.destroy = destroy;
    dispatcher_agent_funcs.update_agent_filter = update_agent_filter;
    dispatcher_agent_funcs.invalidate_sweep_info = invalidate_sweep_info;
    dispatcher_agent_funcs.warp_time = warp_time;
    dispatcher_agent_funcs.agent_is_predictive = 1;
    int agent_type_id = dispatcher->register_narrow_agent(dispatcher_agent_funcs, PHYS_SHAPE_ALL, PHYS_SHAPE_ALL);
    return agent_type_id;
}

