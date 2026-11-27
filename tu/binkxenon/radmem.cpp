// binkxenon/radmem.obj: from the records 0x6e2548 0x6e2560 0x6e2628
// @flags /TP /O2 /Oc

#include <stdlib.h>
#include "bink/radmem.h"
// @address 0x7cefc0 g_radmem_hooks
s_rad_memory_hooks g_radmem_hooks;

namespace {
enum e_rad_allocation_mode
{
    _rad_allocation_crt = 0,
    _rad_allocation_custom = 3
};

struct s_rad_allocation_header {
    void (*deallocator)(void *);
    unsigned short reserved;
    unsigned char allocation_mode;
    unsigned char alignment_offset;
};
}

// @address 0x6e2548 RADSetMemory
extern "C" void RADSetMemory(void *(*allocator)(unsigned int), void (*deallocator)(void *))
{
    g_radmem_hooks.allocate = allocator;
    g_radmem_hooks.deallocate = deallocator;
}

// @address 0x6e2560 radmalloc
void *radmalloc(int size, int, int, int, int, int)
{
    unsigned char allocationMode = _rad_allocation_crt;
    char *allocationAddress = 0;
    if ((unsigned int)size == 0 || size == -1) return 0;
    if (g_radmem_hooks.allocate != 0) {
        allocationAddress = (char *)g_radmem_hooks.allocate(size + 64);
        if (allocationAddress != 0) {
            if (allocationAddress == (char *)-1) return 0;
            allocationMode = _rad_allocation_custom;
        }
    }
    if (allocationAddress == 0) {
        allocationAddress = (char *)malloc(size + 64);
        if (allocationAddress == 0) return 0;
        allocationMode = _rad_allocation_crt;
    }
    char *alignedMemory = (char *)allocationAddress;
    unsigned char alignmentOffset = (unsigned char)(64 - ((size_t)allocationAddress & 31));
    alignedMemory += alignmentOffset;
#ifdef HALO3_64BIT
    s_rad_allocation_header *header = (s_rad_allocation_header *)alignedMemory - 1;
    header->alignment_offset = alignmentOffset;
    header->allocation_mode = allocationMode;
    if (allocationMode == 3) header->deallocator = g_radmem_hooks.deallocate;
#else
    alignedMemory[-1] = alignmentOffset;
    alignedMemory[-2] = allocationMode;
    if (allocationMode == 3) ((s_rad_allocation_header *)alignedMemory - 1)->deallocator = g_radmem_hooks.deallocate;
#endif
    return alignedMemory;
}

// @address 0x6e2628 radfree
extern "C" void radfree(void *memory)
{
    if (memory) {
        s_rad_allocation_header *header = (s_rad_allocation_header *)memory - 1;
        if (header->allocation_mode == 3)
            header->deallocator((char *)memory - header->alignment_offset);
        else
            free((char *)memory - header->alignment_offset);
    }
}
