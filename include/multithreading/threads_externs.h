#ifndef MULTITHREADING_THREADS_EXTERNS_H
#define MULTITHREADING_THREADS_EXTERNS_H
#include "game/player_control_globals.h"

#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "multithreading/synchronized_value.h"
struct s_thread_assert_arguments {
    const char *statement;
    const char *file;
    long line;
    bool fatal;
};

struct s_thread_name_info {
    unsigned long type;
    const char *name;
    unsigned long thread_id;
    unsigned long flags;
};

struct s_thread_exception_record {
    c_synchronized_long state;
    unsigned long thread_id;
    CONTEXT context;
    unsigned long exception_code;
    unsigned long exception_flags;
    void *exception_address;
    unsigned long parameter_count;
    ULONG_PTR parameters[4];
};


extern "C" { extern unsigned long g_first_exception_tick_count; }
extern c_synchronized_long g_first_exception_flag;
extern s_thread_exception_record g_main_crash_data;
extern __declspec(thread) s_thread_assert_arguments g_thread_assert_arguments;
extern __declspec(thread) bool g_thread_assert_triggered;

extern "C" {
void rasterizer_device_prepare_thread_release(D3DDevice *device);
}

static __forceinline void set_current_thread_index(long thread_index)
{
    g_registered_thread_index = thread_index;
}

#endif // MULTITHREADING_THREADS_EXTERNS_H
