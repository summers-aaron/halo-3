#ifndef MEMORY_RUNTIME_STATE_ALLOCATION_H
#define MEMORY_RUNTIME_STATE_ALLOCATION_H

#include "memory/data.h"

// The allocator for storage carved from the runtime state buffer: allocations are never freed. Not in the March
// build; runtime_state.obj has its vtable (0x8203aeb0) and its allocate (0x8219c820).
class c_runtime_state_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation) {}
};

extern c_runtime_state_allocation g_runtime_state_allocation;

#endif // MEMORY_RUNTIME_STATE_ALLOCATION_H
