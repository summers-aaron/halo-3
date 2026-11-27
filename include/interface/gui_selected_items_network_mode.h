#ifndef INTERFACE_GUI_SELECTED_ITEMS_NETWORK_MODE_H
#define INTERFACE_GUI_SELECTED_ITEMS_NETWORK_MODE_H
#include "interface/gui_selected_item.h"
// Names from the network_mode_* string ids each value is added with (mainmenu.map string table).
enum e_network_mode_category
{
    _network_mode_category_none = -1,
    _network_mode_category_xbox_live_private = 0,
    _network_mode_category_system_link,
    _network_mode_category_offline,
    k_network_mode_category_count
};
enum e_network_mode_choice
{
    _network_mode_choice_none = -1,
    _network_mode_choice_xbox_live_advertise_anyone = 0,
    _network_mode_choice_xbox_live_advertise_friends,
    _network_mode_choice_xbox_live_advertise_invite_only,
    _network_mode_choice_system_link_advertise,
    _network_mode_choice_system_link_browse,
    k_network_mode_choice_count
};
#include "interface/gui_selected_items.h"
#include "interface/gui_data.h"

#include "interface/gui_simple_selected_item.h"
#include "interface/c_gui_screen_widget.h"
#include "memory/static_arrays.h"

class c_gui_network_mode_selected_item : public c_gui_simple_selected_item
{
public:
    __forceinline c_gui_network_mode_selected_item() :
        c_gui_simple_selected_item(_gui_selected_item_none, L"", L"", c_gui_selected_item::_special_item_type_none,
            _gui_selected_item_location_none, 0, 0)
    {
        m_network_mode = _network_mode_category_offline;
        m_choice = k_network_mode_choice_count;
    }

    c_gui_network_mode_selected_item(const wchar_t *name, const wchar_t *description, e_network_mode_category network_mode,
        e_network_mode_choice choice, c_gui_selected_item::e_special_item_type special_type) :
        c_gui_simple_selected_item(_gui_selected_item_advertisement, name, description, special_type, _gui_selected_item_location_none, 0, 0)
    {
        m_network_mode = network_mode;
        m_choice = choice;
    }

    c_gui_network_mode_selected_item &operator=(const c_gui_network_mode_selected_item &other)
    {
        c_gui_simple_selected_item::operator=(other);
        m_network_mode = other.m_network_mode;
        m_choice = other.m_choice;
        return *this;
    }

    long get_network_mode() const { return m_network_mode; }

    long m_network_mode;
    long m_choice;
};

class c_gui_network_mode_category_datasource : public c_gui_ordered_data
{
public:
    c_gui_network_mode_category_datasource(c_gui_screen_widget *screen);
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual const c_gui_selected_item *get_gui_selected_item(long element_handle) const;

protected:
    virtual long get_current_item_count_internal();

private:
    void add_category(long name_handle, long description_handle, e_network_mode_category network_mode, e_network_mode_choice choice, bool special);

public:
    wchar_t m_mode_names[3][48];
    c_static_array<c_gui_network_mode_selected_item, 3> m_selected_items;
    long m_selected_item_count;
    c_gui_screen_widget *m_screen;
};

class c_gui_network_mode_subitem_selectable_item_datasource : public c_gui_ordered_data
{
public:
    c_gui_network_mode_subitem_selectable_item_datasource(c_gui_screen_widget *screen);
    virtual bool initialize(long name, e_network_mode_category network_mode);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual const c_gui_selected_item *get_gui_selected_item(long element_handle) const;
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);

protected:
    virtual long get_current_item_count_internal();

private:
    void add_subitem(long name_handle, long description_handle, e_network_mode_category network_mode, e_network_mode_choice choice);

public:
    wchar_t m_mode_names[3][48];
    c_static_array<c_gui_network_mode_selected_item, 3> m_selected_items;
    long m_selected_item_count;
    c_gui_screen_widget *m_screen;
};

#endif
