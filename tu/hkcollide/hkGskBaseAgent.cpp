// hkcollide/hkGskBaseAgent.obj: from the records 0x63a130 0x63a138 0x63a158 0x63a178 0x63a1b0 0x63a2a0 0x63a348 0x63a4d0 0x63a608 0x63a880 0x63aa80
// @compiler xdk6130
// @flags /TP /O1 /Oc
// @address 0x63a130 ?process_collision@phys_simplex_agent_base@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x63a138 ?cleanup@phys_simplex_agent_base@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x63a2a0 ?compute_separating_normal@phys_simplex_agent_base@@KAXABVphys_collision_proxy@@0MAAVphys_simplex@@AAVphys_vec4@@@Z
#include "phys/phys_agent.h"
#include "phys/phys_simplex_out.h"
#include "phys/phys_collectors.h"
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"


#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_simplex_status.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_simplex_agent_base.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_triangle_shape.h"

void phys_simplex_agent_base::process_collision(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_process_collision_input &, phys_process_collision_output &) {}
void phys_simplex_agent_base::cleanup(phys_constraint_owner &) { delete this; }
// @address 0x63a158 ?invalidate_sweep_info@phys_simplex_agent_base@@MAAXAAUphys_collision_input@@@Z
void phys_simplex_agent_base::invalidate_sweep_info(phys_collision_input &)
{
    m_separating_normal.set_zero4();
    m_separating_normal_time = -1.0f;
}
// @address 0x63a178 ?warp_time@phys_simplex_agent_base@@MAAXMMAAUphys_collision_input@@@Z
void phys_simplex_agent_base::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &)
{
    if (m_separating_normal_time == agent_old_time) {
        m_separating_normal_time = agent_new_time;
    } else {
        m_separating_normal_time = -1.0f;
        m_separating_normal.set_zero4();
    }
}
void phys_simplex_agent_base::compute_separating_normal(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, float tolerance, phys_simplex &gsk, phys_vec4 &normal_out)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(static_cast<const void *>(agent_body_a.get_shape()));
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(static_cast<const void *>(agent_body_b.get_shape()));
    phys_transform list_b_to_a;
    list_b_to_a.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    phys_vec4 simplex_separating_normal;
    gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, list_b_to_a, simplex_separating_normal);
    normal_out.set_rotated_dir(agent_body_a.get_transform().get_rotation(), simplex_separating_normal);
    normal_out(3) = simplex_separating_normal(3) - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
}

// match: 0x63a1b0: favor-speed boundary; without it five functions move, 0x63a1b0 loses 16B.
#pragma optimize("t", on)
// @address 0x63a4d0 ?static_penetrations@phys_simplex_agent_base@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_simplex_agent_base::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &, phys_proxy_pair_collector &collector)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(static_cast<const void *>(agent_body_a.get_shape()));
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(static_cast<const void *>(agent_body_b.get_shape()));
    phys_transform list_b_to_a;
    list_b_to_a.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    phys_simplex_cache cache;
    if (penetration_shape_b->get_type_id() == PHYS_SHAPE_TRIANGLE) {
        cache.init_triangle(penetration_shape_a, reinterpret_cast<const phys_triangle_shape *>(penetration_shape_b), list_b_to_a);
    } else {
        cache.init(penetration_shape_a, penetration_shape_b, list_b_to_a);
    }
    phys_simplex gsk;
    gsk.m_skip_penetration = 1;
    gsk.init(penetration_shape_a, penetration_shape_b, cache);
    phys_vec4 simplex_separating_normal;
    phys_simplex_status status = gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, list_b_to_a, simplex_separating_normal);
    if (status == PHYS_SIMPLEX_OK) {
        float distance = simplex_separating_normal(3) - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
        if (distance < 0.0f) {
            collector.add_proxy_pair(agent_body_a, agent_body_b);
        }
    } else {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}
// @address 0x63aa80 ?get_closest_points@phys_simplex_agent_base@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_simplex_agent_base::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_transform a_from_b;
    phys_simplex::closest_point_input simplex_input;
    a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    phys_vec4 point_on_b;
    phys_vec4 simplex_separating_normal;
    simplex_input.m_shape_a = static_cast<const phys_convex_shape *>(agent_body_a.get_shape());
    simplex_input.m_shape_b = static_cast<const phys_convex_shape *>(agent_body_b.get_shape());
    simplex_input.m_transform_a = &agent_body_a.get_transform();
    simplex_input.m_a_to_b = &a_from_b;
    simplex_input.m_collision_tolerance = input.get_tolerance();
    if (phys_simplex::get_closest_point(simplex_input, m_cache, simplex_separating_normal, point_on_b) == PHYS_SUCCESS) {
        phys_hit_point event(agent_body_a, agent_body_b);
        event.m_hit_contact.m_position = point_on_b;
        event.m_hit_contact.m_separating_normal = simplex_separating_normal;
        collector.add_hit(event);
    }
}
// @address 0x63a1b0 ??0phys_simplex_agent_base@@QAA@ABVphys_collision_proxy@@0PAVphys_contact_manager@@@Z
phys_simplex_agent_base::phys_simplex_agent_base(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    phys_contact_manager *pair_contact_manager)
    : phys_iterative_linear_cast_agent(pair_contact_manager)
{
    m_object_ref_count = 1;
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(agent_body_a.get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(agent_body_b.get_shape());
    phys_transform t;
    t.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    if (penetration_shape_b->get_type_id() == PHYS_SHAPE_TRIANGLE) {
        m_cache.init_triangle(penetration_shape_a, reinterpret_cast<const phys_triangle_shape *>(penetration_shape_b), t);
    } else {
        m_cache.init(penetration_shape_a, penetration_shape_b, t);
    }
    m_separating_normal.set_zero4();
    m_separating_normal(3) = -1.0f;
    m_separating_normal_time = -1.0f;
    float max_pen_a = agent_body_a.get_root_collider()->m_allowed_penetration_depth;
    float max_pen_b = agent_body_b.get_root_collider()->m_allowed_penetration_depth;
    m_allowed_penetration = (float)__fsel(max_pen_a - max_pen_b, max_pen_b, max_pen_a);
}

// @address 0x63a348 ?get_penetrations@phys_simplex_agent_base@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_simplex_agent_base::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(agent_body_a.get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(agent_body_b.get_shape());

    phys_transform a_from_b;
    a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());

    phys_simplex gsk;
    gsk.m_skip_penetration = true;

    phys_vec4 sep_normal_in_a;
    gsk.init(penetration_shape_a, penetration_shape_b, m_cache);
    int gjk_status = gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, a_from_b, sep_normal_in_a);
    if (gsk.m_feature_changed)
    {
        gsk.exit_and_export_cache(m_cache);
    }

    if (gjk_status == 0)
    {
        float dist = sep_normal_in_a(3) - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
        m_separating_normal.set_rotated_dir(agent_body_a.get_transform().get_rotation(), sep_normal_in_a);
        m_separating_normal(3) = dist;
        if (dist < 0.0f)
        {
            collector.add_proxy_pair(agent_body_a, agent_body_b);
        }
    }
    else
    {
        m_separating_normal.set_zero4();
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}

// @address 0x63a880 ?get_closest_point@phys_simplex_agent_base@@IAA?AVphys_bool@@ABVphys_collision_proxy@@0ABUphys_collision_input@@AAUphys_extended_simplex_out@@@Z
phys_bool phys_simplex_agent_base::get_closest_point(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1,
    struct phys_collision_input const &p2, struct phys_extended_simplex_out &p3)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(p0.get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(p1.get_shape());

    phys_transform a_from_b;
    a_from_b.set_mul_inverse_mul(p0.get_transform(), p1.get_transform());
    {
        phys_simplex gsk;
        phys_vec4 simplex_separating_normal;
        gsk.init(penetration_shape_a, penetration_shape_b, m_cache);
        gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, a_from_b, simplex_separating_normal);
        if (gsk.m_feature_changed) {
            gsk.exit_and_export_cache(m_cache);
        }
        gsk.feature_to_closest_distance(simplex_separating_normal, p3);
    }
    p3.m_normal_in_world.set_rotated_dir(p0.get_transform().get_rotation(), p3.m_normal_in_a);
    const float dist = p3.m_distance - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
    p3.m_distance = dist;
    if (dist < p2.m_box_tolerance) {
        p3.m_point_on_a.sub_mul4(penetration_shape_a->get_convex_radius(), p3.m_normal_in_a);
        phys_vec4 point_b_in_a;
        const float neg_dist = -dist;
        point_b_in_a.set_add_mul4(p3.m_point_on_a, p3.m_normal_in_a, neg_dist);
        p3.m_point_b_in_b.set_transformed_inverse_pos(a_from_b, point_b_in_a);
        return true;
    }
    return false;
}

__forceinline phys_bool phys_simplex_agent_base::static_closest_point(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_transform &a_from_b,
    const phys_collision_input &input, phys_simplex_cache &cache, phys_extended_simplex_out &output)
{
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(agent_body_a.get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(agent_body_b.get_shape());
    {
        phys_vec4 simplex_separating_normal;
        phys_simplex gsk;
        gsk.init(penetration_shape_a, penetration_shape_b, cache);
        if (gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, a_from_b, simplex_separating_normal) == PHYS_SIMPLEX_OK) {
            gsk.feature_to_closest_distance(simplex_separating_normal, output);
            output.m_normal_in_world.vec_set_rotated_dir(agent_body_a.get_transform().get_rotation(), output.m_normal_in_a);
            const float dist = output.m_distance - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
            output.m_distance = dist;
            if (dist < input.m_box_tolerance) {
                output.m_point_on_a.add_scaled4(-penetration_shape_a->get_convex_radius(), output.m_normal_in_a);
                phys_vec4 point_b_in_a;
                point_b_in_a.set_add_mul4(output.m_point_on_a, output.m_normal_in_a, -dist);
                output.m_point_b_in_b.set_transformed_inverse_pos(a_from_b, point_b_in_a);
                return true;
            }
        }
    }
    return false;
}
// @address 0x63a608 ?static_closest_points@phys_simplex_agent_base@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_simplex_agent_base::static_closest_points(
    class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
        struct phys_collision_input const &input, class phys_hit_collector &collector)
{
    phys_extended_simplex_out output;
    phys_transform a_from_b;
    phys_simplex_cache cache;
    {
        const phys_convex_shape *sA = static_cast<const phys_convex_shape *>(agent_body_a.get_shape());
        const phys_convex_shape *sB = static_cast<const phys_convex_shape *>(agent_body_b.get_shape());
        a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
        if (sB->get_type_id() == PHYS_SHAPE_TRIANGLE) {
            cache.init_triangle(sA, reinterpret_cast<const phys_triangle_shape *>(sB), a_from_b);
        } else {
            cache.init(sA, sB, a_from_b);
        }
    }
    if (static_closest_point(agent_body_a, agent_body_b, a_from_b, input, cache, output)) {
        phys_hit_point event(agent_body_a, agent_body_b);
        event.m_hit_contact.m_position.set_transformed_pos(agent_body_b.get_transform(), output.m_point_b_in_b);
        event.m_hit_contact.m_separating_normal = output.m_normal_in_world;
        event.m_hit_contact.m_separating_normal(3) = output.m_distance;
        collector.add_hit(event);
    }
}

