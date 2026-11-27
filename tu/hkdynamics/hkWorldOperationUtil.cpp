// hkdynamics/hkWorldOperationUtil.obj: from the records 0x105ff8 0x644458 0x6444d0 0x644518 0x644590 0x6445d0 0x6446a8 0x644788 0x644858 0x6448e0 0x644960 0x644a20 0x644a78 0x644aa0 0x644c70 0x644da0 0x644e60 0x645050 0x645238 0x6453c8 0x645458 0x645520 0x6455c0 0x645a08 0x645a98 0x645cd8 0x646120 0x646698 0x646838 0x646b90 0x646d48 0x646ed8 0x647080 0x6471e8 0x6473c0 0x647428 0x647638 0x648688 0x648770 0x6487d8 0x648cb0 0x648df0 0x648e10
// @flags /TP /O2 /Oc
// @address 0x644518 ?add_constraint_now@phys_world_op_util@@SAPAVphys_constraint@@PAVphys_world@@PAV2@W4callback_firing@1@@Z
// @address 0x6445d0 ?remove_constraint_now@phys_world_op_util@@SAXPAVphys_world@@PAVphys_constraint@@W4callback_firing@1@@Z
// @address 0x6446a8 ?find_inactive_groups@@YAXABVphys_union_find@@ABV?$phys_array@PAVphys_entity@@@@PBVphys_world@@HAAV?$phys_fixed_array@H@@@Z
// @address 0x648cb0 ??$quick_sort_recursive@PAVphys_island@@P6A?AVphys_bool@@PBV1@0@Z@phys_algorithm@@YAXPAPAVphys_island@@HHP6A?AVphys_bool@@PBV1@1@Z@Z


#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_world_agent_util.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_math.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_object.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_constraint_info.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_determinism_util.h"
#include "phys/phys_local_array.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_island.h"
#include "phys/phys_simulation.h"
#include "phys/phys_broadphase.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint.h"
#include "phys/phys_world_constraint_util.h"
#include "phys/phys_action.h"
#include "phys/phys_null_action.h"
#include "phys/phys_union_find.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_string.h"
#include "phys/phys_world_op_queue.h"
#include "phys/phys_world_callback_util.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_thread.h"

struct phys_max_size_motion_storage {
    phys_vec4 m_motion_quads[sizeof(phys_max_size_motion) / sizeof(phys_vec4)];
    __forceinline phys_max_size_motion *init() {
        return new (this) phys_max_size_motion();
    }
};
namespace {
template <class T> struct phys_broadphase_local_array : phys_array<T> {
    T *m_local_memory;
    __forceinline phys_broadphase_local_array(int size) {
        this->m_data = 0;
        this->m_size = 0;
        this->m_capacity_and_flags = PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
        T *local_memory = (T *)allocate_local_stack<T>(size);
        this->m_capacity_and_flags = size | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
        this->m_data = local_memory;
        m_local_memory = local_memory;
    }
    __forceinline ~phys_broadphase_local_array() { deallocate_local_stack(m_local_memory); }
};

}

static __forceinline int phys_world_op_inactive_frames_mt(const phys_motion &motion, int select, int world_flag)
{
    int dc = motion.m_inactive_frame_counts[select];
    int motion_sleep_flag = dc >> 14;
    int inactive_frame_count;
    if (world_flag != motion_sleep_flag) inactive_frame_count = dc & 0x7f;
    else inactive_frame_count = (dc >> 7) & 0x7f;
    return inactive_frame_count & 0x7f;
}
static __forceinline int phys_get_num_inactive_frames(const phys_motion *m, int select, const unsigned char *flags)
{
    int c = m->m_inactive_frame_counts[select];
    if (flags[select] == (c >> 14)) return (c >> 7) & 0x7f;
    return c & 0x7f;
}

#define PHYS_ARRAY_PUSH_BACK(T, A, V) do { phys_array<T> *phys_push_array_ = (A); \
    if (phys_push_array_->m_size == phys_push_array_->get_sector_capacity()) phys_array_util::grow_array_storage(phys_push_array_, sizeof(T)); \
    phys_push_array_->m_data[phys_push_array_->m_size++] = (V); } while (0)

#define PHYS_ARRAY_SET_SIZE(T, A, N) do { phys_array<T> *phys_size_array_ = (A); int phys_size_count_ = (N); \
    int phys_size_cap_ = phys_size_array_->get_sector_capacity(); \
    if (phys_size_cap_ < phys_size_count_) { \
        int phys_size_cap2_ = phys_size_cap_ + phys_size_cap_; \
        int phys_size_new_cap_ = (phys_size_count_ < phys_size_cap2_) ? phys_size_cap2_ : phys_size_count_; \
        phys_array_util::reserve_array_storage(phys_size_array_, phys_size_new_cap_, sizeof(T)); \
    } \
    phys_size_array_->m_size = phys_size_count_; } while (0)
/* hkArray::expandOne() as a statement: R = &a[size++] after growing. */
#define PHYS_ARRAY_EXPAND_ONE(T, A, R) do { phys_array<T> *phys_expand_array_local = (A); \
    if (phys_expand_array_local->m_size == phys_expand_array_local->get_sector_capacity()) \
        phys_array_util::grow_array_storage(phys_expand_array_local, sizeof(T)); \
    (R) = &phys_expand_array_local->m_data[phys_expand_array_local->m_size++]; } while (0)


// @address 0x644da0 ?remove_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z

typedef phys_ref_object phys_ref_header;

// match: 0x647638: preserve the two TLS helper boundaries for sector-loop registers and call scheduling.
static inline phys_thread_memory *phys_world_op_thread_memory_ptr() {
    return (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
}
static inline phys_thread_memory &phys_world_op_thread_memory() {
    phys_thread_memory *instance = phys_world_op_thread_memory_ptr();
    return *instance;
}


namespace {
static __forceinline void leave_island_lock(phys_critical_section *lock) {
    lock->m_current_thread = -1;
    RtlLeaveCriticalSection(&lock->m_section);
}
}
static __forceinline void object_remove_ref(phys_ref_header *object) {
    if (object->m_mem_size_and_flags) {
        if (--object->m_object_ref_count == 0) {
            if (halo_managed_memory_address(object)) object->m_object_ref_count = 1;
            else delete object;
        }
    }
}
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ

namespace {
template <class T> static __forceinline void *allocate_local_stack(int count)
{
    return phys_thread_memory::get_singleton_instance().allocate_stack(count * (int)sizeof(T));
}
static __forceinline void deallocate_local_stack(void *block)
{
    phys_thread_memory::get_singleton_instance().free_stack(block);
}

}


static __forceinline void phys_mem_copy16(void *dst, const void *src, int copy_quad_count)
{
    const unsigned int *src_u32 = (const unsigned int *)src;
    unsigned int *dst_u32 = (unsigned int *)dst;
    for (int i = 0; i < copy_quad_count; i++) {
        dst_u32[0] = src_u32[0];
        dst_u32[1] = src_u32[1];
        dst_u32[2] = src_u32[2];
        dst_u32[3] = src_u32[3];
        dst_u32 += 4;
        src_u32 += 4;
    }
}

namespace {
static __forceinline void remove_attached_constraint(phys_world *world, phys_constraint *constraint) {
    static_cast<phys_island *>(constraint->get_collider_owner())->m_split_check_requested = 1;
    if (world->critical_ops_locked()) {
        phys_world_constraint_util::remove_constraint(constraint);
    } else {
        world->lock_critical_ops();
        phys_world_constraint_util::remove_constraint(constraint);
        world->unlock_and_flush_ops();
    }
}
}
static __forceinline bool can_island_be_sparse(phys_world *world, int size) { return size < world->m_min_island_size; }

namespace {
static __forceinline void put_island_on_dirty_list(phys_world *world, phys_island *island)
{
    if (island->m_dirty_list_idx == 0xffff) {
        island->m_dirty_list_idx = (unsigned short)world->m_dirty_islands.getSize();
        PHYS_ARRAY_PUSH_BACK(phys_island *, &world->m_dirty_islands, island);
    }
}
static __forceinline void merge_islands_if_needed(phys_entity *listener_entity_a, phys_entity *listener_entity_b)
{
    if (!listener_entity_a->body_is_fixed() && !listener_entity_b->body_is_fixed()) {
        phys_island *island_a = listener_entity_a->get_island();
        if (island_a != listener_entity_b->get_island())
            phys_world_op_util::merge_islands(listener_entity_a->m_world, listener_entity_a, listener_entity_b);
    }
}
}

// @address 0x105ff8 ?add_constraint_info@phys_constraint_owner@@QAAXPAVphys_constraint@@AAUphys_constraint_info@@@Z
// 0x105ff8
// match: 0x647638: inline keeps splitSimulationIsland's loop registers (r24-r26 for the island entries); out of line they shift up by one
#if defined(_XBOX) && !defined(__clang__)
inline void phys_constraint_owner::add_constraint_info(phys_constraint *constraint, phys_constraint_info &delta)
#else
void phys_constraint_owner::add_constraint_info(phys_constraint *constraint, phys_constraint_info &delta)
#endif
{
    if (constraint->m_constraint_internal) {
        m_constraint_info.add(delta);
        constraint->m_constraint_internal->add_constraint_info(delta);
        m_constraint_info.m_max_schema_size = phys_math::max_of_two(m_constraint_info.m_max_schema_size,
            (int)constraint->m_constraint_internal->m_schemas_size);
    }
}

// @address 0x644458 ?sub_constraint_info@phys_constraint_owner@@QAAXPAVphys_constraint@@AAUphys_constraint_info@@@Z
// 0x644458

// @address 0x6444d0 ?add_constraint_to_locked_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_constraint@@@Z
// 0x6444d0
void phys_world_op_util::add_constraint_to_locked_island(phys_world *world, phys_constraint *constraint)
{
    phys_world_constraint_util::add_world_constraint(world, constraint);
    phys_world_callback_util::notify_constraint_added(world, constraint);
}

// 0x644518
phys_constraint *phys_world_op_util::add_constraint_now(phys_world *world, phys_constraint *constraint, callback_firing fire_pending_callbacks)
{
    world->lock_critical_ops();
    phys_world_constraint_util::add_world_constraint(world, constraint);
    if (fire_pending_callbacks)
        phys_world_callback_util::notify_constraint_added(world, constraint);
    world->unlock_and_flush_ops();
    return constraint;
}

// @address 0x644590 ?remove_constraint_locked_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_constraint@@@Z
// 0x644590
void phys_world_op_util::remove_constraint_locked_island(phys_world *world, phys_constraint *constraint)
{
    if (world->m_constraint_listeners.m_size)
        phys_world_callback_util::notify_constraint_removed(world, constraint);
    phys_world_constraint_util::remove_constraint(constraint);
}

// 0x6445d0
void phys_world_op_util::remove_constraint_now(phys_world *world, phys_constraint *constraint, callback_firing fire_pending_callbacks)
{
    static_cast<phys_island *>(constraint->get_collider_owner())->m_split_check_requested = 1;
    if (world->critical_ops_locked())
    {
        if (fire_pending_callbacks && world->m_constraint_listeners.m_size)
            phys_world_callback_util::notify_constraint_removed(world, constraint);
        phys_world_constraint_util::remove_constraint(constraint);
    }
    else
    {
        world->lock_critical_ops();
        if (fire_pending_callbacks && world->m_constraint_listeners.m_size)
            phys_world_callback_util::notify_constraint_removed(world, constraint);
        phys_world_constraint_util::remove_constraint(constraint);
        world->unlock_and_flush_ops();
    }
}

// 0x6446a8
void find_inactive_groups(const phys_union_find &union_find_checker,
    const phys_array<phys_entity *> &entities,
    const phys_world *world,
    int union_group_count,
    phys_fixed_array<int> &groups_active)
{
    int i;
    for (i = 0; i < union_group_count; i++) groups_active[i] = 0;
    const unsigned char *flags = world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select;
    for (i = 0; i < entities.getSize(); i++) {
        const phys_motion *motion = &entities[i]->m_motion;
        int inactive_frames1 = phys_get_num_inactive_frames(motion, 1, flags);
        int inactive_frames0 = phys_get_num_inactive_frames(motion, 0, flags);
        int max_inactive_frames = inactive_frames1 & 0x7f;
        if ((inactive_frames0 & 0x7f) > max_inactive_frames) max_inactive_frames = inactive_frames0 & 0x7f;
        if (max_inactive_frames <= 5) groups_active[union_find_checker.m_node_parents[i]] = 1;
    }
}

// @address 0x644788 ?find_inactive_and_estimate@@YAXABVphys_union_find@@ABV?$phys_array@PAVphys_entity@@@@HAAV?$phys_fixed_array@H@@2PAE@Z
// 0x644788
void find_inactive_and_estimate(const phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, int union_group_count,
    phys_fixed_array<int> &groups_active, phys_fixed_array<int> &solver_sizes, unsigned char *flags)
{
    for (int i = 0; i < entities.getSize(); i++) {
        phys_entity *entity = entities[i];
        int group = union_find_checker.m_node_parents[i];
        int n1 = phys_get_num_inactive_frames(&entity->m_motion, 1, flags);
        int n0 = phys_get_num_inactive_frames(&entity->m_motion, 0, flags);
        int mx = n1 & 0x7f;
        if ((n0 & 0x7f) > mx) mx = n0 & 0x7f;
        if (mx <= 5) groups_active[group] = 1;
        int s = entity->m_master_constraints.m_size;
        if (s < 1) s = 1;
        solver_sizes[group] += s;
    }
}

// @address 0x644858 ?estimate_solver_size@@YAXABVphys_union_find@@ABV?$phys_array@PAVphys_entity@@@@HAAV?$phys_fixed_array@H@@2@Z
void estimate_solver_size(const phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, int union_group_count,
    phys_fixed_array<int> &groups_active,
    phys_fixed_array<int> &solver_sizes)
{
    for (int i = 0; i < entities.getSize(); ++i) {
        phys_entity *entity = entities[i];
        int group = union_find_checker.m_node_parents[i];
        groups_active[group] = 1;
        int size = entity->m_master_constraints.m_size;
        int solver_size = (size < 1) ? 1 : size;
        solver_sizes[group] += solver_size;
    }
}

// @address 0x6448e0 ?validate_world@phys_world_op_util@@SAXPAVphys_world@@@Z
// 0x6448e0
void phys_world_op_util::validate_world(phys_world *world)
{
    const phys_array<phys_island *> &world_active_islands = world->m_active_islands;
    for (int i = world_active_islands.getSize() - 1; i >= 0; --i)
        (world_active_islands[i])->is_valid_aabb();
    for (int i = world->m_inactive_islands.getSize() - 1; i >= 0; --i)
        (world->m_inactive_islands[i])->is_valid_aabb();
    (world->m_fixed_island)->is_valid_aabb();
}

// @address 0x644960 ?detach_fixed_partner_agents@phys_world_op_util@@CAXAAUphys_pair_agent_track@@PAVphys_entity@@0W4motion_kind@phys_motion@@@Z
void phys_world_op_util::detach_fixed_partner_agents(phys_pair_agent_track &track_to_scan,
    phys_entity *entity,
    phys_pair_agent_track &removed_agents,
    phys_motion::motion_kind new_motion_type)
{
    for (int i = 0; i < entity->get_linked_collider()->m_collision_entries.getSize(); i++) {
        phys_linked_collider::collision_entry &entry = entity->get_linked_collider()->m_collision_entries[i];
        phys_entity *partner = static_cast<phys_entity *>(entry.m_linked_partner->get_collider_owner());
        if (partner->body_is_fixed()) {
            phys_pair_agent_entry *old_agent_entry = entry.m_agent_entry;
            phys_pair_agent_copy_relink_entry(&removed_agents, old_agent_entry);
            phys_pair_agent_free_entry_internal(track_to_scan, old_agent_entry);
        } else if (new_motion_type == 7) {
            phys_island *island = ::phys_world_agent_util::island_from_agent_entry(entry.m_agent_entry,
                entity->m_owner_island, partner->m_owner_island);
            if (island == entity->m_owner_island) {
                phys_pair_agent_entry *old_agent_entry = entry.m_agent_entry;
                phys_pair_agent_copy_relink_entry(&partner->m_owner_island->m_agent_track, entry.m_agent_entry);
                phys_pair_agent_free_entry_internal(track_to_scan, old_agent_entry);
            }
        }
    }
}

// @address 0x644a78 ?remove_island_from_dirty_list@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
// 0x644a78
void phys_world_op_util::remove_island_from_dirty_list(phys_world *world, phys_island *island)
{
    if (island->m_dirty_list_idx != 0xffff) {
        world->m_dirty_islands[island->m_dirty_list_idx] = 0;
        island->m_dirty_list_idx = 0xffff;
    }
}

// @address 0x644a20 ?less_island_ptr@@YA?AVphys_bool@@PBVphys_island@@0@Z
static phys_bool less_island_ptr(const phys_island *a, const phys_island *b)
{
    if (b == 0) return phys_bool(false);
    if (a == 0) return phys_bool(true);
    return phys_bool(a->m_island_entities[0]->m_entity_uid < b->m_island_entities[0]->m_entity_uid);
}

// @address 0x644aa0 ?add_entity_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@W4phys_entity_activation@@@Z
void phys_world_op_util::add_entity_island(phys_world *world, phys_entity *entity, phys_entity_activation initial_activation)
{
    entity->m_world = world;
    if (entity->body_is_fixed()) {
        world->m_fixed_island->add_entity_internal(entity);
        return;
    }
    phys_island *island;
    if (world->m_want_islands) {
        bool is_body_active = initial_activation == PHYS_ENTITY_ACTIVATION_DO_ACTIVATE;
        phys_array<phys_island *> &islands = is_body_active ? world->m_active_islands : world->m_inactive_islands;
        if (is_body_active) {
            int i = islands.getSize();
            int end = phys_math::max_of_two(0, i - 10);
            while (--i >= end) {
                phys_island *candidate = islands[i];
                int size = phys_math::max_of_two(candidate->m_island_entities.getSize() + 1, candidate->m_constraint_count + 3);
                if (candidate->m_active && can_island_be_sparse(world, size)) {
                    island = islands.back();
                    island->m_sparse_enabled = 1;
                    island->add_entity_internal(entity);
                    return;
                }
            }
        }
        phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        void *storage = ((phys_thread_memory *)memory)->allocate_chunk(sizeof(phys_island), PHYS_MEM_ISLAND);
        ((phys_ref_object *)storage)->m_mem_size_and_flags = sizeof(phys_island);
        island = new (storage) phys_island(world);
        island->m_active = is_body_active;
        island->m_in_active_islands = is_body_active;
        island->m_storage_idx = (unsigned short)islands.getSize();
        island->m_split_check_frames = (unsigned char)island->m_storage_idx;
        PHYS_ARRAY_PUSH_BACK(phys_island *, &islands, island);
    } else {

        island = world->m_active_islands[0];
    }
    island->add_entity_internal(entity);
    island->m_split_check_frames = (unsigned char)entity->m_entity_uid;
}

// @address 0x644c70 ?detach_actions_fixed_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@AAV?$phys_array@PAVphys_action@@@@@Z
// 0x644c70
void phys_world_op_util::detach_actions_fixed_island(phys_world *world, phys_entity *entity, phys_array<phys_action *> &removed_actions)
{
    for (int i = 0; i < entity->m_actions.m_size; ++i) {
        phys_action *action = entity->m_actions[i];
        if (action->m_sim_island == world->m_fixed_island) {
            PHYS_ARRAY_PUSH_BACK(phys_action *, &removed_actions, action);
            action->add_object_reference();
            world->m_fixed_island->remove_action(action);
            world->m_fixed_island->m_action_cleanup_needed = 1;
            phys_island *island = world->m_fixed_island;
            put_island_on_dirty_list(world, island);
        }
    }
}

// @address 0x644da0 ?remove_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_op_util::remove_island(phys_world *world, phys_island *island)
{
    if (!island->m_in_active_islands) {
        world->m_inactive_islands[island->m_storage_idx] =
            world->m_inactive_islands[world->m_inactive_islands.getSize() - 1];
        world->m_inactive_islands[island->m_storage_idx]->m_storage_idx = island->m_storage_idx;
        world->m_inactive_islands.drop_back();
    } else {
        phys_array<phys_island *> &world_active_islands = world->m_active_islands;
        world_active_islands[island->m_storage_idx] = world_active_islands.back();
        world_active_islands[island->m_storage_idx]->m_storage_idx = island->m_storage_idx;
        world_active_islands.drop_back();
    }
    if (island->m_dirty_list_idx != 0xffff) {
        world->m_dirty_islands[island->m_dirty_list_idx] = 0;
        island->m_dirty_list_idx = 0xffff;
    }
}

// @address 0x644e60 ?detach_constraints@phys_world_op_util@@CAXPAVphys_entity@@AAV?$phys_array@PAVphys_constraint@@@@@Z
void phys_world_op_util::detach_constraints(phys_entity *entity, phys_array<phys_constraint *> &removed_constraints)
{
    phys_world *world = entity->m_world;
    for (int i = entity->m_master_constraints.m_size - 1; i >= 0; --i) {
        phys_constraint *constraint = entity->m_master_constraints.m_data[i].m_contact_constraint;
        constraint->add_object_reference();
        remove_attached_constraint(world, constraint);
        PHYS_ARRAY_PUSH_BACK(phys_constraint *, &removed_constraints, constraint);
    }
    phys_array<phys_constraint *> &slave_constraints = entity->m_slave_constraints;
    for (int i = slave_constraints.getSize() - 1; i >= 0; --i) {
        phys_constraint *constraint = slave_constraints[i];
        constraint->add_object_reference();
        remove_attached_constraint(world, constraint);
        PHYS_ARRAY_PUSH_BACK(phys_constraint *, &removed_constraints, constraint);
    }
}

static __forceinline void phys_set_world_select_flags_neg(phys_motion *m, int world_flag0, int world_flag1, int world_sleep_counter)
{
    if (((unsigned)world_sleep_counter % 4) < ((unsigned)m->m_integrate_sleep_counter % 4)) world_flag0 = (~world_flag0) << 14;
    else world_flag0 <<= 14;
    if (world_sleep_counter < m->m_integrate_sleep_counter) world_flag1 = (~world_flag1) << 14;
    else world_flag1 <<= 14;
    m->m_inactive_frame_counts[0] = (unsigned short)((m->m_inactive_frame_counts[0] & 0x3fff) | world_flag0);
    m->m_inactive_frame_counts[1] = (unsigned short)((m->m_inactive_frame_counts[1] & 0x3fff) | world_flag1);
}

static __forceinline void sort_big_island_to_front(phys_world *world, phys_island *island)
{
    int storage_idx = island->m_storage_idx;
    if (storage_idx == 0) return;
    phys_array<phys_island *> &world_active_islands = world->m_active_islands;
    phys_island *other = world_active_islands[0];
    if (island->m_island_entities.getSize() > other->m_island_entities.getSize()) {
        island->m_storage_idx = 0;
        other->m_storage_idx = (unsigned short)storage_idx;
        world_active_islands[0] = island;
        world_active_islands[storage_idx] = other;
    }
}

// @address 0x645050 ?activate_island_internal@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_op_util::activate_island_internal(phys_world *world, phys_island *island)
{
    world->m_inactive_islands[island->m_storage_idx] =
            world->m_inactive_islands[world->m_inactive_islands.getSize() - 1];
    world->m_inactive_islands[island->m_storage_idx]->m_storage_idx = island->m_storage_idx;
    world->m_inactive_islands.drop_back();
    PHYS_ARRAY_PUSH_BACK(phys_island *, &world->m_active_islands, island);
    island->m_storage_idx = (unsigned short)(world->m_active_islands.getSize() + 0xffff);
    island->m_in_active_islands = 1;
    island->m_since_high_freq_check = 0.0f;
    island->m_since_low_freq_check = 0.0f;
    for (int e = 0; e < island->m_island_entities.getSize(); e++) {
        phys_entity *body = island->m_island_entities[e];
        phys_swept_transform_util::set_time_info(0.0f, 0.0f, body->m_motion.m_motion_state);
        unsigned char *sleep_flags = world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select;
        phys_set_world_select_flags_neg(&body->m_motion,
            sleep_flags[0],
            sleep_flags[1],
            world->m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter);
    }
    phys_world_agent_util::warp_time(island,
        island->m_sleep_time,
        world->m_dynamics_step_info.m_step_info.m_step_start_time,
        *(phys_collision_input *)world->m_collision_input);
    sort_big_island_to_front(world, island);
    phys_world_callback_util::notify_island_activated(world, island);
}

// 0x645238
// @address 0x645238 ?deactivate_island_internal@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_op_util::deactivate_island_internal(phys_world *world, phys_island *island)
{
    phys_step_info *step_info = &world->m_dynamics_step_info.m_step_info;
    phys_entity **entities = island->m_island_entities.begin();
    int solver_entity_count = island->m_island_entities.getSize();
    if (!phys_rigid_motion_can_sleep_final(&world->m_dynamics_step_info.m_solver_info,
        step_info,
        reinterpret_cast<phys_motion *const *>(entities),
        solver_entity_count,
        (int)offsetof(phys_entity, m_motion))) {
        island->m_active = 1;
        return;
    }
    PHYS_ARRAY_PUSH_BACK(phys_island *, &world->m_inactive_islands, island);
    world->m_active_islands[island->m_storage_idx] = world->m_active_islands[world->m_active_islands.getSize() - 1];
    world->m_active_islands[island->m_storage_idx]->m_storage_idx = island->m_storage_idx;
    world->m_active_islands.drop_back();
    island->m_storage_idx = (unsigned short)(world->m_inactive_islands.getSize() + 0xffff);
    island->m_in_active_islands = 0;
    island->m_sleep_time = step_info->m_step_start_time;
    for (int i = 0; i < island->m_island_entities.getSize(); i++) {
        phys_entity *rb = island->m_island_entities[i];
        phys_swept_transform_util::freeze_motion_state(island->m_sleep_time, rb->m_motion.m_motion_state);
        rb->m_motion.set_linear_velocity(phys_vec4::get_zero());
        rb->m_motion.set_angular_velocity(phys_vec4::get_zero());
    }
    phys_world_callback_util::notify_island_deactivated(world, island);
    world->check_determinism();
}

// @address 0x6453c8 ?mark_island_inactive@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
// 0x6453c8
void phys_world_op_util::mark_island_inactive(phys_world *world, phys_island *island)
{
    island->m_active = 0;
    put_island_on_dirty_list(world, island);
}

// @address 0x645458 ?mark_island_inactive_mt@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_op_util::mark_island_inactive_mt(phys_world *world, phys_island *island)
{
    island->m_active = 0;
    phys_critical_section *lock = world->m_island_dirty_lock;
    if (!RtlTryEnterCriticalSection(&lock->m_section)) {
        if (phys_critical_section::m_time_locks.get_chain_data() != 0) RtlEnterCriticalSection(&lock->m_section);
        else RtlEnterCriticalSection(&lock->m_section);
    }
    lock->m_current_thread = phys_thread::get_current_thread_id();
    put_island_on_dirty_list(world, island);
    leave_island_lock(world->m_island_dirty_lock);
}

// @address 0x645520 ?mark_island_active@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
// 0x645520
void phys_world_op_util::mark_island_active(phys_world *world, phys_island *island)
{
    island->m_active = 1;
    island->m_low_freq_sleep_counter = 0;
    island->m_hf_sleep_counter = 0;
    put_island_on_dirty_list(world, island);
}

// @address 0x645a08 ?remove_entity_from_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
// 0x645a08
void phys_world_op_util::remove_entity_from_island(phys_world *world, phys_entity *entity)
{
    entity->m_world = 0;
    phys_island *island = entity->get_island();
    island->remove_entity_internal(entity);
    if (island->m_storage_idx != 0xffff && island->m_island_entities.getSize() == 0 && world->m_want_islands) {
        remove_island(world, island);
        delete island;
    }
}

// 0x646698
// @address 0x646698 ?cleanup_dirty_islands@phys_world_op_util@@SAXPAVphys_world@@@Z
void phys_world_op_util::cleanup_dirty_islands(phys_world *world)
{
    if (world->m_dirty_islands.getSize() > 1)
        phys_algorithm::quick_sort_recursive(
            world->m_dirty_islands.begin(), 0, world->m_dirty_islands.getSize() - 1, less_island_ptr);
    while (world->m_dirty_islands.getSize() != 0) {
        phys_island *sim_island = world->m_dirty_islands.back();
        world->m_dirty_islands.drop_back();
        if (sim_island == 0) continue;
        sim_island->m_dirty_list_idx = 0xffff;
        if (sim_island->m_action_cleanup_needed) {
            phys_array<phys_action *> *a = &sim_island->m_actions;
            int j = -1;
            int i = 0;
            for (; i < a->getSize(); i++) {
                if ((*a)[i] == 0) { j = i; i++; break; }
            }
            for (; i < a->getSize(); i++) {
                if ((*a)[i] != 0) { (*a)[j] = (*a)[i]; j++; }
            }
            if (j != -1) PHYS_ARRAY_SET_SIZE(phys_action *, a, j);
            sim_island->m_action_cleanup_needed = 0;
        }
        if (sim_island->m_active != sim_island->m_in_active_islands) {
            if (sim_island->m_active) phys_world_op_util::activate_island_internal(world, sim_island);
            else phys_world_op_util::deactivate_island_internal(world, sim_island);
        }
    }
}

// @address 0x646838 ?update_entity_broadphase@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_op_util::update_entity_broadphase(phys_world *world, phys_entity *entity)
{
    if (world->critical_ops_locked()) {
        phys_world_op::update_entity_broadphase_op operation;
        operation.m_op_entity = entity;
        world->queue_world_op(operation);
        return;
    }
    phys_linked_collider *collidable = entity->get_linked_collider();
    const phys_shape *shape = collidable->m_proxy_shape;
    world->lock_critical_ops();
    if (shape) {
        phys_aabb aabb;
        phys_broadphase_local_array<phys_broadphase_handle_pair> new_pairs(world->m_broadphase_query_size);
        phys_broadphase_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(world->m_broadphase_query_size);
        collidable->m_proxy_shape->get_handle_aabb(collidable->get_transform(), world->m_collision_input->m_box_tolerance * 0.5f, aabb);
        phys_broadphase_handle *handle = &collidable->m_broadphase_handle;
        world->m_broadphase->update_aabbs(&handle, &aabb, 1, new_pairs, sweep_removed_pairs);
        if (new_pairs.getSize() + sweep_removed_pairs.getSize() > 0) {
            phys_typed_broadphase_dispatcher::remove_duplicate_pairs(new_pairs, sweep_removed_pairs);
            ((phys_typed_broadphase_dispatcher const *)world->m_broadphase_dispatcher)
                ->remove_handle_pairs((phys_typed_broadphase_pair *)(sweep_removed_pairs.begin()), sweep_removed_pairs.getSize());
            phys_collision_filter *filter = world->m_collision_filter;
            static_cast<const phys_typed_broadphase_dispatcher *>(world->m_broadphase_dispatcher)->add_handle_pairs(
                reinterpret_cast<phys_typed_broadphase_pair *>(new_pairs.begin()),
                new_pairs.getSize(),
                static_cast<const phys_collider_pair_filter *>(filter));
        }
    }
    world->unlock_and_flush_ops();
}

// @address 0x646b90 ?add_entity_broadphase@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_op_util::add_entity_broadphase(phys_world *world, phys_entity *entity)
{
    phys_linked_collider *collidable = entity->get_linked_collider();
    const phys_shape *shape = collidable->m_proxy_shape;
    if (shape) {
        phys_aabb aabb;
        phys_local_array<phys_broadphase_handle_pair> new_pairs(world->m_broadphase_query_size);
        collidable->m_proxy_shape->get_handle_aabb(collidable->get_transform(), world->m_collision_input->m_box_tolerance * 0.5f, aabb);
        world->m_broadphase->add_broadphase_object(&collidable->m_broadphase_handle, aabb, new_pairs);
        if (new_pairs.getSize() > 0) {
            phys_collision_filter *filter = world->m_collision_filter;
            static_cast<const phys_typed_broadphase_dispatcher *>(world->m_broadphase_dispatcher)->add_handle_pairs(
                reinterpret_cast<phys_typed_broadphase_pair *>(&new_pairs[0]),
                new_pairs.getSize(),
                static_cast<const phys_collider_pair_filter *>(filter));
        }
    }
}

// @address 0x646d48 ?add_volume_broadphase@phys_world_op_util@@SAXPAVphys_world@@PAVphys_volume@@@Z
void phys_world_op_util::add_volume_broadphase(phys_world *world, phys_volume *phantom)
{
    phys_broadphase_local_array<phys_broadphase_handle_pair> new_pairs(world->m_broadphase_query_size);
    phys_aabb aabb;
    phantom->compute_bounds(aabb);
    world->m_broadphase->add_broadphase_object(&phantom->m_collider.m_broadphase_handle, aabb, new_pairs);
    if (new_pairs.getSize() != 0) {
        phys_collision_filter *filter = world->m_collision_filter;
        static_cast<const phys_typed_broadphase_dispatcher *>(world->m_broadphase_dispatcher)->add_handle_pairs(
                reinterpret_cast<phys_typed_broadphase_pair *>(&new_pairs[0]),
            new_pairs.getSize(),
            static_cast<const phys_collider_pair_filter *>(filter));
    }
}

// @address 0x646ed8 ?remove_entity_from_broadphase@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_op_util::remove_entity_from_broadphase(phys_world *world, phys_entity *entity)
{
    phys_linked_collider *collidable = entity->get_linked_collider();
    if (collidable->get_shape()) {
        world->m_world_simulation->reset_collision_info_for_entities(&entity, 1, world, true);
        phys_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(world->m_broadphase_query_size);
        world->m_broadphase->remove_broadphase_object(&collidable->m_broadphase_handle, sweep_removed_pairs);
        if (sweep_removed_pairs.getSize() > 0)
            ((phys_typed_broadphase_dispatcher const *)world->m_broadphase_dispatcher)
                ->remove_handle_pairs((phys_typed_broadphase_pair *)(&sweep_removed_pairs[0]), sweep_removed_pairs.getSize());
    }
}

// @address 0x647080 ?remove_volume_from_broadphase@phys_world_op_util@@SAXPAVphys_world@@PAVphys_volume@@@Z
void phys_world_op_util::remove_volume_from_broadphase(phys_world *world, phys_volume *phantom)
{
    phys_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(world->m_broadphase_query_size);
    world->m_broadphase->remove_broadphase_object(&phantom->m_collider.m_broadphase_handle, sweep_removed_pairs);
    if (sweep_removed_pairs.getSize() != 0)
        ((phys_typed_broadphase_dispatcher const *)world->m_broadphase_dispatcher)
            ->remove_handle_pairs((phys_typed_broadphase_pair *)(&sweep_removed_pairs[0]), sweep_removed_pairs.getSize());
}

// @address 0x6471e8 ?detach_actions_dynamic_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@AAV?$phys_array@PAVphys_action@@@@@Z
// 0x6471e8
void phys_world_op_util::detach_actions_dynamic_island(phys_world *world, phys_entity *entity, phys_array<phys_action *> &removed_actions)
{
    phys_island *island = entity->get_island();
    phys_inline_array<phys_entity *, 16> action_entities;
    for (int i = 0; i < entity->m_actions.m_size; ++i) {
        phys_action *action = entity->m_actions[i];
        action->get_entities(action_entities);
        phys_bool only_fixed_partners(true);
        for (int j = 0; j < action_entities.getSize(); ++j) {
            if (!action_entities[j]->body_is_fixed() && action_entities[j] != entity) {
                only_fixed_partners = phys_bool(false);
                break;
            }
        }
        if (only_fixed_partners) {
            PHYS_ARRAY_PUSH_BACK(phys_action *, &removed_actions, action);
            action->add_object_reference();
            island->remove_action(action);
            island->m_action_cleanup_needed = 1;
            put_island_on_dirty_list(world, island);
        }
    }
}

// @address 0x646120 ?merge_two_islands@phys_world_op_util@@SAPAVphys_island@@PAVphys_world@@PAV2@1@Z
phys_island *phys_world_op_util::merge_two_islands(phys_world *world, phys_island *island_a, phys_island *island_b)
{
    if (island_a->m_island_entities.getSize() <= island_b->m_island_entities.getSize()) {
        if (island_a->m_island_entities.getSize() < island_b->m_island_entities.getSize() || island_a->m_storage_idx > island_b->m_storage_idx) {
            phys_algorithm::swap(island_a, island_b);
        }
    }
    {
        phys_determinism_util::check_single_threaded(island_a->m_storage_idx);
        phys_determinism_util::check_single_threaded(island_b->m_storage_idx);
        phys_determinism_util::check_single_threaded(island_a->m_island_entities.getSize());
        phys_determinism_util::check_single_threaded(island_b->m_island_entities.getSize());
        { for (int i = 0; i < island_a->m_island_entities.getSize(); i++) {
            phys_determinism_util::check_single_threaded(island_a->m_island_entities[i]->m_entity_uid); } }
        { for (int i = 0; i < island_b->m_island_entities.getSize(); i++) {
            phys_determinism_util::check_single_threaded(island_b->m_island_entities[i]->m_entity_uid); } }
    }

    world->lock_critical_ops();

    phys_bool activate = island_a->m_in_active_islands || island_b->m_in_active_islands;
    phys_bool final_active_state = island_a->is_body_active() || island_b->is_body_active();
    if (activate) {
        if (!island_a->m_in_active_islands) {
            island_a->m_active = 1;
            activate_island_internal(world, island_a);
            island_a->m_hf_sleep_counter = island_b->m_hf_sleep_counter;
            island_a->m_low_freq_sleep_counter = island_b->m_low_freq_sleep_counter;
        } else if (!island_b->m_in_active_islands) {
            island_b->m_active = 1;
            activate_island_internal(world, island_b);
        } else {
            island_a->m_hf_sleep_counter = phys_math::min_of_two(island_a->m_hf_sleep_counter,
                island_b->m_hf_sleep_counter);
            island_a->m_low_freq_sleep_counter = phys_math::min_of_two(island_a->m_low_freq_sleep_counter,
                island_b->m_low_freq_sleep_counter);
        }
    }
    island_a->m_sparse_enabled |= island_b->m_sparse_enabled;

    phys_pair_agent_append_track(*&island_a->m_agent_track, *&island_b->m_agent_track);

    {
        unsigned short insert_idx = (unsigned short)island_a->m_island_entities.getSize();
        island_a->m_island_entities.set_array_size(island_a->m_island_entities.getSize() + island_b->m_island_entities.getSize());
        for (int i = 0; i < island_b->m_island_entities.getSize(); ++i, insert_idx++) {
            island_a->m_island_entities[insert_idx] = island_b->m_island_entities[i];
            island_b->m_island_entities[i]->m_owner_island = island_a;
            island_b->m_island_entities[i]->m_storage_idx = insert_idx;
        }
    }

    {
        int insert_idx = island_a->m_actions.getSize();
        island_a->m_actions.set_array_size(island_a->m_actions.getSize() + island_b->m_actions.getSize());
        for (int i = 0; i < island_b->m_actions.getSize(); ++i) {
            if (island_b->m_actions[i] != 0) {
                island_a->m_actions[insert_idx] = island_b->m_actions[i];
                island_a->m_actions[insert_idx]->m_sim_island = island_a;
                insert_idx++;
            }
        }
        island_a->m_actions.set_array_size(insert_idx);
    }

    {
        for (int e = 0; e < island_b->m_island_entities.getSize(); ++e) {
            phys_entity *entity = island_b->m_island_entities[e];
            phys_constraint_internal *ci = entity->m_master_constraints.m_data;
            for (int c = entity->m_master_constraints.m_size - 1; c >= 0; ci++, c--) {
                phys_constraint *constraint = ci->m_contact_constraint;
                constraint->set_collider_owner(island_a);
            }
        }
        island_a->merge_constraint_info(*island_b);
        island_a->m_constraint_count += island_b->m_constraint_count;
        island_b->m_constraint_count = 0;
    }

    {
        phys_array<phys_island *> *removal_island_array;
        unsigned short remove_at_idx;
        if (island_b->m_in_active_islands) {
            removal_island_array = &world->m_active_islands;
            remove_at_idx = island_b->m_storage_idx;
        } else {
            removal_island_array = &world->m_inactive_islands;
            remove_at_idx = island_b->m_storage_idx;
        }
        if (remove_at_idx < removal_island_array->getSize() - 1) {
            (*removal_island_array)[remove_at_idx] = (*removal_island_array)[removal_island_array->getSize() - 1];
            (*removal_island_array)[remove_at_idx]->m_storage_idx = remove_at_idx;
        }
        removal_island_array->drop_back();
    }

    island_a->m_split_check_requested = island_a->m_split_check_requested || island_b->m_split_check_requested;
    island_a->m_action_cleanup_needed = island_a->m_action_cleanup_needed || island_b->m_action_cleanup_needed;
    island_a->m_active = final_active_state;

    if (island_b->m_dirty_list_idx != 0xffff) {
        put_island_on_dirty_list(world, island_a);
    }

    if (island_b->m_dirty_list_idx != 0xffff) {
        world->m_dirty_islands[island_b->m_dirty_list_idx] = 0;
        island_b->m_dirty_list_idx = 0xffff;
    }
    delete island_b;

    if (island_a->m_in_active_islands) {
        sort_big_island_to_front(world, island_a);
    }

    world->unlock_and_flush_ops();
    return island_a;
}

// @address 0x6473c0 ?merge_islands@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@1@Z
// 0x6473c0
void phys_world_op_util::merge_islands(phys_world *world, phys_entity *listener_entity_a, phys_entity *listener_entity_b)
{
    phys_island *island_a = listener_entity_a->get_island();
    phys_island *island_b = listener_entity_b->get_island();
    if (world->critical_ops_locked()) {
        phys_world_op::merge_islands_op operation;
        operation.m_island_entities[0] = listener_entity_a;
        operation.m_island_entities[1] = listener_entity_b;
        world->queue_world_op(operation);
        return;
    }
    merge_two_islands(world, island_a, island_b);
}

// @address 0x647428 ?add_actions_to_entity_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_entity@@AAV?$phys_array@PAVphys_action@@@@@Z
// 0x647428
void phys_world_op_util::add_actions_to_entity_island(phys_world *world, phys_entity *entity, phys_array<phys_action *> &actions)
{
    phys_island *island = entity->get_island();
    phys_inline_array<phys_entity *, 16> action_entities;
    for (int i = 0; i < actions.getSize(); ++i) {
        phys_action *action = actions[i];
        if (island != world->m_fixed_island) {
            island->add_world_action(action);
            object_remove_ref(action);
            action_entities.m_size = 0;
            action->get_entities(action_entities);
            for (int j = 0; j < action_entities.getSize(); ++j) {
                if (!action_entities[j]->body_is_fixed() && action_entities[j] != entity) {
                    phys_entity *partner = action_entities[j];
                    merge_islands_if_needed(partner, entity);
                }
            }
        } else {
            world->m_fixed_island->add_world_action(action);
            object_remove_ref(action);
        }
    }
}

// 0x648688
// @address 0x648688 ?split_island@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_op_util::split_island(phys_world *world, phys_island *cur_island)
{
    phys_island *cur = (phys_island *)cur_island;
    unsigned char is_body_active = cur->m_in_active_islands_bits;
    phys_array<phys_island *> &island_list = is_body_active ? world->m_active_islands : world->m_inactive_islands;
    cur->m_split_check_requested = 0;
    int array_old_size = island_list.getSize();
    phys_world_op_util::split_island(cur_island, world, island_list, 0);
    char cur_active_bits = cur->m_island_active_bits;
    for (int i = array_old_size; i < island_list.getSize(); i++) {
        phys_island *new_island = island_list[i];
        phys_island *nb = (phys_island *)new_island;
        nb->m_island_active_bits = is_body_active;
        nb->m_in_active_islands_bits = is_body_active;
        if (is_body_active && !cur_active_bits)
            phys_world_op_util::mark_island_inactive(world, new_island);
        nb->m_split_check_frames = (unsigned char)(cur->m_split_check_frames - array_old_size + i);
    }
}

// @address 0x648770 ?split_islands@phys_world_op_util@@SAXPAVphys_world@@@Z
// 0x648770
void phys_world_op_util::split_islands(phys_world *world)
{
    if (!world->m_want_islands) return;
    const phys_array<phys_island *> &world_active_islands = world->m_active_islands;
    int island_count = world_active_islands.getSize();
    for (int i = island_count - 1; i >= 0; --i) {
        phys_island *island = world_active_islands[i];
        if (island->m_split_check_requested)
            phys_world_op_util::split_island(world, island);
    }
}

// 0x648cb0
namespace phys_algorithm {
template <class T, class less_cmp> void quick_sort_recursive(T *values, int left, int right, less_cmp less)
{
    T temporary;
    for (;;) {
        int high = right;
        int low = left;
        T pivot = values[(left + right) >> 1];
        do {
            while (less(values[low], pivot)) ++low;
            while (less(pivot, values[high])) --high;
            if (high >= low) {
                if (high != low) {
                    temporary = values[high];
                    values[high] = values[low];
                    values[low] = temporary;
                }
                --high;
                ++low;
            }
        } while (low <= high);
        if (left < high) quick_sort_recursive(values, left, high, less);
        if (low < right) left = low;
        else break;
    }
}
}
template void phys_algorithm::quick_sort_recursive<phys_island *, phys_bool (*)(const phys_island *, const phys_island *)>(
    phys_island **, int, int, phys_bool (*)(const phys_island *, const phys_island *));
#if !defined(_MSC_VER) || defined(__clang__)
// ConstraintInstance/Entity sorts used by hkContinuousSimulation; the PC link needs these bodies,
// while the Xbox build keeps the fold target's bytes (shared address above).
template void phys_algorithm::quick_sort_recursive<phys_constraint *, phys_bool (*)(const phys_constraint *, const phys_constraint *)>(
    phys_constraint **, int, int, phys_bool (*)(const phys_constraint *, const phys_constraint *));
template void phys_algorithm::quick_sort_recursive<phys_entity *, phys_bool (*)(const phys_entity *, const phys_entity *)>(
    phys_entity **, int, int, phys_bool (*)(const phys_entity *, const phys_entity *));
#endif

// @address 0x648df0 ?get_sector_size@phys_pair_agent_track@@QBAHH@Z
// 0x648df0

// @address 0x648e10 ??4phys_motion_state@@QAAAAV0@ABV0@@Z
// 0x648e10
// match: 0x6455c0: inline keeps replaceMotionObject saving r23-r31; out of line it saves r24-r31 and every argument register moves up by one
#if defined(_XBOX) && !defined(__clang__)
inline phys_motion_state &phys_motion_state::operator=(const phys_motion_state &other)
#else
phys_motion_state &phys_motion_state::operator=(const phys_motion_state &other)
#endif
{
    m_body_transform = other.m_body_transform;
    m_swept_transform = other.m_swept_transform;
    m_delta_angle = other.m_delta_angle;
    m_object_radius = other.m_object_radius;
    m_max_lin_vel = other.m_max_lin_vel;
    m_max_ang_vel = other.m_max_ang_vel;
    m_linear_damping = other.m_linear_damping;
    m_ang_damping = other.m_ang_damping;
    m_deactivation_class = other.m_deactivation_class;
    m_deactivation_counter = other.m_deactivation_counter;
    for (unsigned int i = 0; i < 2; ++i) m_sleep_ref_orientation[i] = other.m_sleep_ref_orientation[i];
    return *this;
}

#define PHYS_WORLD_OP_UTIL_ALLOCATE_STACK(P, T, N) do { (P) = static_cast<T *>(allocate_local_stack<T>(N)); } while (0)
#define PHYS_DEALLOCATE_STACK(P) do { phys_free_stack(P); } while (0)
#define PHYS_WORLD_OP_UTIL_DEALLOCATE_STACK(P) do { phys_free_stack(P); } while (0)

static __forceinline void phys_find_inactive_groups(
    const phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, const phys_world *world,
        int union_group_count, phys_fixed_array<int> &is_body_active)
{
    { for (int i = 0; i < union_group_count; i++) { is_body_active[i] = 0; } }
    const unsigned char *world_select_flags = world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select;
    {
        for (int i = 0; i < entities.getSize(); i++) {
            phys_entity *entity = entities[i];
            const phys_motion *motion = &entity->m_motion;
            phys_determinism_util::check_multi_threaded(phys_world_op_inactive_frames_mt(*motion, 0, world_select_flags[0]));
            phys_determinism_util::check_multi_threaded(phys_world_op_inactive_frames_mt(*motion, 1, world_select_flags[1]));
            int inactive_frames = phys_math::max_of_two(phys_world_op_inactive_frames_mt(*motion, 0, world_select_flags[0]),
                phys_world_op_inactive_frames_mt(*motion, 1, world_select_flags[1]));
            if (inactive_frames <= 5) {
                int group = union_find_checker.m_node_parents[i];
                is_body_active[group] = 1;
            }
        }
    }
}
static __forceinline void phys_identify_deactivated_subgroups(phys_union_find &union_find_checker,
    const phys_array<phys_entity *> &entities, const phys_world *world,
    phys_array<int> &group_size_list, phys_fixed_array<signed char> &group_sparse)
{
    int original_group_cnt = group_size_list.getSize();
    phys_local_buffer<int> is_body_active(original_group_cnt);
    phys_find_inactive_groups(union_find_checker, entities, world, original_group_cnt, is_body_active);
    int output_group_cnt = 1;
    int *reindex_groups = is_body_active.begin();
    group_sparse[0] = true;
    for (int i = 0; i < original_group_cnt; i++) {
        if (is_body_active[i]) {
            reindex_groups[i] = 0;
        } else {
            reindex_groups[i] = output_group_cnt;
            group_sparse[output_group_cnt] = false;
            output_group_cnt++;
        }
    }
    if (output_group_cnt > original_group_cnt) {
        group_sparse[0] = false;
        return;
    }
    union_find_checker.reindex_groups(is_body_active, output_group_cnt, group_size_list);
}
// @address 0x645a98 ?find_sleeping_subgroups@@YAXAAVphys_union_find@@ABV?$phys_array@PAVphys_entity@@@@PBVphys_world@@AAV?$phys_array@H@@AAV?$phys_fixed_array@C@@@Z
void find_sleeping_subgroups(phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, const phys_world *world,
    phys_array<int> &group_size_list, phys_fixed_array<signed char> &group_sparse)
{
    phys_identify_deactivated_subgroups(union_find_checker, entities, world, group_size_list, group_sparse);
}

static __forceinline void phys_find_inactive_groups_and_size_solver(const phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities,
    int union_group_count, phys_fixed_array<int> &is_body_active, phys_fixed_array<int> &sizes, unsigned char *inactive_frames_select_flags)
{
    phys_determinism_util::check_multi_threaded(inactive_frames_select_flags[0]);
    phys_determinism_util::check_multi_threaded(inactive_frames_select_flags[1]);
    for (int i = 0; i < entities.getSize(); i++) {
        phys_entity *entity = entities[i];
        int group = union_find_checker.m_node_parents[i];
        int inactive_frames = phys_math::max_of_two(
            phys_world_op_inactive_frames_mt(entity->m_motion, 0, inactive_frames_select_flags[0]),
            phys_world_op_inactive_frames_mt(entity->m_motion, 1, inactive_frames_select_flags[1]));
        phys_determinism_util::check_multi_threaded(entity->m_entity_uid);
        phys_determinism_util::check_multi_threaded(
            phys_world_op_inactive_frames_mt(entity->m_motion, 0, inactive_frames_select_flags[0]));
        phys_determinism_util::check_multi_threaded(
            phys_world_op_inactive_frames_mt(entity->m_motion, 1, inactive_frames_select_flags[1]));
        if (inactive_frames <= 5) {
            is_body_active[group] = 1;
        }
        int size = phys_world_op_util::estimate_island_size(1, entity->get_constraint_masters().getSize());
        sizes[group] += size;
    }
}
static __forceinline void phys_approx_solver_size(
    const phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, int union_group_count,
        phys_fixed_array<int> &is_body_active, phys_fixed_array<int> &sizes)
{
    {
        for (int i = 0; i < entities.getSize(); i++) {
            phys_entity *entity = entities[i];
            int group = union_find_checker.m_node_parents[i];
            is_body_active[group] = 1;
            // match: 0x647638: retain the estimate in the accumulation expression for operand ranking.
            sizes[group] += phys_world_op_util::estimate_island_size(1, entity->get_constraint_masters().getSize());
        }
    }
}
static __forceinline void phys_merge_small_subgroups(phys_world *world, phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities,
    phys_array<int> &group_size_list, phys_fixed_array<signed char> &group_sparse)
{
    int original_group_cnt = group_size_list.getSize();
    phys_local_buffer<int> sizes(original_group_cnt);
    phys_local_buffer<int> is_body_active(original_group_cnt);
    { for (int i = 0; i < original_group_cnt; i++) { is_body_active[i] = 0; sizes[i] = 0; } }
    if (*(const phys_bool *)&world->m_want_sleep) {
        phys_find_inactive_groups_and_size_solver(union_find_checker,
            entities,
            original_group_cnt,
            is_body_active,
            sizes,
            world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select);
    } else {
        phys_approx_solver_size(union_find_checker, entities, original_group_cnt, is_body_active, sizes);
    }
    {
        for (int i = 0; i < original_group_cnt; i++) {
            if (!is_body_active[i]) {
                sizes[i] = -1;
            }
        }
    }
    int output_group_cnt = 0;
    int *reindex_groups = is_body_active.begin();
    {
        int extra_island_size = phys_world_op_util::estimate_island_size(10, 10);
        for (int i = 0; i < original_group_cnt; i++) {
            int group_size = sizes[i];
            if (group_size == -2) { continue; }
            int output_group_id = output_group_cnt++;
            group_sparse[output_group_id] = false;
            reindex_groups[i] = output_group_id;
            if (group_size < 0) { continue; }
            for (int j = i + 1; j < original_group_cnt; j++) {
                if (!can_island_be_sparse(world, group_size + extra_island_size)) {
                    break;
                }
                {
                    int other_group_size = sizes[j];
                    if (other_group_size < 0) {
                        continue;
                    }
                    if (can_island_be_sparse(world, group_size + other_group_size)) {
                        group_sparse[output_group_id] = true;
                        reindex_groups[j] = output_group_id;
                        group_size += other_group_size;
                        sizes[j] = -2;
                    }
                }
            }
        }
    }
    union_find_checker.reindex_groups(is_body_active, output_group_cnt, group_size_list);
}
// @address 0x645cd8 ?merge_small_subgroups@@YAXPAVphys_world@@AAVphys_union_find@@ABV?$phys_array@PAVphys_entity@@@@AAV?$phys_array@H@@AAV?$phys_fixed_array@C@@@Z
void merge_small_subgroups(phys_world *world, phys_union_find &union_find_checker, const phys_array<phys_entity *> &entities, phys_array<int> &group_sizes,
    phys_fixed_array<signed char> &subgroup_active)
{
    int union_group_count = group_sizes.getSize();
    int *sizes;
    PHYS_WORLD_OP_UTIL_ALLOCATE_STACK(sizes, int, union_group_count);
    {
    phys_local_buffer<int> new_group_idx(union_group_count);
    int i;
    for (i = 0; i < union_group_count; i++) {
        new_group_idx[i] = 0;
        sizes[i] = 0;
    }
    if (world->m_want_sleep) {
        const unsigned char *flags = world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select;
        const phys_array<phys_entity *> *ep = &entities; const phys_array<phys_entity *> &entity_list = *ep;
        for (i = 0; i < entity_list.getSize(); i++) {
            phys_entity *entity = entity_list[i];
            int group = union_find_checker.m_node_parents[i];
            int n1 = phys_get_num_inactive_frames(&entity->m_motion, 1, flags);
            int n0 = phys_get_num_inactive_frames(&entity->m_motion, 0, flags);
            int mx = n1 & 0x7f;
            if ((n0 & 0x7f) > mx) mx = n0 & 0x7f;
            if (mx <= 5) new_group_idx[group] = 1;
            int s = phys_math::max_of_two<int>(1, entity->m_master_constraints.m_size);
            sizes[group] += s;
        }
    } else {
        for (i = 0; i < entities.getSize(); i++) {
            phys_entity *entity = entities[i];
            int group = union_find_checker.m_node_parents[i];
            new_group_idx[group] = 1;
            int s = entity->m_master_constraints.m_size;
            if (s < 1) s = 1;
            sizes[group] += s;
        }
    }
    for (i = 0; i < union_group_count; i++) {
        if (new_group_idx[i] == 0) sizes[i] = -1;
    }
    int new_group_count = 0;
    int *group_idx_ptr = new_group_idx.begin();
    for (int g = 0; g < union_group_count; g++) {
        int size = sizes[g];
        if (size == -2) continue;
        int new_group_slot = new_group_count++;
        subgroup_active[new_group_slot] = 0;
        group_idx_ptr[g] = new_group_slot;
        if (size < 0) continue;
        for (int h = g + 1; h < union_group_count; h++) {
            if (!can_island_be_sparse(world, size + 10)) break;
            int s2 = sizes[h];
            if (s2 >= 0) {
                int merged = s2 + size;
                if (can_island_be_sparse(world, merged)) {
                    size = merged;
                    subgroup_active[new_group_slot] = 1;
                    group_idx_ptr[h] = new_group_slot;
                    sizes[h] = -2;
                }
            }
        }
    }
    union_find_checker.reindex_groups(new_group_idx, new_group_count, group_sizes);
    }
    PHYS_WORLD_OP_UTIL_DEALLOCATE_STACK(sizes);
}

// @address 0x647638 ?split_island@phys_world_op_util@@SAXPAVphys_island@@PAVphys_world@@AAV?$phys_array@PAVphys_island@@@@PAV?$phys_array@PAVphys_entity@@@@@Z
void phys_world_op_util::split_island(
    phys_island *cur_island, phys_world *world, phys_array<phys_island *> &new_islands_out, phys_array<phys_entity *> *old_entities_out)
{
    phys_determinism_util::check_multi_threaded(cur_island->m_island_entities.getSize());
    phys_local_buffer<int> entity_group_info(cur_island->m_island_entities.getSize());
    phys_union_find union_find_checker(entity_group_info, cur_island->m_island_entities.getSize());
    phys_bool island_fully_linked = cur_island->is_fully_connected(union_find_checker);
    if (island_fully_linked) {
        cur_island->m_sparse_enabled = false;
        return;
    }
    phys_inline_array<int, 32> group_size_list;
    union_find_checker.assign_groups(group_size_list);
    phys_local_buffer<signed char> group_sparse(group_size_list.getSize() + 1);
    if (cur_island->m_sparse_enabled) {
        if (*(const phys_bool *)&world->m_want_sleep) {
            phys_identify_deactivated_subgroups(union_find_checker, cur_island->m_island_entities, world, group_size_list, group_sparse);
            if (group_size_list.getSize() == 1)
                return;
        } else {
            return;
        }
    } else {
        if ((unsigned int)world->m_min_island_size != 0)
            phys_merge_small_subgroups(world, union_find_checker, cur_island->m_island_entities, group_size_list, group_sparse);
        else
            phys_string::mem_fill(group_sparse.begin(), 0, group_size_list.getSize() + 1);
    }
    {
        int index = union_find_checker.move_biggest_group_first(group_size_list);
        signed char h = group_sparse[0];
        group_sparse[0] = group_sparse[index];
        group_sparse[index] = h;
    }
    int biggest_group_size = group_size_list[0];
    int total_entity_cnt = cur_island->get_entities().getSize();
    int new_island_count = group_size_list.getSize();
    if (new_island_count == 1)
        return;
    phys_local_buffer<phys_island *> islands(new_island_count);
    {
        cur_island->m_sparse_enabled = group_sparse[0] != 0;
        phys_determinism_util::check_multi_threaded(cur_island->m_sparse_enabled);
        phys_bool active = cur_island->is_body_active();
        islands[0] = cur_island;
        for (int j = 1; j < new_island_count; j++) {
            phys_island *new_island = new phys_island(world);
            new_island->m_active = true;
            new_island->m_in_active_islands = true;
            new_island->m_sparse_enabled = group_sparse[j] != 0;
            phys_determinism_util::check_multi_threaded(group_sparse[j]);
            new_island->m_storage_idx = (unsigned short)new_islands_out.getSize();
            new_islands_out.array_push_back(new_island);
            islands[j] = new_island;
            int entity_capacity = group_size_list[j];
            new_island->m_island_entities.reserve_exactly(entity_capacity);
        }
    }
    {
        phys_entity **old_entities = cur_island->m_island_entities.begin();
        int array_old_size = cur_island->get_entities().getSize();
        if (old_entities_out) {
            cur_island->m_island_entities.swap(*old_entities_out);
            cur_island->m_island_entities.clear();
            cur_island->m_island_entities.reserve_exactly(group_size_list[0]);
        } else {
            cur_island->m_island_entities.clear();
        }
        for (int e = 0; e < array_old_size; e++) {
            phys_entity *entity = old_entities[e];
            entity->m_owner_island = islands[entity_group_info[e]];
            phys_island *new_island = entity->m_owner_island;
            entity->m_storage_idx = (unsigned short)new_island->m_island_entities.getSize();
            new_island->m_island_entities.push_back_unchecked(entity);
            if (cur_island != new_island) {
                phys_constraint_internal *ci = entity->m_master_constraints.begin();
                for (int j = 0; j < entity->m_master_constraints.getSize(); ci++, j++) {
                    phys_constraint_info info;
                    ci->get_constraint_info(info);
                    info.m_max_schema_size = cur_island->m_constraint_info.m_max_schema_size;
                    cur_island->m_constraint_count--;
                    cur_island->sub_constraint_info(ci->m_contact_constraint, info);
                    new_island->m_constraint_count++;
                    ci->m_contact_constraint->set_collider_owner(new_island);
                    new_island->add_constraint_info(ci->m_contact_constraint, info);
                }
            }
        }
    }
    {
        const phys_island *world_fixed_island = world->get_fixed_island();
        phys_action **old_actions = cur_island->m_actions.begin();
        int old_action_cnt = cur_island->m_actions.getSize();
        cur_island->m_actions.clear();
        for (int j = 0; j < old_action_cnt; j++) {
            phys_action *action = old_actions[j];
            if (action != phys_null_action::get_null_action()) {
                phys_island *moving_island = 0;
                {
                    phys_inline_array<phys_entity *, 16> entities;
                    action->get_entities(entities);
                    for (int k = 0; k < entities.getSize(); ++k) {
                        moving_island = entities[k]->get_island();
                        if (moving_island != world_fixed_island)
                            break;
                    }
                }
                action->set_island(moving_island);
                moving_island->m_actions.array_push_back(action);
            }
        }
    }
    int small_group_entity_cnt = total_entity_cnt - biggest_group_size;
    if (small_group_entity_cnt * 8 < total_entity_cnt) {
        phys_pair_agent_track &track = cur_island->m_agent_track;
        for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize(); track_sector_index++) {
            phys_pair_agent_sector *cur_sector = track.m_track_sectors[track_sector_index];
            phys_pair_agent_entry *entry = cur_sector->get_track_begin();
            for (; entry < phys_add_byte_offset(cur_sector->get_track_begin(), track.get_sector_size(track_sector_index));) {
                phys_island *new_island = static_cast<phys_entity *>(entry->m_collider[0]->get_collider_owner())->get_island();
                if (new_island->body_is_fixed()) {
                    new_island = static_cast<phys_entity *>(entry->m_collider[1]->get_collider_owner())->get_island();
                }
                if (new_island == cur_island) {
                    entry = phys_add_byte_offset(entry, entry->m_size);
                } else {
                    unsigned long entry_byte_offset = phys_get_byte_offset(cur_sector, entry);
                    phys_pair_agent_copy_relink_entry(&new_island->m_agent_track, entry);
                    phys_pair_agent_free_entry_internal(track, entry);
                    if (track_sector_index >= track.m_track_sectors.getSize())
                        break;
                    cur_sector = track.m_track_sectors[track_sector_index];
                    entry = phys_add_byte_offset(cur_sector->get_track_begin(), entry_byte_offset);
                }
            }
        }
    } else {
        phys_pair_agent_track track;
        if (cur_island->m_agent_track.m_track_sectors.getSize() == 1) {
            track.m_track_sectors.push_back_unchecked(cur_island->m_agent_track.m_track_sectors[0]);
            cur_island->m_agent_track.m_track_sectors.clear();
        } else if (cur_island->m_agent_track.m_track_sectors.getSize() > 1) {
            track.m_track_sectors.reserve_exactly(2);
            track.m_track_sectors.swap(cur_island->m_agent_track.m_track_sectors);
        }
        phys_algorithm::swap(track.m_last_sector_bytes, cur_island->m_agent_track.m_last_sector_bytes);
        {
            for (int track_sector_index = 0; track_sector_index < track.m_track_sectors.getSize(); track_sector_index++) {
                phys_pair_agent_sector *cur_sector = track.m_track_sectors[track_sector_index];
                phys_pair_agent_entry *entry = cur_sector->get_track_begin();
                phys_pair_agent_entry *end_entry = phys_add_byte_offset(entry, track.get_sector_size(track_sector_index));
                for (; entry < end_entry; entry = phys_add_byte_offset(entry, entry->m_size)) {
                    phys_island *new_island = static_cast<phys_entity *>(entry->m_collider[0]->get_collider_owner())->get_island();
                    if (new_island->body_is_fixed()) {
                        new_island = static_cast<phys_entity *>(entry->m_collider[1]->get_collider_owner())->get_island();
                    }
                    phys_pair_agent_copy_relink_entry(&new_island->m_agent_track, entry);
                }
                phys_world_op_thread_memory().free_chunk(cur_sector, 512, PHYS_MEM_COLLISION_INFO);
            }
            track.m_track_sectors.clear();
        }
    }
    {
        cur_island->m_agent_track.m_track_sectors.optimize_capacity(4);
    }
    for (int i = 1; i < new_island_count; i++)
        islands[i]->m_split_check_frames = cur_island->m_split_check_frames + (unsigned char)i;
}

// @address 0x6455c0 ?replace_motion_object@phys_world_op_util@@CAXPAVphys_rigid_body@@W4motion_kind@phys_motion@@Vphys_bool@@2PAVphys_world@@@Z
void phys_world_op_util::replace_motion_object(
    phys_rigid_body *body, phys_motion::motion_kind new_motion_kind, phys_bool new_state_needs_inertia, phys_bool old_state_needs_inertia, phys_world *world)
{
    if (new_state_needs_inertia) {
        if (!old_state_needs_inertia) {
            phys_keyframed_motion *keyframed_motion = static_cast<phys_keyframed_motion *>(&body->m_motion);
            phys_max_size_motion *saved_motion = keyframed_motion->m_saved_motion;
            keyframed_motion->get_motion_snapshot(saved_motion);
            saved_motion->m_inactive_frame_counts[0] = 0;
            saved_motion->m_inactive_frame_counts[1] = 0;
            body->set_quality_type(static_cast<phys_collider_quality>(keyframed_motion->m_saved_quality_idx));
            phys_mem_copy16(&body->m_motion, saved_motion, sizeof(phys_max_size_motion) >> 4);
            delete saved_motion;
        }
        if (body->m_motion.get_type_id() != new_motion_kind && new_motion_kind != phys_motion::BODY_MOTION_DYNAMIC) {
            phys_max_size_motion_storage old_motion_storage;
            phys_max_size_motion *old_motion = old_motion_storage.init();
            phys_mem_copy16(old_motion, &body->m_motion, sizeof(phys_max_size_motion) >> 4);
            phys_mat3 local_inertia;
#if !defined(_XBOX) || defined(__clang__)
            body->get_rigid_motion()->get_inertia_local(local_inertia);
#else
            body->m_motion.get_inertia_local(local_inertia);
#endif
            phys_motion_state *ms = old_motion->get_motion_state();
            phys_rigid_body::create_dynamic_motion(new_motion_kind, ms->m_body_transform.m_translation,
                ms->m_swept_transform.m_rotation_1, old_motion->get_mass(), local_inertia,
                ms->m_swept_transform.m_center_of_mass_local, ms->m_max_lin_vel, ms->m_max_ang_vel,
                (phys_max_size_motion *)&body->m_motion);
            phys_motion *new_motion = &body->m_motion;
            old_motion->phys_motion::get_motion_snapshot(new_motion);
            new_motion->get_motion_state()->m_linear_damping = old_motion->get_motion_state()->m_linear_damping;
            new_motion->get_motion_state()->m_ang_damping = old_motion->get_motion_state()->m_ang_damping;
            old_motion->phys_keyframed_motion::~phys_keyframed_motion();
        }
    } else {
        phys_max_size_motion_storage old_motion_storage;
        phys_max_size_motion *old_motion = old_motion_storage.init();
        phys_mem_copy16(old_motion, &body->m_motion, sizeof(phys_max_size_motion) >> 4);
        phys_keyframed_motion *new_motion;
        if (new_motion_kind == phys_motion::MOTION_FIXED) {
            new_motion = new (&body->m_motion) phys_fixed_motion(body->get_contact_position(), body->get_rotation());
            phys_motion_state &new_motion_state = *new_motion->get_motion_state();
            new_motion_state = *old_motion->get_motion_state();
            new_motion->m_integrate_sleep_counter = old_motion->m_integrate_sleep_counter;
            phys_swept_transform &old_swept_xform = old_motion->get_motion_state()->m_swept_transform;
            if (old_swept_xform.m_center_of_mass1(3) != 0.0f) {
                float freeze_time = world ? world->m_world_simulation->get_current_time() : old_swept_xform.m_center_of_mass0(3) +
                    1.0f / old_swept_xform.m_center_of_mass1(3);
                phys_swept_transform_util::freeze_motion_state(freeze_time, new_motion_state);
            }
        } else {
            new_motion = new (&body->m_motion) phys_keyframed_motion(body->get_contact_position(), body->get_rotation());
            old_motion->phys_motion::get_motion_snapshot(new_motion);
        }
        if (old_state_needs_inertia) {
            phys_thread_memory *system_thread_memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
            void *saved_old_motion_mem = system_thread_memory->allocate_chunk(sizeof(phys_max_size_motion), PHYS_MEM_MOTION);
            ((phys_motion *)saved_old_motion_mem)->m_mem_size_and_flags = sizeof(phys_max_size_motion);
            phys_max_size_motion *saved_old_motion = saved_old_motion_mem ? new (saved_old_motion_mem) phys_max_size_motion() : 0;
            unsigned short mem_size_flags_backup = saved_old_motion->m_mem_size_and_flags;
            phys_mem_copy16(saved_old_motion, old_motion, sizeof(phys_max_size_motion) >> 4);
            saved_old_motion->m_mem_size_and_flags = mem_size_flags_backup;
            saved_old_motion->m_object_ref_count = 1;
            new_motion->m_saved_motion = saved_old_motion;
            new_motion->m_saved_quality_idx = (unsigned short)body->m_collider.get_quality_type();
        } else {
            phys_keyframed_motion *old_keyframed_motion = old_motion;
            new_motion->set_stored_motion(old_keyframed_motion->m_saved_motion);
            new_motion->m_saved_quality_idx = old_keyframed_motion->m_saved_quality_idx;
        }
        body->m_solver_data = 0;
        body->set_quality_type(static_cast<phys_collider_quality>(new_motion_kind == phys_motion::MOTION_FIXED ? 1 : 2));
        old_motion->phys_keyframed_motion::~phys_keyframed_motion();
    }
    body->m_collider.m_motion = body->get_rigid_motion()->get_motion_state();
    if (world) {
        phys_solver_info &solver_info = world->m_dynamics_step_info.m_solver_info;
        unsigned char *sleep_flags = solver_info.m_inactive_frame_select;
        phys_set_world_select_flags_neg(&body->m_motion, sleep_flags[0], sleep_flags[1], solver_info.m_integrate_sleep_counter);
    }
}

// @address 0x6487d8 ?set_rigid_body_motion_type@phys_world_op_util@@SAXPAVphys_rigid_body@@W4motion_kind@phys_motion@@W4phys_entity_activation@@W4phys_entity_filter_update_mode@@@Z
void phys_world_op_util::set_rigid_body_motion_type(phys_rigid_body *body, phys_motion::motion_kind new_motion_kind,
    phys_entity_activation preferred_activation, phys_entity_filter_update_mode filter_update_mode)
{
    int old_motion_type = body->m_motion.m_type;
    if (old_motion_type == new_motion_kind) return;
    bool new_state_needs_inertia = (new_motion_kind != 7 && new_motion_kind != 6);
    bool old_state_needs_inertia = (old_motion_type != 7 && old_motion_type != 6);
    if (new_state_needs_inertia && !old_state_needs_inertia && !body->m_motion.m_saved_motion) return;
    ((phys_world_object *)body)->add_object_reference();
    phys_world *world = body->m_world;
    if (world && ((old_motion_type == 7) != (new_motion_kind == 7))) {
        phys_inline_array<phys_constraint *, 16> constraints;
        phys_inline_array<phys_action *, 16> actions;
        phys_pair_agent_track removed_agents;
        world->m_block_pending_ops = true;
        world->m_critical_ops_allowed = false;
        detach_constraints(body, constraints);
        if (new_motion_kind != 7)
            detach_actions_fixed_island(world, body, actions);
        else
            detach_actions_dynamic_island(world, body, actions);
        detach_fixed_partner_agents(*&body->m_owner_island->m_agent_track, body, removed_agents, new_motion_kind);
        if (old_motion_type != 7 && body->m_owner_island->m_island_entities.getSize() > 2)
            body->m_owner_island->m_split_check_requested = 1;
        remove_entity_from_island(world, body);
        replace_motion_object(body, new_motion_kind, new_state_needs_inertia, old_state_needs_inertia, world);
        add_entity_island(world, body, preferred_activation);
        add_actions_to_entity_island(world, body, actions);
        world->m_critical_ops_allowed = true;
        for (int i = 0; i < constraints.getSize(); i++) {
            phys_constraint *c = constraints[i];
            world->lock_critical_ops();
            phys_world_constraint_util::add_world_constraint(world, c);
            world->unlock_and_flush_ops();
            object_remove_ref((phys_ref_header *)c);
        }
        if (new_motion_kind != 7) {
            phys_array<phys_linked_collider::collision_entry> *body_coll_entries = &((phys_entity *)body)->get_linked_collider()->m_collision_entries;
                phys_array<phys_linked_collider::collision_entry> &entries = *body_coll_entries;
            int i = 0;
            if (i < entries.getSize()) {
                int box_vertex_byte_offset = 0;
                do {
                    phys_linked_collider::collision_entry *entry = (phys_linked_collider::collision_entry *)((char *)entries.m_data + box_vertex_byte_offset);
                    phys_entity *partner = (phys_entity *)entry->m_linked_partner->get_collider_owner();
                    if (!phys_bool(body->body_is_fixed()) && !phys_bool(partner->body_is_fixed()) && body->get_island() != partner->get_island())
                        merge_islands(body->m_world, body, partner);
                    ++i;
                    box_vertex_byte_offset += sizeof(phys_linked_collider::collision_entry);
                } while (i < entries.getSize());
            }
        }
        phys_pair_agent_append_track(*&body->m_owner_island->m_agent_track, removed_agents);
        if (new_motion_kind == 7) {
            phys_swept_transform_util::freeze_motion_state(world->m_dynamics_step_info.m_step_info.m_step_start_time, body->m_motion.m_motion_state);
            world->lock_critical_ops();
            world->m_world_simulation->reset_collision_info_for_entities(reinterpret_cast<phys_entity **>(&body), 1, world, false);
            phys_simulation::collide_entities_broadphase_discrete(reinterpret_cast<phys_entity **>(&body), 1, world);
            phys_world_callback_util::notify_inactive_entity_moved(body->m_world, body);
            world->m_critical_ops_lock_count--;
        }
        world->refilter_entity(body, filter_update_mode, PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
        // match: 0x6487d8: copying a typed false value lets the pending-operation guard fold away.
        world->m_block_pending_ops = phys_bool(false);
        if (world->m_critical_ops_lock_count == 0 && world->m_pending_op_count)
            world->try_execute_pending_ops();
    } else {
        replace_motion_object(body, new_motion_kind, new_state_needs_inertia, old_state_needs_inertia, world);
        if (world) world->refilter_entity(body, filter_update_mode, PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
    }
    ((phys_world_object *)body)->object_remove_ref();
}

