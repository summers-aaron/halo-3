#ifndef MULTITHREADING_THREADS_H
#define MULTITHREADING_THREADS_H

#include "multithreading/thread_system_globals.h"

// Functions defined in tu/blamlibxbox/threads.cpp.

void initialize_thread_management();
void start_thread(long thread_index);
bool wait_for_thread_to_exit(long thread_index, unsigned long timeout_in_milliseconds);

struct _EXCEPTION_POINTERS;

extern "C" void thread_record_first_exception(_EXCEPTION_POINTERS *exception_pointers);

struct s_field_layout;

extern "C" void threads_unlock_restricted_region(s_field_layout *region_layout);

// Data defined in tu/blamlibxbox/threads.cpp that other units use.

class c_interlocked_long;
struct s_thread_system_globals;

extern s_thread_system_globals g_thread_globals;
extern c_interlocked_long g_thread_system_initialized;
extern c_interlocked_long g_thread_should_exit[9];
void havok_notify_thread_running(void *field);
void *havok_thread_user_data_get(long thread_index);
void havok_next_thread_paramerters_get(long thread_index, long *out_thread_index, unsigned long *out_stack_size);
void havok_notify_thread_started(void *thread_handle, long thread_index, unsigned long thread_id, void *argument);
void havok_notify_thread_stopped(void *thread_handle);


extern c_interlocked_long g_thread_crashed[9];

__forceinline const char *get_thread_name_from_thread_id(unsigned long thread_id);

#endif // MULTITHREADING_THREADS_H
