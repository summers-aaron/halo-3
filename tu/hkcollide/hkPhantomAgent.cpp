// hkcollide/hkPhantomAgent.obj: from the records 0x634cf0 0x634d80 0x634d88 0x634da0 0x634dc0 0x634dc8 0x634dd0 0x634dd8 0x634de0 0x634e60 0x634f90 0x635080
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
// @address 0x634cf0 ?cleanup@phys_volume_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x634d80 ?process_collision@phys_volume_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x634d88 ?get_penetrations@phys_volume_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x634da0 ?static_penetrations@phys_volume_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x634dc0 ?get_closest_points@phys_volume_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x634dc8 ?static_closest_points@phys_volume_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x634dd0 ?sweep_cast@phys_volume_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x634dd8 ?static_sweep_cast@phys_volume_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x634de0 ??0phys_volume_agent@@IAA@ABVphys_collision_proxy@@0PAVphys_contact_manager@@@Z
// @address 0x634e60 ?create_volume_agent@phys_volume_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x634f90 ?create_no_volume_agent@phys_volume_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x635080 ?register_agent@phys_volume_agent@@SAXPAVphys_dispatcher@@@Z
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"

#include "phys/phys_volume_callback_shape.h"
// The dispatcher's agent entry for one direction of a shape pair (20 bytes).
#include "phys/phys_dispatcher.h"
#include "phys/phys_volume_agent.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_shape_type_enum.h"
#include "phys/phys_math.h"

void phys_volume_agent::cleanup(phys_constraint_owner &constraint_owner_ref)
{
    if (m_shape_type_a == 29) {
        m_volume_shape_a->volume_leave_event(m_collider_a, m_collider_b);
    }
    if (m_shape_type_b == 29) {
        m_volume_shape_b->volume_leave_event(m_collider_b, m_collider_a);
    }
    delete this;
}
void phys_volume_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
}
void phys_volume_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    collector.add_proxy_pair(agent_body_a, agent_body_b);
}
void phys_volume_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    collector.add_proxy_pair(agent_body_a, agent_body_b);
}
void phys_volume_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
}
void phys_volume_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
}
void phys_volume_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
}
void phys_volume_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
}
phys_volume_agent::phys_volume_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    phys_contact_manager *agent_contact_manager)
    : phys_agent(agent_contact_manager)
{
    m_collider_a = agent_body_a.get_root_collider();
    m_collider_b = agent_body_b.get_root_collider();
    m_shape_type_a = agent_body_a.get_shape()->get_type_id();
    m_shape_type_b = agent_body_b.get_shape()->get_type_id();
}
phys_agent *phys_volume_agent::create_volume_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    phys_volume_agent *pair_agent_entry = new phys_volume_agent(agent_body_a, agent_body_b, agent_contact_manager);
    if (pair_agent_entry->m_shape_type_a == 29) {
        phys_volume_callback_shape *phantom = (phys_volume_callback_shape *)agent_body_a.get_shape();
        phantom->volume_enter_event(agent_body_a.get_root_collider(), agent_body_b.get_root_collider(), input);
        pair_agent_entry->m_volume_shape_a = phantom;
    }
    if (pair_agent_entry->m_shape_type_b == 29) {
        phys_volume_callback_shape *phantom = (phys_volume_callback_shape *)agent_body_b.get_shape();
        phantom->volume_enter_event(agent_body_b.get_root_collider(), agent_body_a.get_root_collider(), input);
        pair_agent_entry->m_volume_shape_b = phantom;
    }
    return pair_agent_entry;
}
phys_agent *phys_volume_agent::create_no_volume_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    if (agent_body_a.get_shape()->get_type_id() == 29) {
        const phys_shape *shape = agent_body_a.get_shape();
        phys_volume_callback_shape *phantom = (phys_volume_callback_shape *)shape;
        phantom->volume_enter_event(agent_body_a.get_root_collider(), agent_body_b.get_root_collider(), input);
    }
    if (agent_body_b.get_shape()->get_type_id() == 29) {
        const phys_shape *shape = agent_body_b.get_shape();
        phys_volume_callback_shape *phantom = (phys_volume_callback_shape *)shape;
        phantom->volume_enter_event(agent_body_b.get_root_collider(), agent_body_a.get_root_collider(), input);
    }
    return 0;
}
void phys_volume_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs dispatcher_agent_funcs;
  dispatcher_agent_funcs.m_flipped = false;
  dispatcher_agent_funcs.m_create_fn = phys_volume_agent::create_volume_agent;
  dispatcher_agent_funcs.m_penetrations_fn = phys_volume_agent::static_penetrations;
  dispatcher_agent_funcs.m_closest_points_fn = phys_volume_agent::static_closest_points;
  dispatcher_agent_funcs.m_linear_cast_func = phys_volume_agent::static_sweep_cast;
  dispatcher_agent_funcs.m_predictive = true;
  dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_VOLUME_CALLBACK, PHYS_SHAPE_ALL);
  dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_ALL, PHYS_SHAPE_VOLUME_CALLBACK);
}
