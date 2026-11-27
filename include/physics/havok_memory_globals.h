#ifndef PHYSICS_HAVOK_MEMORY_GLOBALS_H
#define PHYSICS_HAVOK_MEMORY_GLOBALS_H

#include "physics/memory_allocator_types.h"
#include "phys/phys_memory.h"
#include <stddef.h>

enum
{
#ifdef HALO3_64BIT
    k_startup_pool_size = 0x258a0,
    k_runtime_pool_size = 0x385070,
    k_overflow_pool_size = 0xc90,
    k_2nd_overflow_pool_size = 0xc90,
#else
    k_startup_pool_size = 0x25680,
    k_runtime_pool_size = 0x384e10,
    k_overflow_pool_size = 0xa70,
    k_2nd_overflow_pool_size = 0xa70,
#endif
    k_memory_allocator_buffer_size = k_startup_pool_size + k_runtime_pool_size + k_overflow_pool_size + k_2nd_overflow_pool_size,
};

struct s_havok_memory_globals
{
    bool initialized;
    __declspec(align(16)) char memory_allocator_buffer[k_memory_allocator_buffer_size];
    /* Padding after memory_allocator_buffer (20 bytes, never referenced). */
    long pad0, pad1, pad2, pad3, pad4;
    e_memory_allocator_type active_allocator;
    phys_memory *startup_pool;
    phys_memory *runtime_pool;
    phys_memory *overflow_pool;
    phys_memory *second_overflow_pool;
    char *overflow_pool_memory;
    long overflow_pool_memory_size;
    char overflow_pool_default_memory[0xc8000];
};

typedef char havok_globals_startup_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_memory_globals, startup_pool) == 0x3ab998))) ? 1 : -1];
typedef char havok_globals_runtime_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_memory_globals, runtime_pool) == 0x3ab99c))) ? 1 : -1];
typedef char havok_globals_overflow_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_memory_globals, overflow_pool) == 0x3ab9a0))) ? 1 : -1];

#endif
