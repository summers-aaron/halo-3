// hkcollide/hkMultiRayConvexAgent.obj: from the records 0x628da0 0x628e20 0x629038 0x629250 0x6293a0 0x6293b8 0x629478 0x6294d0 0x629540 0x6296f8 0x629748 0x629798 0x629a00 0x629a50 0x629a98 0x629b48 0x629c38
// @flags /TP /O2
// @address 0x628da0 ?cleanup@phys_multi_ray_convex_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x6293a0 ?get_penetrations@phys_multi_ray_convex_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x629478 ?create_multi_ray_convex_agent@phys_multi_ray_convex_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x6294d0 ?create_convex_multi_ray_agent@phys_multi_ray_convex_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x6296f8 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x629748 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x629a00 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x629a50 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x629a98 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x629b48 ?sweep_cast@?$phys_symmetric_agent@Vphys_multi_ray_convex_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
#include "cseries/crt_xdk.h"
#include "phys/phys_base_types.h"
#include "phys/phys_collectors.h"
#include "phys/phys_collision_input.h"
// @address 0x629250 ?static_penetrations@phys_multi_ray_convex_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x629540 ?register_agent@phys_multi_ray_convex_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x628e20 ?get_closest_points@phys_multi_ray_convex_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
#include "phys/phys_dispatcher.h"

class phys_agent;
class phys_contact_manager;
class phys_collision_proxy;
struct phys_collision_input;
struct phys_linear_cast_collision_input;
class phys_proxy_pair_collector;
class phys_hit_collector;

class phys_constraint_owner;
class phys_statistics_collector;
struct phys_process_collision_input;
struct phys_process_collision_output;
extern "C" {
}

#include "phys/phys_multi_ray_convex_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_sphere_capsule_agent.h"

#include "phys/phys_multi_ray_shape.h"

void phys_multi_ray_convex_agent::cleanup(phys_constraint_owner &owner)
{
    int ray_contact_count = m_process_contact_points.getSize();
    for (int i = 0; i < ray_contact_count; ++i) {
        if (m_process_contact_points[i].m_process_contact_id != 0xffff) {
            m_contact_manager->contact_remove_point(m_process_contact_points[i].m_process_contact_id, owner);
        }
    }
    delete this;
}

void phys_multi_ray_convex_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

phys_agent *phys_multi_ray_convex_agent::create_multi_ray_convex_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_multi_ray_convex_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}

phys_agent *phys_multi_ray_convex_agent::create_convex_multi_ray_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent<phys_multi_ray_convex_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
}


void phys_multi_ray_convex_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_multi_ray_convex_agent::create_convex_multi_ray_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, (phys_shape_type)1, (phys_shape_type)20);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_multi_ray_convex_agent::create_multi_ray_convex_agent;
  backward.m_penetrations_fn = phys_multi_ray_convex_agent::static_penetrations;
  backward.m_closest_points_fn = phys_multi_ray_convex_agent::static_closest_points;
  backward.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, (phys_shape_type)20, (phys_shape_type)1);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_ray_convex_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}





template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_ray_convex_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_ray_convex_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_ray_convex_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float hit_toi = output.m_collision_toi.m_time;
    phys_multi_ray_convex_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (hit_toi != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_multi_ray_convex_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_multi_ray_convex_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_multi_ray_convex_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

template <>
void phys_symmetric_agent<phys_multi_ray_convex_agent>::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_iterative_linear_cast_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_iterative_linear_cast_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

// @address 0x629c38 phys_multi_ray_convex_agent_delete
extern "C" void *phys_multi_ray_convex_agent_delete(phys_multi_ray_convex_agent *pair_agent_entry, unsigned int dtor_flags)
{
    pair_agent_entry->m_process_contact_points.release_memory();
    pair_agent_entry->phys_object::~phys_object();
    if (dtor_flags & 1)
        phys_thread_memory::get_instance_ptr()->free_chunk(pair_agent_entry, pair_agent_entry->m_mem_size_and_flags, PHYS_MEM_AGENT);
    return pair_agent_entry;
}












void phys_multi_ray_convex_agent::get_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_ray_shape *motion_a_state = static_cast<const phys_multi_ray_shape *>(agent_body_a.get_shape());
    const phys_shape *convex_b_shape = agent_body_b.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(agent_body_b.get_transform(), agent_body_a.get_transform());

    const phys_array<phys_multi_ray_shape::multi_ray_entry> &rays = motion_a_state->m_ray_list;
    int ray_total_count = rays.getSize();
    const phys_multi_ray_shape::multi_ray_entry *cur_ray = rays.m_data;

    phys_hit_point event(agent_body_a, agent_body_b);

    phys_ray_input ray_input;
    for (int i = 0; i < ray_total_count; i++) {
        phys_ray_output ray_results;

        ray_input.m_ray_from.set_transformed_pos(b_from_a, cur_ray->m_start);
        ray_input.m_ray_to.set_transformed_pos(b_from_a, cur_ray->m_end);

        if (convex_b_shape->trace_ray(ray_input, ray_results)) {
            const phys_vec4 &normal = ray_results.m_normal;
            const float dist = ray_results.m_hit_frac;
            phys_vec4 ray_hit_pos;
            ray_hit_pos.set_lerp4(ray_input.m_ray_from, ray_input.m_ray_to, dist);

            event.m_hit_contact.m_position.set_transformed_pos(agent_body_b.get_transform(), ray_hit_pos);
            event.m_hit_contact.m_separating_normal.set_rotated_dir(agent_body_b.get_transform().get_rotation(), normal);
            event.m_hit_contact.contact_set_distance((dist - 1.0f) * cur_ray->m_start(3) + motion_a_state->get_ray_penetration_distance());

            collector.add_hit(event);
        }
        cur_ray++;
    }
}

// @address 0x629038 ?static_closest_points@phys_multi_ray_convex_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_multi_ray_convex_agent::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_ray_shape *motion_a_state = static_cast<const phys_multi_ray_shape *>(agent_body_a.get_shape());
    const phys_shape *convex_b_shape = agent_body_b.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(agent_body_b.get_transform(), agent_body_a.get_transform());

    const phys_array<phys_multi_ray_shape::multi_ray_entry> &rays = motion_a_state->m_ray_list;
    int ray_total_count = rays.getSize();
    const phys_multi_ray_shape::multi_ray_entry *cur_ray = rays.m_data;

    phys_hit_point event(agent_body_a, agent_body_b);

    phys_ray_input ray_input;
    for (int i = 0; i < ray_total_count; i++) {
        phys_ray_output ray_results;

        ray_input.m_ray_from.set_transformed_pos(b_from_a, cur_ray->m_start);
        ray_input.m_ray_to.set_transformed_pos(b_from_a, cur_ray->m_end);

        if (convex_b_shape->trace_ray(ray_input, ray_results)) {
            const phys_vec4 &normal = ray_results.m_normal;
            const float dist = ray_results.m_hit_frac;
            phys_vec4 ray_hit_pos;
            ray_hit_pos.set_lerp4(ray_input.m_ray_from, ray_input.m_ray_to, dist);

            event.m_hit_contact.m_position.set_transformed_pos(agent_body_b.get_transform(), ray_hit_pos);
            event.m_hit_contact.m_separating_normal.set_rotated_dir(agent_body_b.get_transform().get_rotation(), normal);
            event.m_hit_contact.contact_set_distance((dist - 1.0f) * cur_ray->m_start(3) + motion_a_state->get_ray_penetration_distance());

            collector.add_hit(event);
        }
        cur_ray++;
    }
}

// @address 0x629798 ?process_collision@phys_multi_ray_convex_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_multi_ray_convex_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &result)
{
    const phys_multi_ray_shape *motion_a_state = static_cast<const phys_multi_ray_shape *>(agent_body_a.get_shape());
    const phys_shape *convex_b_shape = agent_body_b.get_shape();

    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(agent_body_b.get_transform(), agent_body_a.get_transform());

    const phys_array<phys_multi_ray_shape::multi_ray_entry> &rays_a = motion_a_state->m_ray_list;
    int ray_total_count = rays_a.getSize();

    phys_ray_input ray_input;

    const phys_multi_ray_shape::multi_ray_entry *cur_ray = rays_a.m_data;
    for (int i = 0; i < ray_total_count; i++) {
        ray_input.m_ray_from.set_transformed_pos(b_from_a, cur_ray->m_start);
        ray_input.m_ray_to.set_transformed_pos(b_from_a, cur_ray->m_end);

        phys_ray_output ray_results;
        phys_bool ray_did_hit = convex_b_shape->trace_ray(ray_input, ray_results);
        if (!ray_did_hit) {
            if (m_process_contact_points[i].m_process_contact_id != 0xffff) {
                m_contact_manager->contact_remove_point(m_process_contact_points[i].m_process_contact_id, *result.m_constraint_owner);
                m_process_contact_points[i].m_process_contact_id = 0xffff;
            }
        } else {
            phys_process_hit_point &point = *result.m_next_free_contact;
            const phys_vec4 &normal = ray_results.m_normal;

            phys_vec4 ray_hit_pos;
            ray_hit_pos.set_lerp4(ray_input.m_ray_from, ray_input.m_ray_to, ray_results.m_hit_frac);
            point.m_hit_contact.m_position.vec_set_transformed_pos(agent_body_b.get_transform(), ray_hit_pos);

            point.m_hit_contact.m_separating_normal.vec_set_rotated_dir(agent_body_b.get_transform().get_rotation(), normal);
            point.m_hit_contact.contact_set_distance((ray_results.m_hit_frac - 1.0f) * cur_ray->m_start(3) + motion_a_state->get_ray_penetration_distance());

            if (m_process_contact_points[i].m_process_contact_id == 0xffff) {
                unsigned short id = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point.m_hit_contact);
                m_process_contact_points[i].m_process_contact_id = id;
            }

            if (m_process_contact_points[i].m_process_contact_id != 0xffff) {
                result.m_next_free_contact = result.m_next_free_contact + 1;
                point.m_process_contact_id = m_process_contact_points[i].m_process_contact_id;
            }
        }
        cur_ray++;
    }
}

void phys_multi_ray_convex_agent::static_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_multi_ray_shape *multi_ray = static_cast<const phys_multi_ray_shape *>(agent_body_a.get_shape());
    const phys_shape *other_convex = agent_body_b.get_shape();

    phys_transform convex_from_ray;
    convex_from_ray.set_mul_inverse_mul(agent_body_b.get_transform(), agent_body_a.get_transform());

    int ray_shape_count = multi_ray->get_rays().getSize();
    const phys_multi_ray_shape::multi_ray_entry *ray = multi_ray->get_rays().begin();

    phys_ray_input ray_input;
    phys_ray_output ray_cast_out;

    while (ray_shape_count-- && !collector.get_early_out()) {
        ray_input.m_ray_from.set_transformed_pos(convex_from_ray, ray->m_start);
        ray_input.m_ray_to.set_transformed_pos(convex_from_ray, ray->m_end);

        if (other_convex->trace_ray(ray_input, ray_cast_out)) {
            collector.add_proxy_pair(agent_body_a, agent_body_b);
        }
        ray++;
    }
}

// @address 0x6293b8 ??0phys_multi_ray_convex_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_multi_ray_convex_agent::phys_multi_ray_convex_agent(class phys_collision_proxy const &p0, class phys_collision_proxy const &p1,
    struct phys_collision_input const &p2, class phys_contact_manager *p3)
    : phys_iterative_linear_cast_agent(p3), m_process_contact_points(0)
{
    const phys_multi_ray_shape *motion_a_state = static_cast<const phys_multi_ray_shape *>(p0.get_shape());
    int ray_total = motion_a_state->m_ray_list.getSize();
    m_process_contact_points.set_array_size(ray_total);
    for (int i = 0; i < ray_total; i++) {
        m_process_contact_points[i].m_process_contact_id = 0xffff;
    }
}

