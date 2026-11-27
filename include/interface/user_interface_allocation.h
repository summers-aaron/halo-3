#ifndef INTERFACE_USER_INTERFACE_ALLOCATION_H
#define INTERFACE_USER_INTERFACE_ALLOCATION_H

// The tracked placement form of new/delete that the interface code allocates widgets, datasources and tag
// parsers with (`new (__FILE__, __LINE__, _ui_allocation_marker_dummy) T`).
// The marker only selects this overload. The matching delete is what the compiler calls when a constructor
// run from this new throws; it does nothing.

#include <stddef.h>
#include "interface/user_interface_memory.h"
#include "interface/ui_allocation_marker_types.h"

__forceinline void *operator new(size_t size, const char *file, long line, e_ui_allocation_marker marker)
{
    return user_interface_malloc_tracked(size, file, line);
}

inline void operator delete(void *pointer, const char *file, long line, e_ui_allocation_marker marker) {}

#endif // INTERFACE_USER_INTERFACE_ALLOCATION_H
