#ifndef MEMORY_NO_ALLOCATION_H
#define MEMORY_NO_ALLOCATION_H

#include "memory/data.h"

// March cseries.obj: the allocator used for storage which cannot grow or be freed.
class c_no_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation);
};

// Storage for the global no-allocation allocator instance.
extern "C" c_no_allocation g_no_allocation_storage;

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char no_allocation_size[(sizeof(void *) != 4 || (sizeof(c_no_allocation) == 4)) ? 1 : -1];
#endif

#endif // MEMORY_NO_ALLOCATION_H
