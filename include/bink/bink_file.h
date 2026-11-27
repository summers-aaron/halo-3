#ifndef BINK_BINK_FILE_H
#define BINK_BINK_FILE_H

#include <stddef.h>
#include "bink/rad_timer.h"

struct s_bink_file;
struct bink_read_state;
typedef void (*t_bink_callback)(s_bink_file *);

// Xenon file-reader state.
struct s_bink_file
{
    unsigned int (*read_header)(s_bink_file *, int, void *, unsigned int);
    unsigned int (*read_frame)(s_bink_file *, unsigned int, int, void *, unsigned int);
    unsigned int (*get_buffer_size)(s_bink_file *, unsigned int);
    void (*set_info)(s_bink_file *, unsigned int, unsigned int, unsigned int, unsigned int);
    t_bink_callback close;
    int (*idle)(s_bink_file *);
    unsigned int (*background_control)(s_bink_file *, unsigned int);
    bink_read_state *bink;
    volatile unsigned int read_error;
    volatile unsigned int doing_a_read;
    volatile unsigned int bytes_read;
    volatile unsigned int working;
    volatile unsigned int total_time;
    volatile unsigned int foreground_time;
    volatile unsigned int idle_time;
    volatile unsigned int thread_time;
    volatile unsigned int buffer_size;
    volatile unsigned int buffer_high_used;
    volatile unsigned int buffer_available;
    volatile unsigned int buffer_used;
    unsigned int bg_state;
    void *handle;
    volatile unsigned int file_position;
    volatile unsigned int logical_position;
    volatile unsigned int read_buffer;
    volatile unsigned int read_size;
    int : 32;
    volatile unsigned int buffer_begin;
    volatile unsigned int buffer_end;
    volatile unsigned int buffer_position;
    unsigned int shared;
    unsigned int base_offset;
    volatile unsigned int simulate;
    volatile unsigned int simulated_rate;
    volatile int simulated_delay;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    int : 32;
    t_bink_callback try_suspend_callback;
    int (*try_suspend_io_callback)(s_bink_file *);
    t_bink_callback resume_callback;
    t_bink_callback idle_callback;
    int callback;
    unsigned int callback_control[15];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char bink_file_size[(sizeof(void *) != 4 || (sizeof(s_bink_file) == 324)) ? 1 : -1];
typedef char bink_file_buffer_size_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_file, buffer_size) == 64))) ? 1 : -1];
typedef char bink_file_read_error_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_file, read_error) == 32))) ? 1 : -1];
typedef char bink_file_buffer_high_used_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_file, buffer_high_used) == 68))) ? 1 : -1];
typedef char bink_file_buffer_available_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_file, buffer_available) == 72))) ? 1 : -1];
typedef char bink_file_buffer_used_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_file, buffer_used) == 76))) ? 1 : -1];
typedef char bink_file_handle_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, handle) == 84)) ? 1 : -1];
typedef char bink_file_position_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, file_position) == 88)) ? 1 : -1];
typedef char bink_file_logical_position_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, logical_position) == 92)) ? 1 : -1];
typedef char bink_file_read_buffer_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, read_buffer) == 96)) ? 1 : -1];
typedef char bink_file_buffer_begin_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, buffer_begin) == 108)) ? 1 : -1];
typedef char bink_file_base_offset_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, base_offset) == 124)) ? 1 : -1];
typedef char bink_file_try_suspend_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, try_suspend_callback) == 244)) ? 1 : -1];
typedef char bink_file_try_suspend_io_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, try_suspend_io_callback) == 248)) ? 1 : -1];
typedef char bink_file_resume_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, resume_callback) == 252)) ? 1 : -1];
typedef char bink_file_idle_callback_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, idle_callback) == 256)) ? 1 : -1];
typedef char bink_file_callback_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, callback) == 260)) ? 1 : -1];
typedef char bink_file_callback_control_offset[(sizeof(void *) != 4 || (offsetof(s_bink_file, callback_control) == 264)) ? 1 : -1];
#endif

extern "C" {
void BinkFileSetInfo(s_bink_file *file, unsigned int buffer, unsigned int size, unsigned int simulate, unsigned int rate);
int BinkFileIdle(s_bink_file *file);
void BinkFileClose(s_bink_file *file);
unsigned int BinkFileBGControl(s_bink_file *file, unsigned int control);
}

#endif // BINK_BINK_FILE_H
