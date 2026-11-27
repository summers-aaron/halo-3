// @address 0x59f830 ReadFileEx
// blamlibxbox/cache_file_bulk_reader.obj: from the records 0x84d28 0x84dd0 0x84fa8 0x101960 0x101af8 0x1b3510 0x21a7a0 0x21aba0 0x21aec0 0x21afb8 0x21b050 0x21b158 0x21b438 0x21b5d0 0x21b6e8 0x21b6f8 0x21b788 0x70b8d0 0x21b1d8
// @flags /TP /O2 /Oc
// @address 0x84dd0 ?update_with_quantum_entanglement@c_asynchronous_io_arena@@QAAX_N@Z
// @address 0x101af8 ?submit_async_read@c_asynchronous_io_arena@@QAA_NJKKV?$c_basic_buffer@X@@PAVc_asynchronous_io_marker@@@Z
// @address 0x21a7a0 ?close_file@c_asynchronous_io_arena@@QAAXJ@Z
// @address 0x21aba0 ?update@c_sound_cache_bulk_reader@@UAAXK@Z
// @address 0x3e478 ??_7c_sound_cache_file_reader@@6B@

#include <xtl.h>
#include "cseries/crt_xdk.h"
#include "cseries/xdk_memory.h"
#include <ppcintrinsics.h>
#include <string.h>
#include "cseries/cseries.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "cache/cache_file_tag_resource_runtime_manager_storage.h"
#include "main/main.h"
#include "cache/cache_file_bulk_reader.h"
#include "cache/sound_cache_bulk_reader.h"
#include "cache/sound_cache_file_reader.h"

#include "cache/cache_file_bulk_reader_slot_bits_types.h"
#include "cache/async_io_file_types.h"
#include "cache/async_io_last_read_types.h"
#include "cache/cache_file_bulk_reader_set_bits_types.h"

#include "cache/asynchronous_io_marker_types.h"
#include "memory/basic_buffer.h"
#include "cache/indirect_cache_file_reader_types.h"
#include "cseries/xdk_async_io.h"
#include "cache/cache_file_bulk_reader_dependencies.h"
#include "cache/cache_file_bulk_reader_state_types.h"

// @address 0x75deb0 g_asynchronous_io_arena
c_asynchronous_io_arena g_asynchronous_io_arena = { INVALID_HANDLE_VALUE, { 0 }, { 0 }, 0, { 0, 0, -1, 0 } };

#include "cache/cache_file_resource_element_set.h"
#include "memory/bit_vectors.h"

#include "cache/cache_file_resource_gestalt.h"
#include "cache/cache_file_header.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/tag_resource_runtime_interfaces.h"
#include "cache/cache_file_tag_resource_runtime_window_allocator_storage.h"
#include "cache/cache_file_bulk_reader_requests.h"
#include "cache/cache_file_bulk_reader_callbacks.h"

struct s_sound_cache_async_hash_request
{
#ifdef HALO3_64BIT
    c_basic_buffer<void> source_position;
#else
    unsigned __int64 source_position;
#endif
    unsigned char expected_digest[20];
    volatile long *cancel;
    volatile long *progress;
    unsigned char *success;
    unsigned char test_hash;
    unsigned char source_open;
    c_sound_cache_file_reader reader;
    s_sound_cache_hash_state hash;
};
typedef char async_hash_reader_offset[(sizeof(void *) != 4 || ((offsetof(s_sound_cache_async_hash_request, reader) == 0x2c))) ? 1 : -1];
typedef char async_hash_state_offset[(sizeof(void *) != 4 || ((offsetof(s_sound_cache_async_hash_request, hash) == 0x84))) ? 1 : -1];
typedef char async_hash_source_offset[(sizeof(void *) != 4 || ((offsetof(s_sound_cache_hash_state, source) == 0x88))) ? 1 : -1];

static __forceinline void cache_file_bulk_reader_set_io_status(c_asynchronous_io_marker *marker, long status)
{
    __lwsync();
    _InterlockedExchange(&marker->io_status, status);
}

// @address 0x84d28 ?update_bulk_reader@c_cache_file_tag_resource_runtime_manager_storage@@QAAXXZ
void c_cache_file_tag_resource_runtime_manager_storage::update_bulk_reader()
{
    g_asynchronous_io_arena.update_with_quantum_entanglement(false);
    s_cache_file_tag_resource_page_request *request = &m_page_cursor;
    const volatile long &request_id = request->id;
    if (!request->ready && request_id != -1) {
        long completed = request->done.peek();
        if (completed) {
            request->ready = 1;
        }
    }
    if (request->is_ready()) {
        cache_file_bulk_reader_complete_request(request, &m_page_storage);
    }
}

void c_asynchronous_io_arena::update_with_quantum_entanglement(bool block_on_completion_port)
{
    if (block_on_completion_port) {
        main_update_idle_tasks();
    }
    if (outstanding_io_requests <= 0) {
        return;
    }
    int timeout = block_on_completion_port ? -1 : 0;
    int ok;
    do {
        unsigned int bytes_transferred;
#if defined(_WIN64)
        unsigned __int64 completion_key = 0;
        c_asynchronous_io_marker *marker;
        ok = GetQueuedCompletionStatus(completion_port, (unsigned long *)&bytes_transferred, &completion_key,
            (struct _OVERLAPPED **)&marker, timeout);
        int file_index = (int)completion_key;
#else
        int file_index;
        c_asynchronous_io_marker *marker;
        ok = GetQueuedCompletionStatus(completion_port, (unsigned long *)&bytes_transferred, (unsigned long *)&file_index,
            (struct _OVERLAPPED **)&marker, timeout);
#endif
        if (!ok) {
            GetLastError();
        }
        if (marker) {
            files[file_index].outstanding_io_count -= 1;
            c_asynchronous_io_marker *io_marker = marker;
            outstanding_io_requests -= 1;
            if (ok && io_marker->requested_size <= bytes_transferred) {
                cache_file_bulk_reader_set_io_status(io_marker, 1);
            } else {
                cache_file_bulk_reader_set_io_status(io_marker, 0);
            }
        }
    } while (ok && !block_on_completion_port && outstanding_io_requests > 0);
}

// @address 0x84fa8 cache_file_bulk_reader_complete_request
void cache_file_bulk_reader_complete_request(s_cache_file_tag_resource_page_request *request, s_cache_file_tag_resource_page_state *state)
{
    long slot;
    c_sound_cache_hash_source *source;
    s_cache_file_tag_resource_slot_source *slot_source;
    c_asynchronous_io_marker *marker;
    int success;
    state->busy = false;
    slot = request->slot;
    source = request->source;
    if (slot == -1) {
        state->stream.object = 0;
    } else if ((slot & (int)0x80000000) == 0) {
        slot_source = state->sources[slot];
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x84fa8: one argument keeps r5/r6 unset before the slot 3 bctrl
        slot_source->handle(source);
#else
        slot_source->handle(source, 0, 0);
#endif
    } else {
        state->cursor.object = 0;
    }
    marker = request->marker;
    success = (request->success != 0);
    __lwsync();
    _InterlockedExchange((long *)&marker->io_status, success);
    request->ready = 0;
    request->id = -1;
}


bool c_asynchronous_io_arena::submit_async_read(
    long file_index, unsigned long offset, unsigned long size, c_basic_buffer<void> buffer, c_asynchronous_io_marker *marker)
{
    void *handle = files[file_index].handle;
    marker->offset = offset;
    marker->offset_high = 0;
    marker->event = 0;
    marker->requested_size = size;
    unsigned long aligned_size = (size + 4095) & ~4095;
    unsigned long count = aligned_size <= buffer.m_size ? aligned_size : buffer.m_size;
    unsigned long bytes_read;
    long submitted = ReadFile(handle, buffer.m_address, count, &bytes_read, (struct _OVERLAPPED *)marker);
    unsigned long error = GetLastError();
    bool success = submitted != 0 || error == 997;
    if (success) {
        ++outstanding_io_requests;
        ++files[file_index].outstanding_io_count;
        unsigned long read_offset = marker->offset;
        if (last_read.state < 3) {
            last_read.file_index = file_index;
            last_read.offset = read_offset;
            if (last_read.state < 1)
                last_read.state = 1;
        }
        cache_file_bulk_reader_set_io_status(marker, 2);
    } else {
        cache_file_bulk_reader_set_io_status(marker, 0);
    }
    return success;
}

void c_asynchronous_io_arena::close_file(long file_index)
{
    if (valid_files.test(file_index)) {
        s_async_io_last_read *last = &last_read;
        if (file_index == last->file_index) {
            last->state = 0;
            last->file_index = -1;
        }
        if (files[file_index].outstanding_io_count > 0) {
            do {
                update_with_quantum_entanglement(true);
            } while (files[file_index].outstanding_io_count > 0);
        }
        CloseHandle(files[file_index].handle);
        valid_files.set(file_index, false);
    }
}



static __forceinline long cache_file_bulk_reader_first_slot(unsigned long mask)
{
    return 31 - _CountLeadingZeros(mask & -mask);
}

// @address 0x1b3510 ?submit_resource_read@c_cache_file_tag_resource_runtime_manager_storage@@QAAEJEJJV?$c_basic_buffer@X@@PAVc_asynchronous_io_marker@@@Z
unsigned char c_cache_file_tag_resource_runtime_manager_storage::submit_resource_read(
    long index, unsigned char high_priority, long offset, long size, c_basic_buffer<void> buffer, c_asynchronous_io_marker *marker)
{
    bool success = false;
    if (index != -1)
    {
        s_cache_file_tag_resource_data *entry = (s_cache_file_tag_resource_data *)get_gestalt()->resources.address
            + (index & 0xffff);
        if (entry->salt == (index >> 16))
        {
            if (m_streaming_enabled)
            {
                unsigned char alternate = false;
                unsigned long result = 0xFFFFFFFF;
                if (m_element_set.first->find_direct(entry->fixup_index, offset, size, &alternate, &result))
                {
                    success = tag_resource_cache_get_page_data(&m_resource_cache.m_cache, index, alternate != 0, result, size, buffer);
                    if (success)
                        cache_file_bulk_reader_set_io_status(marker, 1);
                }
            }
            if (!success)
            {
                const s_cache_file_zone_manifest *bits =
                    get_gestalt()->dvd_always_streaming_zone_manifests.begin();
                long absolute_index = index & 0xffff;
                bool loaded = bit_vector_test(bits->required_resources.begin(), absolute_index) ||
                    bit_vector_test(bits->streamed_resources.begin(), absolute_index);
                if (!m_streaming_enabled || high_priority || loaded)
                {
                    long sel = entry->fixup_index;
                    s_indirect_cache_file_read_request request;
                    request.buffer = buffer;
                    unsigned char alternate;
                    unsigned long result;
                    if (m_element_set.first->find_with_offset(sel, offset, size, &alternate, &result))
                    {
                        long location;
                        if (alternate)
                            location = tag_resource_fixed_array_get_second_handle(&m_element_set, sel);
                        else
                            location = cache_file_resource_get_first_handle_datum(&m_element_set, sel);
                        request.position = m_window_allocator.get_page_position(location);
                        request.offset = result;
                        request.size = size;
                        request.uncompressed_size = size;
                        request.hash_source_index = -1;
                        request.validate_hash = false;
                        return g_asynchronous_io_arena.submit_indirect_async_read(get_location_atlas(), &request, marker);
                    }
                }
            }
        }
    }
    return success;
}

// 0x21aba0
void c_sound_cache_bulk_reader::update(unsigned long timeout_milliseconds)
{
    m_io_arena->update_with_quantum_entanglement(timeout_milliseconds == 0xFFFFFFFF);
}

static __forceinline void cache_file_bulk_reader_clear_masks(s_cache_file_bulk_reader_slot_set *slots)
{
    slots->mask_1 = 0;
    slots->mask_2 = 0;
}

static void cache_file_bulk_reader_clear_buffers(s_cache_file_bulk_reader_slot *slot)
{
    slot[0].reset(); slot[1].reset();
}

static __forceinline void cache_file_bulk_reader_reset_slots(s_cache_file_bulk_reader_slot_set *slots)
{
    slots->bits.value &= 3;
    cache_file_bulk_reader_clear_masks(slots);
    cache_file_bulk_reader_clear_buffers(slots->slots);
    cache_file_bulk_reader_clear_masks(slots);
}

static void cache_file_bulk_reader_initialize_slots(s_cache_file_bulk_reader_slot_set *slots, c_basic_buffer<void> buffer)
{
    unsigned long count = (buffer.m_size + 0xFFFF) >> 16;
    cache_file_bulk_reader_reset_slots(slots);
    if (count >= 2)
        count = 2;
    slots->stream_count = (unsigned char)count;
    slots->mask_1 = (unsigned char)((1i64 << slots->stream_count) - 1);
    const c_basic_buffer<void> *source = &buffer;
    unsigned long offset = 0;
    for (long index = 0; index < slots->stream_count; index++) {
        c_basic_buffer<void> block;
        block.m_address = (char *)source->m_address + offset;
        block.m_size = 0x10000;
        slots->slots[index].buffer = block;
        offset += 0x10000;
    }
}

// @address 0x21aec0 cache_file_bulk_reader_initialize_hash_state
void cache_file_bulk_reader_initialize_hash_state(s_sound_cache_hash_state *state, unsigned long read_size, c_basic_buffer<void> buffer,
    c_sound_cache_hash_source *source, bool hash_enabled)
{
    cache_file_bulk_reader_initialize_slots(&state->slots, buffer);
    state->read_offset = 0;
    state->read_bytes = 0;
    state->hash_offset = 0;
    state->read_size = read_size;
    state->source = source;
    state->read_completed = 0;
    state->read_failed = 0;
    state->hash_valid = 0;
    state->hash_completed = 0;
    state->hash_enabled = hash_enabled;
}

// @address 0x21afb8 cache_file_bulk_reader_test_expected_hash
bool cache_file_bulk_reader_test_expected_hash(s_sound_cache_hash_state *state, const void *expected_digest)
{
    bool result = false;
    if (state->hash_enabled != 0 && state->hash_valid != 0 && state->hash_completed == 0) {
        int digest[8];
        XHashMemoryEnd((const _XHASH_STATE *)state, (unsigned char *)digest);
        if (expected_digest != 0) {
            result = memcmp(expected_digest, digest, XHASHMEMORY_DIGEST_SIZE) == 0;
        }
        state->hash_completed = 1;
    }
    return result;
}

// @address 0x21b050 cache_file_bulk_reader_finish_pending_reads
void cache_file_bulk_reader_finish_pending_reads(s_sound_cache_hash_state *state, void *reader_address)
{
    c_sound_cache_reader *reader = static_cast<c_sound_cache_reader *>(reader_address);
    if (state->hash_enabled != 0 && state->hash_valid != 0 && state->hash_completed == 0) {
        int digest[6];
        XHashMemoryEnd((const _XHASH_STATE *)state, (unsigned char *)digest);
        state->hash_completed = 1;
    }
    if (state->slots.stream_count != 0) {
        unsigned char stream_index = 0;
        do {
            int mask = 1 << stream_index;
            if ((state->slots.mask_1 & mask) == 0 && (state->slots.mask_2 & mask) == 0) {
                unsigned long bytes_read;
                bool success;
                while (!reader->read_complete(stream_index, &bytes_read, &success)) {
                    reader->update(0);
                }
            }
            stream_index++;
        } while (stream_index < state->slots.stream_count);
    }
}

// @address 0x21b6e8 ?update@c_sound_cache_file_reader@@UAAXK@Z
void c_sound_cache_file_reader::update(unsigned long timeout_milliseconds)
{
    SleepEx(timeout_milliseconds, 1);
}

// @address 0x21b6f8 ?read_complete@c_sound_cache_file_reader@@UAA_NJPAKPA_N@Z
bool c_sound_cache_file_reader::read_complete(long request_index, unsigned long *bytes_read, bool *success)
{
    bool complete = m_completions[request_index].done.peek() != 0;
    if (complete)
    {
        if (m_completions[request_index].error == 0)
        {
            unsigned long size = m_completions[request_index].bytes_read;
            unsigned long remaining = m_total_size - m_contexts[request_index].offset + m_base_offset;
            if (size <= remaining)
                remaining = size;
            *bytes_read = remaining;
            *success = true;
        }
        else
        {
            *success = false;
        }
    }
    return complete;
}



// 0x21b788
// @address 0x21b788 ?cache_file_bulk_reader_handle_async_read@@YA?AW4e_async_completion@@PAUs_async_task@@PAXJ@Z
e_async_completion cache_file_bulk_reader_handle_async_read(s_async_task *task, void *data, long data_size)
{
    s_sound_cache_async_hash_request *request = static_cast<s_sound_cache_async_hash_request *>(data);
    e_async_completion open_result = _async_completion_pending;
    c_sound_cache_hash_source *source = request->hash.source;
    if (!request->source_open) {
        if (source->open(request->source_position)) {
            open_result = _async_completion_pending;
            request->source_open = true;
        } else {
            open_result = _async_completion_done;
            *request->success = false;
        }
    }
    if (request->source_open) {
        e_async_completion completion;
        s_sound_cache_hash_state *hash = &request->hash;
        bool finished = hash->read_failed || hash->read_completed;
        if (!finished) {
            long cancelled = 0;
            if (request->cancel) {
                cancelled = *request->cancel;
                __lwsync();
            }
            if (cancelled) {
                cache_file_bulk_reader_finish_pending_reads(hash, &request->reader);
                *request->success = false;
                completion = _async_completion_done;
            } else {
                cache_file_bulk_reader_update_hash_state(hash, &request->reader);
                completion = _async_completion_pending;
            }
        } else {
            bool success = false;
            if (request->hash.read_completed) {
                if (!request->test_hash || cache_file_bulk_reader_test_expected_hash(hash, request->expected_digest))
                    success = true;
            }
            *request->success = success;
            completion = _async_completion_done;
        }
        volatile long *progress = request->progress;
        if (progress) {
            finished = hash->read_failed || hash->read_completed;
            if (!finished || request->hash.read_completed) {
                unsigned long hashed = hash->hash_offset;
                if (hashed > hash->read_size)
                    hashed = hash->read_size;
                __lwsync();
                _InterlockedExchange(progress, hashed);
            }
        }
        return completion;
    }
    return open_result;
}


// @address 0x101960 ?submit_indirect_async_read@c_asynchronous_io_arena@@QAA_NPAVc_indirect_cache_file_location_atlas@@PBUs_indirect_cache_file_read_request@@PAVc_asynchronous_io_marker@@@Z
bool c_asynchronous_io_arena::submit_indirect_async_read(
    c_indirect_cache_file_location_atlas *atlas, const s_indirect_cache_file_read_request *request, c_asynchronous_io_marker *marker)
{
    s_indirect_cache_file_location location;
    bool result = false;
    if (atlas->get_location(request->position, &location)) {
        result = submit_async_read(location.file_index, location.offset + request->offset, request->size, request->buffer, marker);
    }
    return result;
}


// @address 0x21b5d0 ?start_read@c_sound_cache_file_reader@@UAA_NV?$c_basic_buffer@X@@KPAKJ@Z
bool c_sound_cache_file_reader::start_read(c_basic_buffer<void> buffer, unsigned long offset, unsigned long *bytes_read, long request_index)
{
    bool result = false;
    *bytes_read = 0;
    unsigned long total = m_total_size;
    if (offset <= total) {
        s_async_read_context *context = &m_contexts[request_index];
        unsigned long size = buffer.m_size;
        unsigned long aligned = (total + 4095) & (unsigned long)~4095;
        unsigned long available = aligned - offset;
        unsigned long clamped = (size > available) ? available : size;
        unsigned long base = m_base_offset;
        s_async_read_completion *completion = &m_completions[request_index];
        context->offset_high = 0;
        context->offset = base + offset;
        context->completion = completion;
        completion->requested_size = clamped;
        completion->done.set(0);
        unsigned long submit_available = ((m_total_size + 4095) & (unsigned long)~4095) - offset;
        if (ReadFileEx(m_handle, buffer.m_address, (size > submit_available ? submit_available : size),
                (OVERLAPPED *)context, (LPOVERLAPPED_COMPLETION_ROUTINE)lruv_complete_request) == 0)
        {
            GetLastError();
        } else {
            unsigned long final_size = m_total_size - offset;
            if (size <= final_size)
                final_size = size;
            *bytes_read = final_size;
            result = true;
        }
    }
    return result;
}


// @address 0x70b8d0 asynchronous_io_arena_dispose
void asynchronous_io_arena_dispose(void)
{
    for (long file_index = 0; file_index < 32; file_index++) {
        g_asynchronous_io_arena.close_file(file_index);
    }
    CloseHandle(g_asynchronous_io_arena.completion_port);
}

#if !defined(_XBOX) || defined(__clang__)
// Dynamic initialization of the arena (the Xbox compiler emits it itself):
// clears the request count and file flags, opens the completion port
// and registers the arena's dispose at exit.
__attribute__((constructor)) static void asynchronous_io_arena_initialize(void)
{
    g_asynchronous_io_arena.outstanding_io_requests = 0;
    g_asynchronous_io_arena.valid_files.fill(32, 0);
    g_asynchronous_io_arena.completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
    atexit(asynchronous_io_arena_dispose);
}
#endif


// @address 0x21b158 cache_file_bulk_reader_update_hash_state
void cache_file_bulk_reader_update_hash_state(s_sound_cache_hash_state *state, c_sound_cache_reader *reader)
{
    if (!state->read_failed)
        cache_file_bulk_reader_update_reads(state, reader);
    if (!state->read_failed && !state->read_completed)
        cache_file_bulk_reader_update_hash(state);
    if (state->read_failed)
        cache_file_bulk_reader_finish_pending_reads(state, reader);
}

// @address 0x21b1d8 cache_file_bulk_reader_update_reads
extern "C" void cache_file_bulk_reader_update_reads(s_sound_cache_hash_state *state, void *reader)
{
    s_sound_cache_hash_state * state_loc = state;
    while (cache_file_bulk_reader_first_slot(state_loc->slots.mask_1) != -1 && state_loc->read_offset < state_loc->read_size)
    {
        s_cache_file_bulk_reader_slot_set *slots = &state_loc->slots;
        long index = cache_file_bulk_reader_first_slot(slots->mask_1);
        volatile unsigned char *mask_ptr = &slots->mask_1;
        unsigned long mask = 1 << index;
        *mask_ptr = *mask_ptr & (unsigned char)~mask;
        s_cache_file_bulk_reader_set_bits bits;
        bits.value = slots->bits.value;
        slots->slots[index].bits.index = bits.index;
        s_cache_file_bulk_reader_set_bits next_bits;
        next_bits.value = slots->bits.value;
        ++next_bits.index;
        slots->bits.value = next_bits.value;
        slots->slots[index].bits.offset = 0;
        unsigned long bytes_read;
        const s_cache_file_bulk_reader_slot *slot = &slots->slots[index];
        if (static_cast<c_sound_cache_reader *>(reader)->start_read(slot->buffer, state_loc->read_offset, &bytes_read, index))
            state_loc->read_offset += bytes_read;
        else
        {
            s_cache_file_bulk_reader_slot_set *failed_slots = &state_loc->slots;
            failed_slots->slots[index].bits.offset = 0;
            failed_slots->mask_2 |= mask;
            state_loc->read_failed = true;
        }
    }
    if (!state_loc->read_failed)
    {
        if (!state_loc->hash_valid && state_loc->hash_enabled)
            state_loc->hash_valid = security_initialize_hash(state_loc, 88, true);
        static_cast<c_sound_cache_reader *>(reader)->update(0);
    }
    else
        cache_file_bulk_reader_finish_pending_reads(state_loc, reader);
    if (state_loc->slots.stream_count != 0)
    {
        unsigned char stream_index = 0;
        do
        {
            if ((state_loc->slots.mask_1 & (1 << stream_index)) == 0 && (state_loc->slots.mask_2 & (1 << stream_index)) == 0)
            {
                unsigned long bytes_read;
                bool success;
                if (static_cast<c_sound_cache_reader *>(reader)->read_complete(stream_index, &bytes_read, &success))
                {
                    if (success)
                    {
                        s_cache_file_bulk_reader_slot_set *slots = &state_loc->slots;
                        long index = stream_index;
                        state_loc->read_bytes += bytes_read;
                        slots->slots[index].bits.offset = bytes_read;
                        slots->mask_2 |= (1 << index);
                    }
                    else
                        state_loc->read_failed = true;
                }
            }
            stream_index++;
        } while (stream_index < state_loc->slots.stream_count);
    }
}

// @address 0x21b438 cache_file_bulk_reader_update_hash
extern "C" void cache_file_bulk_reader_update_hash(s_sound_cache_hash_state *state)
{
    s_cache_file_bulk_reader_slot_set *slots = &state->slots;
    if (cache_file_tag_resource_find_ready_slot(slots) != -1)
    {
        long index = cache_file_tag_resource_find_ready_slot(slots);
        c_basic_buffer<void> output;
        long mask = 1 << index;
        slots->mask_2 &= ~mask;
        s_cache_file_bulk_reader_set_bits bits;
        bits.value = slots->bits.value;
        bits.offset++;
        slots->bits.value = bits.value;
        s_cache_file_bulk_reader_slot *slot = &slots->slots[index];
        s_cache_file_bulk_reader_slot_bits slot_bits;
        slot_bits.value = slot->bits.value;
        c_basic_buffer<void> buffer = slot->buffer;
        c_basic_buffer<void> request;
        request.m_address = buffer.m_address;
        request.m_size = slot_bits.offset;
        bool success = state->source->read(request, &output);
        state->hash_offset += request.m_size;
        if (!success)
        {
            slots->mask_2 &= ~mask;
            slots->mask_1 |= mask;
            state->read_failed = true;
        }
        else
        {
            slots->mask_2 &= ~mask;
            slots->mask_1 |= mask;
            if (state->hash_valid && output.m_size)
                XHashMemoryUpdate((_XHASH_STATE *)state, output.m_address, output.m_size);
        }
    }
    else if (state->hash_offset >= state->read_size)
    {
        c_basic_buffer<void> output;
        if (state->source->finish(&output))
        {
            if (state->hash_valid && output.m_size)
                XHashMemoryUpdate((_XHASH_STATE *)state, output.m_address, output.m_size);
            state->read_completed = true;
        }
        else
            state->read_failed = true;
    }
}
