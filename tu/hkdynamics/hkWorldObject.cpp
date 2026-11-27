// hkdynamics/hkWorldObject.obj: from the records 0x63ac38 0x63ac40 0x63ac60 0x63acf0 0x63acf8 0x63ad00 0x63ad08 0x63ad98 0x63ae48 0x63aea0 0x63af10 0x63af88 0x63b060 0x63b138 0x63b198 0x63b270 0x63b350
// @flags /TP /O2 /J /Oc
// @address 0x63ac38 ?set_proxy_shape@phys_world_object@@UAA?AW4Result@phys_world_op@@PBVphys_shape@@@Z
// @address 0x63ac40 ?add_object_reference@phys_world_object@@QAAXXZ
// @address 0x63ac60 ?object_remove_ref@phys_world_object@@QAAXXZ
// @address 0x63acf0 ?mark_for_write_impl@phys_world_object@@QAAXXZ
// @address 0x63acf8 ?check_read_write@phys_world_object@@QAAXXZ
// @address 0x63ad00 ?check_read_only@phys_world_object@@QBAXXZ
// @address 0x63ad08 ?add_property@phys_world_object@@QAAXIUphys_property_value@@@Z
// @address 0x63ae48 ?edit_property@phys_world_object@@QAA?AUphys_property_value@@IU2@@Z
// @address 0x63aea0 ?unlock_property@phys_world_object@@QAAXI@Z
// @address 0x63af10 ?add_reference_critical@phys_world_object@@QAAXXZ
// @address 0x63af88 ?remove_reference_as_critical_op@phys_world_object@@QAAXXZ
// @address 0x63b060 ?gather_memory_stats@phys_world_object@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x63b270 ?lock_property@phys_world_object@@QAAXI@Z
// @address 0x63b350 ??1phys_world_object@@MAA@XZ
#include "phys/phys_dynamics.h"
#include "phys/phys_statistics_collector.h"

#include "phys/phys_world_op_queue.h"

phys_world_op::Result phys_world_object::set_proxy_shape(const phys_shape *shape)
{
    return phys_world_op::STEP_RESULT_DONE;
}

void phys_world_object::add_object_reference(void)
{
    if (m_mem_size_and_flags != 0) {
        m_object_ref_count = (unsigned short)(m_object_ref_count + 1);
    }
}

void phys_world_object::object_remove_ref(void)
{
    phys_ref_object::object_remove_ref();
}

void phys_world_object::mark_for_write_impl(void)
{
}

void phys_world_object::check_read_write(void)
{
}

void phys_world_object::check_read_only(void) const
{
}

void phys_world_object::add_property(unsigned int property_id, phys_property_value value)
{
    int i;
    int count = m_properties.m_size;
    i = 0;
    if (count > 0) {
        phys_property *entry = (phys_property *)m_properties.m_data;
        do {
            if (entry->m_key == property_id)
                return;
            i = i + 1;
            entry = entry + 1;
        } while (i < count);
    }
    phys_array<phys_property> *array = &m_properties;
    if (array->m_size == array->get_sector_capacity())
        phys_array_util::grow_array_storage(array, sizeof(phys_property));
    int index = array->m_size;
    phys_property *entry = array->m_data + index;
    array->m_size = index + 1;
    entry->m_value = value;
    entry->m_key = property_id;
}

// 0x63ae48
phys_property_value phys_world_object::edit_property(unsigned int property_id, phys_property_value value)
{
    for (int property_index = 0; property_index < m_properties.m_size; ++property_index) {
        if (((phys_property *)m_properties.m_data)[property_index].m_key == property_id) {
            phys_property_value old_value = ((phys_property *)m_properties.m_data)[property_index].m_value;
            ((phys_property *)m_properties.m_data)[property_index].m_value = value;
            return old_value;
        }
    }
    phys_property_value empty_property;
    empty_property.m_data = 0;
    return empty_property;
}

// @address 0x63ad98 ?remove_object_property@phys_world_object@@QAA?AUphys_property_value@@I@Z
phys_property_value phys_world_object::remove_object_property(unsigned int key)
{
    for (int i = 0; i < m_properties.getSize(); ++i) {
        if (m_properties[i].m_key == key) {
            phys_property found = m_properties[i];
            m_properties.remove_at_and_copy(i);
            return found.m_value;
        }
    }
    phys_property_value return_value;
    return_value.m_data = 0;
    return return_value;
}

void phys_world_object::unlock_property(unsigned int property_id)
{
    phys_world *world = m_world;
    if (world == 0)
        return;
    phys_critical_section *lock = world->m_property_master_lock;
    if (lock == 0)
        return;
    int i;
    int count = world->m_property_locks.m_size;
    i = 0;
    if (count > 0) {
        phys_world::property_lock *entry = (phys_world::property_lock *)world->m_property_locks.m_data;
        do {
            if (entry->m_key == property_id) {
                lock->leave();
                return;
            }
            i = i + 1;
            entry = entry + 1;
        } while (i < count);
    }
    lock->leave();
}

void phys_world_object::add_reference_critical()
{
    if (get_action_world() != 0 && get_action_world()->critical_ops_locked()) {
        phys_world_op::add_reference_op operation;
        operation.m_world_object = this;
        get_action_world()->queue_world_op(operation);
        return;
    }
    if (m_mem_size_and_flags != 0) {
        m_object_ref_count = m_object_ref_count + 1;
    }
}

// 0x63af88
void phys_world_object::remove_reference_as_critical_op()
{
    if (get_action_world() != 0 && m_world->m_critical_ops_lock_count != 0) {
        phys_world_op::remove_reference_op operation;
        operation.m_world_object = this;
        m_world->queue_world_op(operation);
    } else {
        phys_ref_object::object_remove_ref();
    }
}

void phys_world_object::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->add_child_object("Shape", phys_statistics_collector::stat_category_shared, m_collider.get_shape());
    collector->add_array_stats("CollAgtPtr", phys_statistics_collector::stat_category_runtime, m_collider.m_collision_entries);
    collector->add_array_stats("Properties", phys_statistics_collector::stat_category_engine, m_properties);
}

void phys_world_object::lock_property(unsigned int property_id)
{
    phys_world *world = m_world;
    if (world == 0)
        return;
    phys_critical_section *lock = world->m_property_master_lock;
    if (lock == 0)
        return;
    lock->enter();
    phys_world::property_lock *found = 0;
    int count = m_world->m_property_locks.m_size;
    if (count > 0) {
        phys_world::property_lock *entry = (phys_world::property_lock *)m_world->m_property_locks.m_data;
        do {
            if (entry->m_key == property_id)
                found = entry;
            entry = entry + 1;
        } while ((unsigned int)--count != 0);
    }
    if (found != 0)
        return;
    phys_array<phys_world::property_lock> *array = &m_world->m_property_locks;
    int index = array->m_size;
    int array_new_size = index + 1;
    int capacity = array->get_sector_capacity();
    if (capacity < array_new_size)
        phys_array_util::reserve_array_storage(array, array_new_size < capacity * 2 ? capacity * 2 : array_new_size, sizeof(phys_world::property_lock));
    array->m_size = array_new_size;
    phys_world::property_lock *entry = array->m_data + index;
    entry->m_mt_lock.init();
    entry->m_key = property_id;
}

phys_world_object::~phys_world_object()
{
    if (m_collider.get_shape() != 0)
        m_collider.m_proxy_shape->object_remove_ref();
}

// @address 0x63b138 ??0phys_world_object@@QAA@Vphys_fixup_tag@@@Z
phys_world_object::phys_world_object(phys_fixup_tag flag)
    : m_collider(flag),
      m_properties(flag)
{
    if (flag.m_fixup_finishing != 0)
        m_collider.set_collider_owner(this);
}

// @address 0x63b198 ??0phys_world_object@@IAA@PBVphys_shape@@W4broadphase_kind@0@@Z
phys_world_object::phys_world_object(const phys_shape *shape, broadphase_kind type)
    : m_world(0),
      m_user_data(0),
      m_collider(shape, 0, type),
      m_name(0)
{
    m_collider.set_collider_owner(this);
    if (shape != 0)
        shape->add_object_reference();
}

