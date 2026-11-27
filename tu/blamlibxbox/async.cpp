// blamlibxbox/async.obj: from the records 0x815a8 0x84f90 0x8db10 0x19e920 0x19ea00 0x19eac8 0x19eb80 0x19ed68 0x19ed90 0x19eee0 0x19efc0 0x1f45a0 0x4785f8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x19eac8 async_task_remove
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "multithreading/thread_tls.h"
#include <string.h>
#include "cseries/crt_xdk.h"
// @address 0x589e70 SetThreadPriority
// @address 0x59e430 SwitchToThread
#include "multithreading/synchronization.h"
#include "cseries/async.h"
#include "cseries/async_helpers.h"
#include "cseries/async_queue.h"
#include "cseries/async_xoverlapped.h"
#include "multithreading/threads.h"
// match: 0x325f28: this unit has to declare main_exception_handle_crash before sound_idle (found by image check)
#include "main/main.h"
#include "sound/sound_manager.h"

#include "multithreading/synchronized_value.h"
#include "cseries/compare_tasks_data_types.h"


#include "cseries/async_globals.h"
#include "multithreading/synchronization_types.h"
#include "cseries/async_xoverlapped_types.h"
#include "multithreading/thread_system_globals.h"
#include "memory/tls_globals.h"

#include "interface/user_interface_content_globals.h"


#include "cache/cache_files_pending_load_types.h"
// @address 0x1ed990 ?async_compare_tasks@@YA_NPIAUs_compare_tasks_data@@P6A?AW4e_async_completion@@PAUs_async_task@@@Z2W4e_async_priority@@3PBU3@4@Z
// match: 0x19e920: pointer __restrict qualifier prevents alias reload
#include "cseries/async_compare.h"
// @address 0x9dc200 g_cache_files_windows_pending_load
#include "cache/cache_files_pending_load.h"
#include "cache/cache_files_windows.h"


static __forceinline bool async_test_completion_flag(c_synchronized_long *completion_flag)
{
    return completion_flag->peek() != 0;
}



static __forceinline void internal_mutex_release(long mutex_id)
{
    _InterlockedDecrement(&g_synch_globals.mutexes[mutex_id].lock_count.value);
    ReleaseMutex(g_synch_globals.mutexes[mutex_id].handle);
}
static __forceinline long async_tasks_in_queue()
{
    long count = 0;
    internal_mutex_take(1);
    for (s_async_queue_element *task = g_async_globals.work_list; task; task = task->next)
        count++;
    internal_mutex_release(1);
    return count;
}
void async_idle()
{
    g_async_globals.tasks_added = async_tasks_in_queue();
}

bool simple_yield_function(c_synchronized_long *completion_flag)
{
    return async_test_completion_flag(completion_flag);
}



// @address 0x8db10 async_test_content_catalogue_busy
extern "C" bool async_test_content_catalogue_busy(void)
{
    bool result = false;
    bool busy = false;
    busy = busy || g_user_interface_content_globals.tasks[0].state != 0;
    busy = busy || g_user_interface_content_globals.tasks[1].state != 0;
    busy = busy || g_user_interface_content_globals.tasks[2].state != 0;
    if (busy)
    {
        if (!g_user_interface_content_globals.was_busy)
        {
            g_user_interface_content_globals.started = GetTickCount();
        }
        unsigned long now = GetTickCount();
        g_user_interface_content_globals.was_busy = busy;
        if (now > g_user_interface_content_globals.started + 500)
        {
            result = true;
        }
    }
    else
    {
        g_user_interface_content_globals.was_busy = busy;
    }
    return result;
}

static __forceinline void async_work_list_remove(s_async_queue_element *task)
{
    if (g_async_globals.work_list == task)
    {
        g_async_globals.work_list = task->next;
    }
    else
    {
        s_async_queue_element *previous = g_async_globals.work_list;
        s_async_queue_element **next = &previous->next;
        while (previous->next != task)
        {
            previous = *next;
            next = &previous->next;
        }
        previous->next = task->next;
    }
}

// @address 0x19e920 ?work_list_add_internal_assumes_locked_does_not_set_id_does_not_resume@@YAXPAUs_async_queue_element@@@Z
void work_list_add_internal_assumes_locked_does_not_set_id_does_not_resume(s_async_queue_element *async_queue_element)
{
    s_compare_tasks_data compare_data = {};
    if (!g_async_globals.work_list)
    {
        g_async_globals.work_list = async_queue_element;
        async_queue_element->next = 0;
        return;
    }
    s_async_queue_element *head = g_async_globals.work_list;
    e_async_priority priority = async_queue_element->priority;
    if (priority > head->priority)
    {
        async_queue_element->next = head;
        g_async_globals.work_list = async_queue_element;
        return;
    }
    s_async_queue_element *previous = head;
    while (previous->next)
    {
        if (async_compare_tasks(&compare_data, async_queue_element->callback, previous->next->callback, priority, previous->next->priority,
                &async_queue_element->task, &previous->next->task))
        {
            async_queue_element->next = previous->next;
            previous->next = async_queue_element;
            return;
        }
        previous = previous->next;
    }
    previous->next = async_queue_element;
    async_queue_element->next = 0;
}

extern "C" void async_task_remove(s_async_queue_element *task)
{
    internal_mutex_take(1);
    async_work_list_remove(task);
    task->task_id = -1;
    internal_mutex_release(1);
}

// @address 0x7db620 g_async_task_buffer
s_async_task_buffer g_async_task_buffer;

static __forceinline void async_task_buffer_initialize()
{
    g_async_task_buffer.offset = 0;
    g_async_task_buffer.address = &g_async_task_buffer;
    g_async_task_buffer.size = 32768;
}

// @address 0x19eb80 ?async_initialize@@YAXXZ
void async_initialize(void)
{
    s_async_globals *pool = &g_async_globals;
    memset(pool->elements, 0, sizeof(pool->elements));
    for (long i = 0; i < 25; i++)
    {
        pool->elements[i].next = &pool->elements[i + 1];
        pool->elements[i].task_id = -1;
    }
    pool->elements[24].next = 0;
    pool->free_list = pool->elements;
    pool->work_list = 0;
    async_task_buffer_initialize();
    ::start_thread(3);
    SetThreadPriority(g_thread_globals.handles[3], 1);
}


// @address 0x19ea00 ?async_task_add_ex@@YAJPAUs_async_queue_element@@@Z
long async_task_add_ex(s_async_queue_element *async_queue_element)
{
    s_async_globals *pool = &g_async_globals;
    internal_mutex_take(1);
    work_list_add_internal_assumes_locked_does_not_set_id_does_not_resume(async_queue_element);
    long task_index = async_queue_element - g_async_globals.elements;
    async_queue_element->task_id = task_index;
    async_queue_element->task_id = (pool->task_counter << 8) | task_index;
    pool->task_counter = pool->task_counter + 1;
    long task_id = async_queue_element->task_id;
    internal_mutex_release(1);
    internal_semaphore_release(0);
    return task_id;
}



// @address 0x19ed68 ?async_main@@YAKPAX@Z
unsigned long async_main(void *)
{
    async_work_function();
    return 0;
}

// @address 0x19ed90 ?async_work_function@@YAXXZ
void async_work_function(void)
{
    if (g_thread_should_exit[get_current_thread_index()] != 1) {
        c_synchronized_long *tr11;
        int tr3;
        s_synchronization_globals *synchronization = &g_synch_globals;
        do {
            internal_semaphore_take(0);
            internal_mutex_take(1);
            s_async_queue_element *tr30;
            tr30 = g_async_globals.work_list;
            _InterlockedDecrement(&synchronization->mutexes[1].lock_count.value);
            ReleaseMutex(synchronization->mutexes[1].handle);
            g_async_globals.current_task = tr30;
            if (tr30 != 0) {
                tr3 = tr30->callback(&tr30->task);
                if (tr3 != 0) {
                    if (tr3 == 1) {
                        tr11 = g_async_globals.current_task->done;
                        if (tr11 != 0) {
                            __lwsync();
                            _InterlockedExchange((long *)tr11, 1);
                        }
                    }
                    async_task_remove((s_async_queue_element *)g_async_globals.current_task);
                    tr30 = g_async_globals.current_task;
                    internal_mutex_take(0);
                    tr30->next = g_async_globals.free_list;
                    g_async_globals.free_list = tr30;
                    _InterlockedDecrement(&synchronization->mutexes[0].lock_count.value);
                    ReleaseMutex(synchronization->mutexes[0].handle);
                } else {
                    internal_semaphore_release(0);
                    Sleep(0);
                }
            }
        } while (g_thread_should_exit[get_current_thread_index()] != 1);
    }
}

// @address 0x19eee0 ?async_task_change_priority@@YA_NJW4e_async_priority@@@Z
bool async_task_change_priority(long task_id, e_async_priority priority)
{
    s_async_globals *pool = &g_async_globals;
    bool result = false;
    internal_mutex_take(1);
    long task_index = (unsigned char)task_id;
    if (task_index >= 0 && (unsigned long)task_index < 25) {
        s_async_queue_element *async_queue_element = &pool->elements[task_index];
        if (async_queue_element->task_id == task_id) {
            async_work_list_remove(async_queue_element);
            async_queue_element->priority = priority;
            work_list_add_internal_assumes_locked_does_not_set_id_does_not_resume(async_queue_element);
            result = true;
        }
    }
    internal_mutex_release(1);
    return result;
}


// @address 0x1f45a0 ?dispose@c_cache_files_pending_load_listener@@UAAXPAX@Z
void c_cache_files_pending_load_listener::dispose(void *)
{
    if (g_cache_files_windows_pending_load.copy_buffer != 0)
    {
        async_task_change_priority(g_cache_files_windows_pending_load.task_id, _async_priority_blocking_below_sound);
        do
        {
            __lwsync();
            _InterlockedExchange(&g_cache_files_windows_pending_load.armed.value, 1);
            g_cache_files_windows_pending_load.release_copy_buffer = 1;
            cache_files_windows_idle();
        } while (g_cache_files_windows_pending_load.copy_buffer != 0);
    }
}

// @address 0x4785f8 async_block_until_content_finished
extern "C" void async_block_until_content_finished(void)
{
    for (int i = 0; i < 3; i++)
    {
        overlapped_task_block_until_finished((c_overlapped_task *)&g_user_interface_content_globals.tasks[i].task);
    }
}


// @address 0x19efc0 async_block_until_done
extern "C" bool async_block_until_done(
    c_synchronized_long *done, bool (*yield_function)(c_synchronized_long *), bool idle, bool with_idle, long file, long line)
{
    bool finished = yield_function(done);
    if (!finished)
    {
        while (!(finished = yield_function(done)))
        {
            SwitchToThread();
            if (g_thread_crashed[3].peek() == 1)
                main_exception_handle_crash();
            if (idle)
                sound_idle();
            main_update_idle_tasks();
        }
    }
    return finished;
}

// @address 0x7cff10 g_async_globals
#if !defined(_XBOX) || defined(__clang__)
s_async_globals g_async_globals;
#endif


// @address 0x81ac8 cache_files_windows_idle
// @address 0x949acc ?g_thread_crashed@@3PAVc_interlocked_long@@A
