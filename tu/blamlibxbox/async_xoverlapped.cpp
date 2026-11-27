// blamlibxbox/async_xoverlapped.obj: from the records 0x88880 0x950f8 0x96b40 0x20e4e8 0x20e550 0x20e580 0x20e618 0x20e6a0 0x20e770 0x20e7e8 0x20e860 0x20e938 0x20e9d0 0x20eab0 0x20ec38
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "memory/runtime_state_allocation.h"
#include "shell/shell.h"

#include <string.h>
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "multithreading/synchronization.h"
#include "cseries/async_xoverlapped.h"
#include "memory/memory_pool.h"
#include "networking/online/online_xenon.h"
#include "networking/online/online_lsp_xenon.h"
#include "cseries/async_xoverlapped_memory.h"
// @address 0x8c4274 g_overlapped_memory_pool
struct s_memory_pool;
#include "interface/virtual_keyboard_task.h"

#include "multithreading/synchronization_runtime.h"
#include "multithreading/synchronization_inline.h"
#include "memory/memory_pool_types.h"
#include "memory/normal_allocation.h"
#include "cseries/xam_user_signin.h"
// @address 0x70d394 RtlEnterCriticalSection
// @address 0x70d444 XamUserGetSigninInfo
// @address 0x74b80c g_runtime_state_allocation
// @address 0x8c4448 g_virtual_keyboard_task_instance
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x80608 GetTickCount
#include "multithreading/synchronization_types.h"
#include "cseries/async_xoverlapped_globals.h"
namespace {
class c_async_xoverlapped_scope_lock {
public:
    __forceinline c_async_xoverlapped_scope_lock() { internal_critical_section_enter(24); }
    __forceinline ~c_async_xoverlapped_scope_lock() { internal_critical_section_leave(24); }
};

// match: 0x20e618: direct keyboard access emits a separate pool lis instead of sharing the r30 base for both slots.
struct s_overlapped_dispose_globals {
    s_memory_pool *memory_pool;
    // Address span contains other globals (including g_camera_globals), not object padding.
    char intervening_globals[0x1d4-4];
    c_overlapped_task *virtual_keyboard_task;
};

// Enter the overlapped-task critical section.
static __forceinline void async_xoverlapped_critical_section_enter(long index)
{
    s_synchronization_globals *globals = (s_synchronization_globals *)(&g_synch_globals);
    RtlEnterCriticalSection(&globals->critical_sections[index].critical_section);
    _InterlockedExchange(&globals->critical_sections[index].locking_thread.value, get_current_thread_index());
    _InterlockedIncrement(&globals->critical_sections[index].lock_count.value);
}
}
// @address 0x7e7cc0 g_overlapped_globals
s_overlapped_globals g_overlapped_globals;

namespace {
static __forceinline void virtual_keyboard_dispose_instance()
{
    s_overlapped_dispose_globals *globals = (s_overlapped_dispose_globals *)&g_overlapped_memory_pool;
    if (globals->virtual_keyboard_task)
    {
        c_overlapped_task *task = globals->virtual_keyboard_task;
        task->~c_overlapped_task();
        overlapped_free(task);
        globals->virtual_keyboard_task = 0;
    }
}
static __forceinline void overlapped_memory_dispose()
{
    if (g_overlapped_memory_pool)
    {
        ::memory_pool_dispose(g_overlapped_memory_pool);
        g_overlapped_memory_pool = 0;
    }
}
}

// @address 0x20e4e8 ?reset@c_overlapped_task@@UAAXXZ
void c_overlapped_task::reset()
{
    c_async_xoverlapped_scope_lock lock;
    set_task_state_internal(_overlapped_task_state_none);
}

// @address 0x20e550 ?is_result_successful@c_overlapped_task@@UAA_NKKK@Z
bool c_overlapped_task::is_result_successful(unsigned long calling_result, unsigned long overlapped_error,
    unsigned long overlapped_extended_error)
{
    bool result_successful = (calling_result == 0 || calling_result == 0x3e5)
        && overlapped_error == 0 && overlapped_extended_error == 0;
    return result_successful;
}

void c_overlapped_task::update(unsigned long)
{
}

void c_overlapped_task::success(unsigned long)
{
}

void c_overlapped_task::failure(unsigned long, unsigned long, unsigned long)
{
}

void c_overlapped_task::complete()
{
}


// @address 0x39ebc8 ?get_instance@c_virtual_keyboard_task@@SAPAV1@PBDJW4e_controller_index@@PB_W22KK@Z
// @address 0x20e580 ?overlapped_initialize@@YAXXZ
void overlapped_initialize()
{
    memset(&g_overlapped_globals, 0, sizeof(g_overlapped_globals));
    s_memory_pool *memory_pool = memory_pool_new(
        "xoverlapped memory pool", 0x9bf80, 0, (unsigned int *)&g_runtime_state_allocation);
    g_overlapped_memory_pool = memory_pool;
    if (memory_pool)
    {
        memory_pool->reference_tracking_disabled = true;
        memory_pool->allocation_from_anywhere = true;
    }
    c_virtual_keyboard_task::get_instance("..\\cseries\\async_xoverlapped.cpp", 178, k_any_controller, 0, 0, 0, 0, 0);
}
// @address 0x20e618 ?overlapped_dispose@@YAXXZ
void overlapped_dispose()
{
    overlapped_task_wait_for_all_tasks_to_finish();
    virtual_keyboard_dispose_instance();
    overlapped_memory_dispose();
}

bool overlapped_task_start_internal(c_overlapped_task *task, const char *file, long line)
{
    bool result = false;
    c_async_xoverlapped_scope_lock lock;
    s_task_slot *slot = 0;
    for (unsigned long index = 0; index < 64; index++) {
        if (!g_overlapped_globals.slots[index].task) {
            slot = &g_overlapped_globals.slots[index];
            break;
        }
    }
    if (slot) {
        slot->task = task;
        slot->task->set_task_state_internal(_overlapped_task_state_starting);
        result = true;
    }
    return result;
}


// @address 0x20e770 ?overlapped_task_block_until_finished@@YAXPAVc_overlapped_task@@@Z
void overlapped_task_block_until_finished(c_overlapped_task *task)
{
    c_async_xoverlapped_scope_lock lock;
    s_task_slot *slot = find_task_slot(task);
    if (slot)
        task_block_until_finished(slot);
}

void overlapped_task_terminate(c_overlapped_task *task)
{
    c_async_xoverlapped_scope_lock lock;
    s_task_slot *slot = find_task_slot(task);
    if (slot)
        slot->terminated = true;
}

// @address 0x20e860 ?overlapped_task_wait_for_all_tasks_to_finish@@YAXXZ
void overlapped_task_wait_for_all_tasks_to_finish()
{
    for (unsigned long slot_index = 0; slot_index < 64; slot_index++) {
        async_xoverlapped_critical_section_enter(24);
        s_task_slot *slot = &g_overlapped_globals.slots[slot_index];
        if (slot->task && slot->task->get_task_state() == 2)
            task_block_until_finished(slot);
        internal_critical_section_leave(24);
    }
}

// @address 0x20e9d0 ?task_is_complete@@YA_NPAUs_task_slot@@PAKPIAK22_N@Z
// match: 0x20e9d0: pointer __restrict qualifier prevents alias reload
bool task_is_complete(s_task_slot *task_slot, unsigned long *return_result, unsigned long *__restrict calling_result,
    // match: 0x20e9d0: pointer __restrict qualifier prevents alias reload
    unsigned long *__restrict overlapped_error, unsigned long *__restrict overlapped_extended_error, bool debug)
{
    bool complete = false;
    *return_result = 0;
    *calling_result = task_slot->calling_result;
    *overlapped_error = 0;
    *overlapped_extended_error = 0;
    if (task_slot->task->get_task_state() != 1) {
        if (task_slot->calling_result != ERROR_IO_PENDING) {
            complete = true;
        } else {
            unsigned long current_time = GetTickCount();
            complete = XHasOverlappedIoCompleted(&task_slot->overlapped);
            g_overlapped_globals.pending_poll = true;
            g_overlapped_globals.pending_poll_time = current_time;
            if (complete) {
                *overlapped_error = XGetOverlappedResult(&task_slot->overlapped, return_result, FALSE);
                unsigned long result = *overlapped_error;
                if (result != ERROR_SUCCESS && result != ERROR_IO_INCOMPLETE && result != ERROR_IO_PENDING)
                    *overlapped_extended_error = XGetOverlappedExtendedError(&task_slot->overlapped);
            }
        }
    }
    return complete;
}

// @address 0x20eab0 task_now_finished
extern "C" void task_now_finished(s_task_slot *slot,unsigned long return_result,unsigned long calling_result,unsigned long error,unsigned long extended_error) {
 bool successful;
 if(slot->task->get_task_state() == 1) successful=false;
 else successful=!slot->terminated && slot->task->is_result_successful(calling_result,error,extended_error);
 slot->task->set_task_state_internal(_overlapped_task_state_completing);
 e_overlapped_task_state final_state;
 if(successful) {
  final_state=_overlapped_task_state_success;
  slot->task->success(return_result);
 } else {
  final_state=_overlapped_task_state_failure;
  if(slot->terminated) error=1223;
  slot->task->failure(calling_result,error,extended_error);
 }
 slot->task->complete();
 if(slot->task) {
  if(slot->task->task_was_recycled_during_completion()) slot->task->task_recycled_during_completion(false);
  else if(slot->task->busy()) slot->task->set_task_state_internal(final_state);
 }
 memset(slot,0,sizeof(*slot));
}

// @address 0x20ec38 find_task_slot
extern "C" s_task_slot *find_task_slot(c_overlapped_task *task) {
 s_task_slot *result=0;
 for(unsigned long slot_index=0;slot_index<64;slot_index++) {
  if(g_overlapped_globals.slots[slot_index].task==task) result=&g_overlapped_globals.slots[slot_index];
 }
 return result;
}

// @address 0x20e938 ?task_block_until_finished@@YAXPAUs_task_slot@@@Z
void task_block_until_finished(s_task_slot *task_slot)
{
    unsigned long return_result, calling_result, overlapped_error, overlapped_extended_error;
    while (task_slot->task && !task_is_complete(task_slot, &return_result, &calling_result, &overlapped_error, &overlapped_extended_error, true)) {
        overlapped_update();
        SwitchToThread();
    }
    if (task_slot->task)
        task_now_finished(task_slot, return_result, calling_result, overlapped_error, overlapped_extended_error);
}

// @address 0x88880 ?overlapped_update@@YAXXZ
void overlapped_update()
{
    for (unsigned long slot_index = 0; slot_index < 64; slot_index++) {
        {
            async_xoverlapped_critical_section_enter(24);
            s_task_slot *slot = &g_overlapped_globals.slots[slot_index];
            if (slot->task && slot->task->get_task_state() == 2) {
                unsigned long return_result, calling_result, overlapped_error, overlapped_extended_error;
                if (task_is_complete(slot, &return_result, &calling_result, &overlapped_error, &overlapped_extended_error, false))
                    task_now_finished(slot, return_result, calling_result, overlapped_error, overlapped_extended_error);
                else
                    slot->task->update(return_result);
            }
            internal_critical_section_leave(24);
        }
        {
            async_xoverlapped_critical_section_enter(24);
            s_task_slot *slot = &g_overlapped_globals.slots[slot_index];
            if (slot->task && slot->task->get_task_state() == 1) {
                long pending_count = 0;
                for (unsigned long index = 0; index < 64; index++) {
                    if (g_overlapped_globals.slots[index].task && g_overlapped_globals.slots[index].task->get_task_state() == 2)
                        pending_count++;
                }
                if (pending_count < 3) {
                    unsigned long start_time = GetTickCount();
                    slot->calling_result = slot->task->start(&slot->overlapped);
                    slot->task->set_task_state_internal(_overlapped_task_state_pending);
                    slot->terminated = false;
                    g_overlapped_globals.task_started = true;
                    g_overlapped_globals.task_start_time = start_time;
                }
            }
            internal_critical_section_leave(24);
        }
    }
}

// @address 0x96b40 ?online_lsp_service_available@@YA_NXZ
// 0x96b40: is any local user signed in with online enabled
bool online_lsp_service_available()
{
    bool result = false;
    long user_index = 0;
    while (user_index < 4 && !result)
    {
        long signin_info[12];
        if (::online_local_xuid_is_online_enabled())
        {
            bool signed_in = false;
            if (overlapped_get_signin_info(user_index, 4, signin_info) == 0)
                signed_in = true;
            if (signed_in)
            {
                result = true;
                break;
            }
        }
        user_index++;
    }
    return result;
}

// match: 0x950f8: optimize pragma preserves function frame layout
#pragma optimize("s", on)
// 0x950f8: XamUserGetSigninInfo's HRESULT as a Win32 error code

// @address 0x950f8 overlapped_get_signin_info
extern "C" unsigned long overlapped_get_signin_info(unsigned long user_index, unsigned long flags, void *signin_info)
{
    unsigned long error;
    long result = XamUserGetSigninInfo(user_index, flags, signin_info);
    if (SUCCEEDED(result))
        error = ERROR_SUCCESS;
    else
        error = HRESULT_FACILITY(result) == FACILITY_WIN32 ? HRESULT_CODE(result) : ERROR_FUNCTION_FAILED;
    return error;
}
