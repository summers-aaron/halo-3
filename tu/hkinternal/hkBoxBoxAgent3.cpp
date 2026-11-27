// hkinternal/hkBoxBoxAgent3.obj: from the records 0x68b188 0x68b210 0x68b258 0x68b260 0x68b2a0 0x68b320 0x68b328 0x68b500 0x68b568
// @compiler xdk5632
// @flags /TP /O2 /J /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_collide.h"
// @address 0x68b188 ?bridge_remove_point@phys_box_box_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68b210 ?commit_potential@phys_box_box_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68b258 ?mark_zombie_entry@phys_box_box_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68b260 ?create@phys_box_box_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x68b2a0 ?cleanup@phys_box_box_narrow_agent@@YAPAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@@Z
// @address 0x68b320 ?destroy@phys_box_box_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x68b500 ?init_agent_funcs@phys_box_box_narrow_agent@@YAXAAUnarrow_agent_funcs@phys_dispatcher@@@Z
// @address 0x68b568 ?register_narrow_agent@phys_box_box_narrow_agent@@YAXPAVphys_dispatcher@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_box_box_manifold.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_agent_entry.h"

#include "phys/phys_box_box_narrow_agent.h"

#include "phys/phys_box_box_detector.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collision_input.h"

void phys_box_box_narrow_agent::bridge_remove_point(phys_agent_entry *entry, void *data, unsigned short id)
{
    phys_box_box_manifold *box_manifold = (phys_box_box_manifold *)data;
    int count = box_manifold->m_contact_point_count;
    for (int index = 0; index < count; index++) {
        if (box_manifold->m_process_contact_points[index].m_process_contact_id == id) {
            box_manifold->bridge_remove_point(index);
            entry->m_contact_point_count = entry->m_contact_point_count - 1;
            return;
        }
    }
}

void phys_box_box_narrow_agent::commit_potential(phys_agent_entry *entry, void *data, unsigned short id)
{
    phys_box_box_manifold *box_manifold = (phys_box_box_manifold *)data;
    for (int index = 0; index < box_manifold->get_manifold_point_count(); index++) {
        if (box_manifold->m_process_contact_points[index].m_process_contact_id == 0xffff) {
            box_manifold->m_process_contact_points[index].m_process_contact_id = id;
            return;
        }
    }
}

void phys_box_box_narrow_agent::mark_zombie_entry(phys_agent_entry *entry, void *data, unsigned short id)
{
}

void *phys_box_box_narrow_agent::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data)
{
    new (data) phys_box_box_manifold;
    return (char *)data + 64;
}

void *phys_box_box_narrow_agent::cleanup(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner)
{
    phys_box_box_manifold *box_manifold = (phys_box_box_manifold *)data;
    for (int index = 0; index < box_manifold->get_manifold_point_count(); index++) {
        if (box_manifold->m_process_contact_points[index].m_process_contact_id != 0xffff) {
            agent_contact_manager->contact_remove_point(box_manifold->m_process_contact_points[index].m_process_contact_id, owner);
        }
    }
    box_manifold->m_contact_point_count = 0;
    entry->m_contact_point_count = 0;
    return (char *)data + 64;
}

void phys_box_box_narrow_agent::destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager,
    phys_constraint_owner &owner, phys_dispatcher *dispatcher)
{
    cleanup(entry, data, agent_contact_manager, owner);
}

void phys_box_box_narrow_agent::init_agent_funcs(phys_dispatcher::narrow_agent_funcs &functions)
{
    functions.create = create;
    functions.process = process;
    functions.compute_sep_normal = 0;
    functions.cleanup = cleanup;
    functions.bridge_remove_point = bridge_remove_point;
    functions.commit_potential = commit_potential;
    functions.mark_zombie_entry = mark_zombie_entry;
    functions.update_agent_filter = 0;
    functions.destroy = destroy;
    functions.agent_is_predictive = 0;
}

void phys_box_box_narrow_agent::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs functions;
    init_agent_funcs(functions);
    dispatcher->register_narrow_agent(functions, PHYS_SHAPE_BOX, PHYS_SHAPE_BOX);
}



void *phys_box_box_narrow_agent::process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry,
                              void *agent_data, phys_vec4 *simplex_separating_normal,
                              phys_process_collision_output &output)
{
    phys_box_box_manifold *box_manifold = (phys_box_box_manifold *)agent_data;

    const phys_box_shape *box_a = (const phys_box_shape *)input.m_body_a->get_shape();
    const phys_box_shape *box_b = (const phys_box_shape *)input.m_body_b->get_shape();

    const phys_vec4 &extent_a = box_a->get_half_extents();
    const phys_vec4 &extent_b = box_b->get_half_extents();

    phys_vec4 rA;
    rA.flags_set_all(box_a->get_convex_radius());
    phys_vec4 lever_a4;
    lever_a4.set_add4(extent_a, rA);
    phys_vec4 rB;
    rB.flags_set_all(box_b->get_convex_radius());
    phys_vec4 lever_b4;
    lever_b4.set_add4(extent_b, rB);

    phys_box_box_detector box_box_detector(*input.m_body_a, *input.m_body_b, input.m_input, input.m_contact_manager, &output,
        input.m_a_to_b, input.m_body_a->get_transform(), lever_a4, input.m_body_b->get_transform(), lever_b4,
        input.m_input->get_tolerance());

    box_box_detector.compute_box_manifold(*box_manifold);

    entry->m_contact_point_count = box_manifold->m_contact_point_count;

    return (char *)agent_data + 64;
}
// @address 0x68b328 ?process@phys_box_box_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
