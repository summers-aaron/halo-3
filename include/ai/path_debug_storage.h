#ifndef AI_PATH_DEBUG_STORAGE_H
#define AI_PATH_DEBUG_STORAGE_H

#include <stddef.h>
#include "ai/path_state_types.h"
#include "ai/path_result.h"
#include "ai/obstacle_path_types.h"

// March ai_debug.obj initializes actor_index, time and valid; path.obj saves
// state, then result and 40/4/4 raw/smoothed/avoided path steps, and
// locked at +0x28808, as accessed by path_obstacles.cpp.
struct path_debug_storage
{
    long actor_index;
    long time;
    long : 32;
    bool valid;
    unsigned char : 8;
    short : 16;
    short path_traverse_result;
    short path_build_result;
    path_state state;
    path_result result;
    short raw_step_count;
    path_step raw_steps[40];
    short smoothed_step_count;
    path_step smoothed_steps[4];
    short avoided_step_count;
    path_step avoided_steps[4];
    bool locked;
    unsigned short path_count;
    struct obstacles obstacles[4];
    obstacle_path paths[4];
};

typedef char path_debug_storage_valid_offset[(offsetof(path_debug_storage, valid) == 0xc) ? 1 : -1];
typedef char path_debug_storage_traverse_result_offset[(offsetof(path_debug_storage, path_traverse_result) == 0x10) ? 1 : -1];
typedef char path_debug_storage_state_offset[(sizeof(void *) != 4 || ((offsetof(path_debug_storage, state) == 0x14))) ? 1 : -1];
typedef char path_debug_storage_locked_offset[(sizeof(void *) != 4 || ((offsetof(path_debug_storage, locked) == 0x28808))) ? 1 : -1];
typedef char path_debug_storage_obstacles_offset[(sizeof(void *) != 4 || ((offsetof(path_debug_storage, obstacles) == 0x2880c))) ? 1 : -1];

#endif // AI_PATH_DEBUG_STORAGE_H
