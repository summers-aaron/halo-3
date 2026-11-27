// blamlibxbox/havok_runtime_pool.obj: from the records 0x1039b0 0x103ae8 0x103d80 0x1040b0 0x1042e0 0x1043d8 0x104420 0x1044d8 0x260350 0x2e0168 0x2e04d0 0x2e0548 0x2e0558 0x2e05b0 0x2e0608
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x1044d8 ?free_chunk_batch@c_runtime_pool@@UAAXPAPAXHH@Z
// @address 0x2e0548 ?print_memory_stats@c_runtime_pool@@UAAXPAVphys_out_stream@@@Z
// @address 0x2e0608 ?deallocate_chunk_internal@c_runtime_pool@@QAAXPAXHW4phys_memory_class@@@Z
#include "structures/structure_seam_globals.h"
#include "tag_files/tag_groups.h"
#include "structures/structure_runtime_resources.h"
#include "multithreading/thread_tls.h"
#include <stddef.h>
#include "phys/phys_base_types.h"
#include "phys/phys_collide.h"
#include "physics/havok_collision_surface_shape.h"
#include "math/matrix_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "cseries/cseries.h"
// @address 0x1043d8 ?vertex_ids_to_vertices_impl@c_collision_surface_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x104420 ?free_chunk_batch@c_havok_memory@@UAAXPAPAXHH@Z
#include <vectorintrinsics.h>
#include <string.h>
#include "physics/breakable_surfaces.h"
#include "physics/havok_utilities.h"
#include "physics/havok_runtime_pool.h"
#include "physics/havok_runtime_pool_types.h"
#include "scenario/scenario.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_access.h"
#include "physics/havok_shape_key.h"
#include "physics/collision_surface_types.h"
#include "physics/havok_runtime_pool_cache_entry_types.h"
#include "physics/memory_allocator_types.h"
#include "physics/structure_bsp_seam_link_types.h"
#include "structures/structure_seam_types.h"
#include "cache/cache_files.h"
#include "physics/havok.h"

#include "phys/phys_out_stream.h"
#include "phys/phys_writer.h"
#include "physics/collision_bsp_types.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_critical_section_entry.h"

struct phys_memory_stats;
#include "phys/phys_system_alloc.h"
#include "phys/phys_memory.h"
#include "phys/phys_critical_section.h"

#include "physics/havok_runtime_pool_class.h"
// @address 0x2e04d0 ??_Gc_runtime_pool@@UAAPAXI@Z


#include "scenario/scenario_tag.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_instanced_geometry.h"
#include "phys/phys_transform_shape.h"
#include "physics/havok_constants_types.h"
#include "physics/havok_environment_surface_shape.h"
#include "physics/havok_memory_globals.h"
#include "physics/havok_memory_class.h"

// The by-value wrapper keeps getChildShape's return-register flow identical.
struct c_child_shape_result
{
    const phys_shape *shape;
    __forceinline c_child_shape_result() : shape(0) {}
    __forceinline c_child_shape_result(const c_child_shape_result &other) : shape(other.shape) {}
    __forceinline ~c_child_shape_result() {}
    __forceinline c_child_shape_result &operator=(const phys_shape *value)
    {
        shape = value;
        return *this;
    }
};

static __forceinline const phys_shape *havok_runtime_pool_shape_result_get(c_child_shape_result result)
{
    return result.shape;
}


// @address 0x1039b0 ?allocate_chunk_batch@c_runtime_pool@@UAAXPAPAXHH@Z
void c_runtime_pool::allocate_chunk_batch(void **blocks, int count, int size)
{
    havok_critical_section_enter(&m_critical_section);
    for (int allocated = 0; allocated < count; allocated++)
    {
        blocks[allocated] = allocate_chunk_internal(size, (phys_memory_class)0);
        if (blocks[allocated] == 0)
        {
            for (int index = allocated - 1; index >= 0; index--)
            {
                deallocate_chunk(blocks[index], size);
                blocks[index] = 0;
            }
            break;
        }
    }
    m_critical_section.leave();
}

// @address 0x1040b0 havok_runtime_pool_is_surface_enabled
extern "C" bool havok_runtime_pool_is_surface_enabled(long structure_bsp_index, long, long, long surface_key)
{
    bool enabled = true;
    long index = surface_key & 0xffff;
    long type = (unsigned long)surface_key >> 29;
    switch (type)
    {
    case 3:
    {
        structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
        long geometry_index = ((structure_instanced_geometry_instance *)bsp->instanced_geometry_instances.address)[index].definition_index;
        long surface_index = ((unsigned long)surface_key >> 16) & 0x1fff;
        s_collision_surface *surface = TAG_BLOCK_GET_ELEMENT(
            &g_collision_bsp_cache.users[get_current_thread_index()].entries[structure_bsp_index].data[geometry_index].bsp.surfaces,
            surface_index, s_collision_surface);
        if (TEST_BIT(surface->flags, 3) &&
            !breakable_surface_extant(structure_bsp_index, index, surface->breakable_set_index, surface->breakable_surface_index))
            enabled = false;
        break;
    }
    case 1:
    {
        structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
        s_structure_bsp_resource *resource = &bsp->resource;
        s_structure_bsp_resources *resources = resource->use_resource_items
            ? resource->collision_resource_interface.get_resources()
            : (s_structure_bsp_resources *)resource->raw_resources.address;
        s_collision_surface *surface = &((s_collision_surface *)resources->collision_bsp->surfaces.address)[index];
        if (!TEST_BIT(surface->flags, 3) || breakable_surface_extant(structure_bsp_index, -1, surface->breakable_set_index, surface->breakable_surface_index))
        {
            if (surface->seam_link_index != -1)
            {
                short seam_index = ((s_structure_bsp_seam_link *)bsp->surface_seam_links.address)[surface->seam_link_index].seam_index;
                if (seam_index != -1)
                {
                    s_seam_mapping &mapping = g_structure_seam_globals->mappings[seam_index];
                    bool above = mapping.structure_indices[1] == structure_bsp_index;
                    if (mapping.structure_indices[above] == structure_bsp_index && mapping.structure_indices[!above] != -1)
                        enabled = false;
                }
            }
        }
        else
        {
            enabled = false;
        }
        break;
    }
    }
    return enabled;
}

extern "C" {
// @address 0xee94e0 g_havok_runtime_pool_cache

}


// @address 0x1042e0 havok_runtime_pool_cache_try_and_get
extern "C" bool havok_runtime_pool_cache_try_and_get(long type, unsigned long key, void **storage)
{
    bool found = false;
    long hash = ((key >> (type + 12) | key << (type + 20)) + (key >> (type + 24) | key << (type + 8)) + (key << (type + 36) | key << (type - 4)) + key) & 4095;
    long index = g_havok_runtime_pool_cache.buckets[hash];
    if (index != -1)
    {
        do
        {
            s_havok_runtime_pool_cache_entry *entry = &g_havok_runtime_pool_cache.entries[index];
            if (entry->key == key && entry->type == type)
            {
                found = true;
                *storage = g_havok_runtime_pool_cache.shapes[index];
                break;
            }
            index = entry->next;
        } while (index != -1);
    }
    if (!found)
        *storage = havok_runtime_pool_cache_allocate_entry(type, key);
    return found;
}

void c_collision_surface_shape::vertex_ids_to_vertices_impl(const unsigned short *indices, int count, phys_hull_vertex *dst) const
{
#ifdef HALO3_64BIT
    for (int i = count - 1; i >= 0; i--)
        dst[i].m_simd_quad = m_vertices[indices[i]].m_simd_quad;
#else
    const __vector4 *src = (const __vector4 *)this;
    for (int i = count - 1; i >= 0; i--)
        dst[i].m_simd_quad = src[indices[i] + 3];
#endif
}

static __forceinline const collision_bsp *havok_runtime_pool_collision_bsp_get(long structure_bsp_index)
{
    s_structure_bsp_resource *resource = &global_structure_bsp_get(structure_bsp_index)->resource;
    return (resource->use_resource_items
        ? resource->collision_resource_interface.get_resources()
        : (s_structure_bsp_resources *)resource->raw_resources.address)->collision_bsp;
}





// @address 0x103d80 ?get_child_shape@c_environment_surface_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
const phys_shape *c_environment_surface_shape::get_child_shape(unsigned int key, char (&buffer)[512]) const
{
    c_child_shape_result result;
    bool enabled = is_key_alive(key);
    long index, surface_index, type;
    values_from_shape_key(&index, &surface_index, &type, key);
    if (type == 1)
    {
        void *storage = buffer;
        if (g_havok_constants.cache_environment_shapes && havok_runtime_pool_cache_try_and_get(m_structure_bsp_index, key, &storage))
        {
            c_collision_surface_shape *surface_shape = (c_collision_surface_shape *)storage;
            surface_shape->m_flags.set(1, !enabled);
            result = surface_shape;
        }
        else
        {
            result = new (storage) c_collision_surface_shape(key, m_structure_bsp_index,
                havok_runtime_pool_collision_bsp_get(m_structure_bsp_index),
                0.0f, true, !enabled, -1, index, 0, (e_soft_surface_sink_class)surface_index, 0.2f);
        }
    }
    else if (type == 2)
    {
        structure_instanced_geometry_instance *instance
            = &((structure_instanced_geometry_instance *)global_structure_bsp_get(m_structure_bsp_index)
            ->instanced_geometry_instances.address)[index];
        phys_shape *sub_shape = &instance->collision->shape;
        phys_transform transform;
        sub_shape->object_remove_ref();
        phys_transform_from_matrix4x3(&transform, &instance->transform);
        result = new (buffer) phys_transform_shape(sub_shape, transform);
    }
    else if (type == 3)
    {
        void *storage = buffer;
        if (g_havok_constants.cache_environment_shapes && havok_runtime_pool_cache_try_and_get(m_structure_bsp_index, key, &storage))
        {
            c_collision_surface_shape *surface_shape = (c_collision_surface_shape *)storage;
            surface_shape->m_flags.set(1, !enabled);
            result = surface_shape;
        }
        else
        {
            structure_instanced_geometry_instance *instance
                = &((structure_instanced_geometry_instance *)global_structure_bsp_get(m_structure_bsp_index)
                ->instanced_geometry_instances.address)[index];
            havok_runtime_pool_instance_collision *collision = instance->collision;
            result = new (storage) c_collision_surface_shape(key, collision->geometry_shape.m_structure_bsp_index, collision->geometry_shape.m_bsp,
                collision->geometry_shape.m_radius, true, !enabled, collision->geometry_shape.m_geometry_index, surface_index, &instance->transform,
                _soft_surface_sink_class_default, 0.0f);
        }
    }
    return havok_runtime_pool_shape_result_get(result);
}

// @address 0xa430e0 g_havok_memory_globals



static bool havok_runtime_pool_block_in_range(phys_memory *pool, void *block, unsigned int begin, unsigned int end)
{
    return (char *)block >= (char *)pool + begin && (char *)block <= (char *)pool + end;
}

void c_havok_memory::free_chunk_batch(void **blocks, int count, int size)
{
    if (count > 0 && size > 0)
    {
        phys_memory *heap = g_havok_memory_globals.runtime_pool;
        if (havok_runtime_pool_block_in_range(heap, blocks[0], 0xd50, 0x384d50))
        {
            heap->free_chunk_batch(blocks, count, size);
            return;
        }
        heap = g_havok_memory_globals.startup_pool;
        if (havok_runtime_pool_block_in_range(heap, blocks[0], 0xa70, 0x25670))
            heap->free_chunk_batch(blocks, count, size);
    }
}

void c_runtime_pool::free_chunk_batch(void **blocks, int count, int size)
{
    havok_critical_section_enter(&m_critical_section);
    for (int block_index = 0; block_index < count; block_index++)
        deallocate_chunk(blocks[block_index], size);
    m_critical_section.leave();
}

namespace
{
    static __forceinline int havok_runtime_pool_const_size_to_row(int size)
    {
        if (size <= 8) return 1;
        else if (size <= 16) return 2;
        else if (size <= 32) return 3;
        else if (size <= 48) return 4;
        else if (size <= 64) return 5;
        else if (size <= 96) return 6;
        else if (size <= 128) return 7;
        else if (size <= 160) return 8;
        else if (size <= 192) return 9;
        else if (size <= 256) return 10;
        else if (size <= 320) return 11;
        else if (size <= 512) return 12;
        else if (size <= 1024) return 13;
        else if (size <= 2048) return 14;
        else if (size <= 4096) return 15;
        else if (size <= 8192) return 16;
        else return -1;
    }
}


// @address 0x103ae8 ?allocate_chunk_internal@c_runtime_pool@@QAAPAXHW4phys_memory_class@@@Z
void *c_runtime_pool::allocate_chunk_internal(int size, phys_memory_class memory_class)
{
    void *block = 0;
    if (size <= 8192)
    {
        int row = get_row(size);
        m_allocations_in_use[row].allocations_in_use++;
        void **entry = (void **)m_free_lists[row];
        if (entry)
        {
            m_free_lists[row] = *entry;
            block = record_chopped_allocation_state_change(entry, row, true);
        }
        else
            block = allocate_and_chop_page(size, row);
    }
    return block;
}


// @address 0x260350 havok_runtime_pool_cache_allocate_entry
extern "C" void *havok_runtime_pool_cache_allocate_entry(long type, unsigned long key)
{
    long hash = ((key >> (type + 12) | key << (type + 20)) + (key >> (type + 24) | key << (type + 8)) + (key << (type + 36) | key << (type - 4)) + key) & 4095;
    long index = g_havok_runtime_pool_cache.next_index;
    s_havok_runtime_pool_cache_entry *entry = &g_havok_runtime_pool_cache.entries[index];
    if (g_havok_runtime_pool_cache.count == 512)
    {
        if (entry->previous != -1)
            g_havok_runtime_pool_cache.entries[entry->previous].next = entry->next;
        else
            g_havok_runtime_pool_cache.buckets[entry->hash] = entry->next;
        if (entry->next != -1)
            g_havok_runtime_pool_cache.entries[entry->next].previous = entry->previous;
        memset(entry, -1, sizeof(s_havok_runtime_pool_cache_entry));
    }
    else
    {
        g_havok_runtime_pool_cache.count++;
    }
    long head = g_havok_runtime_pool_cache.buckets[hash];
    entry->key = key;
    entry->type = (unsigned short)type;
    entry->hash = (short)hash;
    entry->previous = -1;
    entry->next = head;
    g_havok_runtime_pool_cache.buckets[hash] = (short)index;
    if (head != -1)
        g_havok_runtime_pool_cache.entries[head].previous = (short)index;
    g_havok_runtime_pool_cache.next_index = (g_havok_runtime_pool_cache.next_index + 1) % 512;
    return g_havok_runtime_pool_cache.shapes[index];
}


// @address 0x2e0168 ??0c_runtime_pool@@QAA@XZ
c_runtime_pool::c_runtime_pool() : m_critical_section(4000)
{
    m_page_start = 0;
    m_page_end = 0;
    m_page_next = 0;
    m_small_page_count = 0;
    m_large_page_count = 0;
    memset(m_chopped_count, 0, sizeof(m_chopped_count));
    memset(m_free_count, 0, sizeof(m_free_count));
    for (int row = 16; row >= 0; row--)
        m_free_lists[row] = 0;
    for (int size = 0; size <= 512; size++)
    {
        int row = havok_runtime_pool_const_size_to_row(size);
        m_small_rows[size] = (signed char)row;
        m_row_sizes[row] = size;
    }
    for (int index = 0; index < 8; index++)
    {
        int size = (index + 1) << 10;
        int row = havok_runtime_pool_const_size_to_row(size);
        m_large_rows[index] = row;
        m_row_sizes[row] = size;
    }
    m_page_count = 0;
    m_system_allocation_count = 0;
    m_system_allocation_size = 0;
    m_peak_system_allocation_size = 0;
    m_page_size = 8192;
    m_page_overhead = 64;
}

void c_runtime_pool::print_memory_stats(phys_out_stream *stream)
{
    stream->printf("Statistics are disabled, please enable them in hkbase/config/hkConfigMemoryStats.h\n");
}

// @address 0x2e0558 ?allocate_chunk@c_runtime_pool@@UAAPAXHW4phys_memory_class@@@Z
void *c_runtime_pool::allocate_chunk(int size, phys_memory_class memory_class)
{
    void *block = 0;
    havok_critical_section_enter(&m_critical_section);
    block = allocate_chunk_internal(size, memory_class);
    m_critical_section.leave();
    return block;
}

// @address 0x2e05b0 ?free_chunk@c_runtime_pool@@UAAXPAXHW4phys_memory_class@@@Z
void c_runtime_pool::free_chunk(void *block, int size, phys_memory_class memory_class)
{
    havok_critical_section_enter(&m_critical_section);
    deallocate_chunk_internal(block, size, memory_class);
    m_critical_section.leave();
}

void c_runtime_pool::deallocate_chunk_internal(void *block, int size, phys_memory_class memory_class)
{
    deallocate_chunk(block, size);
}

// The runtime pool serves paged chunks only; runtime blocks trap if reached.
// @address 0x1b65a8 ?allocate_runtime_block@c_runtime_pool@@UAAPAXHW4phys_memory_class@@@Z
void *c_runtime_pool::allocate_runtime_block(int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?free_runtime_block@c_runtime_pool@@UAAXPAXHW4phys_memory_class@@@Z
void c_runtime_pool::free_runtime_block(void *, int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?preallocate_runtime_block@c_runtime_pool@@UAAXHW4phys_memory_class@@@Z
void c_runtime_pool::preallocate_runtime_block(int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?provide_runtime_block@c_runtime_pool@@UAAXPAXHW4phys_memory_class@@@Z
void c_runtime_pool::provide_runtime_block(void *, int, phys_memory_class)
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?release_runtime_blocks@c_runtime_pool@@UAAXXZ
void c_runtime_pool::release_runtime_blocks()
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x5bec98 ?getStatSynopsis@c_runtime_pool@@UAAXPAUhkMemoryStatistics@@@Z
void c_runtime_pool::getStatSynopsis(phys_memory_stats *) {}

