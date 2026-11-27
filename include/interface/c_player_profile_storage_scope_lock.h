#ifndef INTERFACE_C_PLAYER_PROFILE_STORAGE_SCOPE_LOCK_H
#define INTERFACE_C_PLAYER_PROFILE_STORAGE_SCOPE_LOCK_H

#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "multithreading/thread_tls.h"
#include "cseries/crt_xdk.h"

// Critical section 24 guards the async profile read/write storage in
// tu/interfacelibxbox/c_player_profile_storage.cpp.
namespace {
class c_player_profile_storage_scope_lock {
public:
    __forceinline c_player_profile_storage_scope_lock() { internal_critical_section_enter(24); }
    __forceinline ~c_player_profile_storage_scope_lock()
    {
        s_critical_section_handle *section = &g_synch_globals.critical_sections[24];
        section->lock_count.decrement();
        RtlLeaveCriticalSection(&section->critical_section);
    }
};
// Enters the profile storage critical section with the enter inlined (the dispose loop).
class c_player_profile_storage_inline_scope_lock {
public:
    __forceinline c_player_profile_storage_inline_scope_lock()
    {
        s_critical_section_handle *section = &g_synch_globals.critical_sections[24];
        RtlEnterCriticalSection(&section->critical_section);
        section->locking_thread.set(get_current_thread_index());
        section->lock_count.increment();
    }
    __forceinline ~c_player_profile_storage_inline_scope_lock()
    {
        s_critical_section_handle *section = &g_synch_globals.critical_sections[24];
        section->lock_count.decrement();
        RtlLeaveCriticalSection(&section->critical_section);
    }
};
}

#endif // INTERFACE_C_PLAYER_PROFILE_STORAGE_SCOPE_LOCK_H
