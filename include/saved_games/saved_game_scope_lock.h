#ifndef SAVED_GAMES_SAVED_GAME_SCOPE_LOCK_H
#define SAVED_GAMES_SAVED_GAME_SCOPE_LOCK_H
#include "multithreading/synchronization.h"
#include "saved_games/content_synchronization_globals_types.h"
#include "saved_games/content_critical_section_types.h"
#include "cseries/crt_xdk.h"
namespace {
class c_saved_game_scope_lock
{
public:
    __forceinline c_saved_game_scope_lock()
    {
        internal_critical_section_enter(21);
    }
    __forceinline ~c_saved_game_scope_lock()
    {
        s_content_synchronization_globals *globals = (s_content_synchronization_globals *)(&g_synch_globals);
        _InterlockedDecrement(&globals->critical_sections[21].reference_count);
        RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)globals->critical_sections[21].native_lock);
    }
};
}
#endif
