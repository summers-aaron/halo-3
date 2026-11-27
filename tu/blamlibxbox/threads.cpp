// blamlibxbox/threads.obj: from the records 0x1acb58 0x1acc40 0x1b5fe8 0x1b6060 0x1b61b0 0x1b6258 0x1b6358 0x1b6400 0x1b64f8 0x1d5da8 0x211d50 0x70b6d0 0x70b6d8 0x70b6e0
// @address 0x11df674 g_registered_thread_index
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "multithreading/threads_externs.h"
// @address 0x11df64c g_thread_assert_arguments
// @address 0x11df826 g_thread_assert_triggered
__declspec(thread) s_thread_assert_arguments g_thread_assert_arguments;
__declspec(thread) bool g_thread_assert_triggered;
// @address 0x5a2948 rasterizer_device_prepare_thread_release
#include "rasterizer/rasterizer_main_globals.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_layout.h"
#include <xtl.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/threads.h"
#include "multithreading/synchronization.h"
#include "cseries/async.h"
#include "networking/network_debug_dump.h"
#include "rasterizer/rasterizer_main_thread.h"
#include "sound/sound_xaudio_xenon.h"

#include "multithreading/registered_thread_index.h"

#if !defined(_XBOX) || defined(__clang__)
__declspec(thread) long g_registered_thread_index;
#endif

enum e_thread_priority { priority_default = 0 };

#include "multithreading/thread_definitions.h"

// @address 0x3a940 k_thread_default_definitions
// @rdata 0x8203a940..0x8203aa18
const s_thread_default_row k_thread_default_definitions[] = {
    { "UNKNOWN_THREAD", 4, 0, 4, 0, 0 },
    { "MAIN_THREAD", 0, 0, 4, 0, 0 },
    { "RENDER", 2, 0x64000, 4, (unsigned long (*)(void *))rasterizer_main_render_thread, 0 },
    { "ASYNC_IO", 5, 0x10000, 4, async_main, 0 },
    { "NET_DEBUG", 5, 0x10000, 4, (unsigned long (*)(void *))network_debug_dump_update_upload_queue, 0 },
    { "AUDIO", 4, 0x10000, 7, (unsigned long (*)(void *))sound_xaudio_xenon_run_source_update_loop, 0 },
    { "PROFILER", 5, 0x10000, 4, 0, 0 },
    { "EVENT_LOGS", 4, 0x10000, 4, 0, 0 },
    { "NET_BLOCK", 5, 0x10000, 4, 0, 0 },
};
// @rdata-end

struct s_thread_system_globals;

#include "multithreading/synchronized_value.h"


#include "render/render_view.h"
class c_restricted_section;
static __forceinline bool threads_section_owned_by_current_thread(c_restricted_section *section);
#include "cache/restricted_memory.h"

#include "multithreading/thread_tls.h" // this thread's base of each region is declared by tls_data_arrays.h

#include "rasterizer/rasterizer.h"
static __forceinline bool threads_rasterizer_thread_owns_device() { return g_thread_owning_device.peek() == get_current_thread_index(); }



extern "C" {
// @address 0x7dafcc g_first_exception_tick_count
unsigned long g_first_exception_tick_count;
}

static __forceinline bool threads_section_owned_by_current_thread(c_restricted_section *section)
{
    // match: the exception path originally reads volatile section flags, including the second ownership check.
    bool owned = ((volatile unsigned long &)section->m_internal_flags & 1) && section->m_owner_thread == get_current_thread_index();
    return owned;
}

static __forceinline void threads_restricted_region_handle_crash_of_current_thread()
{
    for (long region_index = 0; region_index < 5; region_index++)
    {
        g_restricted_address[region_index] = NULL;
        if (((s_field_layout *)g_restricted_regions)[region_index].flags & 1)
            threads_unlock_restricted_region(&((s_field_layout *)g_restricted_regions)[region_index]);
    }
}

static __forceinline void thread_release_locks_and_d3d_device()
{
    if (threads_rasterizer_thread_owns_device())
    {
        rasterizer_device_prepare_thread_release(c_rasterizer::get_device());
        c_rasterizer::rasterizer_device_release_thread();
        c_view::abort_current_view_stack();
    }
    threads_restricted_region_handle_crash_of_current_thread();
    synchronization_release_thread_locks();
}

static __forceinline void thread_copy_assert_arguments(_EXCEPTION_POINTERS *exception_pointers)
{
    if (exception_pointers && exception_pointers->ExceptionRecord)
    {
        memcpy(exception_pointers->ExceptionRecord->ExceptionInformation, &g_thread_assert_arguments, sizeof(g_thread_assert_arguments));
        exception_pointers->ExceptionRecord->NumberParameters = 4;
    }
}

// @address 0x1d5da8 threads_unlock_restricted_region
extern "C" void threads_unlock_restricted_region(s_field_layout *region_layout)
{
    c_restricted_memory *region = (c_restricted_memory *)region_layout;
    if ((region->m_internal_flags & 1) != 0) {
        long current_slot;
        if (region->m_primary_locked.peek() != 0)
            current_slot = region->m_mirror_readers.peek() % region->m_mirror_count;
        else
            current_slot = -1;
        long mirror_count = region->m_mirror_count;
        long mirror_index = 0;
        if (mirror_count > 0) {
            do {
                c_restricted_section *section = region->m_mirrors[mirror_index].section;
                if (section != 0 && (section->m_internal_flags & 1) != 0) {
                    if (threads_section_owned_by_current_thread(section)) {
                        if (mirror_index == current_slot)
                            region->internal_unlock_mirror(false);
                        else
                            section->unlock();
                    }
                }
                mirror_index++;
            } while (mirror_index < mirror_count);
        }
        c_restricted_section *primary = region->m_primary_section;
        if (threads_section_owned_by_current_thread(primary))
            primary->unlock();
    }
}

// @address 0x1acb58 thread_copy_exception_record
extern "C" void thread_copy_exception_record(_EXCEPTION_POINTERS *exception_pointers, s_thread_exception_record *record)
{
    if (record != NULL) {
        memset(record, 0, sizeof(s_thread_exception_record));
        if (exception_pointers != NULL) {
            record->thread_id = GetCurrentThreadId();
            record->context = *exception_pointers->ContextRecord;
            record->exception_code = exception_pointers->ExceptionRecord->ExceptionCode;
            record->exception_flags = exception_pointers->ExceptionRecord->ExceptionFlags;
            record->exception_address = exception_pointers->ExceptionRecord->ExceptionAddress;
            if (exception_pointers->ExceptionRecord->ExceptionInformation != NULL) {
                unsigned long count = exception_pointers->ExceptionRecord->NumberParameters;
                if (count >= 4)
                    count = 4;
                record->parameter_count = count;
                memcpy(record->parameters, exception_pointers->ExceptionRecord->ExceptionInformation, count * sizeof(ULONG_PTR));
            }
            record->state.set(1);
        }
    }
}

// @address 0x1acc40 thread_record_first_exception
extern "C" void thread_record_first_exception(_EXCEPTION_POINTERS *exception_pointers)
{
    if (g_first_exception_flag.set(1) == 0)
    {
        long state = g_main_crash_data.state.peek();
        if (state == 0)
        {
            thread_copy_exception_record(exception_pointers, &g_main_crash_data);
            g_main_crash_data.state.set(1);
            g_first_exception_tick_count = GetTickCount();
        }
    }
}

// @address 0x1b5fe8 ?SetThreadName@@YAXKPBD@Z
void SetThreadName(unsigned long thread_id, const char *thread_name)
{
    s_thread_name_info info;
    info.type = 0x1000;
    info.name = thread_name;
    info.thread_id = thread_id;
    info.flags = 0;
    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(unsigned long), (const ULONG_PTR *)&info);
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}

// @address 0x1b6060 ?thread_execution_crash_handler@@YAHPAU_EXCEPTION_POINTERS@@J@Z
int thread_execution_crash_handler(_EXCEPTION_POINTERS *exception_pointers, long thread_index)
{
    g_thread_crashed[thread_index].set(1);
    thread_release_locks_and_d3d_device();
    if (g_thread_assert_triggered)
    {
        thread_copy_assert_arguments(exception_pointers);
        if (exception_pointers->ExceptionRecord)
            exception_pointers->ExceptionRecord->ExceptionCode = 'asrt';
    }
    thread_record_first_exception(exception_pointers);
    for (;;)
        Sleep(1000);
    return EXCEPTION_CONTINUE_SEARCH;
}

// @address 0x1b61b0 thread_execute_start_routine
extern "C" int thread_execute_start_routine(int thread_index)
{
    int result = 1;
    const s_thread_default_row *definition = &k_thread_default_definitions[thread_index];
    set_current_thread_index(thread_index);
    __try
    {
        result = definition->start_routine(definition->parameter);
    }
    __except (thread_execution_crash_handler(GetExceptionInformation(), thread_index))
    {
    }
    return result;
}

// @address 0x1b6258 ?initialize_thread_management@@YAXXZ
void initialize_thread_management()
{
    SetThreadName(GetCurrentThreadId(), "MAIN_THREAD");
    for (int thread_index = 0; thread_index < 9; thread_index++) {
        g_thread_globals.handles[thread_index] = (void *)-1;
        g_thread_crashed[thread_index].set(0);
        g_thread_should_exit[thread_index].set(0);
    }
    g_thread_globals.handles[0] = (void *)-1;
    set_current_thread_index(1);
    g_thread_globals.thread_ids[0] = -1;
    g_thread_globals.handles[1] = (void *)-2;
    g_thread_globals.thread_ids[1] = GetCurrentThreadId();
    g_thread_system_initialized.set(1);
}

// 0x1b6358
void thread_set_priority(long thread_index, e_thread_priority priority)
{
    long priority_value = priority;
    if (priority_value == 0)
        priority_value = k_thread_default_definitions[thread_index].default_priority;

    long native_priority;
    switch (priority_value - 1) {
    case 0:
        native_priority = -15;
        break;
    case 1:
        native_priority = -2;
        break;
    case 2:
        native_priority = -1;
        break;
    case 3:
        native_priority = 0;
        break;
    case 4:
        native_priority = 1;
        break;
    case 5:
        native_priority = 2;
        break;
    case 6:
        native_priority = 15;
        break;
    default:
        // match: 0x1b6358: without this the switch gains a cmplwi-6/bgt range check, a 0x60 frame, and bl+epilogue instead of the tail call
        __assume(0);
    }
    SetThreadPriority((HANDLE)g_thread_globals.handles[thread_index], native_priority);
}

// @address 0x1b6400 ?start_thread@@YAXJ@Z
void start_thread(long thread_index)
{
    g_thread_should_exit[thread_index].set(0);
    g_thread_crashed[thread_index].set(0);
    if (k_thread_default_definitions[thread_index].start_routine) {
        int thread_id;
        HANDLE handle = CreateThread(0, k_thread_default_definitions[thread_index].stack_size,
            (LPTHREAD_START_ROUTINE)thread_execute_start_routine,
            reinterpret_cast<void *>(static_cast<intptr_t>(thread_index)), 0, (unsigned long *)&thread_id);
        const char *name = k_thread_default_definitions[thread_index].name;
        g_thread_globals.handles[thread_index] = handle;
        g_thread_globals.thread_ids[thread_index] = thread_id;
        g_thread_globals.thread_parameters[thread_index] = 0;
        SetThreadName(g_thread_globals.thread_ids[thread_index], name);
        XSetThreadProcessor((HANDLE)g_thread_globals.handles[thread_index], k_thread_default_definitions[thread_index].processor);
        thread_set_priority(thread_index, priority_default);
    }
}

// 0x1b64f8
bool wait_for_thread_to_exit(long thread_index, unsigned long timeout_in_milliseconds)
{
    g_thread_should_exit[thread_index].set(1);
    void *handle = (void *)g_thread_globals.handles[thread_index];
    if (timeout_in_milliseconds != 0xFFFFFFFFUL)
        return wait_for_single_object_internal(handle, timeout_in_milliseconds);
    if (handle != 0) {
        unsigned long result = WaitForSingleObjectEx(handle, 0xFFFFFFFFUL, 0);
        if (result != 0)
            return result == 128;
    }
    return true;
}

// @address 0x211d50 ?havok_notify_thread_running@@YAXPAX@Z
void havok_notify_thread_running(void *field)
{
    player_control_globals = static_cast<s_player_control_globals *>(field);
}




const char *get_registered_thread_name(long thread_index)
{
    (void)&get_thread_name_from_thread_id;
    return k_thread_default_definitions[thread_index].name;
}

__forceinline const char *get_thread_name_from_thread_id(unsigned long thread_id)
{
    const char *name = "unknown thread";
    for (unsigned long thread_index = 0; thread_index < 9; thread_index++)
    {
        if (g_thread_globals.thread_ids[thread_index] == thread_id)
        {
            name = get_registered_thread_name(thread_index);
            break;
        }
    }
    return name;
}

c_interlocked_long g_thread_should_exit[9];
c_interlocked_long g_thread_crashed[9];
c_interlocked_long g_thread_system_initialized;

// @address 0x8a1f08 g_thread_globals
s_thread_system_globals g_thread_globals;

