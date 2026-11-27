// interfacelibxbox/user_interface_memory.obj: from the records 0x484d90 0x484da0 0x484da8 0x484e50 0x484e90 0x484f88
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "multithreading/synchronization.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_memory_allocation.h"
#include "memory/memory_pool.h"
#include "memory/memory_pool_types.h"
#include "memory/physical_memory_allocation.h"
#include "multithreading/synchronization_types.h"
#include "interface/user_interface_memory_globals.h"
#include "memory/memory_pool_inline.h"
#include "cseries/crt_xdk.h"
extern "C" {

// @address 0x119a53d g_user_interface_memory_allocation_failed
bool g_user_interface_memory_allocation_failed;
// @rdata 0x8204e630..0x8204e640
// @address 0x4e630 k_user_interface_memory_pool_name
const char k_user_interface_memory_pool_name[] = "ui memory pool";
// @rdata-end
// @address 0x8c492c g_user_interface_memory_pool
s_memory_pool *g_user_interface_memory_pool;
// @address 0x8c4930 g_user_interface_memory_pool_size
unsigned int g_user_interface_memory_pool_size;
// @address 0x70d3a4 RtlLeaveCriticalSection
}

// @address 0x7c1b80 ?g_user_interface_memory_allocator@@3Vc_physical_memory_allocation@@A
c_physical_memory_allocation g_user_interface_memory_allocator = c_physical_memory_allocation((memory_stage)4);
namespace {
class c_user_interface_memory_scope
{
public:
    __forceinline c_user_interface_memory_scope() { internal_critical_section_enter(19); }
    __forceinline ~c_user_interface_memory_scope()
    {
        s_critical_section_handle *section = &g_synch_globals.critical_sections[19];
        _InterlockedDecrement(&section->lock_count.value);
        RtlLeaveCriticalSection(&section->critical_section);
    }
};
static __forceinline void user_interface_memory_allocation_failed(unsigned int size, const char *file, long line)
{
    if (!g_user_interface_memory_allocation_failed)
        g_user_interface_memory_allocation_failed = true;
}
}
void *c_user_interface_allocation::allocate(unsigned int size, const char *name)
{
    return user_interface_malloc_tracked(size, 0, 0);
}
void c_user_interface_allocation::deallocate(void *pointer)
{
    user_interface_free(pointer);
}
// @address 0x484da8 user_interface_memory_initialize
extern "C" void user_interface_memory_initialize(e_map_memory_configuration configuration)
{
    bool use_small_pool;
    int pool_size;
    s_memory_pool *pool;
    pool_size = 0xd00000;
    use_small_pool = (configuration == 4) || (configuration == 5) || (configuration == 6);
    if (use_small_pool) {
#ifdef HALO3_64BIT
        pool_size = 0x100000;
#else
        pool_size = 0x80000;
#endif
    }
    pool = memory_pool_new(k_user_interface_memory_pool_name, pool_size, 0, (unsigned int *)&g_user_interface_memory_allocator);
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x484da8: shared base register with g_user_interface_memory_pool_size (one lis/addi, stw -4 and 0 off it)
    ((s_memory_pool **)&g_user_interface_memory_pool_size)[-1] = pool;
#else
    g_user_interface_memory_pool = pool;
#endif
    g_user_interface_memory_pool_size = pool_size + sizeof(s_memory_pool);
    if (pool != 0) {
        pool->reference_tracking_disabled = true;
        pool->allocation_from_anywhere = true;
    }
}
void user_interface_memory_dispose()
{
    if (g_user_interface_memory_pool) {
        memory_pool_dispose(g_user_interface_memory_pool);
        g_user_interface_memory_pool = 0;
    }
}
void *user_interface_malloc_tracked(unsigned int size, const char *file, long line)
{
    c_user_interface_memory_scope lock;
    void *pointer;
    if (!memory_pool_block_allocate(g_user_interface_memory_pool, &pointer, size, file, line) &&
        !memory_pool_block_allocate(g_user_interface_memory_pool, &pointer, size, file, line)) {
        user_interface_memory_allocation_failed(size, file, line);
        pointer = 0;
    }
    return pointer;
}
void user_interface_free(const void *pointer)
{
    c_user_interface_memory_scope lock;
    void **reference = (void **)&pointer;
    memory_pool_block_free(g_user_interface_memory_pool, reference);
}
