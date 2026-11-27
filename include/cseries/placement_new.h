#ifndef CSERIES_PLACEMENT_NEW_H
#define CSERIES_PLACEMENT_NEW_H
#include <stddef.h>

inline void *operator new(size_t size, void *memory)
{
    return memory;
}

#endif
