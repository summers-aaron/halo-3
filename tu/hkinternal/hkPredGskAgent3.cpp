// hkinternal/hkPredGskAgent3.obj: from the records 0x68c278 0x68c2b8 0x68c300 0x68c350 0x68c3f0 0x68c458 0x68c4a0 0x68c548 0x68c670 0x68ca58 0x68cac8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x68c278 ?bridge_remove_point@phys_pred_simplex_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68c2b8 ?commit_potential@phys_pred_simplex_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68c300 ?mark_zombie_entry@phys_pred_simplex_narrow_agent@@YAXPAUphys_agent_entry@@PAXG@Z
// @address 0x68c350 ?create@phys_pred_simplex_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x68c3f0 ?cleanup@phys_pred_simplex_narrow_agent@@YAPAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@@Z
// @address 0x68c458 ?destroy@phys_pred_simplex_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x68c4a0 ?phys_pred_simplex_sep_normal@@YAXABVphys_collision_proxy@@0MAAVphys_simplex@@AAVphys_vec4@@@Z
// @address 0x68c548 ?compute_sep_normal@phys_pred_simplex_narrow_agent@@YAXABUphys_narrow_agent_input@@PAXAAVphys_vec4@@@Z
// @address 0x68c670 ?process@phys_pred_simplex_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
// @address 0x68ca58 ?init_agent_funcs@phys_pred_simplex_narrow_agent@@YAXAAUnarrow_agent_funcs@phys_dispatcher@@@Z
#include "phys/phys_base_types.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <stdlib.h>
#include <math.h>
#include "phys/phys_continuous_simplex.h"
#include "phys/phys_simplex_agent_util.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_simplex_manifold.h"
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_pred_simplex_narrow_agent.h"
#include "phys/phys_vec4_util.h"

class phys_contact_manager;
class phys_constraint_owner;
class phys_triangle_shape;


namespace phys_pred_simplex_narrow_agent {

void bridge_remove_point(phys_agent_entry *entry, void *agent_data, unsigned short remove_id)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);
    for (int i = 0; i < simplex_manifold->m_contact_point_count; i++) {
        if (simplex_manifold->m_process_contact_points[i].m_handle_id == remove_id) {
            phys_simplex_manifold_remove_point(simplex_manifold, i);
            return;
        }
    }
}

void commit_potential(phys_agent_entry *entry, void *agent_data, unsigned short commit_id)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);
    for (int i = 0; i < simplex_manifold->m_contact_point_count; i++) {
        if (simplex_manifold->m_process_contact_points[i].m_handle_id == 0xffff) {
            simplex_manifold->m_process_contact_points[i].m_handle_id = commit_id;
            return;
        }
    }
}

void mark_zombie_entry(phys_agent_entry *entry, void *agent_data, unsigned short convert_id)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);
    for (int i = 0; i < simplex_manifold->m_contact_point_count; i++) {
        phys_simplex_manifold::manifold_contact &cp = simplex_manifold->m_process_contact_points[i];
        if (cp.m_handle_id == convert_id) {
            cp.m_dim_a = 0;
            cp.m_dim_b = 0;
            return;
        }
    }
}

void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(input.m_body_a->get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(input.m_body_b->get_shape());
    if (penetration_shape_b->get_type_id() == PHYS_SHAPE_TRIANGLE)
        simplex_cache->init_triangle(penetration_shape_a, reinterpret_cast<const phys_triangle_shape *>(penetration_shape_b), input.m_a_to_b);
    else
        simplex_cache->init(penetration_shape_a, penetration_shape_b, input.m_a_to_b);
    entry->m_contact_point_count = 0;
    static_cast<phys_simplex_agent_entry *>(entry)->m_disable_contact_sweeps = 0;
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);
    simplex_manifold->init();
    int agent_data_bytes = sizeof(phys_simplex_cache) + simplex_manifold->get_total_size_in_bytes();
    return reinterpret_cast<char *>(agent_data) + ((agent_data_bytes + 15) & ~15);
}

void *cleanup(phys_agent_entry *entry, void *agent_data, phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);
    phys_simplex_manifold_cleanup(simplex_manifold, pair_contact_manager, constraint_owner_ref);
    int manifold_bytes = simplex_manifold->get_total_size_in_bytes();
    entry->m_contact_point_count = 0;
    return reinterpret_cast<char *>(agent_data) + ((sizeof(phys_simplex_cache) + manifold_bytes + 15) & ~15);
}

void destroy(phys_agent_entry *entry, void *agent_data, phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref,
    phys_dispatcher *dispatcher)
{
    cleanup(entry, agent_data, pair_contact_manager, constraint_owner_ref);
}

}

void phys_pred_simplex_sep_normal(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
                                        float tolerance, phys_simplex &gsk, phys_vec4 &normal_out)
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

static __forceinline float phys_pred_simplex_narrow_min2(float x, float y)
{
    return (float)__fsel(x - y, y, x);
}

static __forceinline void phys_pred_simplex_narrow_manifold_init(const phys_simplex_manifold &box_manifold,
    const phys_vec4 &master_normal, const phys_collision_proxy &cA,
    const phys_collision_proxy &cB, float keep_contact_max_dist, phys_simplex_manifold_work &work)
{
    work.m_keep_contact = keep_contact_max_dist;
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(static_cast<const void *>(cA.get_shape()));
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(static_cast<const void *>(cB.get_shape()));
    work.m_radius_a = penetration_shape_a->get_convex_radius();
    work.m_radius_b = penetration_shape_b->get_convex_radius();
    const float max_dist_vec = work.m_keep_contact + work.m_radius_a + work.m_radius_b;
    work.m_radius_sum_sq = max_dist_vec * max_dist_vec;
    work.m_master_normal.m_simd_quad = master_normal.m_simd_quad;

    const unsigned short *vertex_ids = box_manifold.get_vertex_ids();

    penetration_shape_a->vertex_ids_to_vertices(vertex_ids, box_manifold.m_vert_count_a, &work.m_vertices[0]);
    phys_vec4_util::transform_points(cA.get_transform(), &work.m_vertices[0], box_manifold.m_vert_count_a, &work.m_vertices[0]);

    int b = box_manifold.m_vert_count_a;
    phys_hull_vertex *verts = &work.m_vertices[b];
    penetration_shape_b->vertex_ids_to_vertices(vertex_ids + b, box_manifold.m_vert_count_b, verts);
    phys_vec4_util::transform_points(cB.get_transform(), verts, box_manifold.m_vert_count_b, verts);
}

namespace phys_pred_simplex_narrow_agent {

void compute_sep_normal(const phys_narrow_agent_input &input, void *agent_data, phys_vec4 &separating_normal_out)
{
    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    const phys_convex_shape *penetration_shape_a = static_cast<const phys_convex_shape *>(input.m_body_a->get_shape());
    const phys_convex_shape *penetration_shape_b = static_cast<const phys_convex_shape *>(input.m_body_b->get_shape());
    phys_simplex gsk;
    gsk.init(penetration_shape_a, penetration_shape_b, *simplex_cache);
    phys_vec4 simplex_separating_normal;
    gsk.get_closest_feature(penetration_shape_a, penetration_shape_b, input.m_a_to_b, simplex_separating_normal);
    gsk.refresh_simplex_cache(*simplex_cache);
    separating_normal_out.set_rotated_dir(input.m_body_a->get_transform().get_rotation(), simplex_separating_normal);
    separating_normal_out(3) = simplex_separating_normal(3) - penetration_shape_a->get_convex_radius() - penetration_shape_b->get_convex_radius();
}

void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data, phys_vec4 *simplex_separating_normal,
    phys_process_collision_output &output)
{
    float dist_at_t1 = input.m_dist_at_t1;

    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    phys_simplex_manifold &simplex_manifold = *reinterpret_cast<phys_simplex_manifold *>(simplex_cache + 1);

    const phys_collision_quality_info &qi = *input.m_input->m_quality_info;
    if (qi.m_use_ccd)
    {
        const float allowed_penetration_a = input.m_body_a->get_root_collider()->m_allowed_penetration_depth;
        const float allowed_penetration_b = input.m_body_b->get_root_collider()->m_allowed_penetration_depth;
        const float allowed_penetration_depth = phys_pred_simplex_narrow_min2(allowed_penetration_a, allowed_penetration_b);

        const float distance = (*simplex_separating_normal)(3);
        const float min_separation = phys_pred_simplex_narrow_min2(
            qi.m_min_separation * allowed_penetration_depth, distance + qi.m_min_extra_separation * allowed_penetration_depth);
        if (dist_at_t1 < min_separation)
        {
            const float toi_separation = phys_pred_simplex_narrow_min2(
                qi.m_toi_separation * allowed_penetration_depth, distance + qi.m_toi_extra_separation * allowed_penetration_depth);
            phys_4d_simplex_collide_toi(input, allowed_penetration_depth, min_separation, toi_separation, *simplex_cache, *simplex_separating_normal, output);
            goto process_step;
        }
    }

    if (dist_at_t1 > qi.m_manifold_sweep_dist && !static_cast<phys_simplex_agent_entry *>(entry)->m_disable_contact_sweeps)
    {
        (*simplex_separating_normal)(3) = dist_at_t1;
        if (simplex_manifold.m_contact_point_count)
        {
            phys_simplex_manifold_work work;
            phys_pred_simplex_narrow_manifold_init(simplex_manifold, *simplex_separating_normal, *input.m_body_a, *input.m_body_b,
                input.m_input->get_tolerance(), work);
            phys_simplex_manifold_verify_get_points(simplex_manifold, work, 0, output, input.m_contact_manager);
            if (simplex_manifold.m_contact_point_count && output.m_potential_contacts)
            {
                *(output.m_potential_contacts->m_next_free_representative++) = output.m_next_free_contact - simplex_manifold.m_contact_point_count;
            }
        }
    }
    else
    {
process_step:
        phys_simplex_util_process_no_sweep(input, entry, simplex_cache, *simplex_cache, simplex_manifold, *simplex_separating_normal, output);

    }
    entry->m_contact_point_count = simplex_manifold.m_contact_point_count;
    return reinterpret_cast<char *>(agent_data) + ((sizeof(phys_simplex_cache) + simplex_manifold.get_total_size_in_bytes() + 15) & ~15);
}

void init_agent_funcs(phys_dispatcher::narrow_agent_funcs &functions)
{
    functions.create = create;
    functions.process = process;
    functions.compute_sep_normal = compute_sep_normal;
    functions.cleanup = cleanup;
    functions.bridge_remove_point = bridge_remove_point;
    functions.commit_potential = commit_potential;
    functions.mark_zombie_entry = mark_zombie_entry;
    functions.destroy = destroy;
    functions.agent_is_predictive = 1;
}

// @address 0x68cac8 ?register_narrow_agent@phys_pred_simplex_narrow_agent@@YAXPAVphys_dispatcher@@W4phys_shape_type@@1@Z
void register_narrow_agent(phys_dispatcher *dispatcher, phys_shape_type shape_type_a, phys_shape_type shape_type_b)
{
    phys_dispatcher::narrow_agent_funcs dispatcher_agent_funcs;
    init_agent_funcs(dispatcher_agent_funcs);
    dispatcher->register_narrow_agent(dispatcher_agent_funcs, shape_type_a, shape_type_b);
}

}

