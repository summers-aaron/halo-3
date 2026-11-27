// blamlibxbox/physical_memory_map.obj: from the records 0x1ac768 0x1ac878 0x1ac948 0x1acad0
// @flags /TP /O2 /Oc
// @address 0x1ac878 physical_memory_pop_stage
// @address 0x1ac948 ?_physical_memory_malloc@@YAPAXW4memory_stage@@PBDJK1J@Z
// @address 0x1acad0 physical_memory_system_malloc
#include "cache/physical_memory_map.h"
#include "memory/physical_memory_map_types.h"
// @address 0x7daf18 g_physical_memory_globals
physical_globals g_physical_memory_globals;

#include "cseries/xdk_memory.h"
// @address 0x589d30 XPhysicalProtect
// @address 0x589ca0 XPhysicalAlloc
// @address 0x59e548 XMemGetPageSize
static __forceinline physical_stage *physical_memory_get_current_stage() {
    return &g_physical_memory_globals.stages[g_physical_memory_globals.current_stage];
}
static __forceinline void physical_memory_push_stage() {
    long old_stage = g_physical_memory_globals.current_stage;
    long next_stage = old_stage + 1;
    g_physical_memory_globals.stages[next_stage] = g_physical_memory_globals.stages[old_stage];
    g_physical_memory_globals.current_stage = next_stage;
}

// @address 0x1ac768 physical_memory_resize_region_initialize
void physical_memory_resize_region_initialize(void) {
    physical_memory_push_stage();
    if (g_physical_memory_globals.listener)
        g_physical_memory_globals.listener->initialize_resize_buffer(physical_memory_current_stage_buffer());
    physical_memory_push_stage();
}

void physical_memory_pop_stage(long stage) {
    c_basic_buffer<void> old_buffer = physical_memory_current_stage_buffer();
    --g_physical_memory_globals.current_stage;
    if (stage != 6) {
        c_basic_buffer<void> new_buffer = physical_memory_current_stage_buffer();
        if ((unsigned long)(g_physical_memory_globals.current_stage - 3) <= 2) {
            if (g_physical_memory_globals.listener) {
                g_physical_memory_globals.listener->resize_no_fail(old_buffer, new_buffer);
            }
        }
    }
}

void * _physical_memory_malloc(enum memory_stage stage, char const * name, long size, unsigned long flags, char const * file, long line) {
    void *result = 0;
    physical_stage *current_stage = physical_memory_get_current_stage();
    unsigned long allocation_size = (size + 65535) & 0xffff0000;
    if (allocation_size > 0) {
        c_basic_buffer<void> old_buffer = physical_memory_current_stage_buffer();
        unsigned long begin = current_stage->begin;
        unsigned long end = current_stage->end;
        unsigned long physical_address = 0;
        unsigned char from_beginning = ((~flags >> 2) & 1);
        if (from_beginning) {
            unsigned long next = begin + allocation_size;
            if (next <= end) {
                current_stage->begin = next;
                physical_address = begin;
            }
        } else {
            unsigned long next = end - allocation_size;
            if (next >= begin) {
                current_stage->end = next;
                physical_address = next;
            }
        }
        if (physical_address) result = physical_memory_map_physical_address(physical_address);
        if (result) {
            c_basic_buffer<void> new_buffer = physical_memory_current_stage_buffer();
            if ((unsigned long)(g_physical_memory_globals.current_stage - 3) <= 2) {
                if (g_physical_memory_globals.listener) {
                    g_physical_memory_globals.listener->resize_no_fail(old_buffer, new_buffer);
                }
            }
            XPhysicalProtect(result, allocation_size, 4);
        }
    } else {
        result = current_stage->next_available_zero_allocation++;
    }
    if (result) {
        ++current_stage->allocation_count;
        if (flags & 4) ++current_stage->fixed_allocation_count;
    }
    return result;
}

void physical_memory_system_malloc(void)

{
  unsigned long page_size;
  unsigned long flags;

  page_size = XMemGetPageSize((const void *)0xa5470000);
  if (page_size == 0x1000) {
    flags = 0;
  } else if (page_size == 0x10000) {
    flags = 0x20000000;
  } else if (page_size == 0x1000000) {
    flags = 0x80000000;
  } else {
    flags = 0;
  }
  XPhysicalAlloc(0x1a790000,0x5470000,0,flags | 4);
  return;
}
