// hkcollide/hkBvAgent.obj: from the records 0x104818 0x638618 0x6386a0 0x638710 0x6387a0 0x6387c8 0x6387f0 0x638818 0x638870 0x638918 0x6389f0 0x638a48 0x638ba0 0x638ce8 0x638e08 0x638f30 0x639050 0x6390d0 0x639140 0x639210 0x639260 0x6392b0 0x639398 0x639410 0x639420 0x639470
// @compiler xdk6130
// @flags /TP /O2
#include "phys/phys_base.h"
// @address 0x638618 ?cleanup@phys_bounding_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x6386a0 ?invalidate_sweep_info@phys_bounding_agent@@UAAXAAUphys_collision_input@@@Z
// @address 0x638710 ?warp_time@phys_bounding_agent@@UAAXMMAAUphys_collision_input@@@Z
// @address 0x639140 ?register_agent@phys_bounding_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x639410 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <vectorintrinsics.h>
#include "phys/phys_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_bounding_shape.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_simple_closest_collector.h"
#include "phys/phys_flag_proxy_pair_collector.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_memory.h"
#include "phys/phys_statistics_collector.h"


extern "C" {
}


// The dispatcher's agent entry for one direction of a shape pair (20 bytes).

#include "phys/phys_bounding_agent.h"



struct s_agent;
extern "C" {
}
extern "C" {
}
extern "C" {
}
extern "C" {
}


extern "C" {

}





namespace {
}


namespace {
}

// @address 0x638818 ?get_penetrations@phys_bounding_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
__forceinline void phys_bounding_agent::get_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy bounding_body_proxy(&agent_body_a);
    bounding_body_proxy.set_proxy_shape(shape->get_bounding_volume_shape());
    m_bounding_agent->get_penetrations(bounding_body_proxy, agent_body_b, input, collector);
}


namespace {
}


extern "C" {
}



// 0x104818
// @address 0x104818 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *free_contact_point = output.m_next_free_contact;
    float old_toi_time = output.m_collision_toi.m_time;
    phys_bounding_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; free_contact_point < output.m_next_free_contact; ++free_contact_point) {
        free_contact_point->m_hit_contact.set_flipped_contact(free_contact_point->m_hit_contact);
    }
    if (old_toi_time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

// 0x638618
void phys_bounding_agent::cleanup(phys_constraint_owner &constraint_owner_ref)
{
    m_bounding_agent->cleanup(constraint_owner_ref);
    if (m_child_agent) {
        m_child_agent->cleanup(constraint_owner_ref);
        m_child_agent = 0;
    }
    delete this;
}

// 0x6386a0
void phys_bounding_agent::invalidate_sweep_info(phys_collision_input &input)
{
    m_bounding_agent->invalidate_sweep_info(input);
    if (m_child_agent != 0) {
        m_child_agent->invalidate_sweep_info(input);
    }
}

// 0x638710
void phys_bounding_agent::warp_time(float warp_old_time, float warp_new_time, phys_collision_input &input)
{
    m_bounding_agent->warp_time(warp_old_time, warp_new_time, input);
    if (m_child_agent) {
        m_child_agent->warp_time(warp_old_time, warp_new_time, input);
    }
}

// 0x6387a0
// @address 0x6387a0 ?bridge_remove_point@phys_bounding_agent@@UAAXG@Z
void phys_bounding_agent::bridge_remove_point(unsigned short bounding_contact_point_id)
{
    if (m_child_agent != 0) {
        m_child_agent->bridge_remove_point(bounding_contact_point_id);
    }
}

// 0x6387c8
// @address 0x6387c8 ?commit_potential@phys_bounding_agent@@UAAXG@Z
void phys_bounding_agent::commit_potential(unsigned short bounding_contact_point_id)
{
    if (m_child_agent != 0) {
        m_child_agent->commit_potential(bounding_contact_point_id);
    }
}

// 0x6387f0
// @address 0x6387f0 ?mark_zombie_entry@phys_bounding_agent@@UAAXG@Z
void phys_bounding_agent::mark_zombie_entry(unsigned short bounding_contact_point_id)
{
    if (m_child_agent != 0) {
        m_child_agent->mark_zombie_entry(bounding_contact_point_id);
    }
}

// 0x638870
// @address 0x638870 ?update_compound_filter@phys_bounding_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_bounding_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy body(&agent_body_a);
    body.set_proxy_shape(shape->get_bounding_volume_shape());
    m_bounding_agent->update_compound_filter(body, agent_body_b, input, owner);
    if (m_child_agent) {
        body.set_proxy_shape(shape->get_child_shape());
        m_child_agent->update_compound_filter(body, agent_body_b, input, owner);
    }
}

// @address 0x638918 ??0phys_bounding_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_bounding_agent::phys_bounding_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
    : phys_agent(pair_contact_manager)
{
    const phys_bounding_shape *agent_bounding_shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy new_proxy_a(&agent_body_a);
    new_proxy_a.set_proxy_shape(agent_bounding_shape->get_bounding_volume_shape());
    m_bounding_agent = input.m_agent_dispatcher->create_collision_agent(new_proxy_a, agent_body_b, input, pair_contact_manager);
    m_child_agent = 0;
}

// 0x6389f0
// @address 0x6389f0 ?create_bounding_shape_agent@phys_bounding_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bounding_agent::create_bounding_shape_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_bounding_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}

// 0x638ce8
// @address 0x638ce8 ?static_sweep_cast@phys_bounding_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_bounding_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy bounding_body_proxy(&agent_body_a);
    bounding_body_proxy.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_shape_type shape_type_a = bounding_body_proxy.get_shape()->get_type_id();
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    phys_dispatcher::agent_funcs::sweep_cast_fn linear_cast_fn = input.m_agent_dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
    phys_simple_closest_collector collector;
    linear_cast_fn(bounding_body_proxy, agent_body_b, input, (phys_hit_collector &)collector, (phys_hit_collector *)&collector);
    if (collector.has_hit()) {
        bounding_body_proxy.set_proxy_shape(shape->get_child_shape());
        shape_type_a = bounding_body_proxy.get_shape()->get_type_id();
        phys_dispatcher::agent_funcs::sweep_cast_fn child_cast_fn = input.m_agent_dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
        child_cast_fn(bounding_body_proxy, agent_body_b, input, cast_hit_collector, start_collector);
    }
}

// 0x638f30
// @address 0x638f30 ?static_closest_points@phys_bounding_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_bounding_agent::static_closest_points(const phys_collision_proxy &a, const phys_collision_proxy &b,
    const phys_collision_input &in, phys_hit_collector &c)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(a.get_shape());
    phys_collision_proxy bounding_proxy(&a);
    bounding_proxy.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_shape_type tb = b.get_shape()->get_type_id();
    phys_shape_type ta = bounding_proxy.get_shape()->get_type_id();
    phys_dispatcher::agent_funcs::penetrations_fn f = in.m_agent_dispatcher->get_penetrations_fn(ta, tb);
    phys_flag_proxy_pair_collector ray_collector;
    f(bounding_proxy, b, in, ray_collector);
    if (ray_collector.has_hit()) {
        bounding_proxy.set_proxy_shape(shape->get_child_shape());
        ta = bounding_proxy.get_shape()->get_type_id();
        phys_dispatcher::agent_funcs::closest_points_fn closest = in.m_agent_dispatcher->get_closest_points_fn(ta, tb);
        closest(bounding_proxy, b, in, c);
        if (ta == PHYS_SHAPE_VOLUME_CALLBACK)
            c.add_volume_callback_shape(bounding_proxy);
    }
}

// 0x6390d0
// @address 0x6390d0 ?create_shape_bounding_agent@phys_bounding_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bounding_agent::create_shape_bounding_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent<phys_bounding_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
}

// 0x639140

void phys_bounding_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_bounding_agent::create_shape_bounding_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, (phys_shape_type)-1, PHYS_SHAPE_BOUNDING);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_bounding_agent::create_bounding_shape_agent;
  backward.m_penetrations_fn = phys_bounding_agent::static_penetrations;
  backward.m_closest_points_fn = phys_bounding_agent::static_closest_points;
  backward.m_linear_cast_func = phys_bounding_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, PHYS_SHAPE_BOUNDING, (phys_shape_type)-1);
}

// 0x639210
// @address 0x639210 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_bounding_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

// 0x639260
// @address 0x639260 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_bounding_agent::static_closest_points(agent_body_b, agent_body_a, input, (phys_hit_collector &)flipped_collector);
}

// 0x6392b0
// @address 0x6392b0 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, cast_hit_collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bounding_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector,
            (phys_hit_collector *)&flipped_start_collector);
    } else {
        phys_bounding_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector, 0);
    }
}

// 0x639398
// @address 0x639398 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_bounding_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

// 0x639410
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_bounding_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

// 0x639420
// @address 0x639420 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bounding_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bounding_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_bounding_agent::get_closest_points(agent_body_b, agent_body_a, input, (phys_hit_collector &)flipped_collector);
}

// 0x639470
// @address 0x639470 ?sweep_cast@?$phys_symmetric_agent@Vphys_bounding_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template <>
void phys_symmetric_agent<phys_bounding_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, cast_hit_collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bounding_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector,
            (phys_hit_collector *)&flipped_start_collector);
    } else {
        phys_bounding_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector, 0);
    }
}

// 0x639050
// @address 0x639050 ?static_penetrations@phys_bounding_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_bounding_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy bounding_body_proxy(&agent_body_a);
    bounding_body_proxy.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_shape_type shape_type_a = bounding_body_proxy.get_shape()->get_type_id();
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    const phys_dispatcher *d = input.m_agent_dispatcher;
    phys_dispatcher::agent_funcs::penetrations_fn f = d->get_penetrations_fn(shape_type_a, shape_type_b);
    f(bounding_body_proxy, agent_body_b, input, collector);
}

// @address 0x638a48 ?process_collision@phys_bounding_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_bounding_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy new_proxy_a(&agent_body_a);
    new_proxy_a.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_flag_proxy_pair_collector union_find_checker;
    m_bounding_agent->get_penetrations(new_proxy_a, agent_body_b, input, union_find_checker);
    if (union_find_checker.has_hit()) {
        new_proxy_a.set_proxy_shape(shape->get_child_shape());
        if (m_child_agent == 0) {
            m_child_agent = input.m_agent_dispatcher->create_collision_agent(new_proxy_a, agent_body_b, input, m_contact_manager);
        }
        m_child_agent->process_collision(new_proxy_a, agent_body_b, input, result);
    } else {
        if (m_child_agent) {
            phys_constraint_owner *owner = result.m_constraint_owner.val();
            m_child_agent->cleanup(*owner);
            m_child_agent = 0;
        }
    }
}

// 0x638ba0
// @address 0x638ba0 ?sweep_cast@phys_bounding_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_bounding_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy new_proxy_a(&agent_body_a);
    new_proxy_a.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_simple_closest_collector union_find_checker;
    m_bounding_agent->sweep_cast(new_proxy_a, agent_body_b, input, (phys_hit_collector &)union_find_checker, (phys_hit_collector *)&union_find_checker);
    if (union_find_checker.has_hit()) {
        new_proxy_a.set_proxy_shape(shape->get_child_shape());
        if (m_child_agent == 0) {
            m_child_agent = input.m_agent_dispatcher->create_collision_agent(new_proxy_a, agent_body_b, input, m_contact_manager);
        }
        m_child_agent->sweep_cast(new_proxy_a, agent_body_b, input, cast_hit_collector, start_collector);
    }
}

// 0x638e08
// @address 0x638e08 ?get_closest_points@phys_bounding_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_bounding_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_bounding_shape *shape = static_cast<const phys_bounding_shape *>(agent_body_a.get_shape());
    phys_collision_proxy new_proxy_a(&agent_body_a);
    new_proxy_a.set_proxy_shape(shape->get_bounding_volume_shape());
    phys_flag_proxy_pair_collector union_find_checker;
    m_bounding_agent->get_penetrations(new_proxy_a, agent_body_b, input, union_find_checker);
    if (union_find_checker.has_hit()) {
        new_proxy_a.set_proxy_shape(shape->get_child_shape());
        if (m_child_agent == 0) {
            m_child_agent = input.m_agent_dispatcher->create_collision_agent(new_proxy_a, agent_body_b, input, m_contact_manager);
        }
        m_child_agent->get_closest_points(new_proxy_a, agent_body_b, input, collector);
    }
}
