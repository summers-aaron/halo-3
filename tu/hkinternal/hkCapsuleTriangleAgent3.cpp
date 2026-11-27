// hkinternal/hkCapsuleTriangleAgent3.obj: from the records 0x68aa20 0x68aa60 0x68aa98 0x68aaa0 0x68ab10 0x68ab80 0x68ab88 0x68ad80 0x68b0e8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x68aa60 ?commit_potential@phys_capsule_triangle_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68aa98 ?mark_zombie_entry@phys_capsule_triangle_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68aaa0 ?create@phys_capsule_triangle_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x68ab10 ?cleanup@phys_capsule_triangle_narrow_agent@@YAPAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@@Z
// @address 0x68ab80 ?destroy@phys_capsule_triangle_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x68b0e8 ?register_narrow_agent@phys_capsule_triangle_narrow_agent@@YAXPAVphys_dispatcher@@@Z
#include "phys/phys_base_types.h"
// @address 0x68aa20 ?bridge_remove_point@phys_capsule_triangle_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z

#include <xtl.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collide_triangle_util.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_agent_entry.h"


#include "phys/phys_capsule_triangle_narrow_agent.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collide_capsule_triangle_util.h"
#include "phys/phys_collision_input.h"

namespace {
struct capsule_tri_agent_data {
    unsigned short contact_ids[3];
    phys_collide_triangle_util::point_tri_distance_cache tri_dist_cache;
};
}

void phys_capsule_triangle_narrow_agent::bridge_remove_point(phys_agent_entry *entry, void *data, unsigned short id)
{
    unsigned short *contact_ids = (unsigned short *)data;
    for (int index = 0; index < 3; index++) {
        if (contact_ids[index] == id) {
            contact_ids[index] = 0xffff;
            return;
        }
    }
}

void phys_capsule_triangle_narrow_agent::commit_potential(phys_agent_entry *entry, void *data, unsigned short id)
{
    unsigned short *contact_ids = (unsigned short *)data;
    for (int index = 0; index < 3; index++) {
        if (contact_ids[index] == 0xffff) {
            contact_ids[index] = id;
            return;
        }
    }
}

void phys_capsule_triangle_narrow_agent::mark_zombie_entry(phys_agent_entry *entry, void *data, unsigned short id)
{
}

void *phys_capsule_triangle_narrow_agent::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data)
{
    const phys_triangle_shape *triangle = (const phys_triangle_shape *)input.m_body_b->get_shape();
    capsule_tri_agent_data *pair_agent_entry = (capsule_tri_agent_data *)data;
    entry->m_contact_point_count = 0;
    pair_agent_entry->contact_ids[0] = 0xffff;
    pair_agent_entry->contact_ids[1] = 0xffff;
    pair_agent_entry->contact_ids[2] = 0xffff;
    phys_collide_triangle_util::setup_point_triangle_distance_cache(triangle->get_vertices(), pair_agent_entry->tri_dist_cache);
    return phys_add_byte_offset(data, (sizeof(capsule_tri_agent_data) + 15) & ~15);
}

void *phys_capsule_triangle_narrow_agent::cleanup(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager,
    phys_constraint_owner &owner)
{
    capsule_tri_agent_data *pair_agent_entry = (capsule_tri_agent_data *)data;
    for (int index = 0; index < 3; index++) {
        if (pair_agent_entry->contact_ids[index] != 0xffff) {
            agent_contact_manager->contact_remove_point(pair_agent_entry->contact_ids[index], owner);
            pair_agent_entry->contact_ids[index] = 0xffff;
        }
    }
    return phys_add_byte_offset(data, (sizeof(capsule_tri_agent_data) + 15) & ~15);
}

void phys_capsule_triangle_narrow_agent::destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager,
    phys_constraint_owner &owner, phys_dispatcher *dispatcher)
{
    cleanup(entry, data, agent_contact_manager, owner);
}

void phys_capsule_triangle_narrow_agent::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs dispatcher_agent_funcs;
    dispatcher_agent_funcs.create = create;
    dispatcher_agent_funcs.process = process;
    dispatcher_agent_funcs.compute_sep_normal = compute_sep_normal;
    dispatcher_agent_funcs.cleanup = cleanup;
    dispatcher_agent_funcs.bridge_remove_point = bridge_remove_point;
    dispatcher_agent_funcs.commit_potential = commit_potential;
    dispatcher_agent_funcs.mark_zombie_entry = mark_zombie_entry;
    dispatcher_agent_funcs.update_agent_filter = 0;
    dispatcher_agent_funcs.destroy = destroy;
    dispatcher_agent_funcs.agent_is_predictive = 0;
    dispatcher->register_narrow_agent(dispatcher_agent_funcs, PHYS_SHAPE_CAPSULE, PHYS_SHAPE_TRIANGLE);
}





// @address 0x68ab88 ?compute_sep_normal@phys_capsule_triangle_narrow_agent@@YAXABUphys_narrow_agent_input@@PAXAAVphys_vec4@@@Z
void phys_capsule_triangle_narrow_agent::compute_sep_normal(const phys_narrow_agent_input &input, void *data, phys_vec4 &normal)
{
    capsule_tri_agent_data *pair_agent_entry = (capsule_tri_agent_data *)data;
    const phys_capsule_shape *capsule = (const phys_capsule_shape *)input.m_body_a->get_shape();
    const phys_triangle_shape *triangle = (const phys_triangle_shape *)input.m_body_b->get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(input.m_body_a->get_transform(), capsule->get_vertices(), 2, segment_end_points);
    phys_vec4_util::transform_points(input.m_body_b->get_transform(), triangle->get_vertices(), 3, tri_vertices);
    phys_contact_point points[3];
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule->get_convex_radius(), tri_vertices, triangle->get_convex_radius(),
        pair_agent_entry->tri_dist_cache, 3.40282e+38f, 0, points);
    const phys_vec4 *selected = &points[0].m_separating_normal;
    if (!(points[0].m_separating_normal(3) < points[1].m_separating_normal(3))) selected = &points[1].m_separating_normal;
    normal.m_simd_quad = selected->get_quad();
}




// @address 0x68ad80 ?process@phys_capsule_triangle_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
void *phys_capsule_triangle_narrow_agent::process(
    const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *data, phys_vec4 *simplex_separating_normal,
        phys_process_collision_output &output)
{
    capsule_tri_agent_data *pair_agent_entry = (capsule_tri_agent_data *)data;
    int ray_contact_count = 0;
    const phys_capsule_shape *capsule = (const phys_capsule_shape *)input.m_body_a->get_shape();
    const phys_triangle_shape *triangle = (const phys_triangle_shape *)input.m_body_b->get_shape();
    phys_vec4 segment_end_points[2];
    phys_vec4_util::transform_points(input.m_body_a->get_transform(), capsule->get_vertices(), 2, segment_end_points);
    phys_vec4 tri_vertices[3];
    phys_vec4_util::transform_points(input.m_body_b->get_transform(), triangle->get_vertices(), 3, tri_vertices);
    phys_contact_point points[3];
    int search_manifold = 1;
    phys_capsule_util_caps_vs_tri(segment_end_points, capsule->get_convex_radius(), tri_vertices, triangle->get_convex_radius(),
        pair_agent_entry->tri_dist_cache, 3.40282e+38f, search_manifold, points);
    const float point_dist0 = points[0].m_separating_normal(3);
    const float point_dist1 = points[1].m_separating_normal(3);
    int reference_idx;
    if (point_dist0 < point_dist1) {
        reference_idx = 0;
        simplex_separating_normal->m_simd_quad = points[0].m_separating_normal.get_quad();
    } else {
        reference_idx = 1;
        simplex_separating_normal->m_simd_quad = points[1].m_separating_normal.get_quad();
    }
    for (int p = 0; p < 3; p++) {
        unsigned short heightfield_contact_id = pair_agent_entry->contact_ids[p];
        if (points[p].m_separating_normal(3) < input.m_input->m_box_tolerance) {
            if (heightfield_contact_id == 0xffff) {
                if (output.m_potential_contacts) {
                    phys_contact_manager *pair_contact_manager = input.m_contact_manager;
                    if (pair_contact_manager->reserve_contact_points(1) == 0) {
                        phys_process_collision_output::contact_ref &cr = *(output.m_potential_contacts->m_next_free_potential++);
                        cr.m_contact_point = output.m_next_free_contact;
                        cr.m_agent_data = data;
                        cr.m_agent_entry = entry;
                        goto export_contact_point;
                    }
                } else {
                    phys_contact_manager *pair_contact_manager = input.m_contact_manager;
                    heightfield_contact_id = pair_contact_manager->add_manifold_contact_point(*input.m_body_a, *input.m_body_b,
                        *input.m_input, output, 0, points[p]);
                }
            }
            if (heightfield_contact_id != 0xffff) {
export_contact_point:
                ray_contact_count++;
                phys_process_hit_point &point = *output.reserve_contact_points(1);
                output.commit_contact_points(1);
                point.m_hit_contact.m_position.m_simd_quad = points[p].m_position.get_quad();
                point.m_hit_contact.m_separating_normal.m_simd_quad = points[p].m_separating_normal.get_quad();
                point.m_process_contact_id = heightfield_contact_id;
                if (p == reference_idx && output.m_potential_contacts) {
                    *(output.m_potential_contacts->m_next_free_representative++) = &point;
                }
            }
        } else {
            if (heightfield_contact_id != 0xffff) {
                input.m_contact_manager->contact_remove_point(heightfield_contact_id, *output.m_constraint_owner);
                heightfield_contact_id = 0xffff;
            }
        }
        pair_agent_entry->contact_ids[p] = heightfield_contact_id;
    }
    entry->m_contact_point_count = (unsigned char)ray_contact_count;
    return phys_add_byte_offset(data, (sizeof(capsule_tri_agent_data) + 15) & ~15);
}

