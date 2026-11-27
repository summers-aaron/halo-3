#ifndef MAIN_GAME_PREFERENCES_GLOBALS_H
#define MAIN_GAME_PREFERENCES_GLOBALS_H

#include "multithreading/synchronized_value.h"

struct s_global_preferences_close_state
{
    bool requested;
    long task;
    c_synchronized_long done;
};

extern s_global_preferences_close_state g_global_preferences_close_state;

#endif // MAIN_GAME_PREFERENCES_GLOBALS_H
