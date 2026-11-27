#ifndef MEMORY_NORMAL_ALLOCATION_H
#define MEMORY_NORMAL_ALLOCATION_H

#include "memory/data.h"

// March cseries.obj: the general-purpose allocator (March c_normal_allocation::allocate calls debug_malloc).
class c_normal_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation);
};

// Storage for the global general-purpose allocator.

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char normal_allocation_size[(sizeof(void *) != 4 || (sizeof(c_normal_allocation) == 4)) ? 1 : -1];
#endif

#endif // MEMORY_NORMAL_ALLOCATION_H
