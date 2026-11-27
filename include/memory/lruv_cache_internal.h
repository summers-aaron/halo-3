#ifndef MEMORY_LRUV_CACHE_INTERNAL_H
#define MEMORY_LRUV_CACHE_INTERNAL_H
#include "multithreading/synchronized_value.h"
#include "memory/lruv_cache_types.h"
#include "memory/data_types.h"
#include "memory/no_allocation.h"
#include "text/font_cache_state.h"

// The cache header and block data header share the allocation returned by lruv_new.
struct s_lruv_cache_allocation
{
    s_lruv_cache cache;
    s_data_array blocks;
};

struct s_lruv_completion
{
    c_synchronized_long done;
    long result_a;
    long result_b;
};

// OVERLAPPED's four words precede the completion pointer (async_helpers.cpp).
struct s_lruv_completion_context
{
    unsigned long internal;
    unsigned long internal_high;
    unsigned long offset;
    unsigned long offset_high;
    s_lruv_completion *completion;
};

struct s_lruv_critical_section_scope
{
    long section;
    bool entered;
};

extern "C" {
void critical_section_scope_construct(s_lruv_critical_section_scope *, long);
}
#endif
