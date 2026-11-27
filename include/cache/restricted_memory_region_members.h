#ifndef CACHE_RESTRICTED_MEMORY_REGION_MEMBERS_H
#define CACHE_RESTRICTED_MEMORY_REGION_MEMBERS_H

#include "cache/restricted_memory.h"
#include "cache/restricted_memory_regions_inline.h"

// Publishes a new member's address through the owner's allocator setter (a __tls_set_*_allocator function).
template <void (*k_set_allocator_function)(void *)>
struct t_restricted_allocator_publisher
{
    static __forceinline void publish(void *address)
    {
        k_set_allocator_function(address);
    }
};

// Publishers retain the caller-specific, inlined address publication after registration.
template <class t_publisher>
static __forceinline long restricted_region_add_member(long region, const char *name, const char *type,
    unsigned int size, long alignment, field_proc_t setter)
{
    long member_index = g_restricted_regions[region].add_member(name, type, size, alignment, setter, 0, 0);
    void *address = restricted_region_get_member_address(region, member_index);
    t_publisher::publish(address);
    return member_index;
}

template <class t_publisher>
static __forceinline long restricted_region_add_member(long region, const char *name, const char *type,
    unsigned int size, long alignment, field_proc_t setter, field_proc_t post_copy, field_proc_t post_update)
{
    long member_index = g_restricted_regions[region].add_member(name, type, size, alignment, setter, post_copy, post_update);
    void *address = restricted_region_get_member_address(region, member_index);
    t_publisher::publish(address);
    return member_index;
}

#endif // CACHE_RESTRICTED_MEMORY_REGION_MEMBERS_H
