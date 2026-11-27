// hkinternal/hkAgentNnMachine.obj: from the records 0x69f668 0x69f670 0x69f698 0x69f6c8 0x69f730 0x69f7c0 0x69f858 0x69f8c0 0x69f920 0x69f9f8 0x69fab0 0x69fb80 0x69fc58 0x69fe10 0x6a0018 0x6a0170 0x6a0550 0x6a0688 0x6a07c0 0x6a0830 0x6a0c08 0x6a1100
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x69f668 phys_pair_agent_touch
// @address 0x69f670 ?phys_pair_agent_last_entry@@YAPAUphys_pair_agent_entry@@AAUphys_pair_agent_track@@H@Z
// @address 0x69f698 ?phys_pair_agent_relink_entry@@YAXPAUphys_pair_agent_entry@@@Z
// @address 0x69f6c8 phys_pair_agent_update_compound_filter
// @address 0x69f858 phys_pair_agent_find
// @address 0x69f9f8 ?phys_pair_agent_alloc_entry@@YAPAUphys_pair_agent_entry@@AAUphys_pair_agent_track@@H@Z
// @address 0x69fe10 phys_pair_agent_free_entry_internal
// @address 0x6a0018 phys_pair_agent_copy_relink_entry
// @address 0x6a0688 phys_pair_agent_append_track
// @address 0x6a07c0 ?phys_pair_agent_destroy_track@@YAXAAUphys_pair_agent_track@@PAVphys_dispatcher@@AAVphys_constraint_owner@@@Z
#include "cseries/crt_xdk.h"
#include "phys/phys_base.h"
#include <ppcintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_collide.h"

#include "phys/phys_memory.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_pair_agent_track.h"
#include "phys/phys_narrow_agent.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_constraint_owner.h"


extern "C" {
}
extern "C" {
}
extern "C" {
struct phys_pair_agent_entry;
}
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_pair_agent_sector_array.h"
#include "phys/phys_dispatcher.h"

// TIM storage follows the common 28-byte NN entry, with agent data at +48.
struct phys_pair_agent_sweep_entry : phys_pair_agent_entry {
    float m_separating_normal_time;
    phys_vec4 m_separating_normal;
    char m_agent_data[16];
    const phys_collision_proxy *get_collider_a() { return m_collider[0]; }
    const phys_collision_proxy *get_collider_b() { return m_collider[1]; }
};

#include "phys/phys_pair_agent_sector.h"

enum { PHYS_AGENT_TYPE_BRIDGE = 1 };

struct phys_pair_agent_body_temp
{
    phys_collision_proxy m_body_a;         
    phys_collision_proxy m_body_b;         
    phys_transform m_transform_a; 
    phys_transform m_transform_b; 
};

#include "phys/phys_agent.h"
#include "phys/phys_swept_transform_util_sweep.h"

static inline void mem_copy16(void *destination, const void *source, int count)
{
    int *output = (int *)destination; const int *input = (const int *)source;
    for (int i = 0; i < count; i++) {
        output[0] = input[0]; output[1] = input[1]; output[2] = input[2]; output[3] = input[3];
        output += 4; input += 4;
    }
}

class phys_dispatcher;

namespace {
static __forceinline phys_array<phys_pair_agent_sector *> &sectors(phys_pair_agent_track &track)
{
    return *reinterpret_cast<phys_array<phys_pair_agent_sector *> *>(&track.m_track_sectors.m_data);
}
static __forceinline phys_pair_agent_entry *last_entry_of(phys_pair_agent_track &track, int type)
{
    phys_array<phys_pair_agent_sector *> &array = sectors(track);
    return reinterpret_cast<phys_pair_agent_entry *>(array.m_data[array.m_size - 1]->m_data + track.m_last_sector_bytes - 128);
}
static __forceinline void relink_entry(phys_pair_agent_entry *entry)
{
    unsigned short integrate_body_index_a = entry->m_agent_index_on_collider[0];
    unsigned short integrate_body_index_b = entry->m_agent_index_on_collider[1];
    entry->m_collider[0]->m_collision_entries.m_data[integrate_body_index_a].m_agent_entry = entry;
    entry->m_collider[1]->m_collision_entries.m_data[integrate_body_index_b].m_agent_entry = entry;
}
typedef void (*agent_update_filter_fn)(phys_pair_agent_entry *, void *, phys_linked_collider *, phys_linked_collider *, phys_collision_input *, void *,
    phys_constraint_owner *, phys_dispatcher *);
static __forceinline agent_update_filter_fn update_agent_filter(phys_dispatcher *dispatcher, unsigned char type)
{
    return reinterpret_cast<agent_update_filter_fn>(dispatcher->m_narrow_agent_funcs[type].update_agent_filter);
}
static __forceinline void free_last_entry(phys_pair_agent_track &track, int type)
{
    track.m_last_sector_bytes -= 128;
    if (track.m_last_sector_bytes == 0) {
        phys_pair_agent_sector *sector = sectors(track).back();
        sectors(track).drop_back(1);
        phys_thread_memory *tm = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        tm->free_chunk(sector, 512, (phys_memory_class)37);
        track.m_last_sector_bytes = 512;
    }
    phys_array<phys_pair_agent_sector *> &array = sectors(track);
    if ((array.m_capacity_and_flags & (int)0x80000000) == 0) {
        int requested = array.getSize() + 4;
        if (requested < 1 || requested * 2 < array.get_sector_capacity())
            phys_array_util::shrink_array_storage(&array, (int)sizeof(phys_pair_agent_sector *), (char *)static_cast<phys_inline_array<phys_pair_agent_sector *, 1> &>(array).m_storage, 1);
    }
}
static __forceinline void copy_sectors_backwards(phys_pair_agent_sector **destination, phys_pair_agent_sector *const *source, int count)
{
    for (int i = count - 1; i >= 0; --i)
        destination[i] = source[i];
}
static __forceinline void reserve_sectors(phys_array<phys_pair_agent_sector *> &array, int size)
{
    int capacity = array.get_sector_capacity();
    if (capacity < size) {
        int doubled_cap = capacity * 2;
        int array_new_capacity = size < doubled_cap ? doubled_cap : size;
        phys_array_util::reserve_array_storage(&array, array_new_capacity, (int)sizeof(phys_pair_agent_sector *));
    }
}
}

// 0x69f668
extern "C" void phys_pair_agent_touch(phys_pair_agent_entry *entry, phys_collision_input *input)
{
}

// 0x69f670
phys_pair_agent_entry *phys_pair_agent_last_entry(phys_pair_agent_track &track, int type)
{
    return last_entry_of(track, type);
}

// 0x69f698
void phys_pair_agent_relink_entry(phys_pair_agent_entry *entry)
{
    relink_entry(entry);
}

// 0x69f6c8
extern "C" void phys_pair_agent_update_compound_filter(phys_pair_agent_entry *entry, phys_collision_input *input, phys_constraint_owner *owner)
{
    phys_pair_agent_sweep_entry *header = reinterpret_cast<phys_pair_agent_sweep_entry *>(entry);
    void *agent_data;
    switch (header->m_stream_command) {
    case 2:
    case 6:
        agent_data = &header->m_separating_normal;
        break;
    case 4:
        agent_data = header->m_agent_data;
        break;
    case 0:
        return;
    default:
        return;
    }
    phys_dispatcher *dispatcher = input->m_agent_dispatcher;
    agent_update_filter_fn function = update_agent_filter(dispatcher, header->m_agent_type);
    if (function) {
        function(entry, agent_data, entry->m_collider[0], entry->m_collider[1],
                 input, header->m_contact_manager, owner, dispatcher);
    }
}

// 0x69f858
extern "C" phys_pair_agent_entry *phys_pair_agent_find(phys_linked_collider *a, phys_linked_collider *b)
{
    phys_linked_collider *smaller;
    phys_linked_collider *partner;
    if (a->m_collision_entries.m_size <= b->m_collision_entries.m_size) {
        smaller = a;
        partner = b;
    } else {
        smaller = b;
        partner = a;
    }
    for (int i = 0; i < smaller->m_collision_entries.m_size; ++i) {
        if ((phys_linked_collider *)smaller->m_collision_entries.m_data[i].m_linked_partner == partner)
            return smaller->m_collision_entries.m_data[i].m_agent_entry;
    }
    return 0;
}

// 0x69f9f8
phys_pair_agent_entry *phys_pair_agent_alloc_entry(phys_pair_agent_track &track, int dispatch_agent_type)
{
    unsigned cur = track.m_last_sector_bytes;
    if (cur < 0x200) {
        phys_pair_agent_sector *tail_sector = track.m_track_sectors.m_data[track.m_track_sectors.m_size - 1];
        track.m_last_sector_bytes = cur + 128;
        return reinterpret_cast<phys_pair_agent_entry *>(tail_sector->m_data + cur);
    }
    phys_thread_memory *tm = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    phys_pair_agent_sector *p = (phys_pair_agent_sector *)tm->allocate_chunk(0x200, (phys_memory_class)37);
    phys_array<phys_pair_agent_sector *> *arr = &track.m_track_sectors;
    if (arr->m_size == (arr->m_capacity_and_flags & 0x3fffffff))
        phys_array_util::grow_array_storage(arr, (int)sizeof(phys_pair_agent_sector *));
    arr->m_data[arr->m_size] = p;
    arr->m_size++;
    track.m_last_sector_bytes = 128;
    return (phys_pair_agent_entry *)p;
}


// @address 0x69fab0 phys_pair_agent_pop_last_entry
extern "C" void phys_pair_agent_pop_last_entry(phys_pair_agent_track &track)
{
    phys_array<phys_pair_agent_sector *> *sector_array;
    phys_pair_agent_sector *last_sector;
    int count;
    track.m_last_sector_bytes = track.m_last_sector_bytes - 128;
    if (track.m_last_sector_bytes == 0) {
        sector_array = &track.m_track_sectors;
        last_sector = *reinterpret_cast<phys_pair_agent_sector *volatile *>(&sector_array->m_data[sector_array->m_size - 1]);
        sector_array->m_size = *reinterpret_cast<volatile int *>(&sector_array->m_size) - 1;
        phys_thread_memory *tm = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        tm->free_chunk(last_sector, 0x200, (phys_memory_class)37);
        track.m_last_sector_bytes = 0x200;
    }
    sector_array = &track.m_track_sectors;
    if ((sector_array->m_capacity_and_flags & (int)0x80000000) == 0) {
        count = sector_array->m_size + 4;
        if (count < 1 || (count << 1) < (sector_array->m_capacity_and_flags & 0x3fffffff))
            phys_array_util::shrink_array_storage(sector_array, (int)sizeof(phys_pair_agent_sector *), (char *)static_cast<phys_inline_array<phys_pair_agent_sector *,
                1> *>(sector_array)->m_storage, 1);
    }
    return;
}

// 0x69fe10
extern "C" void phys_pair_agent_free_entry_internal(phys_pair_agent_track &track, phys_pair_agent_entry *entry)
{
    int type = entry->m_agent_type;
    phys_pair_agent_entry *last = last_entry_of(track, type);
    if (entry != last) {
        mem_copy16(entry, last, 8);
        relink_entry(entry);
    }
    free_last_entry(track, type);
}

phys_pair_agent_entry *phys_pair_agent_copy_relink_entry(phys_pair_agent_track *track, phys_pair_agent_entry *source)
{
    phys_pair_agent_entry *entry = phys_pair_agent_alloc_entry(*track, source->m_force_collide_ppu);
    mem_copy16(entry, source, 8);
    phys_linked_collider *pair_collider_a = entry->m_collider[0];
    unsigned int entry_idx_a = entry->m_agent_index_on_collider[0];
    unsigned int entry_idx_b = entry->m_agent_index_on_collider[1];
    pair_collider_a->m_collision_entries.m_data[entry_idx_a].m_agent_entry = entry;
    entry->m_collider[1]->m_collision_entries.m_data[entry_idx_b].m_agent_entry = entry;
    return entry;
}


extern "C" void phys_pair_agent_append_track(phys_pair_agent_track &destination, phys_pair_agent_track &source)
{
    while (destination.m_last_sector_bytes != 512 && sectors(source).getSize() != 0) {
        phys_pair_agent_entry *entry = last_entry_of(source, 0);
        phys_pair_agent_copy_relink_entry(&destination, entry);
        free_last_entry(source, 0);
    }
    if (sectors(source).getSize() != 0) {
        sectors(destination).insert_at(sectors(destination).getSize(), sectors(source).begin(), sectors(source).getSize());
        destination.m_last_sector_bytes = source.m_last_sector_bytes;
        sectors(source).clear();
        source.m_last_sector_bytes = 512;
    }
}

// 0x6a07c0
void phys_pair_agent_destroy_track(phys_pair_agent_track &track, phys_dispatcher *dispatcher, phys_constraint_owner &owner)
{
    while (track.m_track_sectors.m_size != 0) {
        phys_pair_agent_entry *entry = last_entry_of(track, 0);
        phys_pair_agent_destroy(track, entry, dispatcher, owner);
    }
}

// @address 0x6a1100 ?insert_at@?$phys_array@PAUphys_pair_agent_sector@@@@QAAXHPBQAUphys_pair_agent_sector@@H@Z
template <>
void phys_array<phys_pair_agent_sector *>::insert_at(int index, phys_pair_agent_sector *const *entries, int count)
{
    int array_new_size = count + m_size;
    int move_cnt = m_size - index;
    if (array_new_size > get_sector_capacity())
        reserve_sectors(*this, array_new_size);
    copy_sectors_backwards(m_data + index + count, m_data + index, move_cnt);
    copy_sectors_backwards(m_data + index, entries, count);
    m_size = array_new_size;
}

typedef void (*agent_warp_time_fn)(phys_pair_agent_entry *, void *, float, float, phys_collision_input *);
// @address 0x69f7c0 phys_pair_agent_warp_time
extern "C" void phys_pair_agent_warp_time(phys_pair_agent_entry *entry, float agent_old_time, float agent_new_time, phys_collision_input *input)
{
    phys_pair_agent_sweep_entry *base = (phys_pair_agent_sweep_entry *)entry;
    int command = base->m_stream_command;
    switch (command) {
    case 4: {
        char *agent_data = base->m_agent_data;
        if (!(base->m_separating_normal_time != agent_old_time)) {
            base->m_separating_normal_time = agent_new_time;
        } else {
            base->m_separating_normal_time = -1.0f;
        }
        unsigned char agent_type_id = base->m_agent_type;
        agent_warp_time_fn func = reinterpret_cast<agent_warp_time_fn>(input->m_agent_dispatcher.val()->m_narrow_agent_funcs[agent_type_id].warp_time);
        if (func) {
            func(entry, agent_data, agent_old_time, agent_new_time, input);
        }
        return;
    }
    case 2:
    case 6: {
        unsigned char agent_type_id = base->m_agent_type;
        agent_warp_time_fn func = reinterpret_cast<agent_warp_time_fn>(input->m_agent_dispatcher.val()->m_narrow_agent_funcs[agent_type_id].warp_time);
        if (func) {
            func(entry, (char *)&base->m_separating_normal, agent_old_time, agent_new_time, input);
        }
        break;
    }
    default:
        break;
    }
}

// @address 0x69f8c0 phys_pair_agent_assert_track_valid
extern "C" void phys_pair_agent_assert_track_valid(phys_pair_agent_track &track)
{
    for (int track_sector_index = 0; track_sector_index < sectors(track).getSize(); )
    {
        phys_pair_agent_sector *cur_sector = sectors(track)[track_sector_index];
        phys_pair_agent_entry *entry = cur_sector->get_track_begin();
        track_sector_index++;
        phys_pair_agent_entry *end_entry = (track_sector_index == sectors(track).getSize())
            ? reinterpret_cast<phys_pair_agent_entry *>(reinterpret_cast<char *>(entry) + track.m_last_sector_bytes)
            : cur_sector->get_track_end();
        for (; entry < end_entry; entry = reinterpret_cast<phys_pair_agent_entry *>(reinterpret_cast<char *>(entry) + entry->m_size))
        {
        }
    }
}

typedef void (*agent_invalidate_sweep_fn)(phys_pair_agent_entry *, void *, phys_collision_input *);
// @address 0x69f730 phys_pair_agent_invalidate_sweep
extern "C" void phys_pair_agent_invalidate_sweep(phys_pair_agent_entry *entry, phys_collision_input *input)
{
    int command = entry->m_stream_command;
    switch (command) {
    case 4: {
        phys_pair_agent_sweep_entry *sweep_entry = (phys_pair_agent_sweep_entry *)entry;
        sweep_entry->m_separating_normal_time = -1.0f;
        sweep_entry->m_separating_normal.set_zero4();
        unsigned char agent_type_id = entry->m_agent_type;
        agent_invalidate_sweep_fn func =
            reinterpret_cast<agent_invalidate_sweep_fn>(input->m_agent_dispatcher.val()->m_narrow_agent_funcs[agent_type_id].invalidate_sweep_info);
        if (func) {
            func(entry, (char *)entry + 48, input);
        }
        return;
    }
    case 2:
    case 6: {
        unsigned char agent_type_id = entry->m_agent_type;
        agent_invalidate_sweep_fn func =
            reinterpret_cast<agent_invalidate_sweep_fn>(input->m_agent_dispatcher.val()->m_narrow_agent_funcs[agent_type_id].invalidate_sweep_info);
        if (func) {
            func(entry, (char *)entry + 32, input);
        }
        break;
    }
    default:
        break;
    }
}




typedef phys_array<phys_linked_collider::collision_entry> phys_collision_entries;
typedef void *(*narrow_create_fn)(const phys_narrow_agent_input &input, phys_pair_agent_entry *entry, void *agent_data);

// @address 0x69fb80 phys_pair_agent_get_type
extern "C" void phys_pair_agent_get_type(
    phys_collider *collision_proxy_a, phys_collider *collision_proxy_b, const phys_process_collision_input &input, int &agent_type_out, int &out_is_flipped)
{
    int agent_type_id;
    {
        phys_dispatcher *dispatcher = input.m_agent_dispatcher;
        phys_shape_type geom_kind_a = collision_proxy_a->get_shape()->get_type_id();
        phys_shape_type geom_kind_b = collision_proxy_b->get_shape()->get_type_id();
        agent_type_id = dispatcher->get_narrow_agent_type(geom_kind_a, geom_kind_b, input.m_create_pred_agents);
        int flip = (dispatcher->get_narrow_agent_symmetric(agent_type_id) == phys_narrow_agent::NARROW_AGENT_ASYMMETRIC_FLIPPED);
        if (agent_type_id == PHYS_AGENT_TYPE_BRIDGE)
        {
            flip = dispatcher->is_pair_flipped(geom_kind_a, geom_kind_b);
        }
        out_is_flipped = flip;

        if (flip)
        {
            agent_type_id = dispatcher->get_narrow_agent_type(geom_kind_b, geom_kind_a, input.m_create_pred_agents);
        }
    }
    agent_type_out = agent_type_id;
}

// @address 0x6a0550 phys_pair_agent_destroy
extern "C" void phys_pair_agent_destroy(
    phys_pair_agent_track &track, phys_pair_agent_entry *entry_arg, phys_dispatcher *dispatcher, phys_constraint_owner &constraint_owner_ref)
{
    phys_pair_agent_entry *entry = (phys_pair_agent_entry *)entry_arg;
    {
        int command = entry->m_stream_command;
        switch (command)
        {
            case 2:
            case 6:
            {
                void *agent_data = (char *)entry + 32;
                dispatcher->get_narrow_agent_destroy_fn(entry->m_agent_type)(entry_arg, agent_data, entry->m_contact_manager, constraint_owner_ref, dispatcher);
                break;
            }
            case 4:
            {
                void *agent_data = (char *)entry + 48;
                dispatcher->get_narrow_agent_destroy_fn(entry->m_agent_type)(entry_arg, agent_data, entry->m_contact_manager, constraint_owner_ref, dispatcher);
                break;
            }
            case 0:
                break;
            default:
                break;
        }
    }
    {
        for (int i = 0; i < 2; i++)
        {
            phys_linked_collider *collidable = entry->m_collider[i];
            int delete_idx = entry->m_agent_index_on_collider[i];

            collidable->m_collision_entries.remove_at(delete_idx);

            if (delete_idx < collidable->m_collision_entries.getSize())
            {
                phys_pair_agent_entry *partner_entry = (phys_pair_agent_entry *)collidable->m_collision_entries.m_data[delete_idx].m_agent_entry;
                int idx = partner_entry->m_collider[0] == collidable ? 0 : 1;
                partner_entry->m_agent_index_on_collider[idx] = (unsigned short)delete_idx;
            }
            collidable->m_collision_entries.optimize_capacity(4);
        }
    }
    phys_pair_agent_free_entry_internal(track, entry_arg);
}

// @address 0x69fc58 phys_pair_agent_create
extern "C" phys_pair_agent_entry *phys_pair_agent_create(phys_pair_agent_track &track, phys_linked_collider *collider_a, phys_linked_collider *collider_b,
    int agent_type_id, const phys_process_collision_input &input, phys_contact_manager *pair_contact_manager)
{
    phys_dispatcher *dispatcher = input.m_agent_dispatcher;
    phys_pair_agent_entry *entry = phys_pair_agent_alloc_entry(track, 0);
    phys_collision_entries &entries_a = collider_a->m_collision_entries;
    entry->m_collider[0] = (phys_linked_collider *)collider_a;
    entry->m_collider[1] = (phys_linked_collider *)collider_b;
    entry->m_contact_manager = pair_contact_manager;
    entry->m_agent_index_on_collider[0] = (unsigned short)collider_a->m_collision_entries.m_size;
    entry->m_agent_index_on_collider[1] = (unsigned short)collider_b->m_collision_entries.m_size;
    phys_linked_collider::collision_entry &slot_a = entries_a.expand_one();
    phys_collision_entries &entries_b = collider_b->m_collision_entries;
    phys_linked_collider::collision_entry &slot_b = entries_b.expand_one();
    slot_a.m_agent_entry = entry;
    slot_b.m_agent_entry = entry;
    slot_a.m_linked_partner = collider_b;
    slot_b.m_linked_partner = collider_a;
    phys_narrow_agent_input agent_input;
    agent_input.m_body_a = (const phys_collision_proxy *)collider_a;
    agent_input.m_body_b = (const phys_collision_proxy *)collider_b;
    agent_input.m_contact_manager = entry->m_contact_manager;
    agent_input.m_input = &input;
    agent_input.m_a_to_b.set_mul_inverse_mul(*static_cast<const phys_transform *>(collider_a->m_motion),
        *static_cast<const phys_transform *>(collider_b->m_motion));
    unsigned short quality_a = collider_a->m_broadphase_handle.m_quality_type;
    unsigned short quality_b = collider_b->m_broadphase_handle.m_quality_type;
    unsigned char quality = input.m_agent_dispatcher->m_quality_table[quality_a][quality_b];
    entry->m_quality_idx = quality;
    input.m_quality_info = input.m_agent_dispatcher->get_collision_quality_info((signed char)quality);
    entry->m_agent_type = (unsigned char)agent_type_id;
    void *agent_data;
    if (dispatcher->get_narrow_agent_sep_normal_fn(agent_type_id) != 0) {
        static_cast<phys_pair_agent_sweep_entry *>(entry)->m_separating_normal_time = -1.0f;
        entry->m_stream_command = 4;
        (&static_cast<phys_pair_agent_sweep_entry *>(entry)->m_separating_normal)->set_zero4();
        agent_data = (char *)entry + 48;
    } else {
        entry->m_stream_command = 2;
        agent_data = (char *)entry + 32;
    }
    narrow_create_fn create_fn = reinterpret_cast<narrow_create_fn>(dispatcher->m_narrow_agent_funcs[entry->m_agent_type].create);
    create_fn(agent_input, entry, agent_data);
    entry->m_size = 0x80;
    return entry;
}


// @address 0x69f920 ?phys_pair_agent_init_input_at_time@@YAXAAUphys_narrow_agent_input@@AAUphys_pair_agent_body_temp@@0@Z
void phys_pair_agent_init_input_at_time(struct phys_narrow_agent_input &p0, struct phys_pair_agent_body_temp &temp, struct phys_narrow_agent_input &output)
{
    phys_narrow_agent_input &input = p0;

    phys_swept_transform_util::lerp_swept(input.m_body_a->get_motion_state()->get_swept_transform(),
        input.m_input->m_step_info.m_step_start_time, temp.m_transform_a);
    phys_swept_transform_util::lerp_swept(input.m_body_b->get_motion_state()->get_swept_transform(),
        input.m_input->m_step_info.m_step_start_time, temp.m_transform_b);

    output.m_body_a = &temp.m_body_a;
    output.m_body_b = &temp.m_body_b;
    output.m_contact_manager = input.m_contact_manager;
    output.m_input = input.m_input;

    temp.m_body_a.set_proxy_shape(input.m_body_a->get_shape(), input.m_body_a->get_shape_key());
    temp.m_body_b.set_proxy_shape(input.m_body_b->get_shape(), input.m_body_b->get_shape_key());

    new (&temp.m_body_a) phys_collision_proxy(input.m_body_a, &temp.m_transform_a);
    new (&temp.m_body_b) phys_collision_proxy(input.m_body_b, &temp.m_transform_b);

    output.m_a_to_b.set_mul_inverse_mul(temp.m_transform_a, temp.m_transform_b);
}

static __forceinline void init_input_at_time(phys_narrow_agent_input &p0, phys_pair_agent_body_temp &temp, phys_narrow_agent_input &output)
{
    phys_narrow_agent_input &input = p0;

    phys_swept_transform_util::lerp_swept(input.m_body_a->get_motion_state()->get_swept_transform(),
        input.m_input->m_step_info.m_step_start_time, temp.m_transform_a);
    phys_swept_transform_util::lerp_swept(input.m_body_b->get_motion_state()->get_swept_transform(),
        input.m_input->m_step_info.m_step_start_time, temp.m_transform_b);

    output.m_body_a = &temp.m_body_a;
    output.m_body_b = &temp.m_body_b;
    output.m_contact_manager = input.m_contact_manager;
    output.m_input = input.m_input;

    temp.m_body_a.set_proxy_shape(input.m_body_a->get_shape(), input.m_body_a->get_shape_key());
    temp.m_body_b.set_proxy_shape(input.m_body_b->get_shape(), input.m_body_b->get_shape_key());

    new (&temp.m_body_a) phys_collision_proxy(input.m_body_a, &temp.m_transform_a);
    new (&temp.m_body_b) phys_collision_proxy(input.m_body_b, &temp.m_transform_b);

    output.m_a_to_b.set_mul_inverse_mul(temp.m_transform_a, temp.m_transform_b);
}


typedef phys_pair_agent_sweep_entry phys_pair_agent_process_entry;
// @address 0x6a0170 ?phys_pair_agent_process_inline@@YAXPAUphys_pair_agent_entry@@HABUphys_process_collision_input@@AAHAAUphys_process_collision_output@@PAVphys_contact_manager@@@Z
static __forceinline phys_pair_agent_process_entry *get_process_entry(phys_pair_agent_entry *entry) {
    return reinterpret_cast<phys_pair_agent_process_entry *>(entry); }
__forceinline void phys_pair_agent_process_inline(struct phys_pair_agent_entry *entry, int p1, struct phys_process_collision_input const &input,
    int &sweep_entry_count, struct phys_process_collision_output &output, class phys_contact_manager *pair_contact_manager)
{
    int command = get_process_entry(entry)->m_stream_command;
    switch (command) {
    case 6: {
        phys_agent *pair_agent_entry = get_process_entry(entry)->m_stream_agent;
        pair_agent_entry->process_collision(*get_process_entry(entry)->m_collider[0], *get_process_entry(entry)->m_collider[1], input, output);
        break;
    }
    case 2: {
        phys_pair_agent_process_entry *e = get_process_entry(entry);
        phys_narrow_agent_process_input narrow_input;
        narrow_input.m_body_a = e->get_collider_a();
        narrow_input.m_body_b = e->get_collider_b();
        narrow_input.m_contact_manager = e->m_contact_manager;
        narrow_input.m_input = &input;
        const phys_motion_state *motion_a_state = narrow_input.m_body_a->get_motion_state();
        const phys_motion_state *motion_b_state = narrow_input.m_body_b->get_motion_state();
        phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, narrow_input.m_input->m_step_info.m_delta_time,
            narrow_input.m_lin_sweep_info);
        narrow_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());
        narrow_input.m_sector_offset_delta = p1;
        void *agent_data = (char *)e + 32;
        input.m_agent_dispatcher->get_narrow_agent_process_fn(e->m_agent_type)(narrow_input, entry, agent_data, 0, output);
        break;
    }
    case 4: {
        phys_pair_agent_process_entry *e = get_process_entry(entry);
        const phys_motion_state *motion_a_state;
        const phys_motion_state *motion_b_state;
        phys_narrow_agent_process_input narrow_input;
        narrow_input.m_body_a = e->get_collider_a();
        narrow_input.m_body_b = e->get_collider_b();
        narrow_input.m_contact_manager = e->m_contact_manager;
        narrow_input.m_input = &input;
        narrow_input.m_sector_offset_delta = p1;
        motion_a_state = narrow_input.m_body_a->get_motion_state();
        motion_b_state = narrow_input.m_body_b->get_motion_state();
        phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, narrow_input.m_input->m_step_info.m_delta_time,
            narrow_input.m_lin_sweep_info);
        void *agent_data = (char *)e + 48;
        float dist_at_t1;
        do {
            if (!(e->m_separating_normal_time == input.m_step_info.m_step_start_time)) {
                const phys_collision_quality_info &ci = *input.m_quality_info;
                if (!ci.m_use_ccd) {
                    e->m_separating_normal_time = input.m_step_info.m_end_time;
                    dist_at_t1 = -3.40282e+38f;
                    e->m_separating_normal.set_zero4();
                    e->m_separating_normal(3) = dist_at_t1;
                    break;
                }
                phys_pair_agent_body_temp prev_body_temp;
                phys_narrow_agent_input prev_agent_input;
                init_input_at_time(narrow_input, prev_body_temp, prev_agent_input);
                input.m_agent_dispatcher->get_narrow_agent_sep_normal_fn(e->m_agent_type)(prev_agent_input, agent_data, e->m_separating_normal);
            }
            e->m_separating_normal_time = input.m_step_info.m_end_time;
            {
                float worst_approach_delta = narrow_input.m_lin_sweep_info.dot4_xyz1(e->m_separating_normal);
                dist_at_t1 = e->m_separating_normal(3) - worst_approach_delta;
            }
            if (dist_at_t1 >= input.get_tolerance()) {
                e->m_separating_normal(3) = dist_at_t1;
                if (e->m_contact_point_count)
                    input.m_agent_dispatcher->get_narrow_agent_cleanup_fn(e->m_agent_type)(entry, agent_data,
                        narrow_input.m_contact_manager, *output.m_constraint_owner.val());
                sweep_entry_count++;
                return;
            }
        } while (false);
        narrow_input.m_dist_at_t1 = dist_at_t1;
        narrow_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());
        input.m_agent_dispatcher->get_narrow_agent_process_fn(e->m_agent_type)(narrow_input, entry, agent_data, &e->m_separating_normal, output);
        break;
    }
    default:
        break;
    }
}

// @address 0x6a0830 phys_pair_agent_process
extern "C" void phys_pair_agent_process(phys_pair_agent_entry *entry, const phys_process_collision_input &input,
    phys_process_collision_output &output, phys_contact_manager *pair_contact_manager)
{
    int sweep_entry_count = 0;
    phys_pair_agent_process_inline(entry, 0, input, sweep_entry_count, output, pair_contact_manager);
}

template <typename T>
static __forceinline T *phys_pair_agent_add_byte_offset(T *base, unsigned int offset)
{
    return reinterpret_cast<T *>(reinterpret_cast<char *>(base) + offset);
}

// @address 0x6a0c08 phys_pair_agent_process_track
extern "C" void phys_pair_agent_process_track(phys_constraint_owner *constraint_owner_ref, phys_pair_agent_track &pair_agent_track,
    const phys_process_collision_input &input)
{
    int sweep_entry_count = 0;
    __declspec(align(16)) phys_process_collision_output process_output(constraint_owner_ref);
    for (int track_sector_index = 0; track_sector_index < sectors(pair_agent_track).getSize(); )
    {
        phys_pair_agent_sector *cur_sector = sectors(pair_agent_track)[track_sector_index];
        phys_pair_agent_entry *entry = cur_sector->get_track_begin();
        track_sector_index++;
        phys_pair_agent_entry *end_entry = (track_sector_index == sectors(pair_agent_track).getSize()) ?
            phys_pair_agent_add_byte_offset(entry, pair_agent_track.m_last_sector_bytes) : cur_sector->get_track_end();
        for (; entry < end_entry; entry = phys_pair_agent_add_byte_offset(entry, entry->m_size))
        {
            const phys_collider *collider_a = reinterpret_cast<const phys_collider *>(static_cast<const phys_linked_collider *>(get_process_entry(entry)->m_collider[0]));
            const phys_collider *collider_b = reinterpret_cast<const phys_collider *>(static_cast<const phys_linked_collider *>(get_process_entry(entry)->m_collider[1]));
            process_output.reset();
            __dcbt(0, get_process_entry(entry)->m_contact_manager);
            __dcbt(0, phys_pair_agent_add_byte_offset(entry, 128));
            phys_pair_agent_process_inline(entry, 0, input, sweep_entry_count, process_output,
                static_cast<phys_contact_manager *>(get_process_entry(entry)->m_contact_manager));
            if (!havok_get_available_memory())
                phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
            if (phys_memory::get_singleton_instance().m_memory_state == phys_memory::MEMORY_STATE_EXHAUSTED)
                return;
            if (!process_output.is_array_empty())
                static_cast<phys_contact_manager *>(get_process_entry(entry)->m_contact_manager)->process_contact(*collider_a,
                    *collider_b, input, process_output);
        }
    }
}

