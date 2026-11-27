#ifndef INTERFACE_GUI_SELECTED_ITEMS_LEVEL_H
#define INTERFACE_GUI_SELECTED_ITEMS_LEVEL_H

#include "interface/gui_data.h"
#include "interface/gui_simple_selected_item.h"
#include "game/game_types.h"

class c_gui_level_selected_item : public c_gui_simple_selected_item
{
public:
    __forceinline long get_special_item_type() const { return m_special_type; }
    __forceinline long get_sort_key() const { return m_sort_key; }
    __forceinline e_map_id get_map_id() const { return m_map_id; }
    __forceinline c_gui_level_selected_item() :
        c_gui_simple_selected_item(_gui_selected_item_none, L"", L"",
            c_gui_selected_item::_special_item_type_none, _gui_selected_item_location_none, 0, 0)
    {
        m_sort_key = -1;
        m_map_id = _map_id_none;
        m_flag3 = 0;
        m_extra = 0;
    }
    __forceinline c_gui_level_selected_item(const c_gui_level_selected_item &other) :
        c_gui_simple_selected_item(other)
    {
        m_sort_key = other.m_sort_key;
        m_map_id = other.m_map_id;
        m_flag3 = other.m_flag3;
        m_extra = other.m_extra;
    }
    c_gui_level_selected_item(const wchar_t *name, const wchar_t *description, long campaign_id, e_map_id map_id, unsigned char locked, long special_type);
    c_gui_level_selected_item &operator=(const c_gui_level_selected_item &other);
    friend class c_gui_level_subitem_selectable_item_datasource;
    friend class c_gui_level_category_datasource;
public:
    long m_sort_key;
    e_map_id m_map_id;
    union
    {
        unsigned char m_flag3;
        bool m_locked;
    };
    unsigned short m_extra;
};

struct s_gui_level_list_entry
{
    wchar_t m_name[48];
    c_gui_level_selected_item m_item;
    __forceinline s_gui_level_list_entry() {}
};
typedef char gui_level_selected_item_size[(sizeof(void *) != 4 || ((sizeof(c_gui_level_selected_item) == 0x1a0))) ? 1 : -1];
typedef char gui_level_list_entry_size[(sizeof(void *) != 4 || ((sizeof(s_gui_level_list_entry) == 0x200))) ? 1 : -1];

class c_gui_level_subitem_selectable_item_datasource : public c_gui_ordered_data
{
public:
    c_gui_level_subitem_selectable_item_datasource();
    // Initializes the level entries for the given controller, name, and campaign.
    virtual bool initialize_levels(long controller, long name, long campaign);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long item_index, long string_id, long *value);
    virtual bool get_integer_value(long item_index, long string_id, long *value);
    virtual const c_gui_selected_item *get_gui_selected_item(long item_index) const;
protected:
    virtual long get_current_item_count_internal();
public:
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
public:
    s_gui_level_list_entry m_entries[64];    // 512 bytes each
    long m_item_count;
};

class c_gui_level_category_datasource : public c_gui_ordered_data
{
public:
    c_gui_level_category_datasource();
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_integer_value(long item_index, long string_id, long *value);
    virtual bool initialize(long name);
protected:
    virtual long get_current_item_count_internal();
    c_gui_level_selected_item m_items[64];
    long m_item_count;
};


// Functions defined in tu/interfacelibxbox/gui_selected_items_level.cpp.

class c_gui_level_subitem_selectable_item_datasource;
class c_gui_level_selected_item;

extern "C" int __cdecl level_selected_item_sort_proc(const void *element_a, const void *element_b);

#endif // INTERFACE_GUI_SELECTED_ITEMS_LEVEL_H
