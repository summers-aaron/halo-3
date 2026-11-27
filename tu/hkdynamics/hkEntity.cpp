// hkdynamics/hkEntity.obj: from the records 0x259cd8 0x63b4b0 0x63b4c8 0x63b578 0x63b5a0 0x63b5c8 0x63b5d0 0x63b5d8 0x63b628 0x63b670 0x63b6b8 0x63b708 0x63b798 0x63b8a0 0x63b908 0x63b970 0x63b980 0x63b9b8 0x63b9f0 0x63bb70 0x63bc28 0x63bce0 0x63bd98 0x63bea8 0x63c050 0x63c230 0x63c5a8 0x63c5c0 0x63c5f0 0x63c658 0x63c6b8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x259cd8 ?set_world_select_flags_neg@phys_motion@@QAAXHHH@Z
// @address 0x63b4c8 ?set_deactivator@phys_entity@@IAAXPAVphys_entity_deactivator@@@Z
// @address 0x63b578 ?phys_entity_is_active@@YA?AVphys_bool@@PBVphys_entity@@@Z
// @address 0x63b5a0 ?is_body_active@phys_entity@@QBA?AVphys_bool@@XZ
// @address 0x63b5d0 ?get_constraint_masters_rw_impl@phys_entity@@IAAAAV?$phys_small_array@Uphys_constraint_internal@@@@XZ
// @address 0x63b5d8 ?get_constraint_slaves_impl@phys_entity@@IBAABV?$phys_array@PAVphys_constraint@@@@XZ
// @address 0x63b628 ?remove_activation_listener@phys_entity@@QAAXPAVphys_entity_activation_listener@@@Z
// @address 0x63b670 ?remove_collision_listener@phys_entity@@QAAXPAVphys_collision_listener@@@Z
// @address 0x63b6b8 ?find_contact_manager_to@phys_entity@@QAAPAVphys_dynamics_contact_manager@@PBV1@@Z
// @address 0x63b8a0 ?activate_as_critical_op@phys_entity@@QAAXXZ
// @address 0x63b908 ?deactivate_critical@phys_entity@@QAAXXZ
// @address 0x63b970 ?get_chain_constraint_count@phys_entity@@QBAHXZ
// @address 0x63b980 ?get_entity_constraint@phys_entity@@QAAPAVphys_constraint@@H@Z
// @address 0x63b9b8 ?get_entity_constraint@phys_entity@@QBAPBVphys_constraint@@H@Z
// @address 0x63b9f0 ?gather_memory_stats@phys_entity@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x63c5c0 ?get_entity_vtable@@YAPBXXZ
// @address 0x63b5c8 ?get_constraint_masters_impl@phys_entity@@IBAABV?$phys_small_array@Uphys_constraint_internal@@@@XZ
// @address 0x63b708 ?activate@phys_entity@@QAAXXZ
// @address 0x63bb70 ?add_entity_listener@phys_entity@@QAAXPAVphys_entity_listener@@@Z
// @address 0x63bc28 ?add_entity_activation_listener@phys_entity@@QAAXPAVphys_entity_activation_listener@@@Z
// @address 0x63bce0 ?add_collision_listener@phys_entity@@QAAXPAVphys_collision_listener@@@Z
// @address 0x63c5a8 ?fixup_loaded_entity@@YAXPAX@Z
// @address 0x63c5f0 ??_Gphys_max_size_motion@@UAAPAXI@Z
// @address 0x63c658 ??_Gphys_motion@@UAAPAXI@Z
// @address 0x63c6b8 ??_Gphys_entity@@UAAPAXI@Z
#include "phys/phys_dynamics.h"
#include <string.h>
#include "phys/phys_constraint.h"
#include "phys/phys_entity_callback_util.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_island.h"

class phys_constraint;


#include "phys/phys_world_op_queue.h"


#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_dynamics_contact_manager.h"


#include "phys/phys_world_op_util.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_entity_activation_listener.h"
#include "phys/phys_entity_listener.h"
#include "phys/phys_type_desc.h"

// 0x259cd8 natural source
void phys_motion::set_world_select_flags_neg(int select_flag0, int select_flag1, int world_counter)
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

// 0x63b4c8 natural source
void phys_entity::set_deactivator(phys_entity_deactivator *deactivator)
{
    if (deactivator) deactivator->add_object_reference();
    if (m_body_deactivator) m_body_deactivator->object_remove_ref();
    m_body_deactivator = deactivator;
}

// 0x63b578 natural source
phys_bool phys_entity_is_active(const phys_entity *entity)
{
    if (entity->get_island() == 0)
        return false;
    return entity->get_island()->is_body_active();
}

// 0x63b5a0 natural source
phys_bool phys_entity::is_body_active() const
{
    return phys_entity_is_active(this);
}

const phys_small_array<phys_constraint_internal> &phys_entity::get_constraint_masters_impl() const
{
    return m_master_constraints;
}

// 0x63b5d0 natural source
phys_small_array<phys_constraint_internal> &phys_entity::get_constraint_masters_rw_impl()
{
    return m_master_constraints;
}

// 0x63b5d8 natural source
const phys_array<phys_constraint *> &phys_entity::get_constraint_slaves_impl() const
{
    return m_slave_constraints;
}

// 0x63b628 natural source
void phys_entity::remove_activation_listener(phys_entity_activation_listener *listener)
{
    int i = m_activation_listeners.array_index_of(listener);
    m_activation_listeners[i] = 0;
}

// 0x63b670 natural source
void phys_entity::remove_collision_listener(phys_collision_listener *listener)
{
    int i = m_collision_listeners.array_index_of(listener);
    m_collision_listeners[i] = 0;
}

// 0x63b6b8 natural source
phys_dynamics_contact_manager *phys_entity::find_contact_manager_to(const phys_entity *entity)
{
    int collision_entry_count = m_collider.m_collision_entries.getSize();
    for (int i = 0; i < collision_entry_count; ++i) {
        const phys_collider *partner = m_collider.m_collision_entries[i].m_linked_partner;
        if (partner == &entity->m_collider)
            return static_cast<phys_dynamics_contact_manager *>(m_collider.m_collision_entries[i].m_agent_entry->m_contact_manager);
    }
    return 0;
}

void phys_entity::activate()
{
    if (!phys_entity_is_active(this) && !body_is_fixed() && m_world != 0) {
        m_motion.m_inactive_frame_counts[0] = 0;
        m_motion.m_inactive_frame_counts[1] = 0;
        phys_world_op_util::mark_island_active(m_world, m_owner_island);
        m_motion.set_world_select_flags_neg(m_world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select[0],
            m_world->m_dynamics_step_info.m_solver_info.m_inactive_frame_select[1],
            m_world->m_dynamics_step_info.m_solver_info.m_integrate_sleep_counter);
    }
}


// @address 0x63b798 ?deactivate@phys_entity@@QAAXXZ
void phys_entity::deactivate()
{
    if (phys_entity_is_active(this)) {
        if (m_owner_island->m_sparse_enabled || m_owner_island->m_split_check_requested) {
            int old = m_world->m_min_island_size;
            m_world->m_min_island_size = 0;
            m_owner_island->m_sparse_enabled = false;
            m_owner_island->m_split_check_requested = true;
            phys_world_op_util::split_island(m_world, m_owner_island);
            m_world->m_min_island_size = old;
        }
        phys_island *island = m_owner_island;
        int i = 0;
        if (island->m_island_entities.getSize() > 0) {
            phys_entity **entities = island->m_island_entities.m_data;
            do {
                phys_entity *other = *entities;
                i++;
                entities++;
                other->m_motion.m_sleep_ref_position[0](3) = 3.40282e+38f;
                other->m_motion.m_sleep_ref_position[1](3) = 3.40282e+38f;
            } while (i < island->m_island_entities.getSize());
        }
        phys_world_op_util::mark_island_inactive(m_world, island);
    }
}

// 0x63b8a0 natural source
void phys_entity::activate_as_critical_op()
{
    if (m_world && m_world->critical_ops_locked()) {
        phys_world_op::activate_entity_op operation;
        operation.m_op_entity = this;
        m_world->queue_world_op(operation);
    } else {
        activate();
    }
}

// 0x63b908 natural source
void phys_entity::deactivate_critical()
{
    if (m_world && m_world->critical_ops_locked()) {
        phys_world_op::deactivate_entity_op operation;
        operation.m_op_entity = this;
        m_world->queue_world_op(operation);
    } else {
        deactivate();
    }
}

// 0x63b970 natural source
int phys_entity::get_chain_constraint_count() const
{
    return m_master_constraints.getSize() + m_slave_constraints.getSize();
}

// 0x63b980 natural source
phys_constraint *phys_entity::get_entity_constraint(int index)
{
    if (index < m_master_constraints.getSize())
        return m_master_constraints[index].m_contact_constraint;
    else
        return m_slave_constraints[index - m_master_constraints.getSize()];
}

// 0x63b9b8 natural source
const phys_constraint *phys_entity::get_entity_constraint(int index) const
{
    if (index < m_master_constraints.getSize())
        return m_master_constraints[index].m_contact_constraint;
    else
        return m_slave_constraints[index - m_master_constraints.getSize()];
}

// 0x63b9f0 natural source
void phys_entity::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("Entity", phys_statistics_collector::stat_category_instance, this);
    this->phys_world_object::gather_memory_stats(collector);
    collector->add_child_object("SavedMotion", phys_statistics_collector::stat_category_engine, m_motion.m_saved_motion);
    collector->add_child_object("Deactivator", phys_statistics_collector::stat_category_engine, m_body_deactivator);
    collector->add_small_array_stats("CollisionListnr", phys_statistics_collector::stat_category_engine, m_collision_listeners);
    collector->add_small_array_stats("ActLstnrPtrs", phys_statistics_collector::stat_category_engine, m_activation_listeners);
    collector->add_small_array_stats("ListenerPtrs.", phys_statistics_collector::stat_category_engine, m_entity_listeners);
    collector->end_object();
}

void phys_entity::add_entity_listener(phys_entity_listener *listener)
{
    int empty_slot_idx = m_entity_listeners.array_index_of(0);
    if (empty_slot_idx >= 0) {
        m_entity_listeners[empty_slot_idx] = listener;
    } else {
        m_entity_listeners.array_push_back(listener);
    }
}

void phys_entity::add_entity_activation_listener(phys_entity_activation_listener *listener)
{
    int empty_slot_idx = m_activation_listeners.array_index_of(0);
    if (empty_slot_idx >= 0) {
        m_activation_listeners[empty_slot_idx] = listener;
    } else {
        m_activation_listeners.array_push_back(listener);
    }
}

void phys_entity::add_collision_listener(phys_collision_listener *listener)
{
    int empty_slot_idx = m_collision_listeners.array_index_of(0);
    if (empty_slot_idx >= 0) {
        m_collision_listeners[empty_slot_idx] = listener;
    } else {
        m_collision_listeners.array_push_back(listener);
    }
}


// @address 0x63b4b0 ?destroy_loaded_entity@@YAXPAX@Z
void destroy_loaded_entity(void *p)
{
    static_cast<phys_entity *>(p)->~phys_entity();
}

void fixup_loaded_entity(void *p)
{
    phys_fixup_tag f;
    f.m_fixup_finishing = 1;
    new (p) phys_entity(f);
}

// @address 0x63bd98 ??0phys_entity@@IAA@PBVphys_shape@@@Z
phys_entity::phys_entity(const phys_shape *shape)
    : phys_world_object(shape, BROADPHASE_KIND_ENTITY)
{
    m_storage_idx = (unsigned short)-1;
    m_owner_island = 0;
    m_body_deactivator = 0;
    m_entity_uid = (unsigned int)-1;
    m_solver_data = 0;
}

// @address 0x63bea8 ??0phys_entity@@QAA@Vphys_fixup_tag@@@Z
phys_entity::phys_entity(phys_fixup_tag flag)
    : phys_world_object(flag), m_body_material(flag), m_motion(flag)
{
    if (flag.m_fixup_finishing) {
        void *motion = &m_motion;
        switch (m_motion.get_type_id()) {
        case phys_motion::BODY_MOTION_SPHERE_INERTIA:
            new (motion) phys_sphere_motion(flag);
            break;
        case phys_motion::BODY_MOTION_STABLE_SPHERE_INERTIA:
            new (motion) phys_stabilized_sphere_motion(flag);
            break;
        case phys_motion::BODY_MOTION_BOX_INERTIA:
            new (motion) phys_box_motion(flag);
            break;
        case phys_motion::BODY_MOTION_STABLE_BOX_INERTIA:
            new (motion) phys_stabilized_box_motion(flag);
            break;
        case phys_motion::MOTION_KEYFRAMED:
            new (motion) phys_keyframed_motion(flag);
            break;
        case phys_motion::MOTION_FIXED:
            new (motion) phys_fixed_motion(flag);
            break;
        case phys_motion::BODY_MOTION_THIN_BOX_INERTIA:
            new (motion) phys_thin_box_motion(flag);
            break;
        default:
            break;
        }
    }
}

// @address 0x63c050 ??1phys_entity@@UAA@XZ
phys_entity::~phys_entity()
{
    phys_entity_callback_util::notify_entity_deleted(this);
    if (m_body_deactivator) {
        m_body_deactivator->object_remove_ref();
    }
}

// @address 0x63c230 ?free_internal_arrays@phys_entity@@UAAXXZ
void phys_entity::free_internal_arrays()
{
    phys_array<phys_linked_collider::collision_entry> &collision_entries = m_collider.m_collision_entries;
    if (collision_entries.getSize() == 0) {
        collision_entries.clear_and_deallocate();
    }
    if (m_master_constraints.getSize() == 0) {
        m_master_constraints.clear_and_deallocate();
    }
    if (m_slave_constraints.getSize() == 0) {
        m_slave_constraints.clear_and_deallocate();
    }
    if (m_constraint_runtime.getSize() == 0) {
        m_constraint_runtime.clear_and_deallocate();
    }
    {
        phys_bool has_collision_listeners = false;
        for (int i = 0; i < m_collision_listeners.getSize(); i++) {
            if (m_collision_listeners[i] != 0) {
                has_collision_listeners = true;
                break;
            }
        }
        if (!has_collision_listeners) {
            m_collision_listeners.clear_and_deallocate();
        }
    }
    {
        phys_bool has_activation_listeners = false;
        for (int i = 0; i < m_activation_listeners.getSize(); i++) {
            if (m_activation_listeners[i] != 0) {
                has_activation_listeners = true;
                break;
            }
        }
        if (!has_activation_listeners) {
            m_activation_listeners.clear_and_deallocate();
        }
    }
    {
        phys_bool has_entity_listeners = false;
        for (int i = 0; i < m_entity_listeners.getSize(); i++) {
            if (m_entity_listeners[i] != 0) {
                has_entity_listeners = true;
                break;
            }
        }
        if (!has_entity_listeners) {
            m_entity_listeners.clear_and_deallocate();
        }
    }
    if (m_actions.getSize() == 0) {
        m_actions.clear_and_deallocate();
    }
}

// 0x63c5c0 natural source
const void *get_entity_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_entity)];
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (storage) phys_entity(flag);
    const void *result;
    memcpy(&result, storage, sizeof(result));
    return result;
}


#if !defined(_MSC_VER) || defined(__clang__)
// hkMaxSizeMotion vtable owned by hkEntity.obj; the PC link needs this definition, while the Xbox
// build takes the bytes from the image splice (address tag in hkRigidBody.cpp). The words
// below are the shipped .rdata bytes (code addresses) for the link only.
extern "C" {
const void *phys_max_size_motion_vtable[28] = {
    (const void *)0x8263c5f0, (const void *)0x825bec98, (const void *)0x8264fce8, (const void *)0x8264fcf0,
    (const void *)0x8264ff08, (const void *)0x8264ff30, (const void *)0x8264fcf8, (const void *)0x8264fd00,
    (const void *)0x8264ff58, (const void *)0x8264ff80, (const void *)0x8264c098, (const void *)0x8264c0a8,
    (const void *)0x8264c0b8, (const void *)0x8264c0c8, (const void *)0x8264c0e0, (const void *)0x8264c140,
    (const void *)0x8264c150, (const void *)0x8264fd08, (const void *)0x8264fd10, (const void *)0x8264fd18,
    (const void *)0x8264fd28, (const void *)0x8264fd20, (const void *)0x8264fd30, (const void *)0x8264c260,
    (const void *)0x8264fd38, (const void *)0x8264fd40, 0, (const void *)0x8205b598
};
}
#endif

const phys_type_desc phys_entity_type_desc = phys_type_desc("hkEntity", fixup_loaded_entity,
    destroy_loaded_entity, get_entity_vtable());

