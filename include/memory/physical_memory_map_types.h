#ifndef MEMORY_PHYSICAL_MEMORY_MAP_TYPES_H
#define MEMORY_PHYSICAL_MEMORY_MAP_TYPES_H

#include "cseries/pointer_utilities.h"
#include "memory/basic_buffer.h"

struct buffer {
    void *address;
    unsigned long size;
    __forceinline buffer(void *begin, void *end)
        : address(begin), size((char *)end - (char *)begin) {}
};

class c_physical_memory_contiguous_region_listener
{
public:
    virtual void initialize_resize_buffer(c_basic_buffer<void> buffer) = 0;
    virtual void resize_no_fail(c_basic_buffer<void> old_buffer, c_basic_buffer<void> new_buffer) = 0;
    virtual void dispose_resize_buffer(c_basic_buffer<void> buffer) = 0;
};
struct physical_stage {
    unsigned long begin, end;
    char *next_available_zero_allocation;
    unsigned long allocation_count;
    unsigned long fixed_allocation_count;
};
// Layout agrees with the startup initialization in shell_xenon.cpp.
struct physical_globals {
    c_physical_memory_contiguous_region_listener *listener;
    long current_stage;
    unsigned long begin;
    unsigned long end;
    long initialized;
    physical_stage stages[8];
};
extern "C" physical_globals g_physical_memory_globals;

static __forceinline void *physical_memory_map_physical_address(unsigned long address)
{
    return pointer_from_address(address | 0xa0000000);
}

static __forceinline c_basic_buffer<void> physical_memory_current_stage_buffer()
{
    physical_stage *stage = &g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
    char *begin = static_cast<char *>(physical_memory_map_physical_address(stage->begin));
    char *end = static_cast<char *>(physical_memory_map_physical_address(stage->end));
    return c_basic_buffer<void>(begin, end - begin);
}

#endif
