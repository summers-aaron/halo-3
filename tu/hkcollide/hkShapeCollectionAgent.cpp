// hkcollide/hkShapeCollectionAgent.obj: from the records 0x636090 0x636110 0x636178 0x636200 0x6362c0 0x636380 0x636448 0x636510 0x636640 0x636778 0x6368b8 0x636b30 0x636d28 0x636d80 0x636df0 0x636ec0
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
// @address 0x636df0 ?register_agent@phys_compound_shape_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x636090 ?cleanup@phys_compound_shape_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x636110 ?invalidate_sweep_info@phys_compound_shape_agent@@UAAXAAUphys_collision_input@@@Z
// @address 0x636178 ?warp_time@phys_compound_shape_agent@@UAAXMMAAUphys_collision_input@@@Z
// @address 0x636200 ?process_collision@phys_compound_shape_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x6362c0 ?get_closest_points@phys_compound_shape_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x636380 ?sweep_cast@phys_compound_shape_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x636448 ?get_penetrations@phys_compound_shape_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x636510 ?static_closest_points@phys_compound_shape_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x636640 ?static_sweep_cast@phys_compound_shape_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x636778 ?static_penetrations@phys_compound_shape_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x636d28 ?create_list_a_agent@phys_compound_shape_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x636ec0 ??_G?$phys_symmetric_agent@Vphys_compound_shape_agent@@@@UAAPAXI@Z

#include "phys/phys_dispatcher.h"

typedef unsigned int phys_shape_key;

#include "phys/phys_collision_input.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_statistics_collector.h"



extern "C" {
}


#include "phys/phys_compound_shape_agent.h"

#include "phys/phys_symmetric_agent.h"




void phys_compound_shape_agent::cleanup(phys_constraint_owner &owner)
{
    for (int i = 0; i < m_key_agents.getSize(); ++i) {
        phys_agent *pair_agent_entry = m_key_agents[i].m_key_agent;
        pair_agent_entry->cleanup(owner);
    }
    delete this;
}

void phys_compound_shape_agent::invalidate_sweep_info(phys_collision_input &input)
{
    for (int i = 0; i < m_key_agents.getSize(); ++i) {
        phys_agent *pair_agent_entry = m_key_agents[i].m_key_agent;
        pair_agent_entry->invalidate_sweep_info(input);
    }
}

void phys_compound_shape_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    for (int i = 0; i < m_key_agents.getSize(); ++i) {
        phys_agent *pair_agent_entry = m_key_agents[i].m_key_agent;
        pair_agent_entry->warp_time(agent_old_time, agent_new_time, input);
    }
}

void phys_compound_shape_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    KeyAgentPair *key_agent_pair = m_key_agents.begin();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (int i = m_key_agents.getSize() - 1; i >= 0; --i) {
        new_operand_a.set_proxy_shape(shape_container->get_child_shape(key_agent_pair->m_key, shape_scratch_buffer), key_agent_pair->m_key);
        key_agent_pair->m_key_agent->process_collision(new_operand_a, agent_body_b, input, output);
        ++key_agent_pair;
    }
}

void phys_compound_shape_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    KeyAgentPair *key_agent_pair = m_key_agents.begin();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (int i = m_key_agents.getSize() - 1; i >= 0; --i) {
        new_operand_a.set_proxy_shape(shape_container->get_child_shape(key_agent_pair->m_key, shape_scratch_buffer), key_agent_pair->m_key);
        key_agent_pair->m_key_agent->get_closest_points(new_operand_a, agent_body_b, input, collector);
        ++key_agent_pair;
    }
}

void phys_compound_shape_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    KeyAgentPair *key_agent_pair = m_key_agents.begin();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (int i = m_key_agents.getSize() - 1; i >= 0; --i) {
        new_operand_a.set_proxy_shape(shape_container->get_child_shape(key_agent_pair->m_key, shape_scratch_buffer), key_agent_pair->m_key);
        key_agent_pair->m_key_agent->sweep_cast(new_operand_a, agent_body_b, input, collector, start_collector);
        ++key_agent_pair;
    }
}

void phys_compound_shape_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    KeyAgentPair *key_agent_pair = m_key_agents.begin();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (int i = m_key_agents.getSize() - 1; i >= 0; --i) {
        new_operand_a.set_proxy_shape(shape_container->get_child_shape(key_agent_pair->m_key, shape_scratch_buffer), key_agent_pair->m_key);
        key_agent_pair->m_key_agent->get_penetrations(new_operand_a, agent_body_b, input, collector);
        if (collector.get_early_out()) return;
        ++key_agent_pair;
    }
}

void phys_compound_shape_agent::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (phys_shape_key key = shape_container->get_first_key(); key != (phys_shape_key)-1; key = shape_container->get_next_key(key)) {
        if (input.m_compound_filter->allows_collision(input, agent_body_b, agent_body_a, *shape_container, key)) {
            new_operand_a.set_proxy_shape(shape_container->get_child_shape(key, shape_scratch_buffer), key);
            phys_shape_type shape_type_a = new_operand_a.get_shape()->get_type_id();
            phys_dispatcher::agent_funcs::closest_points_fn func = input.m_agent_dispatcher->get_closest_points_fn(shape_type_a, shape_type_b);
            func(new_operand_a, agent_body_b, input, collector);
        }
    }
}

void phys_compound_shape_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (phys_shape_key key = shape_container->get_first_key(); key != (phys_shape_key)-1; key = shape_container->get_next_key(key)) {
        if (input.m_compound_filter->allows_collision(input, agent_body_b, agent_body_a, *shape_container, key)) {
            new_operand_a.set_proxy_shape(shape_container->get_child_shape(key, shape_scratch_buffer), key);
            phys_shape_type shape_type_a = new_operand_a.get_shape()->get_type_id();
            phys_dispatcher::agent_funcs::sweep_cast_fn func = input.m_agent_dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
            func(new_operand_a, agent_body_b, input, collector, start_collector);
        }
    }
}

void phys_compound_shape_agent::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    phys_collision_proxy new_operand_a(&agent_body_a);
    phys_shape_type shape_type_b = agent_body_b.get_shape()->get_type_id();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    for (phys_shape_key key = shape_container->get_first_key(); key != (phys_shape_key)-1; key = shape_container->get_next_key(key)) {
        if (input.m_compound_filter->allows_collision(input, agent_body_b, agent_body_a, *shape_container, key)) {
            new_operand_a.set_proxy_shape(shape_container->get_child_shape(key, shape_scratch_buffer), key);
            phys_shape_type shape_type_a = new_operand_a.get_shape()->get_type_id();
            phys_dispatcher::agent_funcs::penetrations_fn func = input.m_agent_dispatcher->get_penetrations_fn(shape_type_a, shape_type_b);
            func(new_operand_a, agent_body_b, input, collector);
            if (collector.get_early_out()) return;
        }
    }
}

// @address 0x6368b8 ?update_compound_filter@phys_compound_shape_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_compound_shape_agent::update_compound_filter(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_collision_proxy new_operand_a(&agent_body_a);
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    int multi_sphere_child_count = shape_container->get_child_shape_count();
    phys_shape_key key = shape_container->get_first_key();
    if (multi_sphere_child_count > 0) {
    unsigned int i = (unsigned int)multi_sphere_child_count;
    do {
        int index = get_agent_index(key);
        if (input.m_compound_filter->allows_collision(input, agent_body_b, agent_body_a, *shape_container, key)) {
            new_operand_a.set_proxy_shape(shape_container->get_child_shape(key, shape_scratch_buffer), key);
            if (index == -1) {
                KeyAgentPair &new_pair = m_key_agents.expand_one();
                new_pair.m_key = key;
                new_pair.m_key_agent = input.m_agent_dispatcher->create_collision_agent(new_operand_a, agent_body_b, input, m_contact_manager);
            } else {
                m_key_agents[index].m_key_agent->update_compound_filter(new_operand_a, agent_body_b, input, owner);
            }
        } else if (index != -1) {
            m_key_agents[index].m_key_agent->cleanup(owner);
            m_key_agents.remove_at(index);
        }
        key = shape_container->get_next_key(key);
    } while (--i != 0);
    }
}


// @address 0x636b30 ??0phys_compound_shape_agent@@QAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_compound_shape_agent::phys_compound_shape_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
    : phys_agent(pair_contact_manager)
{
    phys_collision_proxy new_operand_a(&agent_body_a);
    const phys_shape_container *shape_container = agent_body_a.get_shape()->get_shape_container();
    int multi_sphere_child_count = shape_container->get_child_shape_count();
    m_key_agents.reserve(multi_sphere_child_count);
    __declspec(align(16)) phys_shape_buffer shape_scratch_buffer;
    phys_shape_key key = shape_container->get_first_key();
    for (int i = 0; i < multi_sphere_child_count; ++i) {
        new_operand_a.set_proxy_shape(shape_container->get_child_shape(key, shape_scratch_buffer), key);
        if (input.m_compound_filter->allows_collision(input, agent_body_b, agent_body_a, *shape_container, key)) {
            KeyAgentPair &ap = *m_key_agents.expand_by_unchecked(1);
            ap.m_key_agent = input.m_agent_dispatcher->create_collision_agent(new_operand_a, agent_body_b, input, pair_contact_manager);
            ap.m_key = key;
        }
        key = shape_container->get_next_key(key);
    }
}

phys_agent *phys_compound_shape_agent::create_list_a_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_compound_shape_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}


// @address 0x636d80 ?create_list_b_agent@phys_compound_shape_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_compound_shape_agent::create_list_b_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_symmetric_agent<phys_compound_shape_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

// Registers collection collision agents in both shape orders.
void phys_compound_shape_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs forward;

    forward.m_create_fn = phys_compound_shape_agent::create_list_b_agent;
    forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_penetrations;
    forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_closest_points;
    forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast;
    forward.m_flipped = true;
    forward.m_predictive = true;
    dispatcher->register_collision_agent(forward, (phys_shape_type)-1, (phys_shape_type)2);

    phys_dispatcher::agent_funcs backward;
    backward.m_create_fn = phys_compound_shape_agent::create_list_a_agent;
    backward.m_penetrations_fn = phys_compound_shape_agent::static_penetrations;
    backward.m_closest_points_fn = phys_compound_shape_agent::static_closest_points;
    backward.m_linear_cast_func = phys_compound_shape_agent::static_sweep_cast;
    backward.m_flipped = false;
    backward.m_predictive = true;
    dispatcher->register_collision_agent(backward, (phys_shape_type)2, (phys_shape_type)-1);
}
