// hkcollide/hkBvTreeStreamAgent.obj: from the records 0x1037f8 0x627bd8 0x627bf0 0x627c08 0x627c20 0x627c78 0x627c80 0x627c88 0x627d10 0x627db8 0x627e40 0x627e98 0x6283b8 0x6284e8 0x628558 0x628628 0x6286f8 0x628818 0x6288e8 0x628960 0x628a58 0x628a68 0x628a78 0x628ac8 0x628b18 0x628cb0
// @flags /TP /O2
// @address 0x627e98 ?process_collision@phys_bvh_stream_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
// @address 0x628558 ?register_agent@phys_bvh_stream_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x628628 ?register_multi_ray_agent@phys_bvh_stream_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x628818 ?register_convex_list_agent@phys_bvh_stream_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x6288e8 ??1phys_bvh_stream_agent@@MAA@XZ
// @address 0x628960 ??_Gphys_bvh_stream_agent@@MAAPAXI@Z
// @address 0x628960 ??_Ephys_bvh_stream_agent@@MAAPAXI@Z
// @address 0x628a58 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
// @address 0x628a68 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_algorithm_sort.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "physics/havok_memory.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_step_info.h"
#include "phys/phys_memory.h"
#include "phys/phys_batch_agent_visitor_input.h"

#include "phys/phys_bvh_agent.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_agent_machine_util.h"
#include "phys/phys_compound_shape_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_convex_list_filter.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_bvh_stream_agent.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_swept_transform_util.h"


// The vtables of the two symmetric wrappers.
// @address 0x1538 ??_7?$phys_symmetric_agent@Vphys_bvh_stream_agent@@@@6B@
// @address 0x15b50 ??_7?$phys_symmetric_agent@Vphys_compound_shape_agent@@@@6B@
// @address 0x6289c8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x628a10 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x628bc8 ?sweep_cast@?$phys_symmetric_agent@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z

// @address 0x627d10 ?gather_memory_stats@phys_bvh_stream_agent@@MBAXPAVphys_statistics_collector@@@Z
void phys_bvh_stream_agent::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("BvTreeAgt3", phys_statistics_collector::stat_category_runtime, this);
    const phys_batch_agent_track &track = m_agent_track;
    if ((track.m_track_sectors.m_capacity_and_flags & (int)0x80000000) == 0)
        collector->add_chunk_stats("SectorPtrs", phys_statistics_collector::stat_category_runtime, track.m_track_sectors.m_data,
            track.m_track_sectors.m_size << 2,
            track.m_track_sectors.m_capacity_and_flags << 2);
    phys_agent_machine_util::gather_batch_agent_stats(track, collector);
}

// Each register function adds a shape pair to the dispatcher: the forward entry for (a, b) and the backward entry
// for (b, a), each with the four agent functions and two flags.

void phys_bvh_stream_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_bvh_stream_agent::create_bvh_shape_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, PHYS_SHAPE_BVH, PHYS_SHAPE_CONVEX);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_bvh_stream_agent::create_shape_bounding_agent;
  backward.m_penetrations_fn = phys_bvh_agent::static_penetrations;
  backward.m_closest_points_fn = phys_bvh_agent::static_closest_points;
  backward.m_linear_cast_func = phys_bvh_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, PHYS_SHAPE_CONVEX, PHYS_SHAPE_BVH);
}

void phys_bvh_stream_agent::register_multi_ray_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_bvh_stream_agent::create_bvh_shape_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, PHYS_SHAPE_BVH, PHYS_SHAPE_MULTI_RAY);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_bvh_stream_agent::create_shape_bounding_agent;
  backward.m_penetrations_fn = phys_bvh_agent::static_penetrations;
  backward.m_closest_points_fn = phys_bvh_agent::static_closest_points;
  backward.m_linear_cast_func = phys_bvh_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, PHYS_SHAPE_MULTI_RAY, PHYS_SHAPE_BVH);
}

void phys_bvh_stream_agent::register_convex_list_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_bvh_stream_agent::dispatch_bvh_convex_list;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, PHYS_SHAPE_BVH, PHYS_SHAPE_CONVEX_LIST);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_bvh_stream_agent::dispatch_convex_list_bvh;
  backward.m_penetrations_fn = phys_bvh_agent::static_penetrations;
  backward.m_closest_points_fn = phys_bvh_agent::static_closest_points;
  backward.m_linear_cast_func = phys_bvh_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, PHYS_SHAPE_CONVEX_LIST, PHYS_SHAPE_BVH);
}

phys_bvh_stream_agent::~phys_bvh_stream_agent()
{
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_stream_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_bvh_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

// 0x628a58
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_stream_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_bvh_stream_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
template <>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_compound_shape_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
// @address 0x6283b8 ?dispatch_convex_list_bvh@phys_bvh_stream_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bvh_stream_agent::dispatch_convex_list_bvh(
    const phys_collision_proxy &body_a, const phys_collision_proxy &body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry;
    if (pair_contact_manager != 0) {
        unsigned int type = input.m_convex_list_filter->get_convex_list_mode(body_a, body_b, input);
        if (type >= 1) {
            if (type != 1) {
                if (type >= 3) {
                    return 0;
                }
            } else {
                pair_agent_entry = (phys_agent *)phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_compound_shape_agent), PHYS_MEM_AGENT);
                pair_agent_entry->m_mem_size_and_flags = sizeof(phys_compound_shape_agent);
                return (phys_agent *)new (pair_agent_entry) phys_compound_shape_agent(body_a, body_b, input, pair_contact_manager);
            }
        }
        pair_agent_entry = (phys_agent *)phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_bvh_stream_agent), PHYS_MEM_AGENT);
        pair_agent_entry->m_mem_size_and_flags = sizeof(phys_bvh_stream_agent);
        return (phys_agent *)new (pair_agent_entry) phys_bvh_stream_agent(body_a, body_b, input, pair_contact_manager);
    } else {
        pair_agent_entry = (phys_agent *)phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_symmetric_agent<phys_bvh_stream_agent>), PHYS_MEM_AGENT);
        pair_agent_entry->m_mem_size_and_flags = sizeof(phys_symmetric_agent<phys_bvh_stream_agent>);
        new (pair_agent_entry) phys_symmetric_agent<phys_bvh_stream_agent>(body_a, body_b, input, 0);
        return pair_agent_entry;
    }
}
// @address 0x6286f8 ?dispatch_bvh_convex_list@phys_bvh_stream_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bvh_stream_agent::dispatch_bvh_convex_list(const phys_collision_proxy &a, const phys_collision_proxy &b,
    const phys_collision_input &in, phys_contact_manager *pair_contact_manager)
{
    phys_agent *o;
    unsigned int nb;
    if (pair_contact_manager != 0) {
        const phys_convex_list_filter *f = in.m_convex_list_filter;
        unsigned int t = f->get_convex_list_mode(b, a, in);
        switch (t) { case 1: { nb = sizeof(phys_symmetric_agent<phys_compound_shape_agent>); phys_thread_memory *thread_mem = phys_thread_memory::get_instance_ptr();
            o = (phys_agent *)thread_mem->allocate_chunk(nb, PHYS_MEM_AGENT); o->m_mem_size_and_flags = (unsigned short)nb;
            new (o) phys_symmetric_agent<phys_compound_shape_agent>(a, b, in, pair_contact_manager);
            return o; } case 0: case 2: break; default: return 0; }
        nb = sizeof(phys_symmetric_agent<phys_bvh_stream_agent>); phys_thread_memory *m = phys_thread_memory::get_instance_ptr();
        o = (phys_agent *)(m)->allocate_chunk(nb, PHYS_MEM_AGENT); o->m_mem_size_and_flags = (unsigned short)nb;
        new (o) phys_symmetric_agent<phys_bvh_stream_agent>(a, b, in, pair_contact_manager);
    } else {
        nb = sizeof(phys_symmetric_agent<phys_bvh_stream_agent>); phys_thread_memory *m = phys_thread_memory::get_instance_ptr();
        o = (phys_agent *)(m)->allocate_chunk(nb, PHYS_MEM_AGENT); o->m_mem_size_and_flags = (unsigned short)nb;
        new (o) phys_symmetric_agent<phys_bvh_stream_agent>(a, b, in, 0);
    }
    return o;
}



// @address 0x1037f8 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_stream_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi_time = output.m_collision_toi.m_time;
    phys_bvh_stream_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi_time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

// @address 0x627db8 ??0phys_bvh_stream_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_bvh_stream_agent::phys_bvh_stream_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
    : phys_agent(pair_contact_manager)
{
    m_agent_dispatcher = input.m_agent_dispatcher;
    m_aabb_min.set_zero4();
    m_aabb_max.set_zero4();
    phys_batch_agent_create(&m_agent_track);
}
// @address 0x627bd8 ?get_penetrations@phys_bvh_stream_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_bvh_stream_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_bvh_agent::static_penetrations(agent_body_a, agent_body_b, input, collector);
}
// @address 0x627bf0 ?get_closest_points@phys_bvh_stream_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_bvh_stream_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_bvh_agent::static_closest_points(agent_body_a, agent_body_b, input, collector);
}
// @address 0x627c08 ?sweep_cast@phys_bvh_stream_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_bvh_stream_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_bvh_agent::static_sweep_cast(agent_body_a, agent_body_b, input, cast_hit_collector, start_collector);
}
// @address 0x627c20 ?cleanup@phys_bvh_stream_agent@@UAAXAAVphys_constraint_owner@@@Z
void phys_bvh_stream_agent::cleanup(phys_constraint_owner &owner)
{
    phys_batch_agent_destroy(m_agent_track, m_agent_dispatcher, m_contact_manager, owner);
    delete this;
}
// @address 0x627c78 ?invalidate_sweep_info@phys_bvh_stream_agent@@UAAXAAUphys_collision_input@@@Z
void phys_bvh_stream_agent::invalidate_sweep_info(phys_collision_input &input)
{
    phys_batch_agent_invalidate_sweep(&m_agent_track, &input);
}
// @address 0x627c80 ?warp_time@phys_bvh_stream_agent@@UAAXMMAAUphys_collision_input@@@Z
void phys_bvh_stream_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    phys_batch_agent_warp_time(&m_agent_track, agent_old_time, agent_new_time, &input);
}
// @address 0x627c88 ?update_compound_filter@phys_bvh_stream_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_bvh_stream_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    m_aabb_min.set_zero4();
    m_aabb_max.set_zero4();
    const phys_shape *shape = agent_body_b.get_shape();
    const phys_shape_container *container = shape->get_shape_container();
    phys_batch_agent_visitor_input visitor;
    visitor.m_body_a = &agent_body_a;
    visitor.m_compound_proxy_b = &agent_body_b;
    visitor.m_input = &input;
    visitor.m_contact_manager = m_contact_manager;
    visitor.m_constraint_owner = &owner;
    visitor.m_container_shape_b = container;
    phys_batch_agent_update_compound_filter(m_agent_track, visitor);
}
// @address 0x627e40 ?create_shape_bounding_agent@phys_bvh_stream_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bvh_stream_agent::create_shape_bounding_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    phys_agent *pair_agent_entry = (phys_agent *)phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_bvh_stream_agent), PHYS_MEM_AGENT);
    pair_agent_entry->m_mem_size_and_flags = sizeof(phys_bvh_stream_agent);
    phys_agent *result = (phys_agent *)new (pair_agent_entry) phys_bvh_stream_agent(agent_body_a, agent_body_b, input, agent_contact_manager);
    return result;
}
// @address 0x6284e8 ?create_bvh_shape_agent@phys_bvh_stream_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bvh_stream_agent::create_bvh_shape_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *agent_contact_manager)
{
    phys_agent *pair_agent_entry = (phys_agent *)phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_symmetric_agent<phys_bvh_stream_agent>), PHYS_MEM_AGENT);
    pair_agent_entry->m_mem_size_and_flags = sizeof(phys_symmetric_agent<phys_bvh_stream_agent>);
    new (pair_agent_entry) phys_symmetric_agent<phys_bvh_stream_agent>(agent_body_a, agent_body_b, input, agent_contact_manager);
    phys_agent *result = pair_agent_entry;
    return result;
}
// @address 0x628a78 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_compound_shape_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x628ac8 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_compound_shape_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x628b18 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi_time = output.m_collision_toi.m_time;
    phys_compound_shape_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi_time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

// @address 0x628cb0 ?sweep_cast@?$phys_symmetric_agent@Vphys_compound_shape_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template <>
void phys_symmetric_agent<phys_compound_shape_agent>::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_compound_shape_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_compound_shape_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

// hkBvTreeStreamAgent::processCollision inlines hkSweptTransformUtil::calcTimInfo and the bounding-volume tree query
// of hkBvTreeAgent (calcAabbAndQueryTree), which is written out here.
static __forceinline phys_result phys_bvh_stream_agent_query_tree(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_transform &b_from_a,
                                                                      const phys_vec4 &lin_sweep_info, const phys_process_collision_input &input,
                                                                      phys_aabb *cached_aabb, phys_array<phys_shape_key> &out_hit_list)
{
    phys_aabb aabb;
    {
        const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
        const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();

        phys_vec4 sweep_info;
        sweep_info.set_rotated_inverse_dir(agent_body_b.get_transform().get_rotation(), lin_sweep_info);

        phys_vec4 aabb_extents;
        if (input.m_quality_info->m_use_ccd) {
            const float second_order_err_a = (motion_a_state->m_delta_angle(3) + motion_b_state->m_delta_angle(3)) * motion_a_state->m_object_radius;
            const float second_order_err_b = motion_b_state->m_delta_angle(3) * motion_b_state->m_delta_angle(3) * motion_b_state->m_object_radius;
            const float check_epsilon = input.m_box_tolerance * 0.5f + (second_order_err_a + second_order_err_b);
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);

            phys_vec4 mass_center_a_in_b;
            {
                phys_vec4 radius_vec4;
                radius_vec4.set_all3(motion_a_state->m_object_radius + input.m_box_tolerance * 0.5f + second_order_err_b);
                mass_center_a_in_b.set_transformed_inverse_pos(agent_body_b.get_transform(), motion_a_state->get_swept_transform().m_center_of_mass1);
                phys_vec4 max_reach; max_reach.set_add4(mass_center_a_in_b, radius_vec4);
                phys_vec4 min_radius_corner; min_radius_corner.set_sub4(mass_center_a_in_b, radius_vec4);
                aabb.m_min_limit.set_max4(aabb.m_min_limit, min_radius_corner);
                aabb.m_max_limit.set_min4(aabb.m_max_limit, max_reach);
            }

            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);

            {
                if (motion_b_state->m_delta_angle(3) > 0.0f) {
                    phys_vec4 rel_pos; rel_pos.set_sub4(mass_center_a_in_b, motion_b_state->get_swept_transform().m_center_of_mass_local);
                    phys_vec4 offset_out; offset_out.set_cross(rel_pos, motion_b_state->m_delta_angle);
                    sweep_info.add_scaled4(input.m_step_info.m_delta_time * motion_b_state->get_swept_transform().get_inv_delta_time(), offset_out);
                }

                phys_vec4 zero; zero.set_zero4();
                phys_vec4 min_path; min_path.set_min4(zero, sweep_info);
                phys_vec4 bvh_max_path; bvh_max_path.set_max4(zero, sweep_info);
                aabb.m_min_limit.set_add4(aabb.m_min_limit, min_path);
                aabb.m_max_limit.set_add4(aabb.m_max_limit, bvh_max_path);
            }
        } else {
            const float check_epsilon = input.m_box_tolerance * 0.5f;
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);
            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        }

        if (cached_aabb) {
            if (cached_aabb->contains(aabb)) {
                return PHYS_FAILURE;
            }

            phys_vec4 zero; zero.set_zero4();
            phys_vec4 min_path; min_path.set_min4(zero, sweep_info);
            phys_vec4 bvh_max_path; bvh_max_path.set_max4(zero, sweep_info);

            phys_vec4 expand_vec; expand_vec.set_all3(input.m_box_tolerance * 0.5f);
            aabb.m_min_limit.set_sub4(aabb.m_min_limit, expand_vec);
            aabb.m_max_limit.set_add4(aabb.m_max_limit, expand_vec);

            const float max_expand = 0.4f;
            const float frames_look_ahead = -2.0f;

            phys_vec4 min_extent_path; min_extent_path.set_mul4(frames_look_ahead, bvh_max_path);
            phys_vec4 max_extent_path; max_extent_path.set_mul4(frames_look_ahead, min_path);

            phys_vec4 max_expand_vec; max_expand_vec.set_mul4(max_expand, aabb_extents);
            max_extent_path.set_min4(max_extent_path, max_expand_vec);
            phys_vec4 min_expand4; min_expand4.set_neg4(max_expand_vec);
            min_extent_path.set_max4(min_extent_path, min_expand4);

            aabb.m_min_limit.set_add4(aabb.m_min_limit, min_extent_path);
            aabb.m_max_limit.set_add4(aabb.m_max_limit, max_extent_path);
            *cached_aabb = aabb;
        }
    }

    const phys_bvh_shape *bvh_shape_b = static_cast<const phys_bvh_shape *>(agent_body_b.get_shape());
    bvh_shape_b->query_aabb(aabb, out_hit_list);
    return PHYS_SUCCESS;
}

// @address 0x627e98
void phys_bvh_stream_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    phys_narrow_agent_process_input narrow_input;
    {
        narrow_input.m_body_a = &agent_body_a;
        narrow_input.m_body_b = &agent_body_b;
        narrow_input.m_contact_manager = m_contact_manager;
        narrow_input.m_input = &input;

        const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
        const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();
        phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, input.m_step_info.m_delta_time, narrow_input.m_lin_sweep_info);
        narrow_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());
    }

    phys_inline_array<phys_shape_key, 128> hit_key_list;
    hit_key_list.push_back_unchecked(0xffffffff);

    phys_result prepare_ok;
    {
        phys_transform b_from_a;
        b_from_a.set_to_inverse(narrow_input.m_a_to_b);
        prepare_ok = phys_bvh_stream_agent_query_tree(agent_body_a, agent_body_b, b_from_a, narrow_input.m_lin_sweep_info, input,
            (phys_aabb *)&m_aabb_min, hit_key_list);
    }

    if (prepare_ok == PHYS_SUCCESS) {
        int bvh_hit_count = hit_key_list.getSize();
        int sector_count = m_agent_track.m_track_sectors.m_size;
        if (((bvh_hit_count / 4) + 1 - sector_count) * 512 > havok_get_available_memory()) {
            phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
        } else {
            phys_algorithm::quick_sort(hit_key_list.begin(), hit_key_list.getSize());

            const phys_bvh_shape *bvh_shape_b = static_cast<const phys_bvh_shape *>(agent_body_b.get_shape());
            const phys_shape_container *shape_container = bvh_shape_b->get_shape_container();
            phys_batch_agent_process(m_agent_track, 0, narrow_input, shape_container, hit_key_list.begin(), hit_key_list.getSize(), output);
        }
    } else {
        const phys_bvh_shape *bvh_shape_b = static_cast<const phys_bvh_shape *>(agent_body_b.get_shape());
        const phys_shape_container *shape_container = bvh_shape_b->get_shape_container();
        phys_batch_agent_process(m_agent_track, 0, narrow_input, shape_container, 0, 0, output);
    }
}

