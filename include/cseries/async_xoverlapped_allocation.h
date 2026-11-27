#ifndef CSERIES_ASYNC_XOVERLAPPED_ALLOCATION_H
#define CSERIES_ASYNC_XOVERLAPPED_ALLOCATION_H

#include <stddef.h>
#include "cseries/async_xoverlapped_memory.h"
#include "networking/online/overlapped_allocation_marker_types.h"

__forceinline void *operator new(size_t size, const char *file, long line, e_overlapped_allocation_marker) throw()
{
    return overlapped_malloc_tracked(size, 0, 0);
}

template<class t_task> static __forceinline void overlapped_track_delete(t_task *task)
{
    task->~t_task();
    overlapped_free(task);
}

#endif
