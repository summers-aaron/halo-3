#include "main/levels_enumeration_task.h"
// blamlibxbox/autosave_queue.obj: from the records 0x1e5bf0 0x1e5cd0 0x1e5e18 0x1e61c0 0x1e62f8 0x1e64c8 0x2219b8 0x221a38 0x221f68 0x222208 0x2222d8 0x222338 0x222538 0x2225e8 0x2226f8 0x222978 0x222a98 0x228288 0x2283f0 0x2284d0 0x2286f8 0x228790 0x228cc0 0x228f08 0x2290f8 0x4eeb68 0x4eebb0
// @address 0x95e608 main_game_globals
// @flags /TP /O2 /Oc
// @address 0x1aed20 ?file_reference_get_name@@YAPADPBUs_file_reference@@KPADJ@Z
// @address 0x1e81d0 ?saved_game_filename_extension_by_game_file_type@@YAPBDW4e_saved_game_file_type@@_N@Z
// @address 0x1aec40 ?file_reference_create_from_path@@YAPAUs_file_reference@@PAU1@PBD_N@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x1ad6d0 ?file_delete@@YA_NPAUs_file_reference@@@Z
// @address 0x1ae418 ?find_files_start@@YAXPAUs_find_file_data@@KPBUs_file_reference@@PBD@Z
// @address 0x1ae4a0 ?find_files_end@@YAXPAUs_find_file_data@@@Z
// @address 0x1ae500 ?find_files_next@@YA_NPAUs_find_file_data@@PAUs_file_reference@@PAUs_file_last_modification_date@@@Z
// @address 0x1e82e8 ?saved_game_read_metadata_from_file@@YA_NPAUs_file_reference@@PAUs_saved_game_item_metadata_packed@@@Z

#include "saved_games/autosave_queue_types.h"
#include "main/main_game.h"
#include "saved_games/saved_game_collect_files.h"
#include "memory/data_types.h"
#include "cseries/async_helpers.h"
#include "game/game_types.h"
#include <ppcintrinsics.h>
#include <string.h>
#include "multithreading/synchronization.h"
#include "game/game.h"
#include "networking/tools/network_blf.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "tag_files/files_windows_types.h"
#include "game/game_engine_sandbox.h"
#include "main/levels.h"
#include "cache/cache_files.h"
#include "saved_games/autosave_queue.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/content_catalogue_iteration.h"
#include "memory/data.h"
#include "cseries/cseries.h"
#include "cseries/async.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_session_interface_globals.h"
#include "saved_games/buffered_file_writer_types.h"
#include "saved_games/iterator_types.h"
#include "networking/tools/blf_header_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/saved_game_files_io.h"
#include "cseries/crt_xdk.h"
#include "cseries/async_completion.h"
#include "saved_games/game_state_procs.h"
#include "interface/gui_custom_bitmap_storage.h"
#include "interface/gui_selected_items_saved_film.h"
#include "shell/shell_types.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include "main/levels_globals.h"
#include "tag_files/file_reference.h"
#include "interface/controller_types.h"
#include "multithreading/synchronized_value.h"
#include "main/levels_map.h"
#include "networking/tools/blf_authentication_types.h"
#include "saved_games/saved_film_recording_types.h"
#include "networking/tools/blf_end_of_file_types.h"
#include "saved_games/autosave_queue_dependencies.h"
#include "saved_games/content_catalogue_queries.h"
#include "saved_games/content_item_types.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_synchronization_globals_types.h"
#include "saved_games/saved_game_file_types.h"
#include "tag_files/c_file_reference.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "saved_games/game_content_types.h"
#include "game/game_engine_base_variant.h"
#include "text/unicode.h"

// match: 0x2286f8: the queue read calls the out-of-line CRT copy routine.
#pragma function(memcpy)

#pragma pack(push, 4)
struct s_autosave_queue_file_header
{
    s_blf_chunk_start_of_file start;
    s_blf_chunk_content_header content;
    s_blf_chunk_end_of_file end_of_file;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct s_blf_chunk_end_of_file_with_rsa
{
    s_blf_header header;
    long total_file_size;
    unsigned char authentication_type;
    unsigned char rsa_signature[256];
};
#pragma pack(pop)


namespace {
class c_autosave_queue_scope_lock
{
public:
    __forceinline c_autosave_queue_scope_lock()
    {
        internal_critical_section_enter(21);
    }
    __forceinline ~c_autosave_queue_scope_lock()
    {
        s_content_synchronization_globals *globals = (s_content_synchronization_globals *)(&g_synch_globals);
        _InterlockedDecrement(&globals->critical_sections[21].reference_count);
        RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)&globals->critical_sections[21].native_lock);
    }
};
}

struct s_autosave_queue_enumeration_task
{
    long controller_index;
    long state;
    long content_item_index;
    s_find_file_data *find_file_data;
};

enum e_autosave_queue_purge_state
{
    _autosave_queue_purge_state_start = 1,
    _autosave_queue_purge_state_find = 2,
    _autosave_queue_purge_state_done = 3
};

namespace {

struct s_async_autosave_queue_save_item_task
{
    long category;
    s_saved_game_queue *queue;
    bool find_started;
};

} // namespace

struct s_autosave_queue_string
{
    c_static_string<256> text;
};

union s_autosave_queue_level_enumeration_task
{
    s_autosave_queue_enumeration_task enumeration;
    s_async_task async_task;
};

union s_autosave_queue_async_task
{
    s_async_autosave_queue_save_item_task save_item;
    s_async_task async_task;
    char storage[0x120];
};

struct s_autosave_queue_buffered_file_writer {
    s_buffered_file_writer writer;
    char buffer[0x1000];
    __forceinline s_autosave_queue_buffered_file_writer(s_file_reference *file)
    {
        writer.file = file;
        writer.buffer = buffer;
        writer.buffer_size = sizeof(buffer);
        writer.buffer_position = 0;
    }
    __forceinline void flush()
    {
        if (writer.buffer_position > 0)
        {
            file_write(writer.file, writer.buffer_size, writer.buffer);
            writer.buffer_position = 0;
        }
    }
};

typedef char autosave_queue_file_size_offset[
    (offsetof(s_autosave_queue_file_header, end_of_file) +
        offsetof(s_blf_chunk_end_of_file, total_file_size) == 0x144) ? 1 : -1];
typedef char autosave_queue_busy_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, busy) == 0x8cac))) ? 1 : -1];
typedef char autosave_queue_buffer_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, buffer) == 0x8cc8))) ? 1 : -1];
typedef char autosave_queue_buffer_size_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue, buffer_size) == 0x8ccc))) ? 1 : -1];
typedef c_gui_saved_film_subitem_datasource s_autosave_queue_owner;
typedef char saved_game_queue_owner_category_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue_owner, category) == 0x19b9c))) ? 1 : -1];
typedef char saved_game_queue_owner_queue_offset[(sizeof(void *) != 4 || ((offsetof(s_autosave_queue_owner, async) == 0x19ba8))) ? 1 : -1];

// @address 0x1e61c0 levels_delete_flagged_levels
extern "C" void levels_delete_flagged_levels(void)
{
    c_data_iterator<s_datum> iterator;

    iterator.begin(g_level_globals.campaign_levels);
    while (iterator.next()) {
        if (iterator.get_datum()->flags & 1)
            ::datum_delete(g_level_globals.campaign_levels, iterator.get_index());
    }

    iterator.begin(g_level_globals.map_levels);
    while (iterator.next()) {
        if (iterator.get_datum()->flags & 1)
            ::datum_delete(g_level_globals.map_levels, iterator.get_index());
    }

    iterator.begin(g_level_globals.multiplayer_levels);
    while (iterator.next()) {
        if (iterator.get_datum()->flags & 1)
            ::datum_delete(g_level_globals.multiplayer_levels, iterator.get_index());
    }
}

// @address 0x1e62f8 levels_load_campaign_file
extern "C" void levels_load_campaign_file(s_file_reference *file, const char *map_directory, bool downloadable)
{
    bool valid = false;
    unsigned long error;
    if (file_open(file, 1, &error))
    {
        unsigned long file_size;
        char buffer[0x18ac];
        if (file_get_size(file, &file_size) && file_size <= sizeof(buffer) && file_read(file, file_size, false, buffer))
        {
            const s_levels_campaign_source *campaign;
            bool byte_swap;
            const char *chunk_buffer = 0;
            long chunk_size = 0;
            bool eof_found = false;
            if (network_blf_verify_start_of_file(buffer, file_size, &byte_swap, &chunk_size) &&
                network_blf_read_next_known_chunk(buffer, file_size, byte_swap, 'cmpn', 1, &chunk_size, &chunk_buffer, 0, 0, &eof_found))
            {
                campaign = chunk_buffer ? (const s_levels_campaign_source *)(chunk_buffer - sizeof(s_blf_header)) : 0;
                if (campaign &&
                    network_blf_read_next_known_chunk(buffer, file_size, byte_swap, '_eof', 1, &chunk_size,
                        &chunk_buffer, 0, 0, &eof_found) &&
                    chunk_buffer && chunk_size == sizeof(s_blf_chunk_end_of_file_with_rsa) && eof_found &&
                    network_blf_verify_end_of_file(buffer, file_size, byte_swap, chunk_buffer - sizeof(s_blf_header),
                        _blf_file_authentication_signature))
                {
                    valid = true;
                }
            }
            if (valid)
                levels_add_campaign(campaign, byte_swap, map_directory, downloadable);
        }
        file_close(file);
    }
}

// @address 0x1e64c8 levels_load_mapinfo_file
extern "C" void levels_load_mapinfo_file(s_file_reference *file, const char *map_directory, bool downloadable)
{
    bool valid = false;
    unsigned long error;
    if (file_open(file, 1, &error))
    {
        unsigned long file_size;
        char buffer[0x52e4];
        if (file_get_size(file, &file_size) && file_size <= sizeof(buffer) && file_read(file, file_size, false, buffer))
        {
            const char *level;
            bool byte_swap;
            const char *chunk_buffer = 0;
            long chunk_size = 0;
            bool eof_found = false;
            if (network_blf_verify_start_of_file(buffer, file_size, &byte_swap, &chunk_size) &&
                network_blf_read_next_known_chunk(buffer, file_size, byte_swap, 'levl', 3, &chunk_size, &chunk_buffer, 0, 0, &eof_found))
            {
                level = chunk_buffer ? chunk_buffer - sizeof(s_blf_header) : 0;
                if (level &&
                    network_blf_read_next_known_chunk(buffer, file_size, byte_swap, '_eof', 1, &chunk_size,
                        &chunk_buffer, 0, 0, &eof_found) &&
                    chunk_buffer &&
                    chunk_size == sizeof(s_blf_chunk_end_of_file_with_rsa) &&
                    eof_found &&
                    network_blf_verify_end_of_file(buffer, file_size, byte_swap, chunk_buffer - sizeof(s_blf_header),
                        _blf_file_authentication_signature))
                {
                    valid = true;
                }
            }
            if (valid)
                levels_add_map(level, byte_swap, map_directory, downloadable);
        }
        file_close(file);
    }
}

// @address 0x1e5bf0 levels_update_enumeration
extern "C" void levels_update_enumeration(void)
{
    unsigned char idle;
    unsigned long query[4];
    s_autosave_queue_level_enumeration_task task;
    if ((signed char)g_level_globals.enumerate_info_files != 0 ||
        (signed char)g_level_globals.enumerate_dlc_files == 0)
        return;
    if (g_level_globals.enumeration_task_id == -1 || g_level_globals.enumeration_complete.peek() != 0)
        idle = 0;
    else
        idle = 1;
    if (idle || network_join_test_catalogues_busy(query))
        return;
    levels_delete_flagged_levels();
    task.enumeration.controller_index = 0;
    task.enumeration.state = 0;
    task.enumeration.content_item_index = -1;
    task.enumeration.find_file_data = &g_level_globals.enumeration_file_data;
    g_level_globals.enumeration_task_id = async_task_add_ex(_async_priority_blocking_generic,
        &task.async_task, _async_category_saved_games,
        reinterpret_cast<e_async_completion (*)(s_async_task *)>(saved_game_files_enumerate_from_autosave_queue_find_next_file),
        &g_level_globals.enumeration_complete, true);
    g_level_globals.enumerate_dlc_files = 0;
}

// @address 0x1e5cd0 levels_enumerate_info_files
extern "C" e_async_completion levels_enumerate_info_files(s_levels_enumeration_task *task)
{
    s_file_last_modification_date modification_date;
    s_file_reference file;
    char name[256];
    char extension[256];

    switch (task->state)
    {
    case 0:
    {
        g_maps_directory = "d:\\maps\\";
        csnzprintf(name, sizeof(name), "%sinfo", g_maps_directory);
        s_file_reference *directory = file_reference_create_from_path(&file, name, true);
        find_files_start(task->find_file_data, 0, directory, "*.*");
        task->state++;
        break;
    }
    case 1:
        if (find_files_next(task->find_file_data, &file, &modification_date))
        {
            file_reference_get_name(&file, 1, name, sizeof(name));
            file_reference_get_name(&file, 8, extension, sizeof(extension));
            if (csstricmp(extension, "campaign") == 0)
                levels_load_campaign_file(&file, name, false);
            else if (csstricmp(extension, "mapinfo") == 0)
                levels_load_mapinfo_file(&file, name, false);
        }
        else
        {
            find_files_end(task->find_file_data);
            task->state++;
        }
        break;
    }

    return task->state == 2 ? _async_completion_done : _async_completion_pending;
}

static __forceinline bool autosave_queue_catalogue_busy(const c_content_catalogue *catalogue)
{
    short state = (short)catalogue->m_enumeration_task.get_task_state();
    return state == 1 || state == 2 || state == 3;
}

static __forceinline long autosave_queue_next_controller_index(long controller_index)
{
    long next = -1;
    if (controller_index >= 0 && controller_index < 3)
        next = controller_index + 1;
    return next;
}

// @address 0x1e5e18 ?saved_game_files_enumerate_from_autosave_queue_find_next_file@@YA_NPAUs_autosave_queue_enumeration_task@@@Z
#if defined(_XBOX) && !defined(__clang__)
// match: 0x1e5e18: the Xbox symbol is mangled with a bool return (_N); the PC build returns e_async_completion so async_work_function reads a full-width result
bool saved_game_files_enumerate_from_autosave_queue_find_next_file(s_autosave_queue_enumeration_task *task)
#else
e_async_completion saved_game_files_enumerate_from_autosave_queue_find_next_file(s_autosave_queue_enumeration_task *task)
#endif
{
    s_file_last_modification_date modification_date;
    s_file_reference file;
    char name[256];
    char extension[256];
    s_file_reference directory;

    switch (task->state)
    {
    case 0:
    {
        task->content_item_index = -1;
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[task->controller_index];
        if (content_catalogue_has_storage_device(catalogue))
        {
            if (!autosave_queue_catalogue_busy((const c_content_catalogue *)catalogue))
                task->state = 1;
        }
        else
        {
            task->state = 4;
        }
        break;
    }
    case 1:
    {
        task->state = 4;
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[task->controller_index];
        long content_item_index;
        if (task->content_item_index == -1)
            content_item_index = content_catalogue_get_first_item_index(catalogue);
        else
            content_item_index = content_catalogue_get_next_item_index(catalogue, task->content_item_index);
        task->content_item_index = content_item_index;
        if (content_item_index != -1)
        {
            task->state = 1;
            if (content_catalogue_get_game_content_type(catalogue,
                content_item_index) == 0 && ((s_content_item *)catalogue)->make_ready_for_access(task->content_item_index, true))
                task->state = 2;
        }
        break;
    }
    case 2:
    {
        long content_item_index = task->content_item_index;
        task->state = 1;
        bool found = false;
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[task->controller_index];
        {
            c_autosave_queue_scope_lock lock;
            if (content_catalogue_has_storage_device(catalogue))
            {
                s_content_item *content_item = content_catalogue_try_and_get_item(catalogue, content_item_index);
                if (content_item)
                    found = content_item->get_directory_reference(&directory);
            }
        }
        if (found)
        {
            find_files_start(task->find_file_data, 0, &directory, "*.*");
            task->state = 3;
        }
        else
        {
            content_catalogue_close_item(catalogue, task->content_item_index, true);
        }
        break;
    }
    case 3:
        if (find_files_next(task->find_file_data, &file, &modification_date))
        {
            file_reference_get_name(&file, 1, name, sizeof(name));
            file_reference_get_name(&file, 8, extension, sizeof(extension));
            if (csstricmp(extension, "campaign") == 0)
                levels_load_campaign_file(&file, name, true);
            else if (csstricmp(extension, "mapinfo") == 0)
                levels_load_mapinfo_file(&file, name, true);
        }
        else
        {
            find_files_end(task->find_file_data);
            content_catalogue_close_item((c_content_catalogue *)&content_catalogues().catalogues[task->controller_index],
                task->content_item_index, true);
            task->state = 1;
        }
        break;
    case 4:
        task->controller_index = autosave_queue_next_controller_index(task->controller_index);
        while (task->controller_index != -1)
        {
            c_controller_interface *controller = (c_controller_interface *)&controller_globals().controllers[task->controller_index];
            if (controller->in_use() && !controller->is_guest())
                break;
            task->controller_index = autosave_queue_next_controller_index(task->controller_index);
        }
        if (task->controller_index >= 0 && task->controller_index < 4)
            task->state = 0;
        else
            task->state = 5;
        break;
    }

    return task->state == 5 ? _async_completion_done : _async_completion_pending;
}

// @address 0x2219b8 autosave_queue_set_entry_name
extern "C" inline void autosave_queue_set_entry_name(c_gui_screenshot_selected_item *entry, const wchar_t *name)
{
    ustrnzcpy_inline(entry->m_metadata.name, name, 16);
}

// @address 0x221a38 autosave_queue_set_entry_description
extern "C" inline void autosave_queue_set_entry_description(c_gui_screenshot_selected_item *entry, const wchar_t *description)
{
    ustrnzcpy_inline(entry->m_metadata.description, description, 128);
}

// @address 0x4ee5e0 ?get_file_path@c_gui_screenshot_selected_item@@UBA_NPADJ@Z
bool c_gui_screenshot_selected_item::get_file_path(char *buffer, long size) const
{
    const s_file_reference *reference = &file_reference;
    file_location_get_full_path(reference->location, reference->path, buffer, size);
    return true;
}

// @address 0x222208 ??4c_gui_screenshot_selected_item@@QAAAAV0@ABV0@@Z
c_gui_screenshot_selected_item &c_gui_screenshot_selected_item::operator=(const c_gui_screenshot_selected_item &other)
{
    c_gui_simple_selected_item::operator=(other);
    controller_index = other.controller_index;
    file_reference = other.file_reference;
    for (long index = 0; index < sizeof(author); index++)
        author[index] = other.author[index];
    date = other.date;
    size_in_bytes = other.size_in_bytes;
    bitmap_list_item_index = other.bitmap_list_item_index;
    return *this;
}

// @address 0x2222d8 autosave_queue_clear_entries
extern "C" long autosave_queue_clear_entries(s_autosave_queue *queue)
{
    long result = 0;
    if (!queue->busy) {
        queue->catalogue_enumerated = 0;
        queue->item_count = 0;
        queue->selected_index = -1;
        queue->selection_valid = 0;
        result = 1;
    }
    return result;
}

// 0x222338
// @address 0x222338 ?autosave_queue_save_current_game_variant_to_queue@@YAJPAUs_autosave_queue@@W4e_controller_index@@@Z
long autosave_queue_save_current_game_variant_to_queue(struct s_autosave_queue * queue, enum e_controller_index item_index)
{
    long result = 0;
    if (!queue->busy) {
        if (queue->task_id == -1) {
            s_controller *controller = &controller_globals().controllers[queue->controller_index];
            if (((c_controller_interface *)controller)->in_use()) {
                c_content_catalogue *catalogue =
                    (c_content_catalogue *)&content_catalogues().catalogues[controller - controller_globals().controllers];
                if (content_catalogue_has_storage_device(catalogue)) {
                    c_gui_screenshot_selected_item *entry = &queue->items[((long)item_index)];
                    if (!entry->get_enabled()) {
                        char path[256];
                        s_file_reference reference;
                        entry->get_file_path(path, 256);
                        long content_item = content_catalogue_find_item_by_file_reference(catalogue,
                            file_reference_create_from_path(&reference, path, false));
                        queue->content_item_index = content_item;
                        if (content_item != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item, true)) {
                            long task = async_read_entire_file(path, queue->buffer, queue->buffer_size, &queue->file_size,
                                                     _async_category_saved_games, _async_priority_blocking_generic, &queue->lock,
                                                     &queue->task_done);
                            queue->task_id = task;
                            if (task != -1) {
                                queue->busy = 2;
                                queue->loading_item_index = ((long)item_index);
                                queue->selected_index = -1;
                                result = 1;
                                queue->selection_valid = 0;
                            }
                        }
                    } else {
                        result = 1;
                        queue->selected_index = ((long)item_index);
                        queue->selection_valid = 0;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x222538 autosave_queue_finish_task
extern "C" void autosave_queue_finish_task(s_autosave_queue *queue)
{
    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[queue->controller_index];
    queue->task_id = -1;
    queue->busy = 0;
    __lwsync();
    _InterlockedExchange(&queue->lock.value, 0);
    content_catalogue_close_item(catalogue, queue->content_item_index, true);
    queue->content_item_index = -1;
}

// @address 0x2225e8 autosave_queue_delete_item
// 0x2225e8
extern "C" bool autosave_queue_delete_item(s_autosave_queue *queue, long item_index)
{
    bool result = false;
    if (!queue->busy) {
        s_controller *controller = &controller_globals().controllers[queue->controller_index];
        if (((c_controller_interface *)controller)->in_use()) {
            c_content_catalogue *catalogue =
                (c_content_catalogue *)&content_catalogues().catalogues[controller - controller_globals().controllers];
            if (content_catalogue_has_storage_device(catalogue)) {
                long content_item = content_catalogue_find_item_by_file_reference(
                    catalogue, (const s_file_reference *)&queue->items[item_index].file_reference);
                if (content_item != -1) {
                    queue->task_id = saved_game_files_delete_content_item_async(queue->controller_index, content_item,
                                                     &queue->lock,
                                                     &queue->task_done);
                }
                if (queue->task_id != -1) {
                    queue->busy = 3;
                    result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x2226f8 autosave_queue_set_item_text
// 0x2226f8
extern "C" bool autosave_queue_set_item_text(s_autosave_queue *queue, long item_index, const wchar_t *name, const wchar_t *description)
{
    bool result = false;
    if (queue->busy == 0) {
        long controller_index = queue->controller_index;
        queue->name_valid = false;
        queue->description_valid = false;
        s_controller *controller = &controller_globals().controllers[controller_index];
        if (((c_controller_interface *)controller)->in_use()) {
            c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller - controller_globals().controllers];
            if (content_catalogue_has_storage_device(catalogue)) {
                const wchar_t *name_arg = 0;
                const wchar_t *description_arg = 0;
                if (name != 0) {
                    ustrnzcpy_inline(queue->name_text, name, 16);
                    name_arg = queue->name_text;
                    queue->name_valid = true;
                }
                if (description != 0) {
                    ustrnzcpy_inline(queue->description_text, description, 128);
                    description_arg = queue->description_text;
                    queue->description_valid = true;
                }
                char path[256];
                queue->items[item_index].get_file_path(path, 256);
                long content_item = content_catalogue_find_item_by_file_reference(
                    catalogue, (const s_file_reference *)&queue->items[item_index].file_reference);
                if (content_item != -1)
                    queue->task_id = saved_game_files_update_metadata_async(queue->controller_index, content_item,
                        name_arg, description_arg,
                                                       &queue->lock, &queue->task_done);
                if (queue->task_id != -1) {
                    queue->busy = 4;
                    queue->pending_index = item_index;
                    result = true;
                }
            }
        }
    }
    return result;
}

// 0x222978
// @address 0x222978 autosave_queue_finish_set_item_text
extern "C" void autosave_queue_finish_set_item_text(s_autosave_queue *queue)
{
    if (queue->busy != 4)
        return;
    if (queue->task_id == -1)
        return;
    c_synchronized_long *state = &queue->task_done;
    if (state->peek() != 1)
        return;
    c_synchronized_long *lock = &queue->lock;
    if (lock->peek() == 1) {
        long index = queue->pending_index;
        if (index >= 0 && index < queue->item_count) {
            c_gui_screenshot_selected_item *entry = &queue->items[index];
            if (entry != 0) {
                if (queue->name_valid)
                    autosave_queue_set_entry_name(entry, queue->name_text);
                if (queue->description_valid)
                    autosave_queue_set_entry_description(entry, queue->description_text);
            }
        }
    }
    queue->task_id = -1;
    queue->busy = 0;
    lock->set(0);
}

// @address 0x761554 ?g_game_state_autosave_listener@@3Vc_game_state_autosave_listener@@A
// Xbox uses the preinitialized listener at this address. Its vtable points to
// the buffer-sizing routine, the shared empty body, and bitmap-storage reset.
#if !defined(_XBOX) || defined(__clang__)
void c_game_state_autosave_listener::set_mode(long mode, s_game_state_buffer_description *description)
{
    if (mode == 1)
    {
        description->type = 1;
        description->size = 0x96000;
        return;
    }
    description->size = 0;
    description->type = 0;
}

void c_game_state_autosave_listener::update(void *)
{
}

void c_game_state_autosave_listener::dispose(void *)
{
    gui_custom_bitmap_storage_reset();
}

c_game_state_autosave_listener g_game_state_autosave_listener;
#endif

// @address 0x222a98 autosave_queue_update
extern "C" void autosave_queue_update(s_autosave_queue *queue)
{
    if (!queue->initialized) {
        bool started = true;
        volatile s_autosave_queue *manager = g_gui_custom_bitmap_storage_manager;
        manager->buffer_size = 0x96159;
        void *result = game_state_procs_try_and_get_slot(2, 0, 0x96159,
            &g_game_state_autosave_listener);
        manager->buffer = static_cast<s_autosave_queue_file_header *>(result);
        if (result != 0)
            manager->busy = 1;
        else
            started = false;
        if (started) {
            volatile s_autosave_queue *storage_manager = g_gui_custom_bitmap_storage_manager;
            storage_manager->initialized = true;
        }
    }
    if (!queue->initialized)
        return;
    autosave_queue_populate_from_catalogue(queue);
    if (queue->busy == 2) {
        if (queue->task_id != -1) {
            if (queue->task_done.peek() == 1) {
                if (queue->lock.peek() == 1) {
                    if (queue->file_size >= (unsigned long)(queue->buffer->end_of_file.total_file_size + 0x159)) {
                        queue->selected_index = queue->loading_item_index;
                        queue->selection_valid = 1;
                    }
                } else {
                    queue->selected_index = queue->loading_item_index;
                    queue->selection_valid = 0;
                }
                autosave_queue_finish_task(queue);
            }
        }
    }
    if (queue->busy == 3 && queue->task_id != -1 && queue->task_done.peek() == 1) {
        c_synchronized_long *lock = &queue->lock;
        lock->peek();
        queue->busy = 0;
        queue->task_id = -1;
        lock->set(0);
        autosave_queue_clear_entries(queue);
    }
    autosave_queue_finish_set_item_text(queue);
}

// @address 0x2284d0 autosave_queue_save_item_remove_oldest_item_for_space
extern "C" long autosave_queue_save_item_remove_oldest_item_for_space(s_async_autosave_queue_save_item_task *task)
{
    s_saved_game_queue *queue = task->queue;
    s_file_reference directory;

    if (queue->state.peek() == _autosave_queue_purge_state_start)
    {
        if (queue->cancel.peek() == 0)
        {
            file_reference_create_from_path(&directory, k_saved_game_autosave_directory, true);
            find_files_start(&queue->find_file_data, 0, &directory, "*.*");
            queue->state.set(_autosave_queue_purge_state_find);
            task->find_started = true;
        }
        else
        {
            queue->state.set(_autosave_queue_purge_state_done);
        }
    }
    else if (queue->state.peek() == _autosave_queue_purge_state_find)
    {
        if (queue->cancel.peek() == 0)
        {
            s_saved_game_queue_item item;

            if (find_files_next(&queue->find_file_data, &item.file, &item.modification_date))
            {
                if (saved_game_is_file_in_category(queue->category, &item.file))
                {
                    if (saved_game_read_metadata_from_file(&item.file, &item.metadata))
                    {
                        item.valid = 1;
                        queue->items[queue->write_count.peek()] = item;
                        queue->write_count.increment();
                    }
                    else
                    {
                        file_delete(&item.file);
                    }
                }
            }
            else
            {
                queue->state.set(_autosave_queue_purge_state_done);
            }
        }
        else
        {
            queue->state.set(_autosave_queue_purge_state_done);
        }
    }

    if ((unsigned long)queue->write_count.peek() == 26)
        queue->state.set(_autosave_queue_purge_state_done);

    long done = queue->state.peek() != _autosave_queue_purge_state_find;
    if (done == 1 && task->find_started)
        find_files_end(&queue->find_file_data);

    return done;
}

// @address 0x2286f8 saved_game_queue_try_and_get_next
extern "C" unsigned char saved_game_queue_try_and_get_next(s_saved_game_queue *queue, s_saved_game_queue_item *item)
{
    bool result = false;
    if (queue->has_items())
    {
        long index = queue->read_count.increment() - 1;
        memcpy(item, &queue->items[index], sizeof(s_saved_game_queue_item));
        result = true;
    }
    return result;
}


// @address 0x228790 saved_game_is_file_in_category
extern "C" bool saved_game_is_file_in_category(long category, const s_file_reference *reference)
{
    s_autosave_queue_string filename;
    s_autosave_queue_string extension;
    bool extension_matches = false;
    char filename_buffer[256];
    char extension_buffer[256];

    file_reference_get_name(reference, 4, filename_buffer, 256);
    file_reference_get_name(reference, 8, extension_buffer, 256);
    filename.text.set(filename_buffer);
    extension.text.set(extension_buffer);
    bool filename_matches = filename.text.starts_with("asq");

    if ((unsigned long)category <= _saved_game_file_type_shot)
    {
        extension_matches = strncmp(extension.text.get_string(),
            saved_game_filename_extension_by_game_file_type((e_saved_game_file_type)category, true), 256) == 0;
    }
    else if (category == 14)
    {
        for (long file_type = 0; file_type < 14; file_type++)
        {
            extension_matches = strncmp(extension.text.get_string(),
                saved_game_filename_extension_by_game_file_type((e_saved_game_file_type)file_type, true), 256) == 0;
            if (extension_matches)
                break;
        }
    }
    else if (category == 15)
    {
        for (long file_type = 1; file_type <= 9; file_type++)
        {
            extension_matches = strncmp(extension.text.get_string(),
                saved_game_filename_extension_by_game_file_type((e_saved_game_file_type)file_type, true), 256) == 0;
            if (extension_matches)
                break;
        }
    }
    return filename_matches && extension_matches;
}

// @address 0x2283f0 saved_game_queue_start_enumeration
extern "C" void saved_game_queue_start_enumeration(s_saved_game_queue *queue, long category)
{
    async_block_until_done(&queue->done, simple_yield_function, 1, 0, 0, 0);
    if (network_session_interface_has_gamestate_bonus())
    {
        s_autosave_queue_async_task task;
        memset(&task, 0, sizeof(task));
        task.save_item.category = queue->category;
        task.save_item.queue = queue;
        queue->category = category;
        queue->cancel.set(0);
        queue->state.set(_autosave_queue_purge_state_start);
        queue->thread_id = async_task_add_ex(_async_priority_blocking_generic,
        &task.async_task, _async_category_saved_games,
        reinterpret_cast<e_async_completion (*)(s_async_task *)>(autosave_queue_save_item_remove_oldest_item_for_space),
        &queue->done, true);
    }
}

s_async_state::~s_async_state()
{
    queue.cancel.set(1);
    async_block_until_done(&queue.done, simple_yield_function, 1, 0, 0, 0);
}

// @address 0x4eeb68 ?initialize@c_gui_saved_film_subitem_datasource@@UAA_NJ@Z
bool c_gui_saved_film_subitem_datasource::initialize(long name)
{
    m_name = name;
    if (category == 1)
        saved_game_queue_start_enumeration(&async.queue, 11);
    return true;
}

// @address 0x4eebb0 ?dispose@c_gui_saved_film_subitem_datasource@@UAAXXZ
void c_gui_saved_film_subitem_datasource::dispose()
{
    s_saved_game_queue *queue = &async.queue;
    queue->cancel.set(1);
    async_block_until_done(&queue->done, simple_yield_function, 1, 0, 0, 0);
}

static __forceinline bool autosave_queue_find_saved_game_variants(s_saved_game_queue_item *items, long maximum_count, long *out_count)
{
    long count = 0;
    bool result = saved_game_collect_files(0, items, 0, maximum_count, &count, 0);
    *out_count = count;
    return result;
}

// @address 0x228cc0 saved_game_save_current_variant
extern "C" void saved_game_save_current_variant(void)
{
    long count;
    game_options *options = &tls_game_globals()->options;
    c_game_variant *variant = &options->game_variant;
    s_saved_game_queue_item items[25];
    if (!autosave_queue_find_saved_game_variants(items, 25, &count))
        return;

    long newest_index = -1;

    for (long index = 0; index < count; index++)
    {
        if (newest_index == -1 || CompareFileTime((const FILETIME *)&items[newest_index].modification_date,
                (const FILETIME *)&items[index].modification_date) < 0)
            newest_index = index;
    }
    if (newest_index != -1 && items[newest_index].metadata.unique_id == game_variant_compute_unique_id(variant))
        return;

    s_blffile_game_variant file_data;
    s_autosave_queue_string path;
    long engine_type = variant->engine_type;
    file_data.get_metadata() = options->game_variant.get_active_variant()->m_metadata;
    file_data.get_metadata().unique_id = game_variant_compute_unique_id(variant);
    game_variant_set_from((c_game_variant *)&file_data.game_variant.game_variant, (const c_game_variant *)variant);
    file_data.end_of_file.total_file_size = 0x3a8;
    saved_game_files_get_autosave_path(game_engine_index_to_saved_game_file_type((e_game_engine_type)engine_type), &path);

    c_file_reference file(path.text.get_string(), 2);
    s_autosave_queue_buffered_file_writer writer(&file);
    buffered_file_write(&writer.writer, &file_data, sizeof(file_data));
    writer.flush();
    if (file.m_open)
        file_close(&file);
}

// @address 0x745ce4 g_saved_game_category_size_limits
extern "C" { long g_saved_game_category_size_limits[] = { 0x19000, 0x177000, 0x1f400000 }; }

// @address 0x2290f8 saved_game_purge_old_files
extern "C" bool saved_game_purge_old_files(long category)
{
    bool result;
    bool keep_deleting = true;
    long count = 0;
    long total_size = 0;
    long file_sizes[26];
    s_saved_game_queue_item items[26];

    do
    {
        long used_size = 0;
        long oldest_index = -1;
        result = saved_game_collect_files(category, items, file_sizes, 26, &count, &total_size);
        if (!result)
            break;

        long item_count = count;
        for (long index = 0; index < item_count; index++)
        {
            used_size += file_sizes[index];
            if (oldest_index == -1 || CompareFileTime((const FILETIME *)&items[oldest_index].modification_date,
                (const FILETIME *)&items[index].modification_date) > 0)
                oldest_index = index;
        }

        if (total_size > g_saved_game_category_size_limits[category])
            break;

        if (used_size + total_size > g_saved_game_category_size_limits[category] || item_count > 25)
        {
            if (!file_delete(&items[oldest_index].file))
                break;

            long move_count = item_count - oldest_index - 1;
            memmove(&items[oldest_index], &items[oldest_index + 1], move_count * sizeof(s_saved_game_queue_item));
            memmove(&file_sizes[oldest_index], &file_sizes[oldest_index + 1], move_count * sizeof(long));
            count = item_count - 1;
        }
        else
        {
            keep_deleting = false;
        }
    }
    while (keep_deleting);

    return result;
}

static __forceinline bool autosave_queue_catalogue_enumeration_complete(const c_content_catalogue *catalogue)
{
    return (catalogue->m_flags.value >> 1) & 1;
}

// @address 0x221f68 autosave_queue_populate_from_catalogue
extern "C" void autosave_queue_populate_from_catalogue(s_autosave_queue *queue)
{
    bool complete = true;
    if (!queue->catalogue_enumerated)
    {
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[queue->controller_index];
        if (catalogue != 0 && content_catalogue_has_storage_device(catalogue))
        {
            if (!autosave_queue_catalogue_enumeration_complete(catalogue))
            {
                complete = false;
            }
            else
            {
                long index = content_catalogue_get_first_item_index(catalogue);
                while (index != -1)
                {
                    if (queue->item_count >= 50)
                        break;
                    if (content_catalogue_get_game_content_type(catalogue, index) == _game_content_type_shot)
                    {
                        bool corrupt = content_catalogue_is_item_corrupt(catalogue, index);
                        s_file_reference reference;
                        s_saved_game_item_metadata_packed metadata;
                        if (::c_start_menu_game_editor::load_progress_ui_for_catalogue((c_start_menu_game_editor *)catalogue,
                                static_cast<e_controller_index>(index), &reference) &&
                            content_catalogue_item_metadata_matches(catalogue, index, &metadata))
                        {
                            long item_index = queue->item_count++;
                            queue->items[item_index] = c_gui_screenshot_selected_item(&metadata, metadata.author,
                                metadata.date, metadata.size_in_bytes, _gui_stored_item_location_saved_game_file,
                                queue->controller_index, &reference, corrupt);
                        }
                    }
                    index = content_catalogue_get_next_item_index(catalogue, index);
                }

                for (long i = 0; i < queue->item_count / 2; i++)
                {
                    c_gui_screenshot_selected_item temporary(queue->items[i]);
                    long swap_index = queue->item_count - i - 1;
                    queue->items[i] = queue->items[swap_index];
                    queue->items[swap_index] = temporary;
                }
            }
        }
        if (complete)
        {
            queue->busy = 0;
            queue->catalogue_enumerated = 1;
        }
    }
}

// @address 0x228288 saved_game_cleanup_storage
extern "C" void saved_game_cleanup_storage()
{
    s_file_reference file;
    s_file_reference directory;
    s_find_file_data find_file_data;

    game_globals_storage *game = tls_game_globals();
    bool game_active = game != 0 && (game->initializing != 0 || game->map_active != 0);
    if (!game_active || game->options.game_mode == 3 || main_game_globals.reset_in_progress != 0)
    {
        return;
    }

    file_reference_create_from_path(&directory, k_saved_game_autosave_directory, true);
    find_files_start(&find_file_data, 0, &directory, "*.temp");
    if (find_files_next(&find_file_data, &file, 0))
    {
        do
        {
#ifdef HALO3_64BIT
            // The recorder can already have opened its temporary file during map setup.
            if (saved_film_is_recording_to_file(&file))
                continue;
#endif
            file_delete(&file);
        } while (find_files_next(&find_file_data, &file, 0));
    }
    find_files_end(&find_file_data);

    if (!((unsigned char)game_is_cooperative() != 0 && (unsigned char)saved_film_is_recording() == 0))
    {
        saved_game_purge_old_files(2);
    }

    game = tls_game_globals();
    if (game->options.game_mode == 2 && (unsigned char)game_engine_sandbox_is_active() == 0)
    {
        saved_game_purge_old_files(0);
        saved_game_save_current_variant();
        saved_game_files_write_map_autosave(saved_game_purge_old_files(1));
    }
}

// @address 0x228f08 saved_game_collect_files
extern "C" bool saved_game_collect_files(
    long category, s_saved_game_queue_item *items, long *file_sizes, long maximum_count, long *count, long *total_size)
{
    s_saved_game_queue queue;
    queue.category = -1;
    queue.thread_id = -1;
    *count = 0;

    long total;
    if (category >= 1u)
    {
        if (category != 1u)
        {
            saved_game_queue_start_enumeration(&queue, 11);
            total = 0x6400000;
        }
        else
        {
            saved_game_queue_start_enumeration(&queue, 10);
            total = 0xe1f0;
        }
    }
    else
    {
        saved_game_queue_start_enumeration(&queue, 15);
        total = 0x3bc;
    }
    if (total_size != 0)
        *total_size = total;

    while (true)
    {
        bool running = queue.state.peek() != 0 && queue.state.peek() != 3;
        if (!running)
        {
            if (!queue.has_items())
                break;
        }

        long index = *count;
        if (index < maximum_count)
        {
            if (queue.has_items() && saved_game_queue_try_and_get_next(&queue, &items[index]))
            {
                if (file_sizes != 0 && !file_get_size(&items[*count].file, (unsigned long *)&file_sizes[*count]))
                    continue;
                (*count)++;
                continue;
            }
            Sleep(0);
            continue;
        }
        break;
    }

    queue.cancel.set(1);
    async_block_until_done(&queue.done, simple_yield_function, 1, 0, 0, 0);
    return true;
}
