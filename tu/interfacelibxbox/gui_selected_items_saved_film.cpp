// interfacelibxbox/gui_selected_items_saved_film.obj: from the records 0x221c08 0x221c88 0x4ee2b0 0x4ee330 0x4ee3f0 0x4ee5e0 0x4ee618 0x4ee738 0x4ee7a8 0x4ee8b0 0x4ee960 0x4ee9d0 0x4ee9d8 0x4eea00 0x4eeab8 0x4eec00 0x4eec20 0x4eed78 0x4ef070 0x4ef0f8 0x4ef218 0x4ef250 0x4ef260 0x4ef3f0 0x4ef490 0x4ef638 0x4ef650 0x4ef768
// @flags /TP /O2 /Oc
// @address 0x27e3b0 ??0s_blf_saved_film@@QAA@XZ
#include <stddef.h>
#include "shell/utility_drive.h"
// @address 0x8a1ead g_utility_drive_mounted
#include "game/map_variant.h"
// @address 0x1aea50 ?file_location_get_full_path@@YAXFPBDPADJ@Z
// @address 0x46f190 ?get_alpha_is_internal_beta@@YA_NXZ
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x4ee3f0 ?get_variants@c_gui_saved_film_selected_item@@QBA_NPAXPAVc_game_variant@@PAVc_map_variant@@@Z
// @address 0x4ee5e0 ?get_file_path@c_gui_saved_film_selected_item@@UBA_NPADJ@Z
// @address 0x4ee618 ?initialize@c_gui_saved_film_category_datasource@@UAA_NJ@Z
// @address 0x4ee7a8 ?add_category@c_gui_saved_film_category_datasource@@AAAXJJW4e_saved_film_category@@J@Z
// @address 0x4ee8b0 ?get_integer_value@c_gui_saved_film_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4ee960 ?get_text_value@c_gui_saved_film_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4ee9d0 ?get_current_item_count_internal@c_gui_saved_film_category_datasource@@MAAJXZ
// @address 0x4ee9d8 ?get_string_id_value@c_gui_saved_film_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4eea00 ??0c_gui_saved_film_subitem_datasource@@QAA@HH@Z
// @address 0x482078 ?get_screen_by_screen_index@c_window_manager@@QAAPAVc_gui_screen_widget@@J@Z
// @address 0x4ee2b0 ??0c_gui_saved_film_selected_item@@QAA@XZ
// @address 0x491d68 ??0c_gui_simple_selected_item@@QAA@W4e_gui_selected_item_type@@PB_W1W4e_special_item_type@c_gui_selected_item@@W4e_gui_selected_item_location@@JJ@Z
// @address 0x4ef3f0 ??4c_gui_saved_film_selected_item@@QAAAAV0@ABV0@@Z
// @address 0x4ef218 ?get_gui_selected_item@c_gui_saved_film_subitem_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x4ef070 ?get_text_value@c_gui_saved_film_subitem_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x284b30 content_catalogue_item_metadata_matches
// @address 0x4eed78 ?get_integer_value@c_gui_saved_film_subitem_datasource@@UAA_NJJPAJ@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "memory/static_arrays.h"
#include "interface/gui_data.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "text/unicode.h"
#include "tag_files/files_windows.h"
#include "interface/user_interface.h"
#include "cseries/async.h"
#include "shell/shell_types.h"
#include "game/game_variant.h"
#include "interface/gui_selected_items_saved_film.h"
#include "interface/gui_selected_items.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/file_reference.h"
#include "tag_files/string_ids.h"
#include "saved_games/saved_game_file_category_types.h"
#include "saved_games/saved_game_item_metadata.h"

#include "interface/gui_selected_items_saved_film_externs.h"
#include "saved_games/content_catalogue_iteration.h"
#include "saved_games/content_item_types.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"
// @address 0x567f4 ??_7c_gui_screenshot_selected_item@@6B@
// @address 0x567f4 ??_7c_gui_saved_film_selected_item@@6B@

#include "saved_games/content_catalogue_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/saved_film_recording_types.h"
#include "multithreading/synchronized_value.h"
// match: 0x4ee3f0: get_variants was tuned against non-initializing workspace flags.
// Keep this unit's flag parameters and fields on one private type until that function is re-matched.

struct s_saved_film_workspace {
    bool invalid;
    gui_selected_items_saved_film_synchronized_long saved_complete;
    gui_selected_items_saved_film_synchronized_long saved_success;
    gui_selected_items_saved_film_synchronized_long builtin_success;
    gui_selected_items_saved_film_synchronized_long builtin_complete;
    __declspec(align(16)) char file_path[256];
    s_blf_saved_film film;
};

   /* ustrnzcpy */

#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_window_manager.h"



#include "interface/gui_simple_selected_item.h"
#include "saved_games/autosave_queue_types.h"

   /* g_window_manager */

// c_static_wchar_string<1024>::set declared in the template

// @address 0x56890 ??_7c_gui_saved_film_subitem_datasource@@6B@
// @address 0x4eeab8 ??_Gc_gui_saved_film_subitem_datasource@@UAAPAXI@Z
// @address 0x4eeab8 ??_Ec_gui_saved_film_subitem_datasource@@UAAPAXI@Z

#include <new>
#include "saved_games/saved_game_file_types.h"

// The enumeration writes a full queue item; the trailing bytes are never read here.
struct s_saved_film_scratch { s_saved_game_queue_item item; char trailing_bytes[8]; };

// @address 0x221c88 ??0c_gui_screenshot_selected_item@@QAA@PBUs_saved_game_item_metadata_packed@@PBD_K2W4e_gui_selected_item_location@@JPBUs_file_reference@@_N@Z
c_gui_screenshot_selected_item::c_gui_screenshot_selected_item(const s_saved_game_item_metadata_packed *metadata,
    const char *author_name, unsigned __int64 creation_date, unsigned __int64 file_size,
    e_gui_selected_item_location, long controller, const s_file_reference *reference, bool corrupt) :
    c_gui_simple_selected_item(_gui_selected_item_screenshot, metadata, _gui_stored_item_location_saved_game_file, corrupt, false)
{
    controller_index = controller;
    memset(&file_reference, 0, sizeof(file_reference));
    date = creation_date;
    size_in_bytes = file_size;
    bitmap_list_item_index = -1;
    memcpy(&file_reference, reference, offsetof(s_file_reference, handle));
    csstrnzcpy(author, author_name, sizeof(author));
}

c_gui_saved_film_selected_item::c_gui_saved_film_selected_item() :
    c_gui_simple_selected_item(_gui_selected_item_none, L"", L"",
        c_gui_selected_item::_special_item_type_none, _gui_selected_item_location_none, 0, 0)
{
    controller_index = -1;
    memset(&file_reference, 0, sizeof(file_reference));
    m_sort_key = 0;
    m_saved_film_category = _saved_film_category_none;
}

// @address 0x221c08 ??0c_gui_screenshot_selected_item@@QAA@XZ
c_gui_screenshot_selected_item::c_gui_screenshot_selected_item() :
    c_gui_simple_selected_item(_gui_selected_item_none, L"", L"",
        c_gui_selected_item::_special_item_type_none, _gui_selected_item_location_none, 0, 0)
{
    controller_index = -1;
    memset(&file_reference, 0, sizeof(file_reference));
    bitmap_list_item_index = -1;
}

// @address 0x4ee330 ??0c_gui_saved_film_selected_item@@QAA@PBUs_saved_game_item_metadata_packed@@W4e_saved_film_category@@W4e_gui_selected_item_location@@JPBUs_file_reference@@J_N4@Z
c_gui_saved_film_selected_item::c_gui_saved_film_selected_item(const s_saved_game_item_metadata_packed *metadata,
    e_saved_film_category category, e_gui_selected_item_location location, long controller,
    const s_file_reference *reference, long, bool corrupt, bool created) :
    c_gui_simple_selected_item(_gui_selected_item_film, metadata, location, corrupt, created)
{
    controller_index = controller;
    memset(&file_reference, 0, sizeof(file_reference));
    m_saved_film_category = category;
    m_sort_key = 0;
    memcpy(&file_reference, reference, offsetof(s_file_reference, handle));
}

bool c_gui_saved_film_selected_item::get_variants(void * game_variant, class c_game_variant * _sym_p2, class c_map_variant * _sym_p3) const
{
    bool result = false;
    s_saved_film_workspace work;
    if (get_location() != 1) {
        const s_file_reference *reference = &this->file_reference;
        work.builtin_success.value = 0;
        work.builtin_complete.value = 0;
        file_location_get_full_path(reference->location, reference->path, work.file_path, sizeof(work.file_path));
        int task = async_read_entire_file(work.file_path, &work.film, sizeof(work.film), 0, 8, 5, &work.builtin_success, &work.builtin_complete);
        if (task != -1) {
            async_block_until_done(&work.builtin_complete, simple_yield_function, 0, 0, 0, 0);
            if (work.builtin_success.peek() == 1) {
                work.invalid = false;
                if (saved_film_parse_options_from_buffer(&work.film, game_variant, ((void *)_sym_p2), ((void *)_sym_p3), &work.invalid))
                    result = true;
            }
        }
    } else {
        c_content_catalogue *catalogue = &content_catalogues().catalogues[this->controller_index];
        int content_item = content_catalogue_find_item_by_file_reference(catalogue, &this->file_reference);
        if (content_item != -1 && ((unsigned char)((s_content_item *)catalogue)->make_ready_for_access(content_item, 1))) {
            work.saved_success.value = 0;
            work.saved_complete.value = 0;
            int task = saved_game_files_read(this->controller_index, content_item, &work.film,
                sizeof(work.film), &work.saved_success, &work.saved_complete);
            if (task != -1) {
                async_block_until_done(&work.saved_complete, simple_yield_function, 0, 0, 0, 0);
                if (work.saved_success.peek() == 1) {
                    work.invalid = false;
                    if (saved_film_parse_options_from_buffer(&work.film, game_variant, ((void *)_sym_p2), ((void *)_sym_p3), &work.invalid))
                        result = true;
                }
            }
            content_catalogue_close_item(catalogue, content_item, 1);
        }
    }
    return result;
}

bool c_gui_saved_film_selected_item::get_file_path(char *buffer, long size) const
{
    const s_file_reference *reference = &file_reference;
    file_location_get_full_path((unsigned short)reference->location, reference->path, buffer, size);
    return 1;
}

bool c_gui_saved_film_category_datasource::initialize(long datasource_name)

{
  unsigned char internal_beta;
  
  m_name = datasource_name;
  c_gui_saved_film_category_datasource *ds = this;
  ds->add_category(_string_id_recent_films, _string_id_recent_films_description, _saved_film_category_recent_films, 0);
  if (g_utility_drive_mounted) {
    ds->add_category(_string_id_file_share_owner, _string_id_file_share_owner_description, _saved_film_category_none, 1);
  }
  internal_beta = get_alpha_is_internal_beta();
  if (internal_beta == '\0') {
    ds->add_category(_string_id_film_clip, _string_id_film_clip_description, _saved_film_category_film_clip, 2);
    ds->add_category(_string_id_campaign_film, _string_id_campaign_film_description, _saved_film_category_campaign_film, 2);
  }
  ds->add_category(_string_id_multiplayer_film, _string_id_multiplayer_film_description, _saved_film_category_multiplayer_film, 2);
  internal_beta = get_alpha_is_internal_beta();
  if (internal_beta == '\0') {
    ds->add_category(_string_id_editor_film, _string_id_editor_film_description, _saved_film_category_editor_film, 2);
  }
  return 1;
}

// @address 0x4ee738 ?get_column_names@c_gui_saved_film_category_datasource@@UAAXQAJPAJ@Z
void c_gui_saved_film_category_datasource::get_column_names(long *const column_names, long *count)
{
    *count = 0;
    column_names[0] = _string_id_category;
    *count = *count + 1;
    column_names[*count] = _string_id_title;
    *count = *count + 1;
    column_names[*count] = _string_id_description;
    *count = *count + 1;
    column_names[*count] = _string_id_special_status;
    *count = *count + 1;
}

void c_gui_saved_film_category_datasource::add_category(long title_id, long description_id, e_saved_film_category category, long mode)
{
    c_static_wchar_string<1024> title;
    c_static_wchar_string<1024> description;
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_screen_index(m_screen_index);
    if (m_category_count < 7 && screen->get_string_by_string_id(title_id, &title) && screen->get_string_by_string_id(description_id, &description))
    {
        title.copy_to(m_categories[m_category_count].title, 48);
        description.copy_to(m_categories[m_category_count].description, 256);
        m_categories[m_category_count].category = category;
        m_categories[m_category_count].mode = mode;
        m_category_count++;
    }
}

bool c_gui_saved_film_category_datasource::get_integer_value(long element_handle, long value_name, long *value_address)
{
    bool result = false;
    *value_address = -1;
    if (element_handle >= 0 && element_handle < m_category_count)
    {
        switch (value_name)
        {
        case _saved_film_string_special_status:
            result = true;
            *value_address = m_categories[element_handle].mode;
            break;
        case _saved_film_string_image_index:
        {
            result = true;
            long category_mode = m_categories[element_handle].mode;
            if (category_mode == 0)
                *value_address = 0;
            else if (category_mode == 1)
                result = *value_address = 1;
            else
                *value_address = m_categories[element_handle].category;
            break;
        }
        case _saved_film_string_category:
            result = true;
            *value_address = m_categories[element_handle].category;
            break;
        }
    }
    return result;
}

bool c_gui_saved_film_category_datasource::get_text_value(long category_entry_index, long field_id, c_static_wchar_string<1024> *out_string)
{
    bool success = false;
    if (category_entry_index >= 0 && category_entry_index < m_category_count)
    {
        if (field_id != _saved_film_string_title)
        {
            if (field_id == _saved_film_string_description)
            {
                out_string->set(m_categories[category_entry_index].description);
                success = true;
            }
        }
        else
        {
            out_string->set(m_categories[category_entry_index].title);
            success = true;
        }
    }
    return success;
}

long c_gui_saved_film_category_datasource::get_current_item_count_internal()
{
    return m_category_count;
}

bool c_gui_saved_film_category_datasource::get_string_id_value(long element_handle, long string_id, long *result)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *result = _string_id_film_category_image;
        found = true;
    }
    return found;
}

c_gui_saved_film_subitem_datasource::c_gui_saved_film_subitem_datasource(int index,
    int cat) : c_gui_ordered_data(), item_count(0),category(cat),controller(index),busy(false) {}

// 0x4eed78
bool c_gui_saved_film_subitem_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool valid = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < this->item_count) {
        switch (value_name) {
        case _saved_film_string_locked:
            *value = this->items[element_handle].get_location() ? -1 : 0;
            valid = true;
            break;
        case _saved_film_string_game_engine_index: {
            const s_gui_selected_items_block *item = &this->items[element_handle].m_metadata;
            if (item->campaign_id == -1)
                *value = item->game_engine_type;
            else
                *value = -1;
            valid = true;
            break;
        }
        case _saved_film_string_map_id: {
            const s_gui_selected_items_block *item = &this->items[element_handle].m_metadata;
            *value = item->map_id;
            valid = true;
            break;
        }
        case _saved_film_string_corrupt_file:
            *value = -(this->items[element_handle].get_enabled() == 0);
            valid = true;
            break;
        case _saved_film_string_item_saved: {
            c_gui_saved_film_selected_item *item = &this->items[element_handle];
            if (item->get_location() == 2) {
                s_gui_selected_items_block metadata;
                const unsigned __int64 *unique_id = (const unsigned __int64 *)item->get_metadata(&metadata);
                *value = (unsigned char)saved_game_files_has_unique_id(this->controller, *unique_id);
                valid = true;
            } else {
                *value = 1;
                valid = true;
            }
            break;
        }
        case _saved_film_string_image_index: {
            c_gui_saved_film_selected_item *item = &this->items[element_handle];
            s_gui_selected_items_block metadata;
            long file_type = ((const s_gui_selected_items_block *)item->get_metadata(&metadata))->file_type;
            if (file_type != 11) {
                if (file_type == 12) {
                    *value = 1;
                    valid = true;
                }
            } else {
                *value = 0;
                valid = true;
            }
            break;
        }
        case _saved_film_string_map_image_type: {
            c_gui_saved_film_selected_item *item = &this->items[element_handle];
            s_gui_selected_items_block metadata;
            long file_type = ((const s_gui_selected_items_block *)item->get_metadata(&metadata))->file_type;
            if (file_type != 11) {
                if (file_type == 12) {
                    *value = 4;
                    valid = true;
                }
            } else {
                *value = 3;
                valid = true;
            }
            break;
        }
        case _saved_film_string_new:
            *value = -(this->items[element_handle].get_corrupt() == 0);
            valid = true;
            break;
        }
    }
    return valid;
}

typedef char saved_film_subitem_async_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_saved_film_subitem_datasource, async) == 0x19ba8) )) ? 1 : -1];
typedef char saved_film_screen_queue_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_saved_film_subitem_datasource, async) == 0x19ba8) )) ? 1 : -1];

// @address 0x4eec00 ?update@c_gui_saved_film_subitem_datasource@@UAAXXZ
void c_gui_saved_film_subitem_datasource::update()
{
    if (this->category == 1) {
        gui_saved_film_screen_load_from_queue(this);
        return;
    }
    gui_saved_film_screen_load_from_catalogue(this);
}

// @address 0x4eec20 ?get_column_names@c_gui_saved_film_subitem_datasource@@UAAXQAJPAJ@Z
void c_gui_saved_film_subitem_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _string_id_title;
    column_names[(*column_count)++] = _string_id_description;
    column_names[(*column_count)++] = _string_id_locked;
    column_names[(*column_count)++] = _string_id_game_engine_index;
    column_names[(*column_count)++] = _string_id_map_id;
    column_names[(*column_count)++] = _string_id_item_saved;
    column_names[(*column_count)++] = _string_id_image_name;
    column_names[(*column_count)++] = _string_id_no_hdd;
    column_names[(*column_count)++] = _string_id_no_items;
    column_names[(*column_count)++] = _string_id_item_can_be_saved;
    column_names[(*column_count)++] = _string_id_item_built_in;
    column_names[(*column_count)++] = _string_id_item_options;
}

// 0x4ef070

bool c_gui_saved_film_subitem_datasource::get_text_value(long index, long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    if (index >= 0 && index < this->item_count) {
        s_gui_selected_items_block metadata;
        result = saved_game_files_get_metadata_string(this->items[index].get_metadata(&metadata), string_id, string);
    }
    return result;
}

// 0x4ef0f8
// @address 0x4ef0f8 ?get_string_id_value@c_gui_saved_film_subitem_datasource@@UAA_NJJPAJ@Z
bool c_gui_saved_film_subitem_datasource::get_string_id_value(long context, long string_id, long *result)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *result = _string_id_film_image;
        found = true;
    } else if (string_id == _string_id_no_hdd) {
        *result = _string_id_recent_films_no_hdd;
        found = true;
    } else if (string_id == _string_id_no_items) {
        *result = this->category == 1 ? _string_id_recent_films_no_items : _string_id_recent_films_no_items_in_category;
        found = true;
    } else if (string_id == _string_id_item_can_be_saved) {
        *result = _string_id_film_can_be_saved;
        found = true;
    } else if (string_id == _string_id_item_saved) {
        *result = _string_id_film_saved;
        found = true;
    } else if (string_id == _string_id_item_built_in) {
        *result = _string_id_film_built_in;
        found = true;
    } else if (string_id == _string_id_item_options) {
        *result = _string_id_film_options;
        found = true;
    }
    return found;
}

const c_gui_selected_item *c_gui_saved_film_subitem_datasource::get_gui_selected_item(long index) const
{
    const c_gui_saved_film_subitem_datasource *self = this;
    const c_gui_saved_film_selected_item *result = 0;
    if (index >= 0) {
        if (index < self->item_count) {
            result = &self->items[index];
        }
    }
    return (const c_gui_selected_item *)result;
}

// @address 0x4ef250 ?get_current_item_count_internal@c_gui_saved_film_subitem_datasource@@MAAJXZ
long c_gui_saved_film_subitem_datasource::get_current_item_count_internal()
{
    return item_count;
}

// 0x4ef3f0
c_gui_saved_film_selected_item &c_gui_saved_film_selected_item::operator=(const c_gui_saved_film_selected_item &other)
{
    m_type = other.m_type;
    m_special_type = other.m_special_type;
    m_location = other.m_location;
    m_flag14 = other.m_flag14;
    m_flag15_byte = other.m_flag15_byte;
    m_flag16_byte = other.m_flag16_byte;
    m_metadata = other.m_metadata;
    controller_index = other.controller_index;
    file_reference = other.file_reference;
    m_sort_key = other.m_sort_key;
    m_saved_film_category = other.m_saved_film_category;
    return *this;
}

// @address 0x4ef638 ?is_busy@c_gui_saved_film_subitem_datasource@@UAA_NXZ
bool c_gui_saved_film_subitem_datasource::is_busy()
{
    return busy == false;
}


extern "C" {
// @address 0x4ef650 gui_saved_film_screen_test_entry
bool gui_saved_film_screen_test_entry(const c_gui_saved_film_subitem_datasource *screen, const s_saved_game_item_metadata_packed *entry)
{
    switch (screen->category) {
    case 1:
        return true;
    case 2:
        return s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)entry->file_type) == _saved_game_file_category_clip;
    case 3:
        return s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)entry->file_type) == _saved_game_file_category_film
            && entry->campaign_id != -1;
    case 5:
        return s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)entry->file_type) == _saved_game_file_category_film
            && entry->campaign_id == -1 && entry->game_engine_type == 5;
    case 4:
    default:
        return s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)entry->file_type) == _saved_game_file_category_film
            && entry->campaign_id == -1 && entry->game_engine_type != 5;
    }
}
}
// 0x4ef490

// @address 0x4ef490 gui_saved_film_screen_load_from_catalogue
extern "C" void gui_saved_film_screen_load_from_catalogue(c_gui_saved_film_subitem_datasource *screen)
{
    s_saved_game_item_metadata_packed entry;
    s_file_reference reference;
    if (screen->busy)
        return;
    c_content_catalogue *catalogue = &content_catalogues().catalogues[screen->controller];
    if (!content_catalogue_has_storage_device(catalogue))
        return;
    if (!(_rotl(catalogue->m_flags.value, 31) & 1))
        return;
    int index = content_catalogue_get_first_item_index(catalogue);
    {
        while (index != -1) {
            if (screen->item_count >= 151)
                break;
            if (content_catalogue_item_metadata_matches(catalogue, index, &entry)) {
                if (entry.file_type == 11 || entry.file_type == 12) {
                    if (gui_saved_film_screen_test_entry(screen, &entry) &&
                        c_start_menu_game_editor::load_progress_ui_for_catalogue((c_start_menu_game_editor *)catalogue, (e_controller_index)index,
                            (s_file_reference *)&reference))
                    {
                        long slot = screen->item_count++;
                        screen->items[slot] = c_gui_saved_film_selected_item(&entry,
                            static_cast<e_saved_film_category>(screen->category), _gui_stored_item_location_saved_game_file,
                            screen->controller, &reference, 0, content_catalogue_is_item_corrupt(catalogue, index),
                            content_catalogue_is_item_created(catalogue, index));
                    }
                }
            }
            index = content_catalogue_get_next_item_index(catalogue, index);
        }
    }
    qsort(screen->items, screen->item_count, sizeof(c_gui_saved_film_selected_item), (int (*)(const void *, const void *))gui_saved_film_item_compare);
    screen->busy = 1;
}

// @address 0x4ef768 gui_saved_film_item_compare
extern "C" int gui_saved_film_item_compare(const c_gui_saved_film_selected_item *item_a, const c_gui_saved_film_selected_item *item_b)
{
    int result;
    s_gui_selected_items_block metadata_a;
    s_gui_selected_items_block metadata_b;
    item_a->get_metadata(&metadata_a);
    item_b->get_metadata(&metadata_b);
    unsigned __int64 sort_key_b = item_b->m_sort_key;
    unsigned __int64 sort_key_a = item_a->m_sort_key;
    if (sort_key_a < sort_key_b)
        result = 1;
    else if (sort_key_a > sort_key_b)
        result = -1;
    else if (metadata_a.date > metadata_b.date)
        result = -1;
    else if (metadata_a.date < metadata_b.date)
        result = 1;
    else
        result = wcscmp(item_a->get_title(), item_b->get_title());
    return result;
}

// @address 0x4ef260 gui_saved_film_screen_load_from_queue
extern "C" void gui_saved_film_screen_load_from_queue(c_gui_saved_film_subitem_datasource *screen)
{
    if (!screen->busy)
    {
        s_saved_film_scratch scratch;
        s_saved_game_queue *queue = &screen->async.queue;
        for (;;)
        {
            long write_count = queue->write_count.value;
            __lwsync();
            long read_count = queue->read_count.value;
            __lwsync();
            bool has_items = write_count > read_count;
            if (!has_items || !saved_game_queue_try_and_get_next(queue, &scratch.item))
                break;
            if (scratch.item.valid == 1 &&
                gui_saved_film_screen_test_entry(screen, &scratch.item.metadata))
            {
                if (screen->item_count < 151)
                {
                    long slot = screen->item_count++;
                    screen->items[slot] = c_gui_saved_film_selected_item(&scratch.item.metadata,
                        static_cast<e_saved_film_category>(screen->category), _gui_stored_item_location_autosave_queue,
                        screen->controller, &scratch.item.file, 0, false, false);
                }
            }
        }
        qsort(screen->items, screen->item_count, sizeof(c_gui_saved_film_selected_item), (int (*)(const void *, const void *))gui_saved_film_item_compare);
        bool busy = queue->state.peek() != 0 && queue->state.peek() != 3;
        if (!busy)
        {
            long write_count = queue->write_count.value;
            __lwsync();
            long read_count = queue->read_count.value;
            __lwsync();
bool has_more = write_count > read_count;
if (!has_more)
                screen->busy = 1;
        }
    }
}

typedef char saved_film_workspace_completion_offset[(offsetof(s_saved_film_workspace, saved_complete) == 4) ? 1 : -1];

typedef char saved_film_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_saved_film_selected_item, m_metadata) == 0x18))) ? 1 : -1];
typedef char saved_film_item_file_offset[(sizeof(void *) != 4 || (offsetof(c_gui_saved_film_selected_item, file_reference) == 0x194)) ? 1 : -1];
typedef char saved_film_category_mode_offset[(offsetof(s_saved_film_category, mode) == 0x264) ? 1 : -1];

typedef char saved_film_async_offset[(sizeof(void *) != 4 || (offsetof(c_gui_saved_film_subitem_datasource, async) == 0x19ba8)) ? 1 : -1];
typedef char saved_film_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_saved_film_selected_item) == 0x2b8))) ? 1 : -1];
typedef char saved_film_async_find_data_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_queue, find_file_data) == 0x3680))) ? 1 : -1];
typedef char saved_film_async_task_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_queue, thread_id) == 0x3a08))) ? 1 : -1];
typedef char saved_film_async_done_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_queue, done) == 0x3a0c))) ? 1 : -1];
typedef char saved_film_async_cancel_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_queue, cancel) == 0x3a10))) ? 1 : -1];
