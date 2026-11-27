#ifndef BINK_BINK_ALLOCATION_H
#define BINK_BINK_ALLOCATION_H

#include "memory/data.h"

class c_bink_allocation : public c_allocation_base
{
public:
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *address);
};

#endif // BINK_BINK_ALLOCATION_H
