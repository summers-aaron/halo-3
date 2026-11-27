#ifndef MEMORY_PHYSICAL_MEMORY_ALLOCATION_H
#define MEMORY_PHYSICAL_MEMORY_ALLOCATION_H

#include "memory/data.h"

enum memory_stage;

// Allocator that hands out physical memory from one memory stage (release layout: the
// March 2007 debug build also keeps the allocation name after m_stage).
class c_physical_memory_allocation : public c_allocation_base
{
public:
    c_physical_memory_allocation(memory_stage stage) : m_stage(stage) {}
    virtual void *allocate(unsigned int size, const char *name);
    // Release frees nothing: allocations live until their memory stage is popped.
    virtual void deallocate(void *allocation) {}

private:
    long m_stage;
};

typedef char physical_memory_allocation_size[(sizeof(void *) != 4 || ((sizeof(c_physical_memory_allocation) == 8))) ? 1 : -1];

#endif // MEMORY_PHYSICAL_MEMORY_ALLOCATION_H
