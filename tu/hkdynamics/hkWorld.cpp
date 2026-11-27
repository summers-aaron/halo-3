// hkdynamics/hkWorld.obj: from the records 0x63cb68 0x63cb78 0x63cce8 0x63ccf8 0x63cd00 0x63cd08 0x63cda0 0x63cdb8 0x63cdd0 0x63ce68 0x63ce70 0x63ce78 0x63ce80 0x63ce88 0x63ce90 0x63cf00 0x63cf08 0x63cf10 0x63cf28 0x63cf40 0x63cf58 0x63cf60 0x63cf68 0x63d018 0x63d020 0x63d0e0 0x63d0e8 0x63d140 0x63d158 0x63d160 0x63d178 0x63d198 0x63d1b8 0x63d3b0 0x63d538 0x63d638 0x63d768 0x63d7b0 0x63d7f8 0x63d840 0x63d888 0x63d8d0 0x63d918 0x63d960 0x63d9a8 0x63d9f0 0x63da38 0x63da80 0x63dac8 0x63dc60 0x63dcb8 0x63ddf8 0x63dea8 0x63df08 0x63dfb8 0x63e060 0x63e170 0x63e258 0x63e308 0x63e378 0x63e3f0 0x63e540 0x63e5a8 0x63e610 0x63e628 0x63e6f0 0x63e810 0x63e880 0x63e8f0 0x63e960 0x63e9d0 0x63ea40 0x63eab0 0x63eb20 0x63eb90 0x63ec00 0x63ec70 0x63ece0 0x63ed50 0x63f488 0x63f510 0x63f670 0x63fff0 0x640130 0x640368 0x640720 0x640e30 0x6411e0 0x6413d8 0x641670 0x6417c8 0x641c90 0x642058 0x6432e0 0x643468 0x643600 0x6439d8 0x643b20 0x643b98 0x643c78 0x643cd8 0x643db0 0x643ed8 0x643fe8 0x644058 0x6440e0 0x6440e8 0x6440f0 0x6440f8 0x644100 0x644220 0x6442d8 0x6443f0
// @flags /TP /O2 /Oc
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ
#include "phys/phys_agent_vtables.h"
// @address 0x167ac ??_7phys_null_contact_manager@@6B@
// @address 0x16800 ??_7phys_null_contact_manager_factory@@6B@
// @address 0x1687c k_hk_world_linear_caster_vtable
#include "phys/phys_world_op_queue.h"
#include "phys/phys_world_callback_util.h"
// @address 0x63e258 ?get_world_as_one_system@phys_world@@QBAPAVphys_physics_system@@XZ
// @address 0x63e540 ?get_integration_mem_usage@phys_world@@QAAHXZ
// @address 0x63e5a8 ?lock@phys_world@@QAAXXZ
// @address 0x63e810 ?add_action_listener@phys_world@@QAAXPAVphys_action_listener@@@Z
// @address 0x63e880 ?add_constraint_listener@phys_world@@QAAXPAVphys_constraint_listener@@@Z
// @address 0x63e8f0 ?add_entity_listener@phys_world@@QAAXPAVphys_entity_listener@@@Z
// @address 0x63e960 ?add_volume_listener@phys_world@@QAAXPAVphys_volume_listener@@@Z
// @address 0x63e9d0 ?add_island_activation_listener@phys_world@@QAAXPAVphys_island_activation_listener@@@Z
// @address 0x63ea40 ?add_post_collide_listener@phys_world@@QAAXPAVphys_post_collide_listener@@@Z
// @address 0x63eab0 ?add_post_simulation_listener@phys_world@@QAAXPAVphys_post_simulation_listener@@@Z
// @address 0x63eb20 ?add_post_integrate_listener@phys_world@@QAAXPAVphys_post_integrate_listener@@@Z
// @address 0x63eb90 ?add_island_post_collide_listener@phys_world@@QAAXPAVphys_island_post_collide_listener@@@Z
// @address 0x63ec00 ?add_island_post_integrate_listener@phys_world@@QAAXPAVphys_island_post_integrate_listener@@@Z
// @address 0x63ec70 ?add_collision_listener@phys_world@@QAAXPAVphys_collision_listener@@@Z
// @address 0x63ece0 ?add_world_delete_listener@phys_world@@QAAXPAVphys_world_deletion_listener@@@Z
// @address 0x63fff0 ?activate_region@phys_world@@QAAXABVphys_aabb@@@Z
// @address 0x643c78 ?remove_action@phys_world@@QAAXPAVphys_action@@@Z
// @address 0x63d1b8 ?phys_world_setup_contact_factories@@YAXPAVphys_world@@PAVphys_dispatcher@@@Z
// @address 0x63e378 ?find_initial_contacts@phys_world@@QAAXPAPAVphys_entity@@H@Z
// @address 0x63e3f0 ?find_all_initial_contacts@phys_world@@QAAXXZ
// @address 0x63e6f0 ?add_volume@phys_world@@QAAPAVphys_volume@@PAV2@@Z
// @address 0x6440e0 ??_Ephys_null_collision_filter@@W7AAPAXI@Z
// @address 0x6440e8 ??_Ephys_null_collision_filter@@WM@AAPAXI@Z
// @address 0x6440f0 ??_Ephys_null_collision_filter@@WBA@AAPAXI@Z
// @address 0x6440f8 ??_Ephys_null_collision_filter@@WBE@AAPAXI@Z
// @address 0x644100 ??_Gphys_null_collision_filter@@UAAPAXI@Z
// @address 0x644220 ??_Gphys_typed_broadphase_dispatcher@@QAAPAXI@Z
// @address 0x6442d8 ??_Gphys_world_op_queue@@QAAPAXI@Z
// @address 0x6443f0 ??_Gphys_world@@UAAPAXI@Z
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "physics/havok_memory.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_base_types.h"
#include "phys/phys_physics_system.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_broadphase_border_listener.h"
#include "phys/phys_entity_pair_broadphase_listener.h"
#include "phys/phys_volume_broadphase_listener.h"
#include "phys/phys_broadphase_listener.h"
#include "phys/phys_local_array.h"
#include "phys/phys_default_maintenance_manager.h"
#include "phys/phys_world_maintenance_manager.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_default_convex_list_filter.h"
#include "phys/phys_convex_list_filter.h"
#include "phys/phys_base.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
#include "phys/phys_pair_agent_track.h"
#include "phys/phys_multithread_config.h"
#include "phys/phys_world.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_math.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_semaphore.h"
#include "phys/phys_step_info.h"
#include "phys/phys_error.h"
#include "phys/phys_string.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "phys/phys_collectors.h"
#include "phys/phys_thread_token.h"
#include "phys/phys_constraint_info.h"
#include "phys/phys_solver_elem_temp.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_action.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint_chain.h"
#include "phys/phys_constraint_chain_action.h"
#include "phys/phys_simple_constraint_contact_manager.h"
#include "phys/phys_report_contact_manager.h"
#include "phys/phys_edge_weld_util.h"
#include "phys/phys_null_agent.h"
#include "phys/phys_err_stream.h"
#include "phys/phys_world_op_queue_types.h"
#include "phys/phys_broadphase_border.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_entity_callback_util.h"

#include "phys/phys_simple_world_ray_caster.h"
#include "phys/phys_world_ray_caster.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_agent_config.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_bridge_data.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_thread.h"
#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_memory.h"
#include "phys/phys_half.h"
#include "phys/phys_collision_quality_info.h"
#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_island.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_world_linear_caster.h"

enum { k_havok_sdk_version_number = 40501 };

struct phys_memory_usage {
    void *m_dispatch_fn;
    phys_memory::memory_state m_memory_state;
    int m_memory_limit;
    int m_memory_ref_count;
    int m_sys_alloc_count;
    int m_allocated;
    int m_total_sys_memory;
    int m_current_sys_memory;
    int m_peak_sys_memory;
    int m_runtime_alloc_count;
    int m_runtime_allocated;
};

typedef char phys_memory_usage_offsets[
    (sizeof(void *) != 4 || (offsetof(phys_memory_usage, m_allocated) == 20 && offsetof(phys_memory_usage, m_runtime_allocated) == 40)) ? 1 : -1];

class phys_world_memory_watchdog : public phys_ref_object {
public:
    virtual void free_heap_memory(phys_world *world);
    int m_memory_limit;
    void watch_memory(phys_world *world);
};

#include "phys/phys_null_contact_manager_factory.h"

__forceinline void phys_world_memory_watchdog::watch_memory(phys_world *world)
{
    const phys_memory_usage *memory = static_cast<const phys_memory_usage *>(static_cast<const void *>(&phys_memory::get_singleton_instance()));
    if (memory->m_allocated + memory->m_runtime_allocated > m_memory_limit) {
        free_heap_memory(world);
    }
}

phys_contact_point_id phys_null_contact_manager::add_contact_point_impl(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_process_collision_input &, phys_process_collision_output &, const phys_simplex_cache *, phys_contact_point &)
{ return 0; }
phys_result phys_null_contact_manager::reserve_contact_points_impl(int) { return PHYS_SUCCESS; }
void phys_null_contact_manager::remove_contact_point_impl(phys_contact_point_id, phys_constraint_owner &) {}
void phys_null_contact_manager::process_contact_impl(const phys_collider &, const phys_collider &,
    const phys_process_collision_input &, phys_process_collision_data &) {}
phys_contact_manager::toi_verdict phys_null_contact_manager::add_toi_impl(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_process_collision_input &, phys_process_collision_output &, float, phys_contact_point &,
    const phys_simplex_cache *, float &, phys_contact_point_material &) { return TOI_EVENT_ACCEPT; }
void phys_null_contact_manager::remove_toi_impl(const phys_collider &, const phys_collider &,
    phys_constraint_owner &, phys_contact_point_material &) {}
void phys_null_contact_manager::cleanup() {}

namespace {
template <typename T>
static __forceinline int ptr_array_index_of(const phys_array<T *> &array, const void *item)
{
    for (int i = 0; i < array.m_size; ++i)
        if (array.m_data[i] == item)
            return i;
    return -1;
}

static __forceinline bool critical_section_held(const phys_critical_section *lock)
{
    return lock->m_current_thread != (unsigned __int64)-1;
}

static __forceinline void add_object_ref(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0)
        ++object->m_object_ref_count;
}

static __forceinline void remove_object_ref(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0) {
        if (--object->m_object_ref_count == 0) {
            if (halo_managed_memory_address(object)) {
                object->m_object_ref_count = 1;
            } else {
                delete object;
            }
        }
    }
}
}

static __forceinline int island_memory_usage(phys_island *island)
{
    int elem_temp_size =
        island->m_constraint_info.m_solver_temp_count * int(sizeof(phys_solver_elem_temp)) + 2 * int(sizeof(phys_solver_elem_temp));
    int accum_bytes =
        island->get_entities().getSize() * int(sizeof(phys_vel_accum)) + int(sizeof(phys_vel_accum)) + 16;
    return accum_bytes + elem_temp_size + island->m_constraint_info.m_schemas_size + PHYS_SIZE_OF_END_SCHEMA;
}

namespace {
static __forceinline phys_atom *last_atom_of(const phys_constraint_internal &constraint)
{
    phys_atom *atom = constraint.get_atoms();
    while (atom->is_modifier_type() != 0) {
        phys_modifier_atom *modifier = static_cast<phys_modifier_atom *>(atom);
        atom = modifier->m_child_shape;
    }
    return atom;
}

static __forceinline void physics_system_array_push(phys_array<phys_physics_system *> &array, phys_physics_system *system)
{
    if (array.m_size == (array.m_capacity_and_flags & 0x3fffffff))
        phys_array_util::grow_array_storage(&array, static_cast<int>(sizeof(phys_physics_system *)));
    array.m_data[array.m_size++] = system;
}

template <typename T>
static __forceinline void collect_array_stats(
    phys_statistics_collector *collector, const char *name, int type, const phys_array<T> &array)
{
    if ((array.m_capacity_and_flags & (int)0x80000000) == 0) {
        collector->add_chunk_stats(
            name,
            static_cast<phys_statistics_collector::stat_category>(type),
            array.m_data,
            array.m_size * static_cast<int>(sizeof(T)),
            (array.m_capacity_and_flags & 0x3fffffff) * static_cast<int>(sizeof(T)));
    }
}

template <typename T>
static __forceinline void collect_ptr_array_stats(
    phys_statistics_collector *collector, const char *name, int type, const phys_array<T *> &array)
{
    if ((array.m_capacity_and_flags & (int)0x80000000) == 0) {
        collector->add_chunk_stats(
            name,
            static_cast<phys_statistics_collector::stat_category>(type),
            array.m_data,
            array.m_size * static_cast<int>(sizeof(T *)),
            (array.m_capacity_and_flags & 0x3fffffff) * static_cast<int>(sizeof(T *)));
    }
}

static __forceinline void run_pending_world_ops(phys_world *world)
{
    world->m_pending_op_count = 0;
    world->m_pending_ops->execute_all_pending();
}

// match: hkWorld.obj inlines queue execution; shared-header users retain their accepted call boundary.
static __forceinline void try_pending_world_ops(phys_world *world)
{
    if (world->m_critical_ops_lock_count == 0 && world->m_pending_op_count != 0 && !world->m_block_pending_ops)
        run_pending_world_ops(world);
}

static __forceinline void unlock_and_try_world_ops(phys_world *world)
{
    --world->m_critical_ops_lock_count;
    try_pending_world_ops(world);
}
}

template <typename T>
static __forceinline void ptr_array_add(phys_array<T *> *array, T *item)
{
    if (array->m_size == (array->m_capacity_and_flags & 0x3fffffff))
        phys_array_util::grow_array_storage(array, static_cast<int>(sizeof(T *)));
    array->m_data[array->m_size] = item;
    array->m_size++;
}

namespace {
static __forceinline void mark_island_dirty(phys_world *world, phys_island *island)
{
    if (island->m_dirty_list_idx == 0xffff) {
        island->m_dirty_list_idx = (unsigned short)world->m_dirty_islands.m_size;
        phys_array<phys_island *> &dirty = world->m_dirty_islands;
        if (dirty.m_size == (dirty.m_capacity_and_flags & 0x3fffffff))
            phys_array_util::grow_array_storage(&dirty, static_cast<int>(sizeof(phys_island *)));
        dirty.m_data[dirty.m_size] = island;
        ++dirty.m_size;
    }
}
}

// @address 0x63cb68 ??0phys_multithread_config@@QAA@XZ
// @address 0x63cdb8 ?collide@phys_world@@QAA?AW4phys_step_result@@XZ
// @address 0x63ce80 ?step_begin_st@phys_world@@QAAXM@Z
// @address 0x63cf58 ?check_determinism@phys_world@@QAAXXZ
// @address 0x63cda0 ?integrate_world@phys_world@@QAA?AW4phys_step_result@@M@Z
// @address 0x63ce78 ?sim_at_psi@phys_world@@QBA_NXZ
// @address 0x63cf10 ?get_job_queue@phys_world@@QAAPAVphys_job_queue@@XZ
// @address 0x63cf08 ?reset_thread_tokens@phys_world@@QAAXXZ
// @address 0x63cf60 ?get_memory_watchdog@phys_world@@QBAPAVphys_world_memory_watchdog@@XZ
// @address 0x63cd00 ?set_low_freq_sleep_period@phys_world@@QAAXM@Z
// @address 0x63d158 ?queue_world_op@phys_world@@QAAXABUbase_op@phys_world_op@@@Z
// @address 0x63ce70 ?sim_at_marker@phys_world@@QBA_NXZ
// @address 0x63ce88 ?step_process_mt@phys_world@@QAAXABW4phys_thread_token@@@Z
// @address 0x63cf28 ?get_multithread_config@phys_world@@QAAXAAUphys_multithread_config@@@Z
// @address 0x63d140 ?execute_pending_ops@phys_world@@AAAXXZ
// @address 0x63ccf8 ?set_high_freq_sleep_period@phys_world@@QAAXM@Z
// @address 0x63ce68 ?set_frame_time_marker@phys_world@@QAAXM@Z
// @address 0x63cf40 ?set_multithread_config@phys_world@@QAAXABUphys_multithread_config@@@Z
// @address 0x63d018 ?check_access_active_islands@phys_world@@QBAXXZ
// @address 0x63d0e0 ?get_broadphase_border@phys_world@@QBAPAVphys_broadphase_border@@XZ
// @address 0x63da80 ?remove_world_deletion_listener@phys_world@@QAAXPAVphys_world_deletion_listener@@@Z
// @address 0x63d840 ?remove_volume_listener@phys_world@@QAAXPAVphys_volume_listener@@@Z
// @address 0x63d9f0 ?remove_island_post_integrate_listener@phys_world@@QAAXPAVphys_island_post_integrate_listener@@@Z
// @address 0x63d918 ?remove_world_post_simulation_listener@phys_world@@QAAXPAVphys_post_simulation_listener@@@Z
// @address 0x63d160 ?unlock@phys_world@@QAAXXZ
// @address 0x63d7f8 ?remove_entity_listener@phys_world@@QAAXPAVphys_entity_listener@@@Z
// @address 0x63da38 ?remove_collision_listener@phys_world@@QAAXPAVphys_collision_listener@@@Z
// @address 0x63d9a8 ?remove_island_post_collide_listener@phys_world@@QAAXPAVphys_island_post_collide_listener@@@Z
// @address 0x63d7b0 ?remove_constraint_listener@phys_world@@QAAXPAVphys_constraint_listener@@@Z
// @address 0x63d198 ?unlock_island_constraints@phys_world@@QAAXPAVphys_island@@@Z
// @address 0x63d178 ?is_locked@phys_world@@QAA_NXZ
// @address 0x63d8d0 ?remove_world_post_collide_listener@phys_world@@QAAXPAVphys_post_collide_listener@@@Z
// @address 0x63d768 ?remove_action_listener@phys_world@@QAAXPAVphys_action_listener@@@Z
// @address 0x63d888 ?remove_island_activation_listener@phys_world@@QAAXPAVphys_island_activation_listener@@@Z
// @address 0x63d960 ?remove_world_post_integrate_listener@phys_world@@QAAXPAVphys_post_integrate_listener@@@Z
// @address 0x63cd08 ?step_delta_time@phys_world@@QAA?AW4phys_step_result@@M@Z
// @address 0x63cdd0 ?advance_time@phys_world@@QAA?AW4phys_step_result@@XZ
// @address 0x63ce90 ?step_end_st@phys_world@@QAAXXZ
// @address 0x63cf68 ?set_memory_watchdog@phys_world@@QAAXPAVphys_world_memory_watchdog@@@Z
// @address 0x63d020 ?set_broadphase_border@phys_world@@QAAXPAVphys_broadphase_border@@@Z
// @address 0x63e308 ?queue_callback@phys_world@@QAAPAVuser_callback@phys_world_op@@PAV23@PAX@Z
// @address 0x63e610 ?lock_island_for_constraints@phys_world@@QAAXPAVphys_island@@@Z
// @address 0x643fe8 ?create_contact_manager@phys_null_contact_manager_factory@@UAAPAVphys_contact_manager@@ABVphys_collider@@0ABUphys_collision_input@@@Z
// @address 0x63dcb8 ?enumerate_inactive_entities@@YA?AVphys_bool@@PBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63ddf8 ?enumerate_active_entities@@YA?AVphys_bool@@PBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63dea8 ?enumerate_world_entities@@YAXPBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63df08 ?enumerate_island_constraints@@YAXPAVphys_island@@PAVphys_physics_system@@@Z
// @address 0x63dfb8 ?enumerate_world_constraints@@YAXPBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63e060 ?enumerate_world_actions@@YAXPBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63e170 ?enumerate_world_volumes@@YAXPBVphys_world@@PAVphys_physics_system@@@Z
// @address 0x63e628 ?attach_action_to_entity@phys_world@@QAAXPAVphys_action@@PAVphys_entity@@@Z
// @address 0x63d3b0 ?add_chain_entity@phys_world@@QAAPAVphys_entity@@PAV2@W4phys_entity_activation@@@Z
// @address 0x63d538 ?remove_entity@phys_world@@QAA?AVphys_bool@@PAVphys_entity@@@Z
// @address 0x63d638 ?remove_volume@phys_world@@QAAXPAVphys_volume@@@Z
// @address 0x63f510 ?get_world_as_systems@phys_world@@QBAXAAV?$phys_array@PAVphys_physics_system@@@@@Z
// @address 0x643b20 ?create_and_add_constraint@phys_world@@QAAPAVphys_constraint@@PAVphys_rigid_body@@0PAVphys_constraint_data@@@Z
// @address 0x63cb78 ?phys_world_update_volume_filter@@YAXPAVphys_volume@@PAVphys_collider@@PAVphys_collision_filter@@@Z
// @address 0x6439d8 ?add_world_constraint@phys_world@@QAAPAVphys_constraint@@PAV2@@Z
// @address 0x643b98 ?remove_constraint@phys_world@@QAA?AVphys_bool@@PAVphys_constraint@@@Z
// @address 0x6411e0 ?add_world_action@phys_world@@QAAPAVphys_action@@PAV2@@Z
// @address 0x6413d8 ?remove_action_now@phys_world@@QAAXPAVphys_action@@@Z
// @address 0x641670 ?detach_entity_action@phys_world@@QAAXPAVphys_action@@PAVphys_entity@@@Z
// @address 0x643ed8 ?set_collision_filter@phys_world@@QAAXPAVphys_collision_filter@@Vphys_bool@@W4phys_world_filter_update_mode@@W4phys_compound_filter_update_mode@@@Z
// @address 0x644058 ??0phys_null_collision_filter@@QAA@XZ
// @address 0x63cf00 ?get_thread_token@phys_world@@QAA?AW4phys_thread_token@@XZ
// @address 0x63d0e8 ?trace_ray@phys_world@@QBAXABUphys_world_ray_input@@AAVphys_ray_hit_collector@@@Z
// @address 0x63dc60 ?trace_ray@phys_world@@QBAXABUphys_world_ray_input@@AAUphys_world_ray_output@@@Z
// 0x63cb68 (natural source)
phys_multithread_config::phys_multithread_config() { m_max_serial_constraints = 70; }

// 0x63cb78 (natural source)
void phys_world_update_volume_filter(phys_volume *phantom, phys_collider *collidable, phys_collision_filter *filter)
{
    phys_bool overlapping = phantom->has_overlapping_collider(collidable);
    phys_collider *volume_collider = &phantom->m_collider;
    if (filter->allows_collision(*volume_collider, *collidable)) {
        if (!overlapping)
            phantom->insert_overlapping_collider(collidable);
        if (collidable->get_type_id() == 2) {
            phys_volume *other = (phys_volume *)collidable->get_collider_owner();
            phys_bool other_overlapping = other->has_overlapping_collider(volume_collider);
            if (!other_overlapping) {
                phys_volume *other_volume = (phys_volume *)collidable->get_collider_owner();
                other_volume->insert_overlapping_collider(volume_collider);
            }
        }
    } else {
        if (overlapping)
            phantom->erase_overlapping_collider(collidable);
        if (collidable->get_type_id() == 2) {
            phys_volume *other = (phys_volume *)collidable->get_collider_owner();
            phys_bool other_overlapping = other->has_overlapping_collider(volume_collider);
            if (other_overlapping) {
                phys_volume *other_volume = (phys_volume *)collidable->get_collider_owner();
                other_volume->erase_overlapping_collider(volume_collider);
            }
        }
    }
}

// @address 0x63cce8 ?set_world_gravity@phys_world@@QAAXABVphys_vec4@@@Z
void phys_world::set_world_gravity(const phys_vec4 &v)
{
    m_gravity = v;
}

// 0x63ccf8 (natural source)
void phys_world::set_high_freq_sleep_period(float period) { m_hf_sleep_period = period; }

// 0x63cd00 (natural source)
void phys_world::set_low_freq_sleep_period(float period) { m_low_freq_sleep_period = period; }

// 0x63cd08 (natural source)
phys_step_result phys_world::step_delta_time(float body_delta_time)
{
    phys_step_result result = m_world_simulation->step_delta_time(body_delta_time);
    if (result == PHYS_STEP_RESULT_SUCCESS && m_memory_watchdog != 0)
        m_memory_watchdog->watch_memory(this);
    return result;
}

// 0x63cda0 (natural source)
phys_step_result phys_world::integrate_world(float body_delta_time) { return m_world_simulation->integrate_world(body_delta_time); }

// 0x63cdb8 (natural source)
phys_step_result phys_world::collide() { return m_world_simulation->collide(); }

// 0x63cdd0 (natural source)
phys_step_result phys_world::advance_time()
{
    phys_step_result result = m_world_simulation->advance_time();
    if (result == PHYS_STEP_RESULT_SUCCESS && m_memory_watchdog != 0)
        m_memory_watchdog->watch_memory(this);
    return result;
}

// 0x63ce68 (natural source)
void phys_world::set_frame_time_marker(float frame_delta_time) { m_world_simulation->set_frame_time_marker(frame_delta_time); }

// 0x63ce70 (natural source)
bool phys_world::sim_at_marker() const { return m_world_simulation->sim_at_marker(); }

// 0x63ce78 (natural source)
bool phys_world::sim_at_psi() const { return static_cast<const phys_simulation *>(m_world_simulation)->sim_at_psi(); }

// 0x63ce80 (natural source)
void phys_world::step_begin_st(float body_delta_time) { static_cast<phys_multithreaded_simulation *>(m_world_simulation)->step_begin_st(body_delta_time); }

// 0x63ce88 (natural source)
void phys_world::step_process_mt(const phys_thread_token &thread_token)
{
    static_cast<phys_multithreaded_simulation *>(m_world_simulation)->step_process_mt(thread_token);
}

// 0x63ce90 (natural source)
void phys_world::step_end_st()
{
    static_cast<phys_multithreaded_simulation *>(m_world_simulation)->step_end_st();
    if (m_memory_watchdog != 0)
        m_memory_watchdog->watch_memory(this);
}

// 0x63cf00 (natural source)
phys_thread_token phys_world::get_thread_token()
{
    return static_cast<phys_multithreaded_simulation *>(m_world_simulation)->get_thread_token();
}

// 0x63cf08 (natural source)
void phys_world::reset_thread_tokens() { static_cast<phys_multithreaded_simulation *>(m_world_simulation)->reset_thread_tokens(); }

// 0x63cf10 (natural source)
phys_job_queue *phys_world::get_job_queue() { return static_cast<phys_multithreaded_simulation *>(m_world_simulation)->get_job_queue(); }

// 0x63cf28 (natural source)
void phys_world::get_multithread_config(phys_multithread_config &config)
{
    static_cast<phys_multithreaded_simulation *>(m_world_simulation)->get_multithread_config(config);
}

// 0x63cf40 (natural source)
void phys_world::set_multithread_config(const phys_multithread_config &config)
{
    static_cast<phys_multithreaded_simulation *>(m_world_simulation)->set_multithread_config(config);
}

// 0x63cf58 (natural source)
void phys_world::check_determinism() {}

// 0x63cf60 (natural source)
phys_world_memory_watchdog *phys_world::get_memory_watchdog() const { return m_memory_watchdog; }

// 0x63cf68 (natural source)
void phys_world::set_memory_watchdog(phys_world_memory_watchdog *memory_watchdog)
{
    if (memory_watchdog != 0)
        add_object_ref(memory_watchdog);
    if (m_memory_watchdog != 0)
        remove_object_ref(m_memory_watchdog);
    m_memory_watchdog = memory_watchdog;
}

// 0x63d018 (natural source)
void phys_world::check_access_active_islands() const {}

// 0x63d020 (natural source)
void phys_world::set_broadphase_border(phys_broadphase_border *border)
{
    if (m_broadphase_border != 0) {
        m_broadphase_border->deactivate();
        remove_object_ref(m_broadphase_border);
    }
    m_broadphase_border = border;
    if (border != 0)
        add_object_ref(border);
}

// 0x63d0e0 (natural source)
phys_broadphase_border *phys_world::get_broadphase_border() const { return m_broadphase_border; }

// 0x63d0e8 (natural source)
void phys_world::trace_ray(const phys_world_ray_input &input, phys_ray_hit_collector &collector) const
{
    phys_world_ray_caster ray_caster;
    phys_collision_filter *filter = m_collision_filter;
    ray_caster.trace_ray(*m_broadphase, input, filter, static_cast<char *>(0), collector);
}

// 0x63d140 (natural source)
void phys_world::execute_pending_ops() { run_pending_world_ops(this); }

// 0x63d158 (natural source)
void phys_world::queue_world_op(const phys_world_op::base_op &operation)
{
    m_pending_ops->queue_world_op(operation);
}

// 0x63d160 (natural source)
void phys_world::unlock()
{
    m_world_lock->leave();
}

// 0x63d178 (natural source)
bool phys_world::is_locked()
{
    return critical_section_held(m_world_lock);
}

// 0x63d198 (natural source)
void phys_world::unlock_island_constraints(phys_island *island)
{
    if (m_constraint_edit_lock == 0)
        return;
    m_constraint_edit_lock->leave();
}

inline void phys_motion::set_world_select_flags_neg(int select_flag0, int select_flag1, int world_counter)
{
    if ((unsigned int)(world_counter & 3) < (unsigned int)(m_integrate_sleep_counter & 3))
        select_flag0 = ~select_flag0 << 14;
    else
        select_flag0 = select_flag0 << 14;
    if (world_counter < m_integrate_sleep_counter)
        select_flag1 = ~select_flag1 << 14;
    else
        select_flag1 = select_flag1 << 14;
    m_inactive_frame_counts[0] = (m_inactive_frame_counts[0] & 0x3fff) | select_flag0;
    m_inactive_frame_counts[1] = (m_inactive_frame_counts[1] & 0x3fff) | select_flag1;
}

// 0x63d3b0 (natural source)
phys_entity *phys_world::add_chain_entity(phys_entity *entity, phys_entity_activation activation)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::add_entity_op operation;
        operation.m_op_entity = entity;
        operation.m_add_activation = activation;
        m_pending_ops->queue_world_op(operation);
        return 0;
    }
    if (phys_ptr_to_i32(entity->m_collider.m_motion) == 0)
        entity->m_collider.m_motion = entity->get_motion_state();
    phys_swept_transform_util::set_time_info(0.0f, 0.0f, entity->m_motion.m_motion_state);
    allow_critical_ops(false);
    ++m_last_entity_uid;
    entity->m_entity_uid = m_last_entity_uid;
    entity->add_object_reference();
    phys_world_op_util::add_entity_island(this, entity, activation);
    entity->m_motion.set_world_select_flags_neg(m_dynamics_step_info.m_solver_info.m_inactive_frame_select[0],
        m_dynamics_step_info.m_solver_info.m_inactive_frame_select[1],
        m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter);
    allow_critical_ops(true);
    ++m_critical_ops_lock_count;
    phys_world_op_util::add_entity_broadphase(this, entity);
    phys_world_callback_util::notify_entity_added(this, entity);
    phys_entity_callback_util::notify_entity_added(entity);
    unlock_and_try_world_ops(this);
    return entity;
}

// 0x63d538 (natural source)
phys_bool phys_world::remove_entity(phys_entity *entity)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::remove_entity_op operation;
        operation.m_op_entity = entity;
        m_pending_ops->queue_world_op(operation);
        return false;
    }
    ++m_critical_ops_lock_count;
    phys_world_op_util::remove_entity_from_broadphase(this, entity);
    phys_world_callback_util::notify_entity_removed(this, entity);
    phys_entity_callback_util::notify_entity_removed(entity);
    allow_critical_ops(false);
    phys_world_op_util::remove_entity_from_island(this, entity);
    if (entity->m_mem_size_and_flags == 0)
        entity->free_internal_arrays();
    entity->object_remove_ref();
    allow_critical_ops(true);
    unlock_and_try_world_ops(this);
    return true;
}

// 0x63d638 (natural source)
void phys_world::remove_volume(phys_volume *phantom)
{
    if (m_critical_ops_lock_count + m_critical_ops_volume_locks != 0) {
        phys_world_op::remove_volume_op operation;
        operation.m_overlap_volume = phantom;
        m_pending_ops->queue_world_op(operation);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_world_callback_util::notify_volume_removed(this, phantom);
    phantom->notify_volume_removed();
    phys_world_op_util::remove_volume_from_broadphase(this, phantom);
    int index = m_border_volumes.array_index_of(phantom);
    m_border_volumes.remove_at(index);
    phantom->m_world = 0;
    if (phantom->m_mem_size_and_flags == 0)
        phantom->free_internal_arrays();
    phantom->object_remove_ref();
    unlock_and_try_world_ops(this);
}

phys_volume *phys_world::add_volume(phys_volume *phantom)
{
    if (m_critical_ops_lock_count + m_critical_ops_volume_locks != 0) {
        phys_world_op::add_volume_op operation;
        operation.m_overlap_volume = phantom;
        m_pending_ops->queue_world_op(operation);
        return 0;
    }
    ++m_critical_ops_lock_count;
    if (phantom->m_collider.get_motion_state() == 0)
        phantom->m_collider.m_motion = phantom->get_motion_state();
    phantom->m_world = this;
    phantom->add_object_reference();
    m_border_volumes.array_push_back(phantom);
    phys_world_op_util::add_volume_broadphase(this, phantom);
    phys_world_callback_util::notify_volume_added(this, phantom);
    phantom->notify_volume_added();
    unlock_and_try_world_ops(this);
    return phantom;
}

// 0x63d768 (natural source)
void phys_world::remove_action_listener(phys_action_listener *listener)
{
    int index = ptr_array_index_of(m_action_listeners, listener);
    m_action_listeners.m_data[index] = 0;
}

// 0x63d7b0 (natural source)
void phys_world::remove_constraint_listener(phys_constraint_listener *listener)
{
    int index = ptr_array_index_of(m_constraint_listeners, listener);
    m_constraint_listeners.m_data[index] = 0;
}

// 0x63d7f8 (natural source)
void phys_world::remove_entity_listener(phys_entity_listener *listener)
{
    int index = ptr_array_index_of(m_entity_listeners, listener);
    m_entity_listeners.m_data[index] = 0;
}

// 0x63d840 (natural source)
void phys_world::remove_volume_listener(phys_volume_listener *listener)
{
    int index = ptr_array_index_of(m_volume_listeners, listener);
    m_volume_listeners.m_data[index] = 0;
}

// 0x63d888 (natural source)
void phys_world::remove_island_activation_listener(phys_island_activation_listener *listener)
{
    int index = ptr_array_index_of(m_island_activation_listeners, listener);
    m_island_activation_listeners.m_data[index] = 0;
}

// 0x63d8d0 (natural source)
void phys_world::remove_world_post_collide_listener(phys_post_collide_listener *listener)
{
    int index = ptr_array_index_of(m_post_collide_listeners, listener);
    m_post_collide_listeners.m_data[index] = 0;
}

// 0x63d918 (natural source)
void phys_world::remove_world_post_simulation_listener(phys_post_simulation_listener *listener)
{
    int index = ptr_array_index_of(m_post_sim_listeners, listener);
    m_post_sim_listeners.m_data[index] = 0;
}

// 0x63d960 (natural source)
void phys_world::remove_world_post_integrate_listener(phys_post_integrate_listener *listener)
{
    int index = ptr_array_index_of(m_post_integrate_listeners, listener);
    m_post_integrate_listeners.m_data[index] = 0;
}

// 0x63d9a8 (natural source)
void phys_world::remove_island_post_collide_listener(phys_island_post_collide_listener *listener)
{
    int index = ptr_array_index_of(m_island_post_collide_listeners, listener);
    m_island_post_collide_listeners.m_data[index] = 0;
}

// 0x63d9f0 (natural source)
void phys_world::remove_island_post_integrate_listener(phys_island_post_integrate_listener *listener)
{
    int index = ptr_array_index_of(m_island_post_integ_listeners, listener);
    m_island_post_integ_listeners.m_data[index] = 0;
}

// 0x63da38 (natural source)
void phys_world::remove_collision_listener(phys_collision_listener *listener)
{
    int index = ptr_array_index_of(m_collision_listeners, listener);
    m_collision_listeners.m_data[index] = 0;
}

// 0x63da80 (natural source)
void phys_world::remove_world_deletion_listener(phys_world_deletion_listener *listener)
{
    int index = ptr_array_index_of(m_world_delete_listeners, listener);
    m_world_delete_listeners.m_data[index] = 0;
}

// @address 0x63dac8 ?get_desc@phys_world@@QBAXAAVphys_world_desc@@@Z
void phys_world::get_desc(phys_world_desc &c) const
{
    c.m_gravity = m_gravity;
    c.m_broadphase_query_size = m_broadphase_query_size;
    c.m_broadphase_bounds.m_min_limit = m_broadphase_extents[0];
    c.m_broadphase_bounds.m_max_limit = m_broadphase_extents[1];
    c.m_collision_tolerance = m_collision_input->m_box_tolerance;
    c.m_collision_filter = m_collision_filter;
    c.m_convex_list_filter = m_convex_list_filter;
    c.m_border_behaviour = m_broadphase_border
        ? m_broadphase_border->m_type
        : phys_world_desc::BROADPHASE_BORDER_DO_NOTHING;
    c.m_expected_max_lin_vel = m_collision_dispatcher->m_expected_max_lin_vel;
    c.m_expected_min_psi_dt = m_collision_dispatcher->m_expected_min_psi_dt;
    c.m_convex_edge_snap_threshold = m_convex_edge_snap_threshold;
    c.m_convex_edge_snap_threshold = m_concave_edge_snap_threshold;
    c.m_toi_weld_reject = m_toi_weld_reject;
    c.m_memory_watchdog = m_memory_watchdog;
    c.m_broadphase_marker_count = m_broadphase_marker_count;
    c.m_toi_event_queue_size = m_toi_event_queue_size;
    c.m_contact_point_gen = m_contact_point_gen;
    c.m_contact_resting_vel = m_dynamics_step_info.m_solver_info.m_contact_resting_vel;
    c.m_solver_tau = m_dynamics_step_info.m_solver_info.m_query_tau;
    c.m_solver_damp = m_dynamics_step_info.m_solver_info.m_solver_damping;
    c.m_solver_iterations = m_dynamics_step_info.m_solver_info.m_step_count;
    c.m_solver_microsteps = m_dynamics_step_info.m_solver_info.m_micro_step_count;
    c.m_coherent_solver_order = m_dynamics_step_info.m_solver_info.m_coherent_solver_order;
    c.m_inactive_frame_select0 =
        m_dynamics_step_info.m_solver_info.m_inactive_frame_select[0];
    c.m_inactive_frame_select1 =
        m_dynamics_step_info.m_solver_info.m_inactive_frame_select[1];
    c.m_integrate_sleep_counter = m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter;
    c.m_iter_cast_early_out_dist = m_collision_input->m_agent_config->m_iter_cast_early_out_dist;
    c.m_iter_cast_max_iters = m_collision_input->m_agent_config->m_iter_cast_max_iters;
    c.m_legacy_edge_weld = m_collision_input->m_legacy_edge_weld;
    c.m_hf_sleep_period = m_hf_sleep_period;
    c.m_low_freq_sleep_period = m_low_freq_sleep_period;
    c.m_wake_on_body_move = m_wake_on_body_move;
    c.m_want_legacy_sleep = m_want_legacy_sleep;
    c.m_toi_response_rotate_normal = m_toi_response_rotate_normal;
    c.m_sleep_ref_distance = m_sleep_ref_distance;
    c.m_sleep_enabled = m_want_sleep;
    c.m_simulation_type = static_cast<phys_world_desc::simulation_kind>(static_cast<int>(m_simulation_type));
    c.m_frame_marker_psi_snap = m_world_simulation->m_frame_marker_psi_snap;
    c.m_islands_enabled = m_want_islands;
    c.m_single_thread_actions = m_single_thread_actions;
    c.m_min_island_size = m_min_island_size;
}

// 0x63dc60 (natural source)
void phys_world::trace_ray(const phys_world_ray_input &input, phys_world_ray_output &output) const
{
    phys_simple_world_ray_caster ray_caster;
    phys_collision_filter *filter = m_collision_filter;
    ray_caster.trace_ray(*m_broadphase, input, filter, 0, output);
}

// 0x63dcb8 (natural source)
phys_bool enumerate_inactive_entities(const phys_world *world, phys_physics_system *system)
{
    phys_bool has_fixed_entities = false;
    if (phys_ptr_to_i32(world->m_fixed_island) != 0) {
        const phys_array<phys_entity *> &entities = world->m_fixed_island->m_island_entities;
        for (int i = 0; i < entities.getSize(); ++i) {
            if (i != 0 || phys_ptr_to_i32(entities[0]->m_collider.m_proxy_shape) != 0)
                system->add_system_rigid_body(static_cast<phys_rigid_body *>(entities[i]));
        }
        has_fixed_entities = entities.getSize() > 0;
    }
    const phys_array<phys_island *> &islands = world->m_inactive_islands;
    for (phys_island *const *island = islands.begin(); island != islands.end(); ++island) {
        const phys_array<phys_entity *> &entities = (*island)->m_island_entities;
        for (int i = 0; i < entities.getSize(); ++i)
            system->add_system_rigid_body(static_cast<phys_rigid_body *>(entities[i]));
    }
    return islands.getSize() > 0 || has_fixed_entities;
}

// 0x63ddf8 (natural source)
phys_bool enumerate_active_entities(const phys_world *world, phys_physics_system *system)
{
    const phys_array<phys_island *> &islands = world->m_active_islands;
    for (phys_island *const *island = islands.begin(); island != islands.end(); ++island) {
        const phys_array<phys_entity *> &entities = (*island)->m_island_entities;
        for (int i = 0; i < entities.getSize(); ++i)
            system->add_system_rigid_body((phys_rigid_body *)entities[i]);
    }
    return islands.getSize() > 0;
}

// 0x63dea8 (natural source)
void enumerate_world_entities(const phys_world *world, phys_physics_system *system)
{
    enumerate_inactive_entities(world, system);
    system->system_set_active(enumerate_active_entities(world, system));
}

// 0x63df08 (natural source)
void enumerate_island_constraints(phys_island *island, phys_physics_system *system)
{
    for (int i = 0; i < island->m_island_entities.getSize(); ++i) {
        phys_entity *entity = island->m_island_entities[i];
        const phys_small_array<phys_constraint_internal> &master_constraints = entity->m_master_constraints;
        for (int j = 0; j < master_constraints.m_size; ++j) {
            const phys_constraint_internal &constraint = master_constraints.m_data[j];
            phys_atom *atom = last_atom_of(constraint);
            int type = atom->get_type_id();
            if (type != 22)
                system->add_world_constraint(constraint.m_contact_constraint);
        }
    }
}

// 0x63dfb8 (natural source)
void enumerate_world_constraints(const phys_world *world, phys_physics_system *system)
{
    const phys_array<phys_island *> &active = world->m_active_islands;
    for (phys_island *const *island = active.begin(); island != active.end(); ++island)
        enumerate_island_constraints(*island, system);
    const phys_array<phys_island *> &inactive = world->m_inactive_islands;
    for (phys_island *const *island = inactive.begin(); island != inactive.end(); ++island)
        enumerate_island_constraints(*island, system);
}

// 0x63e060 (natural source)
void enumerate_world_actions(const phys_world *world, phys_physics_system *system)
{
    const phys_array<phys_island *> &active = world->m_active_islands;
    for (phys_island *const *island = active.begin(); island != active.end(); ++island) {
        const phys_array<phys_action *> &actions = (*island)->m_actions;
        for (int i = 0; i < actions.getSize(); ++i)
            system->add_world_action(actions[i]);
    }
    const phys_array<phys_island *> &inactive = world->m_inactive_islands;
    for (phys_island *const *island = inactive.begin(); island != inactive.end(); ++island) {
        const phys_array<phys_action *> &actions = (*island)->m_actions;
        for (int i = 0; i < actions.getSize(); ++i)
            system->add_world_action(actions[i]);
    }
}

// 0x63e170 (natural source)
void enumerate_world_volumes(const phys_world *world, phys_physics_system *system)
{
    phys_broadphase_border *border = world->get_broadphase_border();
    const phys_array<phys_volume *> &phantoms = world->m_border_volumes;
    if (border != 0) {
        for (int i = 0; i < phantoms.getSize(); ++i) {
            phys_volume *phantom = phantoms[i];
            if (phantom != border->m_border_volumes[0] && phantom != border->m_border_volumes[1] &&
                phantom != border->m_border_volumes[2] && phantom != border->m_border_volumes[3] &&
                phantom != border->m_border_volumes[4] && phantom != border->m_border_volumes[5])
                system->add_volume(phantom);
        }
    } else {
        for (int i = 0; i < phantoms.getSize(); ++i)
            system->add_volume(phantoms[i]);
    }
}

// 0x63e308 (natural source)
phys_world_op::user_callback *phys_world::queue_callback(phys_world_op::user_callback *callback, void *action_user_data)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::user_callback_op operation;
        operation.m_callback = callback;
        operation.m_user_data = action_user_data;
        m_pending_ops->queue_world_op(operation);
    } else {
        callback->world_op_user_callback(action_user_data);
    }
    return callback;
}

// 0x63e378 (natural source)
void phys_world::find_initial_contacts(phys_entity **entities, int solver_entity_count)
{
    phys_step_info step_info(m_world_simulation->get_current_psi_time(), m_world_simulation->get_current_psi_time());
    m_world_simulation->collide_entities_discrete(entities, solver_entity_count, this, step_info, phys_simulation::CONTACT_SEARCH_EXTRA);
}

// 0x63e3f0 (natural source)
void phys_world::find_all_initial_contacts()
{
    const phys_array<phys_island *> &world_active_islands = m_active_islands;
    for (int i = 0; i < world_active_islands.getSize(); i++) {
        find_initial_contacts(world_active_islands[i]->m_island_entities.begin(), world_active_islands[i]->m_island_entities.getSize());
    }
    for (int i = 0; i < m_inactive_islands.getSize(); i++) {
        find_initial_contacts(m_inactive_islands[i]->m_island_entities.begin(), m_inactive_islands[i]->m_island_entities.getSize());
    }
}

int phys_world::get_integration_mem_usage()
{
    int max_mem_use = 0;
    const phys_array<phys_island *> &world_active_islands = m_active_islands;
    for (int i = world_active_islands.getSize() - 1; i >= 0; i--) {
        phys_island *active_island = world_active_islands[i];
        int mem_usage = island_memory_usage(active_island);
        max_mem_use = phys_math::max_of_two(max_mem_use, mem_usage);
    }
    return max_mem_use;
}

void phys_world::lock()
{
    phys_critical_section *lock = m_world_lock;
    if (!RtlTryEnterCriticalSection(&lock->m_section)) {
        // match: 0x63e5a8: retain m_timeLocks branch so RtlTryEnterCriticalSection keeps a single cmpwi
        if (phys_critical_section::m_time_locks.get_chain_data() != 0)
            RtlEnterCriticalSection(&lock->m_section);
        else
            RtlEnterCriticalSection(&lock->m_section);
    }
    lock->m_current_thread = phys_thread::get_current_thread_id();
}

// 0x63e610 (natural source)
void phys_world::lock_island_for_constraints(phys_island *island)
{
    if (m_constraint_edit_lock == 0)
        return;
    m_constraint_edit_lock->enter();
}

// 0x63e628 (natural source)
void phys_world::attach_action_to_entity(phys_action *action, phys_entity *entity)
{
    entity->m_actions.array_push_back(action);
    if (action->get_island()->body_is_fixed() && !entity->body_is_fixed()) {
        action->get_island()->remove_action(action);
        entity->get_island()->add_world_action(action);
    } else {
        phys_island *entity_island = entity->get_island();
        if (entity_island != action->get_island() && !entity->body_is_fixed())
            phys_world_op_util::merge_islands(this, entity, action->get_island()->m_island_entities[0]);
    }
}

void phys_world::add_action_listener(phys_action_listener *listener)
{
  ptr_array_add(&m_action_listeners, listener);
}

void phys_world::add_constraint_listener(phys_constraint_listener *listener)
{
  ptr_array_add(&m_constraint_listeners, listener);
}

void phys_world::add_entity_listener(phys_entity_listener *listener)
{
  ptr_array_add(&m_entity_listeners, listener);
}

void phys_world::add_volume_listener(phys_volume_listener *listener)
{
  ptr_array_add(&m_volume_listeners, listener);
}

void phys_world::add_island_activation_listener(phys_island_activation_listener *listener)
{
  ptr_array_add(&m_island_activation_listeners, listener);
}

void phys_world::add_post_collide_listener(phys_post_collide_listener *listener)
{
  ptr_array_add(&m_post_collide_listeners, listener);
}

void phys_world::add_post_simulation_listener(phys_post_simulation_listener *listener)
{
  ptr_array_add(&m_post_sim_listeners, listener);
}

void phys_world::add_post_integrate_listener(phys_post_integrate_listener *listener)
{
  ptr_array_add(&m_post_integrate_listeners, listener);
}

void phys_world::add_island_post_collide_listener(phys_island_post_collide_listener *listener)
{
  ptr_array_add(&m_island_post_collide_listeners, listener);
}

void phys_world::add_island_post_integrate_listener(phys_island_post_integrate_listener *listener)
{
  ptr_array_add(&m_island_post_integ_listeners, listener);
}

void phys_world::add_collision_listener(phys_collision_listener *listener)
{
  ptr_array_add(&m_collision_listeners, listener);
}

void phys_world::add_world_delete_listener(phys_world_deletion_listener *listener)
{
  ptr_array_add(&m_world_delete_listeners, listener);
}

// 0x63f510 (natural source)
void phys_world::get_world_as_systems(phys_array<phys_physics_system *> &systems) const
{
    phys_physics_system *system = new phys_physics_system();
    physics_system_array_push(systems, system);
    enumerate_active_entities(this, system);
    enumerate_world_volumes(this, system);
    enumerate_world_constraints(this, system);
    enumerate_world_actions(this, system);
    system = new phys_physics_system();
    if (enumerate_inactive_entities(this, system)) {
        system->system_set_active(false);
        physics_system_array_push(systems, system);
    } else if (system != 0) {
        delete system;
    }
}

// 0x6411e0 (natural source)
phys_action *phys_world::add_world_action(phys_action *action)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::add_action_op operation;
        operation.m_chain_action = action;
        m_pending_ops->queue_world_op(operation);
        return 0;
    }
    add_object_ref(action);
    ++m_critical_ops_lock_count;
    phys_inline_array<phys_entity *, 4> entities(0);
    action->get_entities(entities);
    action->m_world = this;
    phys_entity *first_entity = 0;
    for (int i = 0; i < entities.getSize(); ++i) {
        entities[i]->m_actions.array_push_back(action);
        phys_island *island = entities[i]->get_island();
        if (!island->body_is_fixed()) {
            if (first_entity == 0) {
                first_entity = entities[i];
                island->add_world_action(action);
            } else {
                phys_island *first_island = first_entity->get_island();
                if (first_island != entities[i]->get_island())
                    phys_world_op_util::merge_islands(this, first_entity, entities[i]);
            }
        }
    }
    if (first_entity == 0)
        entities[0]->get_island()->add_world_action(action);
    unlock_and_try_world_ops(this);
    phys_world_callback_util::notify_action_added(this, action);
    return action;
}

// 0x6413d8 (natural source)
void phys_world::remove_action_now(phys_action *action)
{
    add_object_ref(action);
    ++m_critical_ops_lock_count;
    phys_world_callback_util::notify_action_removed(this, action);
    {
        phys_inline_array<phys_entity *, 4> entities(0);
        action->get_entities(entities);
        for (int i = 0; i < entities.getSize(); ++i) {
            int index = entities[i]->m_actions.array_index_of(action);
            entities[i]->m_actions.remove_at(index);
        }
        phys_island *island = action->get_island();
        island->remove_action(action);
        action->m_world = 0;
        remove_object_ref(action);
        mark_island_dirty(island->m_world, island);
    }
    unlock_and_try_world_ops(this);
    remove_object_ref(action);
}

// 0x641670 (natural source)
void phys_world::detach_entity_action(phys_action *action, phys_entity *entity)
{
    int index = entity->m_actions.array_index_of(action);
    entity->m_actions.remove_at(index);
    entity->get_island()->m_state = (entity->get_island()->m_state & 63) | 64;
    phys_inline_array<phys_entity *, 4> entities(0);
    action->get_entities(entities);
    phys_island *island = 0;
    for (int i = 0; i < entities.getSize(); ++i) {
        if (entities[i] != entity) {
            island = entities[i]->get_island();
            if (!island->body_is_fixed())
                break;
        }
    }
    if (island != action->get_island()) {
        action->get_island()->remove_action(action);
        island->add_world_action(action);
    }
}

// 0x6439d8 (natural source)
phys_constraint *phys_world::add_world_constraint(phys_constraint *constraint)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::add_constraint_op operation;
        operation.m_contact_constraint = constraint;
        m_pending_ops->queue_world_op(operation);
        return 0;
    }
    if (constraint->get_chain_data()->get_type_id() == 12) {
        phys_bridge_data *bridge_data = static_cast<phys_bridge_data *>(constraint->get_chain_data());
        if (bridge_data->m_schemas_size == 0) {
            phys_constraint_data::runtime_info info;
            bridge_data->m_constraint_data->get_runtime_info(true, info);
            bridge_data->m_schemas_size = (unsigned short)info.m_external_runtime_size;
            bridge_data->m_solver_result_count = (unsigned short)info.m_solver_result_count;
        }
    }
    block_pending_ops(true);
    constraint->bind_nulls_to_fixed_body();
    phys_constraint *result = phys_world_op_util::add_constraint_now(this, constraint, phys_world_op_util::OP_CALLBACKS_FIRE);
    if (constraint->get_type_id() == phys_constraint::CONSTRAINT_KIND_CHAIN) {
        phys_constraint_chain *chain = static_cast<phys_constraint_chain *>(constraint);
        if (phys_ptr_to_i32(chain->m_chain_action->m_world) == 0)
            add_world_action(chain->m_chain_action);
        chain->get_chain_data()->get_constraint_info_count();
    }
    block_pending_ops(false);
    try_pending_world_ops(this);
    return result;
}

// 0x643b20 (natural source)

// 0x643b98 (natural source)
phys_bool phys_world::remove_constraint(phys_constraint *constraint)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::remove_constraint_op operation;
        operation.m_contact_constraint = constraint;
        m_pending_ops->queue_world_op(operation);
        return false;
    }
    ++m_critical_ops_lock_count;
    if (constraint->get_type_id() == phys_constraint::CONSTRAINT_KIND_CHAIN) {
        phys_constraint_chain *chain = static_cast<phys_constraint_chain *>(constraint);
        if (chain->m_chain_action->m_world == this)
            remove_action_now(chain->m_chain_action);
    }
    phys_world_op_util::remove_constraint_now(this, constraint, phys_world_op_util::OP_CALLBACKS_FIRE);
    unlock_and_try_world_ops(this);
    return true;
}

void phys_world::remove_action(phys_action *action)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::remove_action_op operation;
        operation.m_chain_action = action;
        m_pending_ops->queue_world_op(operation);
    } else {
        remove_action_now(action);
    }
}

// 0x643ed8 (natural source)

phys_null_collision_filter::phys_null_collision_filter()
{
    m_filter_kind = filter_kind_null;
}

// 0x643fe8 (natural source)
phys_contact_manager *phys_null_contact_manager_factory::create_contact_manager(const phys_collider &a, const phys_collider &b,
    const phys_collision_input &input)
{
    return &m_contact_manager;
}

// 0x63d1b8 (natural source)

phys_physics_system *phys_world::get_world_as_one_system() const
{
    phys_physics_system *node = new phys_physics_system();
    enumerate_inactive_entities(this, node);
    node->system_set_active(enumerate_active_entities(this, node));
    enumerate_world_volumes(this, node);
    enumerate_world_constraints(this, node);
    enumerate_world_actions(this, node);
    return node;
}

void phys_world::activate_region(const phys_aabb &activate_bounds)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::activate_region_op operation;
        operation.m_volume_aabb = static_cast<phys_aabb *>(
            phys_thread_memory::get_instance_ptr()->allocate_chunk(sizeof(phys_aabb), PHYS_MEM_DYNAMICS));
        phys_string::mem_copy(operation.m_volume_aabb, &activate_bounds, sizeof(phys_aabb));
        m_pending_ops->queue_world_op(operation);
        return;
    }
    phys_array<phys_broadphase_handle_pair> pairs;
    m_broadphase->query_single_aabb(activate_bounds, pairs);
    int i = 0;
    if (pairs.getSize() > 0) {
        do {
            phys_typed_broadphase_handle *handle = static_cast<phys_typed_broadphase_handle *>(pairs[i].m_b);
            phys_collider *collidable = static_cast<phys_collider *>(handle->get_collider_owner());
            if (collidable->get_type_id() == 1) {
                phys_entity *entity = static_cast<phys_entity *>(collidable->get_collider_owner());
                if (entity != 0)
                    entity->activate();
            }
            i++;
        } while (i < pairs.getSize());
    }
}

phys_constraint *phys_world::create_and_add_constraint(phys_rigid_body *constraint_first_body, phys_rigid_body *second_body, phys_constraint_data *data)
{
    phys_constraint *constraint = new phys_constraint(constraint_first_body, second_body, data, phys_constraint::PRIORITY_PSI);
    constraint->m_user_data = data->m_user_data;
    add_world_constraint(constraint);
    return constraint;
}

void phys_world::set_collision_filter(
    phys_collision_filter *filter, phys_bool update, phys_world_filter_update_mode world_filter_mode, phys_compound_filter_update_mode collection_mode)
{
    if (filter == 0) {
        filter = new phys_null_collision_filter();
    } else {
        add_object_ref(filter);
    }
    remove_object_ref(m_collision_filter);
    m_collision_filter = filter;
    m_collision_input->m_compound_filter = filter;
    if (update)
        refilter_world(world_filter_mode, collection_mode);
}

void phys_world_setup_contact_factories(phys_world *world, phys_dispatcher *world_dispatcher)
{
    phys_contact_manager_factory *simple = new phys_simple_contact_manager::report_contact_factory(world);
    phys_contact_manager_factory *string_storage_rep = new phys_report_contact_manager::report_contact_factory(world);
    phys_null_contact_manager_factory *none = static_cast<phys_null_contact_manager_factory *>(
        phys_thread_memory::get_singleton_instance().allocate_chunk(sizeof(phys_null_contact_manager_factory), PHYS_MEM_DYNAMICS));
    none->m_mem_size_and_flags = sizeof(phys_null_contact_manager_factory);
    none->phys_null_contact_manager_factory::phys_null_contact_manager_factory();

    world_dispatcher->register_contact_factory_all(simple, 1);
    world_dispatcher->register_contact_factory_all(string_storage_rep, 2);
    world_dispatcher->register_contact_factory_all(none, 3);

    remove_object_ref(simple);
    remove_object_ref(string_storage_rep);
    remove_object_ref(none);
}

// @address 0x643468 ?get_penetrations@phys_world@@QBAXPBVphys_collider@@ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_world::get_penetrations(const phys_collider *collider_a, const phys_collision_input &input, phys_proxy_pair_collector &collector) const
{
    phys_aabb aabb;
    float rest_tolerance = input.m_box_tolerance - m_collision_input->m_box_tolerance * 0.5f;
    collider_a->m_proxy_shape->get_handle_aabb(*(const phys_transform *)collider_a->get_motion_state(), rest_tolerance, aabb);
    phys_inline_array<phys_broadphase_handle_pair, 128> hits(0);
    m_broadphase->query_single_aabb(aabb, hits);
    phys_broadphase_handle_pair *p = hits.m_data;
    phys_shape_type shape_type_a = collider_a->m_proxy_shape->get_type_id();
    for (int i = hits.m_size - 1; i >= 0; p++, i--) {
        const phys_typed_broadphase_handle *tp = (const phys_typed_broadphase_handle *)p->m_b;
        const phys_collider *collider_b = (const phys_collider *)((const char *)tp + tp->m_owner_offset);
        if (collider_a == collider_b) continue;
        if (!m_collision_filter->allows_collision(*collider_a, *collider_b)) continue;
        const phys_shape *penetration_shape_b = collider_b->m_proxy_shape;
        if (!penetration_shape_b) continue;
        phys_shape_type shape_type_b = penetration_shape_b->get_type_id();
        phys_dispatcher::agent_funcs::penetrations_fn agent_get_penetrations_fn = input.m_agent_dispatcher->get_penetrations_fn(shape_type_a, shape_type_b);
        agent_get_penetrations_fn(*collider_a, *collider_b, input, collector);
        if (collector.get_early_out()) break;
    }
}

// @address 0x640130 ?refilter_volume@phys_world@@QAAXPAVphys_volume@@W4phys_compound_filter_update_mode@@@Z
void phys_world::refilter_volume(phys_volume *phantom, phys_compound_filter_update_mode update_compound_filter)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::update_volume_filter_op op;
        op.m_overlap_volume = phantom;
        op.m_update_compound_shapes = update_compound_filter;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_collider *volume_collider = &phantom->m_collider;
    phys_local_array<phys_broadphase_handle_pair> batch_pairs_out(m_broadphase_query_size);
    m_broadphase->requery_single_object(&volume_collider->m_broadphase_handle, batch_pairs_out);
    for (int i = 0; i < batch_pairs_out.getSize(); i++) {
        if (batch_pairs_out[i].m_b == &volume_collider->m_broadphase_handle)
            continue;
        phys_collider *collidable = (phys_collider *)((phys_typed_broadphase_handle *)batch_pairs_out[i].m_b)->get_collider_owner();
        phys_world_update_volume_filter(phantom, collidable, m_collision_filter);
    }
    if (update_compound_filter)
        phantom->update_compound_filter();
    unlock_and_try_world_ops(this);
}

// @address 0x640368 ?refilter_entity@phys_world@@QAAXPAVphys_entity@@W4phys_entity_filter_update_mode@@W4phys_compound_filter_update_mode@@@Z
void phys_world::refilter_entity(
    phys_entity *entity, phys_entity_filter_update_mode entity_filter_update_mode, phys_compound_filter_update_mode update_compound_filter)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::update_entity_filter_op op;
        op.m_op_entity = entity;
        op.m_filter_update_mode = entity_filter_update_mode;
        op.m_update_compound_shapes = update_compound_filter;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_inline_array<phys_broadphase_handle_pair, 128> updated_pairs(0);
    if (entity_filter_update_mode == PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK) {
        m_broadphase->requery_single_object(&entity->get_collider()->m_broadphase_handle, updated_pairs);
        for (int i = 0; i < updated_pairs.getSize(); i++) {
            phys_collider *collidable = (phys_collider *)((phys_typed_broadphase_handle *)updated_pairs.m_data[i].m_b)->get_collider_owner();
            phys_volume *phantom = phys_get_volume(collidable);
            if (phantom) {
                phys_world_update_volume_filter(phantom, entity->get_collider_rw(), m_collision_filter);
                if (update_compound_filter)
                    phantom->update_compound_filter();
                updated_pairs.remove_at(i--);
            }
        }
        phys_array<phys_linked_collider::collision_entry> &collision_entries = entity->get_linked_collider()->m_collision_entries;
        phys_inline_array<phys_broadphase_handle_pair, 128> present_pairs(0);
        for (int i = 0; i < collision_entries.getSize(); i++) {
            phys_broadphase_handle_pair &pair = present_pairs.expand_one();
            pair.m_a = &entity->get_linked_collider()->m_collision_entries.m_data[i].m_agent_entry->m_collider[0]->m_broadphase_handle;
            pair.m_b = &entity->get_linked_collider()->m_collision_entries.m_data[i].m_agent_entry->m_collider[1]->m_broadphase_handle;
        }
        phys_typed_broadphase_dispatcher::remove_duplicate_pairs(present_pairs, updated_pairs);
    }
    {
        phys_collider_quality quality_type_a = entity->get_linked_collider()->get_quality_type();
        phys_array<phys_linked_collider::collision_entry> &collision_entries = entity->get_linked_collider()->m_collision_entries;
        for (int i = 0; i < collision_entries.getSize(); i++) {
            phys_linked_collider::collision_entry &entry = collision_entries.m_data[i];
            int pair_quality_idx = phys_dispatcher::COLLIDE_QUALITY_INVALID;
            if (m_collision_filter->allows_collision(*entity->get_collider(), *entry.m_linked_partner)) {
                phys_collider_quality quality_type_b = entry.m_linked_partner->get_quality_type();
                pair_quality_idx = get_dispatcher()->get_collision_quality_idx(quality_type_a, quality_type_b);
            }
            if (pair_quality_idx == phys_dispatcher::COLLIDE_QUALITY_INVALID) {
                phys_world_agent_util::remove_agent_and_toi_events(entry.m_agent_entry);
                i--;
                entity->get_island()->m_state = (entity->get_island()->m_state & 63) | 64;
            } else if (update_compound_filter == PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES) {
                phys_entity *listener_entity_a = entity;
                phys_entity *listener_entity_b = (phys_entity *)entry.m_linked_partner->get_collider_owner();
                phys_island *island = listener_entity_a->body_is_fixed() ? listener_entity_b->get_island() : listener_entity_a->get_island();
                phys_pair_agent_update_compound_filter(entry.m_agent_entry, m_collision_input, island);
            }
        }
    }
    if (updated_pairs.getSize() > 0) {
        const phys_collider_pair_filter *filter = m_collision_filter;
        m_broadphase_dispatcher->add_handle_pairs(reinterpret_cast<phys_typed_broadphase_pair *>(updated_pairs.m_data), updated_pairs.getSize(), filter);
    }
    unlock_and_try_world_ops(this);
}

// @address 0x643600 ?refilter_world@phys_world@@QAAXW4phys_world_filter_update_mode@@W4phys_compound_filter_update_mode@@@Z
void phys_world::refilter_world(phys_world_filter_update_mode entity_filter_update_mode, phys_compound_filter_update_mode update_compound_filter)
{
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::update_world_filter_op op;
        op.m_filter_update_mode = entity_filter_update_mode;
        op.m_update_compound_shapes = update_compound_filter;
        m_pending_ops->queue_world_op(op);
        return;
    }
    block_pending_ops(true);
    if (entity_filter_update_mode == PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK) {
        {
            const phys_array<phys_island *> &world_active_islands = m_active_islands;
            for (int i = 0; i < world_active_islands.getSize(); ++i) {
                const phys_array<phys_entity *> &entities = world_active_islands.m_data[i]->m_island_entities;
                for (int j = 0; j < entities.getSize(); j++) {
                    phys_entity *entity = entities[j];
                    refilter_entity(entity, PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, update_compound_filter);
                }
            }
        }
        {
            for (int i = 0; i < m_inactive_islands.getSize(); ++i) {
                const phys_array<phys_entity *> &entities = m_inactive_islands[i]->get_entities();
                for (int j = 0; j < entities.getSize(); j++) {
                    phys_entity *entity = entities[j];
                    refilter_entity(entity, PHYS_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, update_compound_filter);
                }
            }
        }
    } else {
        ++m_critical_ops_lock_count;
        {
            const phys_array<phys_island *> *island_arrays[2] = { &m_active_islands, &m_inactive_islands };
            for (int a = 0; a < 2; a++) {
                phys_inline_array<phys_pair_agent_entry *, 32> agents_to_remove(0);
                for (int i = 0; i < island_arrays[a]->getSize(); ++i) {
                    phys_island *island = island_arrays[a]->m_data[i];
                    phys_pair_agent_track &track = island->m_agent_track;
                    agents_to_remove.m_size = 0;
                    for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize();) {
                        phys_pair_agent_sector *cur_sector = track.m_track_sectors.m_data[track_sector_index];
                        phys_pair_agent_entry *agent_entry = cur_sector->get_track_begin();
                        track_sector_index++;
                        phys_pair_agent_entry *end_entry = (track_sector_index == track.m_track_sectors.getSize())
                            ? phys_add_byte_offset(agent_entry, track.m_last_sector_bytes)
                            : cur_sector->get_track_end();
                        for (; agent_entry < end_entry; agent_entry = phys_add_byte_offset(agent_entry, agent_entry->m_size)) {
                            int pair_quality_idx = phys_dispatcher::COLLIDE_QUALITY_INVALID;
                            if (m_collision_filter->allows_collision(*agent_entry->m_collider[0], *agent_entry->m_collider[1])) {
                                phys_collider_quality quality_type_a = agent_entry->m_collider[0]->get_quality_type();
                                phys_collider_quality quality_type_b = agent_entry->m_collider[1]->get_quality_type();
                                pair_quality_idx = get_dispatcher()->get_collision_quality_idx(quality_type_a, quality_type_b);
                            }
                            if (pair_quality_idx == phys_dispatcher::COLLIDE_QUALITY_INVALID) {
                                agents_to_remove.array_push_back(agent_entry);
                                island->m_state = (island->m_state & 63) | 64;
                            } else if (update_compound_filter == PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES)
                                phys_pair_agent_update_compound_filter(agent_entry, m_collision_input, island);
                        }
                    }
                    while (agents_to_remove.getSize()) {
                        phys_pair_agent_entry *pair_agent_entry = agents_to_remove.m_data[agents_to_remove.m_size - 1];
                        agents_to_remove.m_size--;
                        phys_world_agent_util::remove_agent_and_toi_events(pair_agent_entry);
                    }
                }
            }
        }
        --m_critical_ops_lock_count;
    }
    if (entity_filter_update_mode == PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK) {
        for (int i = 0; i < m_border_volumes.getSize(); i++) {
            phys_volume *phantom = m_border_volumes.m_data[i];
            refilter_volume(phantom, update_compound_filter);
        }
    }
    block_pending_ops(false);
    try_pending_world_ops(this);
}

// @address 0x643db0 ?remove_physics_system@phys_world@@QAAXPBVphys_physics_system@@@Z
void phys_world::remove_physics_system(const phys_physics_system *system)
{
    int c = 0;
    if (system->get_system_constraints().m_size > 0) {
        do {
            remove_constraint(system->get_system_constraints().m_data[c]);
            c++;
        } while (c < system->get_system_constraints().m_size);
    }
    int a = 0;
    if (system->get_actions().m_size > 0) {
        do {
            remove_action(system->get_actions().m_data[a]);
            a++;
        } while (a < system->get_actions().m_size);
    }
    int count = system->get_rigid_bodies().m_size;
    if (count < 4) {
        int i = 0;
        if (count > 0) {
            do {
                remove_entity((phys_entity *)system->get_rigid_bodies().m_data[i]);
                i++;
            } while (i < system->get_rigid_bodies().m_size);
        }
    } else {
        remove_entity_batch((phys_entity *const *)system->get_rigid_bodies().m_data, count);
    }
    count = system->get_volumes().m_size;
    if (count > 0)
        remove_volume_batch(system->get_volumes().m_data, count);
}

// @address 0x6432e0 ?get_closest_points@phys_world@@QBAXPBVphys_collider@@ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_world::get_closest_points(const phys_collider *collider_a, const phys_collision_input &input, phys_hit_collector &collector) const
{
    phys_aabb aabb;
    float rest_tolerance = input.m_box_tolerance - m_collision_input->m_box_tolerance * 0.5f;
    collider_a->m_proxy_shape->get_handle_aabb(*(const phys_transform *)collider_a->get_motion_state(), rest_tolerance, aabb);
    phys_inline_array<phys_broadphase_handle_pair, 128> hits(0);
    m_broadphase->query_single_aabb(aabb, hits);
    phys_broadphase_handle_pair *p = hits.m_data;
    phys_shape_type shape_type_a = collider_a->m_proxy_shape->get_type_id();
    for (int i = hits.m_size - 1; i >= 0; p++, i--) {
        const phys_typed_broadphase_handle *tp = (const phys_typed_broadphase_handle *)p->m_b;
        const phys_collider *collider_b = (const phys_collider *)((const char *)tp + tp->m_owner_offset);
        if (collider_a == collider_b) continue;
        if (!m_collision_filter->allows_collision(*collider_a, *collider_b)) continue;
        const phys_shape *penetration_shape_b = collider_b->m_proxy_shape;
        if (!penetration_shape_b) continue;
        phys_shape_type shape_type_b = penetration_shape_b->get_type_id();
        phys_dispatcher::agent_funcs::closest_points_fn closest_point_fn = input.m_agent_dispatcher->get_closest_points_fn(shape_type_a, shape_type_b);
        closest_point_fn(*collider_a, *collider_b, input, collector);
    }
}

// @address 0x63f488 ?sweep_cast@phys_world@@QBAXPBVphys_collider@@ABUphys_linear_cast_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_world::sweep_cast(const phys_collider *collider_a, const phys_linear_cast_input &input, phys_hit_collector &cast_hit_collector,
    phys_hit_collector *start_point_collector) const
{
    phys_world_linear_caster linear_caster;
    linear_caster.sweep_cast(*m_broadphase, collider_a, input, m_collision_filter, *m_collision_input,
        m_collision_input->m_agent_config, cast_hit_collector, start_point_collector);
}

// @address 0x643cd8 ?add_physics_system@phys_world@@QAAXPBVphys_physics_system@@@Z
void phys_world::add_physics_system(const phys_physics_system *system)
{
    if (system->get_rigid_bodies().getSize() > 0) {
        phys_entity_activation activation = system->is_body_active() ? PHYS_ENTITY_ACTIVATION_DO_ACTIVATE : PHYS_ENTITY_ACTIVATION_DO_NOT_ACTIVATE;
        add_entity_batch((phys_entity *const *)system->get_rigid_bodies().begin(), system->get_rigid_bodies().getSize(), activation);
    }
    if (system->get_volumes().getSize() > 0) {
        add_volume_batch(system->get_volumes().begin(), system->get_volumes().getSize());
    }
    for (int a = 0; a < system->get_actions().getSize(); ++a) {
        if (system->get_actions().m_data[a]) {
            add_world_action(system->get_actions().m_data[a]);
        }
    }
    for (int c = 0; c < system->get_system_constraints().getSize(); ++c) {
        add_world_constraint(system->get_system_constraints().m_data[c]);
    }
}

// @address 0x640e30 ?remove_entity_batch@phys_world@@QAAXPBQAVphys_entity@@H@Z
void phys_world::remove_entity_batch(phys_entity *const *entity_batch, int solver_entity_count)
{
    if (solver_entity_count < 1)
        return;
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::remove_entity_batch_op op;
        op.m_island_entities = const_cast<phys_entity **>(entity_batch);
        op.m_entity_count = (unsigned short)solver_entity_count;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    m_world_simulation->reset_collision_info_for_entities(const_cast<phys_entity **>(entity_batch), solver_entity_count, this, true);
    {
        phys_local_array<phys_broadphase_handle *> collider_handles(solver_entity_count);
        {
            phys_entity *const *entity = entity_batch;
            phys_entity *const *batch_entity_end = entity_batch + solver_entity_count;
            while (entity != batch_entity_end) {
                phys_collider *c = (*entity)->get_collider_rw();
                if (c->get_shape() != 0)
                    collider_handles.m_data[collider_handles.m_size++] = &c->m_broadphase_handle;
                entity++;
            }
        }
        phys_local_array<phys_broadphase_handle_pair> batch_pairs_out(m_broadphase_query_size);
        m_broadphase->remove_broadphase_object_batch(collider_handles, batch_pairs_out);
        ((const phys_typed_broadphase_dispatcher *)m_broadphase_dispatcher)->remove_handle_pairs(
            (phys_typed_broadphase_pair *)batch_pairs_out.m_data, batch_pairs_out.m_size);
    }
    {
        phys_entity *const *entity = entity_batch;
        phys_entity *const *batch_entity_end = entity_batch + solver_entity_count;
        while (entity != batch_entity_end) {
            phys_world_callback_util::notify_entity_removed(this, *entity);
            phys_entity_callback_util::notify_entity_removed(*entity);
            phys_world_op_util::remove_entity_from_island(this, *entity);
            if ((*entity)->m_mem_size_and_flags == 0)
                (*entity)->free_internal_arrays();
            (*entity)->object_remove_ref();
            entity++;
        }
    }
    unlock_and_try_world_ops(this);
}

// @address 0x6417c8 ?add_volume_batch@phys_world@@QAAXPBQAVphys_volume@@H@Z
void phys_world::add_volume_batch(phys_volume *const *volume_batch, int world_volume_count)
{
    if (m_critical_ops_lock_count + m_critical_ops_volume_locks != 0) {
        phys_world_op::add_volume_batch_op op;
        op.m_border_volumes = const_cast<phys_volume **>(volume_batch);
        op.m_volume_count = (unsigned short)world_volume_count;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_local_array<phys_broadphase_handle *> collider_handles(world_volume_count);
    phys_local_array<phys_aabb> volume_aabbs(world_volume_count);
    volume_aabbs.m_size = world_volume_count;
    collider_handles.m_size = world_volume_count;
    for (int i = 0; i < world_volume_count; i++) {
        phys_volume *phantom = volume_batch[i];
        if (!phantom->m_collider.get_motion_state())
            phantom->m_collider.m_motion = phantom->get_motion_state();
        phantom->m_world = this;
        collider_handles.m_data[i] = &phantom->m_collider.m_broadphase_handle;
        phantom->compute_bounds(volume_aabbs.m_data[i]);
        phantom->add_object_reference();
        m_border_volumes.array_push_back(phantom);
        phys_world_callback_util::notify_volume_added(this, phantom);
        phantom->notify_volume_added();
    }
    phys_local_array<phys_broadphase_handle_pair> new_pairs(m_broadphase_query_size);
    m_broadphase->add_object_batch(collider_handles, volume_aabbs, new_pairs);
    const phys_collider_pair_filter *filter = m_collision_filter;
    m_broadphase_dispatcher->add_handle_pairs(
        reinterpret_cast<phys_typed_broadphase_pair *>(new_pairs.m_data), new_pairs.m_size, filter);
    unlock_and_try_world_ops(this);
}

// @address 0x641c90 ?remove_volume_batch@phys_world@@QAAXPBQAVphys_volume@@H@Z
void phys_world::remove_volume_batch(phys_volume *const *volume_batch, int world_volume_count)
{
    if (m_critical_ops_lock_count + m_critical_ops_volume_locks != 0) {
        phys_world_op::remove_volume_batch_op op;
        op.m_border_volumes = const_cast<phys_volume **>(volume_batch);
        op.m_volume_count = (unsigned short)world_volume_count;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_local_array<phys_broadphase_handle *> collider_handles(world_volume_count);
    for (int i = 0; i < world_volume_count; i++) {
        phys_volume *phantom = volume_batch[i];
        collider_handles.m_data[collider_handles.m_size++] = &phantom->m_collider.m_broadphase_handle;
        phys_world_callback_util::notify_volume_removed(this, phantom);
        phantom->notify_volume_removed();
    }
    {
        phys_local_array<phys_broadphase_handle_pair> erased_sweep_pairs(m_broadphase_query_size);
        m_broadphase->remove_broadphase_object_batch(collider_handles, erased_sweep_pairs);
        ((const phys_typed_broadphase_dispatcher *)m_broadphase_dispatcher)->remove_handle_pairs(
            (phys_typed_broadphase_pair *)erased_sweep_pairs.m_data, erased_sweep_pairs.m_size);
    }
    for (int i = 0; i < world_volume_count; i++) {
        phys_volume *phantom = volume_batch[i];
        phantom->m_world = 0;
        m_border_volumes.remove_at(m_border_volumes.array_index_of(phantom));
        if (phantom->m_mem_size_and_flags == 0)
            phantom->free_internal_arrays();
        phantom->object_remove_ref();
    }
    unlock_and_try_world_ops(this);
}

// @address 0x640720 ?add_entity_batch@phys_world@@QAAXPBQAVphys_entity@@HW4phys_entity_activation@@@Z
void phys_world::add_entity_batch(phys_entity *const *entity_batch, int solver_entity_count, phys_entity_activation initial_activation)
{
    if (solver_entity_count == 0)
        return;
    if (m_critical_ops_lock_count != 0) {
        phys_world_op::add_entity_batch_op op;
        op.m_island_entities = const_cast<phys_entity **>(entity_batch);
        op.m_entity_count = (unsigned short)solver_entity_count;
        op.m_add_activation = initial_activation;
        m_pending_ops->queue_world_op(op);
        return;
    }
    ++m_critical_ops_lock_count;
    phys_local_array<phys_broadphase_handle *> collider_handles(solver_entity_count);
    phys_local_array<phys_aabb> volume_aabbs(solver_entity_count);
    phys_island *gabriola_island;
    bool used_gabriola = false;
    if (m_want_islands) {
        gabriola_island = new phys_island(this);
        gabriola_island->m_state = (gabriola_island->m_state & 63) | 64;
        if (initial_activation == PHYS_ENTITY_ACTIVATION_DO_ACTIVATE) {
            gabriola_island->m_storage_idx = (unsigned short)m_active_islands.getSize();
            gabriola_island->m_active_flags = (gabriola_island->m_active_flags & 15) | 80;
        } else {
            gabriola_island->m_storage_idx = (unsigned short)m_inactive_islands.getSize();
            gabriola_island->m_active_flags = gabriola_island->m_active_flags & 15;
        }
    } else {
        gabriola_island = m_active_islands.m_data[0];
        gabriola_island->m_island_entities.reserve(gabriola_island->m_island_entities.getSize() + solver_entity_count);
    }
    {
        float extra_radius = m_collision_input->m_box_tolerance * .5f;
        for (int i = 0; i < solver_entity_count; i++) {
            phys_entity *entity = entity_batch[i];
            entity->m_entity_uid = ++m_last_entity_uid;
            entity->add_object_reference();
            if (!entity->get_collider()->get_motion_state())
                entity->m_collider.m_motion = entity->get_motion_state();
            phys_swept_transform_util::set_time_info(0.0f, 0.0f, entity->m_motion.m_motion_state);
            entity->m_world = this;
            unsigned char *sleep_flags = m_dynamics_step_info.m_solver_info.m_inactive_frame_select;
            entity->get_motion()->set_world_select_flags_neg(
                sleep_flags[0], sleep_flags[1], m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter);
            if (entity->body_is_fixed()) {
                m_fixed_island->add_entity_internal(entity);
            } else {
                used_gabriola = true;
                gabriola_island->add_entity_internal(entity);
            }
            phys_collider *c = entity->get_collider_rw();
            const phys_shape *shape = c->get_shape();
            if (shape) {
                phys_aabb &aabb = *volume_aabbs.expand_by_unchecked(1);
                c->get_shape()->get_handle_aabb(*(const phys_transform *)c->get_motion_state(), extra_radius, aabb);
                collider_handles.m_data[collider_handles.m_size++] = &c->m_broadphase_handle;
            }
        }
    }
    if (m_want_islands) {
        if (used_gabriola) {
            gabriola_island->m_split_check_frames = (unsigned char)gabriola_island->m_island_entities.m_data[0]->m_entity_uid;
            phys_array<phys_island *> &island_array = initial_activation == PHYS_ENTITY_ACTIVATION_DO_ACTIVATE
                ? m_active_islands
                : m_inactive_islands;
            island_array.array_push_back(gabriola_island);
        } else {
            delete gabriola_island;
        }
    }
    phys_local_array<phys_broadphase_handle_pair> batch_pairs_out(m_broadphase_query_size);
    m_broadphase->add_object_batch(collider_handles, volume_aabbs, batch_pairs_out);
    const phys_collider_pair_filter *filter = m_collision_filter;
    m_broadphase_dispatcher->add_handle_pairs(
        reinterpret_cast<phys_typed_broadphase_pair *>(batch_pairs_out.m_data), batch_pairs_out.m_size, filter);
    {
        for (int i = 0; i < solver_entity_count; i++) {
            phys_entity *entity = entity_batch[i];
            phys_world_callback_util::notify_entity_added(this, entity);
            phys_entity_callback_util::notify_entity_added(entity);
        }
    }
    unlock_and_try_world_ops(this);
}

// @address 0x63ed50 ?gather_memory_stats@phys_world@@UBAXPAVphys_statistics_collector@@@Z
void phys_world::gather_memory_stats(phys_statistics_collector *collector) const
{
    phys_statistics_collector * local_collector = collector;
    local_collector->begin_stats_object("hkWorld", static_cast<phys_statistics_collector::stat_category>(2), this);
    {
        for (int i = m_active_islands.m_size - 1; i >= 0; i--) {
            phys_island *island = m_active_islands.m_data[i];
            local_collector->add_child_object("Active", static_cast<phys_statistics_collector::stat_category>(8), island);
        }
    }
    {
        for (int i = m_inactive_islands.m_size - 1; i >= 0; i--) {
            phys_island *island = m_inactive_islands.m_data[i];
            local_collector->add_child_object("Inactive", static_cast<phys_statistics_collector::stat_category>(8), island);
        }
    }
    local_collector->add_child_object("Fixed", static_cast<phys_statistics_collector::stat_category>(8), m_fixed_island);
    {
        collect_array_stats(local_collector, "PhantomPtrs", 4, m_border_volumes);
        for (int i = 0; i < m_border_volumes.m_size; i++) {
            local_collector->add_child_object("Phantoms", static_cast<phys_statistics_collector::stat_category>(2), m_border_volumes.m_data[i]);
        }
    }
    local_collector->push_stats_dir("Internal");
    {
        local_collector->add_child_object("Simulation", static_cast<phys_statistics_collector::stat_category>(4), m_world_simulation);
        collect_array_stats(local_collector, "DirtyIslPtr", 8, m_dirty_islands);
        local_collector->add_child_object("MaintnceMgr", static_cast<phys_statistics_collector::stat_category>(4), m_maintenance_manager);
        local_collector->add_chunk_stats("OpQueue", static_cast<phys_statistics_collector::stat_category>(4), m_pending_ops, sizeof(*m_pending_ops), 0);
        local_collector->push_stats_dir("OpQueue");
        collect_array_stats(local_collector, "Operations", 8, m_pending_ops->m_pending_ops);
        local_collector->pop_dir();
        local_collector->push_stats_dir("Collide");
        {
            local_collector->add_child_object("Broadphase", static_cast<phys_statistics_collector::stat_category>(4), m_broadphase);
            local_collector->add_chunk_stats("BpDispatch", static_cast<phys_statistics_collector::stat_category>(4), m_broadphase_dispatcher, 0x104, 0);
            local_collector->add_chunk_stats("BpDispatch", static_cast<phys_statistics_collector::stat_category>(4), m_entity_pair_listener, 12, 0);
            local_collector->add_chunk_stats("BpDispatch", static_cast<phys_statistics_collector::stat_category>(4), m_volume_broadphase_listener, 8, 0);
            local_collector->add_chunk_stats("BpDispatch", static_cast<phys_statistics_collector::stat_category>(4), m_broadphase_border_listener, 8, 0);
            local_collector->add_chunk_stats("CollInput", static_cast<phys_statistics_collector::stat_category>(4), m_collision_input, 64, 0);
            local_collector->add_child_object("Filter", static_cast<phys_statistics_collector::stat_category>(1), m_collision_filter);
            local_collector->add_child_object("Dispatcher", static_cast<phys_statistics_collector::stat_category>(4), m_collision_dispatcher);
        }
        local_collector->pop_dir();
    }
    local_collector->pop_dir();
    local_collector->push_stats_dir("Listeners");
    collect_ptr_array_stats(local_collector, "EntLisPtr", 4, m_entity_listeners);
    collect_ptr_array_stats(local_collector, "PhantLisPtr", 4, m_volume_listeners);
    collect_ptr_array_stats(local_collector, "ConstrLisPtr", 4, m_constraint_listeners);
    collect_ptr_array_stats(local_collector, "WldDelLisPtr", 4, m_world_delete_listeners);
    collect_ptr_array_stats(local_collector, "IslActLisPtr", 4, m_island_activation_listeners);
    collect_ptr_array_stats(local_collector, "PstSimLisPtr", 4, m_post_sim_listeners);
    collect_ptr_array_stats(local_collector, "PstIntLisPtr", 4, m_post_integrate_listeners);
    collect_ptr_array_stats(local_collector, "PstColLisPtr", 4, m_post_collide_listeners);
    collect_ptr_array_stats(local_collector, "PstIntLisPtr", 4, m_island_post_integ_listeners);
    collect_ptr_array_stats(local_collector, "CollLisPtr", 4, m_collision_listeners);
    local_collector->pop_dir();
    local_collector->end_object();
}

// @address 0x63f670 ??1phys_world@@UAA@XZ
phys_world::~phys_world()
{
    remove_object_ref((phys_ref_object *)m_maintenance_manager);
    m_maintenance_manager = 0;
    if (m_constraint_edit_lock) {
        delete (phys_critical_section *)m_constraint_edit_lock;
        m_constraint_edit_lock = 0;
    }
    if (m_property_master_lock) {
        delete m_property_master_lock;
        m_property_master_lock = 0;
        m_property_locks.m_size = 0;
    }
    if (m_world_lock) {
        delete (phys_critical_section *)m_world_lock;
        m_world_lock = 0;
    }
    if (m_island_dirty_lock) {
        delete m_island_dirty_lock;
        m_island_dirty_lock = 0;
    }
    if (m_memory_watchdog) {
        remove_object_ref(m_memory_watchdog);
        m_memory_watchdog = 0;
    }
    const phys_array<phys_island *> &world_active_islands = m_active_islands;
    {
        while (m_border_volumes.getSize() > 0) {
            remove_volume(m_border_volumes[0]);
        }
        while (world_active_islands.getSize() > 0 && world_active_islands[0]->m_island_entities.getSize()) {
            phys_entity *entity = world_active_islands[0]->m_island_entities[0];
            remove_entity(entity);
        }
        while (m_inactive_islands.getSize() > 0) {
            remove_entity(m_inactive_islands[0]->m_island_entities[0]);
        }
    }
    if (!m_want_islands) {
        phys_world_op_util::remove_island_from_dirty_list(this, world_active_islands.back());
        delete m_active_islands.back();
        m_active_islands.drop_back();
    }
    {
        remove_entity(m_fixed_body);
        m_fixed_body = 0;
        while (m_fixed_island->m_island_entities.getSize() > 0) {
            remove_entity(m_fixed_island->m_island_entities[0]);
        }
        phys_world_op_util::remove_island_from_dirty_list(this, m_fixed_island);
        delete m_fixed_island;
        m_fixed_island = 0;
    }
    phys_world_callback_util::notify_world_deleted(this);
    remove_object_ref((phys_ref_object *)m_broadphase);
    m_broadphase = 0;
    remove_object_ref((phys_ref_object *)m_collision_dispatcher);
    m_collision_dispatcher = 0;
    delete (phys_typed_broadphase_dispatcher *)m_broadphase_dispatcher;
    if (m_broadphase_border) {
        remove_object_ref(m_broadphase_border);
    }
    delete (phys_broadphase_listener *)m_volume_broadphase_listener;
    delete (phys_broadphase_listener *)m_entity_pair_listener;
    delete (phys_broadphase_listener *)m_broadphase_border_listener;
    remove_object_ref(m_collision_filter);
    remove_object_ref((phys_ref_object *)m_convex_list_filter);
    delete m_collision_input->m_agent_config;
    delete m_collision_input;
    remove_object_ref((phys_ref_object *)m_world_simulation);
    delete m_pending_ops;
}

extern "C" {
/* hkBroadPhase::m_defaultCreationFunction */
// @address 0x726480 g_phys_broadphase_default_factory
phys_broadphase *(*g_phys_broadphase_default_factory)(const phys_vec4 &min, const phys_vec4 &max, int sweep_marker_count) =
    phys_axis_sweep16_create_broadphase;
/* hkWorld::m_forceMultithreadedSimulation */
// @address 0x7ceae8 g_phys_world_force_mt_simulation
phys_bool g_phys_world_force_mt_simulation;
}

// @address 0x642058 ??0phys_world@@QAA@ABVphys_world_desc@@I@Z
phys_world::phys_world(const phys_world_desc &info, unsigned int sdk_version)
{
    m_last_entity_uid = (unsigned int)-1;

    m_pending_ops = new phys_world_op_queue(this);
    m_pending_op_queues = 0;
    m_pending_op_count = 0;
    m_critical_ops_lock_count = 0;
    m_critical_ops_volume_locks = 0;
    m_block_pending_ops = false;
    m_critical_ops_allowed = true;
    m_constraint_edit_lock = 0;
    m_property_master_lock = 0;
    m_world_lock = 0;
    m_island_dirty_lock = 0;
    m_pending_op_queue_count = 1;

    {
        m_broadphase_extents[0] = info.m_broadphase_bounds.m_min_limit;
        m_broadphase_extents[1] = info.m_broadphase_bounds.m_max_limit;
        m_broadphase_marker_count = info.m_broadphase_marker_count;
        m_broadphase = g_phys_broadphase_default_factory(
            m_broadphase_extents[0], m_broadphase_extents[1], info.m_broadphase_marker_count);
        m_broadphase_query_size = info.m_broadphase_query_size;
        m_broadphase_update_size = m_broadphase_query_size / 2;
    }
    m_toi_event_queue_size = info.m_toi_event_queue_size;

    if (sdk_version != k_havok_sdk_version_number) {
        char assert_buf[512];
        phys_err_stream report_stream(assert_buf, sizeof(assert_buf));
        report_stream << "** Havok libs built with version [" << static_cast<int>(k_havok_sdk_version_number)
             << "], used with code built with [" << sdk_version << "]. **";
        phys_singleton<phys_error>::get_singleton_instance().message(
            phys_error::MESSAGE_ERROR, 0x53c94b42, assert_buf, ".\\world\\hkWorld.cpp", 2443);
    }

    m_gravity = info.m_gravity;

    phys_edge_weld_util::init_edge_weld_table(info.m_convex_edge_snap_threshold, info.m_concave_edge_snap_threshold);

    m_convex_edge_snap_threshold = info.m_convex_edge_snap_threshold;
    m_concave_edge_snap_threshold = info.m_convex_edge_snap_threshold;
    m_toi_weld_reject = info.m_toi_weld_reject;
    m_wake_on_body_move = info.m_wake_on_body_move;
    m_want_legacy_sleep = info.m_want_legacy_sleep;
    m_toi_response_rotate_normal = info.m_toi_response_rotate_normal;
    m_hf_sleep_period = info.m_hf_sleep_period;
    m_low_freq_sleep_period = info.m_low_freq_sleep_period;
    m_sleep_ref_distance = info.m_sleep_ref_distance;

    {
        phys_solver_info &si = m_dynamics_step_info.m_solver_info;
        si.m_one_const = 1.0f;
        si.set_tau_and_damping(info.m_solver_tau, info.m_solver_damp);
        si.m_contact_resting_vel = info.m_contact_resting_vel;
        si.m_step_count = info.m_solver_iterations;
        si.m_inv_steps = 1.0f / info.m_solver_iterations;
        si.m_micro_step_count = info.m_solver_microsteps;
        si.m_inv_micro_steps = 1.0f / info.m_solver_microsteps;
        si.m_coherent_solver_order = info.m_coherent_solver_order;
        si.m_inactive_frame_select[0] = info.m_inactive_frame_select0;
        si.m_inactive_frame_select[1] = info.m_inactive_frame_select1;
        si.m_integrate_sleep_counter = info.m_integrate_sleep_counter;

        const float expected_delta_time = 0.016f;
        float gravity = info.m_gravity.length_3d();
        if (gravity == 0.0f) {
            gravity = 9.81f;
        }
        const float avg_object_size = gravity * 0.1f;

        for (int i = 0; i < phys_solver_info::SLEEP_CLASS_COUNT; i++) {
            float rel_vel_threshold;
            float rel_deceleration;
            float time_to_sleep;
            switch (i) {
            case phys_solver_info::SLEEP_CLASS_INVALID:
            case phys_solver_info::SLEEP_CLASS_OFF:
                rel_vel_threshold = 1.192092896e-07f;
                rel_deceleration = 0.0f;
                time_to_sleep = 1000;
                break;
            case phys_solver_info::SLEEP_CLASS_LOW:
                rel_vel_threshold = 0.01f;
                rel_deceleration = 0.08f;
                time_to_sleep = 0.1f;
                break;
            case phys_solver_info::SLEEP_CLASS_MEDIUM:
                rel_vel_threshold = 0.017f;
                rel_deceleration = 0.2f;
                time_to_sleep = 0.1f;
                break;
            case phys_solver_info::SLEEP_CLASS_HIGH:
                rel_vel_threshold = 0.02f;
                rel_deceleration = 0.3f;
                time_to_sleep = 0.10f;
                break;
            default:
            case phys_solver_info::SLEEP_CLASS_AGGRESSIVE:
                rel_vel_threshold = 0.025f;
                rel_deceleration = 0.4f;
                time_to_sleep = 0.05f;
                break;
            }
            phys_solver_info::sleep_info &di = si.m_deactivation_info[i];

            di.m_steps_to_sleep = (unsigned short)(time_to_sleep * si.m_step_count / expected_delta_time);

            const float vel_threshold = gravity * rel_vel_threshold;
            float deceleration = gravity * rel_deceleration / vel_threshold;

            di.m_slow_object_vel_mul = 1.0f - expected_delta_time * si.m_inv_steps * deceleration;

            di.m_inv_lin_vel_threshold = 1.0f / vel_threshold;
            di.m_ang_vel_threshold_inv = 1.0f / (avg_object_size * vel_threshold);

            if (rel_deceleration > 0) {
                di.m_rel_sleep_vel_threshold = expected_delta_time * si.m_inv_steps / rel_deceleration;
            } else {
                di.m_rel_sleep_vel_threshold = 3.40282e38f / 16.0f;
            }
            float q = info.m_sleep_ref_distance;
            di.m_max_dist_sq[0] = q;
            di.m_max_dist_sq[1] = q * 4;
            di.m_max_rot_sq[0] = q * 2;
            di.m_max_rot_sq[1] = q * 8;

            di.m_max_dist_sq[0] *= di.m_max_dist_sq[0];
            di.m_max_dist_sq[1] *= di.m_max_dist_sq[1];
            di.m_max_rot_sq[0] = di.m_max_rot_sq[0] * di.m_max_rot_sq[0];
            di.m_max_rot_sq[1] = di.m_max_rot_sq[0] * di.m_max_rot_sq[1];
        }
    }

    m_memory_watchdog = info.m_memory_watchdog;
    if (m_memory_watchdog != 0) {
        add_object_ref(m_memory_watchdog);
    }

    m_want_islands = info.m_islands_enabled;
    m_want_sleep = info.m_sleep_enabled;
    if (!m_want_islands && m_want_sleep) {
        m_want_sleep = false;
    }
    m_single_thread_actions = info.m_single_thread_actions;

    {
        m_broadphase_dispatcher = new phys_typed_broadphase_dispatcher();
        m_volume_broadphase_listener = new phys_volume_broadphase_listener();
        m_entity_pair_listener = new phys_entity_pair_broadphase_listener(this);
        m_broadphase_border_listener = new phys_broadphase_border_listener();

        m_broadphase_dispatcher->set_broadphase_listener(m_volume_broadphase_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
            phys_world_object::BROADPHASE_KIND_VOLUME);
        m_broadphase_dispatcher->set_broadphase_listener(m_volume_broadphase_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
            phys_world_object::BROADPHASE_KIND_ENTITY);
        m_broadphase_dispatcher->set_broadphase_listener(m_volume_broadphase_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
            phys_world_object::BROADPHASE_KIND_VOLUME);

        m_broadphase_dispatcher->set_broadphase_listener(m_entity_pair_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
            phys_world_object::BROADPHASE_KIND_ENTITY);

        m_broadphase_dispatcher->set_broadphase_listener(m_broadphase_border_listener, phys_world_object::BROADPHASE_KIND_ENTITY,
            phys_world_object::BROADPHASE_KIND_BORDER);
        m_broadphase_dispatcher->set_broadphase_listener(m_broadphase_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
            phys_world_object::BROADPHASE_KIND_ENTITY);

        m_broadphase_dispatcher->set_broadphase_listener(m_broadphase_border_listener, phys_world_object::BROADPHASE_KIND_VOLUME,
            phys_world_object::BROADPHASE_KIND_BORDER);
        m_broadphase_dispatcher->set_broadphase_listener(m_broadphase_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
            phys_world_object::BROADPHASE_KIND_VOLUME);
        m_broadphase_dispatcher->set_broadphase_listener(m_broadphase_border_listener, phys_world_object::BROADPHASE_KIND_BORDER,
            phys_world_object::BROADPHASE_KIND_BORDER);

        phys_contact_manager_factory *fallback_contact_factory = new phys_simple_contact_manager::report_contact_factory(this);

        m_collision_dispatcher = new phys_dispatcher(phys_null_agent::create_null_agent, fallback_contact_factory);

        remove_object_ref(fallback_contact_factory);

        if (info.m_collision_filter == 0) {
            m_collision_filter = new phys_null_collision_filter(phys_collision_filter::filter_kind_null);
        } else {
            m_collision_filter = info.m_collision_filter;
            add_object_ref(m_collision_filter);
        }

        if (info.m_convex_list_filter == 0) {
            m_convex_list_filter = new phys_default_convex_list_filter();
        } else {
            m_convex_list_filter = info.m_convex_list_filter;
            add_object_ref(m_convex_list_filter);
        }

        m_collision_input = new phys_process_collision_input;
        phys_process_collision_input &collision_input_ref = *m_collision_input;

        collision_input_ref.m_agent_dispatcher = get_dispatcher();
        collision_input_ref.m_box_tolerance = info.m_collision_tolerance;
        collision_input_ref.m_compound_filter = m_collision_filter;
        collision_input_ref.m_convex_list_filter = m_convex_list_filter;

        collision_input_ref.m_agent_config = new phys_agent_config();
        m_contact_point_gen = info.m_contact_point_gen;
        collision_input_ref.m_agent_config->m_iter_cast_early_out_dist = info.m_iter_cast_early_out_dist;
        collision_input_ref.m_agent_config->m_iter_cast_max_iters = info.m_iter_cast_max_iters;
        collision_input_ref.m_legacy_edge_weld = info.m_legacy_edge_weld;

        collision_input_ref.m_create_pred_agents = phys_bool(false);
        collision_input_ref.m_quality_info = collision_input_ref.m_agent_dispatcher->get_collision_quality_info(phys_dispatcher::COLLIDE_QUALITY_DISCRETE);

        phys_world_setup_contact_factories(this, get_dispatcher());
    }

    if (g_phys_world_force_mt_simulation) {
        phys_world_desc *world_desc_mut = const_cast<phys_world_desc *>(&info);
        world_desc_mut->m_simulation_type = phys_world_desc::SIM_KIND_MULTITHREADED;
    }

    m_min_island_size = 0;
    {
        m_simulation_type = info.m_simulation_type;
        switch (m_simulation_type) {
        case phys_world_desc::SIM_KIND_DISCRETE:
            m_world_simulation = new phys_simulation(this);
            break;
        case phys_world_desc::SIMULATION_TYPE_CONTINUOUS:
            m_world_simulation = new phys_continuous_simulation(this);
            break;
        case phys_world_desc::SIM_KIND_MULTITHREADED: {
            m_constraint_edit_lock = new phys_critical_section(4000);
            m_property_master_lock = new phys_critical_section(4000);
            m_world_simulation = new phys_multithreaded_simulation(this);
            m_min_island_size = info.m_min_island_size;
            break;
        }
        default: {
            break;
        }
        }
        m_world_lock = new phys_critical_section(4000L);
        m_island_dirty_lock = new phys_critical_section(4000L);
        m_world_simulation->m_frame_marker_psi_snap = info.m_frame_marker_psi_snap;
    }

    {
        float gravity_len = m_gravity.length_3d();
        if (gravity_len == 0.0f) {
            gravity_len = 9.81f;
        }

        phys_dispatcher::quality_init_info input;
        input.m_gravity_len = gravity_len;
        input.m_collision_tolerance = m_collision_input->m_box_tolerance;
        input.m_min_dt = info.m_expected_min_psi_dt;
        input.m_max_lin_vel = info.m_expected_max_lin_vel;
        input.m_ccd_enabled = info.m_simulation_type >= phys_world_desc::SIMULATION_TYPE_CONTINUOUS;
        input.m_neg_manifold_sweeps = info.m_contact_point_gen == phys_world_desc::CONTACT_POINT_REJECT_MANY;
        input.m_neg_tol_non4d_contacts = info.m_contact_point_gen >= phys_world_desc::CONTACT_REJECT_DUBIOUS;
        input.m_default_constraint_priority = phys_constraint::PRIORITY_PSI;
        input.m_toi_constraint_priority = phys_constraint::CONSTRAINT_PRIORITY_TOI;
        input.m_toi_higher_priority = phys_constraint::CONSTRAINT_PRIORITY_TOI_HIGHER;
        input.m_toi_forced_priority = phys_constraint::CONSTRAINT_PRIORITY_TOI_FORCED;
        input.m_toi_weld_reject = info.m_toi_weld_reject;

        get_dispatcher()->init_collision_quality(input);

        phys_dispatcher *world_dispatcher = get_dispatcher();
        m_collision_input->m_quality_info = world_dispatcher->get_collision_quality_info(world_dispatcher->COLLIDE_QUALITY_DISCRETE);
    }

    {
        m_fixed_island = new phys_island(this);
        m_fixed_island->m_storage_idx = 0xffff;
        m_fixed_island->m_active = false;
        m_fixed_island->m_in_active_islands = false;

        if (!m_want_islands) {
            phys_island *active_island = new phys_island(this);
            m_active_islands.array_push_back(active_island);
            active_island->m_storage_idx = 0;
        }
    }

    {
        phys_rigid_body_desc fixed_body_desc;
        fixed_body_desc.m_motion_type = phys_motion::MOTION_FIXED;
        fixed_body_desc.m_body_mass = 0;
        m_fixed_body = new phys_rigid_body(fixed_body_desc);
        add_chain_entity(m_fixed_body, PHYS_ENTITY_ACTIVATION_DO_ACTIVATE);
        ((phys_world_object *)m_fixed_body)->object_remove_ref();
    }

    {
        m_dynamics_step_info.m_step_info.m_delta_time = 0.0f;
        m_collision_input->m_dynamics_info = &m_dynamics_step_info;
    }

    if (info.m_border_behaviour != info.BROADPHASE_BORDER_DO_NOTHING) {
        m_broadphase_border = new phys_broadphase_border(this, info.m_border_behaviour);
    } else {
        m_broadphase_border = 0;
    }

    m_maintenance_manager = new phys_default_maintenance_manager();
    m_maintenance_manager->init(this);

    if (info.m_simulation_type != phys_world_desc::SIM_KIND_MULTITHREADED) {
        m_mt_lock.disable_checks();
    }
}

typedef char phys_world_island_size[(sizeof(void *) != 4 || (sizeof(phys_island) == 0x70)) ? 1 : -1];
