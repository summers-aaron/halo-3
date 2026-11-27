#ifndef MEMORY_MEMORY_POOL_INLINE_H
#define MEMORY_MEMORY_POOL_INLINE_H

#include "memory/memory_pool.h"

static __forceinline bool memory_pool_block_allocate(s_memory_pool *pool, void **pointer, long size, const char *file, long line)
{
    bool success = false;
    long handle = memory_pool_block_allocate_handle(pool, 0, size, 0, 0);
    if (handle != 0) {
        success = true;
        *pointer = (char *)pool + handle;
    }
    return success;
}

static __forceinline bool memory_pool_block_allocate(s_memory_pool *pool, void **pointer, long size)
{
    long handle = memory_pool_block_allocate_handle(pool, 0, size, 0, 0);
    bool success = false;
    if (handle != 0) {
        *pointer = (char *)pool + handle;
        success = true;
    }
    return success;
}
static __forceinline void memory_pool_block_free(s_memory_pool *pool, void **pointer)
{
    long handle = *pointer ? (char *)*pointer - (char *)pool : 0;
    ::memory_pool_block_free(pool, handle);
}

#endif // MEMORY_MEMORY_POOL_INLINE_H
