// hkcollide/hkMultiSphereAgent.obj: from the records 0x637650 0x6376d0 0x637808 0x637918 0x637a40 0x637b50 0x637c80 0x637d98 0x637ec8 0x6380b8 0x638110 0x638180 0x638250 0x6382a0 0x6382f0 0x6383d8 0x638428 0x638478 0x638528
// @flags /TP /O2 /Oc
// @address 0x637650 ?cleanup@phys_multi_sphere_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x637ec8 ??0phys_multi_sphere_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x6380b8 ?create_list_a_agent@phys_multi_sphere_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x638110 ?create_list_b_agent@phys_multi_sphere_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x638250 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x6383d8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x638478 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include "phys/phys_math.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
#include "phys/phys_dispatcher.h"
#include <vectorintrinsics.h>
#include "phys/phys_aabb_types.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collide.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_sphere.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_multi_sphere_agent.h"
#include "phys/phys_symmetric_agent.h"

// @address 0x638180 ?register_agent@phys_multi_sphere_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x6376d0 ?process_collision@phys_multi_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x637808 ?get_closest_points@phys_multi_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x637918 ?static_closest_points@phys_multi_sphere_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x637a40 ?sweep_cast@phys_multi_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x637b50 ?static_sweep_cast@phys_multi_sphere_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x637c80 ?get_penetrations@phys_multi_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x637d98 ?static_penetrations@phys_multi_sphere_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x6382a0 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x6382f0 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x638428 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x638528 ?sweep_cast@?$phys_symmetric_agent@Vphys_multi_sphere_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x635a90 ??0phys_motion_state@@QAA@ABV0@@Z






void phys_multi_sphere_agent::cleanup(phys_constraint_owner &owner)
{
    for (int index = 0; index < m_key_agents.getSize(); ++index) {
        phys_agent *pair_agent_entry = m_key_agents[index].m_key_agent;
        pair_agent_entry->cleanup(owner);
    }
    delete this;
}


// 0x6376d0
void phys_multi_sphere_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    KeyAgentPair *pair_agent_entry = m_key_agents.begin();
    phys_vec4 offset;
    for (int index = m_key_agents.getSize() - 1; index >= 0; --index) {
        const phys_vec4 &center = shape->get_spheres()[pair_agent_entry->m_key];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        motion.get_swept_transform().m_center_of_mass0.set_add4(agent_body_a.get_motion_state()->get_swept_transform().m_center_of_mass0, offset);
        motion.get_swept_transform().m_center_of_mass1.set_add4(agent_body_a.get_motion_state()->get_swept_transform().m_center_of_mass1, offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        pair_agent_entry->m_key_agent->process_collision(sphere_proxy, agent_body_b, input, output);
        ++pair_agent_entry;
    }
}

// 0x637808
void phys_multi_sphere_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    KeyAgentPair *pair_agent_entry = m_key_agents.begin();
    phys_vec4 offset;
    for (int index = m_key_agents.getSize() - 1; index >= 0; --index) {
        const phys_vec4 &center = shape->get_spheres()[pair_agent_entry->m_key];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        pair_agent_entry->m_key_agent->get_closest_points(sphere_proxy, agent_body_b, input, collector);
        ++pair_agent_entry;
    }
}

// 0x637918
void phys_multi_sphere_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    phys_vec4 offset;
    for (int index = 0; index < shape->get_sphere_count(); ++index) {
        const phys_vec4 &center = shape->get_spheres()[index];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        phys_shape_type shape_type_a = sphere.get_type_id();
        phys_dispatcher::agent_funcs::closest_points_fn query = input.m_agent_dispatcher->get_closest_points_fn(shape_type_a, shape_type_b);
        query(sphere_proxy, agent_body_b, input, collector);
    }
}

// 0x637a40
void phys_multi_sphere_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    KeyAgentPair *pair_agent_entry = m_key_agents.begin();
    phys_vec4 offset;
    for (int index = m_key_agents.getSize() - 1; index >= 0; --index) {
        const phys_vec4 &center = shape->get_spheres()[index];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        pair_agent_entry->m_key_agent->sweep_cast(sphere_proxy, agent_body_b, input, cast_hit_collector, start_collector);
    }
}

// 0x637b50
void phys_multi_sphere_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    phys_vec4 offset;
    for (int index = 0; index < shape->get_sphere_count(); ++index) {
        const phys_vec4 &center = shape->get_spheres()[index];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        phys_shape_type shape_type_a = sphere.get_type_id();
        phys_dispatcher::agent_funcs::sweep_cast_fn query = input.m_agent_dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
        query(sphere_proxy, agent_body_b, input, cast_hit_collector, start_collector);
    }
}

// 0x637c80
void phys_multi_sphere_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    KeyAgentPair *pair_agent_entry = m_key_agents.begin();
    phys_vec4 offset;
    for (int index = m_key_agents.getSize() - 1; index >= 0; --index) {
        const phys_vec4 &center = shape->get_spheres()[pair_agent_entry->m_key];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        pair_agent_entry->m_key_agent->get_penetrations(sphere_proxy, agent_body_b, input, collector);
        if (collector.get_early_out())
            break;
        ++pair_agent_entry;
    }
}

// 0x637d98
void phys_multi_sphere_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_multi_sphere_shape *shape = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    phys_motion_state motion(*agent_body_a.get_motion_state());
    phys_sphere_shape sphere(0.0f);
    phys_collision_proxy sphere_proxy(&agent_body_a, &motion);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    phys_vec4 offset;
    for (int index = 0; index < shape->get_sphere_count(); ++index) {
        const phys_vec4 &center = shape->get_spheres()[index];
        offset.set_rotated_dir(motion.get_transform().get_rotation(), center);
        motion.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset);
        sphere.set_shape_radius(center(3));
        sphere_proxy.set_proxy_shape(&sphere, index);
        phys_shape_type shape_type_a = sphere.get_type_id();
        phys_dispatcher::agent_funcs::penetrations_fn query = input.m_agent_dispatcher->get_penetrations_fn(shape_type_a, shape_type_b);
        query(sphere_proxy, agent_body_b, input, collector);
        if (collector.get_early_out())
            break;
    }
}


// @address 0x637ec8
phys_multi_sphere_agent::phys_multi_sphere_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
    : phys_agent(pair_contact_manager)
{
    const phys_multi_sphere_shape *multi_sphere_a = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());

    int multi_sphere_child_count = multi_sphere_a->get_sphere_count();
    m_key_agents.reserve(multi_sphere_child_count);

    phys_sphere_shape sphere_shape(0.0f);

    phys_motion_state ms = *agent_body_a.get_motion_state();
    phys_collision_proxy new_operand_a(&agent_body_a, &ms);

    const phys_vec4 *spheres = multi_sphere_a->get_spheres();
    for (int i = 0; i < multi_sphere_child_count; i++) {
        phys_vec4 offset_world;
        offset_world.set_rotated_dir(ms.get_transform().get_rotation(), spheres[0]);
        ms.get_transform().get_translation().set_add4(agent_body_a.get_transform().get_translation(), offset_world);
        ms.get_swept_transform().m_center_of_mass0.set_add4(agent_body_a.get_motion_state()->get_swept_transform().m_center_of_mass0, offset_world);
        ms.get_swept_transform().m_center_of_mass1.set_add4(agent_body_a.get_motion_state()->get_swept_transform().m_center_of_mass1, offset_world);

        sphere_shape.set_shape_radius(spheres[0](3));
        new_operand_a.set_proxy_shape(&sphere_shape, i);

        {
            KeyAgentPair &ap = *m_key_agents.expand_by_unchecked(1);
            ap.m_key_agent = input.m_agent_dispatcher->create_collision_agent(new_operand_a, agent_body_b, input, pair_contact_manager);
            ap.m_key = i;
        }
        spheres++;
    }
}

phys_agent *phys_multi_sphere_agent::create_list_a_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_multi_sphere_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

phys_agent *phys_multi_sphere_agent::create_list_b_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_symmetric_agent<phys_multi_sphere_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

void phys_multi_sphere_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_multi_sphere_agent::create_list_b_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, (phys_shape_type)-1, (phys_shape_type)16);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_multi_sphere_agent::create_list_a_agent;
  backward.m_penetrations_fn = phys_multi_sphere_agent::static_penetrations;
  backward.m_closest_points_fn = phys_multi_sphere_agent::static_closest_points;
  backward.m_linear_cast_func = phys_multi_sphere_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, (phys_shape_type)16, (phys_shape_type)-1);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_sphere_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}


template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_sphere_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flip_cast_collector(input.m_path, cast_hit_collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_multi_sphere_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_cast_collector, &flipped_start_collector);
    } else {
        phys_multi_sphere_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_cast_collector, 0);
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_sphere_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}


template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_sphere_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float toi_time = output.m_collision_toi.m_time;
    phys_multi_sphere_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    if (toi_time != output.m_collision_toi.m_time)
        output.m_collision_toi.flip();
}

// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_multi_sphere_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

template <>
void phys_symmetric_agent<phys_multi_sphere_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flip_cast_collector(input.m_path, cast_hit_collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_multi_sphere_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_cast_collector, &flipped_start_collector);
    } else {
        phys_multi_sphere_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_cast_collector, 0);
    }
}

