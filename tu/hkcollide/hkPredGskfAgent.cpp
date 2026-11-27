// hkcollide/hkPredGskfAgent.obj: from the records 0x630760 0x630800 0x630870
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x630760 ?create_pred_simplex_agent@phys_pred_simplex_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x630800 ?register_agent@phys_pred_simplex_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x630870 ?process_collision@phys_pred_simplex_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z

#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_continuous_simplex.h"
#include "phys/phys_math.h"
#include "phys/phys_agent.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_step_info.h"

#include "phys/phys_swept_transform_util.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_simplex_manifold.h"
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_collectors.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_shape_type_enum.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_simplex_agent_base.h"
#include "phys/phys_simplex_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_pred_simplex_agent.h"
#include "phys/phys_vec4_util.h"
static __forceinline void phys_pred_simplex_agent_manifold_init(const phys_simplex_manifold &box_manifold,
    const phys_vec4 &master_normal, const phys_collision_proxy &cA,
    const phys_collision_proxy &cB, float keep_contact_max_dist, phys_simplex_manifold_work &work)
{
    work.m_keep_contact = keep_contact_max_dist;
    const phys_convex_shape *penetration_shape_a = reinterpret_cast<const phys_convex_shape *>(cA.get_shape());
    const phys_convex_shape *penetration_shape_b = reinterpret_cast<const phys_convex_shape *>(cB.get_shape());
    work.m_radius_a = penetration_shape_a->get_convex_radius();
    work.m_radius_b = penetration_shape_b->get_convex_radius();
    const float max_dist_vec = (work.m_keep_contact + work.m_radius_b) + work.m_radius_a;
    work.m_radius_sum_sq = max_dist_vec * max_dist_vec;
    work.m_master_normal = master_normal;

    if (!box_manifold.m_contact_point_count)
    {
        return;
    }

    const unsigned short *vertex_ids = box_manifold.get_vertex_ids();

    penetration_shape_a->vertex_ids_to_vertices(vertex_ids, box_manifold.m_vert_count_a, &work.m_vertices[0]);
    phys_vec4_util::transform_points(cA.get_transform(), &work.m_vertices[0], box_manifold.m_vert_count_a, &work.m_vertices[0]);

    int b = box_manifold.m_vert_count_a;
    phys_hull_vertex *verts = &work.m_vertices[b];
    penetration_shape_b->vertex_ids_to_vertices(vertex_ids + b, box_manifold.m_vert_count_b, verts);
    phys_vec4_util::transform_points(cB.get_transform(), verts, box_manifold.m_vert_count_b, verts);
}

phys_agent *phys_pred_simplex_agent::create_pred_simplex_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry;
    if (pair_contact_manager != 0)
        pair_agent_entry = new phys_pred_simplex_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
    else
        pair_agent_entry = new phys_simplex_agent_base(agent_body_a, agent_body_b, pair_contact_manager);
    return pair_agent_entry;
}

void phys_pred_simplex_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.m_create_fn = create_pred_simplex_agent;
    dispatcher_agent_funcs.m_penetrations_fn = phys_simplex_agent_base::static_penetrations;
    dispatcher_agent_funcs.m_closest_points_fn = phys_simplex_agent_base::static_closest_points;
    dispatcher_agent_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    dispatcher_agent_funcs.m_flipped = false;
    dispatcher_agent_funcs.m_predictive = true;
    dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_CONVEX, PHYS_SHAPE_CONVEX);
}

void phys_pred_simplex_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    do
    {
        if (!(m_separating_normal_time == input.m_step_info.m_step_start_time))
        {
            if (!input.m_quality_info->m_use_ccd)
            {
                m_separating_normal_time = input.m_step_info.m_end_time;
                break;
            }

            phys_transform tA;
            phys_transform tB;
            phys_collision_proxy proxy_a(&agent_body_a, &tA); proxy_a.set_proxy_shape(agent_body_a.get_shape(), agent_body_a.get_shape_key());
            phys_collision_proxy proxy_b(&agent_body_b, &tB); proxy_b.set_proxy_shape(agent_body_b.get_shape(), agent_body_b.get_shape_key());
            phys_swept_transform_util::lerp_swept(agent_body_a.get_motion_state()->get_swept_transform(), input.m_step_info.m_step_start_time, tA);
            phys_swept_transform_util::lerp_swept(agent_body_b.get_motion_state()->get_swept_transform(), input.m_step_info.m_step_start_time, tB);

            const phys_convex_shape *penetration_shape_a = reinterpret_cast<const phys_convex_shape *>(agent_body_a.get_shape());
            const phys_convex_shape *penetration_shape_b = reinterpret_cast<const phys_convex_shape *>(agent_body_b.get_shape());

            phys_simplex gsk;
            gsk.init(penetration_shape_a, penetration_shape_b, m_cache);
            compute_separating_normal(proxy_a, proxy_b, input.m_quality_info->m_keep_contact, gsk, m_separating_normal);
            gsk.refresh_simplex_cache(m_cache);
        }

        m_separating_normal_time = input.m_step_info.m_end_time;

        const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
        const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();

        phys_vec4 sweep_info; phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, input.m_step_info.m_delta_time, sweep_info);

        float proj_lin_delta = sweep_info.dot3(m_separating_normal);
        float dist_at_t1 = m_separating_normal(3) - proj_lin_delta - sweep_info(3);

        if (dist_at_t1 > input.m_quality_info->m_keep_contact && dist_at_t1 > 0.5f * m_allowed_penetration)
        {
            m_separating_normal(3) = dist_at_t1;
            if (m_box_manifold.m_contact_point_count)
            {
                phys_simplex_manifold_cleanup(&m_box_manifold, m_contact_manager, *output.m_constraint_owner);
            }
            return;
        }

        if (input.m_quality_info->m_use_ccd)
        {
            phys_narrow_agent_process_input narrow_input;
            narrow_input.m_body_a = &agent_body_a;
            narrow_input.m_body_b = &agent_body_b;
            narrow_input.m_input = &input;
            narrow_input.m_contact_manager = m_contact_manager;
            narrow_input.m_dist_at_t1 = dist_at_t1;
            narrow_input.m_lin_sweep_info = sweep_info;

            phys_collision_quality_info &qi = *input.m_quality_info;
            const float distance = m_separating_normal(3);
            const float min_separation = phys_math::min_of_two(
                qi.m_min_separation * m_allowed_penetration, distance + qi.m_min_extra_separation * m_allowed_penetration);
            if (!(dist_at_t1 >= min_separation))
            {
                const float toi_separation = phys_math::min_of_two(
                    qi.m_toi_separation * m_allowed_penetration, distance + qi.m_toi_extra_separation * m_allowed_penetration);

                phys_4d_simplex_collide_toi(narrow_input, m_allowed_penetration, min_separation, toi_separation, m_cache, m_separating_normal, output);
                break;
            }
        }
        {
            if (dist_at_t1 > input.m_quality_info->m_manifold_sweep_dist)
            {
                m_separating_normal(3) = dist_at_t1;

                phys_simplex_manifold_work work;
                phys_pred_simplex_agent_manifold_init(m_box_manifold, m_separating_normal, agent_body_a, agent_body_b, input.get_tolerance(), work);
                phys_simplex_manifold_verify_get_points(m_box_manifold, work, 0, output, m_contact_manager);
                return;
            }
        }
    } while (false);

    {
        process_collision_no_sweep(agent_body_a, agent_body_b, input, output);
    }
}

