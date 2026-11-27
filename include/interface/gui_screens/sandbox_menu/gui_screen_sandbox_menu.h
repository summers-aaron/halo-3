#ifndef INTERFACE_GUI_SCREENS_SANDBOX_MENU_GUI_SCREEN_SANDBOX_MENU_H
#define INTERFACE_GUI_SCREENS_SANDBOX_MENU_GUI_SCREEN_SANDBOX_MENU_H

#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_data.h"
#include "interface/user_interface_messages.h"
#include "memory/static_arrays.h"
#include <stddef.h>

class c_load_object_creation_screen_message : public c_load_screen_message
{
public:
    c_load_object_creation_screen_message(e_controller_index controller, e_window_index window, long layered_position, long category, bool is_secondary);
    __forceinline c_load_object_creation_screen_message(long screen_name, e_controller_index controller,
        e_window_index window, long layered_position, long category)
        : c_load_screen_message(screen_name, controller, window, layered_position)
    {
        m_category = category;
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    long m_category;
};

class c_sandbox_budget_summary_screen_widget : public c_gui_screen_widget
{
public:
    __forceinline c_sandbox_budget_summary_screen_widget(long name) :
        c_gui_screen_widget(name), m_budget_item_string_id(0) {}
    virtual void post_initialize();
    virtual void initialize_datasource();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    static bool handle_global_messages(const c_controller_input_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
    long m_selected_category;
    long m_invoked_from;
    long m_budget_item_string_id;
};

typedef char sandbox_budget_summary_screen_size[
    (sizeof(void *) != 4 || ((sizeof(c_sandbox_budget_summary_screen_widget) == 0x1a98))) ? 1 : -1];
class c_sandbox_object_menu_screen_widget : public c_gui_screen_widget {
public:
    __forceinline c_sandbox_object_menu_screen_widget(long name) : c_gui_screen_widget(name) {}
    virtual ~c_sandbox_object_menu_screen_widget() {}
    virtual void post_initialize();
    virtual void dispose();
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long current_milliseconds);
    long m_category;
    int : 32;
    int : 32;
    int : 32;
public:
    __forceinline void set_category(long category) { m_category = category; }
};

typedef char sandbox_object_menu_screen_size[
    (sizeof(void *) != 4 || ((sizeof(c_sandbox_object_menu_screen_widget) == 0x1a98))) ? 1 : -1];

struct s_sandbox_placeable_object_block;

class c_gui_placeable_object_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_placeable_object_datasource(long block_type) :
        number_of_placeable_object_types(0), m_block_type(block_type) {}
    struct s_sandbox_placeable_object
    {
        long name;
        long object_definition_index;
        float weight;
    };
    __forceinline s_sandbox_placeable_object get_placeable_object(long index) const
    {
        return m_placeable_object_types[index];
    }
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    friend void gui_placeable_object_datasource_update_from_block(c_gui_placeable_object_datasource *datasource, s_sandbox_placeable_object_block *block);
protected:
    virtual long get_current_item_count_internal();
    c_static_array<s_sandbox_placeable_object, 64> m_placeable_object_types;
    long number_of_placeable_object_types;
    long m_block_type;
};

class c_gui_budget_range_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_budget_range_datasource() : m_maximum(0) {}
    virtual bool initialize(long name, long minimum, long maximum);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal();
public:
    long m_minimum;
    long m_maximum;
};

class c_sandbox_budget_items_datasource : public c_gui_data_proxy
{
public:
    __forceinline c_sandbox_budget_items_datasource(c_gui_data *base_datasource,
        c_sandbox_budget_summary_screen_widget *screen, long budget_item_string_id)
    {
        m_base_datasource = base_datasource;
        m_screen = screen;
        m_value = budget_item_string_id;
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    c_sandbox_budget_summary_screen_widget *m_screen;
    long m_value;
};

typedef char sandbox_range_size[(sizeof(void *) != 4 || ((sizeof(c_gui_budget_range_datasource) == 0x114))) ? 1 : -1];
typedef char sandbox_range_minimum_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_budget_range_datasource, m_minimum) == 0x10c))) ? 1 : -1];
typedef char sandbox_placeable_size[(sizeof(void *) != 4 || ((sizeof(c_gui_placeable_object_datasource) == 0x414))) ? 1 : -1];
typedef char sandbox_budget_items_size[(sizeof(void *) != 4 || ((sizeof(c_sandbox_budget_items_datasource) == 20))) ? 1 : -1];

// String IDs used by the sandbox menu screens, datasources and messages.
enum e_sandbox_menu_string_id
{
    _sandbox_menu_string_sandbox_budget_screen = 0x10531,
    _sandbox_menu_string_sandbox_budget_screen_splitscreen = 0x10532,
    _sandbox_menu_string_sandbox_objects_menu = 0x10533,
    _sandbox_menu_string_sandbox_objects_menu_splitscreen = 0x10534,
    _sandbox_menu_string_sandbox_object_properties_panel = 0x10535,
    _sandbox_menu_string_sandbox_object_properties_panel_splitscreen = 0x10536,
    _sandbox_menu_string_create_category_list = 0x10537,
    _sandbox_menu_string_categories_datasource = 0x1053a,
    _sandbox_menu_string_category_weapons = 0x1053d,
    _sandbox_menu_string_category_vehicles = 0x1053e,
    _sandbox_menu_string_category_equipment = 0x1053f,
    _sandbox_menu_string_category_scenery = 0x10540,
    _sandbox_menu_string_category_teleporters = 0x10541,
    _sandbox_menu_string_category_spawners = 0x10542,
    _sandbox_menu_string_category_goals = 0x10543,
    _sandbox_menu_string_common_spinner = 0x10544,
    _sandbox_menu_string_item_cost = 0x10545,
    _sandbox_menu_string_items_left = 0x10546,
    _sandbox_menu_string_budget_datasource = 0x10547,
    _sandbox_menu_string_budget_placed = 0x10549,
    _sandbox_menu_string_budget_maximum = 0x1054a,
    _sandbox_menu_string_budget_minumum = 0x1054b,
    _sandbox_menu_string_budget_total = 0x1054c,
    _sandbox_menu_string_budget_total_price = 0x1054d,
};

// March: gui_screen_sandbox_menu.cpp, constructor c_load_budget_screen_message(e_controller_index, e_window_index, long,
// long, long, bool). The budget summary screen takes the three values in apply_initial_state.
class c_load_budget_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_budget_screen_message(e_controller_index controller, e_window_index window,
        long budget_index, long invoked_from, long budget_item_string_id, bool secondary) :
        c_load_screen_message(secondary ? _sandbox_menu_string_sandbox_budget_screen_splitscreen
                : _sandbox_menu_string_sandbox_budget_screen,
            controller, window, 0x1000c),
        m_budget_index(budget_index), m_invoked_from(invoked_from), m_budget_item_string_id(budget_item_string_id) {}
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;
    long m_budget_index;
    long m_invoked_from;
    long m_budget_item_string_id;
};

typedef char sandbox_budget_message_size[(sizeof(void *) != 4 || ((sizeof(c_load_budget_screen_message) == 0x48))) ? 1 : -1];

struct s_game_engine_globals;
struct game_globals_storage;

extern "C" long g_sandbox_menu_focused_handle;

#endif // INTERFACE_GUI_SCREENS_SANDBOX_MENU_GUI_SCREEN_SANDBOX_MENU_H
