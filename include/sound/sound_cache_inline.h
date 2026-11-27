#ifndef SOUND_SOUND_CACHE_INLINE_H
#define SOUND_SOUND_CACHE_INLINE_H

#include <stddef.h>
#include "cseries/pointer_utilities.h"
#include "cache/asynchronous_io_marker_types.h"
#include "memory/data_typed_access.h"
#include "memory/data_types.h"
#include "memory/lruv_block_types.h"
#include "memory/lruv_cache_types.h"
#include "sound/sound_permutation_chunk.h"

struct s_cache_sound_datum
{
    unsigned short identifier;
    unsigned char software_reference_count;
    unsigned char hardware_reference_count;
    c_asynchronous_io_marker io_marker;
    long tag_index;
    s_sound_permutation_chunk *sound;
};

struct s_sound_cache_globals
{
    s_data_array *sounds;
    void *buffer_address;
    s_lruv_cache *lruv;
};

extern "C" { extern s_sound_cache_globals g_sound_cache_globals; }

static __forceinline unsigned long lruv_get_page_address(s_lruv_cache *cache, unsigned long page_index)
{
    return page_index << cache->page_size_bits;
}

static __forceinline s_lruv_block *lruv_get_block(s_data_array *array, long index)
{
    return datum_get_typed<s_lruv_block>(array, index);
}

static __forceinline unsigned long lruv_get_block_address(s_lruv_cache *cache, long index)
{
    s_lruv_block *block = lruv_get_block((s_data_array *)cache->header.block_data, index);
    unsigned long result = lruv_get_page_address(cache, block->page_index);
    return result;
}

static __forceinline void *sound_cache_get_address(long index)
{
    return offset_pointer(g_sound_cache_globals.buffer_address,
        lruv_get_block_address(g_sound_cache_globals.lruv, index));
}

// March 82a8c230: increment the saturating hardware reference count, return
// the block address and report its unaligned sample size.
static __forceinline void *sound_cache_sound_hardware_lock(s_sound_permutation_chunk *permutation, unsigned long *size)
{
    s_cache_sound_datum *sound = datum_get_typed<s_cache_sound_datum>(g_sound_cache_globals.sounds, permutation->cache_index);
    if (sound->hardware_reference_count < 255)
        ++sound->hardware_reference_count;
    void *data = sound_cache_get_address(permutation->cache_index);
    *size = permutation->get_sample_size();
    return data;
}

struct s_xma2_packet_info
{
    unsigned long first_sample;
    unsigned long last_sample;
};

// The source completion and failure paths release one hardware reference.
static __forceinline void sound_cache_sound_hardware_unlock(s_sound_permutation_chunk *permutation)
{
    s_cache_sound_datum *sound = datum_get_typed<s_cache_sound_datum>(g_sound_cache_globals.sounds, permutation->cache_index);
    if (sound->hardware_reference_count > 0)
        --sound->hardware_reference_count;
}

// March 82a8da78/82a8dba8; flags 3 and 4 gate the two sample bounds.
static __forceinline bool sound_cache_sound_uses_xma2(s_sound_permutation_chunk *permutation)
{
    return permutation->test_flag(5);
}

static __forceinline void sound_cache_sound_get_xma2_info(s_sound_permutation_chunk *permutation, s_xma2_packet_info *info)
{
    info->first_sample = permutation->test_flag(3) ? permutation->first_sample : 0;
    info->last_sample = permutation->test_flag(4) ? permutation->last_sample : 0;
}

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char cache_sound_datum_size[(sizeof(void *) != 4 || (sizeof(s_cache_sound_datum) == 40)) ? 1 : -1];
typedef char cache_sound_datum_io_status_offset[(sizeof(void *) != 4
    || (offsetof(s_cache_sound_datum, io_marker) + offsetof(c_asynchronous_io_marker, io_status) == 28)) ? 1 : -1];
typedef char cache_sound_datum_sound_offset[(sizeof(void *) != 4 || (offsetof(s_cache_sound_datum, sound) == 36)) ? 1 : -1];
typedef char sound_cache_globals_size[(sizeof(void *) != 4 || (sizeof(s_sound_cache_globals) == 12)) ? 1 : -1];
#endif

#endif // SOUND_SOUND_CACHE_INLINE_H
