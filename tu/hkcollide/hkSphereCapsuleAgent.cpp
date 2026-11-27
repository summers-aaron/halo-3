// hkcollide/hkSphereCapsuleAgent.obj: from the records 0x629610 0x62e9d8 0x62ea40 0x62eaa8 0x62eb10 0x62ec70 0x62ec88 0x62ef00 0x62f198 0x62f430 0x62f500 0x62f548 0x62f598 0x62f8a0 0x62f8f0 0x62f940
// @flags /TP /O2 /Oc
// @address 0x62e9d8 ?create_sphere_capsule_agent@phys_sphere_capsule_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62ea40 ?cleanup@phys_sphere_capsule_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62eaa8 ?create_capsule_sphere_agent@phys_sphere_capsule_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62ec70 ?get_penetrations@phys_sphere_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62f430 ?register_agent@phys_sphere_capsule_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x62f500 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62f548 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62f8a0 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62f8f0 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62f940 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_collide.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_sphere_capsule_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_vec4_util.h"

phys_agent *phys_sphere_capsule_agent::create_sphere_capsule_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_sphere_capsule_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_sphere_capsule_agent::cleanup(phys_constraint_owner &owner)
{
    if (m_process_contact_id != 0xffff) {
        m_contact_manager->contact_remove_point(m_process_contact_id, owner);
    }
    delete this;
}

phys_agent *phys_sphere_capsule_agent::create_capsule_sphere_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
    return pair_agent_entry;
}

void phys_sphere_capsule_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

void phys_sphere_capsule_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs capsule_sphere_funcs;
    capsule_sphere_funcs.m_create_fn = create_capsule_sphere_agent;
    capsule_sphere_funcs.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_penetrations;
    capsule_sphere_funcs.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_closest_points;
    capsule_sphere_funcs.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
    capsule_sphere_funcs.m_flipped = true;
    capsule_sphere_funcs.m_predictive = false;
    dispatcher->register_collision_agent(capsule_sphere_funcs, PHYS_SHAPE_CAPSULE, PHYS_SHAPE_SPHERE);
    phys_dispatcher::agent_funcs sphere_capsule_funcs;
    sphere_capsule_funcs.m_create_fn = create_sphere_capsule_agent;
    sphere_capsule_funcs.m_penetrations_fn = static_penetrations;
    sphere_capsule_funcs.m_closest_points_fn = static_closest_points;
    sphere_capsule_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    sphere_capsule_funcs.m_flipped = false;
    sphere_capsule_funcs.m_predictive = false;
    dispatcher->register_collision_agent(sphere_capsule_funcs, PHYS_SHAPE_SPHERE, PHYS_SHAPE_CAPSULE);
}

template<>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped(collector);
    phys_sphere_capsule_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped);
}

template<>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped(collector);
    phys_sphere_capsule_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped);
}

template<>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped(collector);
    phys_sphere_capsule_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped);
}

template<>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped(collector);
    phys_sphere_capsule_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped);
}


template<>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float toi_time = output.m_collision_toi.m_time;
    phys_sphere_capsule_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (toi_time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}


// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_sphere_capsule_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}


// @address 0x629610 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_sphere_capsule_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *second_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flip_collector(input.m_path, collector);
    if (second_collector != 0)
    {
        phys_symmetric_flip_cast_collector flip_collector_alt(input.m_path, *second_collector);
        phys_iterative_linear_cast_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_collector, &flip_collector_alt);
    }
    else
    {
        phys_iterative_linear_cast_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flip_collector, 0);
    }
}

// @address 0x62eb10 ?static_penetrations@phys_sphere_capsule_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_sphere_capsule_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_sphere_shape *sphere_a = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_capsule_shape *capsule_shape_b = (const phys_capsule_shape *)agent_body_b.get_shape();
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 caps_b[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, caps_b);
    phys_collide_triangle_util::point_segment_result result;
    phys_collide_triangle_util::closest_point_on_segment(pos_a, caps_b[0], caps_b[1], result);
    float capsule_radius_sum = sphere_a->get_convex_radius() + capsule_shape_b->get_convex_radius();
    __vector4 a_minus_b = __vsubfp(result.m_edge_point.m_simd_quad, pos_a.m_simd_quad);
    float bvh_dist_sq;
    __stvewx(__vmsum3fp(a_minus_b, a_minus_b), (void *)&bvh_dist_sq, 0);
    if (bvh_dist_sq < capsule_radius_sum * capsule_radius_sum) {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}


// @address 0x62f198 ?static_closest_points@phys_sphere_capsule_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_capsule_agent::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);

    const phys_sphere_shape *sphere_a = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_capsule_shape *capsule_shape_b = (const phys_capsule_shape *)agent_body_b.get_shape();
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 caps_b[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, caps_b);

    phys_collide_triangle_util::point_segment_result result;
    phys_collide_triangle_util::closest_point_on_segment(pos_a, caps_b[0], caps_b[1], result);

    phys_vec4 a_minus_b;
    a_minus_b.m_simd_quad = __vsubfp(pos_a.m_simd_quad, result.m_edge_point.m_simd_quad);
    float capsule_radius_sum = sphere_a->get_convex_radius() + capsule_shape_b->get_convex_radius();
    float ref_dist = input.m_box_tolerance + capsule_radius_sum;
    float bvh_dist_sq;
    __stvewx(__vmsum3fp(a_minus_b.m_simd_quad, a_minus_b.m_simd_quad), &bvh_dist_sq, 0);
    if (bvh_dist_sq >= ref_dist * ref_dist) {
        return;
    }

    phys_vec4 &normal = event.m_hit_contact.m_separating_normal;
    __vector4 neg_quad;
    float dist;
    if (bvh_dist_sq > 0.0f) {
        dist = sqrtf(bvh_dist_sq);
        neg_quad = a_minus_b.m_simd_quad;
    } else {
        dist = 0.0f;
        phys_vec4 edge;
        edge.m_simd_quad = __vsubfp(caps_b[1].m_simd_quad, caps_b[0].m_simd_quad);
        int min = 0;
        int ok_flag1 = 1;
        int ok_flag2 = 2;
        float a0 = fabsf(edge(0));
        float a1 = fabsf(edge(1));
        float a2 = fabsf(edge(2));
        if (a1 < a0) {
            ok_flag1 = 0;
            min = 1;
            a0 = a1;
        }
        if (a2 < a0) {
            ok_flag2 = min;
            min = 2;
        }
#if defined(_XBOX) && !defined(__clang__)
        normal.m_simd_quad = __vxor(normal.m_simd_quad, normal.m_simd_quad);
#else
        normal.m_simd_quad = __vzero();
#endif
        normal(ok_flag1) = edge(ok_flag2);
        normal(ok_flag2) = -edge(ok_flag1);
        neg_quad = normal.m_simd_quad;
    }

    __vector4 temp = __vmsum3fp(neg_quad, neg_quad);
    __vector4 scale;
    scale.v[0] = 1.0f / sqrtf(temp.v[0]);
    scale = __vspltw(scale, 0);
    __vector4 nn = __vmulfp(neg_quad, scale);
    normal.m_simd_quad = nn;

    float offset = capsule_shape_b->get_convex_radius() - dist;
    event.m_hit_contact.m_position.m_simd_quad = __vmaddfp(nn, __vspltw(__lvlx(&offset, 0), 0), pos_a.m_simd_quad);
    normal(3) = dist - capsule_radius_sum;
    collector.add_hit(event);
}
// @address 0x62ef00 ?get_closest_points@phys_sphere_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_capsule_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);

    const phys_sphere_shape *sphere_a = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_capsule_shape *capsule_shape_b = (const phys_capsule_shape *)agent_body_b.get_shape();
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 caps_b[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, caps_b);

    phys_collide_triangle_util::point_segment_result result;
    phys_collide_triangle_util::closest_point_on_segment(pos_a, caps_b[0], caps_b[1], result);

    phys_vec4 a_minus_b;
    a_minus_b.m_simd_quad = __vsubfp(pos_a.m_simd_quad, result.m_edge_point.m_simd_quad);
    float capsule_radius_sum = sphere_a->get_convex_radius() + capsule_shape_b->get_convex_radius();
    float ref_dist = input.m_box_tolerance + capsule_radius_sum;
    float bvh_dist_sq;
    __stvewx(__vmsum3fp(a_minus_b.m_simd_quad, a_minus_b.m_simd_quad), &bvh_dist_sq, 0);
    if (bvh_dist_sq >= ref_dist * ref_dist) {
        return;
    }

    phys_vec4 &normal = event.m_hit_contact.m_separating_normal;
    __vector4 neg_quad;
    float dist;
    if (bvh_dist_sq > 0.0f) {
        dist = sqrtf(bvh_dist_sq);
        neg_quad = a_minus_b.m_simd_quad;
    } else {
        dist = 0.0f;
        phys_vec4 edge;
        edge.m_simd_quad = __vsubfp(caps_b[1].m_simd_quad, caps_b[0].m_simd_quad);
        int min = 0;
        int ok_flag1 = 1;
        int ok_flag2 = 2;
        float a0 = fabsf(edge(0));
        float a1 = fabsf(edge(1));
        float a2 = fabsf(edge(2));
        if (a1 < a0) {
            ok_flag1 = 0;
            min = 1;
            a0 = a1;
        }
        if (a2 < a0) {
            ok_flag2 = min;
            min = 2;
        }
#if defined(_XBOX) && !defined(__clang__)
        normal.m_simd_quad = __vxor(normal.m_simd_quad, normal.m_simd_quad);
#else
        normal.m_simd_quad = __vzero();
#endif
        normal(ok_flag1) = edge(ok_flag2);
        normal(ok_flag2) = -edge(ok_flag1);
        neg_quad = normal.m_simd_quad;
    }

    __vector4 temp = __vmsum3fp(neg_quad, neg_quad);
    __vector4 scale;
    scale.v[0] = 1.0f / sqrtf(temp.v[0]);
    scale = __vspltw(scale, 0);
    __vector4 nn = __vmulfp(neg_quad, scale);
    normal.m_simd_quad = nn;

    float offset = capsule_shape_b->get_convex_radius() - dist;
    event.m_hit_contact.m_position.m_simd_quad = __vmaddfp(nn, __vspltw(__lvlx(&offset, 0), 0), pos_a.m_simd_quad);
    normal(3) = dist - capsule_radius_sum;
    collector.add_hit(event);
}
__forceinline phys_sphere_capsule_agent::closest_point_hit phys_sphere_capsule_agent::get_closest_point_inl(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_point &capsule_closest_point)
{
    const phys_sphere_shape *sphere_a = (const phys_sphere_shape *)agent_body_a.get_shape();
    const phys_capsule_shape *capsule_shape_b = (const phys_capsule_shape *)agent_body_b.get_shape();
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 caps_b[2];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), capsule_shape_b->get_vertices(), 2, caps_b);

    phys_collide_triangle_util::point_segment_result result;
    phys_collide_triangle_util::closest_point_on_segment(pos_a, caps_b[0], caps_b[1], result);

    phys_vec4 a_minus_b;
    a_minus_b.m_simd_quad = __vsubfp(pos_a.m_simd_quad, result.m_edge_point.m_simd_quad);
    float capsule_radius_sum = sphere_a->get_convex_radius() + capsule_shape_b->get_convex_radius();
    float ref_dist = input.m_box_tolerance + capsule_radius_sum;
    float bvh_dist_sq;
    __stvewx(__vmsum3fp(a_minus_b.m_simd_quad, a_minus_b.m_simd_quad), &bvh_dist_sq, 0);
    if (bvh_dist_sq >= ref_dist * ref_dist) {
        return TRI_NEAREST_MISS;
    }

    float dist;
    if (bvh_dist_sq > 0.0f) {
        dist = sqrtf(bvh_dist_sq);
        capsule_closest_point.m_separating_normal.m_simd_quad = a_minus_b.m_simd_quad;
    } else {
        dist = 0.0f;
        phys_vec4 edge;
        edge.m_simd_quad = __vsubfp(caps_b[1].m_simd_quad, caps_b[0].m_simd_quad);
        phys_vec4_util::compute_perpendicular_vec(edge, capsule_closest_point.m_separating_normal);
    }

    capsule_closest_point.m_separating_normal.normalize_xyz();

    float offset = capsule_shape_b->get_convex_radius() - dist;
    capsule_closest_point.m_position.m_simd_quad = __vmaddfp(capsule_closest_point.m_separating_normal.m_simd_quad,
        __vspltw(__lvlx(&offset, 0), 0), pos_a.m_simd_quad);
    capsule_closest_point.m_separating_normal(3) = dist - capsule_radius_sum;
    return TRI_NEAREST_HIT;
}
// @address 0x62ec88 ?get_closest_point@phys_sphere_capsule_agent@@KA?AW4closest_point_hit@1@ABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_contact_point@@@Z
phys_sphere_capsule_agent::closest_point_hit phys_sphere_capsule_agent::get_closest_point(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_point &capsule_closest_point)
{
    return get_closest_point_inl(agent_body_a, agent_body_b, input, capsule_closest_point);
}
// @address 0x62f598 ?process_collision@phys_sphere_capsule_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_sphere_capsule_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &result)
{
    phys_process_hit_point &point = *result.reserve_contact_points(1);
    if (get_closest_point_inl(agent_body_a, agent_body_b, input, point.m_hit_contact) != TRI_NEAREST_MISS) {
        if (m_process_contact_id == 0xffff) {
            m_process_contact_id = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point.m_hit_contact);
        }
        if (m_process_contact_id != 0xffff) {
            point.m_process_contact_id = m_process_contact_id;
            result.commit_contact_points(1);
        } else {
            result.abort_contact_points(1);
        }
    } else {
        result.abort_contact_points(1);
        if (m_process_contact_id != 0xffff) {
            m_contact_manager->contact_remove_point(m_process_contact_id, *result.m_constraint_owner);
            m_process_contact_id = 0xffff;
        }
    }
}

