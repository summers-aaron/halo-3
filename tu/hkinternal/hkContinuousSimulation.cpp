// hkinternal/hkContinuousSimulation.obj: from the records 0x6a11e8 0x6a11f0 0x6a1210 0x6a12a8 0x6a12b0 0x6a12f8 0x6a1588 0x6a1688 0x6a1780 0x6a1850 0x6a18f0 0x6a19b0 0x6a1c50 0x6a1cd8 0x6a1da0 0x6a1ed8 0x6a2098 0x6a21c8 0x6a23b8 0x6a2760 0x6a2bb8 0x6a2cc8 0x6a2e58 0x6a2f08 0x6a2fa0 0x6a3000 0x6a3050 0x6a31f0 0x6a3300 0x6a38e0 0x6a38f0 0x6a4458 0x6a4610 0x6a4878 0x6a5148 0x6a52f8
// @flags /TP /O2 /Oc
// @compiler xdk5632
#include <ppcintrinsics.h>
#include <math.h>
#include <vectorintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_solver.h"
#include "phys/phys_solver_check.h"

#include "phys/phys_base_types.h"
#include "phys/phys_world_callback_util.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_simple_collision_response.h"
#include "phys/phys_world_agent_util.h"
class phys_broadphase;
#include "phys/phys_math.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_simple_constraint_util.h"
#include "phys/phys_string.h"
#include "phys/phys_ptr_map_base.h"
#include "phys/phys_constraint.h"
#include "phys/phys_atoms.h"
#include "phys/phys_simple_contact_data.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collide.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_toi_event.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_continuous_simulation.h"
#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_memory.h"
#include "phys/phys_broadphase_lock.h"
class phys_world;
class phys_continuous_simulation;
class phys_rotation;
#include "phys/phys_contact_point.h"
class phys_contact_manager;
#include "phys/phys_dynamics.h"
#include "phys/phys_thread_memory.h"

struct phys_pair_agent_entry;
struct phys_process_collision_output;
#include "phys/phys_collision_quality_info.h"

class phys_island;

class phys_island;

#include "phys/phys_pair_agent_entry.h"
class phys_step_info;
struct phys_solver_info;
class phys_constraint_query_in;
// match: 0x6a38f0, 0x6a4878: the canonical hkInplaceArray has an outlined implicit destructor;
// retain the inline array lifetime here until hk_base.h can declare its destructor inline.
template <typename T, unsigned N> class phys_continuous_sim_inline_array : public phys_array<T> {
public:
    __forceinline phys_continuous_sim_inline_array(int size = 0)
    {
        this->m_data = m_storage;
        this->m_size = size;
        this->m_capacity_and_flags = N | 0x80000000;
    }
    __forceinline ~phys_continuous_sim_inline_array() {}
    T m_storage[N];
};
class phys_schema;
class phys_solver_elem_temp;

#include "phys/phys_err_stream.h"
#include "phys/phys_error.h"

#include "phys/phys_default_toi_resource_manager.h"

#include "phys/phys_world_op_queue.h"
template <typename T> class phys_fixed_array;

#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_pair_agent_track.h"
#include "phys/phys_island.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_algorithm_sort.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_local_array.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_in_solver.h"

// @address 0x7ceb7e g_phys_toi_simulation_enabled
char g_phys_toi_simulation_enabled;

// @address 0x6a11e8 ?assert_no_entity_collision_info@phys_continuous_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@@Z
void phys_continuous_simulation::assert_no_entity_collision_info(phys_entity **entities, int count, phys_world *world)
{
}

// @address 0x6a11f0 ?less_entity_ptr@@YA?AVphys_bool@@PBVphys_entity@@0@Z
phys_bool less_entity_ptr(const phys_entity *left, const phys_entity *right)
{
    return phys_bool(left->get_uid() < right->get_uid());
}

// @address 0x6a1210 ?less_constraint_ptr@@YA?AVphys_bool@@PBVphys_constraint@@0@Z
phys_bool less_constraint_ptr(const phys_constraint *left, const phys_constraint *right)
{
    phys_constraint_internal *left_internal = left->m_constraint_internal;
    phys_constraint_internal *right_internal = right->m_constraint_internal;
    if (left_internal->m_island_entities[static_cast<const volatile phys_constraint_internal *>(left_internal)->m_master_body]->m_entity_uid < 
        right_internal->m_island_entities[static_cast<const volatile phys_constraint_internal *>(right_internal)->m_master_body]->m_entity_uid)
        return phys_bool(true);
    if (left_internal->m_island_entities[static_cast<const volatile phys_constraint_internal *>(left_internal)->m_master_body]->m_entity_uid == 
        right_internal->m_island_entities[static_cast<const volatile phys_constraint_internal *>(right_internal)->m_master_body]->m_entity_uid)
        return phys_bool(left_internal < right_internal);
    return phys_bool(false);
}

// @address 0x6a12a8 ?assert_no_agent_collision_info@phys_continuous_simulation@@UAAXPAUphys_pair_agent_entry@@@Z
void phys_continuous_simulation::assert_no_agent_collision_info(phys_pair_agent_entry *pair_agent_entry)
{
}

// @address 0x6a12b0 ?warp_time@phys_continuous_simulation@@UAAXM@Z
void phys_continuous_simulation::warp_time(float time)
{
    phys_continuous_simulation *simulation = static_cast<phys_continuous_simulation *>(m_world->m_world_simulation);
    for (int i = 0; i < simulation->m_toi_events.getSize(); ++i) {
        phys_toi_event &event = simulation->m_toi_events[i];
        event.m_time += time;
    }
}

// @address 0x6a1588 ?phys_toi_are_velocities_ok@@YAHMABUphys_constraint_schema_info@@PBMHABUphys_process_collision_input@@@Z
int phys_toi_are_velocities_ok(float body_delta_time, const phys_constraint_schema_info &schema_desc,
    const float *velocities, int count, const phys_process_collision_input &input)
{
    int priority = static_cast<signed char>(schema_desc.m_contact_constraint->m_priority);
    switch (priority) {
    case 1: case 2: case 3: case 4: break;
    default: break;
    }
    phys_collision_quality_info *quality = input.m_agent_dispatcher->get_collision_quality_info(priority);
    float maximum = schema_desc.m_allowed_penetration_depth * quality->m_max_constraint_violation;
    int result = -1;
    for (int i = 0; i < count; ++i) {
        float distance = fabsf(velocities[i]) * body_delta_time;
        if (distance > maximum) {
            maximum = distance;
            result = i;
        }
    }
    return result;
}

// @address 0x6a1850 ?phys_toi_reset_entity_vel_accums@@YAXAAUphys_solver_resources@@ABV?$phys_array@PAVphys_entity@@@@@Z
void phys_toi_reset_entity_vel_accums(phys_solver_resources &resources, const phys_array<phys_entity *> &entities)
{
    int i = 0;
    if (entities.getSize() > 0) {
        do {
            phys_entity *entity = entities[i];
            phys_vel_accum *accumulator = (phys_vel_accum *)((char *)resources.m_vel_accums + entity->m_solver_data);
            phys_motion *motion = &entity->m_motion;
            accumulator->m_lin_vel = motion->m_linear_velocity;
            if (accumulator->m_mat_is_identity) {
                accumulator->m_ang_vel = motion->m_ang_velocity;
            } else {
                accumulator->m_ang_vel.vec_set_rotated_dir(accumulator->get_core_from_world(), motion->m_ang_velocity);
            }
            ++i;
        } while (i < entities.getSize());
    }
}

// @address 0x6a19b0 ?handle_all_tois_till@phys_continuous_simulation@@IAA?AW4phys_step_result@@M@Z
phys_step_result phys_continuous_simulation::handle_all_tois_till(float min_toi_time)
{
    while (1) {
        int earliest_event_idx = -1;
        float earliest_time = min_toi_time;
        for (int i = 0; i < m_toi_events.getSize(); i++) {
            phys_toi_event &toi_event = m_toi_events[i];
            if (toi_event.m_time < earliest_time) {
                earliest_event_idx = i;
                earliest_time = toi_event.m_time;
            } else if (toi_event.m_time == earliest_time && earliest_event_idx != -1) {
                unsigned int toi_uid_pair[2] = { toi_event.m_island_entities[0]->get_uid(), toi_event.m_island_entities[1]->get_uid() };
                unsigned int min_uid_pair[2] = { m_toi_events[earliest_event_idx].m_island_entities[0]->get_uid(),
                                            m_toi_events[earliest_event_idx].m_island_entities[1]->get_uid() };
                if (toi_uid_pair[0] < min_uid_pair[0] || (toi_uid_pair[0] == min_uid_pair[0] && toi_uid_pair[1] < min_uid_pair[1])) {
                    earliest_event_idx = i;
                    earliest_time = toi_event.m_time;
                }
            }
        }
        if (earliest_event_idx < 0) {
            return PHYS_STEP_RESULT_SUCCESS;
        }
        phys_toi_event event = m_toi_events[earliest_event_idx];
        m_toi_events.remove_at(earliest_event_idx);
        m_sim_cur_time = event.m_time;
        phys_step_info physics_step_info;
        physics_step_info.set(m_sim_cur_time, m_current_psi_time);
        m_world->m_dynamics_step_info.m_step_info = physics_step_info;
        m_world->m_collision_input->m_step_info = physics_step_info;
        m_world->lock_critical_ops();
        simulate_toi(m_world, event, m_physics_dt);
        m_world->unlock_and_flush_ops();
        m_toi_count++;
        if (phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED) {
            return PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_TOI_SOLVE;
        }
    }
}

// @address 0x650fc0 ?recollide_after_step_fail@phys_simulation@@IAA?AW4phys_step_result@@XZ
// @address 0x6a1c50 ?advance_time_internal@phys_continuous_simulation@@IAA?AW4phys_step_result@@XZ
phys_step_result phys_continuous_simulation::advance_time_internal()
{
    if (m_prev_step_result != PHYS_STEP_RESULT_SUCCESS) {
        phys_step_result result = recollide_after_step_fail();
        if (result != PHYS_STEP_RESULT_SUCCESS) {
            return result;
        }
    }
    float time = snap_sim_time_to_advance();
    if (m_toi_events.getSize() != 0) {
        phys_step_result result = handle_all_tois_till(time);
        if (result != PHYS_STEP_RESULT_SUCCESS) {
            m_prev_step_result = result;
            return result;
        }
    }
    m_sim_cur_time = time;
    m_prev_step_result = PHYS_STEP_RESULT_SUCCESS;
    return PHYS_STEP_RESULT_SUCCESS;
}

// @address 0x6a1da0 ?add_toi_event@phys_continuous_simulation@@QAAXABUphys_process_collision_output@@ABUphys_pair_agent_entry@@@Z
void phys_continuous_simulation::add_toi_event(const phys_process_collision_output &output, const phys_pair_agent_entry &pair_agent_entry)
{
    if (m_toi_events.getSize() == m_toi_events.get_sector_capacity()) {
        char buffer[512];
        phys_err_stream stream(buffer, 512);
        stream << "Toi event queue full, consider using HK_COLLIDABLE_QUALITY_DEBRIS for some objects or increase hkWorldCinfo::m_sizeOfToiEventQueue";
        phys_error *error = phys_singleton<phys_error>::s_instance;
        error->message(phys_error::MESSAGE_WARNING, 0xf0323454, buffer, ".\\dynamics\\world\\simulation\\continuous\\hkContinuousSimulation.cpp", 969);
        return;
    }
    phys_toi_event &event = *m_toi_events.expand_by_unchecked(1);
    event.m_time = output.m_collision_toi.m_time;
    event.m_separating_vel = output.m_collision_toi.m_separating_speed;
    event.m_contact_point = output.m_collision_toi.m_contact_point;
    event.m_island_entities[0] = (phys_entity *)pair_agent_entry.m_collider[0]->get_collider_owner();
    event.m_island_entities[1] = (phys_entity *)pair_agent_entry.m_collider[1]->get_collider_owner();
    event.m_properties = output.m_collision_toi.m_properties;
    event.m_contact_manager = pair_agent_entry.m_contact_manager;
}

// @address 0x6a2e58 ??0phys_continuous_simulation@@QAA@PAVphys_world@@@Z
phys_continuous_simulation::phys_continuous_simulation(phys_world *world) : phys_simulation(world)
{
    m_frame_counter = 1;
    m_default_toi_resource_manager = new phys_default_toi_resource_manager();
    m_toi_count = 0;
    m_toi_events.reserve(world->m_toi_event_queue_size);
}

// @address 0x6a2fa0 ?advance_time@phys_continuous_simulation@@UAA?AW4phys_step_result@@XZ
phys_step_result phys_continuous_simulation::advance_time()
{
    phys_step_result result = advance_time_internal();
    if (!(m_sim_cur_time < m_simulate_until_time)) {
        phys_world *world = m_world;
        if (world->m_post_sim_listeners.m_size != 0)
            phys_world_callback_util::notify_world_post_simulation(world);
    }
    return result;
}

// @address 0x6a3000 ?reset_collision_info_for_entities@phys_continuous_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@Vphys_bool@@@Z
void phys_continuous_simulation::reset_collision_info_for_entities(phys_entity **entities, int count, phys_world *world, phys_bool skip_reinit)
{
    phys_simulation::reset_collision_info_for_entities(entities, count, world, skip_reinit);
    if (count != 1)
        remove_toi_events_of_entities(entities, count);
    else
        remove_toi_events_of_entity(entities[0]);
}

// @address 0x6a31f0 ?collide_agent_continuous@phys_continuous_simulation@@IAAXPAUphys_pair_agent_entry@@ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
void phys_continuous_simulation::collide_agent_continuous(phys_pair_agent_entry *pair_agent_entry,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    output.reset();
    phys_collision_quality_info *quality = input.m_agent_dispatcher->get_collision_quality_info(static_cast<signed char>(pair_agent_entry->m_quality_idx));
    input.m_quality_info = quality;
    input.m_create_pred_agents = quality->m_use_ccd;
    phys_pair_agent_process(pair_agent_entry, input, output, pair_agent_entry->m_contact_manager);
    if (havok_get_available_memory() == 0)
        phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
    if (phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED)
        return;
    if (!output.is_array_empty())
        pair_agent_entry->m_contact_manager->process_contact(*pair_agent_entry->m_collider[0], *pair_agent_entry->m_collider[1], input, output);
    if (output.has_toi())
        add_toi_event(output, *pair_agent_entry);
}

// @address 0x6a38e0 ?collide_entities_narrowphase_ccd@phys_continuous_simulation@@QAAXPAPAVphys_entity@@HABUphys_process_collision_input@@@Z
void phys_continuous_simulation::collide_entities_narrowphase_ccd(phys_entity **entities, int count, const phys_process_collision_input &input)
{
    process_entity_agents(entities, count, input,
        static_cast<void (phys_simulation::*)(phys_pair_agent_entry *,
            const phys_process_collision_input &, phys_process_collision_output &)>(&phys_continuous_simulation::collide_agent_continuous),
            CONTACT_SEARCH_DEFAULT);
}

// @address 0x6a5148 ??4phys_solver_info@@QAAAAU0@ABU0@@Z
phys_solver_info &phys_solver_info::operator=(const phys_solver_info &other)
{
    m_one_const = other.m_one_const;
    m_query_tau = other.m_query_tau;
    m_solver_damping = other.m_solver_damping;
    m_friction_tau = other.m_friction_tau;
    m_gravity_per_substep = other.m_gravity_per_substep;
    m_gravity_per_step = other.m_gravity_per_step;
    m_integrate_vel_factor = other.m_integrate_vel_factor;
    m_inv_integrate_vel_factor = other.m_inv_integrate_vel_factor;
    m_damp_div_tau = other.m_damp_div_tau;
    m_tau_div_damp = other.m_tau_div_damp;
    m_damp_div_friction_tau = other.m_damp_div_friction_tau;
    m_friction_tau_div_damp = other.m_friction_tau_div_damp;
    m_contact_resting_vel = other.m_contact_resting_vel;
    for (unsigned int i = 0; i < 6; ++i)
        m_deactivation_info[i] = other.m_deactivation_info[i];
    m_delta_time = other.m_delta_time;
    m_inv_dt = other.m_inv_dt;
    m_step_count = other.m_step_count;
    m_micro_step_count = other.m_micro_step_count;
    m_inv_micro_steps = other.m_inv_micro_steps;
    m_inv_steps = other.m_inv_steps;
    m_coherent_solver_order = other.m_coherent_solver_order;
    for (unsigned int i = 0; i < 2; ++i)
        m_inactive_frame_select[i] = other.m_inactive_frame_select[i];
    m_integrate_sleep_counter = other.m_integrate_sleep_counter;
    return *this;
}

// @address 0x6a52f8 ??_Gphys_continuous_simulation@@UAAPAXI@Z
// @address 0x650e50 ??0phys_simulation@@QAA@PAVphys_world@@@Z
// @address 0x650ea0 ??1phys_simulation@@UAA@XZ
// @address 0x6b3dd0 ??0phys_default_toi_resource_manager@@QAA@XZ
// @address 0x6b3dc0 ??1phys_default_toi_resource_manager@@UAA@XZ
// @address 0x60a298 ?allocate_chunk@phys_thread_memory@@QAAPAXHW4phys_memory_class@@@Z
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z
// @address 0x60c180 ?reserve_array_storage@phys_array_util@@YAXPAXHH@Z
// @address 0x5bec98 ?__first_virtual_table_function__@phys_object@@UAAXXZ
// @address 0x650ef0 ?step_delta_time@phys_simulation@@UAA?AW4phys_step_result@@M@Z
// @address 0x651f60 ?integrate_world@phys_simulation@@UAA?AW4phys_step_result@@M@Z
// @address 0x651320 ?collide@phys_simulation@@UAA?AW4phys_step_result@@XZ
// @address 0x652798 ?collide_entities_discrete@phys_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@ABVphys_step_info@@W4find_contacts_mode@1@@Z
// @address 0x6a4878 ?simulate_toi@phys_continuous_simulation@@MAAXPAVphys_world@@AAUphys_toi_event@@M@Z
// @address 0x648cb0 ??$quick_sort_recursive@PAVphys_constraint@@P6A?AVphys_bool@@PBV1@0@Z@phys_algorithm@@YAXPAPAVphys_constraint@@HHP6A?AVphys_bool@@PBV1@1@Z@Z
// @address 0x648cb0 ??$quick_sort_recursive@PAVphys_entity@@P6A?AVphys_bool@@PBV1@0@Z@phys_algorithm@@YAXPAPAVphys_entity@@HHP6A?AVphys_bool@@PBV1@1@Z@Z

static __forceinline phys_bool is_fixed_or_keyframed(const phys_entity *entity)
{
    unsigned char type = static_cast<unsigned char>(entity->m_motion.m_type);
    return phys_bool(type == 7 || type == 6);
}

// @address 0x6a1688 ?phys_toi_activate_fixed_constraints@@YAXAAV?$phys_array@Uphys_constraint_schema_info@@@@AAHH@Z
void phys_toi_activate_fixed_constraints(
    phys_array<phys_constraint_schema_info> &constraint_status, int &first_inactive_status, int tail_check_cnt)
{
    for (int i = constraint_status.getSize() - tail_check_cnt; i < constraint_status.getSize(); i++)
    {
        phys_constraint_internal *internal = reinterpret_cast<phys_constraint_internal *>(constraint_status[i].m_contact_constraint->m_constraint_internal);
        phys_entity **body = internal->m_island_entities;
        if (is_fixed_or_keyframed(body[0]) || is_fixed_or_keyframed(body[1]))
        {
            phys_constraint_schema_info &first = constraint_status[first_inactive_status++];
            phys_constraint_schema_info &current = constraint_status[i];
            phys_constraint_schema_info temp = first;
            first = current;
            current = temp;
        }
    }
}

// @address 0x6a1780 ?phys_toi_activate_linked_constraints@@YAXAAV?$phys_array@Uphys_constraint_schema_info@@@@AAHAAV?$phys_fixed_array@E@@@Z
void phys_toi_activate_linked_constraints(
    phys_array<phys_constraint_schema_info> &constraint_status, int &first_inactive_status, phys_fixed_array<unsigned char> &entity_state)
{
    for (int i = first_inactive_status; i < constraint_status.getSize(); i++)
    {
        phys_constraint_internal *internal = reinterpret_cast<phys_constraint_internal *>(constraint_status[i].m_contact_constraint->m_constraint_internal);
        phys_entity **e = internal->m_island_entities;
        if (entity_state[e[0]->m_storage_idx] == 8 &&
            entity_state[e[1]->m_storage_idx] == 8)
        {
            phys_constraint_schema_info &first = constraint_status[first_inactive_status++];
            phys_constraint_schema_info &current = constraint_status[i];
            phys_constraint_schema_info temp = first;
            first = current;
            current = temp;
        }
    }
}



static __forceinline bool is_fixed_for_toi(const phys_entity *entity)
{
    return static_cast<unsigned char>(entity->m_motion.m_type) == 7;
}

__forceinline void phys_continuous_simulation::notify_toi_event_removed(phys_toi_event &event)
{
    phys_island *island = (!is_fixed_for_toi(event.m_island_entities[0]))
        ? event.m_island_entities[0]->m_owner_island
        : event.m_island_entities[1]->m_owner_island;
    event.m_contact_manager->remove_toi(
        event.m_island_entities[0]->m_collider,
        event.m_island_entities[1]->m_collider,
        *island,
        event.m_properties);
}

// @address 0x6a1ed8 ?remove_toi_events_of_entities@phys_continuous_simulation@@IAAXPAPAVphys_entity@@H@Z
void phys_continuous_simulation::remove_toi_events_of_entities(phys_entity **entities, int count)
{
    if (m_toi_events.getSize() == 0)
        return;
    phys_ptr_map_base<phys_ulong> entity_map;
    entity_map.reserve(count);
    for (int i = 0; i < count; i++)
        entity_map.insert(reinterpret_cast<phys_ulong>(entities[i]), 0);
    for (int i = 0; i < m_toi_events.getSize(); i++)
    {
        phys_toi_event &toi_event = m_toi_events[i];
        map_dummy *map_hit_a = entity_map.map_find_key(reinterpret_cast<phys_ulong>(toi_event.m_island_entities[0]));
        map_dummy *map_hit_b = entity_map.map_find_key(reinterpret_cast<phys_ulong>(toi_event.m_island_entities[1]));
        if (entity_map.is_valid_aabb(map_hit_a) || entity_map.is_valid_aabb(map_hit_b))
        {
            notify_toi_event_removed(toi_event);
            m_toi_events.remove_at(i--);
        }
    }
}

// @address 0x6a2098 ?remove_toi_events_of_entity@phys_continuous_simulation@@IAAXPAVphys_entity@@@Z
void phys_continuous_simulation::remove_toi_events_of_entity(phys_entity *entity)
{
    for (int i = 0; i < m_toi_events.getSize(); i++)
    {
        phys_toi_event &toi_event = m_toi_events[i];
        if (toi_event.m_island_entities[0] == entity || toi_event.m_island_entities[1] == entity)
        {
            notify_toi_event_removed(m_toi_events[i]);
            m_toi_events.remove_at(i--);
        }
    }
}

// @address 0x6a2bb8 ?phys_toi_backstep_freeze_island@@YAXMPAVphys_island@@AAV?$phys_fixed_array@E@@AAV?$phys_array@PAVphys_entity@@@@@Z
void phys_toi_backstep_freeze_island(float time, phys_island *island, phys_fixed_array<unsigned char> &entity_state, phys_array<phys_entity *> &active_entities)
{
    for (int e = 0; e < island->m_island_entities.m_size; ++e)
    {
        phys_entity *body = island->m_island_entities.m_data[e];
        phys_motion_state *motion_state = &body->get_motion()->m_motion_state;
        phys_swept_transform_util::back_step_motion_state(time, *motion_state);
        if (entity_state[e] != 8)
        {
            if (active_entities.m_size == active_entities.get_sector_capacity())
                phys_array_util::grow_array_storage(&active_entities, (int)sizeof(phys_entity *));
            active_entities.m_data[active_entities.m_size] = body;
            active_entities.m_size = active_entities.m_size + 1;
            entity_state[e] = 8;
        }
        __vector4 body_center_of_mass = motion_state->get_swept_transform().m_center_of_mass1.m_simd_quad;
        __vector4 rotation = motion_state->get_swept_transform().m_rotation_1.m_quat_vec.m_simd_quad;
        motion_state->get_swept_transform().m_center_of_mass0.m_simd_quad = body_center_of_mass;
        motion_state->get_swept_transform().m_rotation_0.m_quat_vec.m_simd_quad = rotation;
        motion_state->get_swept_transform().m_center_of_mass0(3) = time;
        motion_state->get_swept_transform().m_center_of_mass1(3) = 1.0f / (island->m_world->m_world_simulation->get_current_psi_time() - time);
    }
}

// @address 0x6a1cd8 ?remove_agent_collision_info@phys_continuous_simulation@@UAAXPAUphys_pair_agent_entry@@@Z
void phys_continuous_simulation::remove_agent_collision_info(phys_pair_agent_entry *pair_agent_entry)
{
    for (int i = m_toi_events.getSize() - 1; i >= 0; i--)
    {
        if (m_toi_events[i].m_contact_manager == pair_agent_entry->m_contact_manager)
        {
            m_toi_events.remove_at(i);
        }
    }
}

// @address 0x6a2cc8 ?phys_toi_check_critical_final@@YA?AVphys_bool@@AAUphys_solver_resources@@ABV?$phys_array@Uphys_constraint_schema_info@@@@AAUphys_process_collision_input@@PAV?$phys_array@Uconstraint_violation_info@phys_toi_resource_manager@@@@@Z
class phys_bool phys_toi_check_critical_final(struct phys_solver_resources &p0,
    class phys_array<struct phys_constraint_schema_info> const &p1, struct phys_process_collision_input &p2,
    class phys_array<struct phys_toi_resource_manager::constraint_violation_info> *p3)
{
    const phys_solver_resources &resources = p0;
    phys_bool all_checks_ok = true;

    for (int i = 0; i < p1.getSize(); ++i)
    {
        const phys_constraint_schema_info &schema_status = p1[i];

        if (schema_status.m_contact_constraint->m_priority != 4)
        {
            continue;
        }

        {
            float velocities[256];

            int velocity_count = phys_solve_toi_violating_vel(
                const_cast<phys_solver_info *>(resources.m_solver_info), schema_status.m_cur_schema, resources.m_vel_accums, 256, velocities);
            int failing_schema_idx = phys_toi_are_velocities_ok(resources.m_step_info->m_delta_time, schema_status, velocities, velocity_count, p2);

            if (failing_schema_idx >= 0)
            {
                if (!p3)
                {
                    return phys_bool(false);
                }
                all_checks_ok = false;

                phys_toi_resource_manager::constraint_violation_info *info = &p3->expand_one();
                info->m_contact_constraint = schema_status.m_contact_constraint;

                phys_constraint_data *data = schema_status.m_contact_constraint->get_chain_data();
                if (data->get_type_id() == 11)
                {
                    phys_simple_contact_data *contact_data = (phys_simple_contact_data *)schema_status.m_contact_constraint->get_chain_data();
                    info->m_contact_point = &contact_data->m_contact_atom->get_contact_points()[failing_schema_idx];
                    info->m_contact_point_props = contact_data->m_contact_atom->get_contact_point_props() + failing_schema_idx;
                }
                else
                {
                    info->m_contact_point = 0;
                    info->m_contact_point_props = 0;
                }
            }
        }
    }

    return all_checks_ok;
}


static __forceinline phys_bool phys_toi_is_constraint_interesting(phys_constraint *constraint, phys_constraint::constraint_priority min_priority)
{
    return min_priority <= constraint->m_priority;
}

// @address 0x6a23b8 ?phys_toi_check_constraints@@YAXAAUphys_solver_resources@@AAUphys_process_collision_input@@AAV?$phys_array@Uphys_constraint_schema_info@@@@AAHW4constraint_priority@phys_constraint@@AAV?$phys_fixed_array@E@@ABV?$phys_array@PAVphys_entity@@@@AAV7@@Z
void phys_toi_check_constraints(phys_solver_resources &solver_resources, phys_process_collision_input &bridge_process_input,
    phys_array<phys_constraint_schema_info> &constraint_status, int &first_inactive_status,
    phys_constraint::constraint_priority min_priority, phys_fixed_array<unsigned char> &entity_state,
    const phys_array<phys_entity *> &touched_bodies, phys_array<phys_entity *> &to_be_activated)
{
    const phys_solver_resources &resources = solver_resources;
    phys_vec4 *orig_vel_stack = phys_alloc_stack<phys_vec4>(touched_bodies.getSize() * 2);
    {
        phys_vec4 *orig_vel_cursor = orig_vel_stack;
        for (int i = 0; i < touched_bodies.getSize(); i++)
        {
            phys_entity *entity = touched_bodies[i];
            phys_vel_accum *accumulator = (phys_vel_accum *)((char *)resources.m_vel_accums + entity->m_solver_data);
            phys_motion *motion = entity->get_motion();
            *(orig_vel_cursor++) = accumulator->m_lin_vel;
            *(orig_vel_cursor++) = accumulator->m_ang_vel;
            phys_swept_transform_util::get_atom_velocity(motion->m_motion_state, accumulator->m_lin_vel,
                accumulator->m_ang_vel);
        }
    }

    {
        for (int i = first_inactive_status; i < constraint_status.getSize(); i++)
        {
            phys_constraint_schema_info &schema_status = constraint_status[i];

            if (phys_toi_is_constraint_interesting(schema_status.m_contact_constraint, min_priority))
            {
                phys_entity **e = reinterpret_cast<phys_constraint_internal *>(schema_status.m_contact_constraint->m_constraint_internal)->m_island_entities;

                int second_slot_idx = entity_state[e[0]->m_storage_idx] == 8;
                phys_entity *entity = e[second_slot_idx];

                if (entity_state[entity->m_storage_idx] == 4)
                {
                    phys_algorithm::swap(constraint_status[first_inactive_status++], constraint_status[i]);
                    continue;
                }

                {
                    float velocities[256];
                    int velocity_count = phys_solve_toi_violating_vel(
                        const_cast<phys_solver_info *>(resources.m_solver_info), schema_status.m_cur_schema, resources.m_vel_accums, 256, velocities);
                    phys_bool velocities_ok = (-1 == phys_toi_are_velocities_ok(resources.m_step_info->m_delta_time, schema_status,
                        velocities, velocity_count, bridge_process_input));

                    if (!velocities_ok)
                    {
                        entity_state[entity->m_storage_idx] = 4;
                        to_be_activated.array_push_back(entity);
                        phys_algorithm::swap(constraint_status[first_inactive_status++], constraint_status[i]);
                    }
                }
            }
        }
    }

    {
        phys_vec4 *orig_vel_cursor = orig_vel_stack;
        for (int i = 0; i < touched_bodies.getSize(); i++)
        {
            phys_entity *entity = touched_bodies[i];
            phys_vel_accum *accumulator = (phys_vel_accum *)((char *)resources.m_vel_accums + entity->m_solver_data);
            accumulator->m_lin_vel = *(orig_vel_cursor++);
            accumulator->m_ang_vel = *(orig_vel_cursor++);
        }
    }

    phys_free_stack<phys_vec4>(orig_vel_stack);
}

// @address 0x6a2f08 ??1phys_continuous_simulation@@UAA@XZ
phys_continuous_simulation::~phys_continuous_simulation()
{
    if (m_default_toi_resource_manager != 0)
        delete m_default_toi_resource_manager;

}

// @address 0x6a3050 ?collide_island_narrowphase_ccd@phys_continuous_simulation@@QAAXPAVphys_island@@ABUphys_process_collision_input@@@Z
void phys_continuous_simulation::collide_island_narrowphase_ccd(phys_island *toi_island, const phys_process_collision_input &input)
{
    phys_process_collision_output process_output(toi_island);
    phys_pair_agent_track &pair_agent_track = toi_island->m_agent_track;
    for (int track_sector_index = 0; track_sector_index < pair_agent_track.m_track_sectors.getSize(); )
    {
        phys_pair_agent_sector **sectors = pair_agent_track.m_track_sectors.m_data;
        phys_pair_agent_sector *cur_sector = sectors[track_sector_index];
        phys_pair_agent_entry *entry = cur_sector->get_track_begin();
        track_sector_index++;
        phys_pair_agent_entry *end_entry = (track_sector_index == pair_agent_track.m_track_sectors.getSize()) ?
            phys_add_byte_offset(entry, pair_agent_track.m_last_sector_bytes) : cur_sector->get_track_end();
        for ( ; entry < end_entry; entry = phys_add_byte_offset(entry, entry->m_size))
        {
            phys_collider *collider_a = entry->m_collider[0];
            phys_collider *collider_b = entry->m_collider[1];
            __dcbt(0, entry->m_contact_manager);
            __dcbt(0, phys_add_byte_offset(entry, 128));
            input.m_quality_info = input.m_agent_dispatcher->get_collision_quality_info(static_cast<signed char>(entry->m_quality_idx));
            input.m_create_pred_agents = input.m_quality_info->m_use_ccd;
            process_output.reset();
            phys_pair_agent_process(entry, input, process_output, entry->m_contact_manager);
            if (havok_get_available_memory() == 0)
                phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
            if (phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED)
                return;
            if (!process_output.is_array_empty())
                entry->m_contact_manager->process_contact(*collider_a, *collider_b, input, process_output);
            if (process_output.has_toi())
                add_toi_event(process_output, *entry);
        }
    }
}

// @address 0x6a18f0 ?phys_toi_restore_updated_transforms@@YAXPAVphys_island@@AAV?$phys_fixed_array@E@@@Z
void phys_toi_restore_updated_transforms(phys_island *island, phys_fixed_array<unsigned char> &entity_state)
{
    for (int i = 0; i < island->m_island_entities.m_size; i++)
    {
        if (entity_state[i] == 1 || entity_state[i] == 2)
        {
            phys_entity *entity = island->m_island_entities.m_data[i];
            phys_motion_state *motion_state = &entity->get_motion()->m_motion_state;
            phys_swept_transform_util::compute_transform_at_t1(motion_state->get_swept_transform(), motion_state->get_transform());
        }
    }
}

class phys_dynamics_contact_manager;
// @address 0x6a12f8 ?phys_toi_simple_collision_response@@YAXPAVphys_world@@ABUphys_toi_event@@MAAV?$phys_array@PAVphys_entity@@@@@Z
void phys_toi_simple_collision_response(class phys_world *p0, struct phys_toi_event const &p1, float p2, class phys_array<class phys_entity *> &p3)
{
    phys_entity *body[2] = { p1.m_island_entities[0], p1.m_island_entities[1] };
    phys_motion *motion[2];
    motion[0] = &body[0]->m_motion;
    motion[1] = &body[1]->m_motion;

    phys_simple_collide_params params;
    params.m_direction = p1.m_contact_point.m_separating_normal;
    params.m_external_sep_vel = p1.m_separating_vel;
    params.m_extra_sep_vel = 0.1f;
    params.m_extra_slope = p2;
    params.m_contact_friction = p1.m_properties.get_friction();
    params.m_contact_restitution = p1.m_properties.get_restitution();

    phys_simple_collision_response::solve_single_output result;
    phys_simple_collision_response::solve_single_contact(p1.m_contact_point, p1.m_time, params, motion[0], motion[1],
        (phys_dynamics_contact_manager *)p1.m_contact_manager, result);

    if (result.m_keyframed_vel_a < 0.0f && result.m_keyframed_vel_b < 0.0f)
    {
        if (!is_fixed_or_keyframed(body[0])) { p3.m_data[p3.m_size] = body[0]; p3.m_size = p3.m_size + 1; }
        if (!is_fixed_or_keyframed(body[1])) { p3.m_data[p3.m_size] = body[1]; p3.m_size = p3.m_size + 1; }
    }
    else if (result.m_keyframed_vel_a < 0.0f && result.m_keyframed_vel_b >= 0.0f)
    {
        if (!is_fixed_or_keyframed(body[0])) { p3.m_data[p3.m_size] = body[0]; p3.m_size = p3.m_size + 1; }
        else { p3.m_data[p3.m_size] = body[1]; p3.m_size = p3.m_size + 1; }
    }
    else if (result.m_keyframed_vel_a >= 0.0f && result.m_keyframed_vel_b < 0.0f)
    {
        if (!is_fixed_or_keyframed(body[1])) { p3.m_data[p3.m_size] = body[1]; p3.m_size = p3.m_size + 1; }
        else { p3.m_data[p3.m_size] = body[0]; p3.m_size = p3.m_size + 1; }
    }
    else
    {
        if (is_fixed_or_keyframed(body[0]) || (!is_fixed_or_keyframed(body[1]) && result.m_keyframed_vel_a > result.m_keyframed_vel_b))
        { p3.m_data[p3.m_size] = body[1]; p3.m_size = p3.m_size + 1; }
        else
        { p3.m_data[p3.m_size] = body[0]; p3.m_size = p3.m_size + 1; }
    }
}


static __forceinline phys_bool phys_toi_is_agent_interesting(phys_pair_agent_entry *entry, phys_constraint::constraint_priority min_priority)
{
    return min_priority <= (unsigned char)static_cast<signed char>(entry->m_quality_idx);
}

static __forceinline void phys_toi_update_transform_if_needed(phys_entity *entity, float time, phys_fixed_array<unsigned char> &entity_state)
{
    if (entity_state[entity->m_storage_idx] == 0)
    {
        entity_state[entity->m_storage_idx] = 1;
        phys_motion_state &ms = entity->m_motion.m_motion_state;
        phys_swept_transform_util::lerp_swept(ms.get_swept_transform(), time, ms.get_transform());
    }
}

static __forceinline void phys_toi_update_manifold(phys_pair_agent_entry *entry, phys_process_collision_input &input, phys_island *island)
{
    phys_process_collision_output process_output(0);
    process_output.m_next_free_contact = process_output.m_process_contact_points;
    process_output.m_collision_toi.m_time = 3.40282e+38f;
    process_output.m_potential_contacts = 0;
    input.m_quality_info = input.m_agent_dispatcher->get_collision_quality_info(5);
    process_output.m_constraint_owner = island;
    input.m_create_pred_agents = 
            input.m_agent_dispatcher->get_collision_quality_info(static_cast<signed char>(entry->m_quality_idx))->m_use_ccd;
    phys_pair_agent_process(entry, input, process_output, entry->m_contact_manager);
    if (!process_output.is_array_empty())
        entry->m_contact_manager->process_contact(*entry->m_collider[0], *entry->m_collider[1], input, process_output);
}

// @address 0x6a21c8 ?phys_toi_update_entity_manifolds@@YAXPAVphys_entity@@W4constraint_priority@phys_constraint@@AAUphys_process_collision_input@@MAAV?$phys_fixed_array@E@@@Z
void phys_toi_update_entity_manifolds(phys_entity *entity, phys_constraint::constraint_priority min_priority,
    phys_process_collision_input &input, float time, phys_fixed_array<unsigned char> &entity_state)
{
    phys_entity *self = entity;
    phys_step_info saved_step_info = input.m_step_info;
    input.m_step_info.m_end_time = time;
    input.m_step_info.m_delta_time = time - input.m_step_info.m_step_start_time;
    if (input.m_step_info.m_delta_time > 0.0f)
        input.m_step_info.m_inv_dt = 1.0f / input.m_step_info.m_delta_time;
    else
        input.m_step_info.m_inv_dt = 0.0f;

    for (int i = 0; i < self->m_collider.m_collision_entries.getSize(); i++)
    {
        phys_linked_collider::collision_entry &ce = self->m_collider.m_collision_entries[i];
        if (phys_toi_is_agent_interesting(ce.m_agent_entry, min_priority))
        {
            phys_entity *other_entity = static_cast<phys_entity *>(ce.m_linked_partner->get_collider_owner());
            if (other_entity->body_is_fixed() || entity_state[other_entity->m_storage_idx] != 8)
            {
                if (!other_entity->body_is_fixed())
                    phys_toi_update_transform_if_needed(other_entity, time, entity_state);
                phys_toi_update_manifold(ce.m_agent_entry, input, self->m_owner_island);
            }
        }
    }
    input.m_step_info = saved_step_info;
}

extern template void phys_algorithm::quick_sort_recursive(phys_constraint **, int, int, phys_bool (*)(const phys_constraint *, const phys_constraint *));
extern template void phys_algorithm::quick_sort_recursive(phys_entity **, int, int, phys_bool (*)(const phys_entity *, const phys_entity *));

// @address 0x6a2760 ?phys_toi_activate_entities_check@@YAXAAUphys_process_collision_input@@W4constraint_priority@phys_constraint@@MABV?$phys_array@PAVphys_entity@@@@AAV?$phys_fixed_array@E@@AAV4@AAV?$phys_array@PAVphys_constraint@@@@@Z
void phys_toi_activate_entities_check(phys_process_collision_input &collision_input,
    phys_constraint::constraint_priority min_priority, float time, const phys_array<phys_entity *> &to_be_activated,
    phys_fixed_array<unsigned char> &entity_state, phys_array<phys_entity *> &new_touched_bodies,
    phys_array<phys_constraint *> &new_touched_constraints)
{
    for (int i = 0; i < to_be_activated.getSize(); i++)
    {
        phys_entity *entity = to_be_activated[i];

        phys_toi_update_transform_if_needed(entity, time, entity_state);
        entity_state[entity->m_storage_idx] = 8;
        phys_toi_update_entity_manifolds(reinterpret_cast<phys_entity *>(entity), min_priority, collision_input, time, entity_state);

        {
            const phys_small_array<phys_constraint_internal> &entity_master_list = entity->m_master_constraints;
            for (int j = 0; j < entity_master_list.getSize(); j++)
            {
                const phys_constraint_internal *c = &entity_master_list[j];
                if (!phys_toi_is_constraint_interesting(c->m_contact_constraint, min_priority))
                    continue;

                phys_entity *other_entity = c->get_other_entity(reinterpret_cast<phys_entity *>(entity));
                if (other_entity->body_is_fixed())
                {
                    new_touched_constraints.array_push_back(c->m_contact_constraint);
                }
                else if (entity_state[other_entity->m_storage_idx] != 8)
                {
                    new_touched_constraints.array_push_back(c->m_contact_constraint);
                    if (entity_state[other_entity->m_storage_idx] < 2)
                    {
                        new_touched_bodies.array_push_back(reinterpret_cast<phys_entity *>(other_entity));
                        phys_toi_update_transform_if_needed(other_entity, time, entity_state);
                        entity_state[other_entity->m_storage_idx] = 2;
                    }
                }
            }
        }
        {
            const phys_array<phys_constraint *> &slave_constraints = entity->m_slave_constraints;
            const int new_constraint_base = new_touched_constraints.getSize();
            const int new_body_base = new_touched_bodies.getSize();

            for (int j = 0; j < slave_constraints.getSize(); j++)
            {
                phys_constraint_internal *c = reinterpret_cast<phys_constraint_internal *>(slave_constraints[j]->m_constraint_internal);
                if (!phys_toi_is_constraint_interesting(c->m_contact_constraint, min_priority))
                    continue;

                phys_entity *other_entity = c->get_other_entity(reinterpret_cast<phys_entity *>(entity));
                if (other_entity->body_is_fixed())
                {
                    new_touched_constraints.array_push_back(c->m_contact_constraint);
                }
                else if (entity_state[other_entity->m_storage_idx] != 8)
                {
                    new_touched_constraints.array_push_back(c->m_contact_constraint);
                    if (entity_state[other_entity->m_storage_idx] < 2)
                    {
                        new_touched_bodies.array_push_back(reinterpret_cast<phys_entity *>(other_entity));
                        phys_toi_update_transform_if_needed(other_entity, time, entity_state);
                        entity_state[other_entity->m_storage_idx] = 2;
                    }
                }
            }

            phys_algorithm::quick_sort(new_touched_constraints.m_data + new_constraint_base,
                new_touched_constraints.getSize() - new_constraint_base, less_constraint_ptr);
            phys_algorithm::quick_sort(new_touched_bodies.m_data + new_body_base, new_touched_bodies.getSize() - new_body_base, less_entity_ptr);
        }
    }
}


// @address 0x6a4458 ?reintegrate_recollide@phys_continuous_simulation@@UAAXPAPAVphys_entity@@HPAVphys_world@@@Z
void phys_continuous_simulation::reintegrate_recollide(phys_entity **entities, int solver_entity_count, phys_world *world)
{
    if (world->critical_ops_locked())
    {
        phys_world_op::reintegrate_recollide_batch_op op;
        op.m_island_entities = entities;
        op.m_entity_count = static_cast<unsigned short>(solver_entity_count);
        world->queue_world_op(op);
        return;
    }
    world->lock_critical_ops();

    const phys_step_info orig_step_info = world->m_collision_input->m_step_info;
    phys_step_info physics_step_info;
    physics_step_info.set(m_sim_cur_time, m_current_psi_time);
    world->m_collision_input->m_step_info = physics_step_info;

    for (int i = 0; i < solver_entity_count; i++)
    {
        phys_motion_state &ms = entities[i]->get_motion()->m_motion_state;
        phys_swept_transform_util::back_step_motion_state(m_sim_cur_time, ms);
    }
    phys_rigid_motion_step(physics_step_info, reinterpret_cast<phys_motion *const *>(entities), solver_entity_count, (int)offsetof(phys_entity, m_motion));

    reset_collision_info_for_entities(entities, solver_entity_count, world, false);
    collide_entities_broadphase_ccd(entities, solver_entity_count, world, 0);
    collide_entities_narrowphase_ccd(entities, solver_entity_count, *world->m_collision_input);

    world->unlock_and_flush_ops();
    world->m_collision_input->m_step_info = orig_step_info;
}

static __forceinline phys_bool phys_continuous_simulation_out_of_memory() {
    return phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED; }

// @address 0x6a4610 ?collide_internal@phys_continuous_simulation@@MAAXABVphys_step_info@@@Z
void phys_continuous_simulation::collide_internal(const phys_step_info &step_info_in)
{
    if (m_prev_step_result != PHYS_STEP_RESULT_SUCCESS)
    {
        for (int i = m_toi_events.getSize() - 1; i >= 0; i--)
        {
            notify_toi_event_removed(m_toi_events[i]);
        }
        m_toi_events.clear();
    }

    {
        m_world->m_dynamics_step_info.m_step_info = step_info_in;
        m_world->m_collision_input->m_step_info = step_info_in;

        phys_solver_info &solver_info = m_world->m_dynamics_step_info.m_solver_info;
        solver_info.m_delta_time = step_info_in.m_delta_time * solver_info.m_inv_steps;
        solver_info.m_inv_dt = step_info_in.m_inv_dt * solver_info.m_step_count;
    }

    {
        m_world->lock_critical_ops();
        const phys_array<phys_island *> &world_active_islands = m_world->get_active_islands();
        for (int i = 0; i < world_active_islands.getSize(); ++i)
        {
            phys_island *island = world_active_islands[i];
            collide_entities_broadphase_ccd(island->m_island_entities.m_data, island->m_island_entities.m_size, m_world, 0);
            if (phys_continuous_simulation_out_of_memory())
            {
                m_world->unlock_and_flush_ops();
                return;
            }
        }
        m_world->unlock_and_flush_ops();
    }

    {
        m_world->lock_critical_ops();
        const phys_array<phys_island *> &world_active_islands = m_world->get_active_islands();
        for (int i = 0; i < world_active_islands.getSize(); ++i)
        {
            phys_island *island = world_active_islands[i];
            collide_island_narrowphase_ccd(island, *m_world->m_collision_input);
            if (phys_continuous_simulation_out_of_memory())
            {
                m_world->unlock_and_flush_ops();
                return;
            }
            if (m_world->m_island_post_collide_listeners.getSize())
            {
                phys_world_callback_util::notify_island_post_collide(m_world, island, step_info_in);
            }
        }
        m_world->unlock_and_flush_ops();
    }
}



// @address 0x6a3300 ?collide_entities_broadphase_ccd@phys_continuous_simulation@@QAAXPAPAVphys_entity@@HPAVphys_world@@PAC@Z
void phys_continuous_simulation::collide_entities_broadphase_ccd(phys_entity **entities, int solver_entity_count, phys_world *world,
    signed char *export_finished)
{
    if (!solver_entity_count)
    {
        return;
    }
    phys_local_array<phys_broadphase_handle_pair> new_pairs(world->m_broadphase_update_size);
    phys_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(world->m_broadphase_update_size);
    {
        phys_aabb *aabb_list = phys_alloc_stack<phys_aabb>(solver_entity_count);
        const phys_broadphase_handle **broadphase_handles = phys_alloc_stack<const phys_broadphase_handle *>(solver_entity_count);
        const float tolerance = world->m_collision_input->m_box_tolerance * 0.5f;
        int aabb_count = solver_entity_count;
        {
            phys_entity **e = entities;
            phys_aabb *aabb = aabb_list;
            const phys_broadphase_handle **handle = broadphase_handles;
            for (int i = aabb_count - 1; i >= 0; e++, i--)
            {
                phys_entity *entity = *e;
                const phys_collider *collidable = &entity->m_collider;
                const phys_motion_state *motion_state = static_cast<const phys_motion_state *>(collidable->m_motion);
                float aabb_tolerance = tolerance + motion_state->m_delta_angle(3) * motion_state->m_object_radius;
                handle[0] = &collidable->m_broadphase_handle;
                collidable->m_proxy_shape->get_handle_aabb(*static_cast<const phys_transform *>(collidable->m_motion), aabb_tolerance, *aabb);
                {
                    phys_vec4 radius; radius.set_all3(motion_state->m_object_radius + tolerance);
                    phys_vec4 max_reach; max_reach.set_add4(motion_state->get_swept_transform().m_center_of_mass1, radius);
                    phys_vec4 min_radius_corner; min_radius_corner.set_sub4(motion_state->get_swept_transform().m_center_of_mass1, radius);
                    aabb->m_min_limit.set_max4(aabb->m_min_limit, min_radius_corner);
                    aabb->m_max_limit.set_min4(aabb->m_max_limit, max_reach);
                }
                {
                    phys_vec4 diff; diff.set_sub4(motion_state->get_swept_transform().m_center_of_mass0, motion_state->get_swept_transform().m_center_of_mass1);
                    phys_vec4 zero; zero.set_zero4();
                    phys_vec4 diff_min; diff_min.set_min4(zero, diff);
                    phys_vec4 diff_max; diff_max.set_max4(zero, diff);
                    aabb->m_min_limit.add4(diff_min);
                    aabb->m_max_limit.add4(diff_max);
                }
                aabb++;
                handle++;
            }
        }
        {
            world->m_broadphase->lock();
            phys_broadphase_handle **mutable_handles = const_cast<phys_broadphase_handle **>(broadphase_handles);
            world->m_broadphase->update_aabbs(mutable_handles, aabb_list, aabb_count, new_pairs, sweep_removed_pairs);
            world->m_broadphase->unlock();
        }
        phys_free_stack(broadphase_handles);
        phys_free_stack(aabb_list);
    }
    if (export_finished && *export_finished != 1)
    {
        volatile signed char *finished = export_finished;
        while (*finished != 1)
        {
        }
    }
    if (new_pairs.getSize() + sweep_removed_pairs.getSize() > 0)
    {
        phys_typed_broadphase_dispatcher::remove_duplicate_pairs(new_pairs, sweep_removed_pairs);
        phys_island *island = entities[0]->m_owner_island;
        world->lock_island_for_constraints(island);
        world->m_broadphase_dispatcher->remove_handle_pairs(static_cast<phys_typed_broadphase_pair *>(sweep_removed_pairs.m_data),
            sweep_removed_pairs.getSize());
        unsigned int available_memory = havok_get_available_memory();
        if (new_pairs.getSize() * 1024u > available_memory)
        {
            phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
        }
        else
        {
            const phys_collider_pair_filter *filter = world->m_collision_filter;
            world->m_broadphase_dispatcher->add_handle_pairs(static_cast<phys_typed_broadphase_pair *>(new_pairs.m_data), new_pairs.getSize(), filter);
        }
        world->unlock_island_constraints(entities[0]->m_owner_island);
    }
}

static __forceinline phys_linked_collider *phys_continuous_simulation_linked_collider(phys_entity *entity)
{
    return static_cast<phys_linked_collider *>(&entity->m_collider);
}
static __forceinline phys_entity *phys_continuous_simulation_entity(phys_entity *entity)
{
    return entity;
}
static __forceinline void phys_toi_build_step_infos(phys_world *world, const phys_toi_event &event, float physics_delta_time, phys_step_info &step_info_out,
    phys_solver_info &solver_info_out, phys_constraint_query_in &constraint_query)
{
    step_info_out.m_step_start_time = event.m_time;
    step_info_out.m_end_time = world->m_world_simulation->get_current_psi_time();
    step_info_out.m_delta_time = step_info_out.m_end_time - step_info_out.m_step_start_time;
    step_info_out.m_inv_dt = 1.0f / step_info_out.m_delta_time;
    solver_info_out = world->m_dynamics_step_info.m_solver_info;
    solver_info_out.m_step_count = 1;
    solver_info_out.m_inv_steps = 1.0f;
    solver_info_out.m_micro_step_count = 1;
    solver_info_out.m_inv_steps = 1.0f;
    solver_info_out.m_delta_time = physics_delta_time;
    solver_info_out.m_inv_dt = 1.0f / physics_delta_time;
    solver_info_out.m_gravity_per_substep.set_zero4();
    solver_info_out.m_gravity_per_step.set_zero4();
    solver_info_out.set_tau_and_damping(0.5f, 1.4f);
    constraint_query.set(solver_info_out, step_info_out);
}

__forceinline void phys_continuous_simulation::delete_island_toi_events(
    const phys_island *island, const phys_fixed_array<unsigned char> &entity_state, phys_array<phys_toi_event> &toi_events)
{
    for (int i = toi_events.getSize() - 1; i >= 0; i--)
    {
        phys_toi_event &event = toi_events[i];
        if ((phys_continuous_simulation_entity(event.m_island_entities[0])->m_owner_island == island &&
            entity_state[phys_continuous_simulation_entity(event.m_island_entities[0])->m_storage_idx] == 8) ||
            (phys_continuous_simulation_entity(event.m_island_entities[1])->m_owner_island == island &&
            entity_state[phys_continuous_simulation_entity(event.m_island_entities[1])->m_storage_idx] == 8))
        {
            notify_toi_event_removed(toi_events[i]);
            toi_events.remove_at(i);
        }
    }
}

__forceinline void phys_continuous_simulation::collide_island_entities_narrowphase_ccd(
    phys_entity **entities, int solver_entity_count, const phys_process_collision_input &input)
{
    phys_island *island = entities[0]->m_owner_island;
    int total_body_cnt = island->m_island_entities.m_size;
    phys_local_array<phys_bool> processed_flags(total_body_cnt);
    processed_flags.m_size = total_body_cnt;
    phys_string::mem_fill(processed_flags.m_data, 0, total_body_cnt);
    phys_process_collision_output process_output(island);
    for (int i = 0; i < solver_entity_count; i++)
    {
        phys_entity *entity = phys_continuous_simulation_entity(entities[i]);
        phys_linked_collider *linked_collider = phys_continuous_simulation_linked_collider(entity);
        processed_flags[entity->m_storage_idx] = true;
        for (int a = 0; a < linked_collider->m_collision_entries.getSize(); a++)
        {
            phys_linked_collider::collision_entry &coll_entry = linked_collider->m_collision_entries[a];
            phys_entity *partner = static_cast<phys_entity *>(coll_entry.m_linked_partner->get_collider_owner());
            if (partner->m_owner_island == island && processed_flags[partner->m_storage_idx])
            {
                continue;
            }
            phys_pair_agent_entry *entry = coll_entry.m_agent_entry;
            process_output.reset();
            input.m_quality_info = input.m_agent_dispatcher->get_collision_quality_info(static_cast<signed char>(entry->m_quality_idx));
            input.m_create_pred_agents = input.m_quality_info->m_use_ccd;
            phys_pair_agent_process(entry, input, process_output, entry->m_contact_manager);
            if (havok_get_available_memory() == 0)
                phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
            if (phys_continuous_simulation_out_of_memory())
            {
                return;
            }
            if (!process_output.is_array_empty())
            {
                entry->m_contact_manager->process_contact(*entry->m_collider[0], *entry->m_collider[1], input, process_output);
            }
            if (process_output.has_toi())
            {
                add_toi_event(process_output, *entry);
            }
        }
    }
}

__forceinline void phys_continuous_simulation::collide_island_entities_ccd(
    phys_entity **entities, int solver_entity_count, phys_world *world, const phys_process_collision_input &input)
{
    if (solver_entity_count)
    {
        collide_entities_broadphase_ccd(entities, solver_entity_count, world, 0);
        if (phys_continuous_simulation_out_of_memory())
        {
            return;
        }
        collide_island_entities_narrowphase_ccd(entities, solver_entity_count, input);
    }
}

// @address 0x6a4878 ?simulate_toi@phys_continuous_simulation@@MAAXPAVphys_world@@AAUphys_toi_event@@M@Z
void phys_continuous_simulation::simulate_toi(phys_world *world, phys_toi_event &event, float physics_delta_time)
{
    if (!g_phys_toi_simulation_enabled)
    {
        g_phys_toi_simulation_enabled = process_flying_colors(3);
        if (!g_phys_toi_simulation_enabled)
            return;
    }
    phys_toi_resources toi_resources;
    if (PHYS_FAILURE == m_default_toi_resource_manager->begin_toi_setup_resources(event, m_toi_events, toi_resources))
    {
        return;
    }
    world->lock_critical_ops();
    {
        phys_step_info step_info;
        phys_solver_info solver_info;
        phys_constraint_query_in constraint_query_in;
        phys_toi_build_step_infos(world, event, physics_delta_time, step_info, solver_info, constraint_query_in);
        phys_solver_resources solver_resources;
        phys_solver_setup::init_solver_state(
            step_info, solver_info, constraint_query_in, toi_resources.m_scratch_buffer, toi_resources.m_scratch_buffer_size, solver_resources);
        phys_island *island = (!is_fixed_for_toi(event.m_island_entities[0]))
            ? event.m_island_entities[0]->m_owner_island
            : event.m_island_entities[1]->m_owner_island;
        int island_size = island->m_island_entities.m_size;
        phys_local_buffer<unsigned char> entity_state(island_size);
        phys_string::mem_fill(entity_state.begin(), 0, island_size);
        phys_continuous_sim_inline_array<phys_entity *, 64> active_entities;
        const phys_result local_solve_result = phys_toi_localized_solve(
            toi_resources, solver_resources, event, *m_default_toi_resource_manager, world, active_entities, entity_state);
        phys_solver_setup::shutdown_solver(solver_resources);
        notify_toi_event_removed(event);
        if (PHYS_SUCCESS == local_solve_result)
        {
            delete_island_toi_events(island, entity_state, m_toi_events);
            phys_step_info collide_step_info;
            collide_step_info.set(event.m_time, m_current_psi_time);
            world->m_collision_input->m_step_info = collide_step_info;
            collide_island_entities_ccd(active_entities.m_data, active_entities.getSize(), world, *world->m_collision_input);
        }
    }
    m_default_toi_resource_manager->end_toi_and_free_resources(event, m_toi_events, toi_resources);
    world->unlock_and_flush_ops();
}

struct phys_collision_input;
static __forceinline phys_bool phys_toi_is_fixed(const phys_entity *entity)
{
    return phys_bool(static_cast<unsigned char>(entity->m_motion.m_type) == 7);
}
static __forceinline void phys_toi_update_transform(phys_entity *entity, float time)
{
    phys_motion_state &ms = entity->m_motion.m_motion_state;
    phys_swept_transform_util::lerp_swept(ms.get_swept_transform(), time, ms.get_transform());
}
static __forceinline phys_collision_input &phys_continuous_simulation_collision_input(phys_world *world)
{
    return *reinterpret_cast<phys_collision_input *>(world->m_collision_input);
}

struct phys_pair_agent_sector;
static __forceinline phys_array<phys_pair_agent_sector *> &phys_continuous_simulation_sector_ptr_array(phys_array<phys_entity *> &arr)
{
    return reinterpret_cast<phys_array<phys_pair_agent_sector *> &>(arr);
}

// @address 0x6a38f0 ?phys_toi_localized_solve@@YA?AW4phys_result@@ABUphys_toi_resources@@AAUphys_solver_resources@@AAUphys_toi_event@@AAVphys_toi_resource_manager@@PAVphys_world@@AAV?$phys_array@PAVphys_entity@@@@AAV?$phys_fixed_array@E@@@Z
phys_result phys_toi_localized_solve(const phys_toi_resources &toi_resources, phys_solver_resources &solver_resources, phys_toi_event &event,
    phys_toi_resource_manager &toi_resource_manager, phys_world *world, phys_array<phys_entity *> &active_entities,
        phys_fixed_array<unsigned char> &entity_state)
{
    phys_continuous_sim_inline_array<phys_constraint_schema_info, 64> constraint_status;
    int first_inactive_status = 0;

    phys_island *island = (!phys_toi_is_fixed(event.m_island_entities[0]))
        ? phys_continuous_simulation_entity(event.m_island_entities[0])->m_owner_island
        : phys_continuous_simulation_entity(event.m_island_entities[1])->m_owner_island;

    phys_local_array<phys_entity *> touched_bodies(island->m_island_entities.m_size);
    {
        if (!phys_toi_is_fixed(event.m_island_entities[0]))
        {
            touched_bodies.push_back_unchecked(event.m_island_entities[0]);
            phys_toi_update_transform(phys_continuous_simulation_entity(event.m_island_entities[0]), event.m_time);
            entity_state[phys_continuous_simulation_entity(event.m_island_entities[0])->m_storage_idx] = 2;
        }
        if (!phys_toi_is_fixed(event.m_island_entities[1]))
        {
            touched_bodies.push_back_unchecked(event.m_island_entities[1]);
            phys_toi_update_transform(phys_continuous_simulation_entity(event.m_island_entities[1]), event.m_time);
            entity_state[phys_continuous_simulation_entity(event.m_island_entities[1])->m_storage_idx] = 2;
        }
    }

    phys_continuous_sim_inline_array<phys_entity *, 64> to_be_activated;
    {
        static_cast<phys_dynamics_contact_manager *>(event.m_contact_manager)->process_toi(event, world->m_toi_response_rotate_normal, to_be_activated);

        if (!to_be_activated.getSize())
        {
            if (!phys_toi_is_fixed(event.m_island_entities[0]))
            {
                phys_motion_state *motion_state = &phys_continuous_simulation_entity(event.m_island_entities[0])->m_motion.m_motion_state;
                phys_swept_transform_util::compute_transform_at_t1(motion_state->get_swept_transform(), motion_state->get_transform());
            }
            if (!phys_toi_is_fixed(event.m_island_entities[1]))
            {
                phys_motion_state *motion_state = &phys_continuous_simulation_entity(event.m_island_entities[1])->m_motion.m_motion_state;
                phys_swept_transform_util::compute_transform_at_t1(motion_state->get_swept_transform(), motion_state->get_transform());
            }
            return PHYS_FAILURE;
        }
    }

    phys_entity *initial_active_bodies[2] = { to_be_activated[0], to_be_activated.getSize() > 1 ? to_be_activated[1] : 0 };

    phys_solve_upload();

    phys_solver_setup::add_accumulators_internal(solver_resources, &touched_bodies[0], touched_bodies.getSize());

    phys_bool activation_preset = true;

    phys_continuous_sim_inline_array<phys_entity *, 64> extra_touched_bodies;
    phys_continuous_sim_inline_array<phys_constraint *, 64> extra_constraints;

    for (int full_steps_left = toi_resources.m_toi_solver_iters; full_steps_left > 0; full_steps_left--)
    {
        while (1)
        {
            extra_constraints.clear();
            extra_touched_bodies.clear();

            int updated_first_inactive = first_inactive_status;

            if (!activation_preset)
            {
                if (first_inactive_status < constraint_status.getSize())
                {
                    to_be_activated.clear();
                    phys_toi_check_constraints(solver_resources, *world->m_collision_input, constraint_status,
                        updated_first_inactive,
                        (phys_constraint::constraint_priority)toi_resources.m_min_process_priority, entity_state, touched_bodies,
                        to_be_activated);
                    if (!to_be_activated.getSize())
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            phys_toi_activate_entities_check(*world->m_collision_input,
                (phys_constraint::constraint_priority)toi_resources.m_min_process_priority, event.m_time, to_be_activated, entity_state,
                extra_touched_bodies, extra_constraints);

            activation_preset = false;

            int active_body_cnt;
            if (!phys_solver_setup::has_room_for_accumulators(solver_resources, extra_touched_bodies.begin(), extra_touched_bodies.getSize())
                || !phys_solver_setup::has_room_for_schemas(solver_resources, extra_constraints.begin(), extra_constraints.getSize())
                || toi_resources.m_max_constraints < constraint_status.getSize() + extra_constraints.getSize()
                || toi_resources.m_max_entities < (active_body_cnt = active_entities.getSize()) + touched_bodies.getSize() + extra_touched_bodies.getSize()
                || toi_resources.m_max_active_entities < active_body_cnt + to_be_activated.getSize())
            {
                {
                    for (int i = 0; i < to_be_activated.getSize(); i++)
                    {
                        phys_entity **entities = to_be_activated.m_data;
                        entity_state[phys_continuous_simulation_entity(entities[i])->m_storage_idx] = 2;
                    }
                }
                {
                    for (int i = 0; i < extra_touched_bodies.getSize(); i++)
                    {
                        phys_entity **entities = extra_touched_bodies.m_data;
                        entity_state[phys_continuous_simulation_entity(entities[i])->m_storage_idx] = 1;
                    }
                }

                int response = toi_resource_manager.on_resources_depleted();

                if (response == 2)
                {
                    phys_toi_backstep_freeze_island(event.m_time, island, entity_state, active_entities);

                    for (int i = 0; i < active_entities.getSize(); i++)
                    {
                        phys_world_agent_util::invalidate_sweep_info(active_entities[i], phys_continuous_simulation_collision_input(world));
                    }

                    active_entities.clear();
                    return PHYS_SUCCESS;
                }

                break;
            }
            else
            {
                phys_solver_setup::add_accumulators_internal(solver_resources, extra_touched_bodies.begin(), extra_touched_bodies.getSize());
                phys_solver_setup::add_schemas_internal(solver_resources, extra_constraints.begin(), extra_constraints.getSize(), constraint_status);

                first_inactive_status = updated_first_inactive;

                {
                    for (int i = 0; i < touched_bodies.getSize(); i++)
                    {
                        // match: 0x6a38f0: named array bases preserve the indexed-load operand ranks.
                        phys_entity **entities = touched_bodies.m_data;
                        if (entity_state[phys_continuous_simulation_entity(entities[i])->m_storage_idx] == 8)
                        {
                            touched_bodies.remove_at(i--);
                        }
                    }
                }

                phys_continuous_simulation_sector_ptr_array(active_entities)
                    .insert_at(active_entities.getSize(), reinterpret_cast<phys_pair_agent_sector *const *>(to_be_activated.begin()),
                        to_be_activated.getSize());

                {
                    touched_bodies.reserve(touched_bodies.getSize() + extra_touched_bodies.getSize());
                    for (int i = 0; i < extra_touched_bodies.getSize(); i++)
                    {
                        phys_entity **entities = extra_touched_bodies.m_data;
                        if (phys_continuous_simulation_entity(entities[i])->m_motion.m_type != 6)
                        {
                            touched_bodies.push_back_unchecked(extra_touched_bodies[i]);
                        }
                    }
                }

                phys_toi_activate_fixed_constraints(constraint_status, first_inactive_status, extra_constraints.getSize());
                phys_toi_activate_linked_constraints(constraint_status, first_inactive_status, entity_state);

                phys_solver_setup::sub_solve(solver_resources, phys_solver_setup::SOLVER_PASS_MODE_B);
            }

            if (!extra_constraints.getSize() && !extra_touched_bodies.getSize())
            {
                break;
            }
        }

        {
            phys_solver_setup::sub_solve(solver_resources, phys_solver_setup::SOLVER_PASS_MODE_A);
        }
    }

    const int top_priority_schemas = 1;
    if (solver_resources.m_schema_list[top_priority_schemas].m_begin != solver_resources.m_schema_list[top_priority_schemas].m_current)
    {
        phys_toi_reset_entity_vel_accums(solver_resources, touched_bodies);
        phys_bool forced_constraints_ok = phys_toi_check_critical_final(solver_resources, constraint_status, *world->m_collision_input, 0);
        if (!forced_constraints_ok)
        {
            int tries_left = toi_resources.m_forced_toi_final_iters;
            while (!forced_constraints_ok && (--tries_left >= 0))
            {
                phys_solve_step_jacobians(*solver_resources.m_solver_info,
            reinterpret_cast<const phys_schema *>(solver_resources.m_schema_list[top_priority_schemas].m_begin),
                    solver_resources.m_vel_accums, reinterpret_cast<phys_solver_elem_temp *>(solver_resources.m_setup_ptr_64));

                phys_toi_reset_entity_vel_accums(solver_resources, touched_bodies);
                forced_constraints_ok = phys_toi_check_critical_final(solver_resources, constraint_status, *world->m_collision_input, 0);
            }
        }

        if (!forced_constraints_ok)
        {
            phys_continuous_sim_inline_array<phys_toi_resource_manager::constraint_violation_info, 32> violated_constraints;

            phys_toi_check_critical_final(solver_resources, constraint_status, *world->m_collision_input, &violated_constraints);

            int response = toi_resource_manager.on_toi_unsolvable(violated_constraints);
            if (response == 2)
            {
                phys_toi_backstep_freeze_island(event.m_time, island, entity_state, active_entities);

                {
                    for (int i = 0; i < active_entities.getSize(); i++)
                    {
                        phys_world_agent_util::invalidate_sweep_info(active_entities[i], phys_continuous_simulation_collision_input(world));
                    }
                }

                active_entities.clear();
                return PHYS_SUCCESS;
            }
        }
    }

    if (!active_entities.getSize())
    {
        phys_continuous_simulation_sector_ptr_array(active_entities).insert_at(0,
            reinterpret_cast<phys_pair_agent_sector *const &>(initial_active_bodies[0]));
        entity_state[phys_continuous_simulation_entity(active_entities[0])->m_storage_idx] = 8;
        if (initial_active_bodies[1] != 0)
        {
            phys_continuous_simulation_sector_ptr_array(active_entities).insert_at(1,
            reinterpret_cast<phys_pair_agent_sector *const &>(initial_active_bodies[1]));
            entity_state[phys_continuous_simulation_entity(active_entities[1])->m_storage_idx] = 8;
        }
    }

    {
        for (int e = 0; e < active_entities.getSize(); e++)
        {
            phys_entity &body = *active_entities[e];
            phys_motion_state &motion_state = phys_continuous_simulation_entity(&body)->m_motion.m_motion_state;
            phys_swept_transform_util::back_step_motion_state(event.m_time, motion_state);
        }
    }

    phys_solver_setup::integrate_one_step(*solver_resources.m_solver_info, *solver_resources.m_step_info, solver_resources.m_vel_accums,
        active_entities.begin(), active_entities.getSize());

    phys_toi_restore_updated_transforms(island, entity_state);

    {
        for (int i = 0; i < active_entities.getSize(); i++)
        {
            phys_world_agent_util::invalidate_sweep_info(active_entities[i], phys_continuous_simulation_collision_input(world));
        }
    }

    return PHYS_SUCCESS;
}
