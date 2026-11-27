// blamlibxbox/async_xoverlapped_memory.obj: from the records 0x2859d0 0x2859e0 0x2859e8 0x285aa0 0x285b20 0x285bb8 0x285bf0 0x285c58 0x2f77d8
// @flags /TP /O2
// @address 0x285aa0 ?overlapped_free@@YAXPBX@Z
// @address 0x2f77d8 async_xoverlapped_compute_cache_sizes
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "memory/memory_pool_types.h"
// @address 0x70d3a4 RtlLeaveCriticalSection
// @address 0x8c4274 g_overlapped_memory_pool
#include "cseries/async_xoverlapped_memory.h"
#include "memory/memory_pool.h"
#include "multithreading/synchronized_value.h"
#include "interface/user_interface_memory_globals.h"

struct s_memory_pool;


struct s_synchronization_globals;


struct cache_size_requirements
{
    unsigned int size_a;
    unsigned int size_b;
    unsigned int size_c;
    unsigned int size_d;
};



// @address 0x793964 ?g_overlapped_allocation@@3Vc_overlapped_allocation@@A
c_overlapped_allocation g_overlapped_allocation;

// @address 0x2859d0 ?allocate@c_overlapped_allocation@@UAAPAXIPBD@Z
void *c_overlapped_allocation::allocate(unsigned int size, const char *name)
{
    return overlapped_malloc_tracked(size, 0, 0);
}

// @address 0x2859e0 ?deallocate@c_overlapped_allocation@@UAAXPAX@Z
void c_overlapped_allocation::deallocate(void *allocation)
{
    overlapped_free(allocation);
    return;
}

void *overlapped_malloc_tracked(unsigned int allocation_size, const char *file_name, long line_number)
{
    s_memory_pool *overlapped_memory_pool;
    int block_handle;
    unsigned char allocation_succeeded;
    void *allocated_block;
    void *allocation_address_storage[2];
    internal_critical_section_enter(25);
    overlapped_memory_pool = g_overlapped_memory_pool;
    allocation_succeeded = 0;
    block_handle = memory_pool_block_allocate_handle(g_overlapped_memory_pool, 0, allocation_size, 0, 0);
    if (!((block_handle == 0))) {
        allocation_succeeded = 1;
        allocated_block = (char *)overlapped_memory_pool + block_handle;
    } else {
        allocated_block = allocation_address_storage[0];
    }
    if (!((((unsigned char)allocation_succeeded) != 0))) {
        allocated_block = 0;
    }
    _InterlockedDecrement(&g_synch_globals.critical_sections[25].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[25].critical_section);
    return allocated_block;
}

void overlapped_free(const void *block_address)
{
    s_memory_pool *overlapped_pool;
    int block_offset;
    internal_critical_section_enter(25);
    overlapped_pool = g_overlapped_memory_pool;
    block_offset = (int)((const char *)block_address - (const char *)overlapped_pool);
    if (block_address == 0)
    {
        block_offset = 0;
    }
    memory_pool_block_free(overlapped_pool, block_offset);
    _InterlockedDecrement(&g_synch_globals.critical_sections[25].lock_count.value);
    RtlLeaveCriticalSection(&g_synch_globals.critical_sections[25].critical_section);
    return;
}

// @address 0x40d30 ??_7s_async_xoverlapped_memory@@6B@

// @address 0x285bb8 ?reset@s_async_xoverlapped_memory@@UAAXXZ
void s_async_xoverlapped_memory::reset()
{
    ::memory_pool_dispose(pool);
    pool = NULL;
}

// @address 0x285bf0 ?game_state_proc_handler_allocate@s_async_xoverlapped_memory@@UAAPAXJJJPAUc_game_state_proc_listener@@@Z
void *s_async_xoverlapped_memory::game_state_proc_handler_allocate(
    long index, long allocation_parameter, long allocation_size, c_game_state_proc_listener *listener)
{
    void *result = NULL;
    s_memory_pool *memory_pool = pool;

    if (memory_pool != NULL)
    {
        int handle = memory_pool_block_allocate_handle(memory_pool, 0, allocation_size, 0, 0);
        if (handle != 0)
        {
            result = (char *)memory_pool + handle;
        }
    }
    return result;
}

// @address 0x285c58 ?game_state_proc_handler_release@s_async_xoverlapped_memory@@UAAXJPAX@Z
void s_async_xoverlapped_memory::game_state_proc_handler_release(long index, void *buffer)
{
    s_memory_pool *memory_pool = pool;
    long offset = (long)((const char *)buffer - (const char *)memory_pool);

    if (buffer == NULL)
    {
        offset = 0;
    }
    memory_pool_block_free(memory_pool, offset);
}

// 0x2f77d8
extern "C" void async_xoverlapped_compute_cache_sizes(
    const s_game_state_buffer_description *descriptions, cache_size_requirements *requirements)
{
    unsigned int maximums[3];
    unsigned int totals[3];

    for (int i = 0; i < 3; i++)
    {
        maximums[i] = 0;
        totals[i] = 0;
    }
    for (int i = 0; i < 6; i++)
    {
        const s_game_state_buffer_description *entry = &descriptions[i];
        maximums[entry->type] = maximums[entry->type] > entry->size ? maximums[entry->type] : entry->size;
        totals[entry->type] = entry->size + totals[entry->type];
    }

    requirements->size_a = maximums[2] > maximums[1] ? maximums[2] : maximums[1];
    requirements->size_b = totals[2] + totals[1];
    requirements->size_c = maximums[0];
    requirements->size_d = totals[0];
}

// @address 0x285b20 ?set_mode@s_async_xoverlapped_memory@@UAAXJPAUs_game_state_buffer_description@@@Z
void s_async_xoverlapped_memory::set_mode(long mode, s_game_state_buffer_description *descriptions)
{
    cache_size_requirements requirements;
    unsigned int size;

    async_xoverlapped_compute_cache_sizes(descriptions, &requirements);
    if (mode == 1 || mode == 6)
    {
        size = requirements.size_b + requirements.size_d;
    }
    else
    {
        size = requirements.size_a;
        if (requirements.size_a <= requirements.size_c)
        {
            size = requirements.size_c;
        }
    }
    pool = memory_pool_new(
        "fallback optional cache memory pool", size + 0x400, 0, (unsigned int *)&allocator);
    pool->allocation_from_anywhere = true;
}
