#ifndef MEMORY_LRUV_CACHE_INLINE_H
#define MEMORY_LRUV_CACHE_INLINE_H

#include "memory/data.h"
#include "memory/lruv_block_types.h"
#include "memory/lruv_cache_types.h"

// Marks a block as used in the cache's current frame (inlined in release builds).
static __forceinline void lruv_block_touch(s_lruv_cache *cache, long block_index)
{
    datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, block_index)->last_used_frame = cache->frame;
}

#endif // MEMORY_LRUV_CACHE_INLINE_H
