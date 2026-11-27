// interfacelibxbox/gui_selected_items_difficulty.obj: from the records 0x2219b0 0x221ab8 0x221ac0 0x221b20 0x221b28 0x221b30 0x221b38 0x4e9020 0x4eb608 0x4eb6c0 0x4eb750 0x4eb758 0x4eb848 0x4eb8b8 0x4eb910 0x4eb9a8 0x4eb9d0
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "cseries/cseries.h"
#include "game/game_types.h"
#include "interface/gui_data.h"
#include "interface/gui_roster_data.h"
#include "interface/user_interface_networking_externs.h"
#include "memory/static_arrays.h"
#include "interface/gui_selected_items.h"
#include "interface/gui_simple_selected_item.h"
#include "interface/gui_selected_items_difficulty.h"
// @address 0x4eb848 ?get_column_names@c_gui_difficulty_category_datasource@@UAAXQAJPAJ@Z
// @address 0x4eb8b8 ?get_integer_value@c_gui_difficulty_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4eb9a8 ?get_string_id_value@c_gui_difficulty_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4eb608 ?find_title_and_description@c_gui_difficulty_selected_item@@SA_NW4e_campaign_difficulty_level@@PAV?$c_static_wchar_string@$0EAA@@@1@Z

enum
{
    _string_id_category = 254,
    _string_id_image_index = 0x10327,
    _string_id_title = 0x29d,
    _string_id_description = 0x29e,
    _string_id_image_name = 0x10326,
    _string_id_difficulty_image = 0x1032d,
};

typedef char difficulty_item_size[(sizeof(void *) != 4 || (sizeof(c_gui_difficulty_selected_item) == 0x198)) ? 1 : -1];
typedef char difficulty_item_value_offset[(sizeof(void *) != 4 || (offsetof(c_gui_difficulty_selected_item, m_difficulty) == 0x190)) ? 1 : -1];
typedef char difficulty_items_offset[(sizeof(void *) != 4 || (offsetof(c_gui_difficulty_category_datasource, m_selected_items) == 0x110)) ? 1 : -1];
typedef char difficulty_item_count_offset[(sizeof(void *) != 4 || (offsetof(c_gui_difficulty_category_datasource, m_selected_item_count) == 0x770)) ? 1 : -1];

struct s_gui_selected_item_file_info
{
    long valid;
    long size;
};

// @address 0x2219b0 ?get_location@c_gui_simple_selected_item@@UBA?AW4e_gui_selected_item_location@@XZ
e_gui_selected_item_location c_gui_simple_selected_item::get_location() const
{
    return m_location;
}

// @address 0x221ab8 ?get_title@c_gui_simple_selected_item@@UBAPB_WXZ
const wchar_t *c_gui_simple_selected_item::get_title() const
{
    return m_metadata.name;
}

// @address 0x221ac0 ?get_description@c_gui_simple_selected_item@@UBAPB_WXZ
const wchar_t *c_gui_simple_selected_item::get_description() const
{
    return m_metadata.description;
}

// @address 0x221998 ?is_valid@c_gui_simple_selected_item@@UBA_NXZ
bool c_gui_simple_selected_item::is_valid() const
{
    return m_type != _gui_selected_item_none;
}

// @address 0x221b20 ?get_flag14@c_gui_simple_selected_item@@UBA_NXZ
bool c_gui_simple_selected_item::get_flag14() const
{
    return m_flag14;
}

// @address 0x221b28 ?get_enabled@c_gui_simple_selected_item@@UBA_NXZ
bool c_gui_simple_selected_item::get_enabled() const
{
    return m_flag15;
}

// @address 0x221b30 ?get_corrupt@c_gui_simple_selected_item@@UBA_NXZ
bool c_gui_simple_selected_item::get_corrupt() const
{
    return m_flag16;
}

// @address 0x221b38 gui_selected_item_get_file_info
extern "C" void gui_selected_item_get_file_info(void *item, long type, s_gui_selected_item_file_info *info)
{
    (void)item;
    if (type == 1)
    {
        info->valid = 1;
        info->size = 0x96000;
        return;
    }
    info->size = 0;
    info->valid = 0;
}

// @address 0x4e9020 ?get_current_item_count_internal@c_gui_roster_data@@MAAJXZ
long c_gui_roster_data::get_current_item_count_internal()
{
    return m_player_count;
}

bool c_gui_difficulty_selected_item::find_title_and_description(
    e_campaign_difficulty_level difficulty, c_static_wchar_string<1024> *title, c_static_wchar_string<1024> *description)
{
    bool found = false;
    long title_id = NONE;
    long description_id = NONE;
    switch (difficulty) {
    case _campaign_difficulty_easy: title_id = 0x119; description_id = 0x102fa; break;
    case _campaign_difficulty_normal: title_id = 0xad; description_id = 0x102fb; break;
    case _campaign_difficulty_heroic: title_id = 0x11a; description_id = 0x102fc; break;
    case _campaign_difficulty_legendary: title_id = 0x11b; description_id = 0x102fd; break;
    }
    if (title_id != NONE && description_id != NONE) {
        user_interface_global_string_get(title_id, (unsigned short *)title);
        user_interface_global_string_get(description_id, (unsigned short *)description);
        found = true;
    }
    return found;
}

// @address 0x56568 ??_7c_gui_difficulty_category_datasource@@6B@
// @address 0x56c64 ??_7c_gui_difficulty_selected_item@@6B@
// @address 0x4c0ee0 ??_Gc_gui_difficulty_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_difficulty_category_datasource@@UAAPAXI@Z

// @address 0x4eb6c0 ??0c_gui_difficulty_category_datasource@@QAA@XZ
c_gui_difficulty_category_datasource::c_gui_difficulty_category_datasource()
{
    m_selected_item_count = 0;
}

long c_gui_difficulty_category_datasource::get_current_item_count_internal()
{
    return m_selected_item_count;
}

// @address 0x4eb758 ?initialize@c_gui_difficulty_category_datasource@@UAA_NJ@Z
bool c_gui_difficulty_category_datasource::initialize(long name)
{
    c_static_wchar_string<1024> difficulty_title;
    c_static_wchar_string<1024> difficulty_description;
    long difficulty = 0;
    m_name = name;
    m_selected_item_count = 0;
    do
    {
        if ((unsigned char)c_gui_difficulty_selected_item::find_title_and_description((e_campaign_difficulty_level)difficulty,
                &difficulty_title, &difficulty_description))
        {
            long index = m_selected_item_count;
            m_selected_item_count = index + 1;
            m_selected_items[index] = c_gui_difficulty_selected_item(difficulty_title.get_string(),
                difficulty_description.get_string(), static_cast<e_campaign_difficulty_level>(difficulty),
                c_gui_selected_item::_special_item_type_none);
        }
        difficulty = difficulty + 1;
    } while (difficulty < 4);
    return 1;
}

void c_gui_difficulty_category_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_category;
    ++*column_count;
    column_names[*column_count] = _string_id_title;
    ++*column_count;
    column_names[*column_count] = _string_id_description;
    ++*column_count;
    column_names[*column_count] = _string_id_image_index;
    ++*column_count;
}

bool c_gui_difficulty_category_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = NONE;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        if (value_name != _string_id_category)
        {
            if (value_name == _string_id_image_index)
            {
                *value = m_selected_items[element_handle].get_difficulty();
                result = true;
            }
        }
        else
        {
            *value = m_selected_items[element_handle].get_difficulty();
            result = true;
        }
    }
    return result;
}

bool c_gui_difficulty_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(m_selected_items[element_handle].get_title());
            found = true;
            break;
        case _string_id_description:
            value->set(m_selected_items[element_handle].get_description());
            found = true;
            break;
        }
    }
    return found;
}

bool c_gui_difficulty_category_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _string_id_difficulty_image;
        found = true;
    }
    return found;
}

const c_gui_selected_item *c_gui_difficulty_category_datasource::get_gui_selected_item(long element_handle) const
{
    return &m_selected_items[element_handle];
}
