// hkinternal/hkPredGskCylinderAgent3.obj: from the records 0x68b600 0x68b650 0x68b708 0x68b7b8 0x68b8c0 0x68b908 0x68c1c0
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x68b600 ?reset_simplex_manifold@@YAXPAUphys_simplex_manifold@@PAUphys_agent_entry@@PAVphys_contact_manager@@AAVphys_constraint_owner@@@Z
// @address 0x68b8c0 ?create@phys_pred_simplex_cylinder_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
#include <string.h>
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_continuous_simplex.h"
#include "phys/phys_simplex_agent_util.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_math.h"
#include "phys/phys_base_types.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_object.h"
#include "phys/phys_simplex_manifold.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_cylinder_shape.h"
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_pred_simplex_narrow_agent.h"
#include "phys/phys_pred_simplex_cylinder_narrow_agent.h"
// @address 0x68c1c0 ?register_narrow_agent@phys_pred_simplex_cylinder_narrow_agent@@YAXPAVphys_dispatcher@@@Z

class phys_ref_object;
#include "phys/phys_thread_memory.h"
class phys_thread_memory;
extern "C" {
}




// The first ten bytes of an hkGskCache as this agent reads them: four capsule-cache entries, one per shape type
// (two bytes each), then the shape type and the count of entries.
struct s_hkPredGskCylinderAgent3_cacheView {
    unsigned short entries[4];
    unsigned char type;
    unsigned char count;
};



class phys_rotation;
#include "phys/phys_base_types.h"
#include "phys/phys_collision_input.h"
class phys_contact_manager;
class phys_constraint_owner;
void reset_simplex_manifold(phys_simplex_manifold *box_manifold, phys_agent_entry *entry,
    phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner)
{
    phys_simplex_manifold_cleanup(box_manifold, agent_contact_manager, owner);
    entry->m_contact_point_count = box_manifold->m_contact_point_count;
    box_manifold->init();
}

static __forceinline unsigned short phys_pred_simplex_cyl_capsule_entry(unsigned short entry)
{
    return ((1 - ((entry >> 7) & 1)) << 4) & 0xfff0;
}
// @address 0x68b650 ?cylinder_to_capsule_cache@@YAXAAVphys_simplex_cache@@PAH@Z
__forceinline void cylinder_to_capsule_cache(phys_simplex_cache &cache, int *flags)
{
    s_hkPredGskCylinderAgent3_cacheView *view = reinterpret_cast<s_hkPredGskCylinderAgent3_cacheView *>(&cache);
    if (flags[0] != 0) {
        view->entries[0] = phys_pred_simplex_cyl_capsule_entry(view->entries[0]);
        if (view->type > 1) {
            view->entries[0] = 16;
            view->entries[1] = 0;
            if (view->type == 3) {
                unsigned short entry = view->entries[3];
                view->type = 2;
                view->entries[2] = entry;
            }
        }
    }
    if (flags[1] == 0)
        return;
    view->entries[view->type] = phys_pred_simplex_cyl_capsule_entry(view->entries[view->type]);
    if (view->count <= 1)
        return;
    view->entries[view->type] = 16;
    view->entries[view->type + 1] = 0;
    if (view->count != 3)
        return;
    view->count = 2;
}

// @address 0x68b708 ?restore_original_shapes_in_proxies@@YAXPAPAV?$phys_padded@PBVphys_collision_proxy@@@@PAPBVphys_collision_proxy@@@Z
__forceinline void restore_original_shapes_in_proxies(phys_padded<const phys_collision_proxy *> **bodies, const phys_collision_proxy **saved_bodies)
{
    for (int i = 0; i < 2; ++i) {
        if (saved_bodies[i]) {
            const phys_collision_proxy *body = *bodies[i];
            body->m_proxy_shape->object_remove_ref();
            *bodies[i] = saved_bodies[i];
        }
    }
}

// @address 0x68b7b8 ?replace_cylinders_with_capsules_in_proxies@@YAXPAPAV?$phys_padded@PBVphys_collision_proxy@@@@PAHPAVphys_collision_proxy@@PAPBV2@@Z
__forceinline void replace_cylinders_with_capsules_in_proxies(
    phys_padded<const phys_collision_proxy *> **proxy_slots, int *capsule_replaced, phys_collision_proxy *temp_proxies,
        const phys_collision_proxy **saved_proxies)
{
    for (int i = 0; i < 2; i++) {
        if (capsule_replaced[i]) {
            const phys_cylinder_shape *cylinder = static_cast<const phys_cylinder_shape *>((*proxy_slots[i])->get_shape());
            phys_capsule_shape *capsule = new phys_capsule_shape(
                cylinder->get_vertex(0), cylinder->get_vertex(1), cylinder->get_cylinder_radius() + cylinder->get_convex_radius() + 0.002f);
            new (&temp_proxies[i]) phys_collision_proxy((*proxy_slots[i]), (*proxy_slots[i])->get_motion_state());
            temp_proxies[i].set_proxy_shape(capsule);
            saved_proxies[i] = (*proxy_slots[i]).val();
            (*proxy_slots[i]) = &temp_proxies[i];
        } else {
            saved_proxies[i] = 0;
        }
    }
}

void *phys_pred_simplex_cylinder_narrow_agent::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data)
{
    static_cast<phys_simplex_agent_entry *>(entry)->m_user_data = 0;
    if (input.m_body_a->get_shape()->get_type_id() == 4)
        static_cast<phys_simplex_agent_entry *>(entry)->m_user_data |= 1;
    if (input.m_body_b->get_shape()->get_type_id() == 4)
        static_cast<phys_simplex_agent_entry *>(entry)->m_user_data |= 2;
    return phys_pred_simplex_narrow_agent::create(input, entry, data);
}


static __forceinline void phys_pred_simplex_cyl_manifold_init(const phys_simplex_manifold &box_manifold, const phys_vec4 &master_normal,
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
    {
        return;
    }

    const unsigned short *vertex_ids = box_manifold.get_vertex_ids();

    penetration_shape_a->vertex_ids_to_vertices(vertex_ids, box_manifold.m_vert_count_a, &work.m_vertices[0]);
    phys_vec4_util::transform_points(cA.get_transform(), &work.m_vertices[0], box_manifold.m_vert_count_a, &work.m_vertices[0]);

    int b = box_manifold.m_vert_count_a;
    phys_hull_vertex *verts = &work.m_vertices[b];
    penetration_shape_b->vertex_ids_to_vertices(vertex_ids + b, box_manifold.m_vert_count_b, verts);
    phys_vec4_util::transform_points(cB.get_transform(), verts, box_manifold.m_vert_count_b, verts);
}

static __forceinline int compute_cylinder_toi(const phys_narrow_agent_process_input &input, float dist_at_t1,
    phys_simplex_cache *simplex_cache, phys_vec4 *simplex_separating_normal, phys_process_collision_output &output)
{
    const phys_collision_quality_info &qi = *input.m_input->m_quality_info;
    if (qi.m_use_ccd)
    {
        const float allowed_penetration_a = input.m_body_a->get_root_collider()->m_allowed_penetration_depth;
        const float allowed_penetration_b = input.m_body_b->get_root_collider()->m_allowed_penetration_depth;
        const float allowed_penetration_depth = phys_math::min_of_two(allowed_penetration_a, allowed_penetration_b);

        const float distance = (*simplex_separating_normal)(3);
        const float min_separation = phys_math::min_of_two(qi.m_min_separation * allowed_penetration_depth,
            distance + qi.m_min_extra_separation * allowed_penetration_depth);
        if (dist_at_t1 >= min_separation)
        {
            return false;
        }
        const float toi_separation = phys_math::min_of_two(qi.m_toi_separation * allowed_penetration_depth,
            distance + qi.m_toi_extra_separation * allowed_penetration_depth);

        phys_4d_simplex_collide_toi(input, allowed_penetration_depth, min_separation, toi_separation, *simplex_cache, *simplex_separating_normal, output);
        return true;
    }
    return false;

}

// @address 0x68b908 ?process@phys_pred_simplex_cylinder_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
void *phys_pred_simplex_cylinder_narrow_agent::process(
    const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data, phys_vec4 *simplex_separating_normal,
        phys_process_collision_output &output)
{
    float dist_at_t1 = input.m_dist_at_t1;

    phys_simplex_cache *simplex_cache = reinterpret_cast<phys_simplex_cache *>(agent_data);
    void *next_agent_data = simplex_cache + 1;
    phys_simplex_manifold *simplex_manifold = reinterpret_cast<phys_simplex_manifold *>(next_agent_data);

    phys_padded<const phys_collision_proxy *> *proxy_slots[2] = { const_cast<phys_padded<const phys_collision_proxy *> *>(&input.m_body_a),
                                                const_cast<phys_padded<const phys_collision_proxy *> *>(&input.m_body_b) };

    int is_cylinder_side[2] = { static_cast<phys_simplex_agent_entry *>(entry)->m_user_data & 1,
        static_cast<phys_simplex_agent_entry *>(entry)->m_user_data & 2 };
    int capsule_replaced[2] = { static_cast<phys_simplex_agent_entry *>(entry)->m_user_data & 4,
        static_cast<phys_simplex_agent_entry *>(entry)->m_user_data & 8 };

    const phys_collision_proxy *saved_proxies[2] = { 0, 0 };
    phys_collision_proxy temp_proxies[2];

    phys_vec4 cylinder_sep_normal;
    phys_pred_simplex_narrow_agent::compute_sep_normal(input, agent_data, cylinder_sep_normal);

    for (int i = 0; i < 2; i++)
    {
        if (is_cylinder_side[i])
        {
            const phys_cylinder_shape *cylinder = static_cast<const phys_cylinder_shape *>((*proxy_slots[i])->get_shape());
            phys_vec4 axis;
            axis.set_sub4(cylinder->get_vertex(1), cylinder->get_vertex(0));
            axis.normalize_xyz();
            axis.vec_set_rotated_dir((*proxy_slots[i])->get_transform().get_rotation(), axis);
            float cos = phys_math::fabs(axis.dot3(cylinder_sep_normal));

            if (capsule_replaced[i])
            {
                if (cos > 0.17f)
                {
                    capsule_replaced[i] = 0;
                    static_cast<phys_simplex_agent_entry *>(entry)->m_user_data &= ~(4 << i);
                    reset_simplex_manifold(simplex_manifold, entry, input.m_contact_manager, *output.m_constraint_owner.val());
                }
            }
            else
            {
                if (cos < 0.085f)
                {
                    capsule_replaced[i] = 1;
                    static_cast<phys_simplex_agent_entry *>(entry)->m_user_data |= (4 << i);
                    reset_simplex_manifold(simplex_manifold, entry, input.m_contact_manager, *output.m_constraint_owner.val());
                }
            }
        }
    }

    do
    {
        const phys_collision_quality_info &qi = *input.m_input->m_quality_info;
        if (!compute_cylinder_toi(input, dist_at_t1, simplex_cache, simplex_separating_normal, output))
        {
            if (dist_at_t1 > qi.m_manifold_sweep_dist)
            {
                (*simplex_separating_normal)(3) = dist_at_t1;
                if (simplex_manifold->m_contact_point_count)
                {
                    replace_cylinders_with_capsules_in_proxies(proxy_slots, capsule_replaced, temp_proxies, saved_proxies);

                    phys_simplex_manifold_work work;
                    phys_pred_simplex_cyl_manifold_init(*simplex_manifold, *simplex_separating_normal, *input.m_body_a,
                        *input.m_body_b, input.m_input->get_tolerance(), work);
                    phys_simplex_manifold_verify_get_points(*simplex_manifold, work, 0, output, input.m_contact_manager);
                    if (simplex_manifold->m_contact_point_count && output.m_potential_contacts)
                    {
                        *(output.m_potential_contacts->m_next_free_representative++) = output.m_next_free_contact - simplex_manifold->m_contact_point_count;
                    }
                }
                break;
            }
        }

        replace_cylinders_with_capsules_in_proxies(proxy_slots, capsule_replaced, temp_proxies, saved_proxies);
        {
            phys_simplex_cache *capsule_simplex_cache = simplex_cache;
            phys_simplex_cache scratch_simplex_cache;
            if (capsule_replaced[0] || capsule_replaced[1])
            {
                capsule_simplex_cache = &scratch_simplex_cache;
                scratch_simplex_cache = *simplex_cache;
                cylinder_to_capsule_cache(scratch_simplex_cache, capsule_replaced);
            }

            phys_simplex_util_process_no_sweep(input, entry, agent_data, *capsule_simplex_cache, *simplex_manifold, *simplex_separating_normal, output);
        }
        *simplex_separating_normal = cylinder_sep_normal;

    } while (false);
    restore_original_shapes_in_proxies(proxy_slots, saved_proxies);

    entry->m_contact_point_count = simplex_manifold->m_contact_point_count;

    return reinterpret_cast<char *>(agent_data) + ((sizeof(phys_simplex_cache) + simplex_manifold->get_total_size_in_bytes() + 15) & ~15);
}
void phys_pred_simplex_cylinder_narrow_agent::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs functions;
    functions.create = phys_pred_simplex_cylinder_narrow_agent::create;
    functions.process = phys_pred_simplex_cylinder_narrow_agent::process;
    functions.compute_sep_normal = phys_pred_simplex_narrow_agent::compute_sep_normal;
    functions.cleanup = phys_pred_simplex_narrow_agent::cleanup;
    functions.bridge_remove_point = phys_pred_simplex_narrow_agent::bridge_remove_point;
    functions.commit_potential = phys_pred_simplex_narrow_agent::commit_potential;
    functions.mark_zombie_entry = phys_pred_simplex_narrow_agent::mark_zombie_entry;
    functions.destroy = phys_pred_simplex_narrow_agent::destroy;
    functions.agent_is_predictive = true;
    functions.symmetric = true;
    dispatcher->register_narrow_agent(functions, PHYS_SHAPE_CYLINDER, PHYS_SHAPE_CONVEX);
    dispatcher->register_narrow_agent(functions, PHYS_SHAPE_CONVEX, PHYS_SHAPE_CYLINDER);
}

