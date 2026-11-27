#ifndef CSERIES_ASYNC_XOVERLAPPED_MEMORY_H
#define CSERIES_ASYNC_XOVERLAPPED_MEMORY_H

#include <stddef.h>

#include "memory/physical_memory_allocation.h"
#include "saved_games/game_state_procs_types.h"
#include "memory/data.h"

// Functions defined in tu/blamlibxbox/async_xoverlapped_memory.cpp that other units call.

void *overlapped_malloc_tracked(unsigned int allocation_size, const char *file_name, long line_number);
void overlapped_free(const void *block_address);

extern "C" char k_overlapped_memory_pool_name[];

struct s_memory_pool;

class c_overlapped_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation);
};

// The game-state proc handler that game_state_procs installs as handlers[0] (vftable 0x82040d30).
struct s_async_xoverlapped_memory : public c_game_state_proc_handler
{
    s_async_xoverlapped_memory(memory_stage stage) : allocator(stage), pool(NULL) {}
    virtual void set_mode(long mode, s_game_state_buffer_description *descriptions);
    virtual void reset();
    virtual void *game_state_proc_handler_allocate(
        long index, long allocation_parameter, long allocation_size, c_game_state_proc_listener *listener);
    virtual void game_state_proc_handler_release(long index, void *buffer);

    c_physical_memory_allocation allocator;
    s_memory_pool *pool;
};

typedef char async_xoverlapped_memory_allocator_offset[
    (sizeof(void *) != 4 || ((offsetof(s_async_xoverlapped_memory, allocator) == 0x4))) ? 1 : -1];
typedef char async_xoverlapped_memory_pool_offset[(sizeof(void *) != 4 || ((offsetof(s_async_xoverlapped_memory, pool) == 0xc))) ? 1 : -1];
typedef char async_xoverlapped_memory_size[(sizeof(void *) != 4 || ((sizeof(s_async_xoverlapped_memory) == 0x10))) ? 1 : -1];

extern "C" s_memory_pool *g_overlapped_memory_pool;
extern c_overlapped_allocation g_overlapped_allocation;

#endif // CSERIES_ASYNC_XOVERLAPPED_MEMORY_H
