// @address 0x59f830 ReadFileEx
// blamlibxbox/async_helpers.obj: from the records 0xa6598 0xa67d8 0x198870 0x1ed188 0x1ed280 0x1ed558 0x1ed5d0 0x1ed668 0x1ed718 0x1ed7a8 0x1ed870 0x1ed928 0x1ed990 0x1eda88 0x1edaa0 0x1edc90 0x1eddc0 0x1edef8 0x1edff0 0x1ee078 0x1ee278 0x1ee360 0x1ee518 0x1ee690 0x1ee750 0x1ee9b0 0x1eea08 0x1eead0 0x1eeb60 0x1eec10 0x1eecc0 0x1eed68 0x1eef30 0x1ef0f8 0x1ef200 0x1ef398 0x1ef4c0 0x1ef6c8 0x1ef790 0x1ef810 0x1ef998 0x1efb90 0x1efbe0 0x1efc20 0x1efc68
#include "cseries/async_block.h"
// @flags /TP /O2 /Oc
// @address 0x59e508 SleepEx
// @address 0x1ed280 ?async_create_file@@YAXPBDKW4e_file_open_disposition@@KW4e_async_category@@W4e_async_priority@@PAUs_file_handle@@PAUs_create_file_task@@@Z
// @address 0x1ed990 ?async_compare_tasks@@YA_NPIAUs_compare_tasks_data@@P6A?AW4e_async_completion@@PAUs_async_task@@@Z2W4e_async_priority@@3PBU3@4@Z
// @address 0x1edff0 ?release_storage@c_async_double_buffer@@QAAXPAVc_allocation_base@@@Z
// @address 0x1eea08 ?flush@c_async_double_buffer@@QAA_NXZ
// @address 0x1ef790 ?delete_file_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0xa67d8 ?ready_to_write@c_async_double_buffer@@QAA_NXZ
// @address 0x1edef8 ?allocate_storage@c_async_double_buffer@@QAA_NPAVc_allocation_base@@J@Z
// @address 0x1ee9b0 ?ready_to_read@c_async_double_buffer@@QAA_NXZ
// @address 0x1eeb60 ?initialize_internal@c_async_double_buffer@@AAAXXZ
// @address 0x1eda88 ?async_simple_callback_task_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x1ef6c8 ?set_file_size_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x1ef0f8 ?async_read_position_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x1ed668 ?async_write_position_ex@@YAJPAX0JKV?$c_flags@W4e_write_position_flags@@K$00@@JJ_NPAVc_synchronized_long@@3@Z
// @address 0x1ee278 ?close_file@c_async_double_buffer@@QAA_NXZ
// @address 0x1ee690 ?prime_buffers_for_reading@c_async_double_buffer@@AAAXXZ
// @address 0x1ee750 ?set_position@c_async_double_buffer@@QAA_NJ@Z
// @address 0x1eec10 ?async_write_buffer@c_async_double_buffer@@AAAXJ@Z
// @address 0x1eecc0 ?async_read_buffer@c_async_double_buffer@@AAAXJ@Z
// @address 0x1eef30 ?async_create_file_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x1ef998 write_buffer_to_file_callback
// @address 0x1edc90 async_write_file_callback
// @address 0x1ed928 ?async_get_file_size@@YAJUs_file_handle@@PCKW4e_async_category@@W4e_async_priority@@PAVc_synchronized_long@@@Z
// @address 0x1ed718 ?async_set_file_size@@YAJUs_file_handle@@JW4e_async_category@@W4e_async_priority@@PAVc_synchronized_long@@3@Z
#include <xtl.h>
#include "tag_files/file_open_disposition_types.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "cseries/async_helpers_internal.h"
#include "shell/utility_drive.h"
#include "multithreading/threads.h"
#include "memory/data.h"
#include "cseries/async.h"

#include "multithreading/synchronized_value.h"
#include "cseries/file_handle_types.h"
#include "cseries/async_buffer_disposition_types.h"
#include "cseries/create_file_task_types.h"
#include "cseries/write_position_flags_types.h"
#include "cseries/compare_tasks_data_types.h"
#include "cseries/async_buffer_file_access_types.h"
#include "cseries/async_double_buffer.h"

#include "cseries/async_read_context.h"
#include "cseries/async_io_error.h"
#include "cache/cache_file_bulk_reader_callbacks.h"
#include "tag_files/file_reference.h"
#include "cseries/pointer_utilities.h"

extern "C" {
// @address 0x7db1d4 g_async_file_io_error
unsigned long g_async_file_io_error;
// @address 0x8a1ead g_utility_drive_mounted
bool g_utility_drive_mounted;
}

struct s_async_read_overlapped_task
{
    void *handle;
    char *buffer;
    unsigned long size;
    unsigned long position;
    c_synchronized_long *progress;
    unsigned long total_bytes_read;
    s_async_read_context context;
    s_async_read_completion completion;
    bool pending;
};

struct s_async_helpers_write_buffer_task
{
    char path[256];
    const char *buffer;
    unsigned long size;
    long direct;
    c_synchronized_long *success_out;
    void *handle;
};

typedef c_flags<e_write_position_flags, unsigned long, k_write_position_flags_count> c_write_position_flags;

namespace {
struct s_async_copy_task
{
    void *source,*destination;
    unsigned long source_position,destination_position;
    char *buffer;
    unsigned long size;
    c_synchronized_long *progress;
    unsigned long copied;
    long state;
};
struct s_async_file_task
{
    char path[256];
    void *buffer;
    unsigned long buffer_size;
    volatile unsigned long *file_size_out;
    c_synchronized_long *success_out;
    void *handle;
    unsigned long file_size;
};
}

namespace {
struct s_async_helpers_simple_callback_task
{
    e_async_completion (*callback)(s_async_task *, void *, long);
#ifdef HALO3_64BIT
    char data[500];
#else
    char data[282];
#endif
    short data_size;
};

struct s_async_helpers_set_size_task
{
    void *handle;
    long size;
    c_synchronized_long *success_out;
};
}

namespace {
struct s_async_helpers_write_header_task
{
    void *handle;
    const void *buffer;
    unsigned long size;
    unsigned long file_size;
    bool flush;
    bool *success_out;
};
}

namespace {
struct s_async_helpers_write_file_task
{
    void *handle;
    unsigned long count;
    const void *buffer;
    unsigned char *result;
};
}

namespace {
struct s_async_copy_file_task
{
    long state;
    void *source;
    void *destination;
    unsigned long file_size;
    unsigned long count;
    unsigned long copied;
    char *buffer;
    unsigned long buffer_size;
    bool flush;
    c_synchronized_long *done;
};
}

namespace {
struct s_async_read_position_task
{
    void *handle;
    char *buffer;
    unsigned long size;
    unsigned long position;
    c_synchronized_long *progress;
    unsigned long read;
};
}

namespace {
struct s_async_write_position_task
{
    void *handle;
    char *source;
    unsigned long source_size;
    unsigned long position;
    c_synchronized_long *bytes_written;
    unsigned long written;
    unsigned long flags;
};
}

namespace {
struct s_write_position_task
{
    void *handle;
    void *buffer;
    long size;
    unsigned long offset;
    c_synchronized_long *bytes_written;
    long written;
    c_write_position_flags flags;
    bool unbuffered;
    // Remainder of the task payload copied by async_task_add_ex (0x120 bytes on 32-bit, 512 bytes on 64-bit).
#ifdef HALO3_64BIT
    char task_payload_storage[512];
#else
    char task_payload_storage[259];
#endif
};
}

namespace {

struct s_async_fixed_write_task
{
    s_async_fixed_write_task() : gate(0) {}

    void *handle;
    void *buffer;
    long size;
    unsigned long offset;
    c_synchronized_long *progress;
    unsigned long total_bytes_read;
    s_async_read_context context;
    c_synchronized_long gate;
    // Remainder of the task payload copied by async_task_add_ex (0x120 bytes on 32-bit, 512 bytes on 64-bit).
#ifdef HALO3_64BIT
    char task_payload_storage[512];
#else
    char task_payload_storage[240];
#endif
};
}

static __forceinline void async_helpers_invalidate_file_handle(s_file_handle *file_handle)
{
    file_handle->handle = (void *)-1;
}

class c_async_double_buffer;

static __forceinline bool async_helpers_file_handle_valid(const c_async_double_buffer *buffer)
{
    return buffer->handle.handle != 0 && buffer->handle.handle != (void *)-1;
}

__forceinline bool c_async_double_buffer::writable() const
{
    return async_helpers_file_handle_valid(this) && (state == 0 || state == 2);
}

__forceinline bool c_async_double_buffer::readable() const
{
    return async_helpers_file_handle_valid(this) && (state == 1 || state == 2);
}

// one step of an async file copy.
// one step of an async whole-file read.


static __forceinline unsigned long async_helpers_seek(void *handle,unsigned long position){long high=0;return SetFilePointer(handle,position,&high,0);}
static __forceinline void async_helpers_report_io_error(bool success){if(!success)g_async_file_io_error=GetLastError();}
static __forceinline int async_helpers_read_file(void *handle, void *buffer, unsigned long size, unsigned long *bytes_read) {
    int success = ReadFile(handle, buffer, size, bytes_read, 0);
    if (!success) g_async_file_io_error = GetLastError();
    return success;
}

struct s_async_task;


bool c_async_double_buffer::ready_to_write()
{
    return async_helpers_file_handle_valid(this) && (state == 0 || state == 2);
}

// @address 0x198870 close_file_callback
extern "C" int close_file_callback(void)
{
    if (g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
    return 1;
}

e_async_completion async_simple_callback_task_callback(s_async_task *task)
{
    s_async_helpers_simple_callback_task *callback_task = (s_async_helpers_simple_callback_task *)task;
    return callback_task->callback(task, callback_task->data, callback_task->data_size);
}


static __forceinline bool async_helpers_handle_valid(void *handle)
{
    return handle != 0 && handle != (void *)-1;
}

// @address 0x1eddc0 async_write_header_callback
extern "C" int async_write_header_callback(s_async_helpers_write_header_task *task)
{
    bool success = false;
    bool size_set = false;
    if (async_helpers_handle_valid(task->handle))
    {
        FILE_END_OF_FILE_INFO end_of_file;
        end_of_file.EndOfFile.QuadPart = task->file_size;
        if (SetFileInformationByHandle(task->handle, FileEndOfFileInfo, &end_of_file, sizeof(end_of_file)))
            size_set = true;
    }
    if (size_set)
    {
        if (async_helpers_seek(task->handle, 0) == -1)
        {
            async_helpers_report_io_error(false);
        }
        else
        {
            unsigned long bytes_written;
            bool written = WriteFile(task->handle, task->buffer, task->size, &bytes_written, 0) != 0;
            async_helpers_report_io_error(written);
            if (written && bytes_written == task->size)
                success = true;
        }
    }
    if (success && task->flush && g_utility_drive_mounted)
        XFlushUtilityDrive();
    *task->success_out = success;
    return 1;
}


extern "C" long async_write_file_callback(s_async_helpers_write_file_task *task)
{
    bool done = false;
    bool ready = false;
    if (async_helpers_handle_valid(task->handle))
    {
        FILE_END_OF_FILE_INFO end_of_file;
        end_of_file.EndOfFile.QuadPart = 0;
        if (SetFileInformationByHandle(task->handle, FileEndOfFileInfo, &end_of_file, sizeof(end_of_file)) != 0)
            ready = true;
    }
    if (ready)
    {
        if (async_helpers_seek(task->handle, 0) == -1)
        {
            async_helpers_report_io_error(false);
        }
        else
        {
            unsigned long bytes_written;
            bool written = WriteFile(task->handle, task->buffer, task->count, &bytes_written, 0) != 0;
            async_helpers_report_io_error(written);
            if (written && bytes_written == task->count)
            {
                if (g_utility_drive_mounted != 0)
                    XFlushUtilityDrive();
                done = true;
            }
        }
    }
    *task->result = done;
    return 1;
}


// @address 0x1edaa0 async_copy_file_callback
extern "C" e_async_completion async_copy_file_callback(s_async_task *task_base)
{
    s_async_copy_file_task *task = (s_async_copy_file_task *)task_base;
    bool failed = false;
    bool finished = false;
    switch (task->state)
    {
    case 0:
        task->state = 1;
        task->file_size = GetFileSize(task->source, 0);
        failed = async_helpers_seek(task->source, 0) != 0;
        break;
    case 1:
        task->state = 2;
        failed = async_helpers_seek(task->destination, 0) != 0;
        break;
    case 2:
        failed = ReadFile(task->source, task->buffer, task->buffer_size, &task->count, 0) == 0;
        task->state = 3;
        break;
    case 3:
    {
        unsigned long written;
        failed = WriteFile(task->destination, task->buffer, task->count, &written, 0) == 0;
        task->copied += written;
        task->state = 2;
        if (task->count < task->buffer_size)
            task->state = 4;
        break;
    }
    case 4:
        finished = true;
        break;
    default:
        // match: 0x1edaa0: unreachable hint omits dead branch check
        __assume(0);
    }
    if (failed)
    {
        GetLastError();
        task->done->set(0);
        finished = true;
    }
    if (finished)
    {
        CloseHandle(task->source);
        CloseHandle(task->destination);
        if (task->flush && g_utility_drive_mounted != 0)
            XFlushUtilityDrive();
        task->done->set(!failed);
    }
    return finished ? _async_completion_done : _async_completion_pending;
}


bool c_async_double_buffer::allocate_storage(c_allocation_base *allocator, long buffer_size)
{
    bool failed = false;
    for (long index = 0; index < get_buffer_count(); ++index)
    {
        s_async_buffer *buffer = &buffers[index];
        buffer->data = allocator->allocate(buffer_size, "dbl buffer");
        buffer->initialized = false;
        if (!buffer->data)
        {
            failed = true;
            break;
        }
        else
        {
            buffer->capacity = buffer_size;
        }
    }
    if (failed)
    {
        for (long index = 0; index < get_buffer_count(); ++index)
        {
            s_async_buffer *buffer = &buffers[index];
            if (buffer->data)
                allocator->deallocate(buffer->data);
            buffer->capacity = 0;
        }
    }
    storage_initialized = !failed;
    return storage_initialized;
}

bool c_async_double_buffer::ready_to_read()
{
    return async_helpers_file_handle_valid(this) && (state == 1 || state == 2);
}

void c_async_double_buffer::initialize_internal()
{
    error = false;
    state = -1;
    current_buffer = -1;
    file_initialized = false;
    file_position = 0;
    requested_position = 0;
    total_transferred = 0;
    pending_buffer = 0;
    end_of_file = false;
    async_helpers_invalidate_file_handle(&handle);
    for (long index = 0; index < get_buffer_count(); ++index)
        initialize_buffer(index);
}


e_async_completion async_read_position_callback(s_async_task *task)
{
    s_async_read_position_task *read_task = (s_async_read_position_task *)task;
    unsigned long transfer_limit = 0x20000;
    unsigned long transfer_size = read_task->size - read_task->read > transfer_limit ? transfer_limit : read_task->size - read_task->read;
    unsigned long bytes_read = 0;
    unsigned long position = async_helpers_seek(read_task->handle, read_task->position);
    if (position == -1)
    {
        async_helpers_report_io_error(false);
    }
    else
    {
        char *buffer = read_task->buffer + read_task->read;
        async_helpers_report_io_error(ReadFile(read_task->handle, buffer, transfer_size, &bytes_read, 0) != 0);
        read_task->read += bytes_read;
        read_task->position += bytes_read;
        if (read_task->progress)
            read_task->progress->set(read_task->read);
    }
    return (e_async_completion)(read_task->size == read_task->read || bytes_read < transfer_size);
}


// @address 0x1ef200 async_read_overlapped_callback
extern "C" e_async_completion async_read_overlapped_callback(s_async_task *task)
{
    s_async_read_overlapped_task *read_task = (s_async_read_overlapped_task *)task;
    e_async_completion result = _async_completion_done;
    char *buffer = read_task->buffer + read_task->total_bytes_read;
    if (!read_task->pending)
    {
        unsigned long transfer_size = read_task->size - read_task->total_bytes_read;
        if (transfer_size > 0x20000UL)
            transfer_size = 0x20000UL;
        s_async_read_context *context = &read_task->context;
        s_async_read_completion *completion = &read_task->completion;
        context->offset = read_task->position;
        context->completion = completion;
        context->offset_high = 0;
        completion->requested_size = transfer_size;
        completion->done.set(0);
        bool success = ReadFileEx(read_task->handle, buffer, transfer_size, (OVERLAPPED *)context,
            (LPOVERLAPPED_COMPLETION_ROUTINE)lruv_complete_request) != 0;
        async_helpers_report_io_error(success);
        if (success)
        {
            read_task->pending = true;
            result = _async_completion_pending;
        }
        else
            return _async_completion_done;
    }
    else
    {
        if (!read_task->completion.done.peek())
            SleepEx(0, 1);
        long completed = read_task->completion.done.peek();
        if (completed && !read_task->completion.error)
        {
                read_task->total_bytes_read += read_task->completion.bytes_read;
                read_task->position += read_task->completion.bytes_read;
                if (read_task->progress)
                    read_task->progress->set(read_task->total_bytes_read);
                if (read_task->total_bytes_read == read_task->size ||
                    read_task->completion.requested_size != read_task->completion.bytes_read)
                    return _async_completion_done;
                read_task->pending = false;
                result = _async_completion_pending;
        }
        else if (!completed)
            return _async_completion_pending;
    }
    // match: 0x1ef200: zero-bit rotate preserves 32-bit GPR value
    return (e_async_completion)_rotl((unsigned int)result, 0);
}


// @address 0x1ef398 ?async_write_position_callback@@YA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion async_write_position_callback(s_async_task *task)
{
    s_async_write_position_task *write_task = (s_async_write_position_task *)task;
    e_async_completion result = _async_completion_pending;
    unsigned long bytes_written = (unsigned long)-1;
    long high = 0;
    if (SetFilePointer(write_task->handle, write_task->position, &high, 0) == -1)
    {
        g_async_file_io_error = GetLastError();
        result = _async_completion_done;
    }
    else
    {
        unsigned long remaining = write_task->source_size - write_task->written;
        unsigned long transfer_size = remaining <= 0x20000 ? remaining : 0x20000;
        char *source = write_task->source + write_task->written;
        if (WriteFile(write_task->handle, source, transfer_size, &bytes_written, 0) == 0)
            g_async_file_io_error = GetLastError();
        write_task->written += bytes_written;
        task->write_position.offset += bytes_written;
        if (write_task->source_size == write_task->written || bytes_written < transfer_size)
        {
            if (write_task->bytes_written)
                write_task->bytes_written->set(write_task->written);
            result = _async_completion_done;
        }
    }
    if (result == _async_completion_done && (write_task->flags & 1))
        FlushFileBuffers(write_task->handle);
    return result;
}

// @address 0x1ef4c0 async_copy_position_callback
extern "C" long async_copy_position_callback(s_async_copy_task *task){
 long complete=0;
 s_async_copy_task *copy=task;
 unsigned long transfer_limit=0x20000;
 unsigned long transfer_size=copy->size-copy->copied>transfer_limit?transfer_limit:copy->size-copy->copied;
 if(copy->state==0){
  unsigned long bytes_read=0;
  unsigned long position=async_helpers_seek(copy->source,copy->source_position);
  async_helpers_report_io_error(position!=-1);
  if(position!=-1){
   int success=ReadFile(copy->source,copy->buffer+copy->copied,transfer_size,&bytes_read,0);
   async_helpers_report_io_error(success && transfer_size==bytes_read);
   bytes_read=success?bytes_read:0;
   copy->source_position+=bytes_read;
  }
  if(bytes_read<transfer_size)complete=1;
  copy->state=1;
 }else{
  unsigned long bytes_written=0;
  unsigned long position=async_helpers_seek(copy->destination,copy->destination_position);
  async_helpers_report_io_error(position!=-1);
  if(position!=-1){
   int success=WriteFile(copy->destination,copy->buffer+copy->copied,transfer_size,&bytes_written,0);
   async_helpers_report_io_error(success && transfer_size==bytes_written);
   bytes_written=success?bytes_written:0;
   copy->copied+=bytes_written;
   copy->destination_position+=bytes_written;
  }
  if(copy->size==copy->copied || bytes_written<transfer_size)complete=1;
  copy->state=0;
 }
 if(copy->progress)copy->progress->set(copy->copied);
 return complete;
}

e_async_completion set_file_size_callback(s_async_task *task)
{
    s_async_helpers_set_size_task *set_size = (s_async_helpers_set_size_task *)task;
    bool success = -1 != async_helpers_seek(set_size->handle, set_size->size);
    async_helpers_report_io_error(success);
    if (success)
    {
        success = SetEndOfFile(set_size->handle) != 0;
        async_helpers_report_io_error(success);
    }
    if (set_size->success_out)
        *set_size->success_out = success;
    return _async_completion_done;
}


// @address 0x1ef810 read_entire_file_callback
extern "C" int read_entire_file_callback(s_async_file_task *task) {
    bool complete = false;
    bool success = true;
    if (task->handle == (void *)-1) {
        task->handle = CreateFileA(task->path, 0x80000000, 1, 0, 3, 128, 0);
        success = task->handle != 0 && task->handle != (void *)-1;
    } else if (task->file_size == -1) {
        task->file_size = GetFileSize(task->handle, 0);
        success = -1 != task->file_size;
        if (success && task->file_size_out) *task->file_size_out = task->file_size;
    } else {
        unsigned long size = task->file_size > task->buffer_size ? task->buffer_size : task->file_size;
        unsigned long bytes_read;
        int read_success = async_helpers_read_file(task->handle, task->buffer, size, &bytes_read);
        success = read_success && bytes_read == size;
        complete = true;
    }
    if (!success || complete) {
        if (task->handle != (void *)-1) CloseHandle(task->handle);
        if (task->success_out) *task->success_out = success;
    }
    int result = complete || !success ? 1 : 0;
    return result;
}

// one step of an async write of a buffer to a file: open, write (the last block padded to 4096), close.
extern "C" int write_buffer_to_file_callback(s_async_task *task_base)
{
    s_async_helpers_write_buffer_task *task = (s_async_helpers_write_buffer_task *)task_base;
    bool complete = false;
    bool success = true;
    if (task->handle == (void *)-1)
    {
        task->handle = CreateFileA(task->path, 0x40000000, 0, 0, 2, 128, 0);
        success = task->handle != (void *)-1;
    }
    else
    {
        if (task->size > 0)
        {
            unsigned long bytes_written;
            unsigned long aligned_size = task->size & ~4095;
            if (task->direct && aligned_size == task->size)
            {
                success = WriteFile(task->handle, task->buffer, task->size, &bytes_written, 0) && bytes_written == task->size;
            }
            else
            {
                if (aligned_size == 0 || (WriteFile(task->handle, task->buffer, aligned_size, &bytes_written, 0) && bytes_written == aligned_size))
                {
                    char last_block[4096];
                    unsigned long remainder = task->size - aligned_size;
                    memcpy(last_block, task->buffer + aligned_size, remainder);
                    memset(last_block + remainder, 0, 4096 - remainder);
                    success = WriteFile(task->handle, last_block, 4096, &bytes_written, 0) && bytes_written == 4096;
                }
            }
        }
        complete = true;
    }
    if (!success || complete)
    {
        if (task->handle != (void *)-1)
        {
            CloseHandle(task->handle);
            if (file_is_cache_path(task->path) && g_utility_drive_mounted)
                XFlushUtilityDrive();
        }
        if (task->success_out)
            *task->success_out = success;
    }
    long result = (!complete && success) ? 0 : 1;
    return result;
}

// @address 0x1efb90 async_close_file_callback
extern "C" e_async_completion async_close_file_callback(s_async_task *task_base)
{
    s_async_callback_task *task = (s_async_callback_task *)task_base;
    if (CloseHandle(task->handle) == 0)
        g_async_file_io_error = GetLastError();
    if (g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
    return _async_completion_done;
}

// @address 0x1efbe0 async_close_file_no_flush_callback
extern "C" e_async_completion async_close_file_no_flush_callback(s_async_task *task_base)
{
    s_async_callback_task *task = (s_async_callback_task *)task_base;
    if (CloseHandle(task->handle) == 0)
        g_async_file_io_error = GetLastError();
    return _async_completion_done;
}

// @address 0x1efc20 get_file_size_callback
extern "C" e_async_completion get_file_size_callback(s_async_task *task_base)
{
    s_async_callback_task *task = (s_async_callback_task *)task_base;
    *task->file_size_out = GetFileSize(task->handle, 0);
    return _async_completion_done;
}

// @address 0x1efc68 async_flush_file_callback
extern "C" e_async_completion async_flush_file_callback(s_async_task *task_base)
{
    s_async_callback_task *task = (s_async_callback_task *)task_base;
    if (FlushFileBuffers(task->handle) == 0)
        g_async_file_io_error = GetLastError();
    return _async_completion_done;
}


long async_write_position_ex(void *handle, void *buffer, long size, unsigned long offset, c_write_position_flags flags, long category,
    long priority, bool unbuffered, c_synchronized_long *bytes_written, c_synchronized_long *done)
{
    s_write_position_task task;
    if (bytes_written)
        bytes_written->set(0);
    memset(&task, 0, sizeof(task));
    task.handle = handle;
    task.buffer = buffer;
    task.size = size;
    task.offset = offset;
    task.bytes_written = bytes_written;
    task.flags = flags;
    task.unbuffered = unbuffered;
    return async_task_add_ex((e_async_priority)priority, (s_async_task *)&task, (e_async_category)category, async_write_position_callback,
        (c_synchronized_long *)done, 1);
}

// @address 0x1ed870 async_write_buffer_to_file
extern "C" long async_write_buffer_to_file(const char *path, const void *buffer, unsigned long size, long category, long priority,
    c_synchronized_long *unused_sync, c_synchronized_long *completion, void *done)
{
    if (completion != 0)
    {
        __lwsync();
        _InterlockedExchange(&completion->value, 0);
    }
    s_async_task task;
    memset(&task, 0, sizeof(task));
    strncpy(task.write_buffer.path, path, 256);
    task.write_buffer.path[255] = 0;
    task.write_buffer.buffer = (void *)buffer;
    task.write_buffer.size = size;
    task.write_buffer.category = category;
    task.write_buffer.completion = completion;
    task.write_buffer.handle = (void *)-1;
    return async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&task, (e_async_category)priority,
        (e_async_completion (*)(s_async_task *))write_buffer_to_file_callback, (c_synchronized_long *)done, 1);
}

static __forceinline bool async_helpers_async_usable()
{
    bool result;
    switch (g_thread_crashed[3].peek())
    {
    case 1:
        result = false;
        break;
    default:
        result = true;
        break;
    }
    return result;
}


// @address 0x1ed558 async_read_position
extern "C" long async_read_position(void *handle, void *buffer, long size, unsigned long offset, long category, long priority,
    c_synchronized_long *bytes_read, c_synchronized_long *done)
{
    s_async_task request;
    memset(&request, 0, sizeof(request));
    request.read_position.handle = handle;
    request.read_position.buffer = (char *)buffer;
    request.read_position.size = size;
    request.read_position.position = offset;
    request.read_position.progress = bytes_read;
    return async_task_add_ex((e_async_priority)priority, (s_async_task *)&request, (e_async_category)category, async_read_position_callback,
        (c_synchronized_long *)done, 1);
}

// @address 0x1ed5d0 async_read_header
extern "C" long async_read_header(void *handle, void *buffer, long size, unsigned long offset, long category, long priority,
    c_synchronized_long *progress, c_synchronized_long *done)
{
    s_async_fixed_write_task task;
    task.gate.set(0);
    memset(&task, 0, sizeof(task));
    task.handle = handle;
    task.buffer = buffer;
    task.offset = 0;
    task.progress = progress;
    task.size = 0x3000;
    return async_task_add_ex((e_async_priority)priority, (s_async_task *)&task, _async_category_cache_files, async_read_overlapped_callback,
        (c_synchronized_long *)done, 0);
}

bool c_async_double_buffer::close_file()
{
    bool result = false;
    if (async_helpers_async_usable() && async_helpers_handle_valid(handle.handle))
    {
        complete_pending_async_buffer_activity();
        c_synchronized_long done(0);
        s_file_handle file = handle;
        if (async_close_file(file, 0, 9, &done) != -1)
        {
            async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
            result = true;
        }
    }
    initialize_internal();
    return result;
}


__forceinline bool c_async_double_buffer::handle_read_state()
{
    bool result = false;
    bool usable = async_helpers_async_usable();
    if (usable)
    {
        unsigned long current_state = state;
        if (current_state >= 1)
        {
            long buffer_index = current_buffer;
            if (current_state != 1 && buffer_index != 0)
            {
                complete_pending_async_buffer_activity();
                pending_buffer = 0;
                file_position = requested_position;
                prime_buffers_for_reading();
                current_buffer = 0;
            }
            result = true;
        }
    }
    return result;
}

__forceinline long c_async_double_buffer::fill_bytes(void *destination, long size)
{
    long index = pending_buffer;
    s_async_buffer *buffer = &buffers[index];

    long available = buffer->pending.peek() - buffer->used;
    long bytes = size <= available ? size : available;
    if (bytes > 0)
    {
        memcpy(destination, (char *)buffer->data + buffer->used, bytes);
        buffer->used += bytes;
    }
    else if (buffer->pending.peek() != buffer->capacity)
    {
        file_initialized = true;
    }
    else
    {
        swap_buffers();
    }
    return bytes;
}

// @address 0x1ee360 ?read@c_async_double_buffer@@QAAXPAXJPAJ@Z
// match: 0x1ee360: pointer __restrict qualifier prevents alias reload
void c_async_double_buffer::read(void *destination, long size, long *__restrict bytes_read)
{
    *bytes_read = 0;
    if (async_helpers_async_usable() && ready_to_read() && handle_read_state())
    {
        while (size > 0)
        {
            long bytes = fill_bytes(destination, size);
            destination = offset_pointer(destination, bytes);
            size -= bytes;
            *bytes_read += bytes;
            if (end_of_file)
            {
                close_file();
                break;
            }
            if (bytes == 0 && file_initialized)
                break;
        }
        requested_position += *bytes_read;
    }
}


__forceinline long c_async_double_buffer::empty_bytes(const void *source, long size)
{
    long index = pending_buffer;
    s_async_buffer *buffer = &buffers[index];

    long available = buffer->capacity - buffer->position;
    long bytes = size <= available ? size : available;
    if (bytes > 0)
    {
        memcpy((char *)buffer->data + buffer->position, source, bytes);
        buffer->position += bytes;
    }
    else
    {
        swap_buffers();
    }
    return bytes;
}

// @address 0xa6598 ?write@c_async_double_buffer@@QAAXPBXJPAJ@Z
// match: 0xa6598: pointer __restrict qualifier prevents alias reload
void c_async_double_buffer::write(const void *source, long size, long *__restrict bytes_written)
{
    *bytes_written = 0;
    if (async_helpers_async_usable() && ready_to_write() && handle_write_state())
    {
        if (error)
        {
            s_async_buffer *buffer = &buffers[pending_buffer];
            async_block_until_done(&buffer->complete, simple_yield_function, 0, 0, 0, 0);
            if (buffer->transferred != buffer->pending.peek())
                end_of_file = true;
            else
                initialize_buffer(pending_buffer);
            error = false;
        }
        while (size > 0)
        {
            long bytes = empty_bytes(source, size);
            source = offset_pointer((void *)source, bytes);
            size -= bytes;
            *bytes_written += bytes;
            if (end_of_file)
                break;
        }
        if (end_of_file)
            close_file();
        else
            requested_position += *bytes_written;
    }
}

void c_async_double_buffer::complete_pending_async_buffer_activity()
{
    if (current_buffer == 1)
    {
        for (long index = 0; index < get_buffer_count(); index++)
        {
            s_async_buffer *buffer = &buffers[index];
            if (buffer->complete.peek() != 0 && buffer->position > 0 && buffer->transferred == 0)
                async_write_buffer(index);
            if (buffer->complete.peek() == 0)
            {
                async_block_until_done(&buffer->complete, simple_yield_function, 0, 0, 0, 0);
                if (buffer->transferred != buffer->pending.peek())
                    end_of_file = true;
                else
                    initialize_buffer(index);
            }
        }
    }
    else if (current_buffer == 0)
    {
        for (long index = 0; index < get_buffer_count(); index++)
            async_block_until_done(&buffers[index].complete, simple_yield_function, 0, 0, 0, 0);
    }
}

void c_async_double_buffer::prime_buffers_for_reading()
{
    if (async_helpers_async_usable() && ready_to_read())
    {
        complete_pending_async_buffer_activity();
        for (long index = 0; index < get_buffer_count(); index++)
            async_read_buffer(index);
        async_block_until_done(&buffers[0].complete, simple_yield_function, 0, 0, 0, 0);
    }
}

bool c_async_double_buffer::set_position(long position)
{
    bool result = false;
    if (async_helpers_async_usable() && (ready_to_write() || ready_to_read()))
    {
        complete_pending_async_buffer_activity();
        for (long index = 0; index < get_buffer_count(); index++)
            initialize_buffer(index);
        if ((writable() || readable()) && position <= total_transferred)
        {
            file_position = position;
            requested_position = position;
            pending_buffer = 0;
            file_initialized = false;
            result = true;
        }
        if (result && current_buffer == 0)
            prime_buffers_for_reading();
    }
    return result;
}

__forceinline long async_write_position(void *handle, void *buffer, long size, unsigned long offset, long category, long priority,
    bool unbuffered, c_synchronized_long *bytes_written, c_synchronized_long *done)
{
    c_write_position_flags flags;
    return async_write_position_ex(handle, buffer, size, offset, flags, category, priority, unbuffered, bytes_written, done);
}

void c_async_double_buffer::async_write_buffer(long index)
{
    s_async_buffer *buffer = &buffers[index];
    buffer->transferred = buffer->position;
    if (async_write_position(handle.handle, buffer->data, buffer->transferred, file_position, 0, 5, unbuffered, &buffer->pending, &buffer->complete) == -1)
    {
        close_file();
    }
    else
    {
        file_position += buffer->transferred;
        total_transferred = total_transferred > file_position ? total_transferred : file_position;
    }
}

void c_async_double_buffer::async_read_buffer(long index)
{
    s_async_buffer *buffer = &buffers[index];
    buffer->transferred = buffer->capacity > total_transferred - file_position ? total_transferred - file_position : buffer->capacity;
    if (async_read_position(handle.handle, buffer->data, buffer->transferred, file_position, 0, 5, &buffer->pending, &buffer->complete) == -1)
    {
        close_file();
    }
    else
    {
        file_position += buffer->transferred;
    }
}

void c_async_double_buffer::swap_buffers()
{
    if (current_buffer == 1)
    {
        async_write_buffer(pending_buffer);
        pending_buffer = (pending_buffer + 1) % get_buffer_count();
        s_async_buffer *buffer = &buffers[pending_buffer];
        async_block_until_done(&buffer->complete, simple_yield_function, 0, 0, 0, 0);
        if (buffer->transferred != buffer->pending.peek())
        {
            end_of_file = true;
            return;
        }
        initialize_buffer(pending_buffer);
    }
    else
    {
        initialize_buffer(pending_buffer);
        async_read_buffer(pending_buffer);
        pending_buffer = (pending_buffer + 1) % get_buffer_count();
        async_block_until_done(&buffers[pending_buffer].complete, simple_yield_function, 0, 0, 0, 0);
    }
}

extern "C" {
}


// Continue tokenizing the directory path.
static __forceinline char *async_helpers_next_directory_token(char *data, const char *delimiters, bool skip_delimiters, csstrtok_data *state)
{
    char *current;
    if (data == 0)
        current = state->next;
    else
        current = data;
    if (current != 0)
    {
        current += (unsigned int)strspn(current, delimiters);
        if (*current == 0)
            current = 0;
    }
    char *token = current;
    if (current != 0)
    {
        current = strpbrk(current, delimiters);
        if (current != 0)
        {
            *current = 0;
            current++;
        }
    }
    state->next = current;
    return token;
}

static __forceinline bool async_helpers_file_valid(s_file_handle volatile *file)
{
    return file->handle != 0 && file->handle != (void *)-1;
}

e_async_completion async_create_file_callback(s_async_task *task)
{
    s_create_file_task *create_task = (s_create_file_task *)task;
    create_task->file->handle = CreateFileA(
        create_task->file_name, create_task->access, create_task->share_mode, 0, create_task->disposition, create_task->flags, 0);
    if ((create_task->file->handle == 0 || create_task->file->handle == (void *)-1) && create_task->create_directories)
    {
        c_static_string<256> directory;
        char path[256];
        csstrtok_data data;
        csstrnzcpy(path, create_task->file_name, 256);
        char *token = csstrtok(path, "\\", true, &data);
        directory.set("");
        while (token)
        {
            directory.append(token);
            token = async_helpers_next_directory_token(0, "\\", true, &data);
            if (token)
                CreateDirectoryA(directory.get_string(), 0);
            directory.append("\\");
        }
        create_task->file->handle = CreateFileA(
            create_task->file_name, create_task->access, create_task->share_mode, 0, create_task->disposition, create_task->flags, 0);
    }
    async_helpers_report_io_error(async_helpers_file_valid(create_task->file));
    bool file_valid = async_helpers_file_valid(create_task->file);
    return _async_completion_done;
}

void async_create_file(const char *path, unsigned long access, e_file_open_disposition disposition, unsigned long attributes,
    e_async_category category, e_async_priority priority, s_file_handle *handle, s_create_file_task *task)
{
    bool create_directories = false;
    unsigned long creation_disposition = 0;
    unsigned long desired_access = 0;
    unsigned long share_mode = 0;
    unsigned long flags_and_attributes = 0;
    memset(task, 0, sizeof(*task));
    switch (disposition)
    {
    case 0:
        creation_disposition = 3;
        break;
    case 1:
        creation_disposition = 2;
        break;
    case 2:
        creation_disposition = 1;
        break;
    case 3:
    case 4:
        creation_disposition = 4;
        break;
    case 5:
        creation_disposition = 5;
        break;
    default:
        // match: 0x1ed280: unreachable hint omits dead branch check
        __assume(0);
    }
    if (disposition == 4)
        create_directories = true;
    if (access & 1)
        desired_access |= 0x80000000;
    if (access & 2)
        desired_access |= 0x40000000;
    if (!(access & 2) || (access & 8))
        share_mode = 1;
    if (attributes & 16)
        share_mode |= 2;
    if (attributes & 1)
        flags_and_attributes |= 0x20000000;
    if (attributes & 8)
        flags_and_attributes |= 0x40000000;
    if (attributes & 2)
        flags_and_attributes |= 0x10000000;
    else if (attributes & 4)
        flags_and_attributes |= 0x8000000;
    csstrnzcpy(task->file_name, path, 256);
    task->access = desired_access;
    task->share_mode = share_mode;
    task->disposition = creation_disposition;
    task->flags = flags_and_attributes;
    task->file = handle;
    task->create_directories = create_directories;
}

long async_get_file_size(s_file_handle file_handle, volatile unsigned long *file_size_out, e_async_category category,
    e_async_priority priority, c_synchronized_long *done)
{
    s_async_task request;
    *(unsigned long *)file_size_out = 0;
    memset(&request, 0, sizeof(request));
    request.get_file_size.handle = file_handle.handle;
    request.get_file_size.file_size_out = (unsigned long *)file_size_out;
    return async_task_add_ex((e_async_priority)priority, (s_async_task *)&request, (e_async_category)category, get_file_size_callback,
        (c_synchronized_long *)done, 1);
}

// @address 0x1ee078 ?open_file@c_async_double_buffer@@QAA_NPBDW4e_async_buffer_file_access@@W4e_async_buffer_disposition@@@Z
bool c_async_double_buffer::open_file(const char *file_path, e_async_buffer_file_access file_access, e_async_buffer_disposition disposition)
{
    bool result = false;
    if (async_helpers_async_usable())
    {
        c_synchronized_long done(0);
        e_file_open_disposition open_disposition = _file_open_disposition_none;
        unsigned long access = (unsigned long)-1;
        switch (disposition)
        {
        case _async_buffer_disposition_create:
            open_disposition = _file_open_disposition_create_always;
            break;
        case _async_buffer_disposition_open:
            open_disposition = _file_open_disposition_open_existing;
            break;
        default:
            // match: 0x1ee078: unreachable hint omits dead branch check
            __assume(0);
        }
        switch (file_access)
        {
        case _async_buffer_file_access_read_write:
            access = 2;
            break;
        case _async_buffer_file_access_read:
            access = 1;
            break;
        case _async_buffer_file_access_write:
            access = 3;
            break;
        default:
            // match: 0x1ee078: unreachable hint omits dead branch check
            __assume(0);
        }
        unbuffered = (access & 2) && file_is_cache_path(file_path);
        s_file_handle *file_handle = &handle;
        s_create_file_task create_task;
        async_create_file(file_path, access, open_disposition, 0, _async_category_none, _async_priority_blocking_below_sound, file_handle, &create_task);
        if (async_task_add_ex(_async_priority_blocking_below_sound, (s_async_task *)&create_task, _async_category_none, async_create_file_callback,
            (c_synchronized_long *)&done, true) == -1)
        {
            file_handle->handle = (void *)-1;
        }
        else
        {
            async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
            if (async_helpers_handle_valid(file_handle->handle))
            {
                bool size_valid = false;
                state = file_access;
                if (disposition == _async_buffer_disposition_open)
                {
                    volatile unsigned long file_size = 0;
                    c_synchronized_long size_done(0);
                    long task_id = -1;
                    task_id = async_get_file_size(*file_handle, &file_size, _async_category_none, _async_priority_blocking_below_sound, &size_done);
                    if (task_id != -1)
                    {
                        async_block_until_done(&size_done, simple_yield_function, 0, 0, 0, 0);
                        total_transferred = file_size;
                        size_valid = true;
                    }
                }
                else
                {
                    size_valid = true;
                }
                if (file_access == _async_buffer_file_access_read)
                {
                    current_buffer = 0;
                    prime_buffers_for_reading();
                    result = true;
                }
                else if (file_access == _async_buffer_file_access_read_write)
                {
                    current_buffer = 1;
                    result = true;
                }
                else if (file_access == _async_buffer_file_access_write)
                {
                    result = true;
                }
                else
                {
                    // match: 0x1ee078: unreachable hint omits dead branch check
                    __assume(0);
                }
                result = result && size_valid;
            }
        }
    }
    return result;
}

long async_set_file_size(s_file_handle file_handle, long size, e_async_category category, e_async_priority priority,
    c_synchronized_long *success_out, c_synchronized_long *done)
{
    if (success_out)
        success_out->set(0);
    s_async_task request;
    memset(&request, 0, sizeof(request));
    request.set_file_size_request.handle = file_handle.handle;
    request.set_file_size_request.size = size;
    request.set_file_size_request.success_out = success_out;
    return async_task_add_ex((e_async_priority)priority, (s_async_task *)&request, _async_category_saved_games, set_file_size_callback,
        (c_synchronized_long *)done, 1);
}

// @address 0x1ed7a8 async_read_entire_file
extern "C" long async_read_entire_file(const char *path, void *buffer, unsigned long buffer_size, volatile unsigned long *file_size_out,
    e_async_category category, e_async_priority priority, c_synchronized_long *completion, c_synchronized_long *done)
{
    if (completion != 0)
        completion->set(0);
    if (file_size_out != 0)
        *file_size_out = 0;
    s_async_task request;
    memset(&request, 0, sizeof(request));
    csstrnzcpy(request.read_entire_file.path, path, 256);
    request.read_entire_file.buffer = buffer;
    request.read_entire_file.buffer_size = buffer_size;
    request.read_entire_file.file_size_out = file_size_out;
    request.read_entire_file.success_out = completion;
    request.read_entire_file.handle = (void *)-1;
    request.read_entire_file.file_size = -1;
    return async_task_add_ex(_async_priority_blocking_generic, (s_async_task *)&request, (e_async_category)category,
        (e_async_completion (*)(s_async_task *))read_entire_file_callback, (c_synchronized_long *)done, 1);
}

// match: 0x1ed990: __restrict alias info settles the copy schedule and register coloring
bool async_compare_tasks(s_compare_tasks_data *__restrict compare_data, e_async_completion (*callback_a)(s_async_task *),
    e_async_completion (*callback_b)(s_async_task *), e_async_priority priority_a, e_async_priority priority_b, const s_async_task *task_a,
    const s_async_task *task_b)
{
    bool result = false;
    if (priority_a > priority_b)
    {
        result = true;
    }
    else if (priority_a < priority_b)
    {
        result = false;
    }
    else if (callback_a == async_read_position_callback && callback_b != async_read_position_callback)
    {
        result = true;
    }
    else if (callback_a != async_read_position_callback && callback_b == async_read_position_callback)
    {
        result = false;
    }
    else if (callback_a == async_read_position_callback && callback_b == async_read_position_callback)
    {
        if (!compare_data->valid)
        {
            result = false;
        }
        else
        {
            const s_async_task *a = (const s_async_task *)task_a;
            const s_async_task *b = (const s_async_task *)task_b;
            unsigned long a_x = address_from_pointer(a->read_position.handle);
            unsigned long b_x = address_from_pointer(b->read_position.handle);
            unsigned long a_y = a->read_position.position;
            unsigned long b_y = b->read_position.position;
            a_x -= compare_data->x;
            b_x -= compare_data->x;
            a_y -= compare_data->y;
            b_y -= compare_data->y;
            if (a_x < b_x)
                result = true;
            else if (a_x > b_x)
                result = false;
            else
                result = a_y < b_y;
        }
    }
    if (callback_b == async_read_position_callback)
    {
        const s_async_task *b = (const s_async_task *)task_b;
        compare_data->x = address_from_pointer(b->read_position.handle);
        compare_data->y = b->read_position.position;
        compare_data->valid = true;
    }
    return result;
}

void c_async_double_buffer::release_storage(c_allocation_base *allocator)
{
    if (storage_initialized)
    {
        close_file();
        for (long index = 0; index < get_buffer_count(); index++)
        {
            s_async_buffer *buffer = &buffers[index];
            if (!buffer->initialized)
                allocator->deallocate(buffer->data);
            buffer->capacity = 0;
            buffer->initialized = false;
        }
        storage_initialized = false;
    }
}

bool c_async_double_buffer::flush()
{
    bool result = false;
    if (async_helpers_async_usable() && ready_to_write())
    {
        s_async_buffer *buffer = &buffers[pending_buffer];
        if (buffer->position > 0)
        {
            async_write_buffer(pending_buffer);
            pending_buffer = (pending_buffer + 1) % get_buffer_count();
            error = true;
        }
        result = true;
    }
    return result;
}

e_async_completion delete_file_callback(s_async_task *task)
{
    s_delete_file_task *delete_task = &task->delete_file;
    s_file_reference reference;
    file_reference_create_from_path(&reference, delete_task->path, delete_task->is_directory);
    bool success = file_delete(&reference);
    if (delete_task->success_out)
        *delete_task->success_out = success;
    return _async_completion_done;
}

// @address 0x1eead0 async_double_buffer_flush
extern "C" void async_double_buffer_flush(c_async_double_buffer *buffer)
{
    if (!async_helpers_async_usable())
        return;
    if (!buffer->ready_to_read() && !buffer->ready_to_write())
        return;
    buffer->complete_pending_async_buffer_activity();
    buffer->flush();
    buffer->complete_pending_async_buffer_activity();
}

// @address 0x1ed188 async_allocate_file_space
extern "C" int async_allocate_file_space(const char *path, void *handle, unsigned long size, __int64 *free_space_used)
{
    FILE_ALLOCATION_INFO allocation;
    ULARGE_INTEGER free_before;
    ULARGE_INTEGER free_after;
    int result;
    GetDiskFreeSpaceExA(path, &free_before, 0, 0);
    result = 0;
    if (async_helpers_handle_valid(handle))
    {
        allocation.AllocationSize.QuadPart = size;
        if (SetFileInformationByHandle(handle, FileAllocationInfo, &allocation, sizeof(allocation)) == 0)
        {
            if (g_utility_drive_mounted != 0)
                XFlushUtilityDrive();
            if (SetFileInformationByHandle(handle, FileAllocationInfo, &allocation, sizeof(allocation)) != 0)
                result = 1;
        }
        else
            result = 1;
    }
    if ((unsigned char)result == 0)
        g_async_file_io_error = GetLastError();
    GetDiskFreeSpaceExA(path, &free_after, 0, 0);
    *free_space_used = free_before.QuadPart - free_after.QuadPart;
    return result;
}


// @address 0x59f7a0 SetFileInformationByHandle
// @address 0x949acc ?g_thread_crashed@@3PAVc_interlocked_long@@A
