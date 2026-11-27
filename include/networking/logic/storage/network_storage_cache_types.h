#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_CACHE_TYPES_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_CACHE_TYPES_H

struct s_network_storage_cache_index
{
    long entry_count;
    char entries[128 * 148];
};

#endif
