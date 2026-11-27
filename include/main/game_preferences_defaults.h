#ifndef MAIN_GAME_PREFERENCES_DEFAULTS_H
#define MAIN_GAME_PREFERENCES_DEFAULTS_H

#include "multithreading/threads.h"
#include "multithreading/thread_tls.h"
#include "multithreading/synchronized_value.h"

namespace {
static __forceinline bool global_preferences_should_use_defaults()
{
    return g_thread_system_initialized.peek() == 0 || get_current_thread_index() == 3;
}
}

#endif // MAIN_GAME_PREFERENCES_DEFAULTS_H
