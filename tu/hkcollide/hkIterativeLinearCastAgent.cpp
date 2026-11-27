// hkcollide/hkIterativeLinearCastAgent.obj: from the records 0x6338d8 0x639560 0x639878
// @flags /TP /O2 /Oc
// @address 0x639560 ?sweep_cast@phys_iterative_linear_cast_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x639878 ?static_sweep_cast@phys_iterative_linear_cast_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6338d8 ??_Gphys_simple_closest_collector@@UAAPAXI@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_agent.h"
#include "phys/phys_collide.h"


#include "phys/phys_simple_closest_collector.h"


#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_agent_config.h"

#include "phys/phys_collision_input.h"

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_statistics_collector.h"

// @address 0x639560
void phys_iterative_linear_cast_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_simple_closest_collector start_point_collector;

    phys_collision_input sphere_input = input;
    sphere_input.m_box_tolerance = sphere_input.m_box_tolerance + input.m_cached_path_length;

    get_closest_points(agent_body_a, agent_body_b, sphere_input, start_point_collector);

    if (!start_point_collector.has_hit())
    {
        return;
    }

    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_contact_point &hit = start_point_collector.get_hit_contact();
    event.m_hit_contact.get_contact_position() = hit.get_contact_position();
    event.m_hit_contact.get_separating_normal() = hit.get_separating_normal();

    if (hit.get_distance() < input.m_box_tolerance)
    {
        if (start_collector)
        {
            start_collector->add_hit(event);
        }
    }

    const phys_vec4 &path = input.m_path;

    const float start_dist = hit.get_distance();
    const float path_along_normal = hit.get_contact_normal().dot3(path);
    const float end_dist = start_dist + path_along_normal;

    if (end_dist > 0.0f)
    {
        return;
    }

    if (path_along_normal + input.m_max_extra_penetration >= 0.0f)
    {
        return;
    }

    if (start_dist <= input.m_agent_config->m_iter_cast_early_out_dist)
    {
        if (start_dist > 0.0f)
        {
            const float cast_frac = start_dist / (start_dist - end_dist);
            if (cast_frac > collector.get_early_out_distance())
            {
                return;
            }
            event.m_hit_contact.contact_set_distance(cast_frac);
        }
        else
        {
            event.m_hit_contact.contact_set_distance(0.0f);
        }
        collector.add_hit(event);
        return;
    }

    event.m_hit_contact.contact_set_distance(start_dist / (start_dist - end_dist));

    {
    phys_simple_closest_collector check_collector;
    phys_motion_state motion_state_copy = *agent_body_a.get_motion_state();
    phys_collision_proxy proxy_a_copy(&agent_body_a, &motion_state_copy);
    proxy_a_copy.set_proxy_shape(agent_body_a.get_shape());

    for (int i = input.m_agent_config->m_iter_cast_max_iters - 1; i >= 0; i--)
    {
        check_collector.reset();

        const phys_vec4 &cast_old_pos = agent_body_a.get_motion_state()->get_transform().get_translation();
        phys_vec4 cast_new_pos;
        cast_new_pos.set_add_mul4(cast_old_pos, path, phys_simd_scalar(event.m_hit_contact.get_distance()));
        motion_state_copy.get_transform().set_translation(cast_new_pos);

        get_closest_points(proxy_a_copy, agent_body_b, sphere_input, check_collector);

        const phys_contact_point &check_contact = check_collector.get_hit_contact();
        const phys_vec4 &normal = check_contact.get_contact_normal();

        float path_along_normal2 = normal.dot3(path);
        if (path_along_normal2 >= 0.0f)
        {
            return;
        }
        path_along_normal2 = -path_along_normal2;

        const float start_dist2 = check_contact.get_distance();
        const float pre_dist = path_along_normal2 * event.m_hit_contact.get_distance();

        if (start_dist2 + pre_dist > path_along_normal2)
        {
            return;
        }

        const float cast_frac = event.m_hit_contact.get_distance() + start_dist2 / path_along_normal2;
        if (cast_frac > collector.get_early_out_distance())
        {
            return;
        }

        event.m_hit_contact.contact_set_position(check_contact.get_contact_position());
        event.m_hit_contact.set_separating_normal(check_contact.get_contact_normal(), cast_frac);

        if (start_dist2 <= input.m_agent_config->m_iter_cast_early_out_dist)
        {
            break;
        }
    }
    }

    collector.add_hit(event);
}

// @address 0x639878
void phys_iterative_linear_cast_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_simple_closest_collector start_point_collector;

    phys_collision_input sphere_input = input;
    sphere_input.m_box_tolerance = sphere_input.m_box_tolerance + input.m_cached_path_length;

    phys_dispatcher::agent_funcs::closest_points_fn get_closest_points = input.m_agent_dispatcher->get_closest_points_fn(
        agent_body_a.get_shape()->get_type_id(), agent_body_b.get_shape()->get_type_id());
    get_closest_points(agent_body_a, agent_body_b, sphere_input, start_point_collector);

    if (!start_point_collector.has_hit())
    {
        return;
    }

    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_contact_point &hit = start_point_collector.get_hit_contact();
    event.m_hit_contact.get_separating_normal() = hit.get_separating_normal();
    event.m_hit_contact.get_contact_position() = hit.get_contact_position();

    if (hit.get_distance() < input.m_box_tolerance)
    {
        if (start_collector)
        {
            start_collector->add_hit(event);
        }
    }

    const phys_vec4 &path = input.m_path;

    const float start_dist = hit.get_distance();
    const float path_along_normal = hit.get_contact_normal().dot3(path);
    const float end_dist = start_dist + path_along_normal;

    if (end_dist > 0.0f)
    {
        return;
    }

    if (path_along_normal + input.m_max_extra_penetration >= 0.0f)
    {
        return;
    }

    if (start_dist <= input.m_agent_config->m_iter_cast_early_out_dist)
    {
        if (start_dist > 0.0f)
        {
            const float cast_frac = start_dist / (start_dist - end_dist);
            if (cast_frac > collector.get_early_out_distance())
            {
                return;
            }
            event.m_hit_contact.contact_set_distance(cast_frac);
        }
        else
        {
            event.m_hit_contact.contact_set_distance(0.0f);
        }
        collector.add_hit(event);
        return;
    }

    event.m_hit_contact.contact_set_distance(start_dist / (start_dist - end_dist));

    {
    phys_simple_closest_collector check_collector;
    phys_motion_state motion_state_copy = *agent_body_a.get_motion_state();
    phys_collision_proxy proxy_a_copy(&agent_body_a, &motion_state_copy);
    proxy_a_copy.set_proxy_shape(agent_body_a.get_shape());

    for (int i = input.m_agent_config->m_iter_cast_max_iters - 1; i >= 0; i--)
    {
        check_collector.reset();

        const phys_vec4 &cast_old_pos = agent_body_a.get_motion_state()->get_transform().get_translation();
        phys_vec4 cast_new_pos;
        cast_new_pos.set_add_mul4(cast_old_pos, path, phys_simd_scalar(event.m_hit_contact.get_distance()));
        motion_state_copy.get_transform().set_translation(cast_new_pos);

        get_closest_points(proxy_a_copy, agent_body_b, sphere_input, check_collector);

        if (!check_collector.has_hit())
        {
            return;
        }

        const phys_contact_point &check_contact = check_collector.get_hit_contact();
        const phys_vec4 &normal = check_contact.get_contact_normal();

        float path_along_normal2 = normal.dot3(path);
        if (path_along_normal2 >= 0.0f)
        {
            return;
        }
        path_along_normal2 = -path_along_normal2;

        const float start_dist2 = check_contact.get_distance();
        const float pre_dist = path_along_normal2 * event.m_hit_contact.get_distance();

        if (start_dist2 + pre_dist > path_along_normal2)
        {
            return;
        }

        const float cast_frac = event.m_hit_contact.get_distance() + start_dist2 / path_along_normal2;
        if (cast_frac > collector.get_early_out_distance())
        {
            return;
        }

        event.m_hit_contact.contact_set_position(check_contact.get_contact_position());
        event.m_hit_contact.set_separating_normal(check_contact.get_contact_normal(), cast_frac);

        if (start_dist2 <= input.m_agent_config->m_iter_cast_early_out_dist)
        {
            break;
        }
    }
    }

    collector.add_hit(event);
}

