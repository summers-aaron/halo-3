#ifndef MAIN_MAIN_TIME_TYPES_H
#define MAIN_MAIN_TIME_TYPES_H

#include <stddef.h>

// Main-thread timing state shared by main_time, observer and player_control.
// March main_time_mark_input_collection_time (828ea018) stores the timestamp at +0x28.
struct s_main_time_globals
{
    bool custom_time;
    bool throttled;
    long game_time;
    __int64 milliseconds;
    __int64 vblank_index;
    __int64 initial_vblank_index;
    __int64 target_vblank_index;
    __int64 input_collection_milliseconds;
    __int64 synchronization_milliseconds;
    __int64 last_system_milliseconds;
};

typedef char main_time_globals_size[(sizeof(s_main_time_globals) == 0x40) ? 1 : -1];
typedef char main_time_milliseconds_offset[(offsetof(s_main_time_globals, milliseconds) == 8) ? 1 : -1];
typedef char main_time_input_collection_offset[(offsetof(s_main_time_globals, input_collection_milliseconds) == 0x28) ? 1 : -1];

#endif // MAIN_MAIN_TIME_TYPES_H
