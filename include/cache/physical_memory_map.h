#ifndef CACHE_PHYSICAL_MEMORY_MAP_H
#define CACHE_PHYSICAL_MEMORY_MAP_H

enum memory_stage;
void *_physical_memory_malloc(memory_stage stage, const char *name, long size, unsigned long flags, const char *file, long line);

// Functions defined in tu/blamlibxbox/physical_memory_map.cpp.

extern "C" void physical_memory_resize_region_initialize(void);
extern "C" void physical_memory_pop_stage(long stage);
extern "C" void physical_memory_system_malloc(void);

#endif // CACHE_PHYSICAL_MEMORY_MAP_H
