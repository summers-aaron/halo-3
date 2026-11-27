#ifndef SIMULATION_SIMULATION_GLOBALS_H
#define SIMULATION_SIMULATION_GLOBALS_H

#include <stddef.h>

class c_simulation_world;
class c_simulation_watcher;
class c_simulation_type_collection;

// simulation.cpp owns the state transitions; March simulation_update asserts
// the world and watcher pointers.
struct s_simulation_globals
{
    bool initialized;
    bool fatal_error;
    bool film_update_pending;
    bool aborted;
    unsigned long abort_time;
    long abort_reason;
    bool skip_next_world_reset;
    bool reset_in_progress;
    bool in_online_networked_session;
    bool loading_saved_game;
    bool recording_film;
    bool saved_film_dispose_pending;
    bool shutdown_requested;
    unsigned char : 8;
    c_simulation_world *world;
    c_simulation_watcher *watcher;
    c_simulation_type_collection *type_collection;
    unsigned long : 32;
    char describe_buffer[256];
};

typedef char simulation_globals_size[(sizeof(void *) != 4 || ((sizeof(s_simulation_globals) == 0x124) )) ? 1 : -1];
typedef char simulation_globals_reset_offset[(offsetof(s_simulation_globals, reset_in_progress) == 0xd) ? 1 : -1];
typedef char simulation_globals_world_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_globals, world) == 0x14) )) ? 1 : -1];
typedef char simulation_globals_watcher_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_globals, watcher) == 0x18) )) ? 1 : -1];
typedef char simulation_globals_types_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_globals, type_collection) == 0x1c) )) ? 1 : -1];
typedef char simulation_globals_description_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_globals, describe_buffer) == 0x24) )) ? 1 : -1];

extern "C" { extern s_simulation_globals g_simulation_globals; }

static __forceinline s_simulation_globals *simulation_globals_get() { return &g_simulation_globals; }

#endif // SIMULATION_SIMULATION_GLOBALS_H
