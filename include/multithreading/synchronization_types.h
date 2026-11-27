#ifndef MULTITHREADING_SYNCHRONIZATION_TYPES_H
#define MULTITHREADING_SYNCHRONIZATION_TYPES_H

#ifndef CSERIES_CRT_XDK_H
#include <xtl.h>
#endif
#include <stddef.h>
#include "multithreading/synchronized_value.h"

class c_synchronization_object
{
public:
    c_interlocked_long locking_thread;
    c_interlocked_long lock_count;

    __forceinline c_synchronization_object() { reset(); }
    __forceinline void reset()
    {
        _InterlockedExchange(&locking_thread.value, -1);
        _InterlockedExchange(&lock_count.value, 0);
    }
    __forceinline long get_locking_thread()
    {
        long owner;
        if (lock_count > 0)
            owner = locking_thread.value;
        else
            owner = -1;
        return owner;
    }
    void decrement_lock_count();
    void set_lock_count(long thread_index, long count);

    __forceinline long get_lock_count()
    {
        return lock_count;
    }
    __forceinline void increment_lock_count(long thread_index)
    {
        _InterlockedExchange(&locking_thread.value, thread_index);
        _InterlockedIncrement(&lock_count.value);
    }
};
struct s_critical_section_handle : c_synchronization_object
{
    RTL_CRITICAL_SECTION critical_section;
};
struct s_mutex_handle : c_synchronization_object
{
    void *handle;
    __forceinline s_mutex_handle() : handle(0) {}
};
class c_semaphore_handle : public c_synchronization_object
{
public:
    void *handle;
    long thread_references[9];
    __forceinline c_semaphore_handle() : handle(0)
    {
        for (long thread_index = 0; thread_index < 9; ++thread_index)
            thread_references[thread_index] = 0;
    }

    long get_thread_reference_count(long thread_index);

    __forceinline void add_thread_reference(long thread_index)
    {
        thread_references[thread_index]++;
    }
    __forceinline void remove_thread_reference(long thread_index)
    {
        thread_references[thread_index] = thread_references[thread_index] - 1 > 0
            ? thread_references[thread_index] - 1 : 0;
    }
};

// Complete synchronization state; the handle methods
// identify the interlocked base, and XDK supplies the native critical section.
struct s_synchronization_globals
{
    volatile bool initialized;
    s_critical_section_handle critical_sections[58];
    s_mutex_handle mutexes[3];
    s_mutex_handle events[7];
    c_semaphore_handle semaphores[5];
    s_synchronization_globals();
};

extern s_synchronization_globals g_synch_globals;

typedef char synchronization_object_size[(sizeof(c_synchronization_object) == 8) ? 1 : -1];
typedef char critical_section_handle_size[(sizeof(void *) != 4 || ((sizeof(s_critical_section_handle) == 36) )) ? 1 : -1];
typedef char mutex_handle_size[(sizeof(void *) != 4 || ((sizeof(s_mutex_handle) == 12) )) ? 1 : -1];
typedef char semaphore_handle_size[(sizeof(void *) != 4 || ((sizeof(c_semaphore_handle) == 48) )) ? 1 : -1];
typedef char synchronization_globals_size[(sizeof(void *) != 4 || ((sizeof(s_synchronization_globals) == 0x994) )) ? 1 : -1];
typedef char synchronization_globals_sections_offset[(sizeof(void *) != 4 || ((offsetof(s_synchronization_globals, critical_sections) == 4) )) ? 1 : -1];
typedef char synchronization_globals_lock_count_55_offset[(sizeof(void *) != 4
    || ((offsetof(s_synchronization_globals, critical_sections)
    + 55 * sizeof(s_critical_section_handle)
    + offsetof(c_synchronization_object, lock_count) == 1988) )) ? 1 : -1];
typedef char synchronization_globals_native_55_offset[(sizeof(void *) != 4
    || ((offsetof(s_synchronization_globals, critical_sections)
    + 55 * sizeof(s_critical_section_handle)
    + offsetof(s_critical_section_handle, critical_section) == 1992) )) ? 1 : -1];

#endif
