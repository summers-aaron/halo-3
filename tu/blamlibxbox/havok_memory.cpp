// blamlibxbox/havok_memory.obj: from the records 0xcc4b8 0x1034f0 0x1035f0 0x1036d0 0x1037d0 0x1038c0 0x1045b0 0x25e5b0 0x25e680 0x25e758 0x25e8d8 0x25e978 0x25ebc8 0x25ec30 0x25ed00 0x25ee18 0x25ee40 0x25ee68 0x25ee90 0x2e0678
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x25ee68 ?havok_thread_memory_deallocate_chunk@@YAXPAXHW4phys_memory_class@@@Z
// @address 0x25ee90 ?havok_thread_memory_allocate_chunk@@YAPAXHW4phys_memory_class@@@Z
#include "phys/phys_base_types.h"
#include <string.h>
#include <stddef.h>
#include "physics/havok_memory.h"
#include "physics/havok_memory_task.h"
#include "physics/havok_memory_listeners.h"
#include "physics/havok.h"
// @address 0x25ebc8 ??_Gc_havok_memory@@UAAPAXI@Z
// @address 0x1038c0 ?allocate_chunk_batch@c_havok_memory@@UAAXPAPAXHH@Z
// @address 0x104420 ?free_chunk_batch@c_havok_memory@@UAAXPAPAXHH@Z

#include "phys/phys_thread_memory.h"
#include "physics/memory_allocator_types.h"
#include "scenario/scenario.h"
#include "physics/havok_component.h"
class phys_out_stream;
struct phys_memory_stats;
#include "phys/phys_memory.h"
#include "physics/havok_memory_class.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "physics/havok_memory_allocator_lock.h"

#include "physics/havok_memory_pool.h"

enum
{
    k_startup_pool_memory_size = 0x24c00,
    k_overflow_pool_default_memory_size = 0xc8000,
};

class c_startup_pool : public c_havok_memory_pool
{
public:
    __forceinline c_startup_pool() :
        c_havok_memory_pool(reinterpret_cast<char *>(this + 1), k_startup_pool_memory_size, 0)
    {
    }
    __forceinline bool contains(void *block)
    {
        return (char *)block >= (char *)this + sizeof(c_startup_pool) &&
            (char *)block <= (char *)this + sizeof(c_startup_pool) + k_startup_pool_memory_size;
    }
};
class c_overflow_pool : public c_havok_memory_pool
{
public:
    __forceinline c_overflow_pool(char *memory, long size) :
        c_havok_memory_pool(memory, size, 0)
    {
    }
};

#include "physics/havok_runtime_pool_class.h"

#include "physics/havok_memory_globals.h"
// @address 0xa430e0 g_havok_memory_globals
s_havok_memory_globals g_havok_memory_globals;

#include "physics/havok_memory_usage.h"

// @address 0x8c4259 g_havok_memory_always_system
bool g_havok_memory_always_system;

namespace {
static __forceinline e_memory_allocator_type havok_get_active_memory_allocator()
{
    return g_havok_memory_globals.active_allocator;
}
static __forceinline phys_memory *havok_get_memory_allocator_class()
{
    return &g_havok_memory;
}
static __forceinline bool havok_memory_is_runtime_pool_memory_address(void *block, phys_memory *memory)
{
    return static_cast<c_runtime_pool *>(memory)->contains(block);
}
static __forceinline bool havok_memory_is_startup_pool_memory_address(void *block, phys_memory *memory)
{
    return static_cast<c_startup_pool *>(memory)->contains(block);
}
static __forceinline bool havok_memory_is_overflow_pool_memory_address(void *block, phys_memory *memory)
{
    return static_cast<c_havok_memory_pool *>(memory)->contains(block);
}
// There is no second overflow pool: report the allocation stats and return null
static __forceinline phys_memory *havok_get_2nd_overflow_pool()
{
    havok_display_stats(true);
    return 0;
}
static __forceinline phys_memory *havok_memory_initialize_startup_pool(char *buffer)
{
    phys_memory *memory = new (buffer) c_startup_pool();
    return memory;
}
static __forceinline phys_memory *havok_memory_initialize_runtime_pool(char *buffer)
{
    phys_memory *memory = new (buffer) c_runtime_pool();
    return memory;
}
static __forceinline phys_memory *havok_memory_initialize_overflow_pool(char *buffer, char *memory, long size)
{
    phys_memory *overflow_pool = new (buffer) c_overflow_pool(memory, size);
    return overflow_pool;
}
}

// @address 0xcc4b8 havok_memory_dispose_overflow_pool
extern "C" void havok_memory_dispose_overflow_pool(void)
{
    if (g_havok_memory_globals.overflow_pool)
    {
        g_havok_memory_globals.overflow_pool->~phys_memory();
        g_havok_memory_globals.overflow_pool_memory = 0;
        g_havok_memory_globals.overflow_pool = 0;
    }
}

// @address 0x1036d0 havok_memory_is_runtime_pool_full
extern "C" bool havok_memory_is_runtime_pool_full(const c_runtime_pool *runtime_pool)
{
    bool result = false;
    if (runtime_pool)
        result = (float)runtime_pool->get_pages_in_use() / 450.0f > 0.89f;
    return result;
}

// @address 0x1034f0 havok_get_memory_condition_type
extern "C" int havok_get_memory_condition_type(int)
{
    c_havok_memory_pool *backup_overflow_pool;
    c_havok_memory_pool *primary_overflow_pool;
    phys_memory *active_runtime_pool;
    float runtime_fraction;
    int second_overflow_condition;
    int overflow_condition;
    int runtime_condition;
    int havok_memory_condition_type;
    int worst_condition;
    second_overflow_condition = 0;
    backup_overflow_pool = static_cast<c_havok_memory_pool *>(g_havok_memory_globals.second_overflow_pool);
    if (backup_overflow_pool != 0 && backup_overflow_pool->m_current_allocation_count > 0)
        second_overflow_condition = 3;
    overflow_condition = 0;
    primary_overflow_pool = static_cast<c_havok_memory_pool *>(g_havok_memory_globals.overflow_pool);
    if (primary_overflow_pool != 0 && primary_overflow_pool->m_current_allocation_count > 0)
        overflow_condition = 3;
    active_runtime_pool = g_havok_memory_globals.runtime_pool;
    if (active_runtime_pool != 0) {
        runtime_condition = 3;
        runtime_fraction = havok_memory_runtime_pool_memory_fraction((phys_memory *)active_runtime_pool);
        if (runtime_fraction < 0.82f) {
            runtime_condition = 2;
            if (runtime_fraction < 0.78f) {
                runtime_condition = 1;
                if (runtime_fraction < 0.74f) runtime_condition = 0;
            }
        }
        havok_memory_condition_type = runtime_condition;
    } else {
        havok_memory_condition_type = 0;
    }
    worst_condition = havok_memory_condition_type > second_overflow_condition ? havok_memory_condition_type : second_overflow_condition;
    if (!((overflow_condition <= worst_condition))) {
        return overflow_condition;
    }
    if (!((havok_memory_condition_type > second_overflow_condition))) {
        havok_memory_condition_type = second_overflow_condition;
    }
    return havok_memory_condition_type;
}

bool havok_memory_has_oversize_allocations(void)
{
    bool result = false;
    if (g_havok_memory_globals.second_overflow_pool)
        result |= static_cast<c_havok_memory_pool *>(g_havok_memory_globals.second_overflow_pool)->calculate_maximum_allocation_size() > 8192;
    if (g_havok_memory_globals.overflow_pool)
        result |= static_cast<c_havok_memory_pool *>(g_havok_memory_globals.overflow_pool)->calculate_maximum_allocation_size() > 8192;
    return result;
}
// @address 0x42d298 ?havok_override_thread_memory_stack_allocations@@YA_NXZ
bool havok_override_thread_memory_stack_allocations()
{
    return false;
}

bool havok_override_thread_memory_allocations(void)
{
    return havok_get_active_memory_allocator() == _memory_allocator_havok;
}

void c_havok_memory::allocate_chunk_batch(void **blocks, int count, int size)
{
    if (count > 0 && size > 0)
    {
        if (havok_get_active_memory_allocator() != _memory_allocator_havok)
        {
            g_havok_memory_globals.runtime_pool->allocate_chunk_batch(blocks, count, size);
            if (!blocks[0])
            {
                g_havok_memory_globals.overflow_pool->allocate_chunk_batch(blocks, count, size);
                if (!blocks[0])
                    havok_get_2nd_overflow_pool()->allocate_chunk_batch(blocks, count, size);
            }
        }
        else
        {
            g_havok_memory_globals.startup_pool->allocate_chunk_batch(blocks, count, size);
        }
    }
}

// @address 0x1045b0 ?havok_get_available_memory@@YAHXZ
int havok_get_available_memory(void)
{
    return !havok_memory_is_runtime_pool_full(static_cast<c_runtime_pool *>(g_havok_memory_globals.runtime_pool)) ? 0x1000000 : 0;
}

void havok_memory_initialize(void)
{
    g_havok_memory_globals.active_allocator = _memory_allocator_havok;
    g_havok_memory_globals.initialized = true;
    g_havok_memory_globals.startup_pool = havok_memory_initialize_startup_pool(g_havok_memory_globals.memory_allocator_buffer);
    g_havok_memory_globals.runtime_pool = havok_memory_initialize_runtime_pool(g_havok_memory_globals.memory_allocator_buffer + k_startup_pool_size);
    csmemset(&g_havok_memory, 0, sizeof(g_havok_memory));
    new (&g_havok_memory) c_havok_memory();
    g_havok_memory.add_object_reference();
}

void havok_memory_update_initialize(void)
{
    long alignment_adjustment = 0;
    if (!g_havok_memory_globals.overflow_pool_memory)
    {
        g_havok_memory_globals.overflow_pool_memory_size = sizeof(g_havok_memory_globals.overflow_pool_default_memory);
        g_havok_memory_globals.overflow_pool_memory = g_havok_memory_globals.overflow_pool_default_memory;
    }
    char *overflow_memory_buffer = g_havok_memory_globals.overflow_pool_memory;
    long overflow_memory_size = g_havok_memory_globals.overflow_pool_memory_size;
    char *aligned_overflow_memory_buffer = overflow_memory_buffer;
    long aligned_overflow_memory_size = overflow_memory_size;
    if (((intptr_t)overflow_memory_buffer & 15) != 0)
    {
        alignment_adjustment = (long)((16 - (intptr_t)overflow_memory_buffer) & 15);
        aligned_overflow_memory_buffer = alignment_adjustment + overflow_memory_buffer;
        aligned_overflow_memory_size = overflow_memory_size - alignment_adjustment;
    }
    g_havok_memory_globals.overflow_pool = havok_memory_initialize_overflow_pool(
        g_havok_memory_globals.memory_allocator_buffer + k_startup_pool_size + k_runtime_pool_size,
        aligned_overflow_memory_buffer, aligned_overflow_memory_size);
}

// @address 0x25e758 ?havok_memory_garbage_collect@@YAXXZ
void havok_memory_garbage_collect(void)
{
    int condition = havok_get_memory_condition_type(0);
    unsigned char oversize = havok_memory_has_oversize_allocations();
    for (int level = !oversize; level < 7 && condition >= 2; ++level)
    {
        int total_iterations = 0;
        if (condition >= 3)
            havok_display_stats(1);
        condition = havok_get_memory_condition_type(0);
        for (int pass = 0; pass < 3 && condition >= 2; ++pass)
        {
            bool reclaim = level == 0 ? pass == 0 : pass == 2;
            int force = pass > 1;
            int iterations = 0;
            while (true)
            {
                if (!havok_cleanup_large_simulation_islands(level, reclaim, force, -1))
                    break;
                oversize = havok_memory_has_oversize_allocations();
                condition = havok_get_memory_condition_type(0);
                if (condition < 2)
                    break;
                if (level == 0 && !oversize)
                {
                    pass = 3;
                    break;
                }
                ++iterations;
                ++total_iterations;
                if (total_iterations % 32 == 0)
                {
                    ::havok_memory_garbage_collect_reset_allocated_state();
                    condition = havok_get_memory_condition_type(0);
                }
                if (iterations > 1024)
                {
                    havok_display_stats(1);
                    break;
                }
            }
        }
    }
    g_havok_memory.m_memory_state = phys_memory::MEMORY_STATE_OK;
}


// @address 0x25e8d8 ?havok_memory_garbage_collect_reset_allocated_state@@YAXXZ
void havok_memory_garbage_collect_reset_allocated_state(void)
{
    bool had_overflow_pool = g_havok_memory_globals.overflow_pool != 0;
    havok_dispose_from_old_structure_bsp(g_active_structure_bsp_mask);
    havok_initialize_all_allocated_state(false);
    havok_memory_task_times()[1] = tls_game_time_globals()->time;
    if (had_overflow_pool && g_havok_memory_globals.overflow_pool == 0)
        havok_memory_update_initialize();
}

// @address 0x25e978 ?havok_compute_memory_in_use@@YAXPAUs_havok_memory_usage@@@Z
void havok_compute_memory_in_use(s_havok_memory_usage *usage)
{
    memset(usage, 0, sizeof(*usage));
    if (g_havok_memory_globals.initialized)
    {
        {
            s_havok_2nd_pool_memory_usage *out = &usage->second_overflow;
            c_havok_memory_pool *memory = static_cast<c_havok_memory_pool *>(g_havok_memory_globals.second_overflow_pool);
            if (memory)
            {
                out->pool_size = 2672;
                int count = memory->m_current_allocation_count;
                out->bytes_in_use = 0;
                out->allocation_count = count;
                for (int i = 0; i < memory->m_current_allocation_count; ++i)
                    out->bytes_in_use += memory->get_allocation(i)->count;
                out->last = 0;
            }
            else
                memset(out, 0, sizeof(*out));
        }
        {
            s_havok_pool_memory_usage *out = &usage->overflow;
            c_havok_memory_pool *memory = static_cast<c_havok_memory_pool *>(g_havok_memory_globals.overflow_pool);
            if (memory)
            {
                out->pool_size = 2672;
                int count = memory->m_current_allocation_count;
                out->bytes_in_use = 0;
                out->allocation_count = count;
                for (int i = 0; i < memory->m_current_allocation_count; ++i)
                    out->bytes_in_use += memory->get_allocation(i)->count;
                out->allocation_count = memory->m_current_allocation_count;
                out->extra = 0;
            }
            else
                memset(out, 0, sizeof(*out));
        }
        {
            s_havok_runtime_memory_usage *out = &usage->runtime;
            phys_memory *memory = g_havok_memory_globals.runtime_pool;
            c_runtime_pool *runtime_memory = static_cast<c_runtime_pool *>(memory);
            out->pool_size = 0x384e10;
            out->block_count = 450;
            out->bytes_in_use = runtime_memory->m_small_page_count + runtime_memory->m_large_page_count;
            out->block_size = 8192;
            out->free_blocks = (int)(runtime_memory->m_page_end - runtime_memory->m_page_next) + 1;
            out->fragmentation = havok_memory_runtime_pool_memory_fraction(memory);
            out->record_count = 0;
            out->maximum_records = 16;
            memset(out->records, 0, sizeof(out->records));
        }
        {
            s_havok_pool_memory_usage *out = &usage->startup;
            c_havok_memory_pool *memory = static_cast<c_havok_memory_pool *>(g_havok_memory_globals.startup_pool);
            out->pool_size = 0x25680;
            int count = memory->m_current_allocation_count;
            out->bytes_in_use = 0;
            out->allocation_count = count;
            for (int i = 0; i < memory->m_current_allocation_count; ++i)
                out->bytes_in_use += memory->get_allocation(i)->count;
            out->extra = 0;
        }
    }
}

c_havok_memory::~c_havok_memory()
{
}

// @address 0x25ec30 ?allocate_chunk@c_havok_memory@@UAAPAXHW4phys_memory_class@@@Z
void *c_havok_memory::allocate_chunk(int size, phys_memory_class memory_class)
{
    void *result;
    if (havok_get_active_memory_allocator() != _memory_allocator_havok)
    {
        result = g_havok_memory_globals.runtime_pool->allocate_chunk(size, memory_class);
        if (!result)
        {
            result = g_havok_memory_globals.overflow_pool->allocate_chunk(size, memory_class);
            if (!result)
                result = havok_get_2nd_overflow_pool()->allocate_chunk(size, memory_class);
        }
    }
    else
    {
        result = g_havok_memory_globals.startup_pool->allocate_chunk(size, memory_class);
    }
    return result;
}

// @address 0x25ed00 ?free_chunk@c_havok_memory@@UAAXPAXHW4phys_memory_class@@@Z
void c_havok_memory::free_chunk(void *block, int size, phys_memory_class memory_class)
{
    if (block)
    {
        if (havok_memory_is_runtime_pool_memory_address(block, g_havok_memory_globals.runtime_pool))
            g_havok_memory_globals.runtime_pool->free_chunk(block, size, memory_class);
        else if (havok_memory_is_startup_pool_memory_address(block, g_havok_memory_globals.startup_pool))
            g_havok_memory_globals.startup_pool->free_chunk(block, size, memory_class);
        else if (havok_memory_is_overflow_pool_memory_address(block, g_havok_memory_globals.overflow_pool))
            g_havok_memory_globals.overflow_pool->free_chunk(block, size, memory_class);
        else
            havok_get_2nd_overflow_pool()->free_chunk(block, size, memory_class);
    }
}

// @address 0x25ee18 ?allocate_runtime_block@c_havok_memory@@UAAPAXHW4phys_memory_class@@@Z
void *c_havok_memory::allocate_runtime_block(int size, phys_memory_class memory_class)
{
    return g_havok_memory_globals.overflow_pool->allocate_chunk(size, memory_class);
}
// @address 0x25ee40 ?free_runtime_block@c_havok_memory@@UAAXPAXHW4phys_memory_class@@@Z
void c_havok_memory::free_runtime_block(void *block, int size, phys_memory_class memory_class)
{
    g_havok_memory_globals.overflow_pool->free_chunk(block, size, memory_class);
}
void havok_thread_memory_deallocate_chunk(void *block, int size, phys_memory_class memory_class)
{
    phys_memory *allocator = havok_get_memory_allocator_class();
    allocator->free_chunk(block, size, memory_class);
}
void *havok_thread_memory_allocate_chunk(int size, phys_memory_class memory_class)
{
    phys_memory *allocator = havok_get_memory_allocator_class();
    return allocator->allocate_chunk(size, memory_class);
}

#if defined(__clang__)
// Port-only: March bodies are ASSERT("unreachable") (physics\havok_memory.cpp:513/561); the
// linker folds both into the shared trap at 0x1b65a8. The Xbox caller (hkWindowsMalloc/
// hkWindowsFree in native hkMemory.obj) binds the tag-absolute: a native definition would drop
// as a fold and strand it (LNK2001). The PC link needs bodies from source; they trap, matching
// the folded bytes.
void *havok_aligned_malloc(int size, int alignment)
{
    (void)size;
    (void)alignment;
    __builtin_trap();
}
void havok_aligned_free(void *block)
{
    (void)block;
    __builtin_trap();
}
#endif

extern "C" {
}

// the fraction of the runtime pool's pages in use: small rows count the chunks out of the pool (chopped, neither
// free in the pool nor cached by this thread), large rows give back their free chunks
// @address 0x2e0678 ?havok_memory_runtime_pool_memory_fraction@@YAMPAVphys_memory@@@Z
float havok_memory_runtime_pool_memory_fraction(phys_memory *memory)
{
    c_runtime_pool *runtime_pool = static_cast<c_runtime_pool *>(memory);
    phys_thread_memory &thread_memory = phys_thread_memory::get_singleton_instance();
    long small_bytes = 0;
    long large_bytes = 0;
    for (long row = 1; row < 17; row++)
    {
        if (runtime_pool->m_row_sizes[row] <= 512)
        {
            long bytes = (runtime_pool->m_chopped_count[row] - thread_memory.m_block_free_list[row].m_elem_count - runtime_pool->m_free_count[row]) *
                runtime_pool->m_row_sizes[row];
            if (runtime_pool->m_row_sizes[row] == 8)
                bytes *= 2;
            small_bytes += bytes;
        }
        else
        {
            large_bytes += runtime_pool->m_free_count[row] * runtime_pool->m_row_sizes[row];
        }
    }
    long pages = (long)(small_bytes / 8192.0f + 1.0f) - (long)(large_bytes / 8192.0f) + runtime_pool->m_large_page_count;
    return pages / 450.0f;
}

// Reset the component array and install the entity listeners for a new map
// (this stand-in would stay a named
// local and get +50-cycle latency-50 edges).  Kept under a unique name so the
// owning unit can later define the real helper without a duplicate symbol.
struct s_data_array;
extern "C" {
// @address 0x8c4260 g_global_entity_listener
// @address 0x8c4264 g_world_entity_listener
}
void havok_components_initialize_for_new_map_ltcg_inline(s_data_array *components, c_entity_listener *entity_listener,
    c_world_entity_listener *world_entity_listener)
{
    components->valid = true;
    g_global_entity_listener = entity_listener;
    g_world_entity_listener = world_entity_listener;
    data_delete_all(components);
}

c_havok_memory g_havok_memory;

// @address 0x11df824 g_havok_memory_allocator_locked
extern "C" {
__declspec(thread) bool g_havok_memory_allocator_locked;
}

// The front allocator forwards chunk calls only; the pool-level runtime hooks trap if reached.
// @address 0x1b65a8 ?preallocate_runtime_block@c_havok_memory@@UAAXHW4phys_memory_class@@@Z
void c_havok_memory::preallocate_runtime_block(int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?provide_runtime_block@c_havok_memory@@UAAXPAXHW4phys_memory_class@@@Z
void c_havok_memory::provide_runtime_block(void *, int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?release_runtime_blocks@c_havok_memory@@UAAXXZ
void c_havok_memory::release_runtime_blocks()
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x5bec98 ?print_memory_stats@c_havok_memory@@UAAXPAVphys_out_stream@@@Z
void c_havok_memory::print_memory_stats(phys_out_stream *) {}

// @address 0x5bec98 ?getStatSynopsis@c_havok_memory@@UAAXPAUhkMemoryStatistics@@@Z
void c_havok_memory::getStatSynopsis(phys_memory_stats *) {}


