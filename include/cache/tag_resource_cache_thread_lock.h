#ifndef CACHE_TAG_RESOURCE_CACHE_THREAD_LOCK_H
#define CACHE_TAG_RESOURCE_CACHE_THREAD_LOCK_H
#include "memory/wrapped_arrays.h"
#include "multithreading/synchronized_value.h"
#include "rasterizer/rasterizer.h"
#include "multithreading/read_write_lock.h"
#include <stddef.h>

class c_allocation_base;
#if defined(_WIN64)
struct _SLIST_HEADER;
struct _SLIST_ENTRY;
typedef _SLIST_HEADER *slist_header_ptr;
typedef _SLIST_ENTRY *slist_entry_ptr;
#else
union _SLIST_HEADER;
struct _SINGLE_LIST_ENTRY;
typedef _SLIST_HEADER *slist_header_ptr;
typedef _SINGLE_LIST_ENTRY *slist_entry_ptr;
#endif

#ifndef _XTL_
extern "C"
{
    slist_entry_ptr __stdcall InterlockedPopEntrySList(slist_header_ptr);
    slist_entry_ptr __stdcall InterlockedFlushSList(slist_header_ptr);
    slist_entry_ptr __stdcall InterlockedPushEntrySList(slist_header_ptr, slist_entry_ptr);
}
#endif
extern "C"
{
    unsigned long __stdcall D3DDevice_InsertFence(D3DDevice *);
    void __stdcall D3DDevice_BlockOnFence(unsigned long);
    int __stdcall D3DDevice_IsFencePending(unsigned long);
}
class c_tag_resource_cache_dpc_event
{
public:
    c_interlocked_int64 m_event;
    __forceinline void allocate_dpc_event()
    {
        long event = D3DDevice_InsertFence(c_rasterizer::get_device());
        m_event.set((unsigned long)event | ((unsigned __int64)1 << 32));
    }
    __forceinline bool wait_on_dpc_event(bool block)
    {
        __int64 event = m_event.peek();
        unsigned long dpc_event = (long)(event & 0xFFFFFFFF);
        bool event_valid = (long)(event >> 32) != 0;
        bool result;
        if (event_valid)
        {
            if (block)
            {
                D3DDevice_BlockOnFence(dpc_event);
                result = true;
            }
            else
            {
                result = !(D3DDevice_IsFencePending(dpc_event) != 0);
            }
        }
        else
        {
            result = true;
        }
        return result;
    }
};

class c_tag_resource_cache_file_access_cache
{
public:
    __forceinline c_tag_resource_cache_file_access_cache() {}
    void initialize(unsigned long count, c_allocation_base *allocator);

    __forceinline void clear()
    {
        for (void **element = m_resource_values.begin(), **last = m_resource_values.get_element(m_resource_values.count());
            element != last; element++)
            *element = 0;
        for (long *element = m_resource_handles.begin(), *last = m_resource_handles.get_element(m_resource_handles.count());
            element != last; element++)
            *element = -1;
        for (__int64 *element = m_resource_pages.begin(), *last = m_resource_pages.get_element(m_resource_pages.count());
            element != last; element++)
            *element = -1;
        for (unsigned long *element = m_resource_flags.begin(), *last = m_resource_flags.get_element(m_resource_flags.count());
            element != last; element++)
            *element = 0;
    }
    __forceinline void set_resource(long handle, void *address, __int64 page) {
        long index = (unsigned short)handle;
        m_resource_values.begin()[index] = address;
        m_resource_handles.begin()[index] = handle;
        m_resource_pages.begin()[index] = page;
    }
    __forceinline void set_resource_flag(long handle, long flag) { m_resource_flags.begin()[(unsigned short)handle] |= 1 << flag; }
    c_wrapped_array<__int64> m_resource_pages;
    c_wrapped_array<void *> m_resource_values;
    c_wrapped_array<long> m_resource_handles;
    c_wrapped_array<unsigned long> m_resource_flags;
    c_allocation_base *m_allocator;
    void *m_allocation;
};


template<class t_event, int k_count> class t_event_queue
{
public:
    struct s_entry
    {
        s_entry *next;
        t_event event;
        // SLIST entries occupy sixteen bytes within the packed queue storage.
        long m_slist_reserved[2];
    };
    s_entry m_entries[k_count];
    __int64 m_pending;
    __int64 m_free;
    bool m_initialized;
    long m_entry_limit;
    const char *m_name;
    __forceinline t_event_queue() : m_initialized(false), m_entry_limit(0), m_name(NULL) {}
    void initialize(long starting_entry_limit);
    void reset();
    __forceinline s_entry *get_top_event_queue_member()
    {
        s_entry *event = (s_entry *)InterlockedPopEntrySList((slist_header_ptr)&m_pending);
        __lwsync();
        return event;
    }
    __forceinline s_entry *release_top_event_queue_member(s_entry *event)
    {
        s_entry *next = event->next;
        __lwsync();
        InterlockedPushEntrySList((slist_header_ptr)&m_free, (slist_entry_ptr)event);
        return next;
    }
    __forceinline s_entry *requeue_top_event_queue_member(s_entry *event)
    {
        s_entry *next = event->next;
        __lwsync();
        InterlockedPushEntrySList((slist_header_ptr)&m_pending, (slist_entry_ptr)event);
        return next;
    }
    __forceinline s_entry *acquire_event_queue_chain()
    {
        s_entry *chain = (s_entry *)InterlockedFlushSList((slist_header_ptr)&m_pending);
        __lwsync();
        return chain;
    }
    __forceinline s_entry *acquire_event_queue_member()
    {
        s_entry *event = (s_entry *)InterlockedPopEntrySList((slist_header_ptr)&m_free);
        __lwsync();
        return event;
    }
    __forceinline void queue_event(s_entry *event)
    {
        __lwsync();
        InterlockedPushEntrySList((slist_header_ptr)&m_pending, (slist_entry_ptr)event);
    }
};

class c_allocation_base;
#pragma pack(push, 8)
class c_tag_resource_cache_thread_lock_lock_freeish
{
public:
    class c_synchronized_access_cache;
    struct s_access_cache_available_event
    {
        c_synchronized_access_cache *cache;
    };
    class c_synchronized_access_cache : public c_tag_resource_cache_file_access_cache
    {
    public:
        c_tag_resource_cache_dpc_event m_dpc_event;
        long m_reference_count;
        t_event_queue<s_access_cache_available_event,3>::s_entry *m_on_release_event;
        __forceinline c_synchronized_access_cache() : m_reference_count(0) {}
        __forceinline void acquire_for_publishing(t_event_queue<s_access_cache_available_event,3>::s_entry *event)
        {
            m_on_release_event = event;
            _InterlockedIncrement(&m_reference_count);
        }
        bool wait_on_dpc_event(bool block);
        __forceinline void reset()
        {
            m_on_release_event = 0;
            m_dpc_event.m_event.set(0);
        }
    };
    __forceinline c_tag_resource_cache_thread_lock_lock_freeish() : m_published_cache(NULL), m_reference_count(0) {}
    c_synchronized_access_cache *m_published_cache;
    // Keep the event queues on separate 32-byte cache-line boundaries.
    __declspec(align(32)) t_event_queue<s_access_cache_available_event, 3> m_available_caches;
    __declspec(align(32)) t_event_queue<s_access_cache_available_event, 3> m_on_release_events;
    __declspec(align(32)) c_synchronized_access_cache m_access_cache_storage[3];
    long m_reference_count;
    c_read_write_lock m_access_lock;
    c_interlocked_int64 m_counter;
    bool m_locked_for_blocking_update;
    __forceinline void lock_for_blocking_update();
    __forceinline void unlock_for_blocking_update();
    __forceinline void lock_for_write() { m_access_lock.write_lock(); m_locked_for_blocking_update = true; }
    __forceinline void unlock_for_write() { m_access_lock.write_unlock(); m_locked_for_blocking_update = false; }
    __forceinline c_tag_resource_cache_file_access_cache *acquire_unlocked_access_cache(bool block)
    {
        return (c_tag_resource_cache_file_access_cache *)dequeue_access_cache(&m_on_release_events, block);
    }
    __forceinline c_tag_resource_cache_file_access_cache *acquire_available_access_cache(bool block)
    {
        return (c_tag_resource_cache_file_access_cache *)dequeue_access_cache(&m_available_caches, block);
    }
    __forceinline void flush_all_access_caches() { c_synchronized_access_cache *cache = publish_cache(0); release_access_cache_internal(cache); }
    __forceinline void release_access_cache_access(c_tag_resource_cache_file_access_cache *cache, bool allocate_event) {
        if (cache && allocate_event) { c_synchronized_access_cache *owned = get_owned_access_cache(cache); owned->m_dpc_event.allocate_dpc_event(); }
    }
    typedef t_event_queue<s_access_cache_available_event, 3> t_access_cache_event_queue;
    void release_access_cache(c_tag_resource_cache_file_access_cache *cache);
    static c_synchronized_access_cache *dequeue_access_cache(t_access_cache_event_queue *queue, bool block);
    void publish_access_cache(c_tag_resource_cache_file_access_cache *cache);
    __forceinline t_event_queue<s_access_cache_available_event,3>::s_entry *allocate_on_release_event(c_synchronized_access_cache *cache)
    {
        t_event_queue<s_access_cache_available_event,3>::s_entry *event = m_on_release_events.acquire_event_queue_member();
        event->event.cache = cache;
        return event;
    }
    __forceinline c_synchronized_access_cache *publish_cache(c_synchronized_access_cache *cache)
    {
        __lwsync();
#if defined(__clang__)
        return __atomic_exchange_n(&m_published_cache, cache, __ATOMIC_SEQ_CST);
#else
        return (c_synchronized_access_cache *)_InterlockedExchange((long *)&m_published_cache, (long)cache);
#endif
    }
    __forceinline void release_access_cache_internal(c_synchronized_access_cache *cache)
    {
        if (cache) {
            if (_InterlockedDecrement(&cache->m_reference_count) == 0) {
                t_event_queue<s_access_cache_available_event,3>::s_entry *event = cache->m_on_release_event;
                if (event)
                    m_on_release_events.queue_event(event);
            }
        }
    }
    void release_unlocked_access_cache(c_tag_resource_cache_file_access_cache *cache);
    void initialize_for_new_map(unsigned long count, c_allocation_base *allocator);
    __forceinline c_synchronized_access_cache *get_owned_access_cache(c_tag_resource_cache_file_access_cache *cache)
    {
        c_synchronized_access_cache *result = 0;
        if (cache)
        {
            for (long index = 0; index < 3; ++index)
            {
                c_tag_resource_cache_file_access_cache *owned = &m_access_cache_storage[index];
                if (owned == cache)
                {
                    result = (c_synchronized_access_cache *)owned;
                    break;
                }
            }
        }
        return result;
    }
    static __forceinline void queue_access_cache(c_synchronized_access_cache *cache, t_event_queue<s_access_cache_available_event,3> *queue)
    {
        t_event_queue<s_access_cache_available_event,3>::s_entry *event = queue->acquire_event_queue_member();
        event->event.cache = cache;
        queue->queue_event(event);
    }
};

#pragma pack(pop)
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char resource_lock_available_queue_offset[
    (sizeof(void *) != 4 || (offsetof(c_tag_resource_cache_thread_lock_lock_freeish, m_available_caches) == 32)) ? 1 : -1];
typedef char resource_lock_release_queue_offset[
    (sizeof(void *) != 4 || (offsetof(c_tag_resource_cache_thread_lock_lock_freeish, m_on_release_events) == 128)) ? 1 : -1];
typedef char resource_lock_queue_size[
    (sizeof(void *) != 4 ||
        (sizeof(c_tag_resource_cache_thread_lock_lock_freeish::t_access_cache_event_queue) == 80)) ? 1 : -1];
typedef char resource_lock_storage_offset[
    (sizeof(void *) != 4 || (offsetof(c_tag_resource_cache_thread_lock_lock_freeish, m_access_cache_storage) == 224)) ? 1 : -1];
typedef char resource_lock_size[(sizeof(void *) != 4 || (sizeof(c_tag_resource_cache_thread_lock_lock_freeish) == 448)) ? 1 : -1];
typedef char resource_access_cache_size[
    (sizeof(void *) != 4 || (sizeof(c_tag_resource_cache_file_access_cache) == 40)) ? 1 : -1];
typedef char resource_access_cache_stride[
    (sizeof(void *) != 4 ||
        (sizeof(c_tag_resource_cache_thread_lock_lock_freeish::c_synchronized_access_cache) == 56)) ? 1 : -1];
#endif
#endif
