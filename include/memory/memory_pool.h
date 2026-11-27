#ifndef MEMORY_MEMORY_POOL_H
#define MEMORY_MEMORY_POOL_H

// Functions defined in tu/blamlibxbox/memory_pool.cpp, plus the small fixed-pool helpers every caller inlines.

#include "memory/memory_pool_types.h"

typedef void (*pool_relocation_callback)(long reference, long offset);
extern "C" pool_relocation_callback g_memory_pool_relocation_callbacks[];

void memory_pool_dispose(s_memory_pool *pool);
void memory_pool_block_free(s_memory_pool *pool, long offset);
void memory_pool_compact(struct s_memory_pool * pool);
extern "C" void memory_pool_initialize(s_memory_pool *pool, const char *pool_name, long pool_size,
    e_memory_pool_callback_list callback_list, c_allocation_base *allocator);
extern "C" int memory_pool_block_allocate_handle(s_memory_pool *pool, long reference, long size, const char *file, long line);
extern "C" int memory_pool_block_reallocate_handle(char *pool, int handle, int new_size, int file, int line);
extern "C" s_memory_pool_block *memory_pool_block_new(s_memory_pool *pool, long size, const char *file, long line);

extern "C" s_memory_pool_block *memory_pool_block_initialize(s_memory_pool *pool, char *memory, int size, const char *name,
    int line, s_memory_pool_block *next, s_memory_pool_block *previous);

extern "C" s_memory_pool *memory_pool_new(const char *pool_name, int pool_size, unsigned int callback_kind, unsigned int *allocation_base);

inline long memory_pool_allocation_size(long size)
{
    return size + sizeof(s_memory_pool) + sizeof(s_memory_pool_block);
}

inline s_memory_pool *fixed_memory_pool_new(const char *name, void *buffer, long size, e_memory_pool_callback_list callback_list)
{
    s_memory_pool *pool = NULL;
    long pool_size = size - memory_pool_allocation_size(0);
    if (size - memory_pool_allocation_size(0) >= 0)
    {
        pool = (s_memory_pool *)buffer;
        memory_pool_initialize(pool, name, pool_size, callback_list, 0);
    }
    return pool;
}

inline void fixed_memory_pool_free(s_memory_pool *pool)
{
}

inline void memory_pool_toggle_reference_tracking(s_memory_pool *pool, bool enable)
{
    pool->reference_tracking_disabled = !enable;
}

inline void memory_pool_toggle_allocation_from_anywhere_in_pool(s_memory_pool *pool, bool enable)
{
    pool->allocation_from_anywhere = enable;
}

#endif // MEMORY_MEMORY_POOL_H
