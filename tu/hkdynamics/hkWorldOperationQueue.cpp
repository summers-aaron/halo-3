// hkdynamics/hkWorldOperationQueue.obj: from the records 0x654208 0x654228 0x6542b8 0x654318 0x654928 0x654980 0x654a10 0x655460 0x6555a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#define PHYS_RIGID_BODY_OUT_OF_LINE
#include "cseries/crt_xdk.h"
#include "phys/phys_world_op_queue.h"
#include "phys/phys_base_types.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_island.h"
#include "phys/phys_simulation.h"
#include "phys/phys_constraint.h"
#include "phys/phys_action.h"
#include "phys/phys_world_op_util.h"
// @address 0x654208 ?add_reference_to@@YAXPAVphys_ref_object@@@Z
// @address 0x654228 ?remove_reference_from_object@@YAXPAVphys_ref_object@@@Z
// @address 0x6542b8 ?island_less@@YA?AVphys_bool@@ABUbiggest_op@phys_world_op@@0@Z
// @address 0x6555a0 ??$quick_sort_recursive@Ubiggest_op@phys_world_op@@P6A?AVphys_bool@@ABU12@0@Z@phys_algorithm@@YAXPAUbiggest_op@phys_world_op@@HHP6A?AVphys_bool@@ABU12@1@Z@Z
// @address 0x6555a0 ??$quick_sort_recursive@Uweighted_line@phys_hull_builder@@P6A?AVphys_bool@@ABU12@0@Z@phys_algorithm@@YAXPAUweighted_line@phys_hull_builder@@HHP6A?AVphys_bool@@ABU12@1@Z@Z

#include <vectorintrinsics.h>
#include "physics/havok_memory.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_string.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_base.h"

class phys_world;

#include "phys/phys_critical_section.h"
#include "phys/phys_thread_memory_allocator.h"
#include "phys/phys_aabb_types.h"

#include "phys/phys_algorithm_sort.h"
#include "phys/phys_hull_builder.h"
#include "phys/phys_world_op_queue_types.h"
#include "phys/phys_world_op_user_callback.h"
namespace phys_world_op {
struct entity_op : public base_op {
    phys_ref_object *m_object;
};
struct entity_batch_op : public base_op {
    phys_entity **m_island_entities;
    unsigned short m_entity_count;
};
struct volume_batch_op : public base_op {
    phys_volume **m_border_volumes;
    unsigned short m_volume_count;
};
}

namespace phys_world_op {
struct set_motion_kind_op : public base_op {
    phys_rigid_body *m_op_rigid_body;
    phys_enum<phys_motion::motion_kind, unsigned char> m_motion_type;
    phys_enum<phys_entity_activation, unsigned char> m_add_activation;
    phys_enum<phys_entity_filter_update_mode, unsigned char> m_filter_update_mode;
};

}

void add_reference_to(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0)
        ++object->m_object_ref_count;
}

void remove_reference_from_object(phys_ref_object *object)
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

phys_bool island_less(const phys_world_op::biggest_op &a,
                  const phys_world_op::biggest_op &b)
{
    unsigned int island_a = a.m_island_entities[0]->get_island()->m_storage_idx;
    unsigned int island_b = b.m_island_entities[0]->get_island()->m_storage_idx;
    return island_a < island_b || (island_a == island_b &&
        a.m_island_entities[1]->get_island()->m_storage_idx <
        b.m_island_entities[1]->get_island()->m_storage_idx);
}

namespace phys_algorithm {
template void quick_sort_recursive<phys_world_op::biggest_op,
    phys_bool (*)(const phys_world_op::biggest_op &, const phys_world_op::biggest_op &)>(
    phys_world_op::biggest_op *, int, int,
    phys_bool (*)(const phys_world_op::biggest_op &, const phys_world_op::biggest_op &));
#if !defined(_MSC_VER) || defined(__clang__)
// WeightedLine sort used by hkGeomConvexHullBuilder::mergeHulls; the PC link needs this body,
// while the Xbox build keeps the fold target's bytes (shared address above).
template <>
void quick_sort_recursive<phys_hull_builder::weighted_line,
    phys_bool (*)(const phys_hull_builder::weighted_line &, const phys_hull_builder::weighted_line &)>(
    phys_hull_builder::weighted_line *sort_items, int d, int h,
    phys_bool (*less_than_fn)(const phys_hull_builder::weighted_line &, const phys_hull_builder::weighted_line &))
{
    for (;;) {
        int i = h;
        int j = d;
        __declspec(align(16)) phys_hull_builder::weighted_line str;
        str = sort_items[(d + h) >> 1];
        do {
            while (less_than_fn(sort_items[j], str)) { j++; }
            while (less_than_fn(str, sort_items[i])) { i--; }
            if (i >= j) {
                if (i != j) {
                    phys_hull_builder::weighted_line sort_pivot;
                    sort_pivot = sort_items[i];
                    sort_items[i] = sort_items[j];
                    sort_items[j] = sort_pivot;
                }
                i--;
                j++;
            }
        } while (j <= i);
        if (d < i) {
            quick_sort_recursive(sort_items, d, i, less_than_fn);
        }
        if (j < h) {
            d = j;
        } else {
            break;
        }
    }
}
#endif
}

class phys_action;
class phys_constraint;

// @address 0x654928 ??0phys_world_op_queue@@QAA@PAVphys_world@@@Z
phys_world_op_queue::phys_world_op_queue(phys_world *m_world) : m_display_lock(4000)
{
    this->m_world = m_world;
    m_defer_island_merges = false;
}
// @address 0x654980 ??1phys_world_op_queue@@QAA@XZ
phys_world_op_queue::~phys_world_op_queue() {}

// @address 0x654318 ?queue_world_op@phys_world_op_queue@@QAAXABUbase_op@phys_world_op@@@Z
void phys_world_op_queue::queue_world_op(const phys_world_op::base_op &operation)
{
    m_display_lock.enter();
    phys_world_op::biggest_op &queued_op = m_pending_ops.expand_one();
    phys_world_op::base_op &op = queued_op;
    m_world->m_pending_op_count++;

    phys_string::mem_copy4(&queued_op, &operation, sizeof(queued_op) / 4);

    switch (op.m_type) {
    case phys_world_op::OP_ENTITY_ATTACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_ENTITY_DETACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_SET_MOTION_TYPE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_WORLD_OBJECT_SET_SHAPE:
    {
        phys_world_op::set_object_shape_op &pending_entity_op = static_cast<phys_world_op::set_object_shape_op &>(op);
        add_reference_to(pending_entity_op.m_world_object);
        add_reference_to(const_cast<phys_shape *>(pending_entity_op.m_proxy_shape));
        break;
    }
    case phys_world_op::OP_ENTITY_BATCH_ATTACH:
    {
        phys_world_op::entity_batch_op &pending_entity_op = static_cast<phys_world_op::entity_batch_op &>(op);
        pending_entity_op.m_island_entities = phys_alloc_chunk<phys_entity *>(pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
        const phys_world_op::entity_batch_op &pending_entity_batch = static_cast<const phys_world_op::entity_batch_op &>(operation);
        phys_string::mem_copy(pending_entity_op.m_island_entities, pending_entity_batch.m_island_entities,
            sizeof(phys_entity *) * pending_entity_batch.m_entity_count);
        phys_entity **e = pending_entity_op.m_island_entities;
        phys_entity **entity_end = e + pending_entity_op.m_entity_count;
        while (e < entity_end)
            add_reference_to((e++)[0]);
        break;
    }
    case phys_world_op::OP_ENTITY_BATCH_DETACH:
    {
        phys_world_op::entity_batch_op &pending_entity_op = static_cast<phys_world_op::entity_batch_op &>(op);
        pending_entity_op.m_island_entities = phys_alloc_chunk<phys_entity *>(pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
        const phys_world_op::entity_batch_op &pending_entity_batch = static_cast<const phys_world_op::entity_batch_op &>(operation);
        phys_string::mem_copy(pending_entity_op.m_island_entities, pending_entity_batch.m_island_entities,
            sizeof(phys_entity *) * pending_entity_batch.m_entity_count);
        phys_entity **e = pending_entity_op.m_island_entities;
        phys_entity **entity_end = e + pending_entity_op.m_entity_count;
        while (e < entity_end)
            add_reference_to((e++)[0]);
        break;
    }
    case phys_world_op::OP_CONSTRAINT_ATTACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_CONSTRAINT_DETACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_ACTION_ATTACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_ACTION_DETACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_ISLAND_MERGE:
    {
        add_reference_to(queued_op.m_island_entities[0]);
        add_reference_to(queued_op.m_island_entities[1]);
        if (m_defer_island_merges) {
            m_pending_island_merges.array_push_back(queued_op);
            m_pending_ops.drop_back();
        }
        break;
    }
    case phys_world_op::VOLUME_ATTACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::VOLUME_DETACH:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::VOLUME_BATCH_ATTACH:
    {
        phys_world_op::volume_batch_op &pending_entity_op = static_cast<phys_world_op::volume_batch_op &>(op);
        pending_entity_op.m_border_volumes = phys_alloc_chunk<phys_volume *>(pending_entity_op.m_volume_count, PHYS_MEM_DYNAMICS);
        const phys_world_op::volume_batch_op &pending_entity_batch = static_cast<const phys_world_op::volume_batch_op &>(operation);
        phys_string::mem_copy(pending_entity_op.m_border_volumes, pending_entity_batch.m_border_volumes,
            sizeof(phys_volume *) * pending_entity_batch.m_volume_count);
        phys_volume **p = pending_entity_op.m_border_volumes;
        phys_volume **volumes_end = p + pending_entity_op.m_volume_count;
        while (p < volumes_end)
            add_reference_to((p++)[0]);
        break;
    }
    case phys_world_op::VOLUME_BATCH_DETACH:
    {
        phys_world_op::volume_batch_op &pending_entity_op = static_cast<phys_world_op::volume_batch_op &>(op);
        pending_entity_op.m_border_volumes = phys_alloc_chunk<phys_volume *>(pending_entity_op.m_volume_count, PHYS_MEM_DYNAMICS);
        const phys_world_op::volume_batch_op &pending_entity_batch = static_cast<const phys_world_op::volume_batch_op &>(operation);
        phys_string::mem_copy(pending_entity_op.m_border_volumes, pending_entity_batch.m_border_volumes,
            sizeof(phys_volume *) * pending_entity_batch.m_volume_count);
        phys_volume **p = pending_entity_op.m_border_volumes;
        phys_volume **volumes_end = p + pending_entity_op.m_volume_count;
        while (p < volumes_end)
            add_reference_to((p++)[0]);
        break;
    }
    case phys_world_op::OP_ENTITY_REFRESH_BROADPHASE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::VOLUME_UPDATE_BROADPHASE:
    {
        phys_world_op::update_volume_broadphase_op &pending_entity_op = static_cast<phys_world_op::update_volume_broadphase_op &>(op);
        const phys_world_op::update_volume_broadphase_op &pending_entity_batch = static_cast<const phys_world_op::update_volume_broadphase_op &>(operation);
        add_reference_to(pending_entity_op.m_overlap_volume);
        pending_entity_op.m_volume_aabb = new phys_aabb(pending_entity_batch.m_volume_aabb->m_min_limit, pending_entity_batch.m_volume_aabb->m_max_limit);
        break;
    }
    case phys_world_op::OP_REFILTER_ENTITY:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_REFILTER_VOLUME:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_REFILTER_WORLD:
    {
        break;
    }
    case phys_world_op::OP_ENTITY_BATCH_RESTEP:
    {
        phys_world_op::entity_batch_op &pending_entity_op = static_cast<phys_world_op::entity_batch_op &>(op);
        pending_entity_op.m_island_entities = phys_alloc_chunk<phys_entity *>(pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
        const phys_world_op::entity_batch_op &pending_entity_batch = static_cast<const phys_world_op::entity_batch_op &>(operation);
        phys_string::mem_copy(pending_entity_op.m_island_entities, pending_entity_batch.m_island_entities,
            sizeof(phys_entity *) * pending_entity_batch.m_entity_count);
        phys_entity **e = pending_entity_op.m_island_entities;
        phys_entity **entity_end = e + pending_entity_op.m_entity_count;
        while (e < entity_end)
            add_reference_to((e++)[0]);
        break;
    }
    case phys_world_op::OP_REFRESH_MOVED_BODY_INFO:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_SET_POS_AND_ROT:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_SET_LIN_VEL:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_SET_ANG_VEL:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_APPLY_LIN_IMPULSE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_APPLY_POINT_IMPULSE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::RIGID_BODY_APPLY_ANG_IMPULSE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_WORLD_OBJECT_ADD_REF:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        m_pending_ops.drop_back();
        m_world->m_pending_op_count--;
        break;
    }
    case phys_world_op::OP_WORLD_OBJECT_REMOVE_REF:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        remove_reference_from_object(pending_entity_op.m_object);
        m_pending_ops.drop_back();
        m_world->m_pending_op_count--;
        break;
    }
    case phys_world_op::OP_REGION_WAKE:
    {
        break;
    }
    case phys_world_op::OP_ENTITY_WAKE:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_ENTITY_SLEEP:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    case phys_world_op::OP_USER_CALLBACK:
    {
        phys_world_op::entity_op &pending_entity_op = static_cast<phys_world_op::entity_op &>(op);
        add_reference_to(pending_entity_op.m_object);
        break;
    }
    default:
        break;
    }

    m_display_lock.leave();
}

static __forceinline void phys_world_op_queue_copy_back(phys_world_op::biggest_op *dst, const phys_world_op::biggest_op *src, int count)
{
    for (int i = count - 1; i >= 0; i--)
        dst[i] = src[i];
}
// @address 0x655460 ?insert_at@?$phys_array@Ubiggest_op@phys_world_op@@@@QAAXHPBUbiggest_op@phys_world_op@@H@Z
template<> void phys_array<phys_world_op::biggest_op>::insert_at(int index, const phys_world_op::biggest_op *entries, int count)
{
    int array_new_size = m_size + count;
    int move_cnt = m_size - index;
    int capacity = m_capacity_and_flags & 0x3fffffff;
    if (capacity < array_new_size)
    {
        int array_doubled_capacity = capacity * 2;
        phys_array_util::reserve_array_storage(this, array_new_size < array_doubled_capacity ? array_doubled_capacity : array_new_size,
            sizeof(phys_world_op::biggest_op));
    }
    phys_world_op_queue_copy_back(m_data + index + count, m_data + index, move_cnt);
    phys_world_op_queue_copy_back(m_data + index, entries, count);
    m_size = array_new_size;
}



// @address 0x654a10 ?execute_all_pending@phys_world_op_queue@@QAAXXZ
void phys_world_op_queue::execute_all_pending()
{
    {
        if (m_pending_island_merges.getSize()) {
            phys_algorithm::quick_sort(m_pending_island_merges.begin(), m_pending_island_merges.getSize(), island_less);
            m_pending_ops.insert_at(0, m_pending_island_merges.begin(), m_pending_island_merges.getSize());
            m_pending_island_merges.clear();
        }
    }

    phys_array<phys_world_op::biggest_op> stolen_pending_ops;
    stolen_pending_ops.swap(m_pending_ops);
    m_world->m_pending_op_queue_count++;

    for (int i = 0; i < stolen_pending_ops.getSize(); i++) {
        phys_world_op::biggest_op &queued_op = stolen_pending_ops[i];
        phys_world_op::base_op &op = queued_op;

        switch (op.m_type) {
        case phys_world_op::OP_ENTITY_ATTACH:
        {
            phys_world_op::add_entity_op &pending_entity_op = static_cast<phys_world_op::add_entity_op &>(op);
            if (pending_entity_op.m_op_entity->get_action_world() == 0)
                m_world->add_chain_entity(pending_entity_op.m_op_entity, pending_entity_op.m_add_activation);
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::OP_ENTITY_DETACH:
        {
            phys_world_op::remove_entity_op &pending_entity_op = static_cast<phys_world_op::remove_entity_op &>(op);
            if (pending_entity_op.m_op_entity->get_action_world() == m_world)
                m_world->remove_entity(pending_entity_op.m_op_entity);
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::RIGID_BODY_SET_MOTION_TYPE:
        {
            phys_world_op::set_motion_kind_op &pending_entity_op = static_cast<phys_world_op::set_motion_kind_op &>(op);
            pending_entity_op.m_op_rigid_body->set_motion_type(pending_entity_op.m_motion_type, pending_entity_op.m_add_activation,
                pending_entity_op.m_filter_update_mode);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::OP_WORLD_OBJECT_SET_SHAPE:
        {
            phys_world_op::set_object_shape_op &pending_entity_op = static_cast<phys_world_op::set_object_shape_op &>(op);
            pending_entity_op.m_world_object->set_proxy_shape(pending_entity_op.m_proxy_shape);
            remove_reference_from_object(pending_entity_op.m_world_object);
            pending_entity_op.m_proxy_shape->object_remove_ref();
            break;
        }
        case phys_world_op::OP_ENTITY_BATCH_ATTACH:
        {
            phys_world_op::add_entity_batch_op &pending_entity_op = static_cast<phys_world_op::add_entity_batch_op &>(op);
            m_world->add_entity_batch(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count,
                phys_entity_activation(pending_entity_op.m_add_activation));
            phys_entity **e = pending_entity_op.m_island_entities;
            phys_entity **entity_end = e + pending_entity_op.m_entity_count;
            while (e < entity_end)
                remove_reference_from_object((e++)[0]);
            phys_free_chunk<phys_entity *>(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::OP_ENTITY_BATCH_DETACH:
        {
            phys_world_op::remove_entity_batch_op &pending_entity_op = static_cast<phys_world_op::remove_entity_batch_op &>(op);
            m_world->remove_entity_batch(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count);
            phys_entity **e = pending_entity_op.m_island_entities;
            phys_entity **entity_end = e + pending_entity_op.m_entity_count;
            while (e < entity_end)
                remove_reference_from_object((e++)[0]);
            phys_free_chunk<phys_entity *>(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::OP_CONSTRAINT_ATTACH:
        {
            phys_world_op::add_constraint_op &pending_entity_op = static_cast<phys_world_op::add_constraint_op &>(op);
            if (pending_entity_op.m_contact_constraint->get_collider_owner() == 0) {
                if (pending_entity_op.m_contact_constraint->get_entity_a()->get_action_world() == m_world &&
                    pending_entity_op.m_contact_constraint->get_entity_b()->get_action_world() == m_world)
                    m_world->add_world_constraint(pending_entity_op.m_contact_constraint);
            }
            remove_reference_from_object(pending_entity_op.m_contact_constraint);
            break;
        }
        case phys_world_op::OP_CONSTRAINT_DETACH:
        {
            phys_world_op::remove_constraint_op &pending_entity_op = static_cast<phys_world_op::remove_constraint_op &>(op);
            if (pending_entity_op.m_contact_constraint->get_collider_owner() != 0)
                m_world->remove_constraint(pending_entity_op.m_contact_constraint);
            pending_entity_op.m_contact_constraint->object_remove_ref();
            break;
        }
        case phys_world_op::OP_ACTION_ATTACH:
        {
            phys_world_op::add_action_op &pending_entity_op = static_cast<phys_world_op::add_action_op &>(op);
            if (pending_entity_op.m_chain_action->get_action_world() == 0) {
                phys_bool all_actions_in_world = true;
                phys_array<phys_entity *> action_entities;
                pending_entity_op.m_chain_action->get_entities(action_entities);
                for (int k = 0; k < action_entities.getSize(); k++) {
                    if (action_entities[k]->get_action_world() != m_world) {
                        all_actions_in_world = false;
                        break;
                    }
                }
                if (all_actions_in_world)
                    m_world->add_world_action(pending_entity_op.m_chain_action);
            }
            pending_entity_op.m_chain_action->object_remove_ref();
            break;
        }
        case phys_world_op::OP_ACTION_DETACH:
        {
            phys_world_op::remove_action_op &pending_entity_op = static_cast<phys_world_op::remove_action_op &>(op);
            if (pending_entity_op.m_chain_action->get_island() != 0)
                m_world->remove_action(pending_entity_op.m_chain_action);
            pending_entity_op.m_chain_action->object_remove_ref();
            break;
        }
        case phys_world_op::OP_ISLAND_MERGE:
        {
            if (queued_op.m_island_entities[0]->get_action_world() == m_world && queued_op.m_island_entities[1]->get_action_world() == m_world)
                phys_world_op_util::merge_islands_if_needed(queued_op.m_island_entities[0], queued_op.m_island_entities[1]);
            remove_reference_from_object(queued_op.m_island_entities[0]);
            remove_reference_from_object(queued_op.m_island_entities[1]);
            break;
        }
        case phys_world_op::VOLUME_ATTACH:
        {
            phys_world_op::add_volume_op &pending_entity_op = static_cast<phys_world_op::add_volume_op &>(op);
            if (pending_entity_op.m_overlap_volume->get_action_world() == 0)
                m_world->add_volume(pending_entity_op.m_overlap_volume);
            remove_reference_from_object(pending_entity_op.m_overlap_volume);
            break;
        }
        case phys_world_op::VOLUME_DETACH:
        {
            phys_world_op::remove_volume_op &pending_entity_op = static_cast<phys_world_op::remove_volume_op &>(op);
            if (pending_entity_op.m_overlap_volume->get_action_world() == m_world)
                m_world->remove_volume(pending_entity_op.m_overlap_volume);
            remove_reference_from_object(pending_entity_op.m_overlap_volume);
            break;
        }
        case phys_world_op::VOLUME_BATCH_ATTACH:
        {
            phys_world_op::add_volume_batch_op &pending_entity_op = static_cast<phys_world_op::add_volume_batch_op &>(op);
            m_world->add_volume_batch(pending_entity_op.m_border_volumes, pending_entity_op.m_volume_count);
            phys_volume **p = pending_entity_op.m_border_volumes;
            phys_volume **volumes_end = p + pending_entity_op.m_volume_count;
            while (p < volumes_end)
                remove_reference_from_object((p++)[0]);
            phys_free_chunk<phys_volume *>(pending_entity_op.m_border_volumes, pending_entity_op.m_volume_count, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::VOLUME_BATCH_DETACH:
        {
            phys_world_op::remove_volume_batch_op &pending_entity_op = static_cast<phys_world_op::remove_volume_batch_op &>(op);
            m_world->remove_volume_batch(pending_entity_op.m_border_volumes, pending_entity_op.m_volume_count);
            phys_volume **p = pending_entity_op.m_border_volumes;
            phys_volume **volumes_end = p + pending_entity_op.m_volume_count;
            while (p < volumes_end)
                remove_reference_from_object((p++)[0]);
            phys_free_chunk<phys_volume *>(pending_entity_op.m_border_volumes, pending_entity_op.m_volume_count, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::OP_ENTITY_REFRESH_BROADPHASE:
        {
            phys_world_op::update_entity_broadphase_op &pending_entity_op = static_cast<phys_world_op::update_entity_broadphase_op &>(op);
            if (pending_entity_op.m_op_entity->get_action_world() == m_world)
                phys_world_op_util::update_entity_broadphase(m_world, pending_entity_op.m_op_entity);
            pending_entity_op.m_op_entity->object_remove_ref();
            break;
        }
        case phys_world_op::VOLUME_UPDATE_BROADPHASE:
        {
            phys_world_op::update_volume_broadphase_op &pending_entity_op = static_cast<phys_world_op::update_volume_broadphase_op &>(op);
            if (pending_entity_op.m_overlap_volume->get_action_world() == m_world)
                pending_entity_op.m_overlap_volume->update_broadphase(*pending_entity_op.m_volume_aabb);
            remove_reference_from_object(pending_entity_op.m_overlap_volume);
            delete pending_entity_op.m_volume_aabb;
            break;
        }
        case phys_world_op::OP_REFILTER_ENTITY:
        {
            phys_world_op::update_entity_filter_op &pending_entity_op = static_cast<phys_world_op::update_entity_filter_op &>(op);
            if (pending_entity_op.m_op_entity->get_action_world() == m_world)
                m_world->refilter_entity(pending_entity_op.m_op_entity, phys_entity_filter_update_mode(pending_entity_op.m_filter_update_mode),
                    phys_compound_filter_update_mode(pending_entity_op.m_update_compound_shapes));
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::OP_REFILTER_VOLUME:
        {
            phys_world_op::update_volume_filter_op &pending_entity_op = static_cast<phys_world_op::update_volume_filter_op &>(op);
            if (pending_entity_op.m_overlap_volume->get_action_world() == m_world)
                m_world->refilter_volume(pending_entity_op.m_overlap_volume, phys_compound_filter_update_mode(pending_entity_op.m_update_compound_shapes));
            remove_reference_from_object(pending_entity_op.m_overlap_volume);
            break;
        }
        case phys_world_op::OP_REFILTER_WORLD:
        {
            phys_world_op::update_world_filter_op &pending_entity_op = static_cast<phys_world_op::update_world_filter_op &>(op);
            m_world->refilter_world(phys_world_filter_update_mode(pending_entity_op.m_filter_update_mode),
                phys_compound_filter_update_mode(pending_entity_op.m_update_compound_shapes));
            break;
        }
        case phys_world_op::OP_REFRESH_MOVED_BODY_INFO:
        {
            phys_world_op::update_moved_body_op &pending_entity_op = static_cast<phys_world_op::update_moved_body_op &>(op);
            phys_rigid_body::refresh_warped_body(pending_entity_op.m_op_entity);
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::RIGID_BODY_SET_POS_AND_ROT:
        {
            phys_world_op::set_pos_rot_op &pending_entity_op = static_cast<phys_world_op::set_pos_rot_op &>(op);
            pending_entity_op.m_op_rigid_body->set_position_and_rotation(pending_entity_op.m_pos_and_rot[0],
                reinterpret_cast<phys_quat &>(pending_entity_op.m_pos_and_rot[1]));
            phys_free_chunk<phys_vec4>(pending_entity_op.m_pos_and_rot, 2, PHYS_MEM_DYNAMICS);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::RIGID_BODY_SET_LIN_VEL:
        {
            phys_world_op::set_lin_vel_op &pending_entity_op = static_cast<phys_world_op::set_lin_vel_op &>(op);
            phys_vec4 velocity; velocity.set(pending_entity_op.m_linear_velocity[0], pending_entity_op.m_linear_velocity[1],
                pending_entity_op.m_linear_velocity[2]);
            pending_entity_op.m_op_rigid_body->set_linear_velocity(velocity);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::RIGID_BODY_SET_ANG_VEL:
        {
            phys_world_op::set_ang_vel_op &pending_entity_op = static_cast<phys_world_op::set_ang_vel_op &>(op);
            phys_vec4 velocity; velocity.set(pending_entity_op.m_ang_velocity[0], pending_entity_op.m_ang_velocity[1], pending_entity_op.m_ang_velocity[2]);
            pending_entity_op.m_op_rigid_body->set_angular_velocity(velocity);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::RIGID_BODY_APPLY_LIN_IMPULSE:
        {
            phys_world_op::apply_lin_impulse_op &pending_entity_op = static_cast<phys_world_op::apply_lin_impulse_op &>(op);
            phys_vec4 impulse; impulse.set(pending_entity_op.m_linear_impulse[0], pending_entity_op.m_linear_impulse[1], pending_entity_op.m_linear_impulse[2]);
            pending_entity_op.m_op_rigid_body->apply_lin_impulse(impulse);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::RIGID_BODY_APPLY_POINT_IMPULSE:
        {
            phys_world_op::apply_point_impulse_op &pending_entity_op = static_cast<phys_world_op::apply_point_impulse_op &>(op);
            phys_vec4 &point = pending_entity_op.m_point_impulse[0];
            phys_vec4 &impulse = pending_entity_op.m_point_impulse[1];
            pending_entity_op.m_op_rigid_body->apply_point_impulse(impulse, point);
            phys_free_chunk<phys_vec4>(pending_entity_op.m_point_impulse, 2, PHYS_MEM_DYNAMICS);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::RIGID_BODY_APPLY_ANG_IMPULSE:
        {
            phys_world_op::apply_ang_impulse_op &pending_entity_op = static_cast<phys_world_op::apply_ang_impulse_op &>(op);
            phys_vec4 impulse; impulse.set(pending_entity_op.m_ang_impulse[0], pending_entity_op.m_ang_impulse[1], pending_entity_op.m_ang_impulse[2]);
            pending_entity_op.m_op_rigid_body->apply_ang_impulse(impulse);
            remove_reference_from_object(pending_entity_op.m_op_rigid_body);
            break;
        }
        case phys_world_op::OP_REGION_WAKE:
        {
            phys_world_op::activate_region_op &pending_entity_op = static_cast<phys_world_op::activate_region_op &>(op);
            phys_aabb &aabb = *pending_entity_op.m_volume_aabb;
            m_world->activate_region(aabb);
            phys_free_chunk<phys_aabb>(pending_entity_op.m_volume_aabb, 1, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::OP_ENTITY_WAKE:
        {
            phys_world_op::activate_entity_op &pending_entity_op = static_cast<phys_world_op::activate_entity_op &>(op);
            pending_entity_op.m_op_entity->activate();
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::OP_ENTITY_SLEEP:
        {
            phys_world_op::deactivate_entity_op &pending_entity_op = static_cast<phys_world_op::deactivate_entity_op &>(op);
            if (pending_entity_op.m_op_entity->get_action_world() == m_world)
                pending_entity_op.m_op_entity->deactivate();
            remove_reference_from_object(pending_entity_op.m_op_entity);
            break;
        }
        case phys_world_op::OP_ENTITY_BATCH_RESTEP:
        {
            phys_world_op::reintegrate_recollide_batch_op &pending_entity_op = static_cast<phys_world_op::reintegrate_recollide_batch_op &>(op);
            m_world->m_world_simulation->reintegrate_recollide(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count, m_world);
            phys_entity **e = pending_entity_op.m_island_entities;
            phys_entity **entity_end = e + pending_entity_op.m_entity_count;
            while (e < entity_end)
                remove_reference_from_object((e++)[0]);
            phys_free_chunk<phys_entity *>(pending_entity_op.m_island_entities, pending_entity_op.m_entity_count, PHYS_MEM_DYNAMICS);
            break;
        }
        case phys_world_op::OP_USER_CALLBACK:
        {
            phys_world_op::user_callback_op &pending_entity_op = static_cast<phys_world_op::user_callback_op &>(op);
            pending_entity_op.m_callback->world_op_user_callback(pending_entity_op.m_user_data);
            remove_reference_from_object(pending_entity_op.m_callback);
            break;
        }
        default:
            break;
        }
        if (m_pending_ops.getSize())
            execute_all_pending();
    }

    stolen_pending_ops.swap(m_pending_ops);
    m_pending_ops.clear();
    m_world->m_pending_op_queue_count--;
}
