// blamlibxbox/tag_resource_cache_thread_lock.obj: from the records 0x85708 0x88450 0x88ef8 0x2aa838 0x2aa950 0x2aa9d8 0x2aaa40 0x2aaab8
// @flags /TP /O2 /Oc

#include "cache/tag_resource_cache_thread_lock_externs.h"
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <string.h>
#include "cseries/pointer_utilities.h"
#include "multithreading/synchronization.h"
#include "text/font_cache.h"
#include "multithreading/read_write_lock.h"
#include "multithreading/critical_section_scope.h"
#include "memory/raw_storage_buffer.h"
#include "memory/wrapped_array_allocation_description.h"
#include <new>

#include "multithreading/synchronized_value.h"
#include "cache/tag_resource_cache_thread_lock.h"


void c_tag_resource_cache_thread_lock_lock_freeish::release_unlocked_access_cache(c_tag_resource_cache_file_access_cache *cache)
{
    c_synchronized_access_cache *owned = get_owned_access_cache(cache);
    owned->reset();
    queue_access_cache(owned, &m_available_caches);
    --m_reference_count;
}


// @address 0x88450 ?publish_access_cache@c_tag_resource_cache_thread_lock_lock_freeish@@QAAXPAVc_tag_resource_cache_file_access_cache@@@Z
void c_tag_resource_cache_thread_lock_lock_freeish::publish_access_cache(c_tag_resource_cache_file_access_cache *cache)
{
    c_synchronized_access_cache *synchronized_access_cache = get_owned_access_cache(cache);
    t_event_queue<s_access_cache_available_event,3>::s_entry *on_release_event = allocate_on_release_event(synchronized_access_cache);
    synchronized_access_cache->acquire_for_publishing(on_release_event);
    ++m_reference_count;
    c_critical_section_scope critical_section(47);
    c_synchronized_access_cache *previous_access_cache = publish_cache(synchronized_access_cache);
    release_access_cache_internal(previous_access_cache);
}


// @address 0x88ef8 ?release_access_cache@c_tag_resource_cache_thread_lock_lock_freeish@@QAAXPAVc_tag_resource_cache_file_access_cache@@@Z
void c_tag_resource_cache_thread_lock_lock_freeish::release_access_cache(c_tag_resource_cache_file_access_cache *cache)
{
    if (cache)
    {
        c_synchronized_access_cache *owned = get_owned_access_cache(cache);
        release_access_cache_internal(owned);
    }
    m_access_lock.read_unlock();
}

// @address 0x2aa950 ?dequeue_access_cache@c_tag_resource_cache_thread_lock_lock_freeish@@SAPAVc_synchronized_access_cache@1@PAV?$t_event_queue@Us_access_cache_available_event@c_tag_resource_cache_thread_lock_lock_freeish@@$02@@_N@Z
c_tag_resource_cache_thread_lock_lock_freeish::c_synchronized_access_cache *
c_tag_resource_cache_thread_lock_lock_freeish::dequeue_access_cache(t_access_cache_event_queue *queue, bool block)
{
    c_synchronized_access_cache *result = 0;
    t_access_cache_event_queue::s_entry *event = queue->get_top_event_queue_member();
    if (event)
    {
        if (event->event.cache->wait_on_dpc_event(block))
        {
            result = event->event.cache;
            queue->release_top_event_queue_member(event);
        }
        else
        {
            queue->requeue_top_event_queue_member(event);
        }
    }
    return result;
}

bool c_tag_resource_cache_thread_lock_lock_freeish::c_synchronized_access_cache::wait_on_dpc_event(bool block)
{
    return m_dpc_event.wait_on_dpc_event(block);
}

template<> void t_event_queue<c_tag_resource_cache_thread_lock_lock_freeish::s_access_cache_available_event,3>::initialize(long starting_entry_limit)
{
    // match: 0x2aaa40: value range assumption guides branch lowering
    __assume(starting_entry_limit == 3);
    m_pending = 0;
    m_free = 0;
    for (unsigned long index = 0; index < 3; ++index)
        m_entries[index].next = 0;
    m_initialized = true;
    m_entry_limit = starting_entry_limit;
    if (m_entry_limit != 0)
    {
        long last_entry_index = m_entry_limit - 1;
        for (long index = last_entry_index; index >= 0; --index)
        {
            __lwsync();
            InterlockedPushEntrySList((slist_header_ptr)&m_free, (slist_entry_ptr)&m_entries[index]);
        }
    }
}

template<> void t_event_queue<c_tag_resource_cache_thread_lock_lock_freeish::s_access_cache_available_event,3>::reset()
{
    s_entry *event = acquire_event_queue_chain();
    while (event)
        event = release_top_event_queue_member(event);
}

// @address 0x4163c ??_7?$c_wrapped_array_allocation_description@PAX@@6B@
// @address 0x4163c ??_7?$c_wrapped_array_allocation_description@K@@6B@
// @address 0x2aeee0 ?get_unaligned_size@?$c_wrapped_array_allocation_description@PAX@@UBAKXZ
// @address 0x2ffad0 ?assign_storage@?$c_wrapped_array_allocation_description@PAX@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2aeee0 ?get_unaligned_size@?$c_wrapped_array_allocation_description@K@@UBAKXZ
// @address 0x2ffad0 ?assign_storage@?$c_wrapped_array_allocation_description@K@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
template <>
unsigned long c_wrapped_array_allocation_description<void *>::get_unaligned_size() const
{
    return m_element_count * sizeof(void *) + __alignof(void *) - 1;
}

template <>
void c_wrapped_array_allocation_description<void *>::assign_storage(c_raw_storage_buffer_iterator &iterator) const
{
    iterator.set_wrapped_array<void *>(*m_wrapped_array, m_element_count);
}

template <>
unsigned long c_wrapped_array_allocation_description<unsigned long>::get_unaligned_size() const
{
    return m_element_count * sizeof(unsigned long) + __alignof(unsigned long) - 1;
}

template <>
void c_wrapped_array_allocation_description<unsigned long>::assign_storage(c_raw_storage_buffer_iterator &iterator) const
{
    iterator.set_wrapped_array<unsigned long>(*m_wrapped_array, m_element_count);
}

__forceinline void c_tag_resource_cache_file_access_cache::initialize(unsigned long count, c_allocation_base *allocator)
{
    c_wrapped_array_no_init<s_allocation_description_holder> list_storage;
    c_wrapped_array_allocation_description<void *> values_description(m_resource_values, count);
    c_wrapped_array_allocation_description<long> handles_description(m_resource_handles, count);
    c_wrapped_array_allocation_description<__int64> pages_description(m_resource_pages, count);
    s_allocation_description_holder descriptions[] = {
        { values_description },
        { handles_description },
        { pages_description },
        // match: 0x2aa838: building the flags description in place schedules its target store after holders[2]
        { c_wrapped_array_allocation_description<unsigned long>(m_resource_flags, count) }
    };
    // match: 0x2aa838: storing the allocator after the descriptions keeps the loop pointer based at m_resource_handles
    m_allocator = allocator;
    c_wrapped_array<s_allocation_description_holder> *list =
        new (&list_storage) c_wrapped_array<s_allocation_description_holder>(descriptions);
    allocate_list_contiguous(allocator, *list, "tag resource access cache", &m_allocation, 0);
}

// @address 0x2aa838 ?initialize_for_new_map@c_tag_resource_cache_thread_lock_lock_freeish@@QAAXKPAVc_allocation_base@@@Z
void c_tag_resource_cache_thread_lock_lock_freeish::initialize_for_new_map(unsigned long count, c_allocation_base *allocator)
{
    m_available_caches.reset();
    m_on_release_events.reset();
    for (long index = 0; index < 3; index++)
    {
        c_synchronized_access_cache *cache = &m_access_cache_storage[index];
        cache->initialize(count, allocator);
        ++m_reference_count;
        release_unlocked_access_cache(cache);
    }
}
