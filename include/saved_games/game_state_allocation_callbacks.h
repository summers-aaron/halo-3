#ifndef SAVED_GAMES_GAME_STATE_ALLOCATION_CALLBACKS_H
#define SAVED_GAMES_GAME_STATE_ALLOCATION_CALLBACKS_H

#include "cache/restricted_memory.h"

// March game_state.obj: the callbacks the game state's restricted regions run on each allocation. The two classes'
// functions are identical (the linker folds them, and keeps the two vtables).
class c_gamestate_deterministic_allocation_callbacks : public c_restricted_memory_callbacks
{
public:
    virtual unsigned int filter_size_request(unsigned int size);
    virtual void handle_allocation(const c_restricted_memory *memory, const char *name, const char *type, long member,
        void *address, unsigned int size);
    virtual void handle_release(const c_restricted_memory *memory, long member, void *address, unsigned int size) {}
};

class c_gamestate_nondeterministic_allocation_callbacks : public c_restricted_memory_callbacks
{
public:
    virtual unsigned int filter_size_request(unsigned int size);
    virtual void handle_allocation(const c_restricted_memory *memory, const char *name, const char *type, long member,
        void *address, unsigned int size);
    virtual void handle_release(const c_restricted_memory *memory, long member, void *address, unsigned int size) {}
};

extern c_gamestate_deterministic_allocation_callbacks g_gamestate_deterministic_allocation_callbacks;
extern c_gamestate_nondeterministic_allocation_callbacks g_gamestate_nondeterministic_allocation_callbacks;

#endif // SAVED_GAMES_GAME_STATE_ALLOCATION_CALLBACKS_H
