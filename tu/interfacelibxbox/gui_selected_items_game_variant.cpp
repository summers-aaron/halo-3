// interfacelibxbox/gui_selected_items_game_variant.obj: from the records 0x243ad8 0x4ecdf0 0x4ece70 0x4ecf40 0x4ed180 0x4ed200 0x4ed368 0x4ed4f0 0x4ed5d8 0x4ed5e0 0x4ed650 0x4ed6f8 0x4ed720 0x4ed7d0 0x4ed960 0x4eda00 0x4eda50 0x4eda68 0x4edc48 0x4edcd0 0x4eddb8 0x4eddf0 0x4ede00 0x4edf88 0x4ee130 0x4ee148 0x4ee178 0x4f4038
// @flags /TP /O2
#include "tag_files/files_windows_types.h"
#include "tag_files/tag_types.h"
#include "shell/utility_drive.h"
// @address 0x8a1ead g_utility_drive_mounted
#include <string.h>
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/game_globals.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "interface/user_interface.h"
#include "interface/gui_selected_items_game_variant.h"
#include "interface/gui_selected_items.h"
#include "tag_files/files_windows.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "game/game_engine_group.h"
#include "cseries/async.h"
#include "saved_games/saved_game_files.h"
#include "saved_games/content_catalogue_close.h"
#include "interface/user_interface_error_manager.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "saved_games/content_item_types.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_catalogue_iteration.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "game/game_engine_variant_lookup.h"
#include "text/string_list_strings.h"
#include "cseries/crt_xdk.h"
#include "interface/gui_selected_items_game_variant_dependencies.h"
// @address 0x243ad8 ?get_game_engine_item_count@c_gui_game_variant_selected_item@@SAJW4e_game_engine_type@@@Z
// @address 0x4ed180 ?get_file_path@c_gui_game_variant_selected_item@@UBA_NPADJ@Z
// @address 0x4ed368 ?add_game_engine@c_gui_game_variant_category_datasource@@AAAXPAVc_gui_data@@W4e_game_engine_type@@@Z
// @address 0x4ed4f0 ?get_game_engine_sort_order@c_gui_game_variant_category_datasource@@KAJPAVc_gui_data@@J@Z
// @address 0x4ed5d8 ?get_current_item_count_internal@c_gui_game_variant_category_datasource@@MAAJXZ
// @address 0x4ed5e0 ?get_text_value@c_gui_game_variant_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4ed6f8 ?get_string_id_value@c_gui_game_variant_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4ed960 ??4c_gui_game_variant_selected_item@@QAAAAV0@ABV0@@Z
// @address 0x4eda50 ?update@c_gui_game_variant_subitem_selectable_item_datasource@@UAAXXZ
// @address 0x4eda68 ?get_integer_value@c_gui_game_variant_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4edcd0 ?get_string_id_value@c_gui_game_variant_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4ede00 ?begin_saved_game_files_enumeration@c_gui_game_variant_subitem_selectable_item_datasource@@QAAXXZ
// @address 0x4edf88 gui_game_variant_begin_autosave_queue_enumeration
// @address 0x4ece70 ??0c_gui_game_variant_selected_item@@QAA@PBUs_gui_selected_items_block@@W4e_gui_selected_item_location@@JPBUs_file_reference@@JJ_N3@Z
// @address 0x4ee130 ?is_busy@c_gui_game_variant_subitem_selectable_item_datasource@@UAA_NXZ
// @address 0x4eddb8 ?get_gui_selected_item@c_gui_game_variant_subitem_selectable_item_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x4eddf0 ?get_current_item_count_internal@c_gui_game_variant_subitem_selectable_item_datasource@@MAAJXZ
// @address 0x4ed720 ??_Gc_gui_game_variant_subitem_selectable_item_datasource@@UAAPAXI@Z
// @address 0x4ed720 ??_Ec_gui_game_variant_subitem_selectable_item_datasource@@UAAPAXI@Z
// @address 0x4ee148 ?game_variant_category_sort_proc@@YAHPBX0@Z
// @address 0x4ee178 game_variant_selected_item_sort_proc
// @address 0x4f4038 ?get_column_names@c_gui_game_variant_category_datasource@@UAAXQAJPAJ@Z
// @address 0x1aea50 ?file_location_get_full_path@@YAXFPBDPADJ@Z
// @address 0x2bc0e8 ?game_engine_settings_try_and_get@@YAPBUs_game_engine_settings_definition@@XZ
// @address 0x58e5c0 memset
// @address 0x1ebb00 ?length@?$c_static_wchar_string@$0EAA@@@QBAJXZ
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x7cf604 ?global_game_globals@@3PAUs_game_globals@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x4ecdf0 ??0c_gui_game_variant_selected_item@@QAA@XZ
// @address 0x566e4 ??_7c_gui_game_variant_selected_item@@6B@
// @address 0x4ed200 ?initialize@c_gui_game_variant_category_datasource@@UAA_NJ@Z
// @address 0x4edc48 ?get_text_value@c_gui_game_variant_subitem_selectable_item_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x8b188 ?interface_get_global_datasource@@YAPAVc_gui_data@@J@Z
#include "game/game_engine_settings_definitions.h"
#include "tag_files/file_reference.h"
#include "interface/gui_simple_selected_item.h"

#include "interface/gui_game_variant_datasource_types.h"
class c_gui_selected_item;

#include "game/game_variant.h"
#include "game/game_engine_base_variant.h"

// the autosave queue variant of the enumeration.


#include "saved_games/saved_game_file_types.h"
#include "cseries/async_helpers.h"
#include "saved_games/saved_game_variant_file_types.h"
#include "game/game_engine_variant_display.h"
#include "tag_files/tag_groups.h"
#include "text/unicode.h"

// Value ids for the category and subitem datasources (gui column string ids).
// Key and result names verified against the cache string table (set 1).
enum
{
    _string_id_category = 254,
    _string_id_title = 0x29d,
    _string_id_description = 0x29e,
    _string_id_locked = 0x1fa,
    _string_id_item_disabled = 0x102ff,
    _string_id_has_unique_id = 0x1030a,
    _string_id_image_index = 0x10327,
    _string_id_disabled = 0x103d7,
    _string_id_item_valid = 0x105bb,
    _string_id_image_name = 0x10326,
    _string_id_no_hdd = 0x102e1,
    _string_id_no_items = 0x102e2,
    _string_id_item_can_be_saved = 0x10309,
    _string_id_item_saved = 0x1030a,
    _string_id_item_built_in = 0x1030b,
    _string_id_item_options = 0x1030c,
    _string_id_recent_games_no_hdd = 0x102e9,
    _string_id_recent_games_no_items = 0x102ea,
    _string_id_game_variant_can_be_saved = 0x10315,
    _string_id_game_variant_saved = 0x10316,
    _string_id_game_variant_built_in = 0x10317,
    _string_id_game_variant_options = 0x10318,
    _string_id_gametype_image = 0x10328,
    _string_id_gametype_variant_image = 0x10329,
};

// 0x4ed7d0

// 0x243ad8
long c_gui_game_variant_selected_item::get_game_engine_item_count(e_game_engine_type game_engine_type)
{
    long result = 0;
    const s_game_engine_settings_definition *settings = game_engine_settings_try_and_get();
    if (settings) {
        switch (game_engine_type) {
        case _game_engine_type_ctf: result = settings->ctf.count; break;
        case _game_engine_type_slayer: result = settings->slayer.count; break;
        case _game_engine_type_oddball: result = settings->oddball.count; break;
        case _game_engine_type_koth: result = settings->juggernaut.count; break;
        case _game_engine_type_sandbox: result = settings->sandbox.count; break;
        case _game_engine_type_vip: result = settings->infection.count; break;
        case _game_engine_type_jugg: result = settings->assault.count; break;
        case _game_engine_type_terries: result = settings->territories.count; break;
        case _game_engine_type_assault: result = settings->king.count; break;
        case _game_engine_type_zombiez: result = settings->vip.count; break;
        }
    }
    return result;
}

c_gui_game_variant_selected_item::c_gui_game_variant_selected_item() :
    c_gui_simple_selected_item(_gui_selected_item_none, L"", L"", c_gui_selected_item::_special_item_type_none,
        _gui_selected_item_location_none, 0, 0)
{
    controller = -1;
    builtin = -1;
    memset(&file, 0, sizeof(file));
    sort = 0;
}

c_gui_game_variant_selected_item::c_gui_game_variant_selected_item(const s_gui_selected_items_block *metadata,
    e_gui_selected_item_location location, long controller, const s_file_reference *file, long builtin, long sort, bool corrupt,
    bool is_new) :
    c_gui_simple_selected_item(_gui_selected_item_game_variant, metadata, location, corrupt, is_new)
{
    this->controller = controller;
    this->builtin = builtin;
    memset(&this->file, 0, sizeof(this->file));
    this->sort = sort;
    if (file != 0)
        memcpy(&this->file, file, offsetof(s_file_reference, handle));
}

bool c_gui_game_variant_selected_item::get_file_path(char *buffer, long size) const
{
    if (get_location() != 1 && get_location() != 2)
        return false;
    const s_file_reference *reference = &file;
    file_location_get_full_path(reference->location, reference->path, buffer, size);
    return true;
}

// 0x4ed200

bool c_gui_game_variant_category_datasource::initialize(long name)
{
    m_name = name;
    c_gui_data *global_datasource = interface_get_global_datasource(502);
    m_item_count = 0;
    if (global_datasource)
    {
        m_categories[0].game_engine_type = 0;
        s_game_variant_category *categories = m_categories.m_storage;
        m_categories[m_item_count].sort_order = 0;
        m_categories[m_item_count].name.set(m_title.m_string);
        ((c_static_wchar_string<256> *)m_categories[m_item_count].description)->set(m_description.m_string);
        m_categories[m_item_count].sort_key = -2;
        ++m_item_count;
        if (g_utility_drive_mounted)
        {
            m_categories[m_item_count].game_engine_type = 1;
            m_categories[m_item_count].sort_order = 0;
            m_categories[m_item_count].name.set(m_default_title.m_string);
            ((c_static_wchar_string<256> *)m_categories[m_item_count].description)->set(m_default_description.m_string);
            m_categories[m_item_count].sort_key = -1;
            ++m_item_count;
        }
        for (long game_engine_type = 0; game_engine_type < 11; ++game_engine_type)
        {
            if (game_engine_type != 0 && game_engine_type != 5)
                add_game_engine(global_datasource, (e_game_engine_type)game_engine_type);
        }
        qsort(categories, m_item_count, sizeof(s_game_variant_category), game_variant_category_sort_proc);
    }
    return true;
}

// March 82c89e30: public wrapper around the variant's protected virtual.
__forceinline void c_game_engine_base_variant::get_game_engine_description(c_static_wchar_string<1024> *description) const
{
    description->m_string[0] = 0;
    if (global_game_globals != 0 && global_game_globals->multiplayer_globals_index != -1)
    {
        const s_multiplayer_globals_definition *tag_instance =
            TAG_GET('mulg', const s_multiplayer_globals_definition, global_game_globals->multiplayer_globals_index);
        if (tag_instance->universal.count > 0)
        {
            const s_multiplayer_universal_globals_definition *tag = (const s_multiplayer_universal_globals_definition *)tag_instance->universal.address;
            long string_id = get_game_engine_default_description_string_id();
            if (string_id != -1)
                text_group_get_string(tag->strings_index, string_id, description);
        }
    }
}

// 0x4ed368
void c_gui_game_variant_category_datasource::add_game_engine(c_gui_data *gui_data, e_game_engine_type game_engine_type)
{
    c_static_wchar_string<1024> name;
    c_static_wchar_string<1024> description;
    name.m_string[0] = 0;
    c_game_variant variant;
    game_engine_variant_initialize_default(&variant, game_engine_type);
    variant.get_active_variant()->get_game_engine_name(&name);
    variant.get_active_variant()->get_game_engine_description(&description);
    if (name.length() > 0 && c_gui_game_variant_selected_item::get_game_engine_item_count(game_engine_type) > 0)
    {
        m_categories[m_item_count].game_engine_type = 2;
        m_categories[m_item_count].sort_order = game_engine_type;
        m_categories[m_item_count].name.set(name.m_string);
        ((c_static_wchar_string<256> *)m_categories[m_item_count].description)->set(description.m_string);
        s_game_variant_category *category = &m_categories[m_item_count];
        category->sort_key = get_game_engine_sort_order(gui_data, game_engine_type);
        ++m_item_count;
    }
}

// 0x4ed4f0
long c_gui_game_variant_category_datasource::get_game_engine_sort_order(c_gui_data *datasource, long game_engine_type)
{
    long sort_order = 0;
    for (long element_handle = datasource->get_first_element_handle(); element_handle != -1;
         element_handle = datasource->get_next_element_handle(element_handle))
    {
        long engine_type = -1;
        long engine_sort_order = -1;
        if (datasource->get_integer_value(element_handle, 0x1000e, &engine_type) &&
            engine_type == game_engine_type &&
            datasource->get_integer_value(element_handle, 0x1000f, &engine_sort_order))
        {
            sort_order = engine_sort_order;
            break;
        }
    }
    return sort_order;
}

// 0x4ed5d8
long c_gui_game_variant_category_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

// 0x4ed5e0
bool c_gui_game_variant_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_item_count) {
        switch (value_name) {
        case _string_id_title:
            value->set(m_categories[element_handle].name);
            found = true;
            break;
        case _string_id_description:
            value->set(m_categories[element_handle].description);
            found = true;
            break;
        }
    }
    return found;
}

// 0x4ed650
bool c_gui_game_variant_category_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool valid = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_item_count) {
        switch (value_name) {
        case _string_id_category:
            *value = m_categories[element_handle].sort_order;
            valid = true;
            break;
        case _string_id_image_index:
            if (m_categories[element_handle].game_engine_type == 0)
                *value = 0;
            else if (m_categories[element_handle].game_engine_type == 1)
                *value = 11;
            else
                *value = m_categories[element_handle].sort_order;
            valid = true;
            break;
        case _string_id_disabled:
            *value = m_categories[element_handle].game_engine_type;
            valid = true;
            break;
        }
    }
    return valid;
}
// 0x4ed6f8
bool c_gui_game_variant_category_datasource::get_string_id_value(long context, long string_id, long *result)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *result = _string_id_gametype_image;
        found = true;
    }
    return found;
}

// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z

static __forceinline void gui_selected_items_game_variant_internal_async_yield_until_done(
    c_synchronized_long *done, bool with_spinner, bool with_idle, long file, long line)
{
    async_block_until_done(done, simple_yield_function, with_spinner, with_idle, file, line);
}

// @address 0x4eda00 ?dispose@c_gui_game_variant_subitem_selectable_item_datasource@@UAAXXZ
void c_gui_game_variant_subitem_selectable_item_datasource::dispose()
{
    m_queue.cancel_and_wait();
}

// 0x4eda50
void c_gui_game_variant_subitem_selectable_item_datasource::update()
{
    if (m_game_engine_type == 0)
        begin_saved_game_files_enumeration();
    else
        gui_game_variant_begin_autosave_queue_enumeration(this);
}

// 0x4eda68
bool c_gui_game_variant_subitem_selectable_item_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool valid = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_item_count) {
        switch (value_name) {
        case _string_id_category:
        case _string_id_image_index: {
            c_gui_game_variant_selected_item *item = &m_entries[element_handle];
            *value = item->get_game_engine_type();
            valid = true;
            break;
        }
        case _string_id_locked: {
            c_gui_game_variant_selected_item *item = &m_entries[element_handle];
            *value = item->get_location() ? -1 : 0;
            valid = true;
            break;
        }
        case _string_id_item_disabled: {
            c_gui_game_variant_selected_item *item = &m_entries[element_handle];
            *value = -(item->get_enabled() == 0);
            valid = true;
            break;
        }
        case _string_id_has_unique_id: {
            c_gui_game_variant_selected_item *item = &m_entries[element_handle];
            if (item->get_location() == _gui_stored_item_location_autosave_queue) {
                s_gui_selected_items_block metadata;
                const unsigned __int64 *unique_id = (const unsigned __int64 *)item->get_metadata(&metadata);
                *value = (unsigned char)saved_game_files_has_unique_id(m_controller_index, *unique_id);
                valid = true;
                break;
            } else {
                *value = 1;
                valid = true;
                break;
            }
        }
        case _string_id_item_valid: {
            c_gui_game_variant_selected_item *item = &m_entries[element_handle];
            *value = -(item->get_corrupt() == 0);
            valid = true;
            break;
        }
        }
    }
    return valid;
}

// 0x4edc48

bool c_gui_game_variant_subitem_selectable_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        const c_gui_game_variant_selected_item *item = &m_entries[element_handle];
        s_gui_selected_items_block metadata;
        result = saved_game_files_get_metadata_string(item->get_metadata(&metadata), value_name, value);
    }
    return result;
}

// 0x4edcd0
bool c_gui_game_variant_subitem_selectable_item_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _string_id_gametype_variant_image;
        found = true;
    } else if (value_name == _string_id_no_hdd) {
        *value = _string_id_recent_games_no_hdd;
        found = true;
    } else if (value_name == _string_id_no_items) {
        *value = _string_id_recent_games_no_items;
        found = true;
    } else if (value_name == _string_id_item_can_be_saved) {
        *value = _string_id_game_variant_can_be_saved;
        found = true;
    } else if (value_name == _string_id_item_saved) {
        *value = _string_id_game_variant_saved;
        found = true;
    } else if (value_name == _string_id_item_built_in) {
        *value = _string_id_game_variant_built_in;
        found = true;
    } else if (value_name == _string_id_item_options) {
        *value = _string_id_game_variant_options;
        found = true;
    }
    return found;
}

const c_gui_selected_item *c_gui_game_variant_subitem_selectable_item_datasource::get_gui_selected_item(long index) const
{
    const c_gui_game_variant_selected_item *result = 0;
    if (index >= 0 && index < m_item_count)
        result = &m_entries[index];
    return (const c_gui_selected_item *)result;
}

// 0x4eddf0
long c_gui_game_variant_subitem_selectable_item_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

// 0x4ede00

void c_gui_game_variant_subitem_selectable_item_datasource::begin_saved_game_files_enumeration()
{
    if (!m_enumeration_complete) {
        s_saved_game_queue_item item;
        while (m_queue.has_items() && saved_game_queue_try_and_get_next(&m_queue, &item)) {
            if (item.valid == 1 && m_item_count < 151) {
                s_gui_selected_items_block metadata;
                gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&metadata), &item.metadata);
                long index = m_item_count++;
                m_entries[index] = c_gui_game_variant_selected_item(&metadata, _gui_stored_item_location_autosave_queue, m_controller_index, &item.file,
                    index + 1, 0, item.valid == 2, false);
            }
        }
#ifdef HALO3_64BIT
        qsort(m_entries, m_item_count, sizeof(m_entries[0]), game_variant_selected_item_sort_proc);
#else
        qsort(m_entries, m_item_count, 0x2b0, game_variant_selected_item_sort_proc);
#endif
        if (!m_queue.active_reload() && !m_queue.has_items())
            m_enumeration_complete = true;
    }
}

extern "C" void gui_game_variant_begin_autosave_queue_enumeration(c_gui_game_variant_subitem_selectable_item_datasource *datasource)
{
    if (datasource->m_enumeration_complete)
        return;
    c_content_catalogue *catalogue = &content_catalogues().catalogues[datasource->m_controller_index];
    if (!content_catalogue_has_storage_device(catalogue))
        return;
    if (!(bool)((catalogue->m_flags.value >> 1) & 1))
        return;
    long index = content_catalogue_get_first_item_index(catalogue);
    if (index != -1) {
        do {
            if (datasource->m_item_count >= 151)
                break;
            s_saved_game_item_metadata_packed metadata;
            if (content_catalogue_item_metadata_matches(catalogue, index, &metadata)) {
                bool active = content_catalogue_is_item_corrupt(catalogue, index);
                if ((unsigned int)(metadata.file_type - 1) <= 8 && (metadata.game_engine_type == datasource->m_game_engine_type || active)) {
                    s_file_reference reference;
                    if (::c_start_menu_game_editor::load_progress_ui_for_catalogue(
                            (c_start_menu_game_editor *)catalogue, (e_controller_index)index, (s_file_reference *)&reference))
                    {
                        s_gui_selected_items_block display;
                        gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&display), (const s_saved_game_item_metadata_packed *)(&metadata));
                        long slot = datasource->m_item_count;
                        datasource->m_item_count = slot + 1;
                        datasource->m_entries[slot] = c_gui_game_variant_selected_item(&display, _gui_stored_item_location_saved_game_file,
                            datasource->m_controller_index, &reference, datasource->m_item_count, 0, active,
                            content_catalogue_is_item_created(catalogue, index));
                    }
                }
            }
            index = content_catalogue_get_next_item_index(catalogue, index);
        } while (index != -1);
    }
#ifdef HALO3_64BIT
    qsort(datasource->m_entries, datasource->m_item_count, sizeof(datasource->m_entries[0]), game_variant_selected_item_sort_proc);
#else
    qsort(datasource->m_entries, datasource->m_item_count, 0x2b0, game_variant_selected_item_sort_proc);
#endif
    datasource->m_enumeration_complete = 1;
}

// True while the subitem list is still being enumerated.
bool c_gui_game_variant_subitem_selectable_item_datasource::is_busy()
{
    return m_enumeration_complete == 0;
}

int game_variant_category_sort_proc(const void *p0, const void *p1)
{
    const c_gui_game_variant_category_datasource::s_game_variant_category *a =
        (const c_gui_game_variant_category_datasource::s_game_variant_category *)p0;
    const c_gui_game_variant_category_datasource::s_game_variant_category *b =
        (const c_gui_game_variant_category_datasource::s_game_variant_category *)p1;
    if (a->sort_key < b->sort_key)
        return -1;
    if (a->sort_key > b->sort_key)
        return 1;
    return wcscmp(a->name, b->name);
}

// 0x4ee178
extern "C" int game_variant_selected_item_sort_proc(const void *a, const void *b)
{
    int result = 0;
    const c_gui_game_variant_selected_item *item_a = (const c_gui_game_variant_selected_item *)a;
    const c_gui_game_variant_selected_item *item_b = (const c_gui_game_variant_selected_item *)b;
    s_gui_selected_items_block metadata_a, metadata_b;
    item_a->get_metadata(&metadata_a);
    item_b->get_metadata(&metadata_b);
    if (item_a->get_special_item_type() > item_b->get_special_item_type()) result = -1;
    else if (item_a->get_special_item_type() < item_b->get_special_item_type()) result = 1;
    else if (item_a->get_location() < item_b->get_location()) result = -1;
    else if (item_a->get_location() > item_b->get_location()) result = 1;
    else if (item_a->get_sort_order() < item_b->get_sort_order()) result = -1;
    else if (item_a->get_sort_order() > item_b->get_sort_order()) result = 1;
    else if (metadata_a.date > metadata_b.date) result = -1;
    else if (metadata_a.date < metadata_b.date) result = 1;
    else result = wcscmp(item_a->get_title(), item_b->get_title());
    return result;
}

// 0x4f4038
void c_gui_game_variant_category_datasource::get_column_names(long *const column_names, long *count)
{
    *count = 0;
    column_names[0] = _string_id_category;
    column_names[++*count] = _string_id_title;
    column_names[++*count] = _string_id_description;
    ++*count;
}

// 0x4ecf40
class c_game_variant;
class c_content_catalogue;

static __forceinline long gui_selected_items_game_variant_synchronized_peek(const c_synchronized_long *value)
{
    long result = value->value;
    __lwsync();
    return result;
}

bool c_gui_game_variant_selected_item::get_game_variant(c_game_variant *game_variant) const
{
    bool result = false;
    switch (get_location()) {
    case 0:
        result = game_engine_tag_defined_variant_get_built_in_variant((e_game_engine_type)m_metadata.game_engine_type, builtin, game_variant);
        break;
    case 1:
        if (game_engine_index_to_saved_game_file_type((e_game_engine_type)m_metadata.game_engine_type) != _saved_game_file_type_none) {
            c_content_catalogue *catalogue = &content_catalogues().catalogues[controller];
            long content_item_index = content_catalogue_find_item_by_file_reference(catalogue, &file);
            if (content_item_index != -1 && ((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true)) {
                s_blffile_game_variant blf_file;
                c_synchronized_long success;
                c_synchronized_long done;
                if (saved_game_files_read(controller, content_item_index, &blf_file, sizeof(blf_file), &success, &done) != -1) {
                    gui_selected_items_game_variant_internal_async_yield_until_done(&done, false, false, 0, 0);
                    if (gui_selected_items_game_variant_synchronized_peek(&success) == 1) {
                        bool valid = false;
                        if (blf_file.copy_to_and_validate(game_variant, &valid)) {
                            result = true;
                            if (!valid)
                                g_gui_error_manager.post_error_with_custom_message(0x2004f, 0, 0, (e_controller_index)controller, false, true);
                        } else {
                            g_gui_error_manager.post_error_with_custom_message(0x2004f, 0, 0, (e_controller_index)controller, false, true);
                        }
                    }
                }
                content_catalogue_close_item(catalogue, content_item_index, true);
            }
        }
        break;
    default:
        s_blffile_game_variant blf_file;
        c_synchronized_long success;
        c_synchronized_long done;
        char file_path[256];
        const s_file_reference *reference = &file;
        file_location_get_full_path(reference->location, reference->path, file_path, sizeof(file_path));
        if (async_read_entire_file(file_path, &blf_file, sizeof(blf_file), 0, _async_category_saved_games, _async_priority_blocking_generic,
                &success, &done) != -1)
        {
            gui_selected_items_game_variant_internal_async_yield_until_done(&done, false, false, 0, 0);
            if (gui_selected_items_game_variant_synchronized_peek(&success) == 1) {
                bool valid = false;
                if (blf_file.copy_to_and_validate(game_variant, &valid))
                    result = true;
            }
        }
        break;
    }
    return result;
}

// @address 0x4ed7d0 ?initialize@c_gui_game_variant_subitem_selectable_item_datasource@@UAA_NJW4e_game_engine_type@@W4e_controller_index@@@Z
bool c_gui_game_variant_subitem_selectable_item_datasource::initialize(long name, e_game_engine_type game_engine_type, e_controller_index controller_index)
{
    m_name = name;
    m_game_engine_type = game_engine_type;
    m_controller_index = controller_index;
    if (game_engine_type != _game_engine_type_none)
    {
        if (game_engine_settings_try_and_get())
        {
            long item_count = c_gui_game_variant_selected_item::get_game_engine_item_count(game_engine_type);
            for (long item_index = 0; item_index < item_count; item_index++)
            {
                c_static_wchar_string<48> variant_name;
                c_static_wchar_string<256> variant_description;
                variant_name.m_string[0] = 0;
                variant_description.m_string[0] = 0;
                if (game_engine_tag_defined_variant_get_strings(game_engine_type, item_index, &variant_name, &variant_description) && m_item_count < 151)
                {
                    s_saved_game_item_metadata_packed metadata;
                    saved_game_files_initialize_metadata(&metadata, -1, game_engine_index_to_saved_game_file_type(game_engine_type),
                        variant_name.m_string, (const wchar_t *)L"", 0, 0);
                    metadata.game_engine_type = game_engine_type;
                    s_gui_selected_items_block display;
                    gui_selected_items_unpack_metadata((s_gui_selected_items_block *)(&display), (const s_saved_game_item_metadata_packed *)(&metadata));
                    ustrnzcpy_inline(display.description, variant_description.m_string, 128);
                    long index = m_item_count++;
                    m_entries[index] = c_gui_game_variant_selected_item(&display, _gui_stored_item_location_built_in, controller_index, 0,
                        item_index, item_index, false, false);
                }
            }
        }
    }
    else
    {
        saved_game_queue_start_enumeration(&m_queue, 15);
    }
    return true;
}

// @address 0x4ec4b0 ?get_column_names@c_gui_game_variant_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
void c_gui_game_variant_subitem_selectable_item_datasource::get_column_names(long *const names, long *count)
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
