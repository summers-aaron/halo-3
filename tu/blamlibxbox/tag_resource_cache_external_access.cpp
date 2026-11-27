// blamlibxbox/tag_resource_cache_external_access.obj: from the records 0x85070 0x850d0 0x855f0 0x87dd8 0x87f58 0x88058 0x140aa8 0x140c38 0x140d00 0x1afc20 0x21c7f0 0x21c970 0x21cb40 0x21cbb8 0x21cc40 0x21cca0 0x21cd60 0x21cd70 0x21cd80 0x21ce40 0x21cf58 0x21d0c8 0x21d3f8 0x21d5e0 0x21d6c0 0x21d818 0x21d948 0x21d9b0 0x21da90 0x21db60 0x21dbd8 0x21dce0 0x21dd50 0x21ddc0 0x21deb0 0x21dfe0 0x21e0f0 0x21e1f8 0x21e378 0x21e498 0x21e510 0x21e608 0x21e648 0x21e910 0x21ea38 0x21eb10 0x21ebf0 0x21ed10 0x21f6e0 0x2aee30
// @flags /TP /O2 /Oc /noGL
#include "cache/tag_resource_cache_address_cache.h"
#include "memory/data_types.h"
#include "cache/tag_resource_external_access_hash_table.h"
#include "cache/tag_resource_external_access_state.h"
#include "tag_files/tag_resource_cache_controller_demand_collector.h"
#include "memory/basic_buffer.h"
#include "memory/allocation_description.h"
#include "memory/wrapped_array_allocation_description.h"
#include "cache/tag_resource_cache_file_datum_handler.h"
#include "memory/static_arrays.h"
#include "memory/data.h"
#include "cache/tag_resource_page_datum.h"
#include "multithreading/read_write_lock.h"
#include "memory/lruv_cache_types.h"
#include "cache/tag_resource_cache_loaded_query.h"
#include "saved_games/game_state_procs_types.h"
// @address 0x1018a0 ?datum_new_at_index@@YAJPAUs_data_array@@J@Z
// @address 0x85708 ?release_unlocked_access_cache@c_tag_resource_cache_thread_lock_lock_freeish@@QAAXPAVc_tag_resource_cache_file_access_cache@@@Z
// @address 0x2aa950 ?dequeue_access_cache@c_tag_resource_cache_thread_lock_lock_freeish@@SAPAVc_synchronized_access_cache@1@PAV?$t_event_queue@Us_access_cache_available_event@c_tag_resource_cache_thread_lock_lock_freeish@@$02@@_N@Z
// @address 0x855f0 ?release_unlocked_resources@c_tag_resource_cache_new@@AAAX_N@Z
// @address 0x21cb40 ?revoke_active_access_cache@c_tag_resource_cache_new@@AAAXXZ
// @address 0x21cd60 ?demand_resource@c_tag_resource_cache_controller_demand_collector@@UAAXPBUs_tag_resource@@@Z
// @address 0x21cd70 ?demand_required_resource@c_tag_resource_cache_controller_demand_collector@@UAAXPBUs_tag_resource@@@Z
// @address 0x21dce0 ?verify_all_required_resources_available@c_tag_resource_cache_new@@AAAXXZ
// @address 0x21d3f8 ?end_tracking_resource@c_tag_resource_cache_new@@QAAXJ@Z
class c_allocation_base;
class c_tag_resource_address_cache_control_interface;
#include <xtl.h>
#include "memory/lruv_block_types.h"
#include "memory/lruv_block_header_types.h"
#include "game/tag_resource_cache_external_access_page_slot_types.h"
#include "tag_files/tag_resource_types.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "main/main_time.h"
#include "memory/lruv_cache.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "multithreading/compiler_barrier.h"
#include "cache/tag_resource_cache_thread_lock.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_lruv_cache_calls.h"

typedef char external_access_layout_check_1[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_page_table) == 100))) ? 1 : -1];
typedef char external_access_layout_check_2[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_page_table, m_async_requests_idle) == 84))) ? 1 : -1];

#include "cache/tag_resource_cache_thread_lock_externs.h"
class c_tag_resource_address_cache_control_interface;
#include "cache/tag_resource_cache_new.h"
typedef char external_access_layout_check_3[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_resource_address_cache) == 208))) ? 1 : -1];
typedef char external_access_layout_check_4[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_inverse_page_table) == 276))) ? 1 : -1];
typedef char external_access_layout_check_5[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_resource_thread_access_lock) == 320))) ? 1 : -1];

struct s_tag_resource_cache_verify_iterator {
    void *datum;
    s_data_iterator iterator;
};

#include <stdlib.h>
#include "cseries/placement_new.h"

struct s_tag_resource_cache_external_access_bit_plane {
    long count;
    unsigned long *bits;
};

struct s_tag_resource_cache_external_access_bit_source {
    s_tag_resource_cache_external_access_bit_plane planes[6];
};

struct s_tag_resource_cache_external_access_bit_iterator {
    long word_index;
    long word_count;
    unsigned long pending_bits;
};

struct s_bit_vector {
    unsigned long *bits;
    long count;
};

class c_tag_resource_cache_external_access_block_allocator {
public:
    virtual void *allocate_lruv(unsigned long size) = 0;
    virtual void *allocate(unsigned long size) = 0;
    virtual void free_block(void *address, unsigned long size) = 0;
};

struct s_tag_resource_cache_external_access_block {
    long allocation_frame;
    bool locked;
    bool free_deferred;
    void *address;
    unsigned long size;
    void *owner;
    __forceinline void set_buffer(void *new_address, unsigned long new_size) {
        address = new_address;
        free_deferred = free_deferred && address != 0;
        size = new_size;
    }
};

enum { k_tag_resource_cache_external_access_block_count = 6 };

struct s_tag_resource_cache_external_access_block_stack : public c_game_state_proc_handler {
    bool enabled;
#ifdef HALO3_64BIT
    __declspec(align(8)) long frame_index;
#else
    long frame_index;
#endif
    s_tag_resource_cache_external_access_block blocks[k_tag_resource_cache_external_access_block_count];
    c_static_flags_no_init<k_tag_resource_cache_external_access_block_count> pending_frees;
    c_tag_resource_cache_external_access_block_allocator *allocator;
};

#include "cache/tag_resource_external_access_calls.h"

#include "cache/tag_resource_cache_published_interface.h"

struct s_tag_resource_cache_external_access_verify_reference {
    c_tag_resource_address_cache *owner;
};

struct s_tag_resource_cache_external_access_verified_resource {
    long handle;
    long external_handle;
    bool optional;
    bool enabled;
    void *owner;
    void *datum;
    void *handler;
};

// @address 0x3e4a0 ??_7s_tag_resource_cache_external_access_handle@@6B@
struct s_tag_resource_cache_external_access_handle {
    __forceinline s_tag_resource_cache_external_access_handle(tag_resource_external_access_table *table, long slot_index)
        : m_table(table), m_slot_index(slot_index) {}

    virtual int add_value(long pair_index, long value);

private:
    tag_resource_external_access_table *m_table;
    long m_slot_index;
};

struct s_tag_resource_cache_lookup_key { long first; long second; long third; };

struct s_tag_resource_cache_external_access_range {
    long *m_elements;
    long m_count;
    __forceinline long *begin() const { return m_elements; }
    __forceinline long *end() const { return m_elements + m_count; }
    static __forceinline long *find_value(long *first, long *last, const long &value)
    {
        for (; first != last; ++first)
            if (*first == value)
                break;
        return first;
    }
    static __forceinline long *remove_copy(long *first, long *last, long *dest, const long &value)
    {
        for (; first != last; ++first)
            if (!(*first == value))
                *dest++ = *first;
        return dest;
    }
    static __forceinline long *remove(long *first, long *last, const long &value)
    {
        first = find_value(first, last, value);
        if (first == last)
            return first;
        else {
            long *first1 = first;
            return remove_copy(++first1, last, first, value);
        }
    }
};

#include <string.h>

#pragma pack(push, 4)

union u_tag_resource_cache_external_access_page_address {
    __int64 value;
    struct { void *data; unsigned long size; };
};

#pragma pack(pop)

union u_resource_key64 {
    __int64 qword;
    struct { void *address; long low_word; };
};

struct s_tag_resource_cache_external_access_bit_set {
    long unused;
    long bits[1];
};

#include "saved_games/saved_film_stream_compressor.h"
#include "cache/tag_resource_paging_task.h"

#include "cache/tag_resource_cache_internal_handle.h"

class c_tag_resource_cache_external_access_iterator;

class c_tag_resource_cache_external_access_iterator {
public:
    virtual bool iterate(long key, long handle, s_tag_resource_cache_external_access_handle *handle_callback) = 0;
};

union u_bit_vector_argument {
    __int64 qword;
    s_bit_vector vector;
};

void c_tag_resource_cache_new::release_unlocked_resources(bool block) {
    c_tag_resource_cache_file_access_cache *cache = 0;
    while ((cache = m_resource_thread_access_lock.acquire_unlocked_access_cache(block)) != 0) {
        m_resource_address_cache.purge_access_cache(cache);
        m_resource_thread_access_lock.release_unlocked_access_cache(cache);
    }
}


// @address 0x88058 ?publish_necessary_resources@c_tag_resource_cache_new@@AAAX_N0@Z
void c_tag_resource_cache_new::publish_necessary_resources(bool a, bool b) {
    c_tag_resource_cache_file_access_cache *cache = m_resource_thread_access_lock.acquire_available_access_cache(false);
    if (cache) {
        m_resource_address_cache.populate_access_cache(cache, false);
        m_resource_thread_access_lock.publish_access_cache(cache);
    }
}

// 0x21dce0
struct s_data_array;

void c_tag_resource_cache_new::verify_all_required_resources_available() {
    c_tag_resource_cache_file_access_cache *cache = tag_resource_cache_thread_lock_lock_freeish_acquire(&m_resource_thread_access_lock);
    s_tag_resource_cache_verify_iterator it;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    _ReadWriteBarrier();
    it.iterator.data = (s_data_array *)m_page_table.m_pages;
    it.datum = data_iterator_next(&it.iterator);
    while (it.datum != 0) {
        it.datum = data_iterator_next(&it.iterator);
    }
    m_resource_thread_access_lock.release_access_cache(cache);
}
// 0x87f58


// 0x140aa8
// @address 0x140aa8 tag_resource_cache_hash_find
inline long tag_resource_cache_hash_find(s_tag_resource_cache_external_access_hash_table *table, long first, long second, long third) {
    long result = -1;
    long bucket = abs(first * 3307 + third * 337 + second * 3) % 8192;
    long probe = bucket;
    long remaining = table->bucket_counts[bucket];
    long iterations = 0;
    while (remaining > 0 && result == -1) {
        if (table->slot_buckets[probe] == bucket) {
            remaining--;
            if (table->keys[probe].first == first && table->keys[probe].second == second && table->keys[probe].third == third) {
                result = probe;
            }
        }
        probe = (probe + 1) % 8192;
        if (iterations++ > 8192) {
            break;
        }
    }
    return result;
}

// 0x140d00
static __forceinline int get_range_offset(s_ranges *object, int index, int *count) {
    s_range *range = &object->ranges[(unsigned short)index];
    *count = range->count;
    return range->offset;
}
// @address 0x140d00 tag_resource_cache_set_bits_from_range
void tag_resource_cache_set_bits_from_range(s_ranges *object, int index, s_bit_vector storage) {
    int count;
    int offset = get_range_offset(object, index, &count);
    if (count > 0) {
        unsigned long *bits = storage.bits;
        do {
            --count;
            int *&indices = object->indices;
            int bit = (unsigned short)indices[offset++];
            bits[bit >> 5] |= 1 << (bit & 31);
        } while ((unsigned)count != 0);
    }
}

 // 0x1afc20
// @address 0x1afc20 tag_resource_cache_free_block
void tag_resource_cache_free_block(s_tag_resource_cache_external_access_block_stack *stack, long block_index, void *address) {
    bool is_top = true;
    for (long index = block_index + 1; index < k_tag_resource_cache_external_access_block_count; index++) {
        if (stack->blocks[index].address != 0) {
            is_top = false;
            break;
        }
    }
    if (is_top) {
        s_tag_resource_cache_external_access_block *block = &stack->blocks[block_index];
        unsigned long block_size = block->size;
        block->set_buffer(0, 0);
        unsigned long size = block_size;
        for (long index = block_index - 1; index >= 0; index--) {
            if (stack->pending_frees.test(index)) {
                block = &stack->blocks[index];
                block_size = block->size;
                block->set_buffer(0, 0);
                size += block_size;
                stack->pending_frees.set(index, false);
            }
        }
        stack->pending_frees.set(block_index, false);
        stack->allocator->free_block(address, size);
    } else if (!stack->pending_frees.test(block_index)) {
        stack->pending_frees.set(block_index, true);
    }
}


void c_tag_resource_cache_new::revoke_active_access_cache() {
    m_resource_thread_access_lock.flush_all_access_caches();
}
static __forceinline void tag_resource_external_access_make_key_out(long value, long flag, long *key) {
    bool positive = (flag == 1);
    unsigned short high = (unsigned short)~(value >> 16);
    if (positive) {
        high |= 0x8000;
    } else {
        high &= 0x7fff;
    }
    long low = (value << 1) & 0x1fffe;
    if (!positive) {
        low += 1;
    }
    *key = (high << 16) | low;
}
static __forceinline long tag_resource_external_access_make_key(long value, long flag) {
    bool positive = (flag == 1);
    unsigned short high = (unsigned short)~(value >> 16);
    if (positive) {
        high |= 0x8000;
    } else {
        high &= 0x7fff;
    }
    long low = (value << 1) & 0x1fffe;
    if (!positive) {
        low += 1;
    }
    return (high << 16) | low;
}
static __forceinline bool tag_resource_external_access_test_bit(const unsigned long *bits, long bit) {
    return (bits[bit >> 5] & (1UL << (bit & 31))) != 0;
}
static __forceinline void tag_resource_external_access_set_bit(unsigned long *bits, long bit) {
    bits[bit >> 5] |= 1 << (bit & 31);
}
// @address 0x21cc40 ?set_active_slot@c_tag_resource_cache_controller_demand_collector@@UAAXJ@Z
void c_tag_resource_cache_controller_demand_collector::set_active_slot(long index) {
    if (index == -1) {
        return;
    }
    m_active_index = index;
    tag_resource_external_access_table *table = m_tables->table;
    if (table == 0) {
        return;
    }
    s_tag_resource_cache_external_access_allocation_slot *slot = &table->slot_list.slots[index];
    if ((unsigned short)slot->generation != 0xffff) {
        return;
    }
    unsigned short generation = (unsigned short)table->value_list.count;
    slot->owner = 0;
    slot->next = -1;
    slot->count = 0;
    slot->generation = generation;
}

// @address 0x21ce40 ?predict_resource@c_tag_resource_cache_controller_demand_collector@@AAAXJJ@Z
void c_tag_resource_cache_controller_demand_collector::predict_resource(long resource_handle, long lod) {
    if (resource_handle == -1) {
        return;
    }
    long active_index = m_active_index;
    m_prediction_count++;
    if (active_index != -1) {
        tag_resource_external_access_table *table = m_tables->table;
        if (table == 0) {
            return;
        }
        s_tag_resource_cache_external_access_allocation_slot *slot = &table->slot_list.slots[active_index];
        if (slot->next == -1) {
            slot->next = (short)table->key_list.count;
        }
        tag_resource_cache_find_or_add_key(table, slot->next, &slot->count, resource_handle, lod);
        return;
    }
    long key = tag_resource_external_access_make_key(resource_handle, lod);
    if (!tag_resource_external_access_test_bit(m_demanded_resources, (unsigned short)key)) {
        m_unique_prediction_count++;
    }
    tag_resource_external_access_set_bit(m_demanded_resources, (unsigned short)key);
}

void c_tag_resource_cache_controller_demand_collector::demand_resource(const s_tag_resource *tag_resource) {
    predict_resource(tag_resource->resource_handle, 1);
}

void c_tag_resource_cache_controller_demand_collector::demand_required_resource(const s_tag_resource *tag_resource) {
    predict_resource(tag_resource->resource_handle, 0);
}

// 0x21d0c8
struct s_data_array;

// @address 0x21d0c8 tag_resource_cache_track_element
void tag_resource_cache_track_element(tag_resource_external_access_set_object *o, int idx, int n)
{
    datum_new_at_index(o->m_inverse_page_table.table, idx);
    int i = idx & 0xffff;
    s_tag_resource_cache_external_access_slot *e = &((s_tag_resource_cache_external_access_slot *)o->m_inverse_page_table.table->data)[i];
    int rfT0 = o->m_inverse_page_table.flags;
    if (rfT0 & 1) {
        e->first = idx;
    } else {
        e->first = o->m_inverse_page_table.cursor;
        o->m_inverse_page_table.cursor = o->m_inverse_page_table.cursor + n;
    }
    e->capacity = n;
    e->count = 0;
    datum_new_at_index(o->m_page_table.m_pages, idx);
    s_tag_resource_page_datum *p = datum_get<s_tag_resource_page_datum>(o->m_page_table.m_pages, idx);
    int cnt = p->state;
    p->state = 0;
    if (cnt > 0) {
        if (cnt >= 4)
        { int w = i >> 5; unsigned long *base = o->m_page_table.m_pinned_pages.m_flags; base[w] &= ~(1 << (i & 31)); }
        if (cnt >= 3)
        { int w = i >> 5; unsigned long *base = o->m_page_table.m_ready_pages.m_flags; base[w] &= ~(1 << (i & 31)); }
    }
}

// 0x21d5e0
static __forceinline bool tag_resource_external_access_bit_test(const unsigned long *bits, long bit) { return (bits[bit >> 5] & (1UL << (bit & 31))) != 0; }
static __forceinline int tag_resource_external_access_count(c_tag_resource_cache_new *cache, long index)
{
    s_data_array *rows = cache->m_inverse_page_table.table;
    return datum_get<s_tag_resource_cache_external_access_slot>(rows, index)->count;
}
// @address 0x21d5e0 ?resource_is_locked_externally@c_tag_resource_cache_new@@QAA_NJ@Z
bool c_tag_resource_cache_new::resource_is_locked_externally(long p1)
{
    int tr4;
    tr4 = p1 << 3 & 524280;
    bool tr3;
    tr3 = 0;
    int tr6;
    tr6 = 0;
    int tr30;
    tr30 = tag_resource_external_access_count(this, p1);
    while (!tr3 && tr6 < tr30) {
        bool tr11;
        tr11 = 0;
        int tr7;
        tr7 = 0;
        int tr5;
        tr5 = this->m_resource_address_cache.m_published_states.count();
        int tr8;
        tr8 = 0;
        int tr10;
        s_tag_resource_cache_external_access_slot_owner *list = &m_inverse_page_table;
        int first = datum_get<s_tag_resource_cache_external_access_slot>(list->table, p1)->first;
        tr10 = this->m_inverse_page_table.list.begin()[first + tr6] & 65535;
        while (!tr11 && tr7 < tr5) {
            const c_tag_resource_address_cache::s_published_resources_state *element
                = &this->m_resource_address_cache.m_published_states.begin()[tr7];
            tr11 = element->published != 0 && tag_resource_external_access_bit_test(element->available.m_flags, tr10);
            tr7 = tr7 + 1;
            tr8 = tr8 + 32;
        }
        tr3 = tr11;
        tr6 = tr6 + 1;
    }
    return tr3;
}

// 0x21d948
void c_tag_resource_cache_published_interface::prepare_for_delete(long delete_handle)
{
    // The interface method runs on the cache object itself: the controller and
    // address cache sit at fixed offsets of the cache.
    c_tag_resource_cache_new *cache = (c_tag_resource_cache_new *)this;
    c_tag_resource_cache_controller *object = &cache->m_controller;
    c_tag_resource_address_cache_control_interface *control
        = (c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache;
    ::c_tag_resource_cache_controller::release_resource_page_references_for_cache(object, delete_handle, control);
    int handle = (unsigned short)delete_handle;
    int byteindex = (handle >> 5) * 4;
    int *words = (int *)object->m_pending_release.m_storage;
    int *word = (int *)((char *)words + byteindex);
    *word &= ~(1 << (handle & 31));
}

// 0x21c970
struct s_data_array;
static __forceinline void tag_resource_cache_external_access_dispose_lruv_cache(c_tag_resource_page_table::c_tag_resource_lruv_cache *lruv)
{
    lruv->resize(0, 0, 0, false, 0);
    s_lruv_cache *cache = lruv->m_cache;
    c_allocation_base *allocator = cache->allocator;
    data_dispose((s_data_array *)cache->header.block_data);
    allocator->deallocate(cache);
    lruv->m_buffer.m_address = 0;
    lruv->m_buffer.m_size = 0;
}
static __forceinline void tag_resource_cache_external_access_dispose_page_table(c_tag_resource_page_table *table)
{
    table->m_async_requests_idle = true;
    table->m_control_interface.m_table = 0;
    tag_resource_cache_external_access_data_make_invalid(table->m_async_requests);
    tag_resource_cache_external_access_data_make_invalid((s_data_array *)table->m_pages);
    table->m_allocator->deallocate(table->m_storage);
    table->m_ready_pages.m_flags = 0;
    table->m_ready_pages.m_count = 0;
    table->m_pinned_pages.m_flags = 0;
    table->m_pinned_pages.m_count = 0;
    tag_resource_cache_external_access_dispose_lruv_cache(&table->m_lruv_cache);
}
static __forceinline void tag_resource_cache_external_access_clear_flags(c_wrapped_flags *flags)
{
    flags->m_flags = 0;
    flags->m_count = 0;
}
static __forceinline void tag_resource_cache_external_access_dispose_address_cache(c_tag_resource_address_cache *cache)
{
    cache->m_controller = 0;
    cache->m_states.set_elements(0, 0);
    cache->m_published_states.set_elements(0, 0);
    tag_resource_cache_external_access_clear_flags(&cache->m_available_addresses);
    tag_resource_cache_external_access_clear_flags(&cache->m_demanded_resources);
    cache->m_allocator->deallocate(cache->m_allocation);
    cache->m_allocation = 0;
}


// @address 0x412f4 ??_7c_data_array_allocation_description@@6B@
static __forceinline bool tag_resource_cache_external_access_index_is_optional(long handle)
{
    return (((unsigned short)(handle >> 16) >> 15) & 1) != 0;
}
static __forceinline void tag_resource_cache_external_access_bit_plane_set_from_flags(
    s_tag_resource_cache_external_access_bit_plane *plane, s_tag_resource_cache_external_access_wrapped_flags value)
{
    plane->bits = value.m_storage;
    plane->count = (value.m_count + 31) >> 5;
}
static __forceinline void tag_resource_cache_external_access_bit_plane_set_from_flags(
    s_tag_resource_cache_external_access_bit_plane *plane, c_wrapped_flags value)
{
    plane->bits = value.m_flags;
    plane->count = (value.m_count + 31) >> 5;
}
static __forceinline bool tag_resource_cache_external_access_verify_resource(
    const s_tag_resource_cache_external_access_verified_resource *resource, long handle)
{
    return resource->optional == tag_resource_cache_external_access_index_is_optional(handle);
}
// @address 0x87dd8 tag_resource_cache_verify_resources
// match: 0x87dd8: noalias declaration preserves caller live registers
extern "C" __declspec(noalias) void tag_resource_cache_verify_resources(
    c_tag_resource_cache_controller *params, s_tag_resource_cache_external_access_verify_reference *reference)
{
    s_tag_resource_cache_verify_iterator data_iterator;
    data_iterator.iterator.absolute_index = -1;
    data_iterator.iterator.index = -1;
    data_iterator.iterator.data = params->m_resources;
    bool different;
    {
        c_tag_resource_address_cache *owner = reference->owner;
        s_tag_resource_cache_external_access_wrapped_flags selected;
        if (params->m_can_load_all_desired_optional)
            selected = params->m_required;
        else
            selected = params->m_predicted;
        s_tag_resource_cache_external_access_bit_source source;
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[0], params->m_available);
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[1], params->m_required);
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[2], owner->m_available_addresses);
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[3], selected);
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[4], params->m_requested);
        tag_resource_cache_external_access_bit_plane_set_from_flags(&source.planes[5], params->m_pending);
        s_tag_resource_cache_external_access_bit_iterator bits;
        bits.word_count = ((unsigned long)params->m_resources->first_unallocated + 31) >> 5;
        bits.pending_bits = 0;
        bits.word_index = -1;
        long bit_index;
        different = tag_resource_cache_find_next_mismatch(&bits, &source, &bit_index) != 0;
    }
    if (different) {
        data_iterator.datum = data_iterator_next(&data_iterator.iterator);
        if (data_iterator.datum) {
            do {
                s_tag_resource_cache_external_access_verified_resource resource;
                resource.optional = tag_resource_cache_external_access_index_is_optional(data_iterator.iterator.index);
                bool (*verify)(const s_tag_resource_cache_external_access_verified_resource *, long) = tag_resource_cache_external_access_verify_resource;
                // match: 0x87dd8: value range assumption guides branch lowering
                __assume(verify(&resource, data_iterator.iterator.index));
                data_iterator.datum = data_iterator_next(&data_iterator.iterator);
            } while (data_iterator.datum);
        }
    }
}
// @address 0x87f58 tag_resource_cache_find_next_mismatch
unsigned char tag_resource_cache_find_next_mismatch(
    s_tag_resource_cache_external_access_bit_iterator *iterator, s_tag_resource_cache_external_access_bit_source *source, long *bit_index)
{
    while (iterator->pending_bits == 0) {
        long index = ++iterator->word_index;
        if (index >= iterator->word_count) {
            break;
        }
        unsigned long other = source->planes[2].bits[index];
        unsigned long mask = source->planes[1].bits[index];
        unsigned long valid = source->planes[4].bits[index];
        unsigned long cleared = source->planes[0].bits[index] & ~mask;
        unsigned long selected = ((source->planes[3].bits[index] & mask) | cleared) & valid;
        unsigned long reference = source->planes[5].bits[index];
        iterator->pending_bits =
            ((selected & mask) ^ reference) |
            ((other & mask) ^ reference) |
            ~(valid | ~cleared) |
            ~(valid | ~other) |
            (selected ^ other);
    }
    long index = iterator->word_index;
    bool result = false;
    if (index < iterator->word_count) {
        unsigned long pending_bits = iterator->pending_bits;
        unsigned long lowest_bit = pending_bits & -pending_bits;
        iterator->pending_bits = (pending_bits - 1) & pending_bits;
        result = true;
        *bit_index = (index << 5) + 31 - _CountLeadingZeros(lowest_bit & -lowest_bit);
    }
    return result;
}
// @address 0x21c7f0 tag_resource_cache_new_initialize
extern "C" long tag_resource_cache_new_initialize(c_tag_resource_cache_new *cache, unsigned long count, unsigned long maximum_page_count, long,
    c_tag_resource_runtime_listener *runtime_listener, c_tag_resource_cache_file_reader *reader,
    c_tag_resource_cache_file_datum_handler *datum_handler, c_tag_resource_page_allocator *page_allocator,
    c_tag_resource_cache_new_loaded_query *loaded_query, c_tag_resource_paging_page_filter *page_filter,
    c_allocation_base *allocator, c_tag_resource_page_range_allocator *range_allocator)
{
    cache->m_allocator = allocator;
    cache->m_published_interface_owner = cache;
    cache->m_page_io_listener_owner.m_owner = cache;
    cache->m_page_table.initialize_for_new_map(maximum_page_count, reader, datum_handler, page_allocator,
        (c_tag_resource_cache_published_interface *)cache, allocator, range_allocator);
    s_tag_resource_cache_external_access_slot_owner *inverse_table = &cache->m_inverse_page_table;
    inverse_table->m_allocator = allocator;
    s_allocation_description_holder descriptions[] = {
        { c_wrapped_array_allocation_description<long>(inverse_table->list, count * 2) },
        { c_data_array_allocation_description(inverse_table->table, "page reference ranges", maximum_page_count, 8, 1) }
    };
    c_wrapped_array_no_init<s_allocation_description_holder> list_storage;
    c_wrapped_array<s_allocation_description_holder> *list =
        new (&list_storage) c_wrapped_array<s_allocation_description_holder>(descriptions);
    allocate_list_contiguous(allocator, *list, "tag resource inverse page table", &inverse_table->m_storage, 0);
    s_data_array *data = inverse_table->table;
    inverse_table->cursor = 0;
    data->valid = 1;
    data_delete_all(data);
    (&cache->m_controller)->initialize_for_new_map(count, &cache->m_page_table.m_control_interface, runtime_listener, datum_handler, allocator);
    cache->m_resources_changed = false;
    cache->m_resources_valid = false;
    cache->m_flag303 = false;
    cache->m_thread_access_locked = false;
    cache->m_resize_succeeded = true;
    cache->m_full_size = true;
    cache->m_resource_interface = datum_handler;
    cache->m_loaded_query = loaded_query;
    cache->m_page_filter = page_filter;
    cache->m_resource_thread_access_lock.initialize_for_new_map(count, allocator);
    (&cache->m_resource_address_cache)
        ->initialize_for_new_map(count, maximum_page_count, 3, allocator);
    return 1;
}
// @address 0x21c970 ?dispose_from_old_map@c_tag_resource_cache_new@@QAAXXZ
void c_tag_resource_cache_new::dispose_from_old_map(void)
{
    tag_resource_cache_external_access_dispose_address_cache(&m_resource_address_cache);
    for (long index = 0; index < 3; index++)
    {
        c_tag_resource_cache_file_access_cache *cache = &m_resource_thread_access_lock.m_access_cache_storage[index];
        cache->m_resource_flags.set_elements(0, 0);
        cache->m_resource_handles.set_elements(0, 0);
        cache->m_resource_values.set_elements(0, 0);
        cache->m_allocator->deallocate(cache->m_allocation);
    }
    m_controller.dispose_from_old_map();
    m_inverse_page_table.dispose_from_old_map();
    tag_resource_cache_external_access_dispose_page_table(&m_page_table);
    m_page_io_listener_owner.m_owner = 0;
    m_published_interface_owner = 0;
    m_allocator = 0;
}

// 0x21d1c8
static __forceinline bool tag_resource_cache_external_access_handle_is_optional(long handle)
{
    bool optional = (((unsigned short)(handle >> 16) & 0x8000) != 0);
    return optional;
}
// 0x21dfe0
typedef char tag_resource_external_access_pairs_offset[(sizeof(void *) != 4
    || offsetof(s_tag_resource_cache_external_access_state, pairs) == 0x28004) ? 1 : -1];
typedef char tag_resource_external_access_second_pair_offset[(sizeof(void *) != 4
    || offsetof(s_tag_resource_cache_external_access_state, pairs) + sizeof(s_tag_resource_cache_external_access_pair) == 0x2800c) ? 1 : -1];
typedef char tag_resource_external_access_value_list_offset[(sizeof(void *) != 4
    || offsetof(s_tag_resource_cache_external_access_state, value_list) == 0x8a038) ? 1 : -1];
typedef t_tag_resource_cache_external_access_list<long, 0x9000> tag_resource_external_access_value_list;
typedef char tag_resource_external_access_array_count_offset[(sizeof(void *) != 4
    || offsetof(tag_resource_external_access_value_list, count) == 0x24000) ? 1 : -1];
// @address 0x21dfe0 tag_resource_cache_reset_state
extern "C" void tag_resource_cache_reset_state(s_tag_resource_cache_external_access_state *state) {
    s_tag_resource_cache_external_access_pair *pairs = state->pairs;
    pairs[0].elements = 0;
    pairs[0].count = 0;
    pairs[1].elements = 0;
    pairs[1].count = 0;
    pairs[2].elements = 0;
    pairs[2].count = 0;
    pairs[3].elements = 0;
    pairs[3].count = 0;
    state->hash_table.count = 0;
    memset(state->hash_table.bucket_counts, 0, sizeof(state->hash_table.bucket_counts));
    memset(state->hash_table.slot_buckets, 0xffff, sizeof(state->hash_table.slot_buckets));
    s_tag_resource_cache_group_hash_table *table = &state->group_hash_table;
    table->entry_count = 0;
    memset(table->hash_entry_count, 0, sizeof(table->hash_entry_count));
    memset(table->original_hash_index, 0xffff, sizeof(table->original_hash_index));
    state->group_count = 0;
    state->key_list.count = 0;
    state->value_list.count = 0;
    state->slot_list.count = 0;
    state->key_list_overflow = 0;
    state->group_overflow = 0;
    state->value_overflow = 0;
    state->slot_overflow = 0;
}

// 0x21e0f0
// @address 0x21ed10 tag_resource_cache_hash_insert
// @address 0x21e0f0 tag_resource_cache_get_or_add_slot
static __forceinline long tag_resource_cache_external_access_find(s_tag_resource_cache_external_access_hash_table *table, long first, long second, long third) {
    long result = -1;
    long index = tag_resource_cache_hash_find(table, first, second, third);
    if (index != -1) {
        result = table->keys[index].value;
    }
    return result;
}
long tag_resource_cache_get_or_add_slot(s_tag_resource_cache_external_access_state *state, long first, long second, long third) {
    s_tag_resource_cache_external_access_hash_table *table = &state->hash_table;
    long result = tag_resource_cache_external_access_find(table, first, second, third);
    long index;
    if (result == -1) {
        if (state->slot_list.count != 0x2000) {
            index = tag_resource_cache_hash_insert(table, first, second, third);
            if (index != -1) {
                s_tag_resource_cache_external_access_key *entry = &table->keys[index];
                if (entry) {
                    s_tag_resource_cache_external_access_state::s_slot_list *slots = &state->slot_list;
                    long slot_index = slots->count++;
                    s_tag_resource_cache_external_access_allocation_slot *slot = &state->slot_list.slots[slot_index];
                    slot->generation = -1;
                    slot->next = -1;
                    entry->value = slot_index;
                    result = slot_index;
                }
            }
        } else {
            state->slot_overflow = 1;
            if (!state->slot_overflow_reported) {
                state->slot_overflow_reported = 1;
            }
        }
    }
    return result;
}

// @address 0x21cbb8 ?get_slot_from_lookup@c_tag_resource_cache_controller_demand_collector@@UAAJJJJ@Z
long c_tag_resource_cache_controller_demand_collector::get_slot_from_lookup(long first, long second, long third) {
    long result;
    s_tag_resource_cache_external_access_tables *lookup = m_tables;
    if (lookup != 0) {
        long found = -1;
        long index = tag_resource_cache_hash_find(&lookup->original_table->hash_table, first, second, third);
        if (index != -1) {
            found = lookup->original_table->hash_table.keys[index].value;
        }
        result = found;
        if (found == -1 && lookup->table != 0) {
            return tag_resource_cache_get_or_add_slot(lookup->table, first, second, third);
        }
    } else {
        result = -1;
    }
    return result;
}

// 0x21e510
// @address 0x21e510 tag_resource_cache_find_group_handle
bool tag_resource_cache_find_group_handle(s_tag_resource_cache_group_hash_table *table, int key, int *output)
{
    int product = abs(key * 3307);
    bool success = false;
    int found = -1;
    int iterations = 0;
    int original = product % 2048;
    int index = original;
    int count = table->hash_entry_count[original];
    while (count > 0 && found == -1) {
        if (table->original_hash_index[index] == original) {
            s_hash_entry *entry = &table->entries[index];
            --count;
            if (entry->key0 == key && entry->key1 == 0 && entry->key2 == 0)
                found = index;
        }
        index = (index + 1) % 2048;
        if (iterations++ > 2048)
            break;
    }
    if (found != -1) {
        success = true;
        *output = table->entries[found].value;
    }
    return success;
}


// 0x21e910
static __forceinline long *tag_resource_external_access_list_end(tag_resource_external_access_key_list *list) {
    return list->data + list->count;
}
static __forceinline void tag_resource_external_access_list_add(tag_resource_external_access_key_list *list, long key) {
    long slot = list->count++;
    list->data[slot] = key;
}
// @address 0x21e910 tag_resource_cache_find_or_add_key
extern "C" void tag_resource_cache_find_or_add_key(tag_resource_external_access_table *table, long index, unsigned short *counter, long value, long flag) {
    long key = tag_resource_external_access_make_key(value, flag);
    bool found = false;
    long *first = &table->key_list.data[index];
    long count = tag_resource_external_access_list_end(&table->key_list) - first;
    long *it;
    for (it = first; it != first + count; it++) {
        if (key == *it) {
            break;
        }
    }
    if (it != first + count) {
        found = true;
    }
    if (found) {
        return;
    }
    if (table->key_list.count != 0x4000) {
        tag_resource_external_access_list_add(&table->key_list, key);
        (*counter)++;
        return;
    }
    table->key_list_overflow = true;
    if (!table->key_list_overflow_reported) {
        table->key_list_overflow_reported = true;
    }
}

// 0x21e498
// @address 0x21e498 tag_resource_cache_set_pair_value
extern "C" void tag_resource_cache_set_pair_value(tag_resource_external_access_table *table, long slot_index, long pair_index, long value, long flag) {
    if (value == -1) {
        return;
    }
    if (pair_index != -1) {
        s_tag_resource_cache_external_access_group *pair =
            &table->groups[(unsigned short)pair_index];
        tag_resource_cache_find_or_add_key(table, pair->first, &pair->count, value, flag);
        return;
    }
    if (slot_index == -1) {
        return;
    }
    s_tag_resource_cache_external_access_allocation_slot *slot = &table->slot_list.slots[slot_index];
    if (slot->next == -1) {
        slot->next = (short)table->key_list.count;
    }
    tag_resource_cache_find_or_add_key(table, slot->next, &slot->count, value, flag);
}

// 0x21e608
// @address 0x21e608 ?add_value@s_tag_resource_cache_external_access_handle@@UAAHJJ@Z
int s_tag_resource_cache_external_access_handle::add_value(long pair_index, long value) {
    tag_resource_cache_set_pair_value(m_table, m_slot_index, pair_index, value, 1);
    return 1;
}

// 0x21ebf0
// @address 0x21ebf0 tag_resource_cache_new_group_entry
long tag_resource_cache_new_group_entry(s_tag_resource_cache_group_hash_table *table, long key, long key1, long key2) {
    long result = -1;
    if (table->entry_count < 2048) {
        long bucket = abs(key * 3307) % 2048;
        long probe = bucket;
        long remaining = table->hash_entry_count[bucket];
        while (remaining > 0) {
            if (table->original_hash_index[probe++ % 2048] == bucket) {
                remaining--;
            }
        }
        bool found;
        do {
            found = table->original_hash_index[probe % 2048] == -1;
            if (!found) {
                probe++;
            }
        } while (!found);
        result = probe % 2048;
        if (result != -1) {
            table->entries[result].key0 = key;
            table->entries[result].key1 = 0;
            table->entries[result].key2 = 0;
            table->original_hash_index[result] = (short)bucket;
            table->hash_entry_count[bucket]++;
        }
        table->entry_count++;
    }
    return result;
}

// 0x21ed10
inline long tag_resource_cache_hash_insert(s_tag_resource_cache_external_access_hash_table *table, long first, long second, long third) {
    long result = -1;
    if (table->count < 8192) {
        long bucket = abs(first * 3307 + third * 337 + second * 3) % 8192;
        long probe = bucket;
        long remaining = table->bucket_counts[bucket];
        while (remaining > 0) {
            if (table->slot_buckets[probe++ % 8192] == bucket) {
                remaining--;
            }
        }
        bool found;
        do {
            found = table->slot_buckets[probe % 8192] == -1;
            if (!found) {
                probe++;
            }
        } while (!found);
        result = probe % 8192;
        if (result != -1) {
            table->keys[result].first = first;
            table->keys[result].second = second;
            table->keys[result].third = third;
            table->slot_buckets[result] = (short)bucket;
            table->bucket_counts[bucket]++;
        }
        table->count++;
    }
    return result;
}

// 0x21eb10
// @address 0x21eb10 tag_resource_cache_initialize_hash_table
void tag_resource_cache_initialize_hash_table(s_tag_resource_cache_external_access_state *state,
    s_tag_resource_cache_external_access_pair list, s_tag_resource_cache_external_access_pair a,
    s_tag_resource_cache_external_access_pair b, s_tag_resource_cache_external_access_pair c, s_tag_resource_cache_external_access_pair d)
{
    state->hash_table.count = 0;
    memset(state->hash_table.bucket_counts, 0, sizeof(state->hash_table.bucket_counts));
    memset(state->hash_table.slot_buckets, 0xffff, sizeof(state->hash_table.slot_buckets));
    s_tag_resource_cache_external_access_pair *image = state->pairs;
    image[0] = a;
    state->pairs[1] = b;
    image[2] = c;
    image[3] = d;
    long index = 0;
    long count = list.count;
    if (count > 0) {
        s_tag_resource_cache_lookup_key *records = static_cast<s_tag_resource_cache_lookup_key *>(list.elements);
        do {
            long *slot = 0;
            long third = records[index].third;
            long found = tag_resource_cache_hash_insert(&state->hash_table, records[index].first, records[index].second, third);
            if (found != -1) {
                slot = &state->hash_table.keys[found].value;
            }
            *slot = index;
            index++;
        } while (index < count);
    }
}

// 0x2aee30
// @address 0x2aee30 tag_resource_cache_delete_inverse_handle
extern "C" void tag_resource_cache_delete_inverse_handle(s_tag_resource_cache_external_access_slot_owner *slot_owner, long index, long value)
{
    s_tag_resource_cache_external_access_slot *slot = datum_get<s_tag_resource_cache_external_access_slot>(slot_owner->table, index);
    c_wrapped_array<long> list = slot_owner->list;
    s_tag_resource_cache_external_access_range range;
    range.m_elements = list.begin() + slot->first;
    range.m_count = slot->count;
    slot->count = (unsigned short)(
        s_tag_resource_cache_external_access_range::remove(range.begin(), range.end(), value) - range.begin());
}

// 0x21ea38
static __forceinline long *tag_resource_cache_external_access_find(long *first, long *last, long value) {
    for (; first != last; ++first) {
        if (*first == value)
            break;
    }
    return first;
}
static __forceinline void tag_resource_cache_external_access_array_add(tag_resource_external_access_value_list *array, long value) {
    long *total_ptr = &array->count;
    long total = (*total_ptr)++;
    array->elements[total] = value;
}
// @address 0x21ea38 tag_resource_cache_find_or_add_value
long tag_resource_cache_find_or_add_value(s_tag_resource_cache_external_access_state *state, long index, unsigned short *count_ptr, long value) {
    unsigned short n = *count_ptr;
    long *begin = (long *)((char *)state + ((index + 0x2280E) << 2));
    long *end = begin + n;
    long result = 0;
    bool found = false;
    long *it = tag_resource_cache_external_access_find(begin, end, value);
    if (it != begin + n)
        found = true;
    if (!found) {
        if (state->value_list.count != 0x9000) {
            tag_resource_cache_external_access_array_add(&state->value_list, value);
            (*count_ptr)++;
            result = 1;
        } else {
            state->value_overflow = 1;
            if (!state->value_overflow_reported)
                state->value_overflow_reported = 1;
        }
    }
    return result;
}

// 0x21deb0

// 0x21d9b0
// @address 0x21d9b0 tag_resource_cache_get_page_data
extern "C" bool tag_resource_cache_get_page_data(c_tag_resource_cache_new *cache, long resource, long optional, long offset, long size,
    c_basic_buffer<void> buffer)
{
    bool result = false;
    long page_index;
    long page_offset;
    if (optional == 1)
        page_index = cache->m_resource_interface->get_optional_page_index(resource);
    else
        page_index = cache->m_resource_interface->get_required_page_index(resource);
    if (optional == 1)
        page_offset = cache->m_resource_interface->get_optional_offset(resource);
    else
        page_offset = cache->m_resource_interface->get_pageable_offset(resource);
    s_tag_resource_page_datum *page = &((s_tag_resource_page_datum *)cache->m_page_table.m_pages->data)[(unsigned short)page_index];
    if (page->state == 4)
    {
#ifdef HALO3_64BIT
        c_basic_buffer<void> page_buffer = page->buffer;
        memcpy(buffer.m_address, (char *)page_buffer.m_address + page_offset + offset, size);
#else
        u_tag_resource_cache_external_access_page_address page_address;
        page_address.value = page->address;
        memcpy(buffer.m_address, (char *)page_address.data + page_offset + offset, size);
#endif
        result = true;
    }
    return result;
}

// 0x21e1f8
template <typename t_type, long k_maximum_count>
static __forceinline s_tag_resource_cache_external_access_pair tag_resource_cache_external_access_list_pair(
    t_tag_resource_cache_external_access_list<t_type, k_maximum_count> *list)
{
    s_tag_resource_cache_external_access_pair pair;
    pair.count = list->count;
    pair.elements = list->elements;
    return pair;
}
// @address 0x21e1f8 tag_resource_cache_find_duplicate_slot
void tag_resource_cache_find_duplicate_slot(s_tag_resource_cache_external_access_state *state, long index) {
    s_tag_resource_cache_external_access_allocation_slot *slot_p1 = &state->slot_list.slots[index];
    for (long found_index = 0; found_index < state->slot_list.count - 1; found_index++) {
        s_tag_resource_cache_external_access_allocation_slot *slot = &state->slot_list.slots[found_index];
        unsigned short need = slot_p1->owner;
        if (slot->owner == need) {
            bool all_found = true;
            long vi = 0;
            do {
                if (vi >= need)
                    break;
                all_found = false;
                long *list = &state->value_list.elements[slot->generation];
                long elem = state->value_list.elements[slot_p1->generation + vi];
                long count = need;
                long *limit = list + count;
                long *base;
                for (base = list; base != limit; base++) {
                    if (*base == elem)
                        break;
                }
                if (base != list + count)
                    all_found = true;
                vi++;
            } while (all_found);
            if (all_found) {
                state->value_list.count = slot_p1->generation;
                slot_p1->generation = slot->generation;
                break;
            }
        }
    }
    s_tag_resource_cache_external_access_pair *pairs = state->pairs;
    pairs[0] = tag_resource_cache_external_access_list_pair((t_tag_resource_cache_external_access_list<long, 0x4000> *)state->key_list.data);
    s_tag_resource_cache_external_access_pair slot_pair = tag_resource_cache_external_access_list_pair(
        (t_tag_resource_cache_external_access_list<s_tag_resource_cache_external_access_allocation_slot, 0x2000> *)&state->slot_list);
    s_tag_resource_cache_external_access_pair array_pair = tag_resource_cache_external_access_list_pair(
        &state->value_list);
    pairs[3] = slot_pair;
    pairs[2] = array_pair;
}

// @address 0x21e378 tag_resource_cache_find_duplicate_group
long tag_resource_cache_find_duplicate_group(s_tag_resource_cache_external_access_state *state, long index)
{
    long result = index;
    s_tag_resource_cache_external_access_group *target = &state->groups[(unsigned short)index];
    unsigned short need = target->count;
    if (need == 0) {
        result = -1;
    } else {
        long total = state->group_count;
        long slot = 0;
        if (total > 0) {
            do {
                s_tag_resource_cache_external_access_group *group = &state->groups[slot];
                if (group->count == need && group->first != target->first) {
                    bool all_found = true;
                    long vi = 0;
                    do {
                        if (vi >= need)
                            break;
                        bool found = false;
                        long *list = &state->key_list.data[group->first];
                        long elem = state->key_list.data[target->first + vi];
                        long *limit = list + group->count;
                        long *base;
                        for (base = list; base != limit; base++) {
                            if (elem == *base)
                                break;
                        }
                        if (base != list + group->count)
                            found = true;
                        all_found = found;
                        vi++;
                    } while (all_found);
                    if (all_found) {
                        result = ((long)group->identifier << 16) | slot;
                        break;
                    }
                }
                slot++;
            } while (slot < total);
        }
    }
    return result;
}


// 0x21f6e0
static __forceinline void tag_resource_cache_external_access_bit_set(long *bits, long bit) {
    bits[bit >> 5] |= 1 << (bit & 31);
}
// @address 0x21f6e0 tag_resource_cache_set_bit
extern "C" void tag_resource_cache_set_bit(s_tag_resource_cache_external_access_bit_set *set, short index) {
    if (index != -1) {
        tag_resource_cache_external_access_bit_set(set->bits, index);
    }
}

// @address 0x85070 ?reset_receiver@c_tag_resource_paging_secondary_task@@UAAXHHH@Z
void c_tag_resource_paging_secondary_task::reset_receiver(int, int, int) {
    s_tag_resource_paging_owner *owner = &context;
    s_saved_film_stream_compressor *decoder = owner->decoder;
    s_tag_resource_paging_codec_stream *stream = &decoder->m_stream;
    tag_resource_paging_fiber_dispose(&stream->fiber);
    tag_resource_paging_fiber_dispose(&stream->fiber);
    owner->decoder = 0;
}

// 0x21d3f8
static __forceinline void *tag_resource_cache_get_permanent_resource_root_address(c_tag_resource_cache_controller *controller, long resource_handle)
{
    long internal_datum_header = resource_handle >> 16;
    bool optional = (unsigned char)(((unsigned short)internal_datum_header >> 15) & 1) != 0;
    unsigned long datum_header = (unsigned short)~internal_datum_header;
    datum_header |= 0x8000;
    unsigned long index = (unsigned short)resource_handle;
    index /= 2;
    long external_handle = (datum_header << 16) | index;
    return controller->m_file_datum_handler->get_permanent_resource_root_address(external_handle, optional);
}
void c_tag_resource_cache_new::end_tracking_resource(long resource)
{
    if (m_resource_interface->any_optional_data(resource)) {
        long handle = tag_resource_cache_make_internal_handle(resource, true);
        void *address = tag_resource_cache_get_permanent_resource_root_address(&m_controller, handle);
        m_resource_address_cache.dispose_resource_address(handle, address);
        long page_index = m_resource_interface->get_optional_page_index(resource);
        long page_absolute_index = (unsigned short)page_index;
        ((s_tag_resource_page_datum *)m_page_table.m_pages->data)[page_absolute_index].io_lock_count--;
        tag_resource_cache_delete_inverse_handle(&m_inverse_page_table, page_index, handle);
        m_controller.end_tracking_resource(handle);
    }
    long handle = tag_resource_cache_make_internal_handle(resource, false);
    void *address = tag_resource_cache_get_permanent_resource_root_address(&m_controller, handle);
    m_resource_address_cache.dispose_resource_address(handle, address);
    if (m_resource_interface->any_pageable_data(resource)) {
        long page_index = m_resource_interface->get_required_page_index(resource);
        ((s_tag_resource_page_datum *)m_page_table.m_pages->data)[(unsigned short)page_index].io_lock_count--;
        tag_resource_cache_delete_inverse_handle(&m_inverse_page_table, page_index, handle);
    }
    m_controller.end_tracking_resource(handle);
}


// @address 0x21cd80 ?set_key_bits@c_tag_resource_cache_controller_demand_collector@@UAAXH@Z
void c_tag_resource_cache_controller_demand_collector::set_key_bits(int key)
{
    s_tag_resource_cache_external_access_tables *tables = m_tables;
    if (tables != 0 && key != -1) {
        s_bit_vector storage;
        storage.bits = m_demanded_resources;
        storage.count = 0xffff;
        if (tables->table != 0) {
            tag_resource_cache_get_or_add_group(tables->table, m_active_index, key);
            tag_resource_external_access_table *table = tables->table;
            int found_index[2];
            found_index[0] = -1;
            if (tag_resource_cache_find_group_handle(&table->group_hash_table, key, found_index)) {
                int index = found_index[0];
                if (index != -1)
                    tag_resource_cache_set_bits_from_range(&table->ranges, index, storage);
            }
        }
    }
}

// @address 0x21cca0 ?update_active_slot@c_tag_resource_cache_controller_demand_collector@@UAAXXZ
void c_tag_resource_cache_controller_demand_collector::update_active_slot()
{
    long active_index = m_active_index;
    if (active_index != -1)
    {
        tag_resource_cache_find_duplicate_slot(m_tables->table, active_index);
        active_index = m_active_index;
#ifdef HALO3_64BIT
        s_bit_vector key16;
        s_bit_vector key2010;
        key16.bits = m_demanded_resources;
        key16.count = 0xffff;
        key2010.bits = m_required_resources;
        key2010.count = 0xffff;
        if (active_index != -1)
            tag_resource_cache_set_bits_from_set(&m_tables->original_table->ranges, active_index, key2010, key16);
#else
        u_resource_key64 key16;
        u_resource_key64 key2010;
        key16.address = m_demanded_resources;
        key16.low_word = 0xffff;
        key2010.address = m_required_resources;
        key2010.low_word = 0xffff;
        if (active_index != -1)
            tag_resource_cache_set_bits_from_set(&m_tables->original_table->ranges, active_index, key2010.qword, key16.qword);
#endif
        unsigned long *active_bits = m_active_bits;
        long bit_index = (unsigned short)m_active_index;
        active_bits[bit_index >> 5] |= 1 << (bit_index & 31);
        m_active_index = -1;
    }
}

static __forceinline long tag_resource_cache_external_access_get_external_handle(long handle)
{
    unsigned short salt = (unsigned short)(handle >> 16);
    unsigned short external_salt = (unsigned short)~salt;
    external_salt |= 0x8000;
    unsigned long absolute_index = handle & 0xffff;
    absolute_index /= 2;
    return (external_salt << 16) | absolute_index;
}
static __forceinline long tag_resource_cache_external_access_get_internal_handle(long handle)
{
    unsigned short salt = (unsigned short)(handle >> 16);
    unsigned short internal_salt = (unsigned short)~salt;
    internal_salt |= 0x8000;
    unsigned long absolute_index = handle & 0xffff;
    absolute_index *= 2;
    return (internal_salt << 16) | absolute_index;
}
static __forceinline bool tag_resource_cache_external_access_controller_resource_valid(c_tag_resource_cache_controller *controller, long handle)
{
    s_data_array *data = controller->m_resources;
    bool valid = false;
    if (handle != -1) {
        long absolute_index = (unsigned short)handle;
        if (absolute_index < data->first_unallocated) {
            short identifier = *(short *)(data->data + data->size * absolute_index);
            if (identifier != 0 && identifier == handle >> 16)
                valid = true;
        }
    }
    return valid;
}
static __forceinline void tag_resource_cache_external_access_release_resource(c_tag_resource_cache_new *cache, long handle)
{
    c_tag_resource_cache_controller *controller = &cache->m_controller;
    ::c_tag_resource_cache_controller::release_resource_page_references_for_cache(controller, handle,
        (c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
    long absolute_index = (unsigned short)handle;
    controller->m_pending_release.m_storage[absolute_index >> 5] &= ~(1 << (absolute_index & 31));
}

// @address 0x21d6c0 tag_resource_cache_new_release_page
extern "C" void tag_resource_cache_new_release_page(c_tag_resource_cache_new *cache, long page_handle)
{
    long count = datum_get<s_tag_resource_cache_external_access_slot>(cache->m_inverse_page_table.table, page_handle)->count;
    for (long index = 0; index < count; ++index) {
        s_tag_resource_cache_external_access_slot_owner *inverse_table = &cache->m_inverse_page_table;
        long handle = inverse_table->list.begin()[datum_get<s_tag_resource_cache_external_access_slot>(inverse_table->table, page_handle)->first
            + index];
        if (!tag_resource_cache_external_access_handle_is_optional(handle)) {
            long internal_handle = tag_resource_cache_external_access_get_internal_handle(tag_resource_cache_external_access_get_external_handle(handle));
            if (tag_resource_cache_external_access_controller_resource_valid(&cache->m_controller, internal_handle))
                ((c_tag_resource_cache_published_interface *)cache)->prepare_for_delete(internal_handle);
        }
        tag_resource_cache_external_access_release_resource(cache, handle);
    }
}

// @address 0x21d818 tag_resource_cache_new_handle_page_move
extern "C" void tag_resource_cache_new_handle_page_move(c_tag_resource_cache_new *cache, long page_handle)
{
    long count = datum_get<s_tag_resource_cache_external_access_slot>(cache->m_inverse_page_table.table, page_handle)->count;
    for (long index = 0; index < count; ++index) {
        s_tag_resource_cache_external_access_slot_owner *inverse_table = &cache->m_inverse_page_table;
        long handle = inverse_table->list.begin()[datum_get<s_tag_resource_cache_external_access_slot>(inverse_table->table, page_handle)->first
            + index];
        if (!tag_resource_cache_external_access_handle_is_optional(handle)) {
            long internal_handle = tag_resource_cache_external_access_get_internal_handle(tag_resource_cache_external_access_get_external_handle(handle));
            if (tag_resource_cache_external_access_controller_resource_valid(&cache->m_controller, internal_handle))
                ::c_tag_resource_cache_controller::release_resource_page_references_for_cache((c_tag_resource_cache_controller *)&cache->m_controller,
                    internal_handle, (c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
        }
        ::c_tag_resource_cache_controller::release_resource_page_references_for_cache((c_tag_resource_cache_controller *)&cache->m_controller, handle,
            (c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
    }
}
extern "C" {
// match: noalias declaration preserves caller live registers
// match: noalias declaration preserves caller live registers
}
// @address 0x21da90 tag_resource_cache_new_commit_dirty
extern "C" void tag_resource_cache_new_commit_dirty(c_tag_resource_cache_new *cache)
{
    if (!cache->m_loaded_query->is_loaded())
        return;
    c_tag_resource_controller_page_table_io_listener *listener = (c_tag_resource_controller_page_table_io_listener *)&cache->m_page_io_listener_owner;
    cache->m_page_table.cancel_async_and_block_until_async_done(listener);
    cache->m_resource_thread_access_lock.lock_for_write();
    cache->m_thread_access_locked = true;
    cache->revoke_active_access_cache();
    cache->release_unlocked_resources(true);
    tag_resource_cache_new_update_pages(cache, true);
    cache->m_controller.update_required_resources((c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
    cache->publish_necessary_resources(false, true);
    cache->m_resource_thread_access_lock.unlock_for_write();
    cache->m_thread_access_locked = false;
    cache->verify_all_required_resources_available();
}
// @address 0x21db60 tag_resource_cache_new_update_pages
extern "C" void tag_resource_cache_new_update_pages(c_tag_resource_cache_new *cache, bool force)
{
    cache->m_controller.adopt_necessary_resources();
    cache->m_controller.orphan_unnecessary_unlocked_resources((c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
    if (cache->m_resources_valid && (!force || cache->m_incremental_publishing))
        tag_resource_page_table_dispose_reserved_pages(&cache->m_page_table);
    tag_resource_page_table_compact_pages(&cache->m_page_table);
    cache->attempt_to_resize(force);
}
// @address 0x21dbd8 tag_resource_cache_new_update_zones
extern "C" void tag_resource_cache_new_update_zones(c_tag_resource_cache_new *cache, long unused, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver)
{
    if (!cache->m_loaded_query->is_loaded())
        return;
    c_tag_resource_controller_page_table_io_listener *listener = (c_tag_resource_controller_page_table_io_listener *)&cache->m_page_io_listener_owner;
    cache->m_page_table.cancel_async_and_block_until_async_done(listener);
    cache->m_resource_thread_access_lock.lock_for_write();
    cache->m_thread_access_locked = true;
    cache->revoke_active_access_cache();
    cache->release_unlocked_resources(true);
    tag_resource_cache_new_update_pages(cache, 0);
    bool success;
    if (!synchronous)
        success = tag_resource_page_table_read_required_pages(&cache->m_page_table, listener, false, synchronous, flag_receiver);
    else
        success = cache->m_page_table.load_required_pages(listener, false, synchronous, flag_receiver);
    if (success)
    {
        cache->m_controller.update_required_resources((c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
        cache->publish_necessary_resources(false, true);
    }
    cache->m_resource_thread_access_lock.unlock_for_write();
    cache->m_thread_access_locked = false;
    if (success)
        cache->verify_all_required_resources_available();
}

// match: noalias declaration preserves caller live registers
// @address 0x21cf58 ?attempt_to_resize@c_tag_resource_cache_new@@AAAX_N@Z
void c_tag_resource_cache_new::attempt_to_resize(bool force)
{
    unsigned long necessary_size = 0;
    unsigned long reading_size = 0;
    tag_resource_page_table_compute_resize_sizes(&m_page_table, &necessary_size, &reading_size);
    unsigned long full_size;
    if (m_resize_policy_enabled) {
        if (!m_allow_optional || (!m_controller.m_can_predict_optional && !force && !m_incremental_publishing))
            full_size = false;
        else
            full_size = true;
    } else {
        full_size = force;
    }
    unsigned long extra_size = reading_size;
    if (!(unsigned char)full_size)
        extra_size = 0;
    bool result = m_page_table.m_lruv_cache.resize(necessary_size, extra_size, m_resize_flags,
        m_controller.m_can_predict_optional, &m_full_size);
    m_resize_succeeded = result;
    if (!result && m_allow_optional) {
        m_full_size = false;
        m_page_table.cancel_async_and_block_until_async_done((c_tag_resource_controller_page_table_io_listener *)&m_page_io_listener_owner);
        tag_resource_page_table_dispose_reserved_pages(&m_page_table);
        tag_resource_page_table_compact_pages(&m_page_table);
        m_page_table.m_lruv_cache.resize(necessary_size, 0, 0, m_controller.m_can_predict_optional, 0);
    }
    bool allow_full;
    if (!force && (m_controller.m_can_predict_optional || !m_incremental_publishing))
        allow_full = false;
    else
        allow_full = true;
    bool full = m_full_size && allow_full;
    m_controller.m_can_load_all_desired_optional = full;
}

// match: noalias declaration preserves caller live registers
static __forceinline void tag_resource_cache_external_access_controller_update_active_resources(
    c_tag_resource_cache_controller *controller, bool *out_changed, void *address_cache)
{
    *out_changed = false;
    tag_resource_cache_reconcile_published_resources(controller, out_changed, address_cache);
    if (controller->m_prediction_pending)
        controller->m_prediction_pending = false;
    ++controller->m_prediction_state;
    if (*out_changed)
        tag_resource_cache_verify_resources((c_tag_resource_cache_controller *)controller,
            (s_tag_resource_cache_external_access_verify_reference *)address_cache);
}

// @address 0x21dd50 ?update_active_resources@c_tag_resource_cache_new@@AAA_NXZ
bool c_tag_resource_cache_new::update_active_resources()
{
    bool result;
    tag_resource_cache_external_access_controller_update_active_resources(&m_controller, &result, &m_resource_address_cache);
    return result;
}


static __forceinline unsigned long tag_resource_cache_external_access_lruv_get_used_size(s_lruv_cache *cache)
{
    long used_pages = 0;
    if (cache->first_block_index != -1)
    {
        s_data_array *data = (s_data_array *)cache->header.block_data;
        s_lruv_block *block = datum_get<s_lruv_block>(data, cache->last_block_index);
        used_pages = block->page_index + block->page_count;
    }
    return used_pages << cache->page_size_bits;
}


// @address 0x21ddc0 tag_resource_cache_new_handle_space_demand
extern "C" void tag_resource_cache_new_handle_space_demand(c_tag_resource_cache_new *cache, unsigned long size)
{
    c_tag_resource_page_table *table = &cache->m_page_table;
    c_tag_resource_controller_page_table_io_listener *listener = (c_tag_resource_controller_page_table_io_listener *)&cache->m_page_io_listener_owner;
    table->cancel_async_and_block_until_async_done(listener);
    cache->m_resource_thread_access_lock.lock_for_write();
    cache->m_thread_access_locked = true;
    cache->revoke_active_access_cache();
    cache->release_unlocked_resources(true);
    tag_resource_page_table_dispose_minimum_size(table, size);
    tag_resource_page_table_compact_pages(table);
    tag_resource_lruv_cache_allocate_buffer(&table->m_lruv_cache, tag_resource_cache_external_access_lruv_get_used_size(table->m_lruv_cache.m_cache));
    cache->m_controller.update_required_resources((c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
    cache->publish_necessary_resources(false, true);
    cache->m_resource_thread_access_lock.unlock_for_write();
    cache->m_thread_access_locked = false;
}

// @address 0x3bebc ??_7c_tag_resource_error_receiver@@6B@
// @address 0x850d0 tag_resource_cache_new_update
extern "C" void tag_resource_cache_new_update(c_tag_resource_cache_new *cache)
{
    c_tag_resource_controller_page_table_io_listener *listener = (c_tag_resource_controller_page_table_io_listener *)&cache->m_page_io_listener_owner;
    cache->m_page_table.update_async_requests(listener);
    cache->m_thread_access_locked = true;
    cache->release_unlocked_resources(false);
    cache->m_thread_access_locked = false;
    tag_resource_page_table_update_next_page(&cache->m_page_table, listener);
    if (cache->m_resources_changed) {
        if ((!cache->m_resize_policy_enabled || !cache->m_incremental_publishing)
            && cache->m_loaded_query->is_loaded()) {
            if (!cache->m_incremental_publishing) {
                cache->m_resource_thread_access_lock.lock_for_write();
                cache->m_thread_access_locked = true;
                cache->revoke_active_access_cache();
                cache->release_unlocked_resources(true);
                cache->attempt_to_resize(true);
                tag_resource_page_table_compact_pages(&cache->m_page_table);
                c_tag_resource_error_receiver receiver;
                tag_resource_page_table_read_required_pages(&cache->m_page_table, listener, cache->m_full_size, false, &receiver);
                cache->m_controller.update_required_resources((c_tag_resource_address_cache_control_interface *)&cache->m_resource_address_cache);
                cache->update_active_resources();
                cache->publish_necessary_resources(false, true);
                cache->m_resource_thread_access_lock.unlock_for_write();
                receiver.finished = true;
                cache->m_thread_access_locked = false;
                main_time_reset();
            } else {
                cache->attempt_to_resize(true);
            }
        }
        cache->m_resources_changed = false;
    } else if (cache->m_incremental_publishing && cache->update_active_resources()) {
        cache->m_thread_access_locked = true;
        cache->release_unlocked_resources(false);
        cache->publish_necessary_resources(false, false);
        cache->m_thread_access_locked = false;
        cache->m_flag303 = false;
    }
    lruv_idle(cache->m_page_table.m_lruv_cache.m_cache);
}

static __forceinline long tag_resource_cache_external_access_group_new(s_tag_resource_cache_external_access_state *state)
{
    long handle = -1;
    if (state->group_count != 0x9000) {
        t_tag_resource_cache_external_access_list<s_tag_resource_cache_external_access_group, 0x9000> *groups =
            (t_tag_resource_cache_external_access_list<s_tag_resource_cache_external_access_group, 0x9000> *)state->groups;
        long index = groups->count++;
        s_tag_resource_cache_external_access_group *group = &state->groups[index];
        if (state->group_identifier == 0xffff)
            state->group_identifier = 0x8000;
        unsigned short salt = state->group_identifier;
        long identifier = (short)salt;
        handle = (identifier << 16) | index;
        state->group_identifier = (unsigned short)(identifier + 1);
        group->identifier = salt;
        group->count = 0;
        group->first = -1;
    } else {
        state->group_overflow = 1;
        if (!state->group_overflow_reported)
            state->group_overflow_reported = 1;
    }
    return handle;
}
// @address 0x21e648 tag_resource_cache_get_or_add_group
void tag_resource_cache_get_or_add_group(s_tag_resource_cache_external_access_state *state, long slot_index, long key)
{
    s_tag_resource_cache_group_hash_table *table = &state->group_hash_table;
    long result = -1;
    if (!tag_resource_cache_find_group_handle(table, key, (int *)&result) && state->iterator) {
        long handle = tag_resource_cache_external_access_group_new(state);
        if (handle != -1) {
            s_tag_resource_cache_external_access_group *group = &state->groups[(unsigned short)handle];
            group->first = state->key_list.count;
            group->count = 0;
            s_tag_resource_cache_external_access_handle handle_callback(state, slot_index);
            bool success = state->iterator->iterate(key, handle, &handle_callback);
            state->pairs[0] = tag_resource_cache_external_access_list_pair((t_tag_resource_cache_external_access_list<long, 0x4000> *)state->key_list.data);
            state->pairs[1] = tag_resource_cache_external_access_list_pair(
                (t_tag_resource_cache_external_access_list<s_tag_resource_cache_external_access_group, 0x9000> *)state->groups);
            if (success) {
                long existing = tag_resource_cache_find_duplicate_group(state, handle);
                if (existing != handle) {
                    state->key_list.count = state->groups[(unsigned short)handle].first;
                    state->group_count = (unsigned short)handle;
                    s_tag_resource_cache_external_access_pair *pairs = state->pairs;
                    pairs[0] = tag_resource_cache_external_access_list_pair((t_tag_resource_cache_external_access_list<long, 0x4000> *)state->key_list.data);
                    s_tag_resource_cache_external_access_pair slot_pair = tag_resource_cache_external_access_list_pair(
                        (t_tag_resource_cache_external_access_list<s_tag_resource_cache_external_access_allocation_slot, 0x2000> *)&state
                            ->slot_list);
                    s_tag_resource_cache_external_access_pair array_pair = tag_resource_cache_external_access_list_pair(
                        &state->value_list);
                    pairs[3] = slot_pair;
                    pairs[2] = array_pair;
                    handle = existing;
                }
            }
            if (success) {
                long entry = tag_resource_cache_new_group_entry(table, key, 0, 0);
                if (entry != -1) {
                    table->entries[entry].value = handle;
                    result = handle;
                }
            }
        }
    }
    if (result != -1 && slot_index != -1) {
        s_tag_resource_cache_external_access_allocation_slot *slot = &state->slot_list.slots[slot_index];
        tag_resource_cache_find_or_add_value(state, slot->generation, &slot->owner, result);
    }
}

static __forceinline bool tag_resource_external_access_test_bit_short(const unsigned long *bits, int bit) {
    return ((1 << (bit & 31)) & bits[bit >> 5]) != 0;
}
static __forceinline s_tag_resource_index_buffer tag_resource_external_access_index_buffer(const s_ranges *ranges)
{
    return ranges->index_buffer;
}

// @address 0x140c38 ?test_active_bit@c_tag_resource_cache_controller_demand_collector@@UAAEJ@Z
unsigned char c_tag_resource_cache_controller_demand_collector::test_active_bit(long index)
{
    unsigned short bit = (unsigned short)index;
    unsigned char result = tag_resource_external_access_test_bit(m_active_bits, bit);
    if (result == 0) {
        s_tag_resource_cache_external_access_published_table *table = m_tables->original_table;
#ifdef HALO3_64BIT
        s_bit_vector key16;
        s_bit_vector key2010;
        key16.bits = m_demanded_resources;
        key16.count = 0xffff;
        key2010.bits = m_required_resources;
        key2010.count = 0xffff;
        result = 0;
        if (index != -1) {
            result = tag_resource_cache_set_bits_from_set(&table->ranges, index, key2010, key16);
        }
#else
        u_resource_key64 key16;
        u_resource_key64 key2010;
        key16.address = m_demanded_resources;
        key16.low_word = 0xffff;
        key2010.address = m_required_resources;
        key2010.low_word = 0xffff;
        result = 0;
        if (index != -1) {
            result = tag_resource_cache_set_bits_from_set(&table->ranges, index, key2010.qword, key16.qword);
        }
#endif
        if (result != 0) {
            tag_resource_external_access_set_bit(m_active_bits, bit);
        }
    }
    return result;
}

// @address 0x21deb0 tag_resource_cache_set_bits_from_set
#ifdef HALO3_64BIT
extern "C" unsigned char tag_resource_cache_set_bits_from_set(s_ranges *ranges, long index, s_bit_vector first, s_bit_vector second)
{
    struct { s_bit_vector vector; } first_bits = { first };
    struct { s_bit_vector vector; } second_bits = { second };
#else
extern "C" unsigned char tag_resource_cache_set_bits_from_set(s_ranges *ranges, long index, __int64 first, __int64 second)
{
    u_bit_vector_argument first_bits;
    u_bit_vector_argument second_bits;
    first_bits.qword = first;
    second_bits.qword = second;
#endif
    unsigned char result = false;
    if ((unsigned long)index < ranges->set_count) {
        s_range_set *set = &ranges->sets[index & 0x1fffffff];
        long first_range = set->first_range;
        if (first_range != -1) {
            long range_count = set->range_count;
            for (long i = 0; i < range_count; i++) {
                int range_index = ranges->range_indices[first_range + i];
                long bit = (unsigned short)range_index;
                if (!tag_resource_external_access_test_bit_short(first_bits.vector.bits, (unsigned short)range_index)) {
                    tag_resource_cache_set_bits_from_range(ranges, range_index, second_bits.vector);
                    tag_resource_external_access_set_bit(first_bits.vector.bits, bit);
                }
            }
            long index_count = set->index_count;
            long first_index = set->first_index;
            for (long i = 0; i < index_count; i++) {
                long bit = (unsigned short)tag_resource_external_access_index_buffer(ranges).elements[first_index + i];
                tag_resource_external_access_set_bit(second_bits.vector.bits, bit);
            }
            result = true;
        }
    }
    return result;
}
