#ifndef ZLIB_BLAM_ZLIB_MEMORY_H
#define ZLIB_BLAM_ZLIB_MEMORY_H

// Functions defined in tu/blamlibxbox/blam_zlib_memory.cpp that other units call, plus the zlib memory tracker
// set-up every caller inlines.

#include "memory/memory_pool.h"
#include "zlib/blam_zlib_memory_types.h"

void *blam_zlib_malloc(void *context, unsigned int items, unsigned int size);
void blam_zlib_free(void *context, void *pointer);

inline void blam_zlib_initialize(s_memory_pool *heap, s_blam_zlib_memory *out_zlib_memory_tracker)
{
    memory_pool_toggle_reference_tracking(heap, false);
    memory_pool_toggle_allocation_from_anywhere_in_pool(heap, true);
    out_zlib_memory_tracker->heap = heap;
}

inline void blam_zlib_dispose(s_blam_zlib_memory *zlib_memory_tracker)
{
    zlib_memory_tracker->heap = NULL;
}

#endif // ZLIB_BLAM_ZLIB_MEMORY_H
