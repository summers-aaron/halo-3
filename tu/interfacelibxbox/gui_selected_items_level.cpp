// interfacelibxbox/gui_selected_items_level.obj: from the records 0x4f0098 0x4f0100 0x4f0270 0x4f0308 0x4f0310 0x4f03a8 0x4f03f0 0x4f0498 0x4f0758 0x4f0768 0x4f07c0 0x4f0858 0x4f0880 0x4f08b8 0x4f0968
// @flags /TP /O2
#include "memory/static_arrays.h"
#include "interface/gui_selected_items_level_externs.h"
#include "game/game_types.h"
#include "interface/gui_data.h"
#include <ppcintrinsics.h>
#include "interface/user_interface.h"
#include "text/unicode.h"
#include "interface/gui_selected_items_level.h"
#include "memory/data_types.h"
// @address 0x491d68 ??0c_gui_simple_selected_item@@QAA@W4e_gui_selected_item_type@@PB_W1W4e_special_item_type@c_gui_selected_item@@W4e_gui_selected_item_location@@JJ@Z
// @address 0x4f0098 ??0c_gui_level_selected_item@@QAA@PB_W0JW4e_map_id@@EJ@Z
// @address 0x4f0270 ??4c_gui_level_selected_item@@QAAAAV0@ABV0@@Z
// @address 0x4f0310 ?get_text_value@c_gui_level_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4f0308 ?get_current_item_count_internal@c_gui_level_category_datasource@@MAAJXZ
// @address 0x4f03a8 ?get_integer_value@c_gui_level_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4f03f0 ??0c_gui_level_subitem_selectable_item_datasource@@QAA@XZ
// @address 0x4f0758 ?get_current_item_count_internal@c_gui_level_subitem_selectable_item_datasource@@MAAJXZ
// @address 0x4f0768 ?get_integer_value@c_gui_level_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4f07c0 ?get_text_value@c_gui_level_subitem_selectable_item_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4f0858 ?get_string_id_value@c_gui_level_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4f0880 ?get_gui_selected_item@c_gui_level_subitem_selectable_item_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x4f08b8 level_selected_item_sort_proc


#include "interface/gui_simple_selected_item.h"
#include "interface/gui_selected_items.h"

#include "main/levels_globals.h"
#include "main/levels.h"
#include "interface/controller_inline.h"
#include "interface/c_player_profile.h"
#include "game/game_engine_string_types.h"
#include "cseries/placement_new.h"
#include "game/player_configuration.h"
#include "interface/user_interface_shared_globals.h"




#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "tag_files/string_ids.h"



class c_gui_level_category_datasource;



static __forceinline long gui_selected_items_level_load_acquire(volatile long *address)
{
    long value = *address;
    __lwsync();
    return value;
}

static __forceinline bool levels_enumeration_busy(s_levels_globals *levels)
{
    long enumeration_task_id = levels->enumeration_task_id;
    if (enumeration_task_id != -1)
    {
        long value = gui_selected_items_level_load_acquire(&levels->enumeration_complete.value);
        if (value == 0)
            return true;
    }
    return false;
}

static __forceinline long gui_selected_items_level_campaign_count(s_levels_globals *levels)
{
    long count;
    unsigned long enabled = levels->enabled;
    if (enabled && !levels_enumeration_busy(levels))
        count = levels->campaign_levels->actual_count;
    else
        count = 0;
    return count;
}

// 0x4f0098
inline c_gui_level_selected_item::c_gui_level_selected_item(
    const wchar_t *name, const wchar_t *description, long campaign_id, e_map_id map_id, unsigned char locked, long special_type) :
    c_gui_simple_selected_item(
        _gui_selected_item_campaign, name, description, (c_gui_selected_item::e_special_item_type)2, _gui_selected_item_location_none, 0, 0)
{
    m_sort_key = campaign_id;
    m_map_id = map_id;
    m_flag3 = locked;
    m_extra = 0;
}

// 0x4f0270
inline c_gui_level_selected_item &c_gui_level_selected_item::operator=(const c_gui_level_selected_item &other)
{
    m_type = other.m_type;
    m_special_type = other.m_special_type;
    m_location = other.m_location;
    m_flag14 = other.m_flag14;
    m_flag15 = other.m_flag15;
    m_flag16 = other.m_flag16;
    m_metadata = other.m_metadata;
    m_sort_key = other.m_sort_key;
    m_map_id = other.m_map_id;
    m_flag3 = other.m_flag3;
    m_extra = other.m_extra;
    return *this;
}



// 0x4f0308
long c_gui_level_category_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

// 0x4f0310
bool c_gui_level_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(m_items[element_handle].get_title());
            found = true;
            break;
        case _string_id_description:
            value->set(m_items[element_handle].get_description());
            found = true;
            break;
        }
    }
    return found;
}

// 0x4f03a8
bool c_gui_level_category_datasource::get_integer_value(long item_index, long string_id, long *value)
{
    bool found = false;
    *value = -1;
    if (item_index >= 0 && item_index < m_item_count)
    {
        if (string_id == 254)
        {
            *value = m_items[item_index].m_sort_key;
            found = true;
        }
    }
    return found;
}

// 0x4f03f0
c_gui_level_subitem_selectable_item_datasource::c_gui_level_subitem_selectable_item_datasource()
{
    m_item_count = 0;
}

long c_gui_level_subitem_selectable_item_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

bool c_gui_level_subitem_selectable_item_datasource::get_integer_value(long item_index, long string_id, long *value)
{
    bool found = false;
    *value = -1;
    if (item_index >= 0 && item_index < m_item_count) {
        if (string_id == _string_id_image_index) {
            *value = m_entries[item_index].m_item.m_map_id;
            found = true;
        }
    }
    return found;
}

bool c_gui_level_subitem_selectable_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(m_entries[element_handle].m_name);
            found = true;
            break;
        case _string_id_description:
            value->set(m_entries[element_handle].m_item.get_description());
            found = true;
            break;
        }
    }
    return found;
}

bool c_gui_level_subitem_selectable_item_datasource::get_string_id_value(long item_index, long string_id, long *value)
{
    bool found = false;
    if (string_id == _string_id_image_name) {
        *value = _string_id_level_image;
        found = true;
    }
    return found;
}

const c_gui_selected_item *c_gui_level_subitem_selectable_item_datasource::get_gui_selected_item(long item_index) const
{
    const c_gui_selected_item *result = 0;
    if (item_index >= 0 && item_index < m_item_count)
        result = (const c_gui_selected_item *)&m_entries[item_index].m_item;
    return result;
}

int __cdecl level_selected_item_sort_proc(const void *element_a, const void *element_b)
{
    const c_gui_level_selected_item *a = (const c_gui_level_selected_item *)element_a;
    const c_gui_level_selected_item *b = (const c_gui_level_selected_item *)element_b;
    int result;
    int x = b->m_special_type; int y = a->m_special_type;
    if (y < x) result = -1;
    else if (y > x) result = 1;
    else {
        x = b->get_sort_key(); y = a->get_sort_key();
        if (y < x) result = -1;
        else if (y > x) result = 1;
        else {
            x = b->get_map_id(); y = a->get_map_id();
            if (y < x) result = -1;
            else if (y > x) result = 1;
            else {
                const wchar_t *name_b = b->get_title();
                const wchar_t *name_a = a->get_title();
                result = wcscmp(name_a, name_b);
            }
        }
    }
    return result;
}

// @address 0x4f0968 gui_selected_items_level_compare_entries
extern "C" int __cdecl gui_selected_items_level_compare_entries(const void *element_a, const void *element_b)
{
    const s_gui_level_list_entry *a = (const s_gui_level_list_entry *)element_a;
    const s_gui_level_list_entry *b = (const s_gui_level_list_entry *)element_b;
    int result;
    const c_gui_level_selected_item *item_a = &a->m_item;
    const c_gui_level_selected_item *item_b = &b->m_item;
    if (item_a->get_special_item_type() < item_b->get_special_item_type()) result = -1;
    else if (item_a->get_special_item_type() > item_b->get_special_item_type()) result = 1;
    else if (item_a->get_sort_key() < item_b->get_sort_key()) result = -1;
    else if (item_a->get_sort_key() > item_b->get_sort_key()) result = 1;
    else if (item_a->get_map_id() < item_b->get_map_id()) result = -1;
    else if (item_a->get_map_id() > item_b->get_map_id()) result = 1;
    else result = wcscmp(item_a->get_title(), item_b->get_title());
    return result;
}

// @address 0x4f0100 ?initialize@c_gui_level_category_datasource@@UAA_NJ@Z
bool c_gui_level_category_datasource::initialize(long name)
{
    const wchar_t *empty = L" ";
    m_name = name;
    m_item_count = 0;
    long campaign_id = 1;
    for (; campaign_id < gui_selected_items_level_campaign_count(&g_level_globals) + 1; campaign_id++)
    {
        long item_index = m_item_count;
        m_item_count = item_index + 1;
        m_items[item_index] = c_gui_level_selected_item(empty, empty, campaign_id, _map_id_none, 0, 0);
    }
    qsort(m_items, m_item_count, sizeof(m_items[0]), level_selected_item_sort_proc);
    return true;
}


// @address 0x1e5208 levels_get_campaign_map_ids
// @address 0x1e7238 levels_try_and_get_datum
// @address 0x1bfa40 text_group_get_string
// @address 0x8d430 qsort
static __forceinline bool gui_selected_items_level_is_campaign_level(unsigned short flags)
{
    return ((flags >> 2) & 1) != 0;
}
static __forceinline void gui_selected_items_level_wstring_copy(wchar_t *destination, const wchar_t *source, long count);
static __forceinline void gui_selected_items_level_wstring_copy(wchar_t * destination, const wchar_t *source, long count)
{
    long index = 0;
    do {
        if ((destination[index] = *source) == 0)
            break;
        source++;
        index++;
    } while (index <= count - 1);
    destination[count - 1] = 0;
}
static __forceinline bool gui_selected_items_level_global_string_get(long string_id, c_static_wchar_string_1024 *string)
{
    bool result = false;
    string->string[0] = 0;
    const s_user_interface_shared_globals *globals = user_interface_shared_tag_globals_try_and_get();
    if (globals && globals->global_strings.index != -1)
        result = text_group_get_string(globals->global_strings.index, string_id, string);
    return result;
}
// @address 0x4f0498 ?initialize_levels@c_gui_level_subitem_selectable_item_datasource@@UAA_NJJJ@Z
bool c_gui_level_subitem_selectable_item_datasource::initialize_levels(long controller, long name, long campaign)
{
    m_name = name;
    c_player_profile_interface *player = controller_get((e_controller_index)controller)->get_player_profile_interface();
    if (player) {
        bool unlocked_found = false;
        long maps[64];
        long count = 64;
        levels_get_campaign_map_ids(campaign, maps, &count);
        m_item_count = 0;
        for (long index = count - 1; index >= 0; --index) {
            s_datum level;
            if (levels_try_and_get_datum((s_data_array *)g_level_globals.map_levels, maps[index], &level) &&
                gui_selected_items_level_is_campaign_level(level.flags))
            {
                short level_index = (short)index;
                bool unlocked = false;
                if (level_index >= 0 && level_index < 32)
                    unlocked = (player->campaign.campaign_map_unlocked[level_index >> 5] & (1UL << (level_index & 31))) != 0;
                if (unlocked || index == 0) {
                    s_gui_level_list_entry &entry = m_entries[m_item_count];
                    gui_selected_items_level_wstring_copy(entry.m_name, level.display_name, 32);
                    char item_storage[sizeof(c_gui_level_selected_item)];
                    c_gui_level_selected_item *item = (c_gui_level_selected_item *)item_storage;
                    s_gui_level_list_entry &selected_entry = m_entries[m_item_count];
                    selected_entry.m_item = *new (item) c_gui_level_selected_item(
                    level.display_name, level.description, campaign, (e_map_id)level.unique_id, 0, 2);
                    unlocked_found = true;
                    ++m_item_count;
                } else if (unlocked_found) {
                    c_static_wchar_string_1024 description, title;
                    description.string[0] = 0;
                    title.string[0] = 0;
                    if (gui_selected_items_level_global_string_get(_string_id_level_locked, &title)) {
                        if (gui_selected_items_level_global_string_get(_string_id_level_locked_description, &description)) {
                            s_gui_level_list_entry &entry = m_entries[m_item_count];
                            ustrnzcpy(entry.m_name, title.string, 48);
                            char item_storage[sizeof(c_gui_level_selected_item)];
                            c_gui_level_selected_item *item = (c_gui_level_selected_item *)item_storage;
                            s_gui_level_list_entry &selected_entry = m_entries[m_item_count];
                            c_gui_level_selected_item *created = new (item)
                                c_gui_level_selected_item(title.string, description.string, campaign, (e_map_id)level.unique_id, 1, 2);
                            selected_entry.m_item = *created;
                            ++m_item_count;
                        }
                    }
                }
            }
        }
        qsort(m_entries, m_item_count, sizeof(m_entries[0]), gui_selected_items_level_compare_entries);
    }
    return true;
}

// @address 0x4f4038 ?get_column_names@c_gui_level_category_datasource@@UAAXQAJPAJ@Z
void c_gui_level_category_datasource::get_column_names(long *const column_names, long *count)
{
    *count = 0;
    column_names[0] = 254;
    column_names[++*count] = _string_id_title;
    column_names[++*count] = _string_id_description;
    ++*count;
}
