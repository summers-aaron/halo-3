#ifndef INTERFACE_GUI_SELECTED_ITEMS_HOPPER_H
#define INTERFACE_GUI_SELECTED_ITEMS_HOPPER_H

#include "interface/gui_data.h"
#include "interface/gui_selected_item.h"
#include "interface/gui_simple_selected_item.h"
#include "interface/hopper_information_types.h"
#include "interface/hopper_category_id_types.h"
#include <stddef.h>

// String ids of the values the hopper datasources answer; each is named
// after its string in the cache string table.
enum e_hopper_string_id
{
    _hopper_string_category = 254,
    _hopper_string_title = 0x29d,
    _hopper_string_description = 0x29e,
    _hopper_string_image_index = 0x10327,
    _hopper_string_category_image = 0x1032e,
    _hopper_string_hopper_image = 0x1032f,
};

struct s_gui_hopper_category_entry
{
    long identifier;
    long value;
    unsigned short name[32];
    unsigned short description[256];
};

class c_gui_hopper_category_datasource : public c_gui_ordered_data
{
public:
    virtual void get_column_names(long *const names, long *count);
    c_gui_hopper_category_datasource();
    virtual bool initialize(long name);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal();
public:
    s_gui_hopper_category_entry m_categories[12];
    long m_item_count;
};
typedef char hopper_category_entry_size[(sizeof(s_gui_hopper_category_entry) == 0x248) ? 1 : -1];
typedef char hopper_category_entries_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_hopper_category_datasource, m_categories) == 0x10c) )) ? 1 : -1];
typedef char hopper_category_count_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_hopper_category_datasource, m_item_count) == 0x1c6c) )) ? 1 : -1];

class c_gui_hopper_selected_item : public c_gui_simple_selected_item
{
public:
    c_gui_hopper_selected_item();
    __forceinline c_gui_hopper_selected_item(const wchar_t *name, const wchar_t *description, const s_hopper_information *information) :
        c_gui_simple_selected_item(_gui_selected_item_hopper, name, description, c_gui_selected_item::_special_item_type_none,
            _gui_selected_item_location_none, 0, 0)
    {
        m_information = *information;
    }

    const s_hopper_information *get_hopper_information() const { return &m_information; }
    void set_enabled(bool value) { m_flag14 = value; }
    c_gui_hopper_selected_item &operator=(const c_gui_hopper_selected_item &other)
    {
        c_gui_simple_selected_item::operator=(other);
        m_information = other.m_information;
        return *this;
    }

    s_hopper_information m_information;
};
class c_gui_hopper_subitem_selectable_item_datasource : public c_gui_ordered_data
{
public:
    c_gui_hopper_subitem_selectable_item_datasource();
    virtual void get_column_names(long *const names, long *count);
    virtual const c_gui_selected_item * get_gui_selected_item(long element_handle) const;
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool initialize(long name, e_hopper_category_id hopper_category_id);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal();
    c_gui_hopper_selected_item m_selected_items[32];
    long m_item_count;
};

typedef char hopper_subitem_size[(sizeof(void *) != 4 || ((sizeof(c_gui_hopper_subitem_selectable_item_datasource) == 0x3e18))) ? 1 : -1];
typedef char hopper_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_hopper_selected_item, m_metadata) == 0x18))) ? 1 : -1];
typedef char hopper_item_information_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_hopper_selected_item, m_information) == 0x190))) ? 1 : -1];
typedef char hopper_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_hopper_selected_item) == 0x1e8))) ? 1 : -1];

#endif
