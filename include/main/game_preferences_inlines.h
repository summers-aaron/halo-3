#ifndef MAIN_GAME_PREFERENCES_INLINES_H
#define MAIN_GAME_PREFERENCES_INLINES_H
#include "main/game_preferences_types.h"

#include "main/thread_globals_lock_inlines.h"
#include "multithreading/threads.h"
#include "multithreading/thread_tls.h"

static __forceinline s_global_preferences *&global_preferences_thread_storage()
{
    return thread_global_preferences;
}

#include "main/game_preferences_defaults.h"

#endif // MAIN_GAME_PREFERENCES_INLINES_H
