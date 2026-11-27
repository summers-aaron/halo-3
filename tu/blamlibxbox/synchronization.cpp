// blamlibxbox/synchronization.obj: from the records 0x81638 0x846a8 0x141df8 0x142278 0x1c19c0 0x1c1b50 0x1c1d80 0x1c1dd8 0x1c1e98 0x1c1f40 0x1c20b8 0x1c2158
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/synchronization_types.h"
#include "multithreading/thread_tls.h"

// @address 0x70d394 RtlEnterCriticalSection
// @address 0x70d3a4 RtlLeaveCriticalSection
// @address 0x70d4a4 RtlTryEnterCriticalSection
// @address 0x70d474 RtlInitializeCriticalSectionAndSpinCount
// @address 0x81708 WaitForSingleObjectEx
// @address 0x88ec0 ReleaseSemaphore
#include "multithreading/synchronization_runtime.h"

struct s_critical_section_definition
{
    const char *name;
    long spin_count;
};
struct s_mutex_definition
{
    const char *name;
    unsigned char initially_owned;
};
struct s_event_definition
{
    const char *name;
    unsigned char manual_reset;
    unsigned char initial_state;
};
#include "multithreading/semaphore_definitions.h"
// @address 0x74b1c0 g_critical_section_definitions
static s_critical_section_definition g_critical_section_definitions[58] = {
    { "CS:Event Logs", 0 },
    { "CS:Data Mine", 0 },
    { "CS:D3D_Device", 0 },
    { "CS:Terminal", 0 },
    { "CS:Network Debug", 0 },
    { "CS:Render Toggle", 0 },
    { "CS:Event RW Lock", 0 },
    { "CS:Header Region", 0 },
    { "CS:Header Section", 0 },
    { "CS:Update Region", 0 },
    { "CS:Update Section", 0 },
    { "CS:Render Region", 0 },
    { "CS:Render Section", 0 },
    { "CS:Shared Region", 0 },
    { "CS:Shared Section", 0 },
    { "CS:Shared Mirror0", 0 },
    { "CS:Shared Mirror1", 0 },
    { "CS:Shared Mirror2", 0 },
    { "CS:UI widgets", 0 },
    { "CS:UI memory", 0 },
    { "CS:UI custom bitmaps", 0 },
    { "CS:Saved Game", 0 },
    { "CS:Levels", 0 },
    { "CS:Autosave Queue", 0 },
    { "CS:XOverlapped", 0 },
    { "CS:XOverlapped Memory", 0 },
    { "CS:Game Engine", 0 },
    { "CS:Http Request Queue", 0 },
    { "CS:LSP Manager", 0 },
    { "CS:Online files", 0 },
    { "CS:Telnet console", 0 },
    { "CS:G-Render Region", 0 },
    { "CS:G-Render Section", 0 },
    { "CS:Bink Prediction", 0 },
    { "CS:Bink Texture 0", 0 },
    { "CS:Bink Texture 1", 0 },
    { "CS:Bink Memory", 0 },
    { "CS:Font Cache", 0 },
    { "CS:Sound Cache", 0 },
    { "CS:Tag Cache", 0 },
    { "CS:Partition Cache", 0 },
    { "CS:Disk Cache", 0 },
    { "CS:Test Cache", 0 },
    { "CS:Prof Display", 0 },
    { "CS:Prof Session", 0 },
    { "CS:Resource RW Lock", 0 },
    { "CS:Simple Resource Access RW Lock", 0 },
    { "CS:Resource Publish", 0 },
    { "CS:Remote Console", 0 },
    { "CS:Observer Updates", 0 },
    { "CS:Web Cache", 0 },
    { "CS:Prof-Gather", 0 },
    { "CS:Prof-Register", 0 },
    { "CS:Optional Cache", 0 },
    { "CS:Net Block Detection", 0 },
    { "CS:Net Statistics", 0 },
    { "CS:DSP-Radio", 0 },
    { "CS:Global Prefs", 0 },
};
// @address 0x74b390 g_mutex_definitions
static s_mutex_definition g_mutex_definitions[3] = {
    { "MTX:AsyncFree", 0 },
    { "MTX:AsyncWork", 0 },
    { "MTX:DataMine", 0 },
};
// @address 0x74b3a8 g_event_definitions
static s_event_definition g_event_definitions[7] = {
    { "EV:CSeriesEvent", 1, 0 },
    { "EV:UploadDebug", 1, 0 },
    { "EV:NetworkDebug", 1, 0 },
    { "EV:VBlankUpdate", 0, 0 },
    { "EV:VBlankThrottle", 0, 0 },
    { "EV:SoundRenderDeferredStart", 0, 0 },
    { "EV:SoundRenderDeferredComplete", 0, 1 },
};
// @address 0x74b3e0 g_semaphore_definitions
static s_semaphore_definition g_semaphore_definitions[5] = {
    { "S4:AsyncWork", 0, 25, 1 },
    { "S4:EventRW", 0, 9, 0 },
    { "S4:ResourceRW", 0, 9, 0 },
    { "S4:SRPublishRW", 0, 9, 0 },
    { "S4:Bink Memory", 0, 9, 0 },
};
#include "multithreading/critical_section_scope.h"

static __forceinline bool synchronization_semaphore_uses_single_thread_reference(long semaphore_id)
{
    return g_semaphore_definitions[semaphore_id].single_thread_reference != 0;
}
long sempahore_get_max_signal_count(long semaphore_id)
{
    return g_semaphore_definitions[semaphore_id].maximum_count;
}

static __forceinline s_synchronization_globals &synchronization_globals()
{
    return g_synch_globals;
}

// match: 0x81638, 0x1c1dd8: twin inlines the wait here; calling 0x1c1d80 directly emits bl (full-image marks called @address functions noinline)
static __forceinline bool wait_for_single_object_internal_helper(void *handle, unsigned long timeout_in_milliseconds)
{
    if (handle) {
        unsigned long wait_result = WaitForSingleObjectEx(handle, timeout_in_milliseconds, 0);
        bool success = false;
        if (wait_result != 0) {
            success = wait_result == 128;
            return success;
        }
        success = true;
        return success;
    } else {
        return true;
    }
}

// @address 0x1c1d80 ?wait_for_single_object_internal@@YA_NPAXK@Z
bool wait_for_single_object_internal(void *handle, unsigned long timeout_in_milliseconds)
{
    return wait_for_single_object_internal_helper(handle, timeout_in_milliseconds);
}

// @address 0x81638 ?internal_mutex_take@@YAXJ@Z
void internal_mutex_take(long mutex_id)
{
    bool result = wait_for_single_object_internal_helper(synchronization_globals().mutexes[mutex_id].handle, ~0UL);
    if (result)
        synchronization_globals().mutexes[mutex_id].increment_lock_count(get_current_thread_index());
}

// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
void internal_critical_section_enter(long index)
{
    volatile long *owner_thread;
    RtlEnterCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    owner_thread = &g_synch_globals.critical_sections[index].locking_thread.value;
    _InterlockedExchange(owner_thread, get_current_thread_index());
    _InterlockedIncrement(owner_thread + 1);
}
// @address 0x141df8 ?internal_semaphore_release@@YAJJ@Z
long internal_semaphore_release(long semaphore_id)
{
    long previous_count = 0;
    int result = ReleaseSemaphore(synchronization_globals().semaphores[semaphore_id].handle, 1, &previous_count);
    if (result) {
        synchronization_globals().semaphores[semaphore_id].remove_thread_reference(
            synchronization_semaphore_uses_single_thread_reference(semaphore_id) ? 0 : get_current_thread_index());
    }
    return previous_count;
}

// @address 0x142278 ?internal_critical_section_try_and_enter@@YA_NJ@Z
bool internal_critical_section_try_and_enter(long critical_section_id)
{
    long result = 1;
    if (critical_section_id != -1) {
        result = RtlTryEnterCriticalSection(&synchronization_globals().critical_sections[critical_section_id].critical_section);
        if (result)
            synchronization_globals().critical_sections[critical_section_id].increment_lock_count(get_current_thread_index());
    }
    return result != 0;
}

// @address 0x1c19c0 ??0s_synchronization_globals@@QAA@XZ
// match: 0x1c19c0: singleton global constructor specialization
s_synchronization_globals::s_synchronization_globals() : initialized((__assume(this == &g_synch_globals), false))
{
}

static __forceinline void synchronization_set_lock_count(c_synchronization_object *object, long thread_index, long count)
{
    _InterlockedExchange(&object->locking_thread.value, thread_index);
    _InterlockedExchange(&object->lock_count.value, count);
}

// @address 0x1c1dd8 ?internal_semaphore_take@@YAXJ@Z
void internal_semaphore_take(long semaphore_id)
{
    bool result = wait_for_single_object_internal_helper(synchronization_globals().semaphores[semaphore_id].handle, ~0UL);
    if (result) {
        synchronization_globals().semaphores[semaphore_id].add_thread_reference(
            synchronization_semaphore_uses_single_thread_reference(semaphore_id) ? 0 : get_current_thread_index());
    }
}

// @address 0x1c1e98 ?internal_critical_section_leave@@YAXJJ@Z
void internal_critical_section_leave(long thread_index, long critical_section_id)
{
    if (g_synch_globals.critical_sections[critical_section_id].get_locking_thread() == thread_index) {
        while (g_synch_globals.critical_sections[critical_section_id].lock_count.value != 0) {
            if (critical_section_id != -1) {
                _InterlockedDecrement(&g_synch_globals.critical_sections[critical_section_id].lock_count.value);
                RtlLeaveCriticalSection(&g_synch_globals.critical_sections[critical_section_id].critical_section);
            }
        }
    }
}
static __forceinline void synchronization_release_thread_semaphores()
{
    if (synchronization_globals().initialized) {
        long thread_index = get_current_thread_index();
        for (long semaphore_id = 0; semaphore_id < 5; ++semaphore_id) {
            long reference_index = synchronization_semaphore_uses_single_thread_reference(semaphore_id) ? 0 : thread_index;
            while (synchronization_globals().semaphores[semaphore_id].thread_references[reference_index] != 0) {
                if (internal_semaphore_release(semaphore_id) == 0)
                    break;
            }
        }
    }
}
static __forceinline void synchronization_release_thread_mutexes()
{
    if (synchronization_globals().initialized) {
        long thread_index = get_current_thread_index();
        for (long mutex_id = 0; mutex_id < 3; ++mutex_id) {
            long owner = synchronization_globals().mutexes[mutex_id].get_locking_thread();
            if (owner == thread_index) {
                while (synchronization_globals().mutexes[mutex_id].lock_count != 0) {
                    _InterlockedDecrement(&synchronization_globals().mutexes[mutex_id].lock_count.value);
                    ReleaseMutex(synchronization_globals().mutexes[mutex_id].handle);
                }
            }
        }
    }
}
static __forceinline void synchronization_release_thread_critical_sections()
{
    if (synchronization_globals().initialized) {
        long thread_index = get_current_thread_index();
        for (long critical_section_id = 0; critical_section_id < 58; ++critical_section_id)
            internal_critical_section_leave(thread_index, critical_section_id);
    }
}
// @address 0x1c1f40 synchronization_release_thread_locks
extern "C" void synchronization_release_thread_locks()
{
    if (synchronization_globals().initialized) {
        synchronization_release_thread_semaphores();
        synchronization_release_thread_mutexes();
        synchronization_release_thread_critical_sections();
    }
}

// @address 0x1c20b8 ?synchronization_leave_registered_sections@@YAXXZ
void synchronization_leave_registered_sections()
{
    if (synchronization_globals().initialized) {
        long thread_index = get_current_thread_index();
        internal_critical_section_leave(thread_index, 0);
        internal_critical_section_leave(thread_index, 1);
        internal_critical_section_leave(thread_index, 18);
        internal_critical_section_leave(thread_index, 37);
        internal_critical_section_leave(thread_index, 20);
        internal_critical_section_leave(thread_index, 21);
        internal_critical_section_leave(thread_index, 22);
        internal_critical_section_leave(thread_index, 23);
    }
}
// @address 0x80608 GetTickCount
// @address 0x59e430 SwitchToThread
static __forceinline bool switch_to_thread() {
    bool result = false;
    if (SwitchToThread()) result = true;
    return result;
}
// @address 0x1c2158 ??0c_critical_section_scope@@QAA@JJPA_N@Z
c_critical_section_scope::c_critical_section_scope(long critical_section, long timeout_in_milliseconds, bool *out_lock_acquired)
{
    // match: 0x1c2158: caller constant specialization for critical_section
    __assume(critical_section == 29);
    // match: 0x1c2158: caller constant specialization for timeout_in_milliseconds
    __assume(timeout_in_milliseconds == 1000);
    m_critical_section = critical_section;
    m_locked = false;
    unsigned long timeout = GetTickCount() + timeout_in_milliseconds;
    *out_lock_acquired = false;
    do {
        m_locked = internal_critical_section_try_and_enter(m_critical_section);
        if (m_locked) {
            *out_lock_acquired = true;
            break;
        } else {
            switch_to_thread();
        }
    } while (GetTickCount() < timeout);
}

// @address 0x1c1b50 ?initialize_synchronization_objects@@YAXXZ
void initialize_synchronization_objects()
{
    s_synchronization_globals &globals = synchronization_globals();
    const s_critical_section_definition *critical_section_definitions = g_critical_section_definitions;
    for (long critical_section_id = 0; critical_section_id < 58; ++critical_section_id) {
        RtlInitializeCriticalSectionAndSpinCount(&globals.critical_sections[critical_section_id].critical_section,
            critical_section_definitions[critical_section_id].spin_count);
        globals.critical_sections[critical_section_id].reset();
    }
    const s_mutex_definition *mutex_definitions = g_mutex_definitions;
    for (long mutex_id = 0; mutex_id < 3; ++mutex_id) {
        globals.mutexes[mutex_id].handle
            = CreateMutexA(0, mutex_definitions[mutex_id].initially_owned, 0);
        globals.mutexes[mutex_id].reset();
        synchronization_set_lock_count(&globals.mutexes[mutex_id],
            get_current_thread_index(), mutex_definitions[mutex_id].initially_owned != 0);
    }
    const s_event_definition *event_definitions = g_event_definitions;
    for (long event_id = 0; event_id < 7; ++event_id) {
        globals.events[event_id].handle
            = CreateEventA(0, event_definitions[event_id].manual_reset, event_definitions[event_id].initial_state, 0);
        globals.events[event_id].reset();
    }
    const s_semaphore_definition *semaphore_definitions = g_semaphore_definitions;
    for (long semaphore_id = 0; semaphore_id < 5; ++semaphore_id) {
        globals.semaphores[semaphore_id].handle = CreateSemaphoreA(0,
            semaphore_definitions[semaphore_id].initial_count, semaphore_definitions[semaphore_id].maximum_count, 0);
        globals.semaphores[semaphore_id].reset();
    }
    globals.initialized = true;
}

s_synchronization_globals g_synch_globals;

