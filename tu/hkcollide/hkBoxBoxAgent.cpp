// hkcollide/hkBoxBoxAgent.obj: from the records 0x62ffe8 0x630060 0x630118 0x6302b0 0x630470 0x630488 0x630640 0x630658 0x6306c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x62ffe8 ?cleanup@phys_box_box_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x630470 ?get_closest_points@phys_box_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x630640 ?get_penetrations@phys_box_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x630658 ?register_agent@phys_box_box_agent@@SAXPAVphys_dispatcher@@@Z
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
// @address 0x6306c0 ?get_penetrations@phys_box_box_detector@@QAA?AVphys_bool@@XZ

#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"


extern "C" {
}


#include "phys/phys_collision_input.h"


#include "phys/phys_box_box_manifold.h"

#include "phys/phys_dispatcher.h"


#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_math.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collision_proxy.h"

#include "phys/phys_simplex_agent.h"

#include "phys/phys_box_box_agent.h"

#include "phys/phys_box_box_detector.h"

// @address 0x14d10 ?phys_simd_vec_0000@@3U__vector4@@B
// @rdata 0x82014d10..0x82014d20
const __vector4 phys_simd_vec_0000 = { 0.0f, 0.0f, 0.0f, 0.0f };
// @rdata-end

void phys_box_box_agent::cleanup(phys_constraint_owner &owner)
{
    for (int point_idx = 0; point_idx < m_box_manifold.get_manifold_point_count(); ++point_idx)
    {
        m_contact_manager->contact_remove_point(m_box_manifold[point_idx].m_process_contact_id, owner);
    }
    delete this;
}

// @address 0x630060 ?create_box_box_agent@phys_box_box_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_box_box_agent::create_box_box_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    const phys_box_shape *box_a = (const phys_box_shape *)agent_body_a.get_shape();
    const phys_box_shape *box_b = (const phys_box_shape *)agent_body_b.get_shape();
    const phys_vec4 &half_extents_a = box_a->get_half_extents();
    const phys_vec4 &half_extents_b = box_b->get_half_extents();
    if (input.m_box_tolerance >= half_extents_a(3) * 1.999f || input.m_box_tolerance >= half_extents_b(3) * 1.999f)
    {
        return phys_simplex_agent::create_simplex_agent(agent_body_a, agent_body_b, input, agent_contact_manager);
    }
    phys_agent *pair_agent_entry = new phys_box_box_agent(agent_contact_manager);
    return pair_agent_entry;
}

void phys_box_box_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    static_closest_points(agent_body_a, agent_body_b, input, collector);
}

void phys_box_box_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

void phys_box_box_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.m_create_fn = create_box_box_agent;
    dispatcher_agent_funcs.m_penetrations_fn = static_penetrations;
    dispatcher_agent_funcs.m_closest_points_fn = static_closest_points;
    dispatcher_agent_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    dispatcher_agent_funcs.m_flipped = false;
    dispatcher_agent_funcs.m_predictive = false;
    dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_BOX, PHYS_SHAPE_BOX);
}



// @address 0x630118 ?process_collision@phys_box_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_box_box_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    const phys_box_shape *box_a = (const phys_box_shape *)agent_body_a.get_shape();
    const phys_box_shape *box_b = (const phys_box_shape *)agent_body_b.get_shape();
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
    phys_transform a_from_b;
    a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    phys_box_box_detector box_box_detector(agent_body_a, agent_body_b, &input, m_contact_manager, &result, a_from_b,
        agent_body_a.get_transform(), lever_a4, agent_body_b.get_transform(), lever_b4, input.get_tolerance());
    box_box_detector.compute_box_manifold(m_box_manifold);
}

phys_bool phys_box_box_detector::get_penetrations()
{
    init_work_vars();
    return PHYS_SUCCESS == check_box_intersection(m_box_tolerance4);
}

// @address 0x630488 ?static_penetrations@phys_box_box_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_box_box_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_box_shape *box_a = (const phys_box_shape *)agent_body_a.get_shape();
    const phys_box_shape *box_b = (const phys_box_shape *)agent_body_b.get_shape();
    phys_vec4 rA;
    rA.flags_set_all(box_a->get_convex_radius());
    phys_vec4 lever_a4;
    lever_a4.set_add4(box_a->get_half_extents(), rA);
    phys_vec4 rB;
    rB.flags_set_all(box_b->get_convex_radius());
    phys_vec4 lever_b4;
    lever_b4.set_add4(box_b->get_half_extents(), rB);
    phys_transform a_from_b;
    a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    phys_box_box_detector box_box_detector(agent_body_a, agent_body_b, 0, 0, 0, a_from_b,
        agent_body_a.get_transform(), lever_a4, agent_body_b.get_transform(), lever_b4, input.m_box_tolerance);
    if (box_box_detector.get_penetrations()) {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}

// @address 0x6302b0 ?static_closest_points@phys_box_box_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_box_box_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_box_shape *box_a = (const phys_box_shape *)agent_body_a.get_shape();
    const phys_box_shape *box_b = (const phys_box_shape *)agent_body_b.get_shape();
    phys_vec4 rA;
    rA.flags_set_all(box_a->get_convex_radius());
    phys_vec4 lever_a4;
    lever_a4.set_add4(box_a->get_half_extents(), rA);
    phys_vec4 rB;
    rB.flags_set_all(box_b->get_convex_radius());
    phys_vec4 lever_b4;
    lever_b4.set_add4(box_b->get_half_extents(), rB);
    phys_transform a_from_b;
    a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());

    phys_hit_point event(agent_body_a, agent_body_b);

    phys_box_box_detector box_box_detector(agent_body_a, agent_body_b, 0, 0, 0, a_from_b,
        agent_body_a.get_transform(), lever_a4, agent_body_b.get_transform(), lever_b4, input.m_box_tolerance);

    phys_bool hit = box_box_detector.compute_closest_point(event.m_hit_contact);
    if (hit) {
        collector.add_hit(event);
    }
}

