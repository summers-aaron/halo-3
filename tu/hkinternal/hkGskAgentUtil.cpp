// hkinternal/hkGskAgentUtil.obj: from the records 0x69f0c8
// @compiler xdk5632
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"

#include <vectorintrinsics.h>
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_simplex_agent_util.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_math.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_object.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_simplex_manifold.h"
#include "phys/phys_simplex_types.h"

class phys_transform;
class phys_statistics_collector;

class phys_constraint_owner;
class phys_contact_manager;

static __forceinline void phys_simplex_util_init_manifold(const phys_simplex_manifold &box_manifold, const phys_vec4 &master_normal,
    const phys_collision_proxy &cA, const phys_collision_proxy &cB, float keep_contact_max_dist, phys_simplex_manifold_work &work)
{
    work.m_keep_contact = keep_contact_max_dist;
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(cA.get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(cB.get_shape());
    work.m_radius_a = penetration_shape_a->get_convex_radius();
    work.m_radius_b = penetration_shape_b->get_convex_radius();
    const float max_dist_vec = work.m_keep_contact + work.m_radius_a + work.m_radius_b;
    work.m_radius_sum_sq = max_dist_vec * max_dist_vec;
    work.m_master_normal = master_normal;
    if (!box_manifold.m_contact_point_count)
        return;
    const unsigned short *vertex_ids = box_manifold.get_vertex_ids();
    penetration_shape_a->vertex_ids_to_vertices(vertex_ids, box_manifold.m_vert_count_a, &work.m_vertices[0]);
    phys_vec4_util::transform_points(cA.get_transform(), &work.m_vertices[0], box_manifold.m_vert_count_a, &work.m_vertices[0]);
    int b = box_manifold.m_vert_count_a;
    phys_hull_vertex *verts = &work.m_vertices[b];
    penetration_shape_b->vertex_ids_to_vertices(vertex_ids + b, box_manifold.m_vert_count_b, verts);
    phys_vec4_util::transform_points(cB.get_transform(), verts, box_manifold.m_vert_count_b, verts);
}

// @address 0x69f0c8 ?phys_simplex_util_process_no_sweep@@YAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAXAAVphys_simplex_cache@@AAUphys_simplex_manifold@@AAVphys_vec4@@AAUphys_process_collision_output@@@Z
void phys_simplex_util_process_no_sweep(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data,
    phys_simplex_cache &simplex_cache, phys_simplex_manifold &simplex_manifold, phys_vec4 &simplex_separating_normal, phys_process_collision_output &output)
{
    phys_vec4 point_on_b;
    phys_simplex::closest_point_input simplex_input;
    simplex_input.m_shape_a = static_cast<const phys_convex_shape *>(input.m_body_a->get_shape());
    simplex_input.m_shape_b = static_cast<const phys_convex_shape *>(input.m_body_b->get_shape());
    simplex_input.m_a_to_b = &input.m_a_to_b;
    simplex_input.m_transform_a = &input.m_body_a->get_transform();
    simplex_input.m_collision_tolerance = input.m_input->get_tolerance();
    static_cast<phys_simplex_agent_entry *>(entry)->m_disable_contact_sweeps = 0;
    if (phys_simplex::get_closest_point(simplex_input, simplex_cache, simplex_separating_normal, point_on_b) == PHYS_FAILURE) {
        if (simplex_manifold.m_contact_point_count)
            phys_simplex_manifold_cleanup(&simplex_manifold, input.m_contact_manager, *output.m_constraint_owner.val());
    } else {
        phys_vec4 welded_normal = simplex_separating_normal;
        if (simplex_input.m_shape_b->get_type_id() == 5) {
            int simplex_weld_result = simplex_input.m_shape_b->weld_contact_point(&simplex_cache.m_vertices[simplex_cache.m_dim_a],
                simplex_cache.m_dim_b, point_on_b, &input.m_body_b->get_transform(), simplex_input.m_shape_a,
                &input.m_body_a->get_transform(), welded_normal);
            if (simplex_weld_result == 0) {
                phys_simplex_manifold_cleanup(&simplex_manifold, input.m_contact_manager, *output.m_constraint_owner.val());
                return;
            } else if (simplex_weld_result == 1) {
                static_cast<phys_simplex_agent_entry *>(entry)->m_disable_contact_sweeps = 1;
            }
        }
        int closest_manifold_point = phys_simplex_manifold_point_exists_resort(simplex_manifold, simplex_cache);
        phys_process_hit_point *result_point_array = output.get_track_end();
        if (int(simplex_manifold.m_contact_point_count) > closest_manifold_point) {
            phys_simplex_manifold_work work;
            phys_simplex_util_init_manifold(simplex_manifold, welded_normal, *input.m_body_a, *input.m_body_b,
                input.m_input->get_tolerance(), work);
            phys_simplex_manifold_verify_get_points(simplex_manifold, work, closest_manifold_point, output, input.m_contact_manager);
        }
        phys_process_hit_point *contact_hit = output.reserve_contact_points(1);
        contact_hit->m_hit_contact.get_contact_position() = point_on_b;
        contact_hit->m_hit_contact.set_separating_normal(welded_normal);
        if (closest_manifold_point) {
            contact_hit->m_process_contact_id = simplex_manifold.m_process_contact_points[0].m_handle_id;
            output.commit_contact_points(1);
        } else {
            const phys_collision_quality_info &sq = *input.m_input->m_quality_info;
            int simplex_dim = simplex_cache.m_dim_a + simplex_cache.m_dim_b;
            float create_range_max = (simplex_dim == 4) ? sq.m_create_4d_contact : sq.m_create_contact;
            if (simplex_separating_normal(3) < create_range_max) {
                int use_legacy_edge_weld = input.m_input->m_legacy_edge_weld ? 1 : 0;
                int add_status = phys_simplex_manifold_add_point(simplex_manifold, *input.m_body_a, *input.m_body_b,
                    *input.m_input, output, simplex_cache, contact_hit, result_point_array, input.m_contact_manager, use_legacy_edge_weld);
                if (add_status == 4) {
                    if (contact_hit->m_process_contact_id != 0xffff) {
                        output.commit_contact_points(1);
                    } else {
                        if (output.m_potential_contacts && agent_data) {
                            if (input.m_contact_manager->reserve_contact_points(1) == PHYS_SUCCESS) {
                                phys_process_collision_output::contact_ref &potential_contact_ref = *output.m_potential_contacts->m_next_free_potential++;
                                potential_contact_ref.m_contact_point = contact_hit;
                                potential_contact_ref.m_agent_entry = entry;
                                potential_contact_ref.m_agent_data = agent_data;
                            } else {
                                phys_simplex_manifold_remove_point(&simplex_manifold, 0);
                                add_status = 5;
                            }
                        } else {
                            contact_hit->m_process_contact_id = input.m_contact_manager->add_manifold_contact_point(*input.m_body_a,
                                *input.m_body_b, *input.m_input, output, &simplex_cache, contact_hit->m_hit_contact);
                            if (contact_hit->m_process_contact_id == 0xffff) {
                                phys_simplex_manifold_remove_point(&simplex_manifold, 0);
                                add_status = 5;
                            }
                            else
                                simplex_manifold.m_process_contact_points[0].m_handle_id = static_cast<unsigned short>(contact_hit->m_process_contact_id.val());
                        }
                        if (add_status == 4)
                            output.commit_contact_points(1);
                    }
                }
                if (add_status == 5) {
                    contact_hit = result_point_array;
                    output.abort_contact_points(1);
                } else if (add_status == 6) {
                    output.commit_contact_points(-1);
                    output.abort_contact_points(1);
                    contact_hit = result_point_array;
                } else if (add_status != 4) {
                    contact_hit = result_point_array + add_status;
                    output.abort_contact_points(1);
                }
            }
        }
        if (output.m_potential_contacts && contact_hit < output.m_next_free_contact.val()) {
            *output.m_potential_contacts->m_next_free_representative = contact_hit;
            output.m_potential_contacts->m_next_free_representative++;
        }
    }
}

