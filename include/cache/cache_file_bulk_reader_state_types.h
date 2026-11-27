#ifndef CACHE_CACHE_FILE_BULK_READER_STATE_TYPES_H
#define CACHE_CACHE_FILE_BULK_READER_STATE_TYPES_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "cache/asynchronous_io_marker_types.h"
#include "cache/cache_file_bulk_reader_slot_bits_types.h"
#include "cache/cache_file_bulk_reader_set_bits_types.h"
#include "memory/basic_buffer.h"
#include "cache/sound_cache_hash_source.h"
#include "multithreading/synchronized_value.h"
#include "cache/sound_cache_file_reader.h"

class c_sound_cache_hash_source;

struct s_cache_file_bulk_reader_slot {
    // match: buffer initialization follows iterator startup.
    __forceinline s_cache_file_bulk_reader_slot() : buffer(c_basic_buffer<void>::_uninitialized) {}
    volatile s_cache_file_bulk_reader_slot_bits bits;
    c_basic_buffer<void> buffer;
    __forceinline void reset() { buffer.m_address = 0; buffer.m_size = 0; }
};

struct s_cache_file_bulk_reader_slot_set {
    volatile s_cache_file_bulk_reader_set_bits bits;
    unsigned char stream_count;
    volatile unsigned char mask_1;
    volatile unsigned char mask_2;
    s_cache_file_bulk_reader_slot slots[2];
};

STATIC_ASSERT(offsetof(s_cache_file_bulk_reader_slot_set, slots) == 8, bulk_reader_slots_offset);

struct s_sound_cache_hash_state {
    char hash_context[88];
    s_cache_file_bulk_reader_slot_set slots;
    unsigned long read_offset;
    unsigned long read_bytes;
    unsigned long hash_offset;
    unsigned long read_size;
    c_sound_cache_hash_source *source;
    unsigned char read_completed;
    unsigned char read_failed;
    signed char hash_valid;
    signed char hash_completed;
    signed char hash_enabled;
};

STATIC_ASSERT_ABI32(offsetof(s_sound_cache_hash_state, source) == 0x88, bulk_reader_hash_source_offset);
STATIC_ASSERT_ABI32(offsetof(s_sound_cache_hash_state, read_completed) == 0x8c, bulk_reader_hash_completed_offset);
STATIC_ASSERT_ABI32(offsetof(s_sound_cache_hash_state, read_failed) == 0x8d, bulk_reader_hash_failed_offset);
STATIC_ASSERT_ABI32(sizeof(s_sound_cache_hash_state) == 0x94, bulk_reader_hash_state_size);

#endif // CACHE_CACHE_FILE_BULK_READER_STATE_TYPES_H
