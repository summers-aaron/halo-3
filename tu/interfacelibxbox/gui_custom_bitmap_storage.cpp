// interfacelibxbox/gui_custom_bitmap_storage.obj: from the records 0x19ec48 0x21b940 0x221b68 0x221d70 0x221ec0 0x222c78 0x4a6450 0x4a65e0 0x4a6678
// @flags /TP /O2 /Oc /noGL
#include "memory/data_types.h"
#include "interface/gui_custom_bitmap_storage_dependencies.h"
#include <string.h>
#include <stddef.h>
#include "cseries/placement_new.h"
#include "multithreading/synchronization.h"
#include "interface/user_interface_memory.h"
#include "cseries/async.h"
#include "saved_games/game_state_procs.h"
#include "text/font_cache.h"
#include "main/main.h"
#include "cache/cache_file_bulk_reader.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "memory/data.h"
#include "cseries/async_helpers.h"
#include "cseries/async_queue.h"
#include "cache/cache_files_windows_async.h"
#include "networking/tools/network_blf.h"
#include "interface/gui_custom_bitmap_storage_manager.h"
#include "cseries/cseries.h"
#include "interface/gui_custom_bitmap_storage_item.h"
#include "networking/online/online_cached_user.h"
#include "interface/gui_custom_bitmap_storage_queue.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "cseries/async_globals.h"
#include "multithreading/synchronization_types.h"
#include "memory/basic_buffer.h"
#include "memory/lruv_cache_internal.h"
#include "cseries/byte_swapping.h"
#include "networking/tools/blf_authentication_types.h"
#include "cache/cache_file_bulk_reader_state_types.h"
#include "cache/sound_cache_file_reader.h"

struct s_gui_custom_bitmap_storage_callback_task
{
    long (*callback)(long, long);
#ifdef HALO3_64BIT
    char data[500];
#else
    char data[282];
#endif
    short data_size;
};

struct s_custom_bitmap_chunk
{
    long size;
    char data[1];
};

static s_autosave_queue *g_gui_custom_bitmap_storage_manager;

struct s_gui_custom_bitmap_storage_read_task
{
    c_basic_buffer<void> destination;
    s_gui_custom_bitmap_storage_hash expected_hash;
    c_synchronized_long *progress;
    c_synchronized_long *bytes_read;
    bool *success;
    bool hash_expected;
    bool opened;
    c_sound_cache_file_reader reader;
    s_sound_cache_hash_state hash_state;

    __forceinline s_gui_custom_bitmap_storage_read_task(c_basic_buffer<void> destination, void *handle, unsigned long base_offset,
        unsigned long size, const s_gui_custom_bitmap_storage_hash *&hash, bool hash_expected, c_basic_buffer<void> buffer,
        c_sound_cache_hash_source *source, bool *success, c_synchronized_long *progress, c_synchronized_long *bytes_read);
};

static __forceinline s_gui_custom_bitmap_storage_hash gui_custom_bitmap_storage_hash_or_empty(const s_gui_custom_bitmap_storage_hash *&hash)
{
    s_gui_custom_bitmap_storage_hash empty_hash;
    if (!hash)
    {
        memset(&empty_hash, 0, sizeof(empty_hash));
        hash = &empty_hash;
    }
    return *hash;
}

__forceinline s_gui_custom_bitmap_storage_read_task::s_gui_custom_bitmap_storage_read_task(
    c_basic_buffer<void> destination, void *handle, unsigned long base_offset,
    unsigned long size, const s_gui_custom_bitmap_storage_hash *&hash, bool hash_expected, c_basic_buffer<void> buffer,
    c_sound_cache_hash_source *source, bool *success, c_synchronized_long *progress, c_synchronized_long *bytes_read) :
    destination(destination), expected_hash(gui_custom_bitmap_storage_hash_or_empty(hash)), progress(progress), bytes_read(bytes_read),
    success(success), hash_expected(hash_expected), opened(false), reader(handle, base_offset, size)
{
    cache_file_bulk_reader_initialize_hash_state(&hash_state, size, buffer, source, hash_expected);
}

// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z

// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x4ef838 ?initialize@c_gui_custom_bitmap_storage_item@@QAA_NJJ_N@Z

#ifdef HALO3_64BIT
static_assert(offsetof(gui_custom_bitmap_storage_slot, item) == 0xc,
    "the storage item must start at byte 12 of the UI's 148-byte datum");
#endif

// @address 0x4a6450 ?allocate_bitmap@c_gui_custom_bitmap_storage_manager@@QAAJJJ_N@Z
long c_gui_custom_bitmap_storage_manager::allocate_bitmap(long width, long height, bool use_compressed_format)
{
    internal_critical_section_enter(0x14);
    unsigned long index = datum_new(m_bitmap_data);
    if (index != 0xffffffff)
    {
#ifdef HALO3_64BIT
        gui_custom_bitmap_storage_slot *slot = (gui_custom_bitmap_storage_slot *)(
            m_bitmap_data->data + (index & 0xffff) * m_bitmap_data->element_size);
#else
        gui_custom_bitmap_storage_slot *slot = (gui_custom_bitmap_storage_slot *)((m_bitmap_data)->data + (index & 0xffff) * 0x94);
#endif
        slot->reference_count = 1;
        c_gui_custom_bitmap_storage_item *item = &slot->item;
        if (!item->initialize(width, height, use_compressed_format))
        {
            gui_custom_bitmap_storage_release(&m_bitmap_data, index);
            index = 0xffffffff;
        }
    }
    internal_critical_section_leave(0x14);
    return index;
}

// @address 0x4a65e0 gui_custom_bitmap_storage_release
extern "C" void gui_custom_bitmap_storage_release(s_data_array **array_reference, unsigned long index)
{
    internal_critical_section_enter(0x14);
    s_data_array *array = *array_reference;
    if (index != 0xffffffff && (long)(index & 0xffff) < array->first_unallocated) {
        gui_custom_bitmap_storage_slot *slot = (gui_custom_bitmap_storage_slot *)(array->data + array->size * (index & 0xffff));
        if (slot->identifier != 0 && (int)slot->identifier == (int)index >> 16)
            slot->reference_count = slot->reference_count - 1;
    }
    internal_critical_section_leave(0x14);
}

// @address 0x4a6678 gui_custom_bitmap_storage_try_and_get
extern "C" c_gui_custom_bitmap_storage_item *gui_custom_bitmap_storage_try_and_get(s_data_array **array_reference, unsigned long index)
{
    internal_critical_section_enter(0x14);
    s_data_array *array = *array_reference;
    c_gui_custom_bitmap_storage_item *result = 0;
    if (index != 0xffffffff && (long)(index & 0xffff) < array->first_unallocated) {
        gui_custom_bitmap_storage_slot *slot = (gui_custom_bitmap_storage_slot *)(array->data + array->size * (index & 0xffff));
        if (slot->identifier != 0 &&
            (int)slot->identifier == (int)index >> 16 &&
            slot->reference_count > 0 &&
            slot->state == 2)
        {
            result = &slot->item;
        }
    }
    internal_critical_section_leave(0x14);
    return result;
}

// @address 0x221d70 gui_custom_bitmap_storage_get_manager
extern "C" s_autosave_queue *gui_custom_bitmap_storage_get_manager(long controller_index)
{
    s_autosave_queue * result = 0;
    if (online_cached_user_can_use_file_share(controller_index))
    {
        result = g_gui_custom_bitmap_storage_manager;
        if (!result)
        {
            result = new (user_interface_malloc_tracked(sizeof(s_autosave_queue), 0, 0)) s_autosave_queue;
            if (result)
            {
                g_gui_custom_bitmap_storage_manager = result;
                volatile s_autosave_queue *queue = result;
                queue->item_count = 0;
                queue->busy = 0;
                queue->task_id = -1;
                queue->lock.value = 0;
                queue->task_done.value = 1;
                queue->controller_index = controller_index;
                queue->catalogue_enumerated = false;
                queue->content_item_index = -1;
                queue->reference_count++;
            }
        }
        else if (result->controller_index == controller_index)
        {
            result->reference_count++;
        }
    }
    return result;
}

static __forceinline s_async_queue_element *gui_custom_bitmap_storage_free_list_get_and_remove(bool block)
{
    s_async_queue_element *element = 0;
    while (!element)
    {
        internal_mutex_take(0);
        if (g_async_globals.free_list)
        {
            element = g_async_globals.free_list;
            g_async_globals.free_list = g_async_globals.free_list->next;
        }
        if (element)
            element->next = 0;
        _InterlockedDecrement(&g_synch_globals.mutexes[0].lock_count.value);
        ReleaseMutex(g_synch_globals.mutexes[0].handle);
        if (!element)
        {
            if (!block)
                break;
            main_update_idle_tasks();
            SwitchToThread();
        }
    }
    return element;
}

// @address 0x19ec48 ?async_task_add_ex@@YAJW4e_async_priority@@PAUs_async_task@@W4e_async_category@@P6A?AW4e_async_completion@@1@ZPAVc_synchronized_long@@_N@Z
long async_task_add_ex(e_async_priority priority, s_async_task *task, e_async_category category,
    e_async_completion (*work_callback)(s_async_task *), c_synchronized_long *done, bool block)
{
    g_async_globals.tasks_added++;
    done->set(0);

    long task_id = NONE;
    s_async_queue_element *element = gui_custom_bitmap_storage_free_list_get_and_remove(block);
    if (element)
    {
        memcpy(&element->task, task, sizeof(s_async_task));
        element->done = done;
        element->priority = priority;
        element->callback = work_callback;
        element->category = category;
        task_id = async_task_add_ex(element);
    }
    return task_id;
}


// match: 0x21b940: keeps memcpy out-of-line (bl + EXTRN memcpy:PROC); removal inlines the 280B copy and flips li/addi order
#pragma function(memcpy)

// @address 0x21b940 gui_custom_bitmap_storage_queue_async_read
extern "C" long gui_custom_bitmap_storage_queue_async_read(e_async_priority priority, void *handle, unsigned long base_offset,
    unsigned long size, const s_gui_custom_bitmap_storage_hash *expected_hash, c_basic_buffer<void> buffer,
    c_sound_cache_hash_source *source, c_basic_buffer<void> destination, bool *success, c_synchronized_long *progress,
    c_synchronized_long *bytes_read, c_synchronized_long *done)
{
    bool hash_expected = expected_hash != 0;
    s_gui_custom_bitmap_storage_read_task task(
        destination, handle, base_offset, size, expected_hash, hash_expected, buffer, source, success, progress, bytes_read);

    s_gui_custom_bitmap_storage_callback_task callback_task;
    memset(&callback_task, 0, sizeof(callback_task));
    callback_task.callback = (long (*)(long, long))cache_file_bulk_reader_handle_async_read;
    memcpy(callback_task.data, &task, sizeof(task));
    callback_task.data_size = sizeof(task);
    return async_task_add_ex(priority, (s_async_task *)&callback_task, _async_category_none, async_simple_callback_task_callback, done, true);
}

// @address 0x222c78 gui_custom_bitmap_storage_load_chunk_into_widget
extern "C" bool gui_custom_bitmap_storage_load_chunk_into_widget(
    s_autosave_queue *manager, c_gui_custom_bitmap_widget *widget, bool use_compressed_format, long aspect_index)
{
    bool success = false;
    if (widget && manager->selection_valid)
    {
        if (manager->selected_index >= 0 && manager->selected_index < NUMBEROF(manager->items))
        {
            long chunk_size = 0;
            const s_custom_bitmap_chunk *chunk = (const s_custom_bitmap_chunk *)network_blf_find_chunk_in_file(
                (const char *)manager->buffer, manager->buffer_size, 'scnd', 1, _blf_file_authentication_none, (unsigned long *)&chunk_size);
#if !defined(_XBOX) || defined(__clang__)
            // the chunk payload's size is big-endian file data, which a little-endian host must swap
            const long chunk_data_size = chunk ? (long)byte_swap_long((unsigned long)chunk->size) : 0;
            if (chunk && chunk_size - 4 == chunk_data_size &&
                gui_custom_bitmap_widget_load_bitmap_from_buffer(widget, use_compressed_format, chunk->data, chunk_data_size, aspect_index))
#else
            if (chunk && chunk_size - 4 == chunk->size &&
                gui_custom_bitmap_widget_load_bitmap_from_buffer(widget, use_compressed_format, chunk->data, chunk->size, aspect_index))
#endif
                success = true;
            if (!success)
            {
                manager->items[manager->selected_index].mark_bitmap_load_failed();
                manager->selection_valid = false;
            }
        }
    }
    return success;
}

// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z
// @address 0x221ec0 gui_custom_bitmap_storage_release_manager
extern "C" void gui_custom_bitmap_storage_release_manager(void)
{
    if (--g_gui_custom_bitmap_storage_manager->reference_count == 0)
    {
        s_autosave_queue *manager = g_gui_custom_bitmap_storage_manager;
        if (manager->task_id != NONE)
        {
            async_block_until_done(&manager->task_done, simple_yield_function, 1, 0, 0, 0);
            manager->task_id = NONE;
        }
        if (manager->buffer)
        {
            game_state_call_after_load_procs(2, manager->buffer);
            manager->buffer = 0;
            manager->initialized = false;
        }
        user_interface_free(g_gui_custom_bitmap_storage_manager);
        g_gui_custom_bitmap_storage_manager = 0;
    }
}

// @address 0x221b68 gui_custom_bitmap_storage_reset
extern "C" void gui_custom_bitmap_storage_reset(void)
{
    s_autosave_queue *manager = g_gui_custom_bitmap_storage_manager;
    async_block_until_done(&manager->task_done, simple_yield_function, 1, 0, 0, 0);
    autosave_queue_finish_task(manager);
    manager->selected_index = manager->loading_item_index;
    manager->selection_valid = false;
    if (manager->buffer)
    {
        game_state_call_after_load_procs(2, manager->buffer);
        manager->buffer = 0;
    }
    autosave_queue_clear_entries(manager);
    manager->initialized = false;
}
