#ifndef CACHE_TAG_RESOURCE_CACHE_THREAD_LOCK_EXTERNS_H
#define CACHE_TAG_RESOURCE_CACHE_THREAD_LOCK_EXTERNS_H

#include "memory/wrapped_arrays.h"
#include "memory/allocation_description.h"

#include "memory/contiguous_allocation.h"

class c_allocation_base;
class c_allocation_description_holder;
struct s_holder_array
{
    int count;
    c_allocation_description_holder **elements;
};

#endif // CACHE_TAG_RESOURCE_CACHE_THREAD_LOCK_EXTERNS_H
