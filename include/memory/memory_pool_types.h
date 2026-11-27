#ifndef MEMORY_MEMORY_POOL_TYPES_H
#define MEMORY_MEMORY_POOL_TYPES_H

#include <stddef.h>
#include "memory/static_arrays.h"

class c_allocation_base;

enum e_memory_pool_callback_list
{
    _memory_pool_callback_list_none = 0,
};

struct s_memory_pool_block
{
    long size;
    long reference;
    long next;
    long previous;
};

struct s_memory_pool
{
    unsigned long signature;
    c_static_string<32> name;
    c_allocation_base *allocator;
    long size;
    long free_size;
    long data_offset;
    long first_block_handle;
    long last_block_handle;
    bool reference_tracking_disabled;
    bool allocation_from_anywhere;
    bool verification_disabled;
    bool debugging_headers_enabled;
    long callback_list;
};

typedef char memory_pool_block_size[(sizeof(s_memory_pool_block) == 16) ? 1 : -1];
typedef char memory_pool_size[(sizeof(void *) != 4 || ((sizeof(s_memory_pool) == 68) )) ? 1 : -1];
typedef char memory_pool_name_offset[(offsetof(s_memory_pool, name) == 4) ? 1 : -1];
typedef char memory_pool_allocator_offset[(sizeof(void *) != 4 || ((offsetof(s_memory_pool, allocator) == 36) )) ? 1 : -1];
typedef char memory_pool_first_offset[(sizeof(void *) != 4 || ((offsetof(s_memory_pool, first_block_handle) == 52) )) ? 1 : -1];
typedef char memory_pool_flags_offset[(sizeof(void *) != 4 || ((offsetof(s_memory_pool, reference_tracking_disabled) == 60) )) ? 1 : -1];
typedef char memory_pool_callbacks_offset[(sizeof(void *) != 4 || ((offsetof(s_memory_pool, callback_list) == 64) )) ? 1 : -1];

#endif // MEMORY_MEMORY_POOL_TYPES_H
