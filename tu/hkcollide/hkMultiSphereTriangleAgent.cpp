// hkcollide/hkMultiSphereTriangleAgent.obj: from the records 0x629cb8 0x629d40 0x629db8 0x629e50 0x62a098 0x62a2e8 0x62a568 0x62a7f8 0x62a8c8 0x62a918 0x62a968 0x62ac88 0x62acd8 0x62ad28
// @flags /TP /O2 /Oc
// @address 0x629cb8 ?create_multi_sphere_tri_agent@phys_multi_sphere_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x629d40 ?cleanup@phys_multi_sphere_triangle_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x629db8 ?create_tri_multi_sphere_agent@phys_multi_sphere_triangle_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62a7f8 ?register_agent@phys_multi_sphere_triangle_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x62a8c8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62a918 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62ac88 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62acd8 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x62ad28 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// match: Havok build configuration: hkVector4 without the user constructors (stack arrays of hkVector4 are plain)
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_multi_sphere_triangle_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_sphere_capsule_agent.h"

phys_agent *phys_multi_sphere_triangle_agent::create_multi_sphere_tri_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_multi_sphere_triangle_agent(agent_body_a, agent_body_b, input, pair_contact_manager);
}
void phys_multi_sphere_triangle_agent::cleanup(phys_constraint_owner &owner)
{
    for (int i = 0; i < 8; ++i)
        if (m_process_contact_id[i] != 0xffff)
            m_contact_manager->contact_remove_point(m_process_contact_id[i], owner);
    delete this;
}
phys_agent *phys_multi_sphere_triangle_agent::create_tri_multi_sphere_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_multi_sphere_triangle_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs triangle_sphere_funcs;
    triangle_sphere_funcs.m_create_fn = create_tri_multi_sphere_agent;
    triangle_sphere_funcs.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_penetrations;
    triangle_sphere_funcs.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_closest_points;
    triangle_sphere_funcs.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_sphere_capsule_agent>::static_sweep_cast;
    triangle_sphere_funcs.m_flipped = true;
    triangle_sphere_funcs.m_predictive = false;
    dispatcher->register_collision_agent(triangle_sphere_funcs, PHYS_SHAPE_TRIANGLE, PHYS_SHAPE_MULTI_SPHERE);
    phys_dispatcher::agent_funcs sphere_triangle_funcs;
    sphere_triangle_funcs.m_create_fn = create_multi_sphere_tri_agent;
    sphere_triangle_funcs.m_penetrations_fn = static_penetrations;
    sphere_triangle_funcs.m_closest_points_fn = static_closest_points;
    sphere_triangle_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    sphere_triangle_funcs.m_flipped = false;
    sphere_triangle_funcs.m_predictive = false;
    dispatcher->register_collision_agent(sphere_triangle_funcs, PHYS_SHAPE_MULTI_SPHERE, PHYS_SHAPE_TRIANGLE);
}
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_sphere_triangle_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_multi_sphere_triangle_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_sphere_triangle_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_multi_sphere_triangle_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_multi_sphere_triangle_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
        point->m_hit_contact.flip();
    if (old_toi != output.m_collision_toi.m_time) output.m_collision_toi.flip();
}
// @address 0x5bec98 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_multi_sphere_triangle_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_multi_sphere_triangle_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_multi_sphere_triangle_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
// @address 0x629e50 ?get_penetrations@phys_multi_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_multi_sphere_triangle_agent::get_penetrations(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_multi_sphere_shape *motion_a_state = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    const phys_triangle_shape *triangle_b = static_cast<const phys_triangle_shape *>(agent_body_b.get_shape());
    const phys_vec4 *vertices = triangle_b->get_vertices();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), vertices, 3, tri_vertices);
    const phys_vec4 *local_spheres = motion_a_state->get_spheres();
    phys_vec4 world_spheres[phys_multi_sphere_shape::MULTI_SPHERE_MAX];
    const int sphere_total = motion_a_state->get_sphere_count();
    phys_vec4_util::transform_points(agent_body_a.get_transform(), &local_spheres[0], sphere_total, &world_spheres[0]);
    phys_hit_point event(agent_body_a, agent_body_b);
    for (int i = 0; i < sphere_total; ++i)
    {
        const phys_vec4 &cur_sphere = world_spheres[i];
        float capsule_radius_sum = triangle_b->get_convex_radius();
        const float sphere_rad = local_spheres[i](3);
        capsule_radius_sum = capsule_radius_sum + sphere_rad;
        {
            phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
            phys_collide_triangle_util::closest_point_on_triangle(cur_sphere, &tri_vertices[0], m_triangle_cache, tri_closest_result);
            if (tri_closest_result.distance < capsule_radius_sum)
            {
                collector.add_proxy_pair(agent_body_a, agent_body_b);
                break;
            }
        }
    }
}
// @address 0x62a098 ?static_penetrations@phys_multi_sphere_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_multi_sphere_triangle_agent::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    const phys_multi_sphere_shape *motion_a_state = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    const phys_triangle_shape *triangle_b = static_cast<const phys_triangle_shape *>(agent_body_b.get_shape());
    const phys_vec4 *vertices = triangle_b->get_vertices();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), vertices, 3, tri_vertices);
    const phys_vec4 *local_spheres = motion_a_state->get_spheres();
    phys_vec4 world_spheres[phys_multi_sphere_shape::MULTI_SPHERE_MAX];
    const int sphere_total = motion_a_state->get_sphere_count();
    phys_vec4_util::transform_points(agent_body_a.get_transform(), &local_spheres[0], sphere_total, &world_spheres[0]);
    phys_collide_triangle_util::closest_point_tri_cache cache;
    phys_collide_triangle_util::setup_closest_point_triangle_cache(triangle_b->get_vertices(), cache);
    phys_hit_point event(agent_body_a, agent_body_b);
    for (int i = 0; i < sphere_total; ++i)
    {
        const phys_vec4 &cur_sphere = world_spheres[i];
        float capsule_radius_sum = triangle_b->get_convex_radius();
        const float sphere_rad = local_spheres[i](3);
        capsule_radius_sum = capsule_radius_sum + sphere_rad;
        {
            phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
            phys_collide_triangle_util::closest_point_on_triangle(cur_sphere, &tri_vertices[0], cache, tri_closest_result);
            if (tri_closest_result.distance < capsule_radius_sum)
            {
                collector.add_proxy_pair(agent_body_a, agent_body_b);
                break;
            }
        }
    }
}

// @address 0x62a2e8 ?get_closest_points@phys_multi_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_multi_sphere_triangle_agent::get_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_sphere_shape *motion_a_state = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    const phys_triangle_shape *triangle_b = static_cast<const phys_triangle_shape *>(agent_body_b.get_shape());
    const phys_vec4 *vertices = triangle_b->get_vertices();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), vertices, 3, tri_vertices);
    const phys_vec4 *local_spheres = motion_a_state->get_spheres();
    phys_vec4 world_spheres[phys_multi_sphere_shape::MULTI_SPHERE_MAX];
    const int sphere_total = motion_a_state->get_sphere_count();
    phys_vec4_util::transform_points(agent_body_a.get_transform(), &local_spheres[0], sphere_total, &world_spheres[0]);
    phys_hit_point event(agent_body_a, agent_body_b);
    for (int i = 0; i < sphere_total; ++i)
    {
        const phys_vec4 &cur_sphere = world_spheres[i];
        float capsule_radius_sum = triangle_b->get_convex_radius();
        const float sphere_rad = local_spheres[i](3);
        capsule_radius_sum = capsule_radius_sum + sphere_rad;
        {
            phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
            phys_collide_triangle_util::closest_point_on_triangle(cur_sphere, &tri_vertices[0], m_triangle_cache, tri_closest_result);
            if (tri_closest_result.distance < capsule_radius_sum + input.get_tolerance())
            {
                event.m_hit_contact.get_contact_position().set_add_mul4(cur_sphere, tri_closest_result.triangle_hit_direction,
                    phys_simd_scalar(triangle_b->get_convex_radius() - tri_closest_result.distance));
                event.m_hit_contact.set_separating_normal(tri_closest_result.triangle_hit_direction, tri_closest_result.distance - capsule_radius_sum);
                collector.add_hit(event);
            }
        }
    }
}
// @address 0x62a568 ?static_closest_points@phys_multi_sphere_triangle_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_multi_sphere_triangle_agent::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    const phys_multi_sphere_shape *motion_a_state = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    const phys_triangle_shape *triangle_b = static_cast<const phys_triangle_shape *>(agent_body_b.get_shape());
    const phys_vec4 *vertices = triangle_b->get_vertices();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), vertices, 3, tri_vertices);
    const phys_vec4 *local_spheres = motion_a_state->get_spheres();
    phys_vec4 world_spheres[phys_multi_sphere_shape::MULTI_SPHERE_MAX];
    const int sphere_total = motion_a_state->get_sphere_count();
    phys_vec4_util::transform_points(agent_body_a.get_transform(), &local_spheres[0], sphere_total, &world_spheres[0]);
    phys_hit_point event(agent_body_a, agent_body_b);
    phys_collide_triangle_util::closest_point_tri_cache cache;
    phys_collide_triangle_util::setup_closest_point_triangle_cache(triangle_b->get_vertices(), cache);
    for (int i = 0; i < sphere_total; ++i)
    {
        const phys_vec4 &cur_sphere = world_spheres[i];
        float capsule_radius_sum = triangle_b->get_convex_radius();
        const float sphere_rad = local_spheres[i](3);
        capsule_radius_sum = capsule_radius_sum + sphere_rad;
        {
            phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
            phys_collide_triangle_util::closest_point_on_triangle(cur_sphere, &tri_vertices[0], cache, tri_closest_result);
            if (tri_closest_result.distance < capsule_radius_sum + input.get_tolerance())
            {
                event.m_hit_contact.get_contact_position().set_add_mul4(cur_sphere, tri_closest_result.triangle_hit_direction,
                    phys_simd_scalar(triangle_b->get_convex_radius() - tri_closest_result.distance));
                event.m_hit_contact.set_separating_normal(tri_closest_result.triangle_hit_direction, tri_closest_result.distance - capsule_radius_sum);
                collector.add_hit(event);
            }
        }
    }
}
// @address 0x62a968 ?process_collision@phys_multi_sphere_triangle_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_multi_sphere_triangle_agent::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &result)
{
    const phys_multi_sphere_shape *motion_a_state = static_cast<const phys_multi_sphere_shape *>(agent_body_a.get_shape());
    const phys_triangle_shape *triangle_b = static_cast<const phys_triangle_shape *>(agent_body_b.get_shape());
    const phys_vec4 *vertices = triangle_b->get_vertices();
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(agent_body_b.get_transform(), vertices, 3, tri_vertices);
    const phys_vec4 *local_spheres = motion_a_state->get_spheres();
    phys_vec4 world_spheres[phys_multi_sphere_shape::MULTI_SPHERE_MAX];
    const int sphere_total = motion_a_state->get_sphere_count();
    phys_vec4_util::transform_points(agent_body_a.get_transform(), &local_spheres[0], sphere_total, &world_spheres[0]);
    const phys_vec4 *cur_sphere = &world_spheres[0];
    const phys_vec4 *local_sphere_ptr = &local_spheres[0];
    for (int j = sphere_total - 1; j >= 0; cur_sphere++, local_sphere_ptr++, j--)
    {
        float capsule_radius_sum = triangle_b->get_convex_radius();
        const float sphere_rad = (*local_sphere_ptr)(3);
        capsule_radius_sum = capsule_radius_sum + sphere_rad;
        {
            phys_collide_triangle_util::closest_point_tri_result tri_closest_result;
            phys_collide_triangle_util::closest_point_on_triangle(*cur_sphere, &tri_vertices[0], m_triangle_cache, tri_closest_result);
            if (tri_closest_result.distance < capsule_radius_sum + input.get_tolerance())
            {
                phys_process_hit_point &point = *result.reserve_contact_points(1);
                point.m_hit_contact.get_contact_position().set_add_mul4(*cur_sphere, tri_closest_result.triangle_hit_direction,
                    phys_simd_scalar(triangle_b->get_convex_radius() - tri_closest_result.distance));
                point.m_hit_contact.set_separating_normal(tri_closest_result.triangle_hit_direction);
                point.m_hit_contact.contact_set_distance(tri_closest_result.distance - capsule_radius_sum);
                if (m_process_contact_id[j] == 0xffff)
                {
                    m_process_contact_id[j] = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point.m_hit_contact);
                }
                if (m_process_contact_id[j] != 0xffff)
                {
                    result.commit_contact_points(1);
                    point.m_process_contact_id = m_process_contact_id[j];
                }
                else
                {
                    result.abort_contact_points(1);
                }
                continue;
            }
        }
        if (m_process_contact_id[j] != 0xffff)
        {
            m_contact_manager->contact_remove_point(m_process_contact_id[j], *result.m_constraint_owner.val());
            m_process_contact_id[j] = 0xffff;
        }
    }
}

