#ifndef MAIN_MAIN_TIME_GLOBALS_H
#define MAIN_MAIN_TIME_GLOBALS_H

#include "main/main_time_types.h"

extern __declspec(thread) s_main_time_globals *g_main_time_globals;

extern "C" unsigned long g_main_time_throttle_swap_index;

#endif // MAIN_MAIN_TIME_GLOBALS_H
