#ifndef CSERIES_ASYNC_HELPERS_H
#define CSERIES_ASYNC_HELPERS_H

#include <stddef.h>
#include "main/levels_enumeration_task.h"

// Task payloads copied by async_task_add_ex (0x120 bytes).
// March async_queue_simple_callback also clears exactly 288 bytes.
#include "cseries/async_types.h"
#include "cseries/file_handle_types.h"
#include "cseries/async.h"
#include <string.h>

template<long k_capacity> class c_static_string;
struct s_file_reference;
struct s_queued_work_item;
struct s_queued_work_item_status_tracker;
struct s_online_file_buffered_writer;
struct s_blam_zlib_memory;
struct z_stream_s;
class c_http_client;
class c_online_files_active_transfer_tracker;
struct s_font_cache_file;
class c_synchronized_long;

// delete_file_callback and async_delete_file:
// a 256-byte path, directory flag at +0x100, completion pointer at +0x104.
struct s_delete_file_task
{
    char path[256];
    bool is_directory;
    c_synchronized_long *success_out;
};

struct s_font_loading_task
{
    s_font_cache_file *file;
};

struct s_work_item_transfer_task {
    s_file_reference *file_reference;
    s_file_reference *source_file;
    s_file_reference *destination_file;
    char flag_c;
    char flag_d;
    long first;
    long second;
    long *succeeded;
    long *cancelled;
    bool *error_forced;
    char *area;
    void get_base_filename(c_static_string<17> *filename);
};

struct s_online_file_enumerate_task {
    s_queued_work_item_status_tracker *transfers;
    long *transfer_count;
    long maximum_transfer_count;
    long *upstream_quota;
    long *transfer_priority;
    long *succeeded;
    long *cancelled;
};

struct s_online_file_upload_task {
    s_work_item_transfer_task transfer;
    long flags;
    char *upload_buffer;
    char *response_buffer;
    long request_cookie;
    unsigned long bytes_uploaded;
    unsigned long file_size;
    long request_id;
    s_queued_work_item *work_item;
    char *url;
    long url_size;
};

struct s_online_file_decompression_task {
    s_work_item_transfer_task transfer;
    union { long bytes_read; long state; };
    union { long bytes_written; unsigned long source_size; };
    union { long bytes_decompressed; unsigned long expected_size; };
    union { long source_handle; long chunk_size; };
    union { long destination_handle; long chunk_position; };
    union { long decompressor; unsigned long output_position; };
    void *buffer;
    long buffer_size;
    s_queued_work_item *work_item;
    s_blam_zlib_memory *zlib_memory;
    z_stream_s *stream;
};

struct s_online_files_tracker_enumerate_task {
    bool *success;
    volatile long *done;
    class c_online_files_active_transfer_tracker *tracker;
};

struct s_online_file_compression_task {
    s_work_item_transfer_task transfer;
    long state;
    unsigned long source_size;
    long chunk_size;
    unsigned long chunk_position;
    unsigned long compressed_position;
    char *buffer;
    long buffer_size;
    s_queued_work_item *work_item;
    s_blam_zlib_memory *zlib_memory;
    z_stream_s *stream;
    s_online_file_buffered_writer *writer;
};

struct s_online_file_download_task {
    s_work_item_transfer_task transfer;
    long mode;
    c_http_client *client;
    char *stream;
    long request_cookie;
    unsigned long flags;
    unsigned long compressed_size;
    unsigned long full_size;
    long request_id;
    s_queued_work_item *work_item;
    char *input;
    char *output;
    long output_capacity;
    s_online_file_buffered_writer *writer;
    unsigned long start_time;
};

struct s_async_task
{
    union
    {
        s_delete_file_task delete_file;
        s_levels_enumeration_task enumerate_levels;
        // Game preferences uses these callbacks with a full 0x120-byte task.
        struct { void *handle; } flush_file;
        struct
        {
            void *handle;
            void *buffer;
            long size;
            unsigned long offset;
            c_synchronized_long *bytes_written;
            unsigned long written;
            long flags;
            bool unbuffered;
        } write_position;
        struct { void *handle; unsigned long *file_size_out; } get_file_size;
        struct { void *handle; long size; c_synchronized_long *success_out; } set_file_size_request;
        struct { void *handle; char *buffer; unsigned long size; unsigned long position; c_synchronized_long *progress; unsigned long read; } read_position;
        struct { char path[256]; void *buffer; unsigned long size; long category; c_synchronized_long *completion; void *handle; } write_buffer;
        struct {
            char path[256];
            void *buffer;
            unsigned long buffer_size;
            volatile unsigned long *file_size_out;
            c_synchronized_long *success_out;
            void *handle;
            unsigned long file_size;
        } read_entire_file;
        struct { void *handle; unsigned long count; const void *buffer; unsigned char *result; } write_file;
        // The cache_files_windows wrappers and async callbacks share these payloads.
        struct {
            void *source;
            void *destination;
            unsigned long source_position;
            unsigned long destination_position;
            void *buffer;
            unsigned long size;
            c_synchronized_long *progress;
            unsigned long copied;
            long state;
            bool extra_flag;
        } copy_position;
        struct {
            void *handle;
            const void *buffer;
            unsigned long size;
            unsigned long file_size;
            bool flush;
            bool *success_out;
        } write_header;
#ifdef HALO3_64BIT
        char data[512];
        char storage[512];
#else
        char data[288];
        char storage[288];
#endif
        s_font_loading_task font_loading_task;
        struct { const s_queued_work_item *item; char filename[256]; volatile long *success; } enqueue_transfer;
        s_online_file_enumerate_task enumerate_files;
        s_online_file_upload_task upload_file;
        s_online_file_compression_task compress_file;
        s_online_file_download_task download_file;
        s_online_file_decompression_task decompress_file;
        s_online_files_tracker_enumerate_task enumerate_transfers;
        struct { char filename[17]; long priority; } bump_transfer_priority;
        struct { char filename[17]; } delete_transfer_files;
    };
};

typedef char async_copy_position_flag_offset[(sizeof(void *) != 4 || ((offsetof(s_async_task, copy_position.extra_flag) == 0x24))) ? 1 : -1];
typedef char async_write_header_result_offset[(sizeof(void *) != 4 || offsetof(s_async_task, write_header.success_out) == 0x14) ? 1 : -1];

// File-handle callbacks read the handle at +0 and the size output at +4.
struct s_async_callback_task
{
    void *handle;
    unsigned long *file_size_out;
};

typedef char async_write_position_flags_offset[(sizeof(void *) != 4 || offsetof(s_async_task, write_position.flags) == 0x18) ? 1 : -1];
typedef char async_write_position_unbuffered_offset[(sizeof(void *) != 4 || offsetof(s_async_task, write_position.unbuffered) == 0x1c) ? 1 : -1];
typedef char s_async_task_size[(sizeof(void *) != 4 || ((sizeof(s_async_task) == 288))) ? 1 : -1];
#if defined(_XBOX)
typedef char s_delete_file_task_size[(sizeof(void *) != 4 || ((sizeof(s_delete_file_task) == 0x108) )) ? 1 : -1];
typedef char s_delete_file_task_directory_offset[(offsetof(s_delete_file_task, is_directory) == 0x100) ? 1 : -1];
typedef char s_delete_file_task_success_offset[(sizeof(void *) != 4 || ((offsetof(s_delete_file_task, success_out) == 0x104) )) ? 1 : -1];
typedef char s_async_callback_task_size[(sizeof(void *) != 4 || ((sizeof(s_async_callback_task) == 8) )) ? 1 : -1];
typedef char s_async_callback_task_handle_offset[(offsetof(s_async_callback_task, handle) == 0) ? 1 : -1];
typedef char s_async_callback_task_file_size_out_offset[(sizeof(void *) != 4 || ((offsetof(s_async_callback_task, file_size_out) == 4) )) ? 1 : -1];

#endif

// Functions defined in tu/blamlibxbox/async_helpers.cpp.

class c_async_double_buffer;

extern "C" void async_double_buffer_flush(c_async_double_buffer *buffer);

struct s_async_task;

extern "C" int write_buffer_to_file_callback(s_async_task *task_base);

class c_synchronized_long;
extern "C" long async_read_entire_file(const char *path, void *buffer, unsigned long buffer_size,
    volatile unsigned long *file_size_out, e_async_category category, e_async_priority priority,
    c_synchronized_long *completion, c_synchronized_long *done);
extern "C" e_async_completion async_close_file_callback(s_async_task *task);
extern "C" e_async_completion async_close_file_no_flush_callback(s_async_task *task);
extern "C" e_async_completion get_file_size_callback(s_async_task *task);
extern "C" e_async_completion async_flush_file_callback(s_async_task *task);
extern "C" e_async_completion async_copy_file_callback(s_async_task *task);
e_async_completion async_read_position_callback(s_async_task *task);
extern "C" e_async_completion async_read_overlapped_callback(s_async_task *task);
e_async_completion async_write_position_callback(s_async_task *task);
e_async_completion set_file_size_callback(s_async_task *task);
extern "C" long async_read_position(void *handle, void *buffer, long size, unsigned long offset, long category, long priority,
    c_synchronized_long *bytes_read, c_synchronized_long *done);
e_async_completion async_create_file_callback(s_async_task *task);
e_async_completion async_simple_callback_task_callback(s_async_task *task);
long async_get_file_size(s_file_handle file_handle, volatile unsigned long *file_size_out, e_async_category category, e_async_priority priority,
    c_synchronized_long *done);
long async_set_file_size(s_file_handle file_handle, long size, e_async_category category, e_async_priority priority,
    c_synchronized_long *success_out, c_synchronized_long *done);
e_async_completion delete_file_callback(s_async_task *task);

static __forceinline long async_close_file(s_file_handle handle, long category, long priority, c_synchronized_long *done)
{
    s_async_task task;
    memset(&task, 0, sizeof(task));
    task.flush_file.handle = handle.handle;
    return async_task_add_ex((e_async_priority)priority, &task, (e_async_category)category, async_close_file_callback,
        done, true);
}

#endif // CSERIES_ASYNC_HELPERS_H
