// hkdynamics/hkPhantom.obj: from the records 0x657bf0 0x657c68 0x657ce0 0x657d48 0x657db8 0x657e28 0x657e98 0x657f10 0x657f88 0x657ff8 0x658068 0x658120 0x6581b0 0x658278
// @flags /TP /O2
// @address 0x657bf0 ??$compact_null_pointers@Vphys_volume_listener@@@@YAXAAV?$phys_array@PAVphys_volume_listener@@@@@Z
// @address 0x657c68 ??$compact_null_pointers@Vphys_volume_overlap_listener@@@@YAXAAV?$phys_array@PAVphys_volume_overlap_listener@@@@@Z
// @address 0x657f88 ?add_volume_listener@phys_volume@@QAAXPAVphys_volume_listener@@@Z
// @address 0x657ff8 ?add_volume_overlap_listener@phys_volume@@QAAXPAVphys_volume_overlap_listener@@@Z
// @address 0x63b350 ??1phys_world_object@@MAA@XZ
#include "phys/phys_world_op_queue.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_typed_broadphase_dispatcher.h"

#include "phys/phys_volume_listener.h"



#include "phys/phys_broadphase.h"

#include "phys/phys_local_array.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collide.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_volume_overlap_listener.h"
#include "phys/phys_thread_memory.h"
#include <ppcintrinsics.h>
#include <stdlib.h>


template <typename T>
void compact_null_pointers(phys_array<T *> &array)
{
    for (int i = array.m_size - 1; i >= 0; i--) {
        if (array.m_data[i] == 0) {
            array.remove_at_and_copy(i);
        }
    }
}



// @address 0x657e98 ?remove_volume_listener@phys_volume@@QAAXPAVphys_volume_listener@@@Z
void phys_volume::remove_volume_listener(phys_volume_listener *listener)
{
    int index = m_volume_listeners.array_index_of(listener);
    m_volume_listeners.remove_at_and_copy(index);
}

// @address 0x657f10 ?remove_volume_overlap_listener@phys_volume@@QAAXPAVphys_volume_overlap_listener@@@Z
void phys_volume::remove_volume_overlap_listener(phys_volume_overlap_listener *listener)
{
    int index = m_overlap_listeners.array_index_of(listener);
    m_overlap_listeners.remove_at_and_copy(index);
}

void phys_volume::add_volume_listener(phys_volume_listener *listener)
{
    m_volume_listeners.array_push_back(listener);
}

void phys_volume::add_volume_overlap_listener(phys_volume_overlap_listener *listener)
{
    m_overlap_listeners.array_push_back(listener);
}


// @address 0x658068 ?gather_memory_stats@phys_volume@@UBAXPAVphys_statistics_collector@@@Z
void phys_volume::gather_memory_stats(phys_statistics_collector *collector) const
{
    phys_world_object::gather_memory_stats(collector);
    collector->add_array_stats("OvrlapLisPtr", phys_statistics_collector::stat_category_engine, m_overlap_listeners);
    collector->add_array_stats("PhantmLisPtr", phys_statistics_collector::stat_category_engine, m_volume_listeners);
}


// @address 0x6581b0 ?free_internal_arrays@phys_volume@@UAAXXZ



// @address 0x658120 ??1phys_volume@@UAA@XZ
phys_volume::~phys_volume()
{
    notify_volume_deleted();
}

void phys_volume::free_internal_arrays()
{
    if (m_overlap_listeners.getSize() == 0)
        m_overlap_listeners.clear_and_deallocate();
    if (m_volume_listeners.getSize() == 0)
        m_volume_listeners.clear_and_deallocate();
}

// @address 0x5bec98 ?update_compound_filter@phys_volume@@UAAXXZ
void phys_volume::update_compound_filter()
{
}

// @address 0x657ce0 ?notify_volume_deleted@phys_volume@@IAAXXZ
void phys_volume::notify_volume_deleted()
{
    phys_array<phys_volume_listener *> &a = m_volume_listeners;
    for (int i = a.getSize() - 1; i >= 0; i--) {
        if (a[i] != 0) {
            phys_volume_listener *l = a[i];
            l->volume_deleted_callback(this);
        }
    }
}

// @address 0x657db8 ?notify_volume_added@phys_volume@@QAAXXZ
void phys_volume::notify_volume_added()
{
    for (int i = m_volume_listeners.getSize() - 1; i >= 0; --i) {
        if (m_volume_listeners[i] != 0) {
            phys_volume_listener *listener = m_volume_listeners[i];
            listener->volume_added_callback(this);
        }
    }
    compact_null_pointers(m_volume_listeners);
}

// @address 0x657e28 ?notify_volume_shape_set@phys_volume@@QAAXXZ
void phys_volume::notify_volume_shape_set()
{
    for (int i = m_volume_listeners.getSize() - 1; i >= 0; --i) {
        if (m_volume_listeners[i] != 0) {
            phys_volume_listener *listener = m_volume_listeners[i];
            listener->volume_shape_set_callback(this);
        }
    }
    compact_null_pointers(m_volume_listeners);
}

// @address 0x657d48 ?notify_volume_removed@phys_volume@@QAAXXZ
void phys_volume::notify_volume_removed()
{
    for (int i = m_volume_listeners.getSize() - 1; i >= 0; --i) {
        if (m_volume_listeners[i] != 0) {
            phys_volume_listener *listener = m_volume_listeners[i];
            listener->volume_removed_callback(this);
        }
    }
    compact_null_pointers(m_volume_listeners);
}


class phys_broadphase;

namespace phys_world_op {
}





// @address 0x658278 ?update_broadphase@phys_volume@@QAAXABVphys_aabb@@@Z
void phys_volume::update_broadphase(const phys_aabb &aabb)
{
    if (m_world != 0) {
        if (m_world->critical_ops_locked_volumes()) {
            phys_world_op::update_volume_broadphase_op op;
            op.m_overlap_volume = this;
            op.m_volume_aabb = const_cast<phys_aabb *>(&aabb);
            m_world->queue_world_op(op);
            return;
        }

        m_world->lock_critical_ops();

        phys_local_array<phys_broadphase_handle_pair> new_pairs(m_world->m_broadphase_update_size);
        phys_local_array<phys_broadphase_handle_pair> sweep_removed_pairs(m_world->m_broadphase_update_size);

        phys_broadphase_handle *this_obj = get_collider_rw()->get_broadphase_handle();

        m_world->get_broadphase()->lock();

        m_world->get_broadphase()->update_aabbs(&this_obj, &aabb, 1, new_pairs, sweep_removed_pairs);

        if (new_pairs.getSize() != 0 || sweep_removed_pairs.getSize() != 0) {
            phys_typed_broadphase_dispatcher::remove_duplicate_pairs(new_pairs, sweep_removed_pairs);

            m_world->m_broadphase_dispatcher->remove_handle_pairs(
                static_cast<phys_typed_broadphase_pair *>(sweep_removed_pairs.begin()), sweep_removed_pairs.getSize());
            m_world->m_broadphase_dispatcher->add_handle_pairs(static_cast<phys_typed_broadphase_pair *>(new_pairs.begin()), new_pairs.getSize(),
                m_world->get_collision_filter());

            compact_null_pointers(m_overlap_listeners);
        }

        m_world->get_broadphase()->unlock();

        m_world->unlock_and_flush_ops();
    }
}

