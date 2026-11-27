// hkcollide/hkSphereSphereAgent.obj: from the records 0x62f9f0 0x62fa60 0x62fae8 0x62fb50 0x62fb68 0x62fcf8 0x62fe38 0x62ff80
// @flags /TP /O2
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
// @address 0x62f9f0 ?cleanup@phys_sphere_sphere_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x62fa60 ?static_penetrations@phys_sphere_sphere_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62fae8 ?create_sphere_sphere_agent@phys_sphere_sphere_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x62fb50 ?get_penetrations@phys_sphere_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x62ff80 ?register_agent@phys_sphere_sphere_agent@@SAXPAVphys_dispatcher@@@Z
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"

extern "C" {
}
#include "phys/phys_dispatcher.h"
#include "phys/phys_iterative_linear_cast_agent.h"
#include "phys/phys_sphere_sphere_agent.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_shape_type_enum.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_input.h"

void phys_sphere_sphere_agent::cleanup(phys_constraint_owner &owner)
{
    if (sphere_contact_id != 0xffff)
    {
        m_contact_manager->contact_remove_point(sphere_contact_id, owner);
        sphere_contact_id = -1;
    }
    delete this;
}

void phys_sphere_sphere_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_vec4 delta;
    delta.set_sub4(agent_body_b.get_transform().get_translation(), agent_body_a.get_transform().get_translation());
    float distance_sq = delta.dot3(delta);
    float pair_radius_sum = static_cast<const phys_sphere_shape *>(agent_body_a.get_shape())->get_convex_radius() +
        static_cast<const phys_sphere_shape *>(agent_body_b.get_shape())->get_convex_radius();
    if (distance_sq < pair_radius_sum * pair_radius_sum)
        collector.add_proxy_pair(agent_body_a, agent_body_b);
}

phys_agent *phys_sphere_sphere_agent::create_sphere_sphere_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    return new phys_sphere_sphere_agent(pair_contact_manager);
}

void phys_sphere_sphere_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

void phys_sphere_sphere_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.m_create_fn = create_sphere_sphere_agent;
    dispatcher_agent_funcs.m_penetrations_fn = static_penetrations;
    dispatcher_agent_funcs.m_closest_points_fn = static_closest_points;
    dispatcher_agent_funcs.m_linear_cast_func = phys_iterative_linear_cast_agent::static_sweep_cast;
    dispatcher->register_collision_agent(dispatcher_agent_funcs, PHYS_SHAPE_SPHERE, PHYS_SHAPE_SPHERE);
}




__forceinline phys_bool phys_sphere_sphere_agent::get_closest_point(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_point &contact)
{
    const phys_sphere_shape *sphere_a = static_cast<const phys_sphere_shape *>(static_cast<const void *>(agent_body_a.get_shape()));
    const phys_sphere_shape *sphere_b = static_cast<const phys_sphere_shape *>(static_cast<const void *>(agent_body_b.get_shape()));
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    const phys_vec4 &pos_b = agent_body_b.get_transform().get_translation();
    phys_vec4 vec; vec.set_sub4(pos_a, pos_b);
    const float bvh_dist_sq = vec.dot3(vec);
    float radius_sum_tol = input.get_tolerance() + sphere_b->get_convex_radius() + sphere_a->get_convex_radius();
    if (bvh_dist_sq < radius_sum_tol * radius_sum_tol) {
        float dist;
        if (bvh_dist_sq > 0.0f) {
            const float inv_dist = phys_math::inv_sqrt(bvh_dist_sq);
            contact.get_separating_normal().set_mul4(inv_dist, vec);
            dist = bvh_dist_sq * inv_dist;
        } else {
            contact.set_contact_normal(phys_transform::get_identity().get_column(0));
            dist = 0.0f;
        }
        dist -= sphere_a->get_convex_radius() + sphere_b->get_convex_radius();
        contact.get_contact_position().set_add_mul4(pos_b, contact.get_contact_normal(), sphere_b->get_convex_radius());
        contact.contact_set_distance(dist);
        return true;
    }
    return false;
}
// @address 0x62fe38 ?static_closest_points@phys_sphere_sphere_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_sphere_agent::static_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point(agent_body_a, agent_body_b, input, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}
// @address 0x62fb68 ?process_collision@phys_sphere_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_sphere_sphere_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    const phys_sphere_shape *sphere_a = static_cast<const phys_sphere_shape *>(static_cast<const void *>(agent_body_a.get_shape()));
    const phys_sphere_shape *sphere_b = static_cast<const phys_sphere_shape *>(static_cast<const void *>(agent_body_b.get_shape()));
    const phys_vec4 &pos_a = agent_body_a.get_transform().get_translation();
    const phys_vec4 &pos_b = agent_body_b.get_transform().get_translation();
    phys_vec4 vec; vec.set_sub4(pos_a, pos_b);
    const float bvh_dist_sq = vec.dot3(vec);
    float radius_sum_tol = sphere_a->get_convex_radius() + sphere_b->get_convex_radius() + input.get_tolerance();
    if (bvh_dist_sq < radius_sum_tol * radius_sum_tol) {
        phys_process_hit_point &point = *result.reserve_contact_points(1);
        float distance;
        if (bvh_dist_sq > 0.0f) {
            const float inv_dist = phys_math::inv_sqrt(bvh_dist_sq);
            point.m_hit_contact.get_separating_normal().set_mul4(inv_dist, vec);
            distance = bvh_dist_sq * inv_dist - (sphere_a->get_convex_radius() + sphere_b->get_convex_radius());
        } else {
        point.m_hit_contact.get_separating_normal() = phys_transform::get_identity().get_column(0);
            distance = -(sphere_a->get_convex_radius() + sphere_b->get_convex_radius());
        }
        point.m_hit_contact.get_contact_position().set_add_mul4(pos_b, point.m_hit_contact.get_contact_normal(), sphere_b->get_convex_radius());
        point.m_hit_contact.contact_set_distance(distance);
        if (sphere_contact_id == 0xffff) {
            sphere_contact_id = m_contact_manager->add_manifold_contact_point(agent_body_a, agent_body_b, input, result, 0, point.m_hit_contact);
        }
        if (sphere_contact_id != 0xffff) {
            point.m_process_contact_id = sphere_contact_id;
            result.commit_contact_points(1);
        } else {
            result.abort_contact_points(1);
        }
    } else {
        if (sphere_contact_id != 0xffff) {
            m_contact_manager->contact_remove_point(sphere_contact_id, *result.m_constraint_owner);
            sphere_contact_id = 0xffff;
        }
    }
}

// @address 0x62fcf8 ?get_closest_points@phys_sphere_sphere_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_sphere_sphere_agent::get_closest_points(
    class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
        struct phys_collision_input const &input, class phys_hit_collector &collector)
{
    phys_hit_point event(agent_body_a, agent_body_b);
    if (get_closest_point(agent_body_a, agent_body_b, input, event.m_hit_contact)) {
        collector.add_hit(event);
    }
}
