// hkcollide/hkGskfAgent.obj: from the records 0x639d10 0x639d60 0x639da8 0x639df0 0x639e38 0x639e80 0x639f28 0x639f90 0x63a010
// @compiler xdk6130
// @flags /TP /O2
#include "phys/phys_base.h"
#include "phys/phys_simplex_agent_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_swept_transform_util.h"
// @address 0x639da8 ?commit_potential@phys_simplex_agent@@UAAXG@Z
// @address 0x639e38 ??0phys_simplex_agent@@IAA@ABVphys_collision_proxy@@0PAVphys_contact_manager@@@Z
// @address 0x639e80 ?create_simplex_agent@phys_simplex_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x639f28 ?register_agent@phys_simplex_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x639d60 ?bridge_remove_point@phys_simplex_agent@@UAAXG@Z
// @address 0x639df0 ?mark_zombie_entry@phys_simplex_agent@@UAAXG@Z

#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_simplex_manifold.h"
#include <vectorintrinsics.h>
#include "phys/phys_agent_entry.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_math.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_thread_memory.h"
// @address 0x6126c0 ?set_mul_inverse_mul@phys_transform@@QAAXABV1@0@Z
extern "C" {
}
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_simplex_agent_base.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_simplex_agent.h"

// @address 0x639d10 ?cleanup@phys_simplex_agent@@UAAXAAVphys_constraint_owner@@@Z
void phys_simplex_agent::cleanup(phys_constraint_owner &owner)
{
    phys_simplex_manifold_cleanup(&m_box_manifold, m_contact_manager, owner);
    delete this;
}

void phys_simplex_agent::bridge_remove_point(unsigned short bounding_contact_point_id)
{
    int count = m_box_manifold.m_contact_point_count;
    int index = 0;
    if (count > 0) {
        for (; index < count; index++)
            if (m_box_manifold.m_process_contact_points[index].m_handle_id == bounding_contact_point_id) {
                phys_simplex_manifold_remove_point(&m_box_manifold, index);
                return;
            }
    }
}

void phys_simplex_agent::commit_potential(unsigned short bounding_contact_point_id)
{
    for (int index = 0; index < m_box_manifold.m_contact_point_count; index++) {
        if (m_box_manifold.m_process_contact_points[index].m_handle_id == 0xffff) {
            m_box_manifold.m_process_contact_points[index].m_handle_id = bounding_contact_point_id;
            break;
        } else {
            continue;
        }
    }
}

void phys_simplex_agent::mark_zombie_entry(unsigned short bounding_contact_point_id)
{
    int index;
    int count = m_box_manifold.m_contact_point_count;
    phys_simplex_manifold::manifold_contact *point;
    for (index = 0; index < count; index++) {
        point = &m_box_manifold.m_process_contact_points[index];
        if (point->m_handle_id == bounding_contact_point_id) {
            point->m_dim_a = 0;
            point->m_dim_b = 0;
            break;
        }
    }
}

// match: 0x639e80: inline expands the constructor into createGskfAgent; out of line it calls it (28 bytes shorter)
#if defined(_XBOX) && !defined(__clang__)
inline phys_simplex_agent::phys_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    phys_contact_manager *pair_contact_manager)
#else
phys_simplex_agent::phys_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    phys_contact_manager *pair_contact_manager)
#endif
    : phys_simplex_agent_base(agent_body_a, agent_body_b, pair_contact_manager)
{
}

phys_agent *phys_simplex_agent::create_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry;
    if (pair_contact_manager != 0) {
        pair_agent_entry = new phys_simplex_agent(agent_body_a, agent_body_b, pair_contact_manager);
    } else {
        pair_agent_entry = new phys_simplex_agent_base(agent_body_a, agent_body_b, pair_contact_manager);
    }
    return pair_agent_entry;
}

void phys_simplex_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.m_create_fn = create_simplex_agent;
    dispatcher_agent_funcs.m_penetrations_fn = phys_simplex_agent_base::static_penetrations;
    dispatcher_agent_funcs.m_closest_points_fn = phys_simplex_agent_base::static_closest_points;
    dispatcher_agent_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    dispatcher_agent_funcs.m_flipped = false;
    dispatcher_agent_funcs.m_predictive = false;
    dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_CONVEX, PHYS_SHAPE_CONVEX);
}

// @address 0x639f90 ?process_collision_no_sweep@phys_simplex_agent@@IAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_simplex_agent::process_collision_no_sweep(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    phys_narrow_agent_process_input bridge_process_input;
    bridge_process_input.m_body_a = &agent_body_a;
    bridge_process_input.m_body_b = &agent_body_b;
    bridge_process_input.m_contact_manager = m_contact_manager;
    bridge_process_input.m_input = &input;
    const phys_motion_state *motion_state_a = agent_body_a.get_motion_state();
    const phys_motion_state *motion_state_b = agent_body_b.get_motion_state();
    bridge_process_input.m_a_to_b.set_mul_inverse_mul(motion_state_a->get_transform(), motion_state_b->get_transform());
#if defined(_XBOX) && !defined(__clang__)
    phys_agent_entry entry;
#else
    // phys_simplex_util_process_no_sweep casts entry to phys_simplex_agent_entry* and writes
    // m_disable_contact_sweeps at +5; a 4-byte phys_agent_entry overruns bridge_process_input.m_body_a on x86_64.
    phys_simplex_agent_entry entry;
#endif
    ::phys_simplex_util_process_no_sweep((bridge_process_input), &entry, (void *)(0),
        (m_cache), (m_box_manifold), (m_separating_normal),
        (output));
}


// @address 0x63a010 ?process_collision@phys_simplex_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_simplex_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    if (!(m_separating_normal_time == input.m_step_info.m_step_start_time)) {
        phys_vec4 sweep_info;
        phys_swept_transform_util::compute_sweep_info(*agent_body_a.get_motion_state(), *agent_body_b.get_motion_state(),
            input.m_step_info.m_delta_time, sweep_info);
        if (m_separating_normal(3) > input.m_box_tolerance) {
            m_separating_normal(3) -= (float)sweep_info.dot4_xyz1(m_separating_normal);
            if (m_separating_normal(3) > input.m_box_tolerance) {
                return;
            }
        }
    }
    m_separating_normal_time = input.m_step_info.m_end_time;
    process_collision_no_sweep(agent_body_a, agent_body_b, input, output);
}
