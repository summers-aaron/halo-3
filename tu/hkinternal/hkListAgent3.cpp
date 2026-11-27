// hkinternal/hkListAgent3.obj: from the records 0x68cb68 0x68cb78 0x68cb88 0x68cb98 0x68cc00 0x68cc50 0x68ce58 0x68cec8
// @flags /TP /O2 /Oc
// @compiler xdk5632
// @address 0x68cb68 ?destroy@phys_list_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x68cc00 ?create@phys_list_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x68cc50 ?process@phys_list_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
// @address 0x68ce58 ?init_agent_funcs@phys_list_narrow_agent@@YAXAAUnarrow_agent_funcs@phys_dispatcher@@@Z
// @address 0x68cec8 ?register_narrow_agent@phys_list_narrow_agent@@YAXPAVphys_dispatcher@@@Z
// @address 0x68cb78 phys_list_narrow_agent_invalidate_sweep
// @address 0x68cb88 phys_list_narrow_agent_warp_time
// @address 0x68cb98 phys_list_narrow_agent_update_compound_filter
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_list_narrow_agent.h"
#include "phys/phys_dispatcher.h"


#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_batch_agent_visitor_input.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_swept_transform_util_sweep.h"

extern "C" void phys_list_narrow_agent_warp_time(phys_agent_entry *entry, void *agent_data, float agent_old_time, float agent_new_time,
    phys_collision_input &input)
{
    phys_batch_agent_warp_time(static_cast<phys_batch_agent_track *>(agent_data), agent_old_time, agent_new_time, &input);
}

void phys_list_narrow_agent_invalidate_sweep(phys_agent_entry *entry, void *agent_data, phys_collision_input &input)
{
    phys_batch_agent_invalidate_sweep(static_cast<phys_batch_agent_track *>(agent_data), &input);
}

void phys_list_narrow_agent_update_compound_filter(phys_agent_entry *entry, void *agent_data, phys_collision_proxy &agent_body_a,
    phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref)
{
    const phys_shape_container *shape_container_b = agent_body_b.get_shape()->get_shape_container();
    phys_batch_agent_visitor_input visitor_input;
    visitor_input.m_body_a = &agent_body_a;
    visitor_input.m_compound_proxy_b = &agent_body_b;
    visitor_input.m_input = &input;
    visitor_input.m_contact_manager = pair_contact_manager;
    visitor_input.m_constraint_owner = &constraint_owner_ref;
    visitor_input.m_container_shape_b = shape_container_b;
    phys_batch_agent_update_compound_filter(*static_cast<phys_batch_agent_track *>(agent_data), visitor_input);
}

void phys_list_narrow_agent::destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager,
    phys_constraint_owner &owner, phys_dispatcher *dispatcher)
{
    phys_batch_agent_destroy(*static_cast<phys_batch_agent_track *>(data), dispatcher, agent_contact_manager, owner);
}

void *phys_list_narrow_agent::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data)
{
    new (data) phys_batch_agent_track;
    phys_batch_agent_create(data);
    return (char *)data + 16;
}

namespace {
__forceinline void phys_list_narrow_agent_init_funcs(phys_dispatcher::narrow_agent_funcs &functions)
{
    functions.create = phys_list_narrow_agent::create;
    functions.process = phys_list_narrow_agent::process;
    functions.compute_sep_normal = 0;
    functions.cleanup = 0;
    functions.bridge_remove_point = 0;
    functions.commit_potential = 0;
    functions.mark_zombie_entry = 0;
    functions.update_agent_filter = phys_list_narrow_agent_update_compound_filter;
    functions.invalidate_sweep_info = phys_list_narrow_agent_invalidate_sweep;
    functions.warp_time = phys_list_narrow_agent_warp_time;
    functions.destroy = phys_list_narrow_agent::destroy;
    functions.agent_is_predictive = 1;
}
}

void phys_list_narrow_agent::init_agent_funcs(phys_dispatcher::narrow_agent_funcs &dispatcher_fns)
{
    phys_list_narrow_agent_init_funcs(dispatcher_fns);
}

void phys_list_narrow_agent::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs functions;
    phys_list_narrow_agent_init_funcs(functions);
    dispatcher->register_narrow_agent(functions, PHYS_SHAPE_ALL, PHYS_SHAPE_LIST);
}



// @address 0x68cc50
void *phys_list_narrow_agent::process(
    const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data, phys_vec4 *simplex_separating_normal,
        phys_process_collision_output &output)
{
    phys_batch_agent_track *batch_agent_track = static_cast<phys_batch_agent_track *>(agent_data);

    const phys_list_shape *list_shape_b = static_cast<const phys_list_shape *>(input.m_body_b->get_shape());

    phys_narrow_agent_process_input sphere_input;
    sphere_input.m_body_a = input.m_body_a;
    sphere_input.m_body_b = input.m_body_b;
    sphere_input.m_contact_manager = input.m_contact_manager;
    sphere_input.m_input = input.m_input;

    const phys_motion_state *motion_a_state = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_b_state = input.m_body_b->get_motion_state();
    phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, input.m_input->m_step_info.m_delta_time, sphere_input.m_lin_sweep_info);
    sphere_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());

    int size = list_shape_b->m_child_info.getSize();
    phys_local_buffer<phys_shape_key> hit_key_list(size + 1, "ListAgent3");
    for (int i = 0; i < size; i++) {
        hit_key_list[i] = i;
    }
    hit_key_list[size] = 0xffffffff;

    phys_batch_agent_track *batch_agent_track_ppu =
        reinterpret_cast<phys_batch_agent_track *>(reinterpret_cast<char *>(batch_agent_track) + input.m_sector_offset_delta);
    phys_batch_agent_process(*batch_agent_track, batch_agent_track_ppu, sphere_input, list_shape_b, hit_key_list.begin(), size, output);

    return reinterpret_cast<char *>(agent_data) + 16;
}

