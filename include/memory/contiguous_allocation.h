#ifndef MEMORY_CONTIGUOUS_ALLOCATION_H
#define MEMORY_CONTIGUOUS_ALLOCATION_H

class c_allocation_base;
template<typename T> class c_wrapped_array;
struct s_allocation_description;
struct s_allocation_description_holder;

bool allocate_list_contiguous(c_allocation_base *allocator, c_wrapped_array<s_allocation_description> list,
    const char *name, void **out_buffer, unsigned long *out_size);

bool allocate_list_contiguous(c_allocation_base *allocator, c_wrapped_array<s_allocation_description_holder> holders,
    const char *name, void **out_base, unsigned long *out_size);

#include "memory/no_allocation.h"

#endif // MEMORY_CONTIGUOUS_ALLOCATION_H
