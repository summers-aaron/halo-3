// hkcollide/hkTransformAgent.obj: from the records 0x635110 0x635168 0x635180 0x635198 0x6351b0 0x6351c8 0x6351e0 0x6352e8 0x6354c0 0x635548 0x6355f0 0x635670 0x635710 0x635790 0x635830 0x6358b0 0x6359c0 0x635a90 0x635bc0 0x635c10 0x635c60 0x635d48 0x635dd8 0x635e70 0x635f20 0x635fa0
// @flags /TP /O2 /J /Oc
// @address 0x635110 ?cleanup@phys_transform_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x635168 ?invalidate_sweep_info@phys_transform_agent@@UAAXAAUphys_collision_input@@@Z
// @address 0x635180 ?warp_time@phys_transform_agent@@UAAXMMAAUphys_collision_input@@@Z
// @address 0x635198 ?bridge_remove_point@phys_transform_agent@@UAAXG@Z
// @address 0x6351b0 ?commit_potential@phys_transform_agent@@UAAXG@Z
// @address 0x6351c8 ?mark_zombie_entry@phys_transform_agent@@UAAXG@Z
// @address 0x6354c0 ?sweep_cast@phys_transform_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6355f0 ?get_closest_points@phys_transform_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x635710 ?get_penetrations@phys_transform_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x635830 ?update_compound_filter@phys_transform_agent@@MAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
// @address 0x6359c0 ?register_agent@phys_transform_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x635c10 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x635c60 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x635dd8 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x635fa0 ?sweep_cast@?$phys_symmetric_agent@Vphys_transform_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
#include "phys/phys_dispatcher.h"
#include <vectorintrinsics.h>
#include "phys/phys_transform_shape.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_transform_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_constraint_owner.h"

// @address 0x635790 ?static_penetrations@phys_transform_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x635a90 ??0phys_motion_state@@QAA@ABV0@@Z
// @address 0x635bc0 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x635e70 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x635f20 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_transform_agent::cleanup(phys_constraint_owner &owner)
{
    m_child_agent->cleanup(owner);
    delete this;
}
void phys_transform_agent::invalidate_sweep_info(phys_collision_input &input)
{
    m_child_agent->invalidate_sweep_info(input);
}
void phys_transform_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    m_child_agent->warp_time(agent_old_time, agent_new_time, input);
}
void phys_transform_agent::bridge_remove_point(unsigned short bounding_contact_point_id)
{
    m_child_agent->bridge_remove_point(bounding_contact_point_id);
}
void phys_transform_agent::commit_potential(unsigned short bounding_contact_point_id)
{
    m_child_agent->commit_potential(bounding_contact_point_id);
}
void phys_transform_agent::mark_zombie_entry(unsigned short bounding_contact_point_id)
{
    m_child_agent->mark_zombie_entry(bounding_contact_point_id);
}
void phys_transform_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector,
    phys_hit_collector *start_collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    m_child_agent->sweep_cast(child_proxy, agent_body_b, input, collector, start_collector);
}
void phys_transform_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    m_child_agent->get_closest_points(child_proxy, agent_body_b, input, collector);
}
void phys_transform_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    m_child_agent->get_penetrations(child_proxy, agent_body_b, input, collector);
}
void phys_transform_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    phys_shape_type shape_type_a = child_proxy.get_shape()->get_type_id();
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    phys_dispatcher::agent_funcs::penetrations_fn function = input.m_agent_dispatcher->get_penetrations_fn(shape_type_a, shape_type_b);
    function(child_proxy, agent_body_b, input, collector);
}
void phys_transform_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    m_child_agent->update_compound_filter(child_proxy, agent_body_b, input, owner);
}

void phys_transform_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_transform_agent::create_transform_b_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_transform_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_transform_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_transform_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, (phys_shape_type)-1, (phys_shape_type)28);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_transform_agent::create_transform_a_agent;
  backward.m_penetrations_fn = phys_transform_agent::static_penetrations;
  backward.m_closest_points_fn = phys_transform_agent::static_closest_points;
  backward.m_linear_cast_func = phys_transform_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, (phys_shape_type)28, (phys_shape_type)-1);
}
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_transform_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_transform_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::static_sweep_cast(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_transform_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_transform_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_transform_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
    phys_process_collision_output &output)
{
    phys_process_hit_point *first_free_point = output.m_next_free_contact;
    float toi_time = output.m_collision_toi.m_time;
    phys_transform_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; first_free_point < output.m_next_free_contact; ++first_free_point) {
        first_free_point->m_hit_contact.set_flipped_contact(first_free_point->m_hit_contact);
    }
    if (toi_time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_constraint_owner &owner)
{
    phys_transform_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
template <> void phys_symmetric_agent<phys_transform_agent>::sweep_cast(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_transform_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_transform_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}
// @address 0x635670 ?static_closest_points@phys_transform_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_transform_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    phys_dispatcher::agent_funcs::closest_points_fn func = input.m_agent_dispatcher->get_closest_points_fn(
        child_proxy.get_shape()->get_type_id(), agent_body_b.get_shape()->get_type_id());
    func(child_proxy, agent_body_b, input, collector);
}
// @address 0x635d48 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_transform_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template <> void phys_symmetric_agent_linear_cast<phys_transform_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_transform_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x6352e8 ?process_collision@phys_transform_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_transform_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_motion_state ms;
    ms.get_transform().set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    const phys_motion_state *src_motion_state = static_cast<const phys_motion_state *>(agent_body_a.m_motion);
    phys_swept_transform &swept_xform = ms.get_swept_transform();
    const phys_swept_transform &src_swept = src_motion_state->get_swept_transform();
    swept_xform.m_center_of_mass0 = src_swept.m_center_of_mass0;
    swept_xform.m_center_of_mass1 = src_swept.m_center_of_mass1;
    swept_xform.m_rotation_0.set_mul(src_swept.m_rotation_0, xform_shape->m_rotation);
    swept_xform.m_rotation_1.set_mul(src_swept.m_rotation_1, xform_shape->m_rotation);
    swept_xform.m_center_of_mass_local.set_transformed_inverse_pos(xform_shape->get_transform(), src_swept.m_center_of_mass_local);
    ms.m_delta_angle = src_motion_state->m_delta_angle;
    ms.m_object_radius = src_motion_state->m_object_radius;
    phys_collision_proxy proxy_copy(&agent_body_a, &ms.get_transform());
    proxy_copy.set_proxy_shape(xform_shape->get_child_shape());
    m_child_agent->process_collision(proxy_copy, agent_body_b, input, result);
}

// 0x635548
// @address 0x635548 ?static_sweep_cast@phys_transform_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_transform_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    const phys_transform_shape *xform_shape = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    phys_transform transform;
    transform.set_mul(agent_body_a.get_transform(), xform_shape->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &transform);
    child_proxy.set_proxy_shape(xform_shape->get_child_shape());
    const phys_dispatcher *d = input.m_agent_dispatcher;
    unsigned shape_type_a = child_proxy.get_shape()->m_type;
    unsigned shape_type_b = agent_body_b.get_shape()->m_type;
    unsigned t = d->m_agent2_types[shape_type_a][shape_type_b];
    phys_dispatcher::agent_funcs::sweep_cast_fn f = d->m_agent2_funcs[t].m_linear_cast_func;
    f(child_proxy, agent_body_b, input, collector, start_collector);
}

__forceinline phys_transform_agent::phys_transform_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
    : phys_agent(pair_contact_manager)
{
    const phys_transform_shape *xform_shape_a = static_cast<const phys_transform_shape *>(agent_body_a.get_shape());
    const phys_shape *bounding_child_shape = xform_shape_a->get_child_shape();
    phys_motion_state ms = *agent_body_a.get_motion_state();
    ms.get_transform().set_mul(agent_body_a.get_transform(), xform_shape_a->get_transform());
    phys_collision_proxy child_proxy(&agent_body_a, &ms);
    child_proxy.set_proxy_shape(bounding_child_shape);
    m_child_agent = input.m_agent_dispatcher->create_collision_agent(child_proxy, agent_body_b, input, pair_contact_manager);
}

// @address 0x6358b0 ?create_transform_b_agent@phys_transform_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_transform_agent::create_transform_b_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_transform_agent *pair_agent_entry = new phys_symmetric_agent<phys_transform_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

// @address 0x6351e0 ?create_transform_a_agent@phys_transform_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_transform_agent::create_transform_a_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_transform_agent *pair_agent_entry = new phys_transform_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

