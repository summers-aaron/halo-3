// hkcollide/hkSphereBoxAgent.obj: from the records 0x62cfa8 0x62d010 0x62d078 0x62d0e0 0x62d1e0 0x62d1f8 0x62d508 0x62d7b0 0x62da58 0x62db28 0x62db70 0x62dbc0 0x62dc10 0x62dc60
// @flags /TP /O2 /Oc
// @address 0x62cfa8 ?cleanup@phys_sphere_box_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62d010 ?create_box_sphere_agent@phys_sphere_box_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62d078 ?create_sphere_box_agent@phys_sphere_box_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62d0e0 ?static_penetrations@phys_sphere_box_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62d1e0 ?get_penetrations@phys_sphere_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62d1f8 ?process_collision@phys_sphere_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x62d508 ?get_closest_points@phys_sphere_box_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62d7b0 ?static_closest_points@phys_sphere_box_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62da58 ?register_agent@phys_sphere_box_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x62db28 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62db70 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62dbc0 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62dc10 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62dc60 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_symmetric_agent_linear_cast.h"


#include "phys/phys_collision_input.h"


#include "phys/phys_vec4_util.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_sphere_box_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_sphere_capsule_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_statistics_collector.h"


void phys_sphere_box_agent::cleanup(phys_constraint_owner &owner)
{
    if (m_contact_id != 0xffff) {
        m_contact_manager->contact_remove_point(m_contact_id, owner);
    }
    delete this;
}

phys_agent *phys_sphere_box_agent::create_box_sphere_agent(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_collision_input &, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry = new phys_symmetric_agent_linear_cast<phys_sphere_box_agent>(pair_contact_manager);
    return pair_agent_entry;
}

phys_agent *phys_sphere_box_agent::create_sphere_box_agent(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_collision_input &, phys_contact_manager *pair_contact_manager)
{
    return new phys_sphere_box_agent(pair_contact_manager);
}

void phys_sphere_box_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &, phys_proxy_pair_collector &collector)
{
    const phys_convex_shape *sphere = (const phys_convex_shape *)agent_body_a.get_shape();
    const phys_box_shape *box = (const phys_box_shape *)agent_body_b.get_shape();
    const phys_vec4 &position = (const phys_vec4 &)agent_body_a.get_transform().get_column(3);
    phys_vec4 sphere_local_pos;
    sphere_local_pos.vec_set_transformed_inverse_pos(agent_body_b.get_transform(), position);
    phys_vec4 closest_local;
    closest_local.set_abs4(sphere_local_pos);
    closest_local.set_min4(closest_local, box->get_half_extents());
    phys_vec4_util::mul_signs4(closest_local, sphere_local_pos);
    phys_vec4 closest_world;
    closest_world.set_transformed_pos(agent_body_b.get_transform(), closest_local);
    phys_vec4 difference;
    difference.set_sub4(closest_world, position);
    float distance_sq = difference.dot3(difference);
    float radius = sphere->get_convex_radius() + box->get_convex_radius();
    if (distance_sq < radius * radius) {
        collector.add_proxy_pair(agent_body_a, agent_body_b);
    }
}

void phys_sphere_box_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

__forceinline phys_bool phys_sphere_box_agent::get_closest_point(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_point &contact_out)
{
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    phys_vec4 local_pos_b;
    local_pos_b.vec_set_transformed_inverse_pos(agent_body_b.get_transform(), pos_a);
    const phys_box_shape *box_b = (const phys_box_shape *)agent_body_b.get_shape();
    const phys_convex_shape *sphere_a = (const phys_convex_shape *)agent_body_a.get_shape();
    phys_vec4 abs_pos_b;
    abs_pos_b.set_abs4(local_pos_b);
    phys_vec4 clipped_abs_pos;
    clipped_abs_pos.set_min4(abs_pos_b, box_b->get_half_extents());
    phys_vec4 delta;
    delta.set_sub4(clipped_abs_pos, abs_pos_b);
    float distance;
    if (delta.compare_less_zero4().any_is_set(phys_vec4_cmp::CMP_MASK_XYZ)) {
        distance = static_cast<float>(delta.normalize_xyz_with_length()) - (sphere_a->get_convex_radius() + box_b->get_convex_radius());
        if (distance > input.get_tolerance()) {
            return false;
        }
        phys_vec4_util::mul_signs4(delta, local_pos_b);
        delta.set_neg4(delta);
        contact_out.get_separating_normal().set_rotated_dir(agent_body_b.get_transform().get_rotation(), delta);
    } else {
        float dx = abs_pos_b(0) - box_b->get_half_extents()(0);
        float dy = abs_pos_b(1) - box_b->get_half_extents()(1);
        float dz = abs_pos_b(2) - box_b->get_half_extents()(2);
        int i;
        if (dx > dy) {
            if (dx > dz) {
                i = 0;
                contact_out.set_contact_normal(agent_body_b.get_transform().get_column(0));
                distance = dx;
            } else {
                i = 2;
                contact_out.set_contact_normal(agent_body_b.get_transform().get_column(2));
                distance = dz;
            }
        } else {
            if (dy > dz) {
                i = 1;
                contact_out.set_contact_normal(agent_body_b.get_transform().get_column(1));
                distance = dy;
            } else {
                i = 2;
                contact_out.set_contact_normal(agent_body_b.get_transform().get_column(2));
                distance = dz;
            }
        }
        if (local_pos_b(i) < 0.0f) {
            contact_out.get_separating_normal().set_neg4(contact_out.get_separating_normal());
        }
        distance = distance - box_b->get_convex_radius() - sphere_a->get_convex_radius();
    }
    contact_out.get_contact_position().set_add_mul4(pos_a, contact_out.get_contact_normal(), phys_simd_scalar(-distance - sphere_a->get_convex_radius()));
    contact_out.contact_set_distance(distance);
    return true;
}

void phys_sphere_box_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    phys_process_hit_point &point = *result.m_next_free_contact;
    if (get_closest_point(agent_body_a, agent_body_b, input, point.m_hit_contact)) {
        if (m_contact_id == 0xffff) {
            m_contact_id = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point.m_hit_contact);
        }
        if (m_contact_id != 0xffff) {
            point.m_process_contact_id = m_contact_id;
            result.commit_contact_points(1);
        }
    } else {
        if (m_contact_id != 0xffff) {
            m_contact_manager->contact_remove_point(m_contact_id, *result.m_constraint_owner);
            m_contact_id = 0xffff;
        }
    }
}

void phys_sphere_box_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point(agent_body_a, agent_body_b, input, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}

void phys_sphere_box_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point(agent_body_a, agent_body_b, input, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}


void phys_sphere_box_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs forward;
    forward.m_create_fn = create_box_sphere_agent;
    forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::static_penetrations;
    forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::static_closest_points;
    forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
    forward.m_flipped = true;
    forward.m_predictive = false;
    dispatcher->register_collision_agent(forward, PHYS_SHAPE_BOX, PHYS_SHAPE_SPHERE);
    phys_dispatcher::agent_funcs backward;
    backward.m_create_fn = create_sphere_box_agent;
    backward.m_penetrations_fn = static_penetrations;
    backward.m_closest_points_fn = static_closest_points;
    backward.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    backward.m_flipped = false;
    backward.m_predictive = false;
    dispatcher->register_collision_agent(backward, PHYS_SHAPE_SPHERE, PHYS_SHAPE_BOX);
}

template <>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_sphere_box_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_sphere_box_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template<>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_sphere_box_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_sphere_box_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float time = output.m_collision_toi.m_time;
    phys_sphere_box_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (time != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_sphere_box_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_sphere_box_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_sphere_box_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

