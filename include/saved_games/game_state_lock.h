#ifndef SAVED_GAMES_GAME_STATE_LOCK_H
#define SAVED_GAMES_GAME_STATE_LOCK_H

#include "multithreading/synchronized_value.h"

struct s_game_state_shared_lock
{
    c_synchronized_long lock;
    volatile unsigned long render_thread_lock;
};

extern "C++" c_synchronized_long g_game_state_lock;

// Load and revert paths set this before callbacks run and clear it
// before unlocking resources (game_state_unlock_and_resume_render_thread).
// The announcer stop callback must leave its game-state queue untouched then.

static __forceinline bool game_state_is_locked()
{
    return g_game_state_lock.value != 0;
}

#endif
