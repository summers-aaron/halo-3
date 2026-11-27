#ifndef CSERIES_ASYNC_DOUBLE_BUFFER_H
#define CSERIES_ASYNC_DOUBLE_BUFFER_H
#include "multithreading/synchronized_value.h"
#include "multithreading/threads.h"
#include "cseries/async_buffer_file_access_types.h"
#include "cseries/file_handle_types.h"
#include "cseries/async_buffer_disposition_types.h"
#include "shell/shell.h"
#include "saved_games/saved_film_runtime.h"
class c_saved_film;
struct game_options;
typedef game_options s_game_options;
class c_interlocked_long;
extern "C" bool saved_film_load_game_state(long index);
class c_allocation_base;
namespace {
struct s_async_buffer
{
    s_async_buffer() {}
    void *data;
    long capacity;
    bool initialized;
    long used;
    long position;
    c_synchronized_long pending;
    long transferred;
    c_synchronized_long complete;
};
}




class c_async_double_buffer;
extern "C" void async_double_buffer_flush(c_async_double_buffer *buffer);

class c_async_double_buffer
{
public:
    __forceinline c_async_double_buffer(long count) : buffer_count(count)
    {
        initialize_internal();
        storage_initialized = false;
    }
    bool open_file(const char *file_path, e_async_buffer_file_access file_access, e_async_buffer_disposition disposition);
    long get_position();
    bool ready_to_write();
    bool allocate_storage(c_allocation_base *allocator, long buffer_size);
    bool ready_to_read();
    bool close_file();
    bool set_position(long position);
    bool flush();
    void release_storage(c_allocation_base *allocator);
    void read(void *buffer, long size, long *bytes_read);
    void write(const void *data, long size, long *bytes_written);
private:
    friend void async_double_buffer_flush(c_async_double_buffer *buffer);
    friend class c_saved_film;
    friend bool saved_film_new_recording(c_saved_film *, const char *, const s_game_options *, e_controller_index);
    friend bool saved_film_load_game_state(long index);
    void initialize_internal();
    void complete_pending_async_buffer_activity();
    void prime_buffers_for_reading();
    void async_write_buffer(long index);
    void async_read_buffer(long index);
    void swap_buffers();
    bool writable() const;
    bool readable() const;
    bool handle_read_state();
    long fill_bytes(void *destination, long size);
    bool handle_write_state();
    long empty_bytes(const void *source, long size);
    __forceinline bool handle_write_state(const c_interlocked_long *crashed);
    __forceinline long get_buffer_count() const { return buffer_count; }
    __forceinline void initialize_buffer(long index)
    {
        s_async_buffer *buffer = &buffers[index];
        buffer->pending.set(0);
        buffer->transferred = 0;
        buffer->position = 0;
        buffer->used = 0;
        buffer->complete.set(1);
    }
public:
    long buffer_count;
    s_async_buffer buffers[3];
    bool unbuffered;
    long state;
    long current_buffer;
    s_file_handle handle;
    long file_position;
    long requested_position;
    long total_transferred;
    long pending_buffer;
    bool storage_initialized;
    bool file_initialized;
    bool end_of_file;
    bool error;
};

__forceinline bool c_async_double_buffer::handle_write_state()
{
    bool result = false;
    bool usable = g_thread_crashed[3].peek() != 1;
    if (usable)
    {
        unsigned long current_state = state;
        if (current_state >= 1)
        {
            if (current_state != 1)
            {
                if (current_buffer != 1)
                {
                complete_pending_async_buffer_activity();
                pending_buffer = 0;
                current_buffer = 1;
                file_position = requested_position;
                }
                result = true;
            }
        }
        else
        {
            result = true;
        }
    }
    return result;
}

__forceinline bool c_async_double_buffer::handle_write_state(const c_interlocked_long *crashed)
{
    bool result = false;
    bool usable;
    switch (crashed[3].peek())
    {
    case 1: usable = false; break;
    default: usable = true; break;
    }
    if (usable)
    {
        unsigned long current_state = state;
        if (current_state >= 1)
        {
            if (current_state != 1)
            {
                if (current_buffer != 1)
                {
                    complete_pending_async_buffer_activity();
                    pending_buffer = 0;
                    current_buffer = 1;
                    file_position = requested_position;
                }
                result = true;
            }
        }
        else
        {
            result = true;
        }
    }
    return result;
}

#endif
