// blamlibxbox/xbox_sound_cache.obj: from the records 0xfaa18 0x137aa8 0x1f6050 0x1f6118 0x1f6230 0x1f6298 0x1f62d0 0x21a850 0x21a998 0x21aaa8 0x21ab38
// @compiler xdk6130
// @flags /TP /O1 /Oc
// @address 0x74b80c g_runtime_state_allocation
#include "memory/data_types.h"
#include "memory/runtime_state_allocation.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"

// @address 0x1f6050 ?sound_cache_new@@YAXXZ
// @address 0x1f6118 ?sound_cache_flush_all_sound_guaranteed@@YAXXZ
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0x815a8 ?async_idle@@YAXXZ
// @address 0x80608 GetTickCount
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x2865e0 ?lruv_new@@YAPAUs_lruv_cache@@PBDJJJPAXP6AX1J_N@ZP6A_N1J@ZP6AE1J@ZPAVc_allocation_base@@J@Z
// @address 0x101960 ?submit_indirect_async_read@c_asynchronous_io_arena@@QAA_NPAVc_indirect_cache_file_location_atlas@@PBUs_indirect_cache_file_read_request@@PAVc_asynchronous_io_marker@@@Z
#include <ppcintrinsics.h>
#include "cache/xbox_sound_cache.h"
#include "cseries/async.h"
#include "memory/lruv_cache.h"
#include "memory/lruv_cache_inline.h"
#include "memory/data.h"
#include "cache/cache_file_tag_resource_runtime_manager_storage.h"
#include "memory/lruv_block_types.h"
#include "memory/lruv_block_header_types.h"
#include "cache/cache_files.h"
#include "memory/physical_memory_map.h"


#include "sound/sound_cache_inline.h"
#include "memory/normal_allocation.h"
#include "cache/geometry_globals.h"
#include "cseries/crt_xdk.h"
#include "memory/data_iterator_noalias.h"
#include "cache/indirect_cache_file_reader_types.h"
#include "cache/cache_file_tag_resource_bulk_reader.h"

struct s_xbox_sound_cache_iterator
{
    s_cache_sound_datum *datum;
    s_data_iterator iterator;
};

static __forceinline s_cache_sound_datum *sound_cache_get_datum(long block_index)
{
    return datum_get_typed<s_cache_sound_datum>(g_sound_cache_globals.sounds, block_index);
}


// match: 0x137aa8, 0xfaa18: keeps release frameless with shift/add datum scaling; removal adds stwu frame+pdata, mulli scaling, record-form bit tests
#pragma optimize("t", on)

// @address 0x137aa8 sound_cache_sound_release
extern "C" void sound_cache_sound_release(s_sound_permutation_chunk *sound_permutation_chunk)
{
    s_sound_cache_globals *globals = &g_sound_cache_globals;
    s_cache_sound_datum *cache_sound = sound_cache_get_datum(sound_permutation_chunk->cache_index);
    cache_sound->software_reference_count--;
    if (!sound_cache_is_block_locked(0, sound_permutation_chunk->cache_index)) {
        long io_status = cache_sound->io_marker.io_status;
        __lwsync();
        if (io_status == 0) {
            lruv_block_delete(globals->lruv, sound_permutation_chunk->cache_index, false);
        }
    }
}


static __forceinline bool xbox_sound_cache_sound_available(const s_cache_sound_datum *cache_sound)
{
    long io_status = cache_sound->io_marker.io_status;
    __lwsync();
    return io_status == 1;
}

static __forceinline bool xbox_sound_cache_sound_io_pending(const s_cache_sound_datum *cache_sound)
{
    long io_status = cache_sound->io_marker.io_status;
    __lwsync();
    return io_status == 0;
}

// @address 0xfaa18 ?_sound_cache_sound_request@@YAKJPAUs_sound_permutation_chunk@@K@Z
unsigned long _sound_cache_sound_request(long definition_index, s_sound_permutation_chunk *chunk, unsigned long request)
{
    unsigned long result = 0;
    bool reference = (request & 2) != 0;
    bool pending = false;
    if (chunk->resource_index != -1 && chunk->cache_index == -1 && definition_index != -1 && (request & 1) != 0)
        sound_cache_allocate_and_read_block(definition_index, chunk, &pending);
    if (chunk->cache_index != -1) {
        lruv_block_touch(g_sound_cache_globals.lruv, chunk->cache_index);
        s_cache_sound_datum *cache_sound = sound_cache_get_datum(chunk->cache_index);
        if (xbox_sound_cache_sound_available(cache_sound))
            result |= 2;
        if (xbox_sound_cache_sound_io_pending(cache_sound))
            result |= 8;
        if (reference) {
            cache_sound->software_reference_count++;
            result |= 4;
        }
    }
    if ((result & 2) == 0 && (result & 8) == 0 && chunk->cache_index != -1)
        result |= 1;
    if (pending)
        result |= 8;
    return result;
}

// ?sound_cache_new@@YAXXZ
void sound_cache_new(void)
{
    s_sound_cache_globals *globals = &g_sound_cache_globals;

    globals->sounds = data_new("xbox sound", 512, sizeof(s_cache_sound_datum), 0, &g_runtime_state_allocation);
    globals->lruv = lruv_new("xbox sound cache", 512, 14, 512, 0, sound_cache_delete_block_proc,
        sound_cache_is_block_locked, 0, &g_runtime_state_allocation, -1);
    globals->lruv->hole_algorithm = 2;
    globals->buffer_address = ::_physical_memory_malloc((memory_stage)1, (char const *)(0), 0x800000, 0, (char const *)(0), -1);
}


static __forceinline void xbox_sound_cache_iterator_begin(s_xbox_sound_cache_iterator *iterator, s_data_array *data)
{
    iterator->iterator.data = data;
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
}
static __forceinline bool xbox_sound_cache_sound_locked(long block_index)
{
    s_cache_sound_datum *sound = sound_cache_get_datum(block_index);
    bool locked = sound->software_reference_count != 0 || sound->hardware_reference_count != 0;
    if (!locked) {
        long io_status = sound->io_marker.io_status;
        __lwsync();
        if (io_status == 2)
            locked = true;
    }
    return locked;
}
static __forceinline void xbox_sound_cache_sound_delete(s_sound_permutation_chunk *sound)
{
    if (sound->cache_index != -1)
        lruv_block_delete(g_sound_cache_globals.lruv, sound->cache_index, false);
}

// ?sound_cache_flush_all_sound_guaranteed@@YAXXZ
void sound_cache_flush_all_sound_guaranteed(void)
{
    unsigned long start_time = GetTickCount();
    s_xbox_sound_cache_iterator iterator;
    xbox_sound_cache_iterator_begin(&iterator, g_sound_cache_globals.sounds);
    while ((iterator.datum = (s_cache_sound_datum *)data_iterator_next(&iterator.iterator)) != 0) {
        s_cache_sound_datum *sound = iterator.datum;
        while (xbox_sound_cache_sound_locked(iterator.iterator.index)) {
            async_idle();
            get_geometry_runtime_manager()->update_bulk_reader();
            if (GetTickCount() - start_time >= 5000)
                break;
        }
        xbox_sound_cache_sound_delete(sound->sound);
    }
}

// @address 0x1f6230 sound_cache_is_block_locked
extern "C" bool sound_cache_is_block_locked(void *client_data, long block_index)
{
    s_cache_sound_datum *cache_sound = sound_cache_get_datum(block_index);
    bool locked = cache_sound->software_reference_count != 0 || cache_sound->hardware_reference_count != 0;
    if (!locked) {
        long io_status = cache_sound->io_marker.io_status;
        __lwsync();
        if (io_status == 2) {
            locked = true;
        }
    }
    return locked;
}

// @address 0x1f6298 ?sound_cache_delete_block_proc@@YAXPAXJ_N@Z
void sound_cache_delete_block_proc(void *client_data, long block_index, bool purge)
{
    s_cache_sound_datum *cache_sound = sound_cache_get_datum(block_index);
    cache_sound->sound->cache_index = -1;
    s_sound_cache_globals *globals = &g_sound_cache_globals;
    ::datum_delete(globals->sounds, block_index);
}

// @address 0x21a850 sound_cache_start_loading_sound
extern "C" bool sound_cache_start_loading_sound(c_asynchronous_io_arena *arena,
    c_indirect_cache_file_location_atlas *atlas,c_indirect_cache_file_bulk_read_iterator *iterator,
 const s_indirect_cache_file_read_request *request,unsigned long offset,unsigned long limit,
 const c_basic_buffer<void> *override_buffer,unsigned long *out_size,c_asynchronous_io_marker *marker)
{
 s_indirect_cache_file_read_request partial_request=*request;
 partial_request.offset+=offset;
 partial_request.buffer=override_buffer?*override_buffer:request->buffer.slice(offset,limit);
 unsigned long bytes_to_read=limit>request->size-offset?request->size-offset:limit;
 partial_request.size=bytes_to_read;
 if(override_buffer) *out_size=bytes_to_read;
 s_indirect_cache_file_read_request submission=partial_request;
 bool success=arena->submit_indirect_async_read(atlas,&submission,marker);
 if(iterator) iterator->read_progress(bytes_to_read,true);
 if(!success && iterator) iterator->read_progress(0,false);
 return success;
}

static __forceinline unsigned long xbox_sound_cache_align_size(unsigned long size)
{
    return (size + 4095) & ~4095;
}
// @address 0x21a998 indirect_cache_file_read_blocking
extern "C" long indirect_cache_file_read_blocking(c_asynchronous_io_arena *arena, c_indirect_cache_file_location_atlas *atlas,
    c_indirect_cache_file_bulk_read_iterator *iterator, const s_indirect_cache_file_read_request *request, unsigned long offset,
    unsigned long limit, const c_basic_buffer<void> *override_buffer, unsigned long *out_size, unsigned long *total_bytes)
{
    c_asynchronous_io_marker marker;
    marker.set_io_status(0);
    long result;
    if (sound_cache_start_loading_sound(arena, atlas, iterator, request, offset, 0x20000000, 0, 0, &marker)) {
        while (!marker.request_completed()) {
            arena->update_with_quantum_entanglement(false);
            if (iterator)
                iterator->read_progress(0, true);
        }
        result = marker.get_io_status() == 1 ? 0 : 2;
        *total_bytes += iterator ? marker.requested_size : xbox_sound_cache_align_size(marker.requested_size);
    } else {
        result = 5;
    }
    return result;
}
// @address 0x21aaa8 ?start_read@c_cache_file_tag_resource_bulk_reader@@UAA_NV?$c_basic_buffer@X@@KPAKJ@Z
bool c_cache_file_tag_resource_bulk_reader::start_read(
    c_basic_buffer<void> buffer, unsigned long offset, unsigned long *out_size, long request_index)
{
    bool result = sound_cache_start_loading_sound(m_io_arena, m_atlas, m_iterator, m_request,
        offset, buffer.m_size, &buffer, out_size, &m_markers[request_index]);
    if (result)
    {
        *m_total_bytes += *out_size;
        m_pending_requests |= 1 << request_index;
    }
    return result;
}

// @address 0x21ab38 ?read_complete@c_cache_file_tag_resource_bulk_reader@@UAA_NJPAKPA_N@Z
bool c_cache_file_tag_resource_bulk_reader::read_complete(
    long request_index, unsigned long *out_bytes_read, bool *out_success)
{
    bool result = false;
    if ((m_pending_requests & (1 << request_index)) != 0)
    {
        long status = m_markers[request_index].io_status;
        __lwsync();
        if (status != 2)
        {
            long final_status = m_markers[request_index].io_status;
            result = true;
            __lwsync();
            if (final_status == 1)
            {
                *out_bytes_read = m_markers[request_index].requested_size;
                *out_success = true;
            }
            else
            {
                *out_success = false;
            }
        }
    }
    return result;
}


static __forceinline c_asynchronous_io_marker *xbox_sound_cache_datum_marker(s_cache_sound_datum *cache_sound)
{
    return &cache_sound->io_marker;
}
static __forceinline long &xbox_sound_cache_datum_tag_index(s_cache_sound_datum *cache_sound)
{
    return cache_sound->tag_index;
}
static __forceinline bool xbox_sound_cache_sound_class_is_dialog(long sound_class)
{
    return sound_class == 47 || sound_class == 46 || sound_class == 45 || sound_class == 32;
}
static __forceinline bool xbox_sound_cache_sound_class_is_music(long sound_class)
{
    return sound_class == 32 || sound_class == 38;
}
static __forceinline bool xbox_sound_cache_sound_class_is_high_priority(long sound_class)
{
    return xbox_sound_cache_sound_class_is_dialog(sound_class) || xbox_sound_cache_sound_class_is_music(sound_class);
}
static __forceinline unsigned char xbox_sound_cache_read_permutation_chunk(const cache_file_tag_instance *instance,
    const s_sound_permutation_chunk *chunk, long size, const c_basic_buffer<void> &buffer, c_asynchronous_io_marker *marker)
{
    struct sound_definition
    {
        unsigned short flags;
        signed char sound_class;
        char pad3[0x18 - 3];
        long resource_index;            
    };
    unsigned char success = false;
    const sound_definition *sound = (const sound_definition *)instance->definition;
    if (sound->resource_index != -1) {
        long chunk_resource_index = chunk->resource_index;
        success = get_geometry_runtime_manager()->submit_resource_read(sound->resource_index,
            xbox_sound_cache_sound_class_is_high_priority(sound->sound_class), chunk_resource_index, size, buffer, marker);
    }
    return success;
}
// @address 0x1f62d0 sound_cache_allocate_and_read_block
extern "C" void sound_cache_allocate_and_read_block(long sound_index, s_sound_permutation_chunk *chunk, bool *out_failed)
{
    s_sound_cache_globals *globals = &g_sound_cache_globals;
    long size = chunk->flags_and_sample_size & 0x3ffffff;
    *out_failed = false;
    if ((size & 0xfff) != 0)
        size = (size | 0xfff) + 1;
    long block_index = lruv_block_new(globals->lruv, -1, size, 0);
    if (block_index != -1) {
        void *block_address = (char *)globals->buffer_address + lruv_get_block_address(globals->lruv, block_index);
        datum_new_at_index(globals->sounds, block_index);
        s_cache_sound_datum *cache_sound = sound_cache_get_datum(block_index);
        chunk->cache_index = block_index;
        xbox_sound_cache_datum_tag_index(cache_sound) = sound_index;
        const cache_file_tag_instance *instance = &global_tag_instances[datum_index_to_absolute_index(sound_index)];
        cache_sound->sound = chunk;
        cache_sound->software_reference_count = 0;
        cache_sound->hardware_reference_count = 0;
        if (!xbox_sound_cache_read_permutation_chunk(instance, chunk, chunk->flags_and_sample_size & 0x3ffffff, c_basic_buffer<void>(block_address, size),
                xbox_sound_cache_datum_marker(cache_sound)))
        {
            s_lruv_cache *lruv = globals->lruv;
            *out_failed = true;
            lruv_block_delete(lruv, block_index, false);
        }
    }
}

// @address 0x7e3634 g_sound_cache_globals
