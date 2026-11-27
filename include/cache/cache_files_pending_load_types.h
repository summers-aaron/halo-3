#ifndef CACHE_CACHE_FILES_PENDING_LOAD_TYPES_H
#define CACHE_CACHE_FILES_PENDING_LOAD_TYPES_H

#include "cache/cache_file_header.h"
#include "cseries/cseries.h"
#include "multithreading/synchronized_value.h"

enum e_cache_file_copy_state
{
    _cache_file_copy_idle = 0,
    _cache_file_copy_open_source = 1,
    _cache_file_copy_source_opened = 2,
    _cache_file_copy_read_header = 3,
    _cache_file_copy_verify_header = 4,
    _cache_file_copy_get_size = 5,
    _cache_file_copy_verify_size = 6,
    _cache_file_copy_create_destination = 7,
    _cache_file_copy_destination_created = 8,
    _cache_file_copy_flush_destination = 9,
    _cache_file_copy_begin_copy = 10,
    _cache_file_copy_copy_chunk = 11,
    _cache_file_copy_chunk_copied = 12,
    _cache_file_copy_begin_stream = 13,
    _cache_file_copy_stream = 14,
    _cache_file_copy_stream_finished = 15,
    _cache_file_copy_flush_complete = 16,
    _cache_file_copy_write_header = 17,
    _cache_file_copy_header_written = 18,
    _cache_file_copy_complete = 19,
    _cache_file_copy_cancel = 20,
    _cache_file_copy_close_source = 21,
    _cache_file_copy_finish = 22,
};

enum e_cache_file_copy_result
{
    _cache_file_copy_pending = 0,
    _cache_file_copy_succeeded = 1,
    _cache_file_copy_cancelled = 2,
    _cache_file_copy_failed = 3
};

struct s_cache_files_windows_cached_map {
    c_static_string<256> name;
    long level;
};

struct s_cache_files_windows_pending_load {
    s_cache_file_header header;
    c_synchronized_long transfer_size;
    long copy_start_time;
    long copy_size;
    long bytes_done;
    unsigned char destination_created;
    bool header_written;
    volatile unsigned long bytes_total;
    long task_id;
    long previous_state;
    long countdown;
    long phase;
    c_synchronized_long task_done;
    long destination_file_index;
    void *source_file_handle;
    c_synchronized_long armed;
    bool stream_success;
    char *copy_buffer;
    long copy_buffer_size;
    bool release_copy_buffer;
    c_static_string<256> m_current_map_name;
    s_cache_files_windows_cached_map m_cached_maps[2];
    long checksum;
};

typedef char pending_load_countdown_offset_check[(offsetof(s_cache_files_windows_pending_load, countdown) == 0x3020) ? 1 : -1];
typedef char pending_load_armed_offset_check[(sizeof(void *) != 4 || ((offsetof(s_cache_files_windows_pending_load, armed) == 0x3034))) ? 1 : -1];
STATIC_ASSERT(offsetof(s_cache_files_windows_pending_load, bytes_total) == 0x3014, pending_load_bytes_total_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_files_windows_pending_load, copy_buffer) == 0x303c, pending_load_copy_buffer_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_files_windows_pending_load, m_cached_maps) == 0x3148, pending_load_cached_maps_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_files_windows_pending_load, checksum) == 0x3350, pending_load_checksum_offset);
STATIC_ASSERT_ABI32(sizeof(s_cache_files_windows_pending_load) == 0x3358, pending_load_size);

#endif
