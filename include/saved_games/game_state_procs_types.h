#ifndef SAVED_GAMES_GAME_STATE_PROCS_TYPES_H
#define SAVED_GAMES_GAME_STATE_PROCS_TYPES_H

#include <stddef.h>

// Layout shared with the owning game_state_procs.cpp.

// The buffer one listener asks for in a memory configuration (type 0 = none).
struct s_game_state_buffer_description
{
    long type;
    unsigned long size;
};

struct c_game_state_proc_listener
{
    virtual void set_mode(long mode, s_game_state_buffer_description *description) = 0;
    virtual void update(void *) = 0;
    virtual void dispose(void *) = 0;
};

struct c_game_state_proc_handler
{
    virtual void set_mode(long mode, s_game_state_buffer_description *descriptions) = 0;
    virtual void reset() = 0;
    virtual void *game_state_proc_handler_allocate(long index, long p1, long p2, c_game_state_proc_listener *listener) = 0;
    virtual void game_state_proc_handler_release(long index, void *buffer) = 0;
};

struct s_game_state_proc_entry
{
    c_game_state_proc_listener *handler;
    void *buffer;
};

struct s_game_state_procs_globals
{
    c_game_state_proc_handler *handlers[2];
    c_game_state_proc_handler *volatile current;
    s_game_state_proc_entry entries[6];
    unsigned long active[1];
};

typedef char game_state_buffer_description_size[(sizeof(s_game_state_buffer_description) == 8) ? 1 : -1];
typedef char game_state_proc_entry_size[(sizeof(void *) != 4 || ((sizeof(s_game_state_proc_entry) == 8))) ? 1 : -1];
typedef char game_state_procs_entries_offset[(sizeof(void *) != 4 || ((offsetof(s_game_state_procs_globals, entries) == 0xc))) ? 1 : -1];
typedef char game_state_procs_globals_size[(sizeof(void *) != 4 || ((sizeof(s_game_state_procs_globals) == 0x40))) ? 1 : -1];

#endif // SAVED_GAMES_GAME_STATE_PROCS_TYPES_H
