#ifndef CACHE_RESTRICTED_MEMORY_REGIONS_INLINE_H
#define CACHE_RESTRICTED_MEMORY_REGIONS_INLINE_H

#include "memory/tls_data_arrays.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions.h"

static __forceinline char *restricted_region_get_base_address(long region)
{
    return g_restricted_address[region];
}

static __forceinline bool restricted_region_locked_for_current_thread(long region)
{
    return g_restricted_address[region] != 0;
}

static __forceinline unsigned int restricted_region_get_member_offset(long region, long member)
{
    return ((s_field_layout *)g_restricted_regions)[region].fields[member].offset;
}

static __forceinline void *restricted_region_get_member_address(long region, long member)
{
    char *base = g_restricted_address[region];
    unsigned int offset = ((s_field_layout *)g_restricted_regions)[region].fields[member].offset;
    return base + offset;
}

#endif // CACHE_RESTRICTED_MEMORY_REGIONS_INLINE_H
