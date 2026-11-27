#include "text/font_cache_state.h"
#include "memory/runtime_state_allocation.h"
#include "cseries/atomic_intrinsics.h"
#include "cseries/async_block.h"
// blamlibxbox/font_cache.obj: from the records 0x847d0 0x189960 0x18aeb8 0x18e8c0 0x18ea38 0x1a01f0 0x1a0278 0x1a0320 0x1a0940 0x1a09a0 0x1a0a10 0x1f0ec0 0x1f0fd0 0x1f10e0 0x1f1260 0x1f1358 0x1f13c0 0x1f1428 0x1f14a0 0x1f14e0 0x1f15c0 0x1f16a0 0x1f16f0 0x1f16f8 0x1f1780 0x1f18c8 0x1f1960 0x1f19c8 0x1f19d8 0x1f19f0 0x1f1cb0 0x1f1e08 0x1f1f00 0x1f1fa8 0x1f2208 0x1f2428 0x1f2650 0x1f2678 0x1f2c60 0x70b8b0 0x70b8b8
// @flags /TP /O2 /Oc
// @address 0x11df6f4 g_font_cache_globals
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z
// @address 0x189960 ??0c_font_cache@@QAA@XZ
// @address 0x18e8c0 font_cache_lock
// @address 0x18ea38 font_cache_unlock
// @address 0x1a01f0 font_cache_initialize
// @address 0x1f0fd0 ?font_cache_delete@@YAXXZ
// @address 0x1f0ec0 font_cache_initialize_pixels
// @address 0x1f1358 ?character_data_get_hardware_block_index@c_font_cache@@UAAJW4e_character_data_index@@@Z
// @address 0x1f13c0 ?character_data_set_hardware_block_index@c_font_cache@@UAAXW4e_character_data_index@@J@Z
// @address 0x1f15c0 ?character_data_initialize@@YAXXZ
// @address 0x1f16f0 font_cache_lru_pixel_is_locked
// @address 0x1f18c8 ?character_data_free@@YAXPAVc_font_cache@@W4e_character_data_index@@@Z
// @address 0x1f16f8 character_data_find
// @address 0x1f1960 font_cache_pixel_block_is_locked
// @address 0x1f19c8 font_cache_compute_hash
// @address 0x1f19d8 font_cache_test_keys_equal
// @address 0x1f1e08 font_cache_initialize_channels
// @address 0x1f2650 font_cache_compare_range
// @address 0x1f2678 font_cache_compare_entry
// @address 0x1f2c60 font_cache_is_metrics_valid
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x2865e0 ?lruv_new@@YAPAUs_lruv_cache@@PBDJJJPAXP6AX1J_N@ZP6A_N1J@ZP6AE1J@ZPAVc_allocation_base@@J@Z
// @address 0x2867b8 ?lruv_delete@@YAXPAUs_lruv_cache@@@Z
#include "text/font_id_types.h"
#include "rasterizer/rasterizer_text_glyph_cache_types.h"
#include <xtl.h>
#include <ppcintrinsics.h>
#include "memory/data.h"
#include "memory/datum_index.h"
#include <stdlib.h>
#include <string.h>
#include "multithreading/synchronization.h"
#include "cseries/runtime_state.h"
#include "cseries/sort.h"
#include "main/game_preferences.h"
#include "memory/hashtable.h"
#include "memory/hashtable_reset.h"
#include "memory/lruv_cache.h"
#include "memory/lruv_cache_inline.h"
#include "tag_files/files_windows.h"
#include "text/font_cache.h"
#include "text/font_group.h"
#include "memory/byte_swapping.h"
#include "cseries/async.h"
#include "cseries/async_helpers.h"
#include "multithreading/synchronized_value.h"
#include "tag_files/file_reference.h"
#include "rasterizer/rasterizer_text.h"
#include "memory/byte_swapping_types.h"
#include "text/font_cache_globals_types.h"
#include "interface/chud/utf32_types.h"
#include "main/main_halt.h"

enum e_character_data_index;
class c_font_cache;
enum e_font_index;
struct s_font_cache_character_metrics;
struct s_hash_table_bucket;
struct s_hash_table;
#include "memory/hashtable_types.h"

#include "memory/lruv_cache_types.h"
#include "memory/lruv_block_types.h"
#include "multithreading/synchronization_types.h"
#include "text/font_cache_file_types.h"
#include "text/font_cache_thread_state_types.h"
#include "text/font_cache_internal_types.h"
#include "text/font_cache_types.h"
struct s_data_array;
struct s_lruv_cache;

// @address 0x93ddf0 g_font_globals
struct s_synchronization_globals;



#include "text/font_character_types.h"

struct s_font_cache_out { long *ptr; };

#include "text/font_cache_character_types.h"



struct s_font_cache_swap_storage
{
    font_cache_swap_definition page;
    font_cache_swap_definition entry;
    font_cache_swap_definition metrics;
};

struct s_font_cache_predict_entry
{
    s_datum_header datum;
    unsigned short reserved;
    union
    {
        long key;
        struct
        {
            unsigned short font;
            unsigned short character;
        };
    };
};
struct s_font_cache_predict_state
{
    s_font_cache_predict_entry *element;
    s_data_iterator iterator;
};

void internal_critical_section_leave(long index)
{
    if (index != -1) {
        _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    }
}

extern "C" s_font_cache_thread_state *font_cache_lock(s_font_cache_thread_state *state)
{
    state->active = 0;
    if (g_font_cache_globals == 0) {
        internal_critical_section_enter(37);
        state->active = 1;
        g_font_cache_globals = &g_internal_font_cache_globals;
    }
    return state;
}

extern "C" void font_cache_unlock(s_font_cache_thread_state *state)
{
    if (state->active != 0) {
        g_font_cache_globals = 0;
        _InterlockedDecrement(&g_synch_globals.critical_sections[37].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[37].critical_section);
        state->active = 0;
    }
}


namespace {
static __forceinline void font_cache_pixels_delete()
{
    if (g_font_cache_globals->pixel_lru) {
        lruv_delete(g_font_cache_globals->pixel_lru);
        g_font_cache_globals->pixel_lru = 0;
    }
    if (g_font_cache_globals->pixels) {
        RtlFreeHeap(GetProcessHeap(), 0, g_font_cache_globals->pixels);
        g_font_cache_globals->pixels = 0;
    }
}
static __forceinline void font_cache_characters_delete()
{
    if (g_font_cache_globals->hash_table.created())
        g_font_cache_globals->hash_table.dispose();
    if (g_font_cache_globals->characters) {
        data_dispose(g_font_cache_globals->characters);
        g_font_cache_globals->characters = 0;
    }
}
}

__forceinline s_font_cache_globals::~s_font_cache_globals()
{
    hash_table_dispose(hash_table.m_hash_table);
}

// @address 0x74b80c g_runtime_state_allocation

extern "C" void font_cache_initialize_pixels(void)
{
    c_font_cache_scope_lock lock;

    if (!g_font_globals.disabled)
    {
        character_data_initialize();
        g_font_cache_globals->pixels = (char *)::runtime_state_malloc((char const *)(""), (char const *)(""), 0x14000);
        g_font_cache_globals->pixel_lru = (s_lruv_cache *)lruv_new("font character data lruv", 0x2800, 3, 0x320, 0,
            (void (*)(void *, long, bool))character_data_free_callback, (bool (*)(void *, long))font_cache_lru_pixel_is_locked, 0,
            &g_runtime_state_allocation, 37);
        g_font_cache_globals->pixel_lru->hole_algorithm = 0;
    }

    g_font_cache_initialized.set(1);
}

// the scope lock's constructor is inlined here; its destructor is font_cache_unlock
void font_cache_delete()
{
    c_font_cache_scope_lock lock;
    g_font_cache_initialized.set(0);
    font_cache_pixels_delete();
    font_cache_characters_delete();
}


// @address 0x18aeb8 ?character_data_touch@c_font_cache@@UAAXW4e_character_data_index@@@Z
// @address 0x1f1428 ?get_pixels@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@PBUs_font_character@@PAG@Z
// @address 0x1f14a0 ?get_character@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@_NPAKPAJPAPBUs_font_character@@@Z


c_font_cache::c_font_cache()
{
}

bool c_font_cache::get_pixels(e_font_id font, utf32 character, const s_font_character *font_character, unsigned short *pixels)
{
    bool result = false;
    const void *pixel_data = font_character + 1;
    if (pixel_data != 0)
    {
        long size = (long)font_character->height * (long)font_character->width;
        if (font_unpack_character_pixels(font_character->packed_size, pixel_data, 0x4000, pixels) == size)
            result = true;
    }
    return result;
}


extern "C" void font_cache_initialize(void)
{
    if (g_font_globals.initialized == 0) {
        memset(&g_font_globals, 0, sizeof(g_font_globals));
        g_font_globals.language = -1;
        font_cache_initialize_pixels();
        font_cache_initialize_channels();
        g_font_globals.initialized = 1;
    }
    c_font_cache cache;
    g_font_globals.disabled = 1;
}


// @address 0x1a0940 font_cache_open_file
extern "C" bool font_cache_open_file(bool block)
{
    char name[256];
    font_group_get_package_name((e_language)g_font_globals.language, name, sizeof(name));
    font_cache_file_load(&g_font_globals.file, -1, name, block);
    return true;
}

// @address 0x1a09a0 font_cache_close_file
extern "C" void font_cache_close_file(void)
{
    font_cache_file_block_until_finished(&g_font_globals.file);
    if (g_font_globals.file.open)
        file_close(&g_font_globals.file.reference);
    memset(&g_font_globals.file, 0, sizeof(g_font_globals.file));
    memset(&g_font_globals.file, 0, sizeof(g_font_globals.file));
    g_font_globals.package = 0;
}
// @address 0x1a0320 font_cache_reset
extern "C" void font_cache_reset(void)
{
    c_font_cache cache;
    if (global_preferences_get_font_language() != -1) {
        global_preferences_set_font_language(-1);
        global_preferences_flush();
    }
    font_cache_flush();
    font_cache_reset_channels();
    font_cache_close_file();
    font_group_initialize_language();
    cache_files_copy_fonts();
    font_cache_open_file(true);
}

// @address 0x1a0a10 font_cache_handle_failure
extern "C" void font_cache_handle_failure(void)
{
    bool failed = g_font_globals.has_failed;
    if (g_font_globals.file.done.peek() != 0 && g_font_globals.file.reserved_140.peek() != 0 && !failed)
        failed = true;
    if (failed) {
        if (g_font_globals.use_cached_fonts) {
            if (global_preferences_get_font_language() != -1) {
                global_preferences_set_font_language(-1);
                global_preferences_flush();
            }
            g_font_globals.use_cached_fonts = false;
        }
        if (g_font_globals.failure_count >= 3) {
            if (!g_font_globals.has_failed)
                g_font_globals.has_failed = true;
            main_halt_and_display_errors();
        }
        g_font_globals.failure_count++;
        font_cache_reset();
    }
}


// @address 0x1a0278 font_cache_update
extern "C" void font_cache_update(void)
{
    if (g_font_globals.initialized && !g_font_globals.disabled) {
        font_cache_handle_failure();
        font_cache_idle();
        if (g_font_cache_channels.active.peek() != 0 && !g_font_globals.disabled) {
            s_font_cache_thread_state state;
            font_cache_lock(&state);
            g_font_cache_channels.reserved++;
            unsigned int remaining = 8;
            s_font_cache_channel *channel = g_font_cache_channels.channels;
            do {
                font_cache_update_channel(0, channel);
                --remaining;
                ++channel;
            } while (remaining != 0);
            font_cache_unlock(&state);
        }
    }
}

long c_font_cache::character_data_get_hardware_block_index(e_character_data_index character_data_index)
{
    long result = -1;
    if (character_data_index != -1 && !g_font_globals.disabled) {
        s_lruv_cache *cache = g_font_cache_globals->pixel_lru;
        s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, character_data_index);
        result = ((font_cache_character_data *)(g_font_cache_globals->pixels + (block->page_index << cache->page_size_bits)))->hardware_block_index;
    }
    return result;
}

void c_font_cache::character_data_set_hardware_block_index(e_character_data_index character_data_index, long hardware_block_index)
{
    if (character_data_index != -1 && !g_font_globals.disabled) {
        s_lruv_cache *cache = g_font_cache_globals->pixel_lru;
        s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, character_data_index);
        ((font_cache_character_data *)(g_font_cache_globals->pixels + (block->page_index << cache->page_size_bits)))->hardware_block_index =
            hardware_block_index;
    }
}

void c_font_cache::character_data_touch(e_character_data_index handle_enum)
{
    long handle = (long)handle_enum;
    if (handle != -1 && !g_font_globals.disabled) {
        s_font_cache_globals *globals = g_font_cache_globals;
        s_lruv_cache *cache = globals->pixel_lru;
        s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, handle);
        ((font_cache_character_data *)(globals->pixels + (block->page_index << cache->page_size_bits)))->frame_index = globals->flags;
        lruv_block_touch(globals->pixel_lru, handle);
    }
}

// @address 0x1f15c0 ?character_data_initialize@@YAXXZ
void character_data_initialize(void)
{
    s_font_cache_globals *globals = g_font_cache_globals;
    globals->characters = data_new("predicted data", 200, 8, 0, &g_runtime_state_allocation);
    s_hash_table *table = hash_table_new("font character hash table", sizeof(e_character_data_index), 0x640, 0x320,
        font_cache_compute_hash, font_cache_test_keys_equal, &g_runtime_state_allocation);
    s_font_cache_globals *current = g_font_cache_globals;
    globals->hash_table.m_hash_table = table;
    data_make_valid(current->characters);
}


extern "C" unsigned char font_cache_lru_pixel_is_locked(long unused, unsigned long handle)
{
    return font_cache_pixel_block_is_locked(handle);
}

extern "C" unsigned char font_cache_pixel_block_is_locked(unsigned long handle)
{
    s_font_cache_globals *globals = g_font_cache_globals;
    s_lruv_cache *cache = globals->pixel_lru;
    s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, handle);
    long value = ((font_cache_character_data *)(globals->pixels + (block->page_index << cache->page_size_bits)))->frame_index;
    return (unsigned char)((globals->flags - value) > 1 ? 0 : 1);
}


extern "C" long character_data_find(long font_character)
{
    long local[2];
    local[0] = -1;
    long result = -1;
    s_font_cache_out out;
    out.ptr = &local[1];
    s_hash_table *table = g_font_cache_globals->hash_table.m_hash_table;
    s_hash_table_bucket *node = ::hash_table_find_internal(table, &font_character);
    if (node != 0 && out.ptr - 1 != 0) {
        memcpy(out.ptr - 1, node + 1, table->entry_size);
        result = local[0];
    }
    return result;
}



// @address 0x7bbad8 g_rasterizer_text_glyph_cache
// Zeroed except handle, which starts at -1.
s_hardware_character_cache_globals g_rasterizer_text_glyph_cache =
{
    false, 0, 0, 0, 0, 0, 0, -1
};

void character_data_free(c_font_cache *font_cache, e_character_data_index character_data_index)
{
    s_lruv_cache *cache = g_font_cache_globals->pixel_lru;
    s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, character_data_index);
    char *pixel = g_font_cache_globals->pixels + (block->page_index << cache->page_size_bits);
    long handle = ((font_cache_character_data *)pixel)->hardware_block_index;
    if (handle != -1 && g_rasterizer_text_glyph_cache.entries[handle].key != 0xffffffff)
        lruv_block_delete_internal(font_cache, handle);
    ::hash_table_remove(g_font_cache_globals->hash_table.m_hash_table, (void const *)(pixel));
}

// @address 0x1f16a0 character_data_free_callback
extern "C" void character_data_free_callback(void *client_data, long character_data_index)
{
    c_font_cache cache;
    character_data_free(&cache, (e_character_data_index)character_data_index);
}


// @address 0x1f1780 ?character_data_allocate@@YA?AW4e_character_data_index@@PAVc_font_cache@@W4e_font_index@@PAUs_font_cache_character_metrics@@JPBX@Z
enum e_character_data_index character_data_allocate(class c_font_cache *font_cache, enum e_font_index character_key,
    struct s_font_cache_character_metrics *metrics, long pixel_count, void const *pixels)
{
    long result = character_data_find(((long)character_key));
    if (result == -1) {
        long allocated = lruv_block_new((s_lruv_cache *)g_font_cache_globals->pixel_lru, -1,
            pixel_count + sizeof(font_cache_character_data), 1);
        result = allocated;
        if (allocated != -1) {
            s_lruv_cache *cache = g_font_cache_globals->pixel_lru;
            s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, allocated);
            char *pixel = g_font_cache_globals->pixels + (block->page_index << cache->page_size_bits);
            long flags = g_font_cache_globals->flags;
            memset(pixel + sizeof(short), 0, sizeof(font_cache_character_data) - sizeof(short));
            ((font_cache_character_data *)pixel)->key = ((long)character_key);
            ((font_cache_character_data *)pixel)->frame_index = flags;
            ((font_cache_character_data *)pixel)->hardware_block_index = -1;
            if (metrics) {
                ((font_cache_character_data *)pixel)->metrics = *metrics;
                memcpy(pixel + sizeof(font_cache_character_data), pixels, pixel_count);
            }
            if (!hash_table_insert(g_font_cache_globals->hash_table.m_hash_table, pixel, &allocated)) {
                long failed = allocated;
                character_data_free(font_cache, (e_character_data_index)failed);
                lruv_block_delete((s_lruv_cache *)g_font_cache_globals->pixel_lru, failed, false);
                return _character_data_index_none;
            }
            result = allocated;
        }
    }
    return (e_character_data_index)result;
}

static __forceinline bool font_cache_pixel_block_current(long block_index)
{
    s_font_cache_globals *globals = g_font_cache_globals;
    s_lruv_cache *cache = globals->pixel_lru;
    s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, block_index);
    long age = ((font_cache_character_data *)(globals->pixels + (block->page_index << cache->page_size_bits)))->frame_index;
    return (globals->flags - age) > 1 ? false : true;
}

// @address 0x1f10e0 font_cache_flush
extern "C" void font_cache_flush(void)
{
    if (g_font_cache_initialized.peek() != 0 && !g_font_globals.disabled) {
        c_font_cache font_cache;
        c_data_iterator<void> iterator;
        data_iterator_begin(&iterator.m_iterator, g_font_cache_globals->characters);
        while (iterator.next())
            datum_delete(g_font_cache_globals->characters, iterator.get_index());
        font_cache_idle();
        font_cache_idle();
        s_data_iterator block_iterator;
        long block_index;
        data_iterator_begin(&block_iterator, (s_data_array *)g_font_cache_globals->pixel_lru->header.block_data);
        data_iterator_next(&block_iterator);
        block_index = block_iterator.index;
        while (block_index != -1) {
            if (!font_cache_pixel_block_current(block_index)) {
                character_data_free(&font_cache, (e_character_data_index)block_index);
                lruv_block_delete((s_lruv_cache *)g_font_cache_globals->pixel_lru, block_index, false);
            }
            data_iterator_next(&block_iterator);
            block_index = block_iterator.index;
        }
        font_cache_reset_channels();
        rasterizer_text_flush_all_glyphs();
    }
}

extern "C" unsigned long font_cache_compute_hash(void *character_key)
{
    return _rotl(*(unsigned short *)(character_key), 2);
}

extern "C" unsigned char font_cache_test_keys_equal(void *first_key, void *second_key)
{
    return *(int *)first_key == *(int *)second_key;
}

extern "C" void font_cache_initialize_channels(void)
{
    c_font_cache_scope_lock lock;
    g_font_cache_channels.reserved = 0;
    unsigned int remaining = 8;
    s_font_cache_channel *channel = g_font_cache_channels.channels;
    do {
        channel->handle = -1;
        channel->state = 0;
        channel->pending = -1;
        channel->read_count.set(0);
        channel->write_count.set(0);
        --remaining;
        channel->size = 0;
        ++channel;
    } while (remaining != 0);
    g_font_cache_channels.active.set(1);
}

static __forceinline bool font_cache_get_file_handle(void **file)
{
    bool result = false;
    if (g_font_globals.file.open != 0) {
        void *handle = g_font_globals.file.reference.handle;
        *file = handle;
        result = handle != 0;
    }
    return result;
}

// @address 0x1f2208 font_cache_update_channel
extern "C" void font_cache_update_channel(unsigned char flag, s_font_cache_channel *channel)
{
    c_synchronized_long *done = &channel->write_count;
    bool finished = done->peek() != 0;
    if (channel->handle != -1) {
        if (channel->pending == -1 && !finished) {
            unsigned long offset = (channel->handle + 1) * 0x8000;
            long priority = flag ? _async_priority_blocking_below_sound : _async_priority_blocking_generic;
            void *file;
            if (font_cache_get_file_handle(&file)) {
                channel->pending = async_read_position(
                    file, channel->data, 0x8000, offset, 7, priority, &channel->read_count, done);
                if (channel->pending != -1) {
                    channel->size = 2;
                }
                else {
                    done->set(1);
                    finished = true;
                }
            }
            else {
                done->set(1);
                finished = true;
            }
        }
        if (flag && channel->pending != -1) {
            async_task_change_priority(channel->pending, _async_priority_blocking_below_sound);
            async_block_until_done(done, simple_yield_function, 0, 0, 0, 4);
            finished = true;
        }
        if (channel->pending != -1 && finished) {
            long bytes_read = channel->read_count.peek();
            channel->pending = -1;
            if (bytes_read == 0x8000) {
                if (font_cache_is_page_valid(channel->data)) {
                    channel->size = 3;
                    channel->read_complete_frame = g_font_cache_channels.reserved;
                }
                else {
                    channel->size = 0;
                    done->set(1);
                }
            }
            else {
                channel->size = 0;
                done->set(1);
            }
        }
    }
}

// @address 0x1f1f00 font_cache_reset_channels
extern "C" void font_cache_reset_channels(void)
{
    c_font_cache_scope_lock lock;
    unsigned int remaining = 8;
    s_font_cache_channel *channel = g_font_cache_channels.channels;
    do {
        if (channel->pending != -1)
            font_cache_update_channel(1, channel);
        channel->handle = -1;
        --remaining;
        channel->size = 0;
        ++channel;
    } while (remaining != 0);
}

extern "C" int font_cache_compare_range(unsigned int value, s_font_cache_range *range)
{
    if (value < range->begin) {
        return -1;
    }
    return value > range->end;
}

extern "C" int font_cache_compare_entry(unsigned int value, unsigned int *other)
{
    unsigned int reference = *other;
    if (value < reference) {
        return -1;
    }
    return value > reference;
}


static __forceinline int font_cache_abs(int x) { return x < 0 ? -x : x; }

extern "C" int font_cache_is_metrics_valid(s_font_cache_character_metrics *metrics)
{
    bool valid = true;
    valid &= font_cache_abs(metrics->advance) <= 0x100;
    valid &= metrics->pixel_count != 0 && metrics->pixel_count <= 0x2000;
    valid &= metrics->width > 0 && metrics->width <= 0x100;
    valid &= metrics->height > 0 && metrics->height <= 64;
    valid &= font_cache_abs(metrics->origin_x) < 0x100;
    return valid & (font_cache_abs(metrics->origin_y) < 64);
}

// @address 0x1f1fa8 font_cache_try_and_get_page
extern "C" long font_cache_try_and_get_page(long key, unsigned long flags, long unused, const void **out_data)
{
    c_font_cache_scope_lock lock;
    *out_data = 0;
    s_font_cache_channel *selected = 0;
    unsigned char block = flags & 1;
    long result;
    if (g_font_globals.package && key >= 0 && key < g_font_globals.package->character_count) {
        for (unsigned long i = 0; i < 8; i++) {
            s_font_cache_channel *channel = &g_font_cache_channels.channels[i];
            if (channel->handle == key) {
                selected = channel;
                break;
            }
            bool replace_allowed = ((flags >> 1) & 1) != 0;
            if (replace_allowed) {
                bool available = true;
                if (!block && channel->handle != -1) {
                    if (channel->pending != -1 || (channel->read_complete_frame != -1 && g_font_cache_channels.reserved - channel->read_complete_frame < 2))
                        available = false;
                }
                if (available) {
                    if (!selected || (channel->pending == -1 && selected->pending != -1) || channel->state < selected->state)
                        selected = channel;
                }
            }
        }
        if (selected) {
            if (selected->handle != key) {
                if (selected->pending != -1)
                    font_cache_update_channel(block, selected);
                selected->state = 0;
                selected->handle = -1;
                selected->pending = -1;
                selected->read_count.set(0);
                selected->write_count.set(0);
                selected->size = 0;
                selected->handle = key;
                long frame = g_font_cache_channels.reserved;
                selected->read_complete_frame = -1;
                selected->size = 0;
                selected->creation_frame = frame;
            }
            font_cache_update_channel(block, selected);
            result = selected->size;
            selected->state = g_font_cache_channels.reserved;
            if (result == 3) {
                *out_data = selected->data;
            }
        } else {
            result = 1;
        }
    } else {
        result = 0;
    }
    return result;
}


// @address 0x3946c k_font_character_swap_codes
// @rdata 0x8203946c..0x82039490
static const long k_font_character_swap_codes[] =
{
    _byte_swap_array_start, 1,
        _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte,
    _byte_swap_array_end
};
// @rdata-end

// @address 0x7460a4 k_font_cache_metrics_swap_definition
font_cache_swap_definition k_font_cache_metrics_swap_definition =
{
    { (void *)"s_font_character", sizeof(s_font_cache_character_metrics), (void *)k_font_character_swap_codes },
    'bysw', 0
};

static __forceinline s_font_cache_swap_storage *font_cache_get_swap_storage()
{
#if !defined(_XBOX) || defined(__clang__)
    static const int k_font_cache_page_swap_codes[] =
    {
        _byte_swap_array_start, 1,
            _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte,
        _byte_swap_array_end
    };
    static const int k_font_cache_page_entry_swap_codes[] =
    {
        _byte_swap_array_start, 1,
            _byte_swap_4byte, _byte_swap_2byte, _byte_swap_2byte,
        _byte_swap_array_end
    };
    static s_font_cache_swap_storage storage =
    {
        { { (void *)"s_font_package", sizeof(s_font_cache_page_header), (void *)k_font_cache_page_swap_codes }, 'bysw', 0 },
        { { (void *)"s_font_package_character", sizeof(s_font_cache_page_entry), (void *)k_font_cache_page_entry_swap_codes }, 'bysw', 0 },
        { { (void *)"s_font_character", sizeof(s_font_cache_character_metrics), (void *)k_font_character_swap_codes }, 'bysw', 0 }
    };
    return &storage;
#else
    return CONTAINING_RECORD(&k_font_cache_metrics_swap_definition, s_font_cache_swap_storage, metrics);
#endif
}

static __forceinline void font_cache_swap(element_type_t *type, void *data)
{
    if (data)
        byte_swap_element(type, (char *)data, type->definition, 0, 0);
}

static __forceinline unsigned char font_cache_validate_page(s_font_cache_page_header *page);

// @address 0x1f2428 font_cache_is_page_valid
extern "C" unsigned char font_cache_is_page_valid(void *data)
{
    return font_cache_validate_page((s_font_cache_page_header *)data);
}


static __forceinline long font_cache_read_page(
    long page_index, unsigned long key, unsigned long flags, const s_font_cache_character_metrics **out_character, const void **out_pixels)
{
    const void *data = 0;
    long state = font_cache_try_and_get_page(page_index, flags, key, &data);
    if (state == 3) {
        const s_font_cache_page_header *page = (const s_font_cache_page_header *)data;
        const s_font_cache_page_entry *entries = (const s_font_cache_page_entry *)((const char *)page + page->entries_offset);
        long index = bsearch_elements((const void *)(uintptr_t)key, entries, page->entry_count, 8,
            (long (*)(const void *, const void *, const void *))font_cache_compare_entry, 0);
        if (index != -1) {
            unsigned long offset = entries[index].offset;
            if (offset >= page->data_offset) {
                unsigned long end = page->data_offset + page->data_size;
                if (offset + sizeof(s_font_cache_character_metrics) <= end) {
                    const s_font_cache_character_metrics *character = (const s_font_cache_character_metrics *)((const char *)page + offset);
                    if ((unsigned char)font_cache_is_metrics_valid((s_font_cache_character_metrics *)character) &&
                        offset + sizeof(s_font_cache_character_metrics) + character->pixel_count <= end)
                    {
                        *out_character = character;
                        *out_pixels = character + 1;
                        return 2;
                    }
                }
            }
        }
        return 0;
    }
    return state != 0;
}

// @address 0x1f1cb0 font_cache_try_and_get_metrics
extern "C" long font_cache_try_and_get_metrics(
    unsigned long key, unsigned long flags, const s_font_cache_character_metrics **out_character, const void **out_pixels)
{
    long result = 0;
    if (!g_font_globals.disabled) {
        s_font_package_header *table = g_font_globals.package;
        if (table) {
            const void *ranges = (const char *)table + table->package_size;
            long page_index = bsearch_elements((const void *)(uintptr_t)key, ranges, table->character_count, 8,
                (long (*)(const void *, const void *, const void *))font_cache_compare_range, 0);
            if (page_index != -1) {
                result = font_cache_read_page(page_index, key, flags, out_character, out_pixels);
            }
        }
    }
    return result;
}

// @address 0x1f14e0 ?font_character_predict@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@@Z
bool c_font_cache::font_character_predict(enum e_font_id font_id, struct utf32 character)
{
    c_flags<long, unsigned long, 2> flags;
    flags.set(1, true);
    int result = (int)font_cache_try_and_get_character(this, (long)font_id, (long)character.character, flags, 0, 0, 0);
    if (result == 1)
    {
        unsigned int key = (unsigned int)(character.character & 0xffff) | ((unsigned int)(font_id & 0xffff) << 16);
        s_font_cache_globals *globals = g_font_cache_globals;
        s_font_cache_predict_state state;
        state.iterator.data = globals->characters;
        state.iterator.absolute_index = -1;
        state.iterator.index = -1;
        bool found = false;
        state.element = (s_font_cache_predict_entry *)data_iterator_next(&state.iterator);
        if (state.element != 0)
        {
            do
            {
                if ((unsigned int)state.element->key == key)
                {
                    found = true;
                    break;
                }
                state.element = (s_font_cache_predict_entry *)data_iterator_next(&state.iterator);
            } while (state.element != 0);
        }
        if (found == 0)
        {
            long datum = datum_new(globals->characters);
            if (datum != -1)
            {
                s_data_array *array = globals->characters;
                s_font_cache_predict_entry *entry = datum_get<s_font_cache_predict_entry>(array, datum);
                entry->key = (long)key;
            }
        }
    }
    return result != 1;
}

// @address 0x1f14a0 ?get_character@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@_NPAKPAJPAPBUs_font_character@@@Z
bool c_font_cache::get_character(
    e_font_id font, utf32 character, bool precache, unsigned long *out_key, long *out_index, const s_font_character **out_character)
{
    c_flags<long, unsigned long, 2> flags;
    flags.set(0, precache);
    flags.set(1, true);
    return font_cache_try_and_get_character(this, (long)font, character.character, flags,
        (void *)out_key, (void *)out_index, (void *)out_character) == 2;
}


// @address 0x1f1260 ?font_cache_idle@@YAXXZ
void font_cache_idle()
{
    if (g_font_cache_initialized.peek() != 0 && !g_font_globals.disabled)
    {
        c_font_cache cache;
        lruv_idle((s_lruv_cache *)g_font_cache_globals->pixel_lru);
        ++g_font_cache_globals->flags;
        c_data_iterator<s_font_cache_predict_entry> iterator;
        c_flags<long, unsigned long, 2> flags(6);
        iterator.m_iterator.absolute_index = -1;
        iterator.m_iterator.index = -1;
        iterator.m_iterator.data = g_font_cache_globals->characters;
        while (iterator.next())
        {
            utf32 character = { iterator.get_datum()->key & 0xffff };
            if (font_cache_try_and_get_character(&cache, iterator.get_datum()->font, character.character, flags, 0, 0, 0) != 1)
                datum_delete(g_font_cache_globals->characters, iterator.get_index());
        }
    }
}

static __forceinline unsigned char font_cache_validate_page(s_font_cache_page_header *page)
{
    bool valid = true;
    if (page)
    {
        element_type_t *const &type = &font_cache_get_swap_storage()->page.type;
        byte_swap_element(type, (char *)page, font_cache_get_swap_storage()->page.type.definition, 0, 0);
    }
    bool header_valid = true;
    header_valid &= page->entry_count > 0;
    header_valid &= page->entries_offset >= sizeof(*page) && page->entries_offset + page->entry_count * sizeof(s_font_cache_page_entry) <= 0x8000;
    header_valid &= page->data_size > 0;
    header_valid &= page->data_offset >= sizeof(*page) && page->data_offset + page->data_size <= 0x8000;
    header_valid &= page->data_offset >= page->entries_offset + page->entry_count * sizeof(s_font_cache_page_entry);
    if (header_valid)
    {
        for (long index = 0; index < page->entry_count && valid; ++index)
        {
            s_font_cache_page_entry *entry = (s_font_cache_page_entry *)((char *)page + page->entries_offset) + index;
            if (entry)
            {
                element_type_t *const &type = &font_cache_get_swap_storage()->entry.type;
                byte_swap_element(type, (char *)entry, font_cache_get_swap_storage()->entry.type.definition, 0, 0);
            }
            long font_index = entry->font_index;
            bool entry_valid = true;
            entry_valid &= font_index >= 0 && font_index <= 15;
            utf32 character = { entry->character & 0xffff };
            entry_valid &= character.character >= 0 && character.character <= 0xffff;
            unsigned long offset = entry->offset;
            entry_valid &= offset >= page->data_offset &&
                offset + sizeof(s_font_cache_character_metrics) <= (unsigned long)(page->data_offset + page->data_size);
            if (entry_valid)
            {
                s_font_cache_character_metrics *metrics = (s_font_cache_character_metrics *)((char *)page + offset);
                font_cache_swap(&font_cache_get_swap_storage()->metrics.type, metrics);
                if (!(unsigned char)font_cache_is_metrics_valid(metrics) || metrics->pixel_count == 0 ||
                    (long)offset + metrics->pixel_count > page->data_offset + page->data_size)
                    valid = false;
            }
            else
                valid = false;
        }
    }
    else
        valid = false;
    return valid;
}

static __forceinline font_cache_character_data *font_cache_character_data_get(long index)
{
    s_lruv_cache *cache = g_font_cache_globals->pixel_lru;
    s_lruv_block *block = datum_get<s_lruv_block>((s_data_array *)cache->header.block_data, index);
    return (font_cache_character_data *)(g_font_cache_globals->pixels + (block->page_index << cache->page_size_bits));
}

// @address 0x1f19f0 font_cache_try_and_get_character
extern "C" long font_cache_try_and_get_character(void *font_cache, long font_id, long character, c_flags<long, unsigned long, 2> flags,
    void *out_key, void *out_index, void *out_character)
{
    long font_index = -1;
    if (g_font_globals.package && font_id >= 0 && (unsigned long)font_id < 16)
        font_index = g_font_globals.package->font_indices[font_id];

    const s_font_cache_character_metrics *cached_character = 0;
    const void *pixels;
    long index = -1;
    long result = 0;
    if (font_index != -1)
    {
        unsigned long key = ((unsigned long)font_index << 16) | (character & 0xffff);
        if (g_font_globals.disabled)
        {
            result = font_cache_try_and_get_metrics(key, flags.get_unsafe(), &cached_character, &pixels);
        }
        else
        {
            index = character_data_find(key);
            if (index != -1)
            {
                font_cache_character_data *data = font_cache_character_data_get(index);
                if (data->metrics.pixel_count)
                {
                    result = 2;
                    cached_character = &data->metrics;
                }
                else
                    result = 0;
            }
            else if (flags.test(1))
            {
                const s_font_cache_character_metrics *metrics;
                result = font_cache_try_and_get_metrics(key, flags.get_unsafe(), &metrics, &pixels);
                if (result == 0)
                {
                    result = font_cache_try_and_get_metrics(((unsigned long)font_index << 16) | 0x25a1, flags.get_unsafe(), &metrics, &pixels);
                    if (result == 0)
                        result = font_cache_try_and_get_metrics((unsigned long)font_index << 16, flags.get_unsafe(), &metrics, &pixels);
                }
                if (result == 2)
                {
                    index = character_data_allocate((c_font_cache *)font_cache, (e_font_index)key,
                        (s_font_cache_character_metrics *)metrics, metrics->pixel_count, pixels);
                    if (index != -1)
                        cached_character = &font_cache_character_data_get(index)->metrics;
                    else
                        result = 0;
                }
                else if (result == 0)
                    index = character_data_allocate((c_font_cache *)font_cache, (e_font_index)key, 0, 0, 0);
            }
            if (index != -1) {
                font_cache_character_data_get(index)->frame_index = g_font_cache_globals->flags;
                lruv_block_touch(g_font_cache_globals->pixel_lru, index);
            }
        }
        if (result == 2)
        {
            if (out_key)
                *(long *)out_key = key;
            if (out_index)
                *(long *)out_index = index;
            if (out_character)
                *(const s_font_cache_character_metrics **)out_character = cached_character;
        }
    }
    return result;
}

s_font_cache_globals g_internal_font_cache_globals;
extern "C" {
__declspec(thread) s_font_cache_globals *g_font_cache_globals;
}
c_synchronized_long g_font_cache_initialized;
s_font_cache_channel_block g_font_cache_channels;
