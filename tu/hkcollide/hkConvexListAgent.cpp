// hkcollide/hkConvexListAgent.obj: from the records 0x62e580 0x632428 0x632440 0x632458 0x632470 0x632488 0x6324a0 0x6324f0 0x632560 0x6325d0 0x632660 0x6327a0 0x6327b8 0x632830 0x6328b0 0x632930 0x6329b8 0x632a38 0x632f40 0x6330b8 0x633248 0x633468 0x633568 0x633580 0x633598 0x6335b0 0x6335c8 0x6335e0 0x633630 0x633680 0x633768 0x633820 0x633990 0x633a48 0x633b60 0x633cd8 0x633e78 0x633f08 0x633f58 0x634040 0x6340b0 0x634100 0x634148 0x634238
// @flags /TP /O2
// Stack vector arrays use trivial construction.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_collide.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "phys/phys_collectors.h"
#include "phys/phys_simplex_manifold_util.h"
#include "phys/phys_object.h"
#include "phys/phys_convex_list_filter.h"
#include "phys/phys_batch_agent_visitor_input.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_swept_transform_util_sweep.h"
#include "phys/phys_flag_proxy_pair_collector.h"
#include "phys/phys_convex_list_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_pred_simplex_agent.h"
#include "phys/phys_simple_closest_collector.h"
#include "phys/phys_closest_hit_collector.h"
#include "phys/phys_compound_shape_agent.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_simplex_types.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_scalar_types.h"
#include <string.h>
#include "phys/phys_step_info.h"
// @address 0x62e580 ??_G?$phys_symmetric_agent@Vphys_convex_list_agent@@@@UAAPAXI@Z
// @address 0x633768 ??_Gphys_closest_hit_collector@@UAAPAXI@Z
// @address 0x633820 ??_Gphys_flag_proxy_pair_collector@@UAAPAXI@Z
// @address 0x633990 ??0phys_process_collision_output_backup@@QAA@ABUphys_process_collision_output@@@Z
class phys_subshape_contact_manager : public phys_contact_manager {
public:
    __forceinline phys_subshape_contact_manager(phys_contact_manager *pair_contact_manager)
        : phys_contact_manager(phys_contact_manager::CONTACT_MANAGER_CONVEX_LIST)
    {
        m_contact_manager = pair_contact_manager;
        m_invalid_point = false;
    }
    virtual unsigned short add_contact_point_impl(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &, const phys_simplex_cache *, phys_contact_point &);
    virtual void cleanup();
    virtual phys_result reserve_contact_points_impl(int count);
    virtual void remove_contact_point_impl(unsigned short sphere_contact_id, phys_constraint_owner &owner);
    virtual void process_contact_impl(const phys_collider &, const phys_collider &, const phys_process_collision_input &, phys_process_collision_data &);
    virtual toi_verdict add_toi_impl(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &, float,
        phys_contact_point &, const phys_simplex_cache *, float &, phys_contact_point_material &);
    virtual void remove_toi_impl(const phys_collider &, const phys_collider &, phys_constraint_owner &, phys_contact_point_material &);
    phys_contact_manager *m_contact_manager;
    signed char m_invalid_point;
};
// A convex shape made of convex children, also its own child container (hkConvexListShape.obj).
class phys_convex_list_shape : public phys_convex_shape, public phys_shape_container {
public:
    float m_min_hull_query_dist;
protected:
    phys_vec4 m_aabb_half_extents;
    phys_vec4 m_aabb_center;
    bool m_use_cached_aabb;
public:
    phys_array<const phys_convex_shape *> m_child_shapes;
};
class phys_process_collision_output_backup {
public:
    phys_process_hit_point *m_next_free_contact;
    phys_process_collision_output::potential_info m_potential_contacts;
    phys_process_collision_data::toi_info m_collision_toi;
    phys_process_collision_output_backup(const phys_process_collision_output &output);
    void rollback_output(const phys_collision_proxy &, const phys_collision_proxy &, phys_process_collision_output &, phys_contact_manager *);
};







// @address 0x632428 ?invalidate_sweep_info@phys_convex_list_agent@@UAAXAAUphys_collision_input@@@Z
void phys_convex_list_agent::invalidate_sweep_info(phys_collision_input &input)
{
    if (m_in_simplex_mode) { phys_simplex_agent_base::invalidate_sweep_info(input); }
    else { phys_batch_agent_invalidate_sweep(&get_stream().m_agent_track, &input); }
}
// @address 0x632440 ?warp_time@phys_convex_list_agent@@UAAXMMAAUphys_collision_input@@@Z
void phys_convex_list_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    if (m_in_simplex_mode) { phys_simplex_agent_base::warp_time(agent_old_time, agent_new_time, input); }
    else { phys_batch_agent_warp_time(&get_stream().m_agent_track, agent_old_time, agent_new_time, &input); }
}
// @address 0x632458 ?bridge_remove_point@phys_convex_list_agent@@UAAXG@Z
void phys_convex_list_agent::bridge_remove_point(unsigned short bounding_contact_point_id)
{
    if (m_in_simplex_mode) { phys_simplex_agent::bridge_remove_point(bounding_contact_point_id); }
}
// @address 0x632470 ?commit_potential@phys_convex_list_agent@@UAAXG@Z
void phys_convex_list_agent::commit_potential(unsigned short bounding_contact_point_id)
{
    if (m_in_simplex_mode) { phys_simplex_agent::commit_potential(bounding_contact_point_id); }
}
// @address 0x632488 ?mark_zombie_entry@phys_convex_list_agent@@UAAXG@Z
void phys_convex_list_agent::mark_zombie_entry(unsigned short bounding_contact_point_id)
{
    if (m_in_simplex_mode) { phys_simplex_agent::mark_zombie_entry(bounding_contact_point_id); }
}
// @address 0x6324a0 ?switch_to_simplex_mode@phys_convex_list_agent@@IAAXAAVphys_constraint_owner@@@Z
void phys_convex_list_agent::switch_to_simplex_mode(phys_constraint_owner &owner)
{
    phys_batch_agent_destroy(get_stream().m_agent_track, m_agent_dispatcher, m_contact_manager, owner);
    m_box_manifold.init();
    m_in_simplex_mode = true;
}
// @address 0x6324f0 ?cleanup@phys_convex_list_agent@@UAAXAAVphys_constraint_owner@@@Z
void phys_convex_list_agent::cleanup(phys_constraint_owner &owner)
{
    if (m_in_simplex_mode) { phys_simplex_manifold_cleanup(&m_box_manifold, m_contact_manager, owner); }
    else { phys_batch_agent_destroy(get_stream().m_agent_track, m_agent_dispatcher, m_contact_manager, owner); }
    delete this;
}
// @address 0x632560 ?update_compound_filter@phys_convex_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_convex_list_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    if (!m_in_simplex_mode) {
        phys_batch_agent_visitor_input visitor;
        visitor.m_body_a = &agent_body_a;
        visitor.m_compound_proxy_b = &agent_body_b;
        visitor.m_input = &input;
        visitor.m_contact_manager = m_contact_manager;
        visitor.m_constraint_owner = &owner;
        visitor.m_container_shape_b = agent_body_b.get_shape()->get_shape_container();
        phys_batch_agent_update_compound_filter(get_stream().m_agent_track, visitor);
    }
}
// @address 0x6325d0 ??0phys_convex_list_agent@@IAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_convex_list_agent::phys_convex_list_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager) :
    phys_simplex_agent(agent_body_a, agent_body_b, pair_contact_manager)
{
    m_agent_dispatcher = input.m_agent_dispatcher;
    m_in_simplex_mode = true;
    m_process_called = false;
    const phys_convex_shape *penetration_shape_a = (const phys_convex_shape *)agent_body_a.get_shape();
    const phys_convex_shape *penetration_shape_b = (const phys_convex_shape *)agent_body_b.get_shape();
    phys_transform list_b_to_a;
    list_b_to_a.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
    m_cache.init(penetration_shape_a, penetration_shape_b, list_b_to_a);
    m_separating_normal(3) = -1.0f;
    m_separating_normal_time = -1.0f;
}
// @address 0x632660 ?static_closest_points@phys_convex_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_convex_list_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    {
        phys_flag_proxy_pair_collector union_find_checker;
        phys_simplex_agent_base::static_penetrations(agent_body_a, agent_body_b, input, union_find_checker);
        if (union_find_checker.has_hit()) {
            phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_closest_points(agent_body_a, agent_body_b, input, collector);
        } else {
            phys_closest_hit_collector closest_hit_collector;
            phys_simplex_agent_base::static_closest_points(agent_body_a, agent_body_b, input, closest_hit_collector);
            if (closest_hit_collector.has_hit()) {
                const phys_convex_list_shape *list_shape_b = reinterpret_cast<const phys_convex_list_shape *>(agent_body_b.get_shape());
                if (closest_hit_collector.get_hit_contact().get_distance() > list_shape_b->m_min_hull_query_dist) {
                    phys_hit_point hit(agent_body_a, agent_body_b);
                    hit.m_hit_contact = closest_hit_collector.get_hit_contact();
                    collector.add_hit(hit);
                } else {
                    phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_closest_points(agent_body_a, agent_body_b, input, collector);
                }
            }
        }
    }
}
// @address 0x6327a0 ?get_closest_points@phys_convex_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_convex_list_agent::get_closest_points(const phys_collision_proxy &a, const phys_collision_proxy &b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    static_closest_points(a, b, input, collector);
}
static __forceinline void phys_convex_list_agent_penetrations_body(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_flag_proxy_pair_collector hit_pair_collector;
    phys_simplex_agent_base::static_penetrations(agent_body_a, agent_body_b, input, hit_pair_collector);
    if (hit_pair_collector.has_hit()) {
        phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_penetrations(agent_body_a, agent_body_b, input, collector);
    }
}
static __forceinline void phys_convex_list_agent_flip_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_compound_shape_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x6327b8 ?static_penetrations@phys_convex_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_convex_list_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_flag_proxy_pair_collector flag_pair_collector;
    phys_simplex_agent_base::static_penetrations(agent_body_a, agent_body_b, input, flag_pair_collector);
    if (flag_pair_collector.has_hit()) {
        phys_convex_list_agent_flip_penetrations(agent_body_a, agent_body_b, input, collector);
    }
}
// @address 0x632830 ?get_penetrations@phys_convex_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_convex_list_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_convex_list_agent_penetrations_body(agent_body_a, agent_body_b, input, collector);
}
// @address 0x6328b0 ?static_sweep_cast@phys_convex_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_convex_list_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_simple_closest_collector closest_contact_hit;
    phys_iterative_linear_cast_agent::static_sweep_cast(agent_body_a, agent_body_b, input, closest_contact_hit, &closest_contact_hit);
    if (closest_contact_hit.has_hit()) {
        phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
    }
}
// @address 0x632930 ?sweep_cast@phys_convex_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// Process the linear cast with a closest-contact collector.
void phys_convex_list_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_simple_closest_collector closest_contact_hit;
    phys_iterative_linear_cast_agent::static_sweep_cast(agent_body_a, agent_body_b, input, closest_contact_hit, &closest_contact_hit);
    if (closest_contact_hit.has_hit()) {
        phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
    }
}
// @address 0x6329b8 ?switch_to_stream_mode@phys_convex_list_agent@@IAAXAAVphys_constraint_owner@@@Z
void phys_convex_list_agent::switch_to_stream_mode(phys_constraint_owner &owner)
{
    phys_simplex_manifold_cleanup(&m_box_manifold, m_contact_manager, owner);
    m_in_simplex_mode = false;
    new (&get_stream()) phys_batch_agent_track;
    phys_batch_agent_create(&get_stream().m_agent_track);
    m_stream_mode_counter = 25;
    get_stream().m_stream_mode_sweep_dist = 0.0f;
}
namespace phys_math {  }
// @address 0x632a38 ?process_collision@phys_convex_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_convex_list_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    const phys_convex_list_shape *cls = reinterpret_cast<const phys_convex_list_shape *>(agent_body_b.get_shape());
    phys_vec4 sweep_info;
    phys_swept_transform_util::compute_sweep_info(*agent_body_a.get_motion_state(), *agent_body_b.get_motion_state(),
        input.m_step_info.m_delta_time, sweep_info);

    phys_process_collision_output_backup output_backup(output);

    if (m_in_simplex_mode)
    {
simplex_mode_label:
        if (m_separating_normal(3) > input.get_tolerance())
        {
            m_separating_normal(3) -= static_cast<float>(sweep_info.dot4_xyz1(m_separating_normal));
            if (m_separating_normal(3) > input.get_tolerance())
            {
                if (m_box_manifold.m_contact_point_count)
                {
                    phys_simplex_manifold_cleanup(&m_box_manifold, m_contact_manager, *output.m_constraint_owner.val());
                }
                m_process_called = true;
                return;
            }
        }

        {
            phys_subshape_contact_manager sub_shape_mapper(m_contact_manager);
            m_contact_manager = &sub_shape_mapper;

            reinterpret_cast<phys_pred_simplex_agent *>(this)->phys_pred_simplex_agent::process_collision(agent_body_a, agent_body_b, input, output);

            m_contact_manager = sub_shape_mapper.m_contact_manager;

            if (sub_shape_mapper.m_invalid_point ||
                (m_box_manifold.m_contact_point_count && !m_process_called &&
                    m_separating_normal(3) < -(2.0f *
                        phys_math::min_of_two(agent_body_a.get_root_collider()->get_allowed_penetration_depth(),
                            agent_body_b.get_root_collider()->get_allowed_penetration_depth()))))
            {
                output_backup.rollback_output(agent_body_a, agent_body_b, output, m_contact_manager);
                switch_to_stream_mode(*output.m_constraint_owner.val());
                goto stream_mode_label;
            }
        }
    }
    else
    {
stream_mode_label:
        if (m_stream_mode_counter-- < 0)
        {
            m_stream_mode_counter = 25;

            {
                phys_simplex::closest_point_input simplex_input;
                phys_transform a_from_b; a_from_b.set_mul_inverse_mul(agent_body_a.get_transform(), agent_body_b.get_transform());
                {
                    simplex_input.m_shape_a = static_cast<const phys_convex_shape *>((const void *)agent_body_a.get_shape());
                    simplex_input.m_shape_b = static_cast<const phys_convex_shape *>((const void *)agent_body_b.get_shape());
                    simplex_input.m_a_to_b = &a_from_b;
                    simplex_input.m_transform_a = &agent_body_a.get_transform();
                    simplex_input.m_collision_tolerance = input.get_tolerance();
                }

                phys_vec4 point_on_b;
                if (phys_simplex::get_closest_point(simplex_input, m_cache, m_separating_normal, point_on_b) != PHYS_SUCCESS)
                {
                    switch_to_simplex_mode(*output.m_constraint_owner.val());
                    output_backup.rollback_output(agent_body_a, agent_body_b, output, m_contact_manager);
                    goto simplex_mode_label;
                }
                get_stream().m_stream_mode_sweep_dist = -m_separating_normal(3);
            }
        }
        get_stream().m_stream_mode_sweep_dist -= static_cast<float>(sweep_info.length_3d());

        phys_narrow_agent_process_input narrow_input;
        {
            narrow_input.m_body_a = &agent_body_a;
            narrow_input.m_body_b = &agent_body_b;
            narrow_input.m_contact_manager = m_contact_manager;
            narrow_input.m_input = &input;
            narrow_input.m_lin_sweep_info = sweep_info;

            const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
            const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();

            narrow_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());
        }

        {
            int size = cls->m_child_shapes.getSize();
            unsigned int *hit_key_list = phys_alloc_stack<unsigned int>(size + 1);
            for (int i = 0; i < size; i++) { hit_key_list[i] = static_cast<unsigned int>(i); }
            hit_key_list[size] = 0xffffffffu;

            phys_batch_agent_process(get_stream().m_agent_track, 0, narrow_input, cls, hit_key_list, size, output);

            phys_free_stack(hit_key_list);
        }
    }

    m_process_called = true;
}
// @address 0x632f40 ?create_convex_vs_list_agent@phys_convex_list_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_convex_list_agent::create_convex_vs_list_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry;
    if (pair_contact_manager != 0) {
        unsigned int type = input.m_convex_list_filter->get_convex_list_mode(agent_body_b, agent_body_a, input);
        if (type >= 1) {
            if (type != 1) {
                if (type >= 3) { pair_agent_entry = 0; }
                else { pair_agent_entry = new phys_pred_simplex_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
            } else { pair_agent_entry = new phys_symmetric_agent<phys_compound_shape_agent>(agent_body_a, agent_body_b, input, pair_contact_manager); }
        } else { pair_agent_entry = new phys_convex_list_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
    } else { pair_agent_entry = new phys_compound_shape_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
    return pair_agent_entry;
}
// @address 0x6330b8 ?create_list_vs_convex_agent@phys_convex_list_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_convex_list_agent::create_list_vs_convex_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_agent *pair_agent_entry;
    if (pair_contact_manager != 0) {
        unsigned int type = input.m_convex_list_filter->get_convex_list_mode(agent_body_a, agent_body_b, input);
        if (type >= 1) {
            if (type != 1) {
                if (type >= 3) { pair_agent_entry = 0; }
                else { pair_agent_entry = new phys_pred_simplex_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
            } else { pair_agent_entry = new phys_compound_shape_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
        } else { pair_agent_entry = new phys_symmetric_agent<phys_convex_list_agent>(agent_body_a, agent_body_b, input, pair_contact_manager); }
    } else { pair_agent_entry = new phys_symmetric_agent<phys_compound_shape_agent>(agent_body_a, agent_body_b, input, pair_contact_manager); }
    return pair_agent_entry;
}
// @address 0x633248 ?create_list_vs_list_agent@phys_convex_list_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_convex_list_agent::create_list_vs_list_agent(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager)
{
    do
    {
        if (pair_contact_manager != 0) {
            unsigned int type = input.m_convex_list_filter->get_convex_list_mode(agent_body_a, agent_body_b, input);
            if (type >= 1) {
                if (type != 1) {
                    if (type >= 3) { break; }
                    else {
                        unsigned int other_list_type = input.m_convex_list_filter->get_convex_list_mode(agent_body_b, agent_body_a, input);
                        if (other_list_type >= 1) {
                            if (other_list_type != 1) {
                                if (other_list_type >= 3) { break; }
                                else { return new phys_pred_simplex_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
                            } else { return new phys_symmetric_agent<phys_compound_shape_agent>(agent_body_a, agent_body_b, input, pair_contact_manager); }
                        } else { return new phys_convex_list_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
                    }
                } else { return new phys_compound_shape_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
            } else { return new phys_symmetric_agent<phys_convex_list_agent>(agent_body_a, agent_body_b, input, pair_contact_manager); }
        } else { return new phys_compound_shape_agent(agent_body_a, agent_body_b, input, pair_contact_manager); }
    } while (false);
    return 0;
}
// @address 0x633468 ?register_agent@phys_convex_list_agent@@SAXPAVphys_dispatcher@@@Z
void phys_convex_list_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs forward;
    forward.m_create_fn = create_list_vs_convex_agent;
    forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_penetrations;
    forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_closest_points;
    forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_sweep_cast;
    forward.m_flipped = true;
    forward.m_predictive = true;
    dispatcher->register_collision_agent(forward, PHYS_SHAPE_CONVEX_LIST, PHYS_SHAPE_CONVEX);
    phys_dispatcher::agent_funcs backward;
    backward.m_create_fn = create_convex_vs_list_agent;
    backward.m_penetrations_fn = static_penetrations;
    backward.m_closest_points_fn = static_closest_points;
    backward.m_linear_cast_func = static_sweep_cast;
    backward.m_flipped = false;
    backward.m_predictive = true;
    dispatcher->register_collision_agent(backward, PHYS_SHAPE_CONVEX, PHYS_SHAPE_CONVEX_LIST);
    phys_dispatcher::agent_funcs both_cmp;
    both_cmp.m_create_fn = create_list_vs_list_agent;
    both_cmp.m_penetrations_fn = static_penetrations;
    both_cmp.m_closest_points_fn = static_closest_points;
    both_cmp.m_linear_cast_func = static_sweep_cast;
    both_cmp.m_flipped = false;
    both_cmp.m_predictive = true;
    dispatcher->register_collision_agent(both_cmp, PHYS_SHAPE_CONVEX_LIST, PHYS_SHAPE_CONVEX_LIST);
}
// @address 0x633568 ?cleanup@phys_subshape_contact_manager@@UAAXXZ
void phys_subshape_contact_manager::cleanup() { m_contact_manager->cleanup(); }
// @address 0x633580 ?reserve_contact_points_impl@phys_subshape_contact_manager@@UAA?AW4phys_result@@H@Z
phys_result phys_subshape_contact_manager::reserve_contact_points_impl(int count) { return m_contact_manager->reserve_contact_points(count); }
// @address 0x633598 ?remove_contact_point_impl@phys_subshape_contact_manager@@UAAXGAAVphys_constraint_owner@@@Z
void phys_subshape_contact_manager::remove_contact_point_impl(unsigned short sphere_contact_id, phys_constraint_owner &owner)
{
    m_contact_manager->contact_remove_point(sphere_contact_id, owner);
}
// @address 0x6335b0 ?process_contact_impl@phys_subshape_contact_manager@@UAAXABVphys_collider@@0ABUphys_process_collision_input@@AAUphys_process_collision_data@@@Z
void phys_subshape_contact_manager::process_contact_impl(
    const phys_collider &a, const phys_collider &b, const phys_process_collision_input &input, phys_process_collision_data &data)
{
    m_contact_manager->process_contact(a, b, input, data);
}
// @address 0x6335c8 ?remove_toi_impl@phys_subshape_contact_manager@@UAAXABVphys_collider@@0AAVphys_constraint_owner@@AAVphys_contact_point_material@@@Z
void phys_subshape_contact_manager::remove_toi_impl(
    const phys_collider &a, const phys_collider &b, phys_constraint_owner &owner, phys_contact_point_material &material)
{
    m_contact_manager->remove_toi(a, b, owner, material);
}
// @address 0x6335e0 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_compound_shape_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x633630 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_compound_shape_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x633680 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_compound_shape_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_compound_shape_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_compound_shape_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}
// @address 0x633a48 ?rollback_output@phys_process_collision_output_backup@@QAAXABVphys_collision_proxy@@0AAUphys_process_collision_output@@PAVphys_contact_manager@@@Z
void phys_process_collision_output_backup::rollback_output(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, phys_process_collision_output &output, phys_contact_manager *agent_contact_manager)
{
    if (output.m_collision_toi.m_time != m_collision_toi.m_time) {
        agent_contact_manager->remove_toi(*agent_body_a.get_root_collider(), *agent_body_b.get_root_collider(),
            *output.m_constraint_owner.val(), output.m_collision_toi.m_properties);
        output.m_collision_toi = m_collision_toi;
    }
    output.m_next_free_contact = m_next_free_contact;
    if (output.m_potential_contacts) {
        phys_process_collision_output::potential_info *potential_contacts = output.m_potential_contacts;
        memcpy(potential_contacts, &m_potential_contacts, sizeof(m_potential_contacts));
    }
}
// @address 0x633b60 ?add_contact_point_impl@phys_subshape_contact_manager@@UAAGABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@PBVphys_simplex_cache@@AAVphys_contact_point@@@Z
unsigned short phys_subshape_contact_manager::add_contact_point_impl(const phys_collision_proxy &a, const phys_collision_proxy &b,
    const phys_process_collision_input &input, phys_process_collision_output &output, const phys_simplex_cache *cache, phys_contact_point &cp)
{
    const phys_collision_proxy *mod_proxy_a = &a;
    const phys_collision_proxy *mod_proxy_b = &b;
    phys_collision_proxy scratch_proxy_a;
    phys_collision_proxy scratch_proxy_b;
    if (a.m_proxy_shape->m_type == 17) {
        int kdop_shape_key = (0xff00 & cache->m_vertices[0]) >> 8;
        for (int i = 1; i < cache->m_dim_a; i++) {
            int other_sub_key = (0xff00 & cache->m_vertices[i]) >> 8;
            if (other_sub_key != kdop_shape_key) { m_invalid_point = true; return 0xffff; }
        }
        __declspec(align(16)) char buffer[512];
        const phys_shape *bounding_child_shape = static_cast<const phys_convex_list_shape *>(a.get_shape())->get_child_shape(kdop_shape_key, buffer);
        new (&scratch_proxy_a) phys_collision_proxy(&a);
        scratch_proxy_a.set_proxy_shape(bounding_child_shape, kdop_shape_key);
        mod_proxy_a = &scratch_proxy_a;
    }
    if (b.m_proxy_shape->m_type == 17) {
        int kdop_shape_key = (0xff00 & cache->m_vertices[cache->m_dim_a]) >> 8;
        for (int i = cache->m_dim_a + 1; i < cache->m_dim_a + cache->m_dim_b; i++) {
            int other_sub_key = (0xff00 & cache->m_vertices[i]) >> 8;
            if (other_sub_key != kdop_shape_key) { m_invalid_point = true; return 0xffff; }
        }
        __declspec(align(16)) char buffer[512];
        const phys_shape *bounding_child_shape = static_cast<const phys_convex_list_shape *>(b.get_shape())->get_child_shape(kdop_shape_key, buffer);
        new (&scratch_proxy_b) phys_collision_proxy(&b);
        scratch_proxy_b.set_proxy_shape(bounding_child_shape, kdop_shape_key);
        mod_proxy_b = &scratch_proxy_b;
    }
    return m_contact_manager->add_contact_point_impl(*mod_proxy_a, *mod_proxy_b, input, output, cache, cp);
}
// @address 0x633cd8 ?add_toi_impl@phys_subshape_contact_manager@@UAA?AW4toi_verdict@phys_contact_manager@@ABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@MAAVphys_contact_point@@PBVphys_simplex_cache@@AAMAAVphys_contact_point_material@@@Z
phys_contact_manager::toi_verdict phys_subshape_contact_manager::add_toi_impl(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output, float time, phys_contact_point &point, const phys_simplex_cache *cache,
    float &contact_projected_vel, phys_contact_point_material &material)
{
    const phys_collision_proxy *mapped_proxy_a = &agent_body_a;
    const phys_collision_proxy *mapped_proxy_b = &agent_body_b;
    phys_collision_proxy child_proxy_a;
    phys_collision_proxy child_proxy_b;
    if (agent_body_a.m_proxy_shape->m_type == 17) {
        int key = cache->m_vertices[0] >> 8;
        for (int i = 1; i < cache->m_dim_a; i++) {
            int other_sub_key = cache->m_vertices[i] >> 8;
            if (other_sub_key != key) { m_invalid_point = true; return TOI_EVENT_REJECT; }
        }
        __declspec(align(16)) char buffer[512];
        const phys_shape *bounding_child_shape = static_cast<const phys_convex_list_shape *>(agent_body_a.get_shape())->get_child_shape(key, buffer);
        new (&child_proxy_a) phys_collision_proxy(&agent_body_a);
        child_proxy_a.set_proxy_shape(bounding_child_shape, key);
        mapped_proxy_a = &child_proxy_a;
    }
    if (agent_body_b.m_proxy_shape->m_type == 17) {
        int key = cache->m_vertices[cache->m_dim_a] >> 8;
        for (int i = cache->m_dim_a + 1; i < cache->m_dim_a + cache->m_dim_b; i++) {
            int other_sub_key = cache->m_vertices[i] >> 8;
            if (other_sub_key != key) { m_invalid_point = true; return TOI_EVENT_REJECT; }
        }
        __declspec(align(16)) char buffer[512];
        const phys_shape *bounding_child_shape = static_cast<const phys_convex_list_shape *>(agent_body_b.get_shape())->get_child_shape(key, buffer);
        new (&child_proxy_b) phys_collision_proxy(&agent_body_b);
        child_proxy_b.set_proxy_shape(bounding_child_shape, key);
        mapped_proxy_b = &child_proxy_b;
    }
    return m_contact_manager->add_toi(*mapped_proxy_a, *mapped_proxy_b, input, output, time, point, cache, contact_projected_vel, material);
}
// @address 0x633f08 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_convex_list_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x633f58 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_convex_list_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_convex_list_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}
// @address 0x634040 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_convex_list_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
// @address 0x6340b0 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_convex_list_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x634100 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_convex_list_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x634148 ?sweep_cast@?$phys_symmetric_agent@Vphys_convex_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template<>
void phys_symmetric_agent<phys_convex_list_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_convex_list_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    } else {
        phys_convex_list_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}
// @address 0x634238 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_convex_list_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}

phys_process_collision_output_backup::phys_process_collision_output_backup(const phys_process_collision_output &output)
{
    m_next_free_contact = output.m_next_free_contact;
    m_collision_toi = output.m_collision_toi;
    if (output.m_potential_contacts) {
        m_potential_contacts = *output.m_potential_contacts;
    }
}


// @address 0x633e78 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_convex_list_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_convex_list_agent>::static_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_flag_proxy_pair_collector union_find_checker;
    union_find_checker.reset();
    phys_simplex_agent_base::static_penetrations(agent_body_b, agent_body_a, input, union_find_checker);
    if (union_find_checker.has_hit()) {
        phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
    }
}
