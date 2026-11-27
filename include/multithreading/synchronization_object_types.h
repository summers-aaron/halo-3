#ifndef MULTITHREADING_SYNCHRONIZATION_OBJECT_TYPES_H
#define MULTITHREADING_SYNCHRONIZATION_OBJECT_TYPES_H

#include "multithreading/synchronization_types.h"
#include "multithreading/synchronized_value.h"

struct s_synchronization_object {
    long owner_thread;
    long lock_count;
    RTL_CRITICAL_SECTION critical_section;
    __forceinline void decrement_lock_count() { _InterlockedDecrement(&lock_count); }
};
struct s_critical_section_table {
    long count;
    s_synchronization_object objects[1];
};

typedef char synchronization_object_size[(sizeof(void *) != 4 || ((sizeof(s_synchronization_object) == 0x24))) ? 1 : -1];

#endif
