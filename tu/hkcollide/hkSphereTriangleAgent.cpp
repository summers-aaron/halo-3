// hkcollide/hkSphereTriangleAgent.obj: from the records 0x62dd10 0x62dd88 0x62ddf0 0x62de70 0x62df38 0x62e010 0x62e188 0x62e310 0x62e4b0 0x62e5e0 0x62e630 0x62e680 0x62e888 0x62e8d8 0x62e928
// @flags /TP /O2 /Oc
// @address 0x62dd10 ?create_sphere_tri_agent@phys_sphere_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62dd88 ?cleanup@phys_sphere_triangle_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62ddf0 ?create_tri_sphere_agent@phys_sphere_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62de70 ?get_penetrations@phys_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62df38 ?static_penetrations@phys_sphere_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62e4b0 ?register_agent@phys_sphere_triangle_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x62e5e0 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62e630 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62e888 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62e8d8 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
// @address 0x62e928 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z

extern "C" {
}

#include "phys/phys_triangle_shape.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_sphere_triangle_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_sphere_capsule_agent.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_memory.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_vec4_util.h"


phys_agent *phys_sphere_triangle_agent::create_sphere_tri_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_sphere_triangle_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_sphere_triangle_agent::cleanup(phys_constraint_owner &owner)
{
    if (m_process_contact_id != 0xffff) {
        m_contact_manager->contact_remove_point(m_process_contact_id, owner);
    }
    delete this;
}

phys_agent *phys_sphere_triangle_agent::create_tri_sphere_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

void phys_sphere_triangle_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_sphere_shape *sphere = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle = (const phys_triangle_shape *)agent_body_b.get_shape();
    const phys_vec4 &position = agent_body_a.get_transform().get_translation();
    phys_vec4 sphere_local_pos;
    sphere_local_pos.set_transformed_inverse_pos(agent_body_b.get_transform(), position);
    phys_collide_triangle_util::closest_point_tri_result result;
    phys_collide_triangle_util::closest_point_on_triangle(sphere_local_pos, &triangle->get_vertex(0), m_closest_point_cache, result);
    float radius = sphere->get_convex_radius() + triangle->get_convex_radius();
    if (result.distance < radius) {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}

void phys_sphere_triangle_agent::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_sphere_shape *sphere = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle = (const phys_triangle_shape *)agent_body_b.get_shape();
    phys_collide_triangle_util::closest_point_tri_cache cache;
    phys_collide_triangle_util::setup_closest_point_triangle_cache(triangle->get_vertices(), cache);
    const phys_vec4 &position = agent_body_a.get_transform().get_translation();
    phys_vec4 sphere_local_pos;
    sphere_local_pos.set_transformed_inverse_pos(agent_body_b.get_transform(), position);
    phys_collide_triangle_util::closest_point_tri_result result;
    phys_collide_triangle_util::closest_point_on_triangle(sphere_local_pos, &triangle->get_vertex(0), cache, result);
    float radius = sphere->get_convex_radius() + triangle->get_convex_radius();
    if (result.distance < radius) {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}


void phys_sphere_triangle_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs triangle_sphere_funcs;
    triangle_sphere_funcs.m_create_fn = create_tri_sphere_agent;
    triangle_sphere_funcs.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_penetrations;
    triangle_sphere_funcs.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_closest_points;
    triangle_sphere_funcs.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
    triangle_sphere_funcs.m_flipped = true;
    triangle_sphere_funcs.m_predictive = false;
    dispatcher->register_collision_agent(triangle_sphere_funcs, (phys_shape_type)5, (phys_shape_type)3);
    phys_dispatcher::agent_funcs sphere_triangle_funcs;
    sphere_triangle_funcs.m_create_fn = create_sphere_tri_agent;
    sphere_triangle_funcs.m_penetrations_fn = static_penetrations;
    sphere_triangle_funcs.m_closest_points_fn = static_closest_points;
    sphere_triangle_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    sphere_triangle_funcs.m_flipped = false;
    sphere_triangle_funcs.m_predictive = false;
    dispatcher->register_collision_agent(sphere_triangle_funcs, (phys_shape_type)3, (phys_shape_type)5);
}

template <>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_sphere_triangle_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_sphere_triangle_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}


template<>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_sphere_triangle_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_sphere_triangle_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}


template <>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_sphere_triangle_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
    {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi != output.m_collision_toi.m_time)
    {
        output.m_collision_toi.flip();
    }
}

// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_sphere_triangle_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_sphere_triangle_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}


__forceinline phys_sphere_triangle_agent::closest_point_hit phys_sphere_triangle_agent::get_closest_point_inl(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_collide_triangle_util::closest_point_tri_cache &cache,
    phys_contact_point &capsule_closest_point)
{
    phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
    const phys_sphere_shape *sphere_a = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_triangle_shape *triangle_b = (const phys_triangle_shape *)agent_body_b.get_shape();
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), triangle_b->get_vertices(), 3, &tri_vertices[0]);
    phys_collide_triangle_util::closest_point_tri_status res = phys_collide_triangle_util::closest_point_on_triangle(pos_a,
        &tri_vertices[0], cache, tri_closest_result);
    float capsule_radius_sum = sphere_a->get_convex_radius() + triangle_b->get_convex_radius();
    if (tri_closest_result.distance < capsule_radius_sum + input.m_box_tolerance) {
        float shift = triangle_b->get_convex_radius() - tri_closest_result.distance;
        capsule_closest_point.m_position.m_simd_quad = __vmaddfp(tri_closest_result.triangle_hit_direction.m_simd_quad,
            __vspltw(__lvlx(&shift, 0), 0), pos_a.m_simd_quad);
        capsule_closest_point.set_separating_normal(tri_closest_result.triangle_hit_direction, tri_closest_result.distance - capsule_radius_sum);
        if (res == phys_collide_triangle_util::TRI_HIT_FACE) return TRI_NEAREST_FACE;
        return TRI_NEAREST_EDGE;
    }
    return TRI_NEAREST_MISS;
}
// @address 0x62e010 ?get_closest_point@phys_sphere_triangle_agent@@SA?AW4closest_point_hit@1@ABVphys_collision_proxy@@0ABUphys_collision_input@@AAUclosest_point_tri_cache@phys_collide_triangle_util@@AAVphys_contact_point@@@Z
phys_sphere_triangle_agent::closest_point_hit phys_sphere_triangle_agent::get_closest_point(
    class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
    struct phys_collision_input const &input, struct phys_collide_triangle_util::closest_point_tri_cache &cache,
    class phys_contact_point &capsule_closest_point)
{
    return get_closest_point_inl(agent_body_a, agent_body_b, input, cache, capsule_closest_point);
}

// @address 0x62e188 ?get_closest_points@phys_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_triangle_agent::get_closest_points(
    class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
        struct phys_collision_input const &input, class phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point_inl(agent_body_a, agent_body_b, input, m_closest_point_cache, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}

// @address 0x62e310 ?static_closest_points@phys_sphere_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_triangle_agent::static_closest_points(
    class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
        struct phys_collision_input const &input, class phys_hit_collector &collector)
{
    phys_collide_triangle_util::closest_point_tri_cache cache;
    phys_collide_triangle_util::setup_closest_point_triangle_cache(((const phys_triangle_shape *)agent_body_b.get_shape())->get_vertices(), cache);
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point_inl(agent_body_a, agent_body_b, input, cache, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}


// @address 0x62e680 ?process_collision@phys_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_sphere_triangle_agent::process_collision(class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
    struct phys_process_collision_input const &input, struct phys_process_collision_output &result)
{
    phys_process_hit_point *point = result.m_next_free_contact;
    if (get_closest_point_inl(agent_body_a, agent_body_b, input, m_closest_point_cache, point->m_hit_contact)) {
        if (m_process_contact_id == 0xffff) {
            m_process_contact_id = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point->m_hit_contact);
        }
        if (m_process_contact_id != 0xffff) {
            point->m_process_contact_id = m_process_contact_id;
            result.m_next_free_contact = result.m_next_free_contact + 1;
        }
    } else if (m_process_contact_id != 0xffff) {
        m_contact_manager->contact_remove_point(m_process_contact_id, *result.m_constraint_owner);
        m_process_contact_id = 0xffff;
    }
}

