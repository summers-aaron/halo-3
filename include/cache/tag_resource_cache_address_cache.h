#ifndef CACHE_TAG_RESOURCE_CACHE_ADDRESS_CACHE_H
#define CACHE_TAG_RESOURCE_CACHE_ADDRESS_CACHE_H
#include "cache/tag_resource_cache_thread_lock.h"
#include "memory/wrapped_flags.h"
#include "game/cached_resource_state_types.h"
class c_allocation_base;
class c_tag_resource_address_cache
{
public:
    struct s_published_resources_state
    {
        __forceinline void mark_required(const long &index)
        {
            unsigned long mask = (1UL << (index & 31));
            required.set_bits(index, mask);
            available.set_bits(index, mask);
        }

        const c_tag_resource_cache_file_access_cache *access_cache;
        c_wrapped_flags available;
        c_wrapped_flags required;
        c_wrapped_flags unknown;
        bool published;
    };
    __forceinline c_tag_resource_address_cache() {}
    void dispose_resource_address(long handle, void *address);
    __forceinline void purge_access_cache(c_tag_resource_cache_file_access_cache *cache) { find_cached_published_state(cache)->published = false; }
    void initialize_for_new_map(unsigned long count, unsigned long maximum_count, unsigned long published_count, c_allocation_base *allocator);
    void initialize_resource_address(long handle, void *address);
    void populate_access_cache(c_tag_resource_cache_file_access_cache *access_cache, bool required_only);
    __forceinline s_published_resources_state *find_cached_published_state(const c_tag_resource_cache_file_access_cache *access_cache)
    {
        s_published_resources_state *result = 0;
        for (long index = 0; index < m_published_states.count(); index++)
        {
            s_published_resources_state *state = &m_published_states.begin()[index];
            if (state->access_cache == access_cache)
            {
                result = state;
                break;
            }
        }
        return result;
    }
    __forceinline s_published_resources_state *find_or_add_cached_published_state(const c_tag_resource_cache_file_access_cache *access_cache)
    {
        s_published_resources_state *state = find_cached_published_state(access_cache);
        if (!state)
        {
            state = find_cached_published_state(0);
            state->access_cache = access_cache;
        }
        return state;
    }
    __forceinline bool is_resource_address_available(long internal_handle) const
    {
        return m_available_addresses.test((unsigned short)internal_handle);
    }
    __forceinline bool is_external_resource_demanded(long handle) const
    {
        return m_demanded_resources.test((unsigned short)handle);
    }

    c_tag_resource_address_cache *m_controller;
    c_wrapped_array<s_cached_resource_state> m_states;
    c_wrapped_array<__int64> m_pages;
    c_wrapped_flags m_available_addresses;
    c_wrapped_flags m_required_resources;
    c_wrapped_flags m_demanded_resources;
    // Constructed but never allocated or tested: March has only three flag sets
    // and no shipped-code reference touches this one, so its Bungie name is lost.
    c_wrapped_flags m_unused_flags;
    c_wrapped_array<s_published_resources_state> m_published_states;
    void *m_allocation;
    c_allocation_base *m_allocator;
};
#endif
