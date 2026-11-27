// interfacelibxbox/gui_selected_items_hopper.obj: from the records 0x4f3520 0x4f35a0 0x4f3670 0x4f3678 0x4f36e8 0x4f3758 0x4f3780 0x4f3928 0x4f3968 0x4f39b8 0x4f3a88 0x4f3ab0
// @flags /TP /O2 /Oc
#include "memory/static_arrays.h"
#include <new>
// @address 0x4f3520 ??0c_gui_hopper_selected_item@@QAA@XZ
// @address 0x42d298 ?get_file_path@c_gui_simple_selected_item@@UBA_NPADJ@Z
// @address 0x4f35a0 ?initialize@c_gui_hopper_category_datasource@@UAA_NJ@Z
// @address 0x4f3670 ?get_current_item_count_internal@c_gui_hopper_category_datasource@@MAAJXZ
// @address 0x4f3678 ?get_text_value@c_gui_hopper_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4f36e8 ?get_integer_value@c_gui_hopper_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4f3758 ?get_string_id_value@c_gui_hopper_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4f3780 ?initialize@c_gui_hopper_subitem_selectable_item_datasource@@UAA_NJW4e_hopper_category_id@@@Z
// @address 0x4f3928 ?get_current_item_count_internal@c_gui_hopper_subitem_selectable_item_datasource@@MAAJXZ
// @address 0x4f3968 ?get_integer_value@c_gui_hopper_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4f3a88 ?get_string_id_value@c_gui_hopper_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4f3ab0 ?get_gui_selected_item@c_gui_hopper_subitem_selectable_item_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
#include <string.h>
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "interface/user_interface_matchmaking.h"
#include "interface/gui_selected_items_hopper.h"
#include "interface/gui_data.h"
#include <stddef.h>
#include "interface/utf8_types.h"
#include "interface/hopper_category_id_types.h"
#include "tag_files/string_ids.h"

__forceinline bool gui_selected_items_hopper_get_hopper_description(unsigned short hopper_identifier, unsigned short *description)
{
    bool result = false;
    const utf8 *source = multiplayer_game_hopper_get_description(hopper_identifier);
    if (source)
    {
        ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)source, (wchar_t *)(description), 256, (long *)(0));
        result = true;
    }
    return result;
}

c_gui_hopper_selected_item::c_gui_hopper_selected_item() :
    c_gui_simple_selected_item(_gui_selected_item_none, L"", L"", c_gui_selected_item::_special_item_type_none,
        _gui_selected_item_location_none, 0, 0)
{
    memset(&m_information, 0, sizeof(m_information));
}

bool c_gui_hopper_category_datasource::initialize(long name)
{
    long category_ids[14];
    m_name = name;
    int category_count = user_interface_matchmaking_get_category_ids(category_ids);
    m_item_count = 0;
    if (category_count > 0)
    {
        long *category_id_cursor = category_ids;
        unsigned int categories_remaining = category_count;
        do
        {
            int category_id = *category_id_cursor;
            if (user_interface_matchmaking_get_category_name(category_id, m_categories[m_item_count].name) &&
                user_interface_matchmaking_get_category_description(category_id, m_categories[m_item_count].description))
            {
                m_categories[m_item_count].identifier = category_id;
                int category_value = user_interface_matchmaking_get_category_value(category_id);
                m_categories[m_item_count].value = category_value;
                m_item_count++;
            }
            categories_remaining--;
            category_id_cursor++;
        } while (categories_remaining != 0);
    }
    return true;
}

long c_gui_hopper_category_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

bool c_gui_hopper_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool succeeded = false;
    if (element_handle >= 0 && element_handle < m_item_count &&
        (value_name == _hopper_string_title || value_name == _hopper_string_description))
    {
        const unsigned short *field_text = value_name != _hopper_string_title ?
            m_categories[element_handle].description : m_categories[element_handle].name;
        value->set((const wchar_t *)field_text);
        succeeded = true;
    }
    return succeeded;
}

bool c_gui_hopper_category_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        if (value_name != _hopper_string_category)
        {
            if (value_name == _hopper_string_image_index)
            {
                *value = m_categories[element_handle].value;
                result = true;
            }
        }
        else
        {
            *value = m_categories[element_handle].identifier;
            result = true;
        }
    }
    return result;
}

bool c_gui_hopper_category_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _hopper_string_category_image;
        found = true;
    }
    return found;
}

bool c_gui_hopper_subitem_selectable_item_datasource::initialize(long name, e_hopper_category_id hopper_category_id)
{
    m_name = name;
    m_item_count = 0;
    if ((int)hopper_category_id != -1) {
        unsigned short hopper_ids[32];
        long hopper_count = ::user_interface_matchmaking_get_hopper_ids((e_hopper_category_id)(long)hopper_category_id, (unsigned short *)(hopper_ids));
        m_item_count = 0;
        for (long index = 0; index < hopper_count; index++) {
            s_hopper_information information;
            if (!::user_interface_matchmaking_get_hopper_information(hopper_ids[index], &information)) continue;
            if (!information.visible) continue;
            unsigned short description[256];
            const wchar_t *text = L" ";
            if (gui_selected_items_hopper_get_hopper_description(information.id, description))
                text = (const wchar_t *)description;
            c_gui_hopper_selected_item item((const wchar_t *)information.name, text, &information);
            m_selected_items[m_item_count] = item;
            m_selected_items[m_item_count].set_enabled(information.visible && information.enabled);
            m_item_count = m_item_count + 1;
        }
    }
    return true;
}

long c_gui_hopper_subitem_selectable_item_datasource::get_current_item_count_internal()
{
    return m_item_count;
}

bool c_gui_hopper_subitem_selectable_item_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = -1;
    if (element_handle >= 0 && element_handle < m_item_count && value_name == _hopper_string_image_index)
    {
        *value = m_selected_items[element_handle].m_information.number;
        result = true;
    }
    return result;
}

bool c_gui_hopper_subitem_selectable_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_item_count)
    {
        switch (value_name)
        {
        case _hopper_string_title:
            value->set(m_selected_items[element_handle].get_title());
            found = true;
            break;
        case _hopper_string_description:
            unsigned short description[256];
            if (gui_selected_items_hopper_get_hopper_description(m_selected_items[element_handle].get_hopper_information()->id, description))
            {
                value->set((const wchar_t *)description);
            }
            else
            {
                value->set(L" ");
            }
            found = true;
            break;
        }
    }
    return found;
}

bool c_gui_hopper_subitem_selectable_item_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _hopper_string_hopper_image;
        found = true;
    }
    return found;
}

const c_gui_selected_item * c_gui_hopper_subitem_selectable_item_datasource::get_gui_selected_item(long element_handle) const
{
    const c_gui_selected_item *result = 0;
    if (element_handle >= 0 && element_handle < m_item_count)
        result = &m_selected_items[element_handle];
    return result;
}

// @address 0x4f4038 ?get_column_names@c_gui_hopper_category_datasource@@UAAXQAJPAJ@Z
void c_gui_hopper_category_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[0] = _hopper_string_category;
    names[++*count] = _hopper_string_title;
    names[++*count] = _hopper_string_description;
    ++*count;
}
