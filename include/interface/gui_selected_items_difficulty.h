#ifndef INTERFACE_GUI_SELECTED_ITEMS_DIFFICULTY_H
#define INTERFACE_GUI_SELECTED_ITEMS_DIFFICULTY_H
#include "game/game_types.h"
#include "interface/gui_simple_selected_item.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"

class c_gui_difficulty_selected_item : public c_gui_simple_selected_item
{
public:
    c_gui_difficulty_selected_item()
        : c_gui_simple_selected_item(_gui_selected_item_none, L"", L"",
            _special_item_type_none, _gui_selected_item_location_none, 0, 0),
          m_difficulty(NONE)
    {
    }

    c_gui_difficulty_selected_item(const wchar_t *name, const wchar_t *description,
        e_campaign_difficulty_level difficulty, e_special_item_type special_type)
        : c_gui_simple_selected_item(_gui_selected_item_difficulty, name, description,
            special_type, _gui_selected_item_location_none, 0, 0),
          m_difficulty(difficulty)
    {
    }

    static bool find_title_and_description(
        e_campaign_difficulty_level difficulty, c_static_wchar_string<1024> *title, c_static_wchar_string<1024> *description);
    long get_difficulty() const { return m_difficulty; }

    long m_difficulty;
};

// The item array is 8-aligned while the base ends at 0x10c, so the compiler inserts 4 bytes of padding here.
class __declspec(align(8)) c_gui_difficulty_category_datasource : public c_gui_ordered_data
{
protected:
    virtual long get_current_item_count_internal();
public:
    c_gui_difficulty_category_datasource();
    virtual bool initialize(long);
    virtual const c_gui_selected_item *get_gui_selected_item(long element_handle) const;
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);

    c_static_array<c_gui_difficulty_selected_item, 4> m_selected_items;
    long m_selected_item_count;
};
#endif
