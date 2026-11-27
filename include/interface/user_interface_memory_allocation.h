#ifndef INTERFACE_USER_INTERFACE_MEMORY_ALLOCATION_H
#define INTERFACE_USER_INTERFACE_MEMORY_ALLOCATION_H

#include "memory/data.h"

// The allocator user-interface data arrays are made with (interface\user_interface_memory.cpp in the March 2007
// build): allocate and deallocate go to user_interface_malloc_tracked and user_interface_free. The single
// instance holds only its vtable (0x8204e640).
class c_user_interface_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *pointer);
};

// @address 0x7c1b88 g_user_interface_allocation
extern "C" c_user_interface_allocation g_user_interface_allocation;

// Returns the global allocator used for user-interface data.
__forceinline c_allocation_base *get_user_interface_allocation()
{
    return &g_user_interface_allocation;
}

#endif // INTERFACE_USER_INTERFACE_MEMORY_ALLOCATION_H
