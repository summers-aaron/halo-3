// blamlibxbox/blam_zlib_memory.obj: from the records 0x286540 0x286590
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "zlib/blam_zlib_memory.h"
#include "memory/memory_pool_inline.h"
struct s_memory_pool;
namespace {
struct s_zlib_memory { s_memory_pool *heap; };
}
void *blam_zlib_malloc(void *context, unsigned int items, unsigned int size)
{
    s_zlib_memory *zlib_memory = (s_zlib_memory *)context;
    void *pointer;
    bool success = memory_pool_block_allocate(zlib_memory->heap, &pointer, items * size);
    return pointer;
}
void blam_zlib_free(void *context, void *pointer)
{
    s_zlib_memory *zlib_memory = (s_zlib_memory *)context;
    memory_pool_block_free(zlib_memory->heap, &pointer);
}
