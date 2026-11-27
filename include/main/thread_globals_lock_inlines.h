#ifndef MAIN_THREAD_GLOBALS_LOCK_INLINES_H
#define MAIN_THREAD_GLOBALS_LOCK_INLINES_H
#include "main/game_preferences_types.h"

#include "main/thread_globals_lock.h"
#include "multithreading/synchronization.h"

extern s_global_preferences g_global_preferences;

__forceinline c_thread_globals_lock::c_thread_globals_lock(e_inline_thread_globals_lock)
{
    m_acquired = false;
    if (thread_global_preferences == 0)
    {
        internal_critical_section_enter(57);
        m_acquired = true;
        thread_global_preferences = &g_global_preferences;
    }
}

#endif // MAIN_THREAD_GLOBALS_LOCK_INLINES_H
