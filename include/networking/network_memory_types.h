#ifndef NETWORKING_NETWORK_MEMORY_TYPES_H
#define NETWORKING_NETWORK_MEMORY_TYPES_H

#include "networking/network_memory.h"
#include <stddef.h>
#include "cseries/crt_xdk.h"

#include "memory/rockall_front_end.h"

// March rockall_heap.cpp:339 and get_allocation_size (82b49d18) establish
// the frontend pointer, allocation count, locking flag, and Details slot.
class c_rockall_heap
{
public:
    ~c_rockall_heap()
    {
        if (m_rockall_front_end) {
            delete m_rockall_front_end;
            m_rockall_front_end = 0;
        }
    }
    void operator delete(void *pointer) { RtlFreeHeap(GetProcessHeap(), 0, pointer); }
    void *allocate(long size);
    void deallocate(void *allocation);
    __forceinline long get_allocation_size(void *allocation)
    {
        int size;
        if (!m_rockall_front_end->Details(allocation, &size))
            size = -1;
        return size;
    }

    ROCKALL_FRONT_END *m_rockall_front_end;
    long m_allocation_count;
    bool m_locked;
};

struct s_data_array;
class c_simulation_distributed_world;

// Shared network memory usage counters, views, and heap.
// The leading word is unused by the current owner; its meaning is unresolved.
// March agrees on the heap at +0x24, but keeps its debug allocation status
// after this record; that status is not stored here.
struct s_network_shared_memory_globals
{
    unsigned long reserved_0000;
    long total;
    void *low;
    void *high;
    bool initialized;
    c_simulation_distributed_world *distributed_world;
    s_data_array *views;
    s_data_array *distributed_views;
    long heap_size;
    c_rockall_heap *heap;
    void *heap_memory;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char rockall_heap_size[(sizeof(void *) != 4 || sizeof(c_rockall_heap) == 0xc) ? 1 : -1];
typedef char network_shared_memory_globals_heap_offset[(sizeof(void *) != 4 || offsetof(s_network_shared_memory_globals, heap) == 0x24) ? 1 : -1];
typedef char network_shared_memory_globals_size[(sizeof(void *) != 4 || sizeof(s_network_shared_memory_globals) == 0x2c) ? 1 : -1];
#endif

#endif // NETWORKING_NETWORK_MEMORY_TYPES_H
