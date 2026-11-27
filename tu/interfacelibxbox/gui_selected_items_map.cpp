#include "saved_games/saved_game_files_operations.h"
#include "saved_games/content_catalogue_iteration.h"
// interfacelibxbox/gui_selected_items_map.obj: from the records 0x4eb9e0 0x4ebac0 0x4ebcc8 0x4ebd48 0x4ebf60 0x4ebf68 0x4ebfd8 0x4ec0a0 0x4ec0c8 0x4ec1a8 0x4ec258 0x4ec3b0 0x4ec448 0x4ec498 0x4ec4b0 0x4ec5e8 0x4ec7c0 0x4ec848 0x4ec930 0x4ec968 0x4ec978 0x4ecb08 0x4ecca0 0x4eccb8 0x4f3930
// @flags /TP /O2
// @compiler xdk6021
#include "interface/gui_selected_items_map_dependencies.h"
#include "interface/file_share_category_dependencies.h"
// @address 0x8a1ead g_utility_drive_mounted
#include "memory/static_arrays.h"
#include <string.h>
#include <stdlib.h>
#include "game/game_types.h"
#include "interface/gui_data.h"
#include "main/levels_map_datum_types.h"
#include "main/levels_globals.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
// @address 0x4ee148 ?game_variant_category_sort_proc@@YAHPBX0@Z
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x4ebd48 ?initialize@c_gui_map_category_datasource@@UIAA_NJ@Z
// @address 0x4ebf68 ?get_text_value@c_gui_map_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4ec3b0 ??4c_gui_map_selected_item@@QAAAAV0@ABV0@@Z
// @address 0x4ec978 ?begin_saved_game_files_enumeration@c_gui_map_subitem_selectable_item_datasource@@IAAXJW4e_map_id@@@Z
// @address 0x4ecb08 ?begin_autosave_queue_enumeration@c_gui_map_subitem_selectable_item_datasource@@IAAXJW4e_map_id@@@Z
// @address 0x4ec0c8 ??0c_gui_map_subitem_selectable_item_datasource@@QAA@XZ
// @address 0x491d68 ??0c_gui_simple_selected_item@@QAA@W4e_gui_selected_item_type@@PB_W1W4e_special_item_type@c_gui_selected_item@@W4e_gui_selected_item_location@@JJ@Z
// @address 0x4ec498 ?begin_enumeration@c_gui_map_subitem_selectable_item_datasource@@IAAXJW4e_map_id@@@Z
// @address 0x4ebcc8 ?get_file_path@c_gui_map_selected_item@@UBA_NPADJ@Z
// @address 0x4ec968 ?get_current_item_count_internal@c_gui_map_subitem_selectable_item_datasource@@MAAJXZ
// @address 0x4ec0a0 ?get_string_id_value@c_gui_map_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4ec930 ?get_gui_selected_item@c_gui_map_subitem_selectable_item_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x4ebf60 ?get_current_item_count_internal@c_gui_map_category_datasource@@MAAJXZ
// @address 0x4ec448 ?cancel@c_gui_map_subitem_selectable_item_datasource@@QAAXXZ
// @address 0x4ec258 ?initialize@c_gui_map_subitem_selectable_item_datasource@@UAA_NJW4e_map_id@@W4e_controller_index@@@Z
// @address 0x4ecca0 ?is_enumerating@c_gui_map_subitem_selectable_item_datasource@@QAAJXZ
// @address 0x4ec5e8 ?get_integer_value@c_gui_map_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4ec4b0 ?get_column_names@c_gui_map_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
// @address 0x4ec848 ?get_string_id_value@c_gui_map_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x1aea50 ?file_location_get_full_path@@YAXFPBDPADJ@Z
// @address 0x58e5c0 memset
// @address 0x284b30 content_catalogue_item_metadata_matches
#include <ppcintrinsics.h>
#include "interface/gui_selected_items_game_variant.h"
#include "interface/gui_selected_items_hopper.h"
#include "interface/gui_selected_items_level.h"
#include "interface/gui_selected_item_metadata.h"
#include "tag_files/files_windows.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "cseries/async.h"
#include "shell/shell_types.h"
#include "memory/data_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "interface/gui_selected_items_map.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/file_reference.h"
#include "cseries/async_helpers.h"
#include "saved_games/content_item_types.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "cseries/read_write_barrier.h"
#include "cseries/compiler_intrinsics.h"
#include <wchar.h>
#include "cseries/async_block.h"
#include "saved_games/content_catalogue_lookup.h"
// @address 0x58e090 memcpy
// @address 0x58f528 wcscmp

// The indexed entry (680 bytes each, from 272) when the index is in range, else null.



// The owner's list of 620-byte items (from 0x5cc) and its count (0x7ee4).

class c_gui_selected_item;
// The display metadata block a selected item copies out (the item's block at +0x18).

#include "interface/gui_selected_item.h"
#include "interface/gui_selected_items.h"
// @address 0x565d8 ??_7c_gui_map_selected_item@@6B@
// @address 0x4ec1a8 ??_Gc_gui_map_subitem_selectable_item_datasource@@UAAPAXI@Z
// @address 0x4ec1a8 ??_Ec_gui_map_subitem_selectable_item_datasource@@UAAPAXI@Z
#include "interface/gui_simple_selected_item.h"
typedef char c_gui_simple_selected_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_simple_selected_item) == 0x190))) ? 1 : -1];
typedef char c_gui_map_selected_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_map_selected_item) == 0x2a8))) ? 1 : -1];
// What a saved-game file enumeration hands back: the file reference (0x110), the item's kind, then the packed metadata.


#include "interface/gui_game_variant_datasource_types.h"

// Datasource column ids shared by the map category and subitem datasources
// (cache string table names; mirrors e_global_string_id in tag_files/string_ids.h,
// which this lane could not lease, so the shared adoption stays a follow-up).
enum
{
    _string_id_category = 254,
    _string_id_title = 0x29d,
    _string_id_description = 0x29e,
    _string_id_locked = 0x1fa,
    _string_id_map_id = 0x10029,
    _string_id_no_hdd = 0x102e1,
    _string_id_no_items = 0x102e2,
    _string_id_recent_maps_no_hdd = 0x102e5,
    _string_id_recent_maps_no_items = 0x102e6,
    _string_id_corrupt_file = 0x102ff,
    _string_id_item_can_be_saved = 0x10309,
    _string_id_item_saved = 0x1030a,
    _string_id_item_built_in = 0x1030b,
    _string_id_item_options = 0x1030c,
    _string_id_map_variant_can_be_saved = 0x1031d,
    _string_id_map_variant_saved = 0x1031e,
    _string_id_map_variant_built_in = 0x1031f,
    _string_id_map_variant_options = 0x10320,
    _string_id_image_name = 0x10326,
    _string_id_image_index = 0x10327,
    _string_id_map_image = 0x1032a,
    _string_id_map_variant_image = 0x1032b,
    _string_id_special_status = 0x103d7,
    _string_id_new = 0x105bb,
};

// The category list's entries (620 bytes each): kind, id, name, description text, sort value.

// The autosave queue's per-controller session (672 bytes): its flags at 0x298 say whether the queue is ready.

// The autosave queue entry's metadata: its file type (10 = map variant) and its map id.

__forceinline c_gui_map_selected_item::c_gui_map_selected_item() :
    c_gui_simple_selected_item(_gui_selected_item_none, (const wchar_t *)L"", (const wchar_t *)L"",
        c_gui_selected_item::_special_item_type_none, static_cast<e_gui_selected_item_location>(0), 0, 0)
{
    sort = 0;
}





extern "C" {



}

bool c_gui_map_selected_item::get_file_path(char *buffer, long size) const
{
    if (get_location() != 1 && get_location() != 2)
        return false;
    const s_file_reference *reference = &file;
    file_location_get_full_path(reference->location, reference->path, buffer, size);
    return true;
}

// 0x4ebd48
// match: 0x4ebd48: restrict-qualified this preserves the flag-load scheduling.
bool c_gui_map_category_datasource::initialize(long mode) __restrict
{
    this->m_name = mode;
    this->m_item_count = 0;
    this->m_items[this->m_item_count].type = 0;
    this->m_items[this->m_item_count].id = -1;
    this->m_items[this->m_item_count].name.set(this->m_first_name);
    this->m_items[this->m_item_count].text.set(this->m_first_description);
    this->m_items[this->m_item_count].value = -2;
    this->m_item_count++;
    if (content_file_share_enabled()) {
        this->m_items[this->m_item_count].type = 1;
        this->m_items[this->m_item_count].id = -1;
        this->m_items[this->m_item_count].name.set(this->m_second_name);
        this->m_items[this->m_item_count].text.set(this->m_second_description);
        this->m_items[this->m_item_count].value = -1;
        this->m_item_count++;
    }
    long unique_ids[52];
    long unique_id_count = 50;
    levels_get_multiplayer_map_ids(unique_ids, &unique_id_count);
    levels_map_datum level;
    long count = unique_id_count;
    for (long i = 0; i < count; i++) {
        long id = unique_ids[i];
        if (levels_is_map_type_allowed(id) && levels_try_and_get_datum(g_level_globals.multiplayer_levels, id, &level)) {
            this->m_items[this->m_item_count].type = 2;
            this->m_items[this->m_item_count].id = id;
            this->m_items[this->m_item_count].name.set(level.display_name);
            this->m_items[this->m_item_count].text.set(level.description);
            this->m_items[this->m_item_count].value = level.sort_order;
            this->m_item_count++;
        }
    }
    qsort(this->m_items, this->m_item_count, sizeof(s_gui_selected_items_map_category_entry), game_variant_category_sort_proc);
    return 1;
}

// 0x4ebf60
// @address 0x4f4038 ?get_column_names@c_gui_map_category_datasource@@UAAXQAJPAJ@Z
void c_gui_map_category_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[0] = _string_id_category;
    names[++*count] = _string_id_title;
    names[++*count] = _string_id_description;
    ++*count;
}

long c_gui_map_category_datasource::get_current_item_count_internal()
{
    return this->m_item_count;
}

bool c_gui_map_category_datasource::get_text_value(long context, long string_id, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (context >= 0 && context < m_item_count) {
        switch (string_id) {
        case _string_id_title:
            value->set(m_items[context].name);
            found = true;
            break;
        case _string_id_description:
            value->set(m_items[context].text.m_string);
            found = true;
            break;
        }
    }
    return found;
}

bool c_gui_map_category_datasource::get_string_id_value(long context, long string_id, long *result)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *result = _string_id_map_image;
        found = true;
    }
    return found;
}

// 0x4ec0c8
c_gui_map_subitem_selectable_item_datasource::c_gui_map_subitem_selectable_item_datasource() :
    m_map_id(-1),
    m_selected_item_count(0),
    m_controller_index(-1),
    m_enumeration_complete(0)
{
}

// 0x4ec3b0
c_gui_map_selected_item &c_gui_map_selected_item::operator=(const c_gui_map_selected_item &other)
{
    m_type = other.m_type;
    m_special_type = other.m_special_type;
    m_location = other.m_location;
    m_flag14 = other.m_flag14;
    m_flag15 = other.m_flag15;
    m_flag16 = other.m_flag16;
    memcpy(&m_metadata, &other.m_metadata, sizeof(m_metadata));
    controller_index = other.controller_index;
    file = other.file;
    sort = other.sort;
    return *this;
}

// 0x4ec498
void c_gui_map_subitem_selectable_item_datasource::begin_enumeration(long controller_index, e_map_id map_id)
{
    if (m_map_id == -1)
        begin_saved_game_files_enumeration(controller_index, map_id);
    else
        begin_autosave_queue_enumeration(controller_index, map_id);
}

// The subitem datasource column set.
void c_gui_map_subitem_selectable_item_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[(*count)++] = _string_id_title;
    names[(*count)++] = _string_id_description;
    names[(*count)++] = _string_id_image_index;
    names[(*count)++] = _string_id_locked;
    names[(*count)++] = _string_id_item_saved;
    names[(*count)++] = _string_id_image_name;
    names[(*count)++] = _string_id_no_hdd;
    names[(*count)++] = _string_id_no_items;
    names[(*count)++] = _string_id_item_can_be_saved;
    names[(*count)++] = _string_id_item_built_in;
    names[(*count)++] = _string_id_item_options;
}

// 0x4ec7c0: the entry's display text comes from its metadata block through the saved-files formatter.
// @address 0x4ec7c0 ?get_text_value@c_gui_map_subitem_selectable_item_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_map_subitem_selectable_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < this->item_count()) {
        const c_gui_map_selected_item *item =
            this->item_at(element_handle);
        s_gui_selected_items_block metadata;
        result = saved_game_files_get_metadata_string(item->get_metadata(&metadata), value_name, value);
    }
    return result;
}

// Look up the selected item string identifier.
bool c_gui_map_subitem_selectable_item_datasource::get_string_id_value(long context, long string_id, long *result)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *result = _string_id_map_variant_image;
        found = true;
    } else if (string_id == _string_id_no_hdd) {
        *result = _string_id_recent_maps_no_hdd;
        found = true;
    } else if (string_id == _string_id_no_items) {
        *result = _string_id_recent_maps_no_items;
        found = true;
    } else if (string_id == _string_id_item_can_be_saved) {
        *result = _string_id_map_variant_can_be_saved;
        found = true;
    } else if (string_id == _string_id_item_saved) {
        *result = _string_id_map_variant_saved;
        found = true;
    } else if (string_id == _string_id_item_built_in) {
        *result = _string_id_map_variant_built_in;
        found = true;
    } else if (string_id == _string_id_item_options) {
        *result = _string_id_map_variant_options;
        found = true;
    }
    return found;
}

const c_gui_selected_item *c_gui_map_subitem_selectable_item_datasource::get_gui_selected_item(long index) const
{

    const c_gui_map_selected_item *result = 0;
    if (index >= 0 && index < m_selected_item_count)
        result = &m_entries[index];
    return (const c_gui_selected_item *)result;
}

long c_gui_map_subitem_selectable_item_datasource::get_current_item_count_internal()
{
    return m_selected_item_count;
}

void c_gui_map_subitem_selectable_item_datasource::begin_saved_game_files_enumeration(long controller_index, e_map_id map_id)
{
    if (m_enumeration_complete != 0)
        return;
    s_saved_game_queue_item item;
    while (m_queue.has_items() && saved_game_queue_try_and_get_next(&m_queue, &item)) {
        if (item.valid != 1)
            continue;
        if (m_selected_item_count >= 151)
            continue;
        s_gui_selected_items_block metadata;
        gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&metadata), &item.metadata);
        long index = m_selected_item_count++;
        m_entries[index] = c_gui_map_selected_item(&metadata, static_cast<e_gui_selected_item_location>(2),
            m_controller_index, &item.file, 0, -1, item.valid == 2, 0);
    }
    qsort(m_entries, m_selected_item_count, sizeof(m_entries[0]), map_selected_item_sort_proc);
    if (!m_queue.active() && !m_queue.has_items())
        m_enumeration_complete = 1;
}

class c_content_catalogue;
extern "C" {




}

// 0x4ebac0
class c_map_variant;
extern "C" {
}

static __forceinline long gui_selected_items_map_autosave_queue_read_file(
    const s_file_reference *file, void *buffer, long buffer_size, c_synchronized_long *success, c_synchronized_long *done)
{
    char file_path[256];
    file_location_get_full_path(file->location, file->path, file_path, sizeof(file_path));
    return async_read_entire_file(file_path, buffer, buffer_size, 0, _async_category_saved_games, _async_priority_blocking_generic, success, done);
}

bool c_gui_map_selected_item::get_map_variant(c_map_variant *map_variant) const
{
    bool result = false;
    switch (get_location())
    {
    case 1:
    {
        c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        long content_item_index = content_catalogue_find_item_by_file_reference(catalogue, &file);
        if (content_item_index != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true))
        {
            s_blffile_map_variant blf_file;
            c_synchronized_long success(0);
            c_synchronized_long done(0);
            if (saved_game_files_read(controller_index, content_item_index, &blf_file, sizeof(blf_file), &success, &done) != -1)
            {
                async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
                if (success.peek() == 1)
                {
                    bool valid = false;
                    if (blf_file.copy_to_and_validate(map_variant, &valid))
                        result = true;
                }
            }
            content_catalogue_close_item(catalogue, content_item_index, true);
        }
        break;
    }
    case 2:
    {
        s_blffile_map_variant blf_file;
        c_synchronized_long success(0);
        c_synchronized_long done(0);
        if (gui_selected_items_map_autosave_queue_read_file(&file, &blf_file, sizeof(blf_file), &success, &done) != -1)
        {
            async_block_until_done(&done, simple_yield_function, 0, 0, 0, 0);
            if (success.peek() == 1)
            {
                bool valid = false;
                if (blf_file.copy_to_and_validate(map_variant, &valid))
                    result = true;
            }
        }
        break;
    }
    }
    return result;
}

// 0x4ecb08
void c_gui_map_subitem_selectable_item_datasource::begin_autosave_queue_enumeration(long controller_index, e_map_id map_id)
{
    if (!m_enumeration_complete)
    {
        c_content_catalogue *session = &content_catalogues().catalogues[m_controller_index];
        if (content_catalogue_has_storage_device(session) && (unsigned char)((session->m_flags.value >> 1) & 1))
        {
            int handle = content_catalogue_get_first_item_index(session);
            while (handle != -1 && m_selected_item_count < 151)
            {
                s_saved_game_item_metadata_packed metadata;
                s_file_reference file;
                wchar_t name[192];
                if (content_catalogue_item_metadata_matches((const c_content_catalogue *)session, handle, &metadata) &&
                    metadata.file_type == 10 && metadata.map_id == m_map_id &&
                    ((c_start_menu_game_editor *)(const c_content_catalogue *)session)->load_progress_ui((e_controller_index)handle, (s_file_reference *)&file))
                {
                    gui_selected_items_unpack_metadata((s_gui_selected_items_block *)name, &metadata);
                    int index = m_selected_item_count++;
                    int special_type = content_catalogue_is_item_created(session, handle);
                    m_entries[index] = c_gui_map_selected_item((const s_gui_selected_items_block *)name,
                        static_cast<e_gui_selected_item_location>(1), m_controller_index, &file, 0, -1,
                        content_catalogue_is_item_corrupt(session, handle), (unsigned char)special_type);
                }
                handle = content_catalogue_get_next_item_index(session, handle);
            }
            qsort(m_entries, m_selected_item_count, sizeof(m_entries[0]), map_selected_item_sort_proc);
            m_enumeration_complete = 1;
        }
    }
}

// The hopper subitem columns fold onto the same bytes: one copy serves both.
// @address 0x4f3930 ?get_column_names@c_gui_hopper_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
void c_gui_hopper_subitem_selectable_item_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[(*count)++] = _string_id_title;
    names[(*count)++] = _string_id_description;
}

// The level subitem columns (March gui_selected_items_level.obj 0x83727c68: title 710, description 711)
// fold onto the same bytes as the game-variant and hopper copies above.
// @address 0x4f3930 ?get_column_names@c_gui_level_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
void c_gui_level_subitem_selectable_item_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[(*count)++] = _string_id_title;
    names[(*count)++] = _string_id_description;
}

// @address 0x4eb9e0 ??0c_gui_map_selected_item@@QAA@PBUs_gui_selected_items_block@@W4e_gui_selected_item_location@@JPBUs_file_reference@@JJEE@Z
c_gui_map_selected_item::c_gui_map_selected_item(const s_gui_selected_items_block *metadata, e_gui_selected_item_location location,
    long controller_index, const s_file_reference *file, long sort_order, long, unsigned char enabled, unsigned char corrupt) :
    c_gui_simple_selected_item(_gui_selected_item_map, metadata, location, enabled, corrupt)
{
    this->controller_index = controller_index;
    memset(&this->file, 0, sizeof(this->file));
    sort = sort_order;
    if (file != 0)
        memcpy(&this->file, file, offsetof(s_file_reference, handle));
    else
        memset(&this->file, 0, sizeof(this->file));
}




static __forceinline void gui_selected_items_map_wstring_copy(wchar_t *destination, const wchar_t *source, long count)
{
    long i = 0;
    do
    {
        if ((destination[i] = *source) == 0)
            break;
        source++;
        i++;
    } while (i <= count - 1);
    destination[count - 1] = 0;
}

// 0x4ec258
bool c_gui_map_subitem_selectable_item_datasource::initialize(long name, e_map_id map_id, e_controller_index controller_index)
{
    m_name = name;
    m_map_id = map_id;
    m_selected_item_count = 0;
    m_controller_index = controller_index;
    bool success = true;
    if (map_id != -1)
    {
        levels_map_datum level;
        if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, &level))
        {
            s_saved_game_item_metadata_packed record;
            saved_game_files_initialize_metadata((s_saved_game_item_metadata_record *)&record,
                controller_index, 10, level.display_name, (const wchar_t *)L"", 0, 0);
            record.map_id = map_id;
            s_gui_selected_items_block metadata;
            gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&metadata), (const s_saved_game_item_metadata_packed *)((const char *)&record));
            gui_selected_items_map_wstring_copy(metadata.description, level.description, 128);
            c_gui_map_selected_item *entry = &m_entries[m_selected_item_count];
            *entry = c_gui_map_selected_item(&metadata, static_cast<e_gui_selected_item_location>(0),
                controller_index, 0, level.sort_order, -1, 0, 0);
            m_selected_item_count++;
        }
        else
        {
            success = false;
        }
    }
    else
    {
        saved_game_queue_start_enumeration(&m_queue, 10);
    }
    return success;
}
// 0x4ec448
void c_gui_map_subitem_selectable_item_datasource::cancel()
{
    s_gui_map_saved_game_queue *queue = &m_queue;
    queue->cancel.set(1);
    async_block_until_done(&queue->done, simple_yield_function, 1, 0, 0, 0);
}

// 0x4ecca0
long c_gui_map_subitem_selectable_item_datasource::is_enumerating()
{
    return m_enumeration_complete == 0;
}

// 0x4ec5e8
bool c_gui_map_subitem_selectable_item_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        switch (value_name)
        {
        case _string_id_map_id:
        {
            const c_gui_map_selected_item *entry = &m_entries[element_handle];
            *value = entry->get_map_id();
            result = true;
            break;
        }
        case _string_id_locked:
        {
            const c_gui_map_selected_item *entry = &m_entries[element_handle];
            *value = entry->get_location() ? -1 : 0;
            result = true;
            break;
        }
        case _string_id_corrupt_file:
        {
            const c_gui_map_selected_item *view = &m_entries[element_handle];
            *value = -(view->get_enabled() == 0);
            result = true;
            break;
        }
        case _string_id_item_saved:
        {
            const c_gui_map_selected_item *entry = &m_entries[element_handle];
            if (entry->get_location() == 2)
            {
                s_gui_selected_items_block metadata;
                const void *info = entry->get_metadata(&metadata);
                unsigned __int64 unique_id = ((const s_gui_selected_items_block *)info)->unique_id;
                *value = saved_game_files_has_unique_id(m_controller_index, unique_id);
                result = true;
            }
            else
            {
                *value = 1;
                result = true;
            }
            break;
        }
        case _string_id_new:
        {
            const c_gui_map_selected_item *view = &m_entries[element_handle];
            *value = -(view->get_corrupt() == 0);
            result = true;
            break;
        }
        }
    }
    return result;
}

// @address 0x4eccb8 ?map_selected_item_sort_proc@@YAHPBX0@Z
int map_selected_item_sort_proc(const void *a, const void *b)
{
    int result = 0;
    const c_gui_map_selected_item *map_a = (const c_gui_map_selected_item *)a;
    const c_gui_map_selected_item *map_b = (const c_gui_map_selected_item *)b;
    s_gui_selected_items_block metadata_a, metadata_b;
    map_a->get_metadata(&metadata_a);
    map_b->get_metadata(&metadata_b);
    if (map_a->get_special_item_type() < map_b->get_special_item_type()) result = -1;
    else if (map_a->get_special_item_type() > map_b->get_special_item_type()) result = 1;
    else if (map_a->get_location() < map_b->get_location()) result = -1;
    else if (map_a->get_location() > map_b->get_location()) result = 1;
    else if (map_a->get_sort_order() < map_b->get_sort_order()) result = -1;
    else if (map_a->get_sort_order() > map_b->get_sort_order()) result = 1;
    else if (metadata_a.date > metadata_b.date) result = -1;
    else if (metadata_a.date < metadata_b.date) result = 1;
    else result = wcscmp(map_a->get_title(), map_b->get_title());
    return result;
}

// Runs from the datasource's implicit destructor, inlined into its scalar deleting destructor (0x4ec1a8).
inline s_gui_map_saved_game_queue::~s_gui_map_saved_game_queue()
{
    cancel.set(1);
    async_block_until_done(&done, simple_yield_function, 1, 0, 0, 0);
}

// @address 0x4ebfd8 ?get_integer_value@c_gui_map_category_datasource@@UAA_NJJPAJ@Z
bool c_gui_map_category_datasource::get_integer_value(long element_handle, long value_name, long * value)
{
    bool result = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_item_count) {
        switch (value_name) {
        case _string_id_category:
        case _string_id_map_id:
            *value = m_items[element_handle].id;
            _ReadWriteBarrier();
            result = true;
            break;
        case _string_id_image_index:
            if (m_items[element_handle].type == 0) {
                *value = 0;
                result = true;
            } else if (m_items[element_handle].type == 1) {
                *value = 1;
                result = true;
            }
            break;
        case _string_id_special_status:
            *value = m_items[element_handle].type;
            result = true;
            break;
        }
    }
    return result;
}
