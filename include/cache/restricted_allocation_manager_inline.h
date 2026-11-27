#ifndef CACHE_RESTRICTED_ALLOCATION_MANAGER_INLINE_H
#define CACHE_RESTRICTED_ALLOCATION_MANAGER_INLINE_H

#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_region_members.h"

// Generic reserve_memory: registers one member of the allocator's region, publishes its
// address through the allocator setter, and records the owning thread.
template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
__forceinline void *
t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<k_set_allocator_function> >(
        k_region_index, name, type, size, alignment_bits, k_set_allocator_function);
    m_thread_id = g_registered_thread_index;
    return restricted_region_get_member_address(k_region_index, m_member_index);
}

#endif // CACHE_RESTRICTED_ALLOCATION_MANAGER_INLINE_H
