#ifndef MEMORY_PHYSICAL_MEMORY_MAP_H
#define MEMORY_PHYSICAL_MEMORY_MAP_H

// Functions defined in physical_memory_map.obj (March 2007 debug map).

enum memory_stage;

void *_physical_memory_malloc(memory_stage stage, const char *name, long size, unsigned long flags, const char *file, long line);

extern "C" void physical_memory_system_malloc(unsigned long size, void *address, unsigned long flags);
extern "C" void physical_memory_pop_stage(long stage);


#endif // MEMORY_PHYSICAL_MEMORY_MAP_H
