#ifndef BINK_RADMEM_H
#define BINK_RADMEM_H

// The RAD (Bink) allocator, defined in tu/binkxenon/radmem.cpp.

struct s_rad_memory_hooks
{
    void *(*allocate)(unsigned int size);
    void (*deallocate)(void *memory);
};

extern "C" {
extern s_rad_memory_hooks g_radmem_hooks;
}

extern "C" void RADSetMemory(void *(*allocator)(unsigned int), void (*deallocator)(void *));
extern "C" void *radmalloc(int size, int, int, int, int, int);
extern "C" void radfree(void *memory);

#endif // BINK_RADMEM_H
