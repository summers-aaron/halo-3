// binkxenon/popmal.obj: from the records 0x712240 0x712250
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "bink/radmem.h"
#include "bink/popmal.h"
// @address 0x11df2ac g_popmalloc_state
extern "C" unsigned int popmalloctotal(void)
{
    return g_popmalloc_state.total;
}
extern "C" void *popmalloc(
    int size, int allocator_arg_2, int allocator_arg_3, int allocator_arg_4, int allocator_arg_5, int allocator_arg_6)
{
    int aligned_size = (size + 31) & ~31;
    void *allocation = radmalloc(
        g_popmalloc_state.total + aligned_size, allocator_arg_2, allocator_arg_3, allocator_arg_4, allocator_arg_5, allocator_arg_6);
    g_popmalloc_state.total = 0;
    if (allocation) {
        unsigned int count = g_popmalloc_state.count;
        char *next_allocation = (char *)allocation + aligned_size;
        if (count) {
            unsigned int *sizes = g_popmalloc_state.sizes;
            void ***destinations = g_popmalloc_state.destinations;
            for (unsigned int i = 0; i < count; ++i) {
                *destinations[i] = next_allocation;
                next_allocation += sizes[i];
            }
        }
    }
    g_popmalloc_state.count = 0;
    return allocation;
}
