#ifndef CACHE_RESTRICTED_MEMORY_REGISTRATION_H
#define CACHE_RESTRICTED_MEMORY_REGISTRATION_H

#include "cache/restricted_memory.h"
#include "cache/restricted_memory_regions_inline.h"


static __forceinline int restricted_memory_register_member(c_restricted_memory *memory,
    const char *name, const char *type, unsigned int size, int alignment_bits,
    field_proc_t construct, field_proc_t pre_overwrite, field_proc_t post_copy)
{
    return memory->add_member(name, type, size, alignment_bits, construct, pre_overwrite, post_copy);
}


// Register a region member and publish the current thread's address immediately.
static __forceinline long restricted_region_register_member(long region, const char *name, const char *type,
    unsigned int size, long alignment_bits, field_proc_t set_address, field_proc_t constructor, field_proc_t destructor)
{
    long member = restricted_memory_register_member(&g_restricted_regions[region], name, type, size,
        alignment_bits, set_address, constructor, destructor);
    void *address = restricted_region_get_member_address(region, member);
    set_address(address);
    return member;
}

#endif // CACHE_RESTRICTED_MEMORY_REGISTRATION_H
