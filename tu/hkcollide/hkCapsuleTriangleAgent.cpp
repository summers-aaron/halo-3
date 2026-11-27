// hkcollide/hkCapsuleTriangleAgent.obj: from the records 0x62add8 0x62ae58 0x62aed0 0x62af60 0x62b128 0x62b378 0x62b5c0 0x62b820 0x62ba30 0x62bc58 0x62bd28 0x62bfd8 0x62c028 0x62c078 0x62c0c8 0x62c118
// @flags /TP /O2 /Oc
// @address 0x62add8 ?create_capsule_tri_agent@phys_capsule_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62ae58 ?cleanup@phys_capsule_triangle_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62aed0 ?create_tri_capsule_agent@phys_capsule_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62bc58 ?register_agent@phys_capsule_triangle_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x62bfd8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62c028 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62c078 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62c0c8 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62c118 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_symmetric_agent_linear_cast.h"

#include "phys/phys_base.h"
#include "phys/phys_triangle_shape.h"

#include "phys/phys_collision_input.h"

#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_capsule_triangle_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_sphere_capsule_agent.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collide_capsule_triangle_util.h"
#include "phys/phys_vec4_util.h"

phys_agent *phys_capsule_triangle_agent::create_capsule_tri_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_capsule_triangle_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}
void phys_capsule_triangle_agent::cleanup(phys_constraint_owner &owner)
{
    for (int i = 0; i < 3; ++i)
        if (m_process_contact_id[i] != 0xffff)
            m_contact_manager->contact_remove_point(m_process_contact_id[i], owner);
    delete this;
}
phys_agent *phys_capsule_triangle_agent::create_tri_capsule_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_capsule_triangle_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs triangle_sphere_funcs;
    triangle_sphere_funcs.m_create_fn = create_tri_capsule_agent;
    triangle_sphere_funcs.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_penetrations;
    triangle_sphere_funcs.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_closest_points;
    triangle_sphere_funcs.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
    triangle_sphere_funcs.m_flipped = true;
    triangle_sphere_funcs.m_predictive = false;
    dispatcher->register_collision_agent(triangle_sphere_funcs, PHYS_SHAPE_TRIANGLE, PHYS_SHAPE_CAPSULE);
    phys_dispatcher::agent_funcs sphere_triangle_funcs;
    sphere_triangle_funcs.m_create_fn = create_capsule_tri_agent;
    sphere_triangle_funcs.m_penetrations_fn = static_penetrations;
    sphere_triangle_funcs.m_closest_points_fn = static_closest_points;
    sphere_triangle_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    sphere_triangle_funcs.m_flipped = false;
    sphere_triangle_funcs.m_predictive = false;
    dispatcher->register_collision_agent(sphere_triangle_funcs, PHYS_SHAPE_CAPSULE, PHYS_SHAPE_TRIANGLE);
}
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_capsule_triangle_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_capsule_triangle_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_capsule_triangle_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_capsule_triangle_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_capsule_triangle_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    if (old_toi != output.m_collision_toi.m_time) output.m_collision_toi.flip();
}
// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_capsule_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_capsule_triangle_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_capsule_triangle_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
// @address 0x62b820 ?get_penetrations@phys_capsule_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_capsule_triangle_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_contact_point points[3];
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), m_triangle_cache, input.m_box_tolerance, 0, points);
    if (points[0].m_separating_normal.m_simd_quad.w < 0.0f || points[1].m_separating_normal.m_simd_quad.w < 0.0f)
    {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}
// @address 0x62ba30 ?static_penetrations@phys_capsule_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_capsule_triangle_agent::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_collide_triangle_util::point_tri_distance_cache cache;
    const phys_triangle_shape *tri_b_shape = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_collide_triangle_util::setup_point_triangle_distance_cache(tri_b_shape->get_vertices(), cache);
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_contact_point points[3];
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), cache, input.m_box_tolerance, 0, points);
    if (points[0].m_separating_normal.m_simd_quad.w < 0.0f || points[1].m_separating_normal.m_simd_quad.w < 0.0f)
    {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}
// @address 0x62b128 ?get_closest_point@phys_capsule_triangle_agent@@SA?AW4closest_point_hit@1@ABVphys_collision_proxy@@0ABUphys_collision_input@@AAUpoint_tri_distance_cache@phys_collide_triangle_util@@AAVphys_contact_point@@@Z
phys_capsule_triangle_agent::closest_point_hit phys_capsule_triangle_agent::get_closest_point(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_collide_triangle_util::point_tri_distance_cache &cache, phys_contact_point &capsule_closest_point)
{
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_contact_point points[3];
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), cache, input.m_box_tolerance, 0, points);
    float point_dist0 = points[0].m_separating_normal.m_simd_quad.w;
    float point_dist1 = points[1].m_separating_normal.m_simd_quad.w;
    if (point_dist0 < point_dist1)
    {
        if (point_dist0 < input.m_box_tolerance)
        {
            capsule_closest_point.m_position.m_simd_quad = points[0].m_position.m_simd_quad;
            capsule_closest_point.m_separating_normal.m_simd_quad = points[0].m_separating_normal.m_simd_quad;
            return TRI_NEAREST_HIT;
        }
    }
    else
    {
        if (point_dist1 < input.m_box_tolerance)
        {
            capsule_closest_point.m_position.m_simd_quad = points[1].m_position.m_simd_quad;
            capsule_closest_point.m_separating_normal.m_simd_quad = points[1].m_separating_normal.m_simd_quad;
            return TRI_NEAREST_HIT;
        }
    }
    return TRI_NEAREST_MISS;
}
// @address 0x62b378 ?get_closest_points@phys_capsule_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_capsule_triangle_agent::get_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_contact_point points[3];
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), m_triangle_cache, input.m_box_tolerance, 0, points);
    float point_dist0 = points[0].m_separating_normal.m_simd_quad.w;
    float point_dist1 = points[1].m_separating_normal.m_simd_quad.w;
    if (point_dist0 < point_dist1)
    {
        if (point_dist0 >= input.m_box_tolerance) return;
        event.m_hit_contact.m_position.m_simd_quad = points[0].m_position.m_simd_quad;
        event.m_hit_contact.m_separating_normal.m_simd_quad = points[0].m_separating_normal.m_simd_quad;
    }
    else
    {
        if (point_dist1 >= input.m_box_tolerance) return;
        event.m_hit_contact.m_position.m_simd_quad = points[1].m_position.m_simd_quad;
        event.m_hit_contact.m_separating_normal.m_simd_quad = points[1].m_separating_normal.m_simd_quad;
    }
    collector.add_hit(event);
}

// @address 0x62b5c0 ?static_closest_points@phys_capsule_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_capsule_triangle_agent::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_collide_triangle_util::point_tri_distance_cache cache;
    const phys_triangle_shape *tri_b_shape = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_collide_triangle_util::setup_point_triangle_distance_cache(tri_b_shape->get_vertices(), cache);
    phys_hit_point event(agent_body_a, agent_body_b);
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_contact_point points[3];
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), cache, input.m_box_tolerance, 0, points);
    float point_dist0 = points[0].m_separating_normal.m_simd_quad.w;
    float point_dist1 = points[1].m_separating_normal.m_simd_quad.w;
    if (point_dist0 < point_dist1)
    {
        if (point_dist0 >= input.m_box_tolerance) return;
        event.m_hit_contact.m_position.m_simd_quad = points[0].m_position.m_simd_quad;
        event.m_hit_contact.m_separating_normal.m_simd_quad = points[0].m_separating_normal.m_simd_quad;
    }
    else
    {
        if (point_dist1 >= input.m_box_tolerance) return;
        event.m_hit_contact.m_position.m_simd_quad = points[1].m_position.m_simd_quad;
        event.m_hit_contact.m_separating_normal.m_simd_quad = points[1].m_separating_normal.m_simd_quad;
    }
    collector.add_hit(event);
}

// @address 0x62af60 ?get_closest_points_public@phys_capsule_triangle_agent@@CAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAUpoint_tri_distance_cache@phys_collide_triangle_util@@HPAVphys_contact_point@@@Z
void phys_capsule_triangle_agent::get_closest_points_public(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_collide_triangle_util::point_tri_distance_cache &cache, int search_manifold, phys_contact_point *points)
{
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), cache, input.m_box_tolerance, search_manifold, points);
}

__forceinline void phys_capsule_triangle_agent::get_closest_points_inl(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
    phys_collide_triangle_util::point_tri_distance_cache &cache, int search_manifold, phys_contact_point *points)
{
    const phys_capsule_shape *capsule_shape_a = (const phys_capsule_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_a.m_motion, capsule_shape_a->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(*(const phys_transform *)agent_body_b.m_motion, triangle_b->get_vertices(), 3, tri_vertices);
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule_shape_a->get_convex_radius(), tri_vertices,
        triangle_b->get_convex_radius(), cache, input.m_box_tolerance, search_manifold, points);
}
// @address 0x62bd28 ?process_collision@phys_capsule_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_capsule_triangle_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &result)
{
    phys_contact_point points[3];
    get_closest_points_inl(agent_body_a, agent_body_b, input, m_triangle_cache, true, points);
    {
        for (int p = 0; p < 3; p++)
        {
            if (points[p].m_separating_normal.m_simd_quad.w < input.m_box_tolerance)
            {
                if (m_process_contact_id[p] == 0xffff)
                {
                    m_process_contact_id[p] = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, points[p]);
                }
                if (m_process_contact_id[p] != 0xffff)
                {
                    phys_process_hit_point &point = *result.reserve_contact_points(1);
                    result.commit_contact_points(1);
                    point.m_hit_contact.m_position.m_simd_quad = points[p].m_position.m_simd_quad;
                    point.m_hit_contact.m_separating_normal.m_simd_quad = points[p].m_separating_normal.m_simd_quad;
                    point.m_process_contact_id = m_process_contact_id[p];
                }
            }
            else
            {
                if (m_process_contact_id[p] != 0xffff)
                {
                    m_contact_manager->contact_remove_point(m_process_contact_id[p], *result.m_constraint_owner);
                    m_process_contact_id[p] = 0xffff;
                }
            }
        }
    }
}

