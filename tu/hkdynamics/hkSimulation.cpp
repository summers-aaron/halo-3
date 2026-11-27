// hkdynamics/hkSimulation.obj: from the records 0x103740 0x650dd8 0x650e50 0x650ea0 0x650eb0 0x650ed0 0x650ef0 0x650f98 0x650fa8 0x650fb8 0x650fc0 0x651100 0x651158 0x6511f8 0x651298 0x651320 0x651430 0x651720 0x6517f0 0x651f60 0x6520a0 0x6520b0 0x6520d8 0x6525a8 0x652798 0x652868
// @flags /TP /O2 /Oc
// @address 0x103740 ?bump_sleep_flags@phys_solver_info@@QAAXXZ
// @address 0x650dd8 ?asin@phys_math@@YAMM@Z
// @address 0x650e50 ??0phys_simulation@@QAA@PAVphys_world@@@Z
// @address 0x650ea0 ??1phys_simulation@@UAA@XZ
// @address 0x650eb0 ?sim_at_marker@phys_simulation@@QAA_NXZ
// @address 0x650ed0 ?sim_at_psi@phys_simulation@@QBA_NXZ
// @address 0x650ef0 ?step_delta_time@phys_simulation@@UAA?AW4phys_step_result@@M@Z
// @address 0x650f98 ?reset_agent_collision_info@phys_simulation@@IAAXPAUphys_pair_agent_entry@@ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x650fc0 ?recollide_after_step_fail@phys_simulation@@IAA?AW4phys_step_result@@XZ
// @address 0x651158 ?advance_time@phys_simulation@@UAA?AW4phys_step_result@@XZ
// @address 0x6511f8 ?apply_actions@phys_simulation@@IAAXXZ
// @address 0x651298 ?set_rotation_about_com@@YAXPAVphys_rigid_body@@AAVphys_quat@@@Z
// @address 0x651320 ?collide@phys_simulation@@UAA?AW4phys_step_result@@XZ
// @address 0x6520a0 ?collide_entities_narrowphase_discrete@phys_simulation@@QAAXPAPAVphys_entity@@HABUphys_process_collision_input@@W4find_contacts_mode@1@@Z
// @address 0x6520b0 ?reset_collision_info_for_entities@phys_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@Vphys_bool@@@Z
// @address 0x6525a8 ?collide_internal@phys_simulation@@MAAXABVphys_step_info@@@Z
// @address 0x652798 ?collide_entities_discrete@phys_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@ABVphys_step_info@@W4find_contacts_mode@1@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_world_op_queue.h"
#include "phys/phys_world_callback_util.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "physics/havok_memory.h"
#include "phys/phys_solver.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_island.h"
#include "phys/phys_action.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_simulation.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
#include "phys/phys_world_maintenance_manager.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_local_array.h"
#include "phys/phys_ptr_map_base.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_memory.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_constraint_query_in_solver.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_constraint_query_in.h"
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ






void phys_solver_info::bump_sleep_flags()
{
    ++m_integrate_sleep_counter;
    if (((m_integrate_sleep_counter - 4) & 7) == 0)
        m_inactive_frame_select[0] ^= 1;
    if ((m_integrate_sleep_counter & 7) == 0)
        m_inactive_frame_select[0] ^= 2;
    if ((m_integrate_sleep_counter & 15) == 0) {
        m_integrate_sleep_counter = 0;
        m_inactive_frame_select[1] = 1 - m_inactive_frame_select[1];
    }
}

float phys_math::asin(float value)
{
    if (phys_math::fabs(value) >= 1.0f) {
        value = value > 0.0f ? 1.5707964f : -1.5707964f;
        return value;
    } else {
        return (float)::asin(value);
    }
}

phys_simulation::phys_simulation(phys_world *world)
{
    m_world = world;
    m_sim_cur_time = 0.0f;
    m_current_psi_time = 0.0f;
    m_simulate_until_time = -1.0f;
    m_frame_marker_psi_snap = 0.0001f;
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    m_frame_counter = 1;
}

phys_simulation::~phys_simulation()
{
}

bool phys_simulation::sim_at_marker()
{
    return m_sim_cur_time == m_simulate_until_time;
}

bool phys_simulation::sim_at_psi() const
{
    return m_current_psi_time == m_sim_cur_time;
}

phys_step_result phys_simulation::step_delta_time(float body_delta_time)
{
    if (m_prev_step_result == PHYS_STEP_RESULT_SUCCESS || m_prev_step_result == PHYS_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION)
        integrate_world(body_delta_time);
    if (m_prev_step_result == PHYS_STEP_RESULT_SUCCESS || m_prev_step_result == PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE)
        collide();
    if (m_prev_step_result == PHYS_STEP_RESULT_SUCCESS || m_prev_step_result == PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_TOI_SOLVE)
        advance_time();
    return m_prev_step_result;
}

void phys_simulation::reset_agent_collision_info(phys_pair_agent_entry *entry, const phys_process_collision_input &input, phys_process_collision_output &output)
{
    phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&input));
}

// @address 0x651100 ?snap_sim_time_to_advance@phys_simulation@@IAAMXZ
// match: 0x651158: inline expands the time snap into advanceTime; out of line it calls it (64 bytes shorter)
#if defined(_XBOX) && !defined(__clang__)
inline float phys_simulation::snap_sim_time_to_advance()
#else
float phys_simulation::snap_sim_time_to_advance()
#endif
{
    if (m_simulate_until_time != -1.0f && phys_math::fabs(m_simulate_until_time - m_current_psi_time) < m_frame_marker_psi_snap)
        m_simulate_until_time = m_current_psi_time;
    if (m_simulate_until_time == -1.0f) {
        return m_current_psi_time;
    } else {
        return phys_math::min_of_two(m_current_psi_time, m_simulate_until_time);
    }
}


phys_step_result phys_simulation::advance_time()
{
    m_sim_cur_time = snap_sim_time_to_advance();
    if (m_sim_cur_time >= m_simulate_until_time && m_world->m_post_sim_listeners.getSize() != 0)
        phys_world_callback_util::notify_world_post_simulation(m_world);
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    return m_prev_step_result;
}

void phys_simulation::apply_actions()
{
    const phys_array<phys_island *> &world_active_islands = m_world->get_active_islands();
    for (int i = 0; i < world_active_islands.getSize(); ++i) {
        const phys_array<phys_action *> &actions = world_active_islands[i]->m_actions;
        for (int j = 0; j < actions.getSize(); ++j) {
            phys_action *action = actions[j];
            action->apply_action(m_world->m_dynamics_step_info.m_step_info);
        }
    }
}


__forceinline void set_rotation_about_com(phys_rigid_body *body, phys_quat &rotation)
{
    phys_transform *transform = &body->get_motion()->get_motion_state()->m_body_transform;
    transform->m_rotation.set(rotation);
    const __vector4 &local_mass_center = body->get_center_of_mass_local().m_simd_quad;
    transform->m_translation.m_simd_quad = __vsubfp(
        body->get_center_of_mass_in_world().m_simd_quad,
        __vmaddfp(transform->m_rotation.get_column(2).m_simd_quad, __vspltw(local_mass_center, 2),
            __vmaddfp(transform->m_rotation.get_column(1).m_simd_quad, __vspltw(local_mass_center, 1),
                __vmulfp(transform->m_rotation.get_column(0).m_simd_quad, __vspltw(local_mass_center, 0)))));
}

void phys_simulation::collide_entities_narrowphase_discrete(phys_entity **entities, int count,
    const phys_process_collision_input &input, find_contacts_mode find_contacts_fn)
{
    process_entity_agents(entities, count, input, &phys_simulation::collide_agent_discrete, find_contacts_fn);
}

void phys_simulation::reset_collision_info_for_entities(phys_entity **entities, int count, phys_world *world, phys_bool skip_reinit)
{
    if (skip_reinit)
        return;
    process_entity_agents(entities, count, *world->m_collision_input, &phys_simulation::reset_agent_collision_info, CONTACT_SEARCH_DEFAULT);
}

static __forceinline bool sim_memory_failed()
{
    return phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED;
}

phys_step_result phys_simulation::recollide_after_step_fail()
{
    m_world->refilter_world(PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK, PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
    phys_step_info step_info;
    if (m_prev_step_result == PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_TOI_SOLVE)
        step_info.set(m_sim_cur_time, m_current_psi_time);
    else
        step_info.set(m_current_psi_time, m_current_psi_time + m_physics_dt);
    collide_internal(step_info);
    if (sim_memory_failed())
        return m_prev_step_result;
    if (m_prev_step_result == PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE) {
        m_current_psi_time += m_physics_dt;
        if (m_world->m_post_collide_listeners.getSize())
            phys_world_callback_util::notify_world_post_collide(m_world, step_info);
        m_frame_counter = 1;
    }
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    return PHYS_STEP_RESULT_SUCCESS;
}

phys_step_result phys_simulation::collide()
{
    if (m_prev_step_result != PHYS_STEP_RESULT_SUCCESS)
        return recollide_after_step_fail();
    phys_step_info step_info(m_current_psi_time, m_current_psi_time + m_physics_dt);
    collide_internal(step_info);
    if (sim_memory_failed()) {
        m_prev_step_result = PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE;
        return PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE;
    }
    m_current_psi_time += m_physics_dt;
    if (m_world->m_post_collide_listeners.getSize())
        phys_world_callback_util::notify_world_post_collide(m_world, step_info);
    m_frame_counter = 1;
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    return PHYS_STEP_RESULT_SUCCESS;
}


// @address 0x651720 ?collide_agent_discrete@phys_simulation@@IAAXPAUphys_pair_agent_entry@@ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_simulation::collide_agent_discrete(phys_pair_agent_entry *entry, const phys_process_collision_input &bridge_process_input,
    phys_process_collision_output &process_output)
{
    process_output.reset();

    bridge_process_input.m_quality_info = bridge_process_input.m_agent_dispatcher->get_collision_quality_info(1);
    // match: 0x651720: keep the value snapshot formerly passed to hkPadSpu::operator=.
    const phys_bool want_predictive_agents = bridge_process_input.m_agent_dispatcher->get_collision_quality_info(entry->m_quality_idx)->m_use_ccd;
    bridge_process_input.m_create_pred_agents = want_predictive_agents;
    phys_pair_agent_process(entry, bridge_process_input, process_output, entry->m_contact_manager);
    if (havok_get_available_memory() == 0)
        phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
    if (sim_memory_failed())
        return;
    if (!process_output.is_array_empty())
        entry->m_contact_manager->process_contact(*entry->m_collider[0], *entry->m_collider[1], bridge_process_input, process_output);
}


// @address 0x651f60 ?integrate_world@phys_simulation@@UAA?AW4phys_step_result@@M@Z
phys_step_result phys_simulation::integrate_world(float physics_delta_time)
{
    m_physics_dt = physics_delta_time;
    {
        int mem_needed = m_world->get_integration_mem_usage();
        phys_thread_memory &system_thread_memory = phys_thread_memory::get_singleton_instance();
        if (mem_needed > int(system_thread_memory.m_mem_stack.m_end - system_thread_memory.m_mem_stack.m_current) - 16 &&
            mem_needed > havok_get_available_memory()) {
            phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
            m_prev_step_result = PHYS_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION;
            return PHYS_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION;
        }
    }
    phys_step_info physics_step_info(m_current_psi_time, m_current_psi_time + m_physics_dt);
    m_world->m_dynamics_step_info.m_step_info = physics_step_info;
    m_world->m_collision_input->m_step_info = physics_step_info;
    m_world->m_maintenance_manager->perform_maintenance(m_world, physics_step_info);
    integrate_internal(physics_step_info);
    m_frame_counter = 0;
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    return PHYS_STEP_RESULT_SUCCESS;
}


__forceinline void phys_simulation::collide_island_narrowphase_discrete(phys_island *island, const phys_process_collision_input &input)
{
    phys_process_collision_input &collision_input = const_cast<phys_process_collision_input &>(input);
    // match: 0x6525a8: disable prediction before selecting discrete collision quality.
    phys_dispatcher *dispatcher = collision_input.m_agent_dispatcher;
    collision_input.m_create_pred_agents = false;
    collision_input.m_quality_info = dispatcher->get_collision_quality_info(1);
    phys_pair_agent_track &track = island->m_agent_track;
    phys_pair_agent_process_track(island, track, input);
}


// @address 0x6520d8 ?collide_entities_broadphase_discrete@phys_simulation@@SAXPAPAVphys_entity@@HPAVphys_world@@@Z
void phys_simulation::collide_entities_broadphase_discrete(phys_entity **entities, int solver_entity_count, phys_world *world)
{
    phys_local_array<phys_broadphase_handle_pair> new_pairs(world->m_broadphase_update_size);
    phys_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(world->m_broadphase_update_size);
    {
        phys_aabb *aabb_list = phys_alloc_stack<phys_aabb>(solver_entity_count);
        phys_broadphase_handle **broadphase_handles = phys_alloc_stack<phys_broadphase_handle *>(solver_entity_count);
        const float tolerance = world->m_collision_input->m_box_tolerance * 0.5f;
        int aabb_count = solver_entity_count;
        {
            phys_entity **e = entities;
            phys_aabb *aabb = aabb_list;
            phys_broadphase_handle **handle = broadphase_handles;
            for (int i = aabb_count - 1; i >= 0; e++, i--) {
                phys_collider *collidable = (*e)->get_collider_rw();
                handle[0] = collidable->get_broadphase_handle();
                collidable->get_shape()->get_handle_aabb(collidable->get_transform(), tolerance, *aabb);
                aabb++;
                handle++;
            }
        }
        world->m_broadphase->update_aabbs(broadphase_handles, aabb_list, aabb_count, new_pairs, sweep_removed_pairs);
        phys_free_stack(broadphase_handles);
        phys_free_stack(aabb_list);
    }
    if (new_pairs.getSize() + sweep_removed_pairs.getSize() > 0) {
        phys_typed_broadphase_dispatcher::remove_duplicate_pairs(new_pairs, sweep_removed_pairs);
        world->m_broadphase_dispatcher->remove_handle_pairs(static_cast<phys_typed_broadphase_pair *>(sweep_removed_pairs.m_data),
            sweep_removed_pairs.getSize());
        int mem_per_pair_estimate = 128 + 192 + 256 + 64;
        int available_memory = havok_get_available_memory();
        if (new_pairs.getSize() * mem_per_pair_estimate > available_memory) {
            phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
            return;
        }
        world->m_broadphase_dispatcher->add_handle_pairs(static_cast<phys_typed_broadphase_pair *>(new_pairs.m_data),
            new_pairs.getSize(), world->m_collision_filter);
    }
}

void phys_simulation::collide_internal(const phys_step_info &step_info)
{
    m_world->m_dynamics_step_info.m_step_info = step_info;
    m_world->m_collision_input->m_step_info = step_info;
    phys_solver_info &solver_info = m_world->m_dynamics_step_info.m_solver_info;
    solver_info.m_delta_time = step_info.m_delta_time * solver_info.m_inv_steps;
    solver_info.m_inv_dt = step_info.m_inv_dt * (float)solver_info.m_step_count;
    m_world->lock_critical_ops();
    const phys_array<phys_island *> &broadphase_islands = m_world->get_active_islands();
    for (int i = 0; i < broadphase_islands.getSize(); ++i) {
        phys_island *island = broadphase_islands[i];
        collide_entities_broadphase_discrete(&island->m_island_entities[0], island->m_island_entities.getSize(), m_world);
        if (sim_memory_failed()) {
            m_world->unlock_and_flush_ops();
            return;
        }
    }
    m_world->unlock_and_flush_ops();
    m_world->lock_critical_ops();
    const phys_array<phys_island *> &narrowphase_islands = m_world->get_active_islands();
    for (int i = 0; i < narrowphase_islands.getSize(); ++i) {
        phys_island *island = narrowphase_islands[i];
        collide_island_narrowphase_discrete(island, *m_world->m_collision_input);
        if (sim_memory_failed()) {
            m_world->unlock_and_flush_ops();
            return;
        } else {
            if (m_world->m_island_post_collide_listeners.getSize() != 0)
                phys_world_callback_util::notify_island_post_collide(m_world, island, step_info);
        }
    }
    m_world->unlock_and_flush_ops();
}

void phys_simulation::collide_entities_discrete(phys_entity **entities, int count, phys_world *world, const phys_step_info &step_info,
    find_contacts_mode find_contacts_fn)
{
    phys_step_info saved_step_info = step_info;
    phys_process_collision_input *input = world->m_collision_input;
    input->m_step_info = step_info;
    world->m_critical_ops_lock_count++;
    collide_entities_broadphase_discrete(entities, count, world);
    process_entity_agents(entities, count, *input, &phys_simulation::collide_agent_discrete, find_contacts_fn);
    world->unlock_and_flush_ops();
    input->m_step_info = saved_step_info;
}



// @address 0x652868 ?reintegrate_recollide@phys_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@@Z
void phys_simulation::reintegrate_recollide(phys_entity **entities, int count, phys_world *world)
{
    if (world->m_critical_ops_lock_count != 0) {
        phys_world_op::reintegrate_recollide_batch_op operation;
        operation.m_island_entities = entities;
        operation.m_entity_count = (unsigned short)count;
        world->queue_world_op(operation);
        return;
    }
    world->m_critical_ops_lock_count = 1;
    phys_step_info step_info(world->m_world_simulation->m_sim_cur_time, world->m_world_simulation->m_current_psi_time);
    world->m_collision_input->m_step_info = step_info;
    for (int i = 0; i < count; i++)
        phys_swept_transform_util::back_step_motion_state(world->m_world_simulation->m_sim_cur_time, *entities[i]->get_motion()->get_motion_state());
    phys_rigid_motion_step(step_info, (phys_motion *const *)entities, count, (int)offsetof(phys_entity, m_motion));
    collide_entities_broadphase_discrete(entities, count, world);
    for (int i = 0; i < count; i++)
        phys_world_agent_util::invalidate_sweep_info(entities[i], *(phys_collision_input *)world->m_collision_input);
    process_entity_agents(entities, count, *world->m_collision_input, &phys_simulation::collide_agent_discrete, CONTACT_SEARCH_DEFAULT);
    world->unlock_and_flush_ops();
}














// @address 0x6517f0 ?process_entity_agents@phys_simulation@@IAAXPAPAVphys_entity@@HABUphys_process_collision_input@@P81@AAXPAUphys_pair_agent_entry@@1AAUphys_process_collision_output@@@ZW4find_contacts_mode@1@@Z
void phys_simulation::process_entity_agents(phys_entity **entities, int solver_entity_count, const phys_process_collision_input &bridge_process_input,
    void (phys_simulation::*process_fn)(phys_pair_agent_entry *, const phys_process_collision_input &, phys_process_collision_output &),
    find_contacts_mode find_extra_contacts)
{
    phys_ptr_map_base<phys_ulong> processed_entities;
    processed_entities.reserve(solver_entity_count);

    phys_process_collision_output process_output(0);

    for (int i = 0; i < solver_entity_count; i++)
    {
        phys_entity *entity = entities[i];
        phys_linked_collider *linked_collider = entity->get_linked_collider();
        processed_entities.insert((phys_ulong)linked_collider, 0);

        for (int c = 0; c < linked_collider->m_collision_entries.getSize(); c++)
        {
            phys_linked_collider::collision_entry &linked_entry = linked_collider->m_collision_entries[c];

            phys_linked_collider *partner = linked_entry.m_linked_partner;
            phys_ptr_map_base<phys_ulong>::Iterator it = processed_entities.map_find_key((phys_ulong)partner);
            if (processed_entities.is_valid_aabb(it))
            {
                continue;
            }

            {
                phys_pair_agent_entry *entry = linked_entry.m_agent_entry;
                {
                    phys_entity *listener_entity_a = static_cast<phys_entity *>(entry->m_collider[0]->get_collider_owner());
                    phys_entity *listener_entity_b = static_cast<phys_entity *>(entry->m_collider[1]->get_collider_owner());
                    phys_island *island = listener_entity_a->body_is_fixed() ? listener_entity_b->get_island() : listener_entity_a->get_island();
                    process_output.m_constraint_owner = island;
                }

                phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&bridge_process_input));
                (this->*process_fn)(entry, bridge_process_input, process_output);

                if (find_extra_contacts == CONTACT_SEARCH_EXTRA && process_output.m_next_free_contact != process_output.m_process_contact_points)
                {
                    phys_rigid_body *rb = (phys_rigid_body *)entity;
                    phys_quat orig_rotation = rb->get_rigid_motion()->get_rotation();
                    phys_transform orig_transform = rb->get_transform();

                    phys_vec4 normal = process_output.m_process_contact_points[0].m_hit_contact.get_separating_normal();

                    if (linked_entry.m_agent_entry->m_collider[0] != entity->get_collider())
                    {
                        normal.set_neg4(normal);
                    }

                    phys_vec4 diff; diff.set_sub4(rb->get_center_of_mass_in_world(),
                        process_output.m_process_contact_points[0].m_hit_contact.get_contact_position());
                    phys_vec4 rotate_axis_dir; rotate_axis_dir.set_cross(normal, diff);
                    const float len = rotate_axis_dir.length_3d();

                    const float l = normal.dot3(diff);
                    const float d = diff.length_3d();
                    const float rotate_sin_from = l / d;

                    if (len > 0.00001f && rotate_sin_from < 0.9999f)
                    {
                        const float tolerance_mult = 20.0f;

                        float saved_tolerance = bridge_process_input.m_box_tolerance;
                        float search_tolerance = bridge_process_input.m_box_tolerance * tolerance_mult;
                        const_cast<phys_process_collision_input &>(bridge_process_input).m_box_tolerance = search_tolerance;
                        float saved_create_4d_contact = bridge_process_input.m_quality_info->m_create_4d_contact;
                        float saved_create_contact = bridge_process_input.m_quality_info->m_create_contact;
                        bridge_process_input.m_quality_info->m_create_4d_contact = search_tolerance;
                        bridge_process_input.m_quality_info->m_create_contact = search_tolerance;

                        float rotate_sin_to = phys_math::max_of_two((l - search_tolerance * (1.0f / tolerance_mult)) / d, 0.0f);
                        float rotate_angle = phys_math::asin(rotate_sin_from) - phys_math::asin(rotate_sin_to);

                        rotate_axis_dir.mul4(1.0f / len);

                        {
                            phys_quat quat; quat.set_axis_angle(rotate_axis_dir, rotate_angle);
                            phys_quat body_rotation;
                            body_rotation.set_mul(quat, rb->get_rigid_motion()->get_rotation());

                            set_rotation_about_com(rb, body_rotation);

                            phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&bridge_process_input));

                            (this->*process_fn)(entry, bridge_process_input, process_output);
                        }

                        {
                            phys_vec4 rotate_cross_dir; rotate_cross_dir.set_cross(rotate_axis_dir, normal);
                            rotate_cross_dir.normalize_xyz();

                            phys_quat quat; quat.set_axis_angle(rotate_cross_dir, rotate_angle);
                            phys_quat body_rotation; body_rotation.set_mul(quat, orig_rotation);
                            set_rotation_about_com(rb, body_rotation);

                            phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&bridge_process_input));

                            (this->*process_fn)(entry, bridge_process_input, process_output);

                            body_rotation.set_inverse_mul(quat, orig_rotation);
                            set_rotation_about_com(rb, body_rotation);

                            phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&bridge_process_input));

                            (this->*process_fn)(entry, bridge_process_input, process_output);
                        }

                        {
                            const_cast<phys_process_collision_input &>(bridge_process_input).m_box_tolerance = saved_tolerance;
                            bridge_process_input.m_quality_info->m_create_4d_contact = saved_create_4d_contact;
                            bridge_process_input.m_quality_info->m_create_contact = saved_create_contact;

                            rb->get_rigid_motion()->set_transform(orig_transform);
                            phys_pair_agent_invalidate_sweep(entry, const_cast<phys_process_collision_input *>(&bridge_process_input));

                            (this->*process_fn)(entry, bridge_process_input, process_output);
                        }
                    }
                }
                if (sim_memory_failed())
                {
                    return;
                }
            }
        }
    }
}



__forceinline void phys_simulation::integrate_island(phys_island *island, const phys_world_dynamics_step_info &step_info,
    phys_constraint_query_in &constraint_query_in)
{
    int inactive_frame_count;
    if (island->m_constraint_info.m_schemas_size == 0) {
        inactive_frame_count = phys_rigid_motion_force_step(step_info.m_solver_info, step_info.m_step_info,
            step_info.m_solver_info.m_gravity_per_step, reinterpret_cast<phys_motion *const *>(island->m_island_entities.m_data),
            island->m_island_entities.getSize(), (int)offsetof(phys_entity, m_motion));
    } else {
        inactive_frame_count = phys_solver_setup::solve_constraints(
            step_info.m_step_info, step_info.m_solver_info, constraint_query_in, *island, &island->m_island_entities[0], island->m_island_entities.getSize());
    }
    if (inactive_frame_count > 5) {
        if (island->is_body_active()) {
            if (island->m_world->m_want_sleep)
                phys_world_op_util::mark_island_inactive(island->m_world, island);
        }
    }
}

// @address 0x651430 ?integrate_internal@phys_simulation@@IAAXABVphys_step_info@@@Z
void phys_simulation::integrate_internal(const phys_step_info &step_info_in)
{
    {
        m_world->m_dynamics_step_info.m_step_info = step_info_in;
        phys_solver_info &solver_info = m_world->m_dynamics_step_info.m_solver_info;
        solver_info.m_delta_time = step_info_in.m_delta_time * solver_info.m_inv_steps;
        solver_info.m_inv_dt = step_info_in.m_inv_dt * solver_info.m_step_count;
        solver_info.m_gravity_per_substep.set_mul4(solver_info.m_delta_time, m_world->m_gravity);
        float body_delta_time = step_info_in.m_delta_time;
        solver_info.m_gravity_per_step.set_mul4(body_delta_time, m_world->m_gravity);
    }
    {
        phys_solve_upload();
        phys_world_op_util::cleanup_dirty_islands(m_world);
        {
            m_world->lock_critical_ops();
            --m_world->m_critical_ops_volume_locks;
            apply_actions();
            ++m_world->m_critical_ops_volume_locks;
            m_world->unlock_and_flush_ops();
        }
        m_world->lock_critical_ops();
        m_world->m_dynamics_step_info.m_solver_info.bump_sleep_flags();
        phys_constraint_query_in constraint_query_in;
        constraint_query_in.set(m_world->m_dynamics_step_info.m_solver_info, step_info_in);
        const phys_array<phys_island *> &world_active_islands = m_world->get_active_islands();
        for (int i = world_active_islands.getSize() - 1; i >= 0; i--) {
            phys_island *active_island = world_active_islands[i];
            integrate_island(active_island, m_world->m_dynamics_step_info, constraint_query_in);
            if (m_world->m_island_post_integ_listeners.getSize())
                phys_world_callback_util::notify_island_post_integrate(m_world, active_island, step_info_in);
        }
        m_world->unlock_and_flush_ops();
    }
    if (m_world->m_post_integrate_listeners.getSize())
        phys_world_callback_util::notify_world_post_integrate(m_world, step_info_in);
}

// @address 0x650fa8 ?set_frame_time_marker@phys_simulation@@QAAXM@Z
void phys_simulation::set_frame_time_marker(float frame_delta_time)
{
    m_simulate_until_time = m_sim_cur_time + frame_delta_time;
}

// @address 0x650fb8 ?validate_delta_time@phys_simulation@@IAAXM@Z
void phys_simulation::validate_delta_time(float body_delta_time)
{
}

// @address 0x5bec98 ?warp_time@phys_simulation@@UAAXM@Z
void phys_simulation::warp_time(float body_delta_time)
{
}

