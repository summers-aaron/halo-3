#ifndef CACHE_RESTRICTED_ADDRESS_H
#define CACHE_RESTRICTED_ADDRESS_H

// Thread-local base of each restricted memory region, owned by restricted_memory_regions.cpp;
// restricted_region_add_alias reads it and stores g_restricted_alias.
extern __declspec(thread) char *g_restricted_address[];

#endif // CACHE_RESTRICTED_ADDRESS_H
