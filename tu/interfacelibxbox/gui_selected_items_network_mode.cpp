// interfacelibxbox/gui_selected_items_network_mode.obj: from the records 0x4f3ae0 0x4f3b60 0x4f3c00 0x4f3d58 0x4f3db0 0x4f3de0 0x4f3e08 0x4f3ef0 0x4f4088 0x4f40d0 0x4f4168 0x4f4190
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "memory/static_arrays.h"
// @address 0x4f3b60 ?initialize@c_gui_network_mode_category_datasource@@UAA_NJ@Z
// @address 0x4f3c00 ?add_category@c_gui_network_mode_category_datasource@@AAAXJJW4e_network_mode_category@@W4e_network_mode_choice@@_N@Z
// @address 0x4f3e08 ?initialize@c_gui_network_mode_subitem_selectable_item_datasource@@UAA_NJW4e_network_mode_category@@@Z
// @address 0x4f3ef0 ?add_subitem@c_gui_network_mode_subitem_selectable_item_datasource@@AAAXJJW4e_network_mode_category@@W4e_network_mode_choice@@@Z
// @address 0x491d68 ??0c_gui_simple_selected_item@@QAA@W4e_gui_selected_item_type@@PB_W1W4e_special_item_type@c_gui_selected_item@@W4e_gui_selected_item_location@@JJ@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x4f3db0 ?get_gui_selected_item@c_gui_network_mode_subitem_selectable_item_datasource@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x4f3d58 ?get_integer_value@c_gui_network_mode_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4f3de0 ?get_string_id_value@c_gui_network_mode_category_datasource@@UAA_NJJPAJ@Z
// @address 0x4f4088 ?get_integer_value@c_gui_network_mode_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z
// @address 0x4f40d0 ?get_text_value@c_gui_network_mode_subitem_selectable_item_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4f4168 ?get_string_id_value@c_gui_network_mode_subitem_selectable_item_datasource@@UAA_NJJPAJ@Z

#include "interface/user_interface.h"
#include "text/unicode.h"
enum
{
    _string_id_category = 254,
    _string_id_title = 0x29d,
    _string_id_description = 0x29e,
    _string_id_image_name = 0x10326,
    _string_id_image_index = 0x10327,
    _string_id_advertisement_mode_image = 0x10331,
    _string_id_network_mode = 0x1038c,
    _string_id_network_mode_xbox_live_private = 0x1038e,
    _string_id_network_mode_xbox_live_private_description = 0x1038f,
    _string_id_network_mode_system_link = 0x10390,
    _string_id_network_mode_system_link_description = 0x10391,
    _string_id_network_mode_offline = 0x10392,
    _string_id_network_mode_offline_description = 0x10393,
    _string_id_network_mode_xbox_live_advertise_anyone = 0x10394,
    _string_id_network_mode_xbox_live_advertise_anyone_description = 0x10395,
    _string_id_network_mode_xbox_live_advertise_friends = 0x10396,
    _string_id_network_mode_xbox_live_advertise_friends_description = 0x10397,
    _string_id_network_mode_xbox_live_advertise_invite_only = 0x10398,
    _string_id_network_mode_xbox_live_advertise_invite_only_description = 0x10399,
    _string_id_network_mode_system_link_advertise = 0x1039c,
    _string_id_network_mode_system_link_advertise_description = 0x1039d,
    _string_id_network_mode_system_link_browse = 0x1039e,
    _string_id_network_mode_system_link_browse_description = 0x1039f,
};
#include "interface/gui_selected_items_network_mode.h"
#include <new>
typedef char network_mode_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_simple_selected_item, m_metadata) == 0x18))) ? 1 : -1];
typedef char network_mode_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_network_mode_selected_item) == 0x198))) ? 1 : -1];
typedef char network_mode_category_names_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_network_mode_category_datasource, m_mode_names) == 0x10c) )) ? 1 : -1];
typedef char network_mode_category_items_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_network_mode_category_datasource, m_selected_items) == 0x230) ? 1 : -1];
typedef char network_mode_subitem_items_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_network_mode_subitem_selectable_item_datasource, m_selected_items) == 0x230) ? 1 : -1];
typedef char network_mode_category_count_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_network_mode_category_datasource, m_selected_item_count) == 0x6f8) ? 1 : -1];
typedef char network_mode_subitem_size[(sizeof(void *) != 4 || sizeof(c_gui_network_mode_subitem_selectable_item_datasource) == 0x700) ? 1 : -1];


// @address 0x56d78 ??_7c_gui_network_mode_category_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_network_mode_category_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_network_mode_category_datasource@@UAAPAXI@Z
// @address 0x4f3ae0 ??0?$c_static_array@Vc_gui_network_mode_selected_item@@$02@@QAA@XZ
c_gui_network_mode_category_datasource::c_gui_network_mode_category_datasource(c_gui_screen_widget *screen) :
    c_gui_ordered_data(), m_selected_item_count(0), m_screen(screen)
{
}

// @address 0x56de8 ??_7c_gui_network_mode_subitem_selectable_item_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_gui_network_mode_subitem_selectable_item_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_gui_network_mode_subitem_selectable_item_datasource@@UAAPAXI@Z
c_gui_network_mode_subitem_selectable_item_datasource::c_gui_network_mode_subitem_selectable_item_datasource(c_gui_screen_widget *screen) :
    c_gui_ordered_data(), m_selected_item_count(0), m_screen(screen)
{
}


class c_gui_selected_item;

bool c_gui_network_mode_category_datasource::initialize(long name)
{
    m_name = name;
    add_category(_string_id_network_mode_xbox_live_private, _string_id_network_mode_xbox_live_private_description,
        _network_mode_category_xbox_live_private, k_network_mode_choice_count, true);
    add_category(_string_id_network_mode_system_link, _string_id_network_mode_system_link_description,
        _network_mode_category_system_link, k_network_mode_choice_count, true);
    add_category(_string_id_network_mode_offline, _string_id_network_mode_offline_description,
        _network_mode_category_offline, k_network_mode_choice_count, false);
    return true;
}

bool c_gui_network_mode_category_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    *value = NONE;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        if (value_name != _string_id_category)
        {
            if (value_name == _string_id_image_index)
            {
                *value = m_selected_items[element_handle].get_network_mode();
                result = true;
            }
        }
        else
        {
            *value = m_selected_items[element_handle].get_network_mode();
            result = true;
        }
    }
    return result;
}

void c_gui_network_mode_category_datasource::add_category(
    long name_handle, long description_handle, e_network_mode_category network_mode, e_network_mode_choice choice, bool special)
{
    c_static_wchar_string<1024> name;
    c_static_wchar_string<1024> description;
    if (m_screen->get_string_by_string_id(name_handle, &name) && m_screen->get_string_by_string_id(description_handle, &description))
    {
        ustrnzcpy(m_mode_names[m_selected_item_count], name.m_string, 48);
        c_gui_network_mode_selected_item item(L" ", description.m_string, network_mode, k_network_mode_choice_count,
            (c_gui_selected_item::e_special_item_type)(special ? 2 : -1));
        m_selected_items[m_selected_item_count] = item;
        m_selected_item_count++;
    }
}

bool c_gui_network_mode_subitem_selectable_item_datasource::initialize(long name, e_network_mode_category network_mode)
{
    m_name = name;
    unsigned int category = (unsigned int)network_mode;
    if (category >= 1)
    {
        if (category == 1)
        {
            add_subitem(_string_id_network_mode_system_link_advertise, _string_id_network_mode_system_link_advertise_description,
                _network_mode_category_system_link, _network_mode_choice_system_link_advertise);
            add_subitem(_string_id_network_mode_system_link_browse, _string_id_network_mode_system_link_browse_description,
                _network_mode_category_system_link, _network_mode_choice_system_link_browse);
        }
    }
    else
    {
        add_subitem(_string_id_network_mode_xbox_live_advertise_anyone, _string_id_network_mode_xbox_live_advertise_anyone_description,
            _network_mode_category_xbox_live_private, _network_mode_choice_xbox_live_advertise_anyone);
        if (!get_alpha_is_internal_beta())
            add_subitem(_string_id_network_mode_xbox_live_advertise_friends, _string_id_network_mode_xbox_live_advertise_friends_description,
                _network_mode_category_xbox_live_private, _network_mode_choice_xbox_live_advertise_friends);
        add_subitem(_string_id_network_mode_xbox_live_advertise_invite_only, _string_id_network_mode_xbox_live_advertise_invite_only_description,
            _network_mode_category_xbox_live_private, _network_mode_choice_xbox_live_advertise_invite_only);
    }
    return true;
}

void c_gui_network_mode_subitem_selectable_item_datasource::add_subitem(
    long name_handle, long description_handle, e_network_mode_category network_mode, e_network_mode_choice choice)
{
    c_static_wchar_string<1024> name;
    c_static_wchar_string<1024> description;
    if ((unsigned long)m_selected_item_count < 3 &&
        m_screen->get_string_by_string_id(name_handle, &name) &&
        m_screen->get_string_by_string_id(description_handle, &description))
    {
        ustrnzcpy(m_mode_names[m_selected_item_count], name.m_string, 48);
        long index = m_selected_item_count++;
        c_gui_network_mode_selected_item item(name.m_string, description.m_string, network_mode, choice, c_gui_selected_item::_special_item_type_none);
        m_selected_items[index] = item;
    }
}

const c_gui_selected_item *c_gui_network_mode_subitem_selectable_item_datasource::get_gui_selected_item(long element_handle) const
{
    const c_gui_network_mode_selected_item *result =
        element_handle >= 0 && element_handle < m_selected_item_count
            ? &m_selected_items[element_handle] : 0;
    return (const c_gui_selected_item *)result;
}

bool c_gui_network_mode_category_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _string_id_network_mode;
        found = true;
    }
    return found;
}

bool c_gui_network_mode_subitem_selectable_item_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    *value = NONE;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        if (value_name == _string_id_category)
        {
            *value = m_selected_items[element_handle].get_network_mode();
            found = true;
        }
    }
    return found;
}

bool c_gui_network_mode_subitem_selectable_item_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(m_mode_names[element_handle]);
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

bool c_gui_network_mode_subitem_selectable_item_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool found = false;
    if (value_name == _string_id_image_name) {
        *value = _string_id_advertisement_mode_image;
        found = true;
    }
    return found;
}

// @address 0x4f4190 ?get_current_item_count_internal@c_gui_network_mode_subitem_selectable_item_datasource@@MAAJXZ
long c_gui_network_mode_subitem_selectable_item_datasource::get_current_item_count_internal()
{
    return m_selected_item_count;
}

// @address 0x4f4038 ?get_column_names@c_gui_network_mode_category_datasource@@UAAXQAJPAJ@Z
void c_gui_network_mode_category_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[0] = _string_id_category;
    column_names[++*column_count] = _string_id_title;
    column_names[++*column_count] = _string_id_description;
    ++*column_count;
}

// @address 0x4f40d0 ?get_text_value@c_gui_network_mode_category_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_network_mode_category_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool found = false;
    if (element_handle >= 0 && element_handle < m_selected_item_count)
    {
        switch (value_name)
        {
        case _string_id_title:
            value->set(m_mode_names[element_handle]);
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

// @address 0x4f3db0 ?get_gui_selected_item@c_gui_network_mode_category_datasource@@UBAPBVc_gui_selected_item@@J@Z
const c_gui_selected_item *c_gui_network_mode_category_datasource::get_gui_selected_item(long element_handle) const
{
    const c_gui_network_mode_selected_item *result =
        element_handle >= 0 && element_handle < m_selected_item_count
            ? &m_selected_items[element_handle] : 0;
    return (const c_gui_selected_item *)result;
}

// @address 0x4f4190 ?get_current_item_count_internal@c_gui_network_mode_category_datasource@@MAAJXZ
long c_gui_network_mode_category_datasource::get_current_item_count_internal()
{
    return m_selected_item_count;
}

// @address 0x4f4038 ?get_column_names@c_gui_network_mode_subitem_selectable_item_datasource@@UAAXQAJPAJ@Z
void c_gui_network_mode_subitem_selectable_item_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[0] = _string_id_category;
    column_names[++*column_count] = _string_id_title;
    column_names[++*column_count] = _string_id_description;
    ++*column_count;
}
