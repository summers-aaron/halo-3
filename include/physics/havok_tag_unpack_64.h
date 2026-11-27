#ifndef PHYSICS_HAVOK_TAG_UNPACK_64_H
#define PHYSICS_HAVOK_TAG_UNPACK_64_H

#ifdef HALO3_64BIT

#include <stddef.h>
#include <string.h>
#include "tag_files/tag_types.h"

extern "C" {
void *GetProcessHeap(void);
void *RtlAllocateHeap(void *heap, unsigned long flags, size_t bytes);
void *port_havok_unpack_alloc_64(size_t bytes);
}

// Sentinel written into s_tag_block::definition.value once a 32-bit on-disk Havok tag block
// has been unpacked into a low-4GB 64-bit array, ensuring fixup calls are O(1) idempotent.
static const unsigned long k_tag_block_unpacked_64_sentinel = 0x48363455u; // 'H64U'

static __forceinline bool tag_block_is_unpacked_64(const s_tag_block *block)
{
    return block->definition.value == k_tag_block_unpacked_64_sentinel;
}

static __forceinline void tag_block_mark_unpacked_64(s_tag_block *block)
{
    block->definition.value = k_tag_block_unpacked_64_sentinel;
}

// Allocates zero-initialized low-4GB memory (freed on map reload via port/data/cache_swap.cpp
// on Linux x86_64) so the returned pointer fits in 32-bit s_tag_block::address / TAG_POINTER slots.
static __forceinline void *havok_unpack_alloc_64(size_t bytes)
{
    if (bytes == 0)
        bytes = 16;
#if defined(__x86_64__) && defined(__ELF__)
    return port_havok_unpack_alloc_64(bytes);
#else
    return RtlAllocateHeap(GetProcessHeap(), 0x08u /* HEAP_ZERO_MEMORY */, bytes);
#endif
}

#endif // HALO3_64BIT

#endif // PHYSICS_HAVOK_TAG_UNPACK_64_H
