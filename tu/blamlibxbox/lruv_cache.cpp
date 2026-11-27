// blamlibxbox/lruv_cache.obj: from the records 0x101518 0x1015f0 0x101748 0x141a08 0x2865a8 0x2865e0 0x286710 0x2867b8 0x286810 0x286888 0x286978 0x286c78 0x286e08 0x4555f0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
#include "cseries/cseries.h"
// @address 0x101518 ?lruv_idle@@YAXPAUs_lruv_cache@@@Z
// @address 0x1015f0 ?lruv_block_new_at_index@@YAJPAUs_lruv_cache@@JJPAUs_lruv_cache_hole@@J@Z
// @address 0x101748 lruv_block_initialize
// @address 0x141a08 lruv_block_new
// @address 0x2865a8 lruv_complete_request
// @address 0x2865e0 ?lruv_new@@YAPAUs_lruv_cache@@PBDJJJPAXP6AX1J_N@ZP6A_N1J@ZP6AE1J@ZPAVc_allocation_base@@J@Z
// @address 0x286710 ?lruv_setup@@YAXPAUs_lruv_cache@@PBDJPAXP6AX2J_N@ZP6A_N2J@ZP6AE2J@ZPAVc_allocation_base@@J@Z
// @address 0x2867b8 ?lruv_delete@@YAXPAUs_lruv_cache@@@Z
// @address 0x286810 ?lruv_flush@@YAXPAUs_lruv_cache@@@Z
// @address 0x286888 lruv_should_use_hole
// @address 0x286978 ?lruv_cache_find_hole@@YA_NPAUs_lruv_cache@@JJPAUs_lruv_cache_hole@@PAJPA_N@Z
// @address 0x286c78 lruv_block_new_at_page
// @address 0x286e08 lruv_block_delete
// @address 0x4555f0 lruv_block_delete_internal

#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "memory/lruv_cache.h"
#include "text/font_cache.h"
#include "text/font_cache_types.h"
#include "memory/data.h"
#include "memory/lruv_block_types.h"
#include "memory/lruv_block_header_types.h"

#include "multithreading/synchronized_value.h"
#include "rasterizer/rasterizer_text_glyph_cache_types.h"
#include "memory/lruv_cache_types.h"

#include "memory/lruv_cache_internal.h"

struct s_lruv_cache_hole
{
    long previous_block_index;
    unsigned long frame;
    long page_index;
    long priority;
};

namespace {
static __forceinline void lruv_reset_block_ages(s_lruv_cache *cache)
{
    cache->frame = 0x101;
    c_data_iterator<s_lruv_block> iterator;
    iterator.begin(cache->header.block_data);
    while (iterator.next()) {
        unsigned long age = 0x7fffffff - iterator.get_datum()->last_used_frame;
        if (age > 0x100)
            iterator.get_datum()->last_used_frame = 0;
        else
            iterator.get_datum()->last_used_frame = 0x100 - age;
    }
}
}
void lruv_idle(s_lruv_cache *cache)
{
    long critical_section = cache->critical_section;
    if (critical_section != -1)
        internal_critical_section_enter(critical_section);
    if ((unsigned long)cache->frame == 0x7fffffffUL)
        lruv_reset_block_ages(cache);
    else
        cache->frame++;
    for (unsigned long index = 0; index < 8; index++)
        cache->smallest_failed_page_counts[index] = 0x7fffffff;
    internal_critical_section_leave(critical_section);
}

extern "C" void lruv_complete_request(long result_a, long result_b, s_lruv_completion_context *context)
{
    s_lruv_completion *completion = context->completion;
    if (completion)
    {
        completion->result_a = result_a;
        completion->result_b = result_b;
        completion->done.set(1);
    }
}

s_lruv_cache *lruv_new(const char *name, long maximum_size, long page_size_bits, long maximum_block_count, void *client_data,
    void (*delete_proc)(void *, long, bool), bool (*locked_proc)(void *, long), unsigned char (*usage_proc)(void *, long),
    c_allocation_base *allocator, long critical_section_index)
{
    unsigned long size = sizeof(s_lruv_cache) + sizeof(s_data_array) + maximum_block_count * sizeof(s_lruv_block) + ((maximum_block_count + 31) >> 5) * 4;
    s_lruv_cache_allocation *allocation = (s_lruv_cache_allocation *)allocator->allocate(size, name);
    s_lruv_cache *cache = &allocation->cache;
    s_data_array *blocks = &allocation->blocks;
    data_initialize(blocks, name, maximum_block_count, sizeof(s_lruv_block), 0, &g_no_allocation_storage);
    blocks->valid = true;
    data_delete_all(blocks);
    lruv_setup(cache, name, page_size_bits, client_data, delete_proc, locked_proc, usage_proc, allocator, critical_section_index);
    cache->maximum_page_count = maximum_size;
    s_lruv_block_header *header = &cache->header;
    header->block_data = blocks;
    long allocation_ratio = -1;
#if defined(__clang__)
    // The 64-bit port compares the pointer directly (a long truncates an address there).
    if (cache->maximum_page_count != 0 && cache->header.block_data != 0)
#else
    // match: 0x2865e0: the long-typed null check emits cmpwi; comparing the pointer emits cmplwi
    if (cache->maximum_page_count != 0 && (long)cache->header.block_data != 0)
#endif
        allocation_ratio = (cache->maximum_page_count / cache->header.block_data->maximum_count) << 3;
    cache->pages_per_block_ratio = allocation_ratio;
    for (long index = 0; index < 8; index++)
        cache->smallest_failed_page_counts[index] = 0x7fffffff;
    return cache;
}
void lruv_setup(s_lruv_cache *cache, const char *name, long page_size_bits, void *client_data, void (*delete_proc)(void *, long, bool),
    bool (*locked_proc)(void *, long), unsigned char (*usage_proc)(void *, long), c_allocation_base *allocator, long critical_section_index)
{
    memset(cache, 0, sizeof(*cache));
    cache->critical_section = critical_section_index;
    csstrnzcpy(cache->name, name, 32);

    s_lruv_procs *procs = &cache->procs;
    procs->client_data = client_data;
    procs->delete_proc = delete_proc;
    procs->locked_proc = locked_proc;
    procs->usage_proc = usage_proc;

    cache->maximum_page_count = 0;
    cache->page_size_bits = page_size_bits;
    cache->pages_per_block_ratio = -1;

    s_lruv_block_header *header = &cache->header;
    header->block_data = 0;
    header->signature = 0x77656565;

    cache->first_block_index = -1;
    cache->last_block_index = -1;
    cache->frame = 1;
    cache->hole_algorithm = 0;
    cache->allocator = allocator;
}

void lruv_delete(s_lruv_cache *cache)
{
    c_allocation_base *allocator = cache->allocator;
    data_dispose(cache->header.block_data);
    allocator->deallocate(cache);
}

void lruv_flush(s_lruv_cache *cache)
{
    c_data_iterator<s_lruv_block> iterator;
    iterator.begin(cache->header.block_data);
    while (iterator.next())
        lruv_block_delete(cache, iterator.get_index(), false);
}


static __forceinline bool should_use_this_hole_age(const s_lruv_cache *cache, const s_lruv_cache_hole *hole_a, const s_lruv_cache_hole *hole_b)
{
    bool result = false;
    if (hole_a->frame < hole_b->frame || (hole_a->frame == hole_b->frame && hole_a->priority < hole_b->priority))
        result = true;
    return result;
}

static __forceinline bool should_use_this_hole_fragmentation(const s_lruv_cache *cache, const s_lruv_cache_hole *hole_a, const s_lruv_cache_hole *hole_b)
{
    bool result = false;
    long age_a = PIN(cache->frame - hole_a->frame, 0, 300);
    long age_b = PIN(cache->frame - hole_b->frame, 0, 300);
    if (age_a + 150 < age_b)
        result = false;
    else if (age_a > age_b + 150)
        result = true;
    else if (hole_a->priority < hole_b->priority)
        result = true;
    return result;
}

static __forceinline bool should_use_this_hole_blend(const s_lruv_cache *cache, const s_lruv_cache_hole *hole_a, const s_lruv_cache_hole *hole_b)
{
    long age_a = MAX(cache->frame - hole_a->frame, 0);
    long age_b = MAX(cache->frame - hole_b->frame, 0);
    return hole_b->priority * age_a > hole_a->priority * age_b;
}

extern "C" bool lruv_should_use_hole(const s_lruv_cache *cache, long unused, const s_lruv_cache_hole *hole_a, const s_lruv_cache_hole *hole_b)
{
    bool result;
    switch (cache->hole_algorithm)
    {
    case 0:
        result = should_use_this_hole_age(cache, hole_a, hole_b);
        break;
    case 1:
        result = should_use_this_hole_fragmentation(cache, hole_a, hole_b);
        break;
    default:
        result = should_use_this_hole_blend(cache, hole_a, hole_b);
        break;
    }
    return result;
}

static __forceinline s_lruv_block *lruv_cache_get_block(s_lruv_cache *cache, long block_index)
{
    s_data_array *blocks = cache->header.block_data;
    return &((s_lruv_block *)blocks->data)[(unsigned short)block_index];
}

long lruv_block_new_at_index(s_lruv_cache *cache, long datum_index, long block_index, s_lruv_cache_hole *hole, long count)
{
    long index = hole->previous_block_index != -1 ? lruv_cache_get_block(cache, hole->previous_block_index)->next_block_index : cache->first_block_index;
    while (index != -1)
    {
        long current = index;
        s_lruv_block *block = lruv_cache_get_block(cache, current);
        index = block->next_block_index;
        if ((unsigned long)block->page_index >= (unsigned long)(hole->page_index + count) ||
            (unsigned long)(block->page_index + block->page_count) <= (unsigned long)hole->page_index)
            break;
        lruv_block_delete(cache, current, true);
    }

    if (cache->header.block_data->actual_count == cache->header.block_data->maximum_count && block_index != -1)
    {
        if (hole->previous_block_index == block_index)
            hole->previous_block_index = lruv_cache_get_block(cache, block_index)->previous_block_index;
        lruv_block_delete(cache, block_index, true);
    }

    long new_block_index = datum_index != -1 ? datum_new_at_index(cache->header.block_data, datum_index)
                                             : datum_new(cache->header.block_data);
    lruv_block_initialize(cache, hole, count, new_block_index);
    return new_block_index;
}

// match: 0x101748: dropping __restrict reloads hole->previous_block_index (lwz r10,0(r29)) for the second test and recolors r7 to r9
extern "C" void lruv_block_initialize(s_lruv_cache *cache, const s_lruv_cache_hole *__restrict hole, long count, long block_index)
{
    if (block_index != -1)
    {
        s_lruv_critical_section_scope scope;
        critical_section_scope_construct(&scope, cache->critical_section);
        s_lruv_block *block = lruv_cache_get_block(cache, block_index);
        if (hole->previous_block_index == -1)
        {
            if (cache->first_block_index == -1)
            {
                block->previous_block_index = -1;
                cache->last_block_index = block_index;
            }
            else
            {
                s_lruv_block *next_block = lruv_cache_get_block(cache, cache->first_block_index);
                block->previous_block_index = -1;
                next_block->previous_block_index = block_index;
            }
        }
        else
        {
            s_lruv_block *previous_block = lruv_cache_get_block(cache, hole->previous_block_index);
            if (previous_block->next_block_index == -1)
            {
                block->previous_block_index = cache->last_block_index;
                cache->last_block_index = block_index;
            }
            else
            {
                s_lruv_block *next_block = lruv_cache_get_block(cache, previous_block->next_block_index);
                block->previous_block_index = next_block->previous_block_index;
                next_block->previous_block_index = block_index;
            }
        }
        if (hole->previous_block_index == -1)
        {
            block->next_block_index = cache->first_block_index;
            cache->first_block_index = block_index;
        }
        else
        {
            s_lruv_block *previous_block = lruv_cache_get_block(cache, hole->previous_block_index);
            block->next_block_index = previous_block->next_block_index;
            previous_block->next_block_index = block_index;
        }
        block->page_index = hole->page_index;
        block->page_count = count;
        block->last_used_frame = cache->frame;
        if (scope.entered)
            internal_critical_section_leave(scope.section);
    }
}

static __forceinline bool lruv_cache_block_is_unavailable(s_lruv_cache *cache, s_lruv_block *block, long block_index, long minimum_age)
{
    bool result;
    if ((bool)((block->flags >> 1) & 1))
        result = true;
    else if ((unsigned long)(block->last_used_frame + minimum_age) >= (unsigned long)cache->frame)
        result = true;
    else
        result = cache->procs.locked_proc && cache->procs.locked_proc(cache->procs.client_data, block_index);
    return result;
}

bool lruv_cache_find_hole(
    s_lruv_cache *cache, long minimum_page_count, long minimum_age, s_lruv_cache_hole *out_hole, long *out_block_index, bool *out_complete)
{
    s_lruv_cache_hole holes[256];
    s_lruv_cache_hole best_hole;
    bool found = false;
    bool complete = true;
    long best_block_index = -1;
    long best_frame;
    long block_index = cache->first_block_index;
    long hole_read_index = 0;
    long hole_write_index = 0;
    long previous_block_index = -1;
    long page = 0;

    while ((unsigned long)page < (unsigned long)cache->maximum_page_count)
    {
        long next_write_index = (hole_write_index + 1) % 256;
        if (next_write_index == hole_read_index)
        {
            complete = false;
        }
        else
        {
            s_lruv_cache_hole *hole = &holes[hole_write_index];
            hole->previous_block_index = previous_block_index;
            hole->page_index = page;
            hole->frame = 0;
            hole->priority = 0;
            hole_write_index = next_write_index;
        }
        bool unavailable;
        long page_count;
        long frame;
        if (block_index == -1)
        {
            page_count = (long)cache->maximum_page_count - page;
            page = cache->maximum_page_count;
            frame = 0;
            unavailable = false;
        }
        else
        {
            s_lruv_block *block = lruv_cache_get_block(cache, block_index);
            s_lruv_block *tail_block = lruv_cache_get_block(cache, block_index);
            if ((unsigned long)page == (unsigned long)block->page_index)
            {
                frame = block->last_used_frame;
                page_count = block->page_count;
                unavailable = lruv_cache_block_is_unavailable(cache, block, block_index, minimum_age);
                if (!unavailable && (best_block_index == -1 || (unsigned long)block->last_used_frame < (unsigned long)best_frame))
                {
                    best_frame = tail_block->last_used_frame;
                    best_block_index = block_index;
                }
                previous_block_index = block_index;
                block_index = block->next_block_index;
                page = tail_block->page_count;
                page += block->page_index;
                __dcbt(((unsigned short)block_index) * sizeof(s_lruv_block), cache->header.block_data->data);
            }
            else
            {
                page_count = block->page_index - page;
                page = block->page_index;
                frame = 0;
                unavailable = false;
            }
        }

        if (unavailable)
        {
            hole_read_index = hole_write_index;
        }
        else
        {
            for (long hole_index = hole_read_index; hole_index != hole_write_index; hole_index = (hole_index + 1) % 256)
            {
                s_lruv_cache_hole *hole = &holes[hole_index];
                __dcbt(0, &holes[(hole_index + 1) % 256]);
                if ((unsigned long)frame > hole->frame)
                    hole->frame = frame;
                hole->priority = page_count + hole->priority;
                if (hole->priority >= minimum_page_count)
                {
                    if (!found || lruv_should_use_hole(cache, minimum_page_count, hole, &best_hole))
                    {
                        best_hole = *hole;
                        found = true;
                    }
                    hole_read_index = (hole_read_index + 1) % 256;
                }
            }
        }
    }

    if (found)
    {
        *out_block_index = best_block_index;
        *out_hole = best_hole;
    }
    if (out_complete)
        *out_complete = complete;
    return found;
}

extern "C" void lruv_block_delete(s_lruv_cache *cache, long block_index, bool purge)
{
    long critical_section = cache->critical_section;
    if (critical_section != -1)
        internal_critical_section_enter(critical_section);

    s_lruv_block *block = lruv_cache_get_block(cache, block_index);

    if (cache->procs.delete_proc)
        cache->procs.delete_proc(cache->procs.client_data, block_index, purge);

    if (block->previous_block_index != -1)
        lruv_cache_get_block(cache, block->previous_block_index)->next_block_index = block->next_block_index;
    else
        cache->first_block_index = block->next_block_index;

    if (block->next_block_index != -1)
        lruv_cache_get_block(cache, block->next_block_index)->previous_block_index = block->previous_block_index;
    else
        cache->last_block_index = block->previous_block_index;

    ::datum_delete(cache->header.block_data, block_index);
    internal_critical_section_leave(critical_section);
}

// @address 0x7bbad8 g_rasterizer_text_glyph_cache
// @address 0x93ddf0 g_font_globals

extern "C" void lruv_block_delete_internal(c_font_cache *glyph_font_cache, int hardware_block_index)
{
    s_hardware_character *entry = &g_rasterizer_text_glyph_cache.entries[hardware_block_index];
    if (!g_font_globals.disabled)
        glyph_font_cache->character_data_set_hardware_block_index((e_character_data_index)entry->character_data_index, -1);
    unsigned long entry_key = entry->key;
    short previous_entry_index = entry->previous;
    unsigned short next_entry_index = entry->next;
    if (previous_entry_index != -1)
        g_rasterizer_text_glyph_cache.entries[previous_entry_index].next = next_entry_index;
    else
        ((unsigned short *)g_rasterizer_text_glyph_cache.map)[((entry_key >> 16) + entry_key * 59) & 0x1ff] = next_entry_index;
    short successor_entry_index = entry->next;
    if (successor_entry_index != -1)
        g_rasterizer_text_glyph_cache.entries[successor_entry_index].previous = entry->previous;
    entry->previous = -1;
    entry->next = -1;
    entry->key = 0xffffffff;
    entry->character_data_index = -1;
    entry->font_character = 0;
}

static __forceinline bool lruv_cache_datum_is_free(const s_data_array *data, long index)
{
    bool result = false;
    if (index >= 0 && index < data->maximum_count)
        result = ((const s_datum_header *)(data->data + index * data->size))->identifier == 0;
    return result;
}

extern "C" long lruv_block_new(s_lruv_cache *cache, long datum_index, unsigned long size_in_bytes, long minimum_age)
{
    long critical_section = cache->critical_section;
    if (critical_section != -1)
        internal_critical_section_enter(critical_section);
    long page_count = size_in_bytes >> cache->page_size_bits;
    if ((size_in_bytes & ((1UL << cache->page_size_bits) - 1)) != 0)
        page_count++;
    long result = -1;
    long clamped_age = PIN(minimum_age, 0, 7);
    do
    {
        if (page_count >= cache->smallest_failed_page_counts[clamped_age] && clamped_age > 0)
            break;
        if (datum_index != -1)
        {
            s_data_array *blocks = cache->header.block_data;
            long index = (unsigned short)datum_index;
            bool available = false;
            if (index >= 0 && index < blocks->maximum_count)
                available = (((s_datum_header *)(blocks->data + index * blocks->size))->identifier == 0);
            if (!available)
                break;
        }
        s_lruv_cache_hole hole;
        long block_index = -1;
        bool complete;
        if (lruv_cache_find_hole(cache, page_count, minimum_age, &hole, &block_index, &complete))
            result = lruv_block_new_at_index(cache, datum_index, block_index, &hole, page_count);
        else if (minimum_age == clamped_age)
        {
            long failed_page_count = cache->smallest_failed_page_counts[clamped_age];
            if (failed_page_count > page_count)
                failed_page_count = page_count;
            cache->smallest_failed_page_counts[clamped_age] = failed_page_count;
        }
    } while (false);
    internal_critical_section_leave(critical_section);
    return result;
}


// @address 0x286c78 lruv_block_new_at_page
extern "C" long lruv_block_new_at_page(s_lruv_cache *cache, long datum_index, long page_index, unsigned long size_in_bytes)
{
    long result = -1;
    s_data_array *blocks = cache->header.block_data;
    if (lruv_cache_datum_is_free(blocks, (unsigned short)datum_index))
    {
        long page_count = size_in_bytes >> cache->page_size_bits;
        if (size_in_bytes & ((1UL << cache->page_size_bits) - 1))
            page_count++;
        long end_page = page_index + page_count;
        s_lruv_cache_hole hole;
        if (cache->first_block_index == -1)
        {
            if (page_index >= 0 && (unsigned long)page_index < cache->maximum_page_count && (unsigned long)end_page <= cache->maximum_page_count)
            {
                hole.previous_block_index = -1;
                hole.frame = -1;
                hole.page_index = page_index;
                hole.priority = page_count;
                result = lruv_block_new_at_index(cache, datum_index, -1, &hole, page_count);
            }
        }
        else
        {
            long previous_end = 0;
            long block_index = cache->first_block_index;
            while (block_index != -1)
            {
                s_lruv_block *block = lruv_cache_get_block(cache, block_index);
                long start = block->page_index;
                long end = start + block->page_count;
                block_index = block->next_block_index;
                if ((page_index >= start && page_index < end) || (end_page > start && end_page <= end))
                    break;
                if (page_index >= previous_end && page_index < start && end_page <= start)
                {
                    hole.previous_block_index = block->previous_block_index;
                    hole.frame = -1;
                    hole.page_index = page_index;
                    hole.priority = page_count;
                    result = lruv_block_new_at_index(cache, datum_index, -1, &hole, page_count);
                    break;
                }
                if (block->next_block_index == -1)
                {
                    if ((unsigned long)end <= (unsigned long)page_index && (unsigned long)end_page <= cache->maximum_page_count)
                    {
                        hole.previous_block_index = cache->last_block_index;
                        hole.frame = -1;
                        hole.page_index = page_index;
                        hole.priority = page_count;
                        result = lruv_block_new_at_index(cache, datum_index, -1, &hole, page_count);
                        break;
                    }
                }
                else
                {
                    previous_end = end;
                }
            }
        }
    }
    return result;
}
