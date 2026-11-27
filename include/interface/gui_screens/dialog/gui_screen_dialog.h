#ifndef INTERFACE_GUI_SCREENS_DIALOG_GUI_SCREEN_DIALOG_H
#define INTERFACE_GUI_SCREENS_DIALOG_GUI_SCREEN_DIALOG_H

// The dialog screen (Bungie's interface\gui_screens\dialog\gui_screen_dialog.cpp; the March 2007 debug build asserts
// there at line 134). It is used by user_interface_messages.cpp
// (c_load_dialog_screen_message::apply_initial_state writes the March setters' fields) and user_interface_networking.cpp
// (reads get_dialog_name()). The owner gui_screen_dialog.cpp uses this definition.

#include <stddef.h>
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_render_data_size_types.h"
#include "tag_files/string_ids.h"

enum e_gui_dialog_choice;

// Names of this screen's widgets (set 1 of the string_ids table, in table order).
enum e_gui_dialog_string_id
{
    _gui_dialog_string_default_button_key = 0x10067,
    _gui_dialog_string_dialog_items = 0x10068,
};

// March 836a3100 constructs c_gui_ordered_data and copies the choices to +0x10c.
class c_dialog_screen_widget_dialog_items_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_dialog_screen_widget_dialog_items_datasource(long *values, long count) : m_current_item_count(count)
    {
        for (long i = 0; i < count; ++i)
            m_values[i] = values[i];
    }
    virtual void get_column_names(long *const column_names, long *column_count)
    {
        *column_count = 0;
        column_names[(*column_count)++] = _string_id_name;
    }
    virtual bool get_string_id_value(long index, long name, long *value);
protected:
    virtual long get_current_item_count_internal();
public:
    long m_values[4];
    long m_current_item_count;
};
typedef char dialog_items_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_dialog_screen_widget_dialog_items_datasource) == 0x120))) ? 1 : -1];
typedef char dialog_items_datasource_values_offset[
    (sizeof(void *) != 4 || ((offsetof(c_dialog_screen_widget_dialog_items_datasource, m_values) == 0x10c))) ? 1 : -1];

class c_gui_dialog_screen_widget : public c_gui_screen_widget {
public:
    c_gui_dialog_screen_widget(long name);
protected:
    virtual e_render_data_size get_render_data_size() { return _render_data_size_alert; }
public:
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void post_initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
    virtual void dispose();
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();

    void set_dialog_name(long dialog_name);
    // Simple accessors inlined into their callers.
    __forceinline long get_dialog_name() { return m_dialog_name; }
    __forceinline void set_dialog_invoker(long dialog_invoker) { m_dialog_invoker = dialog_invoker; }
    __forceinline void set_test_mode(bool test_mode) { m_test_mode = test_mode; }
    __forceinline void set_dispose_on_success_screen_index(long screen_index) { m_dispose_on_success_screen_index = screen_index; }

private:
    void send_dialog_result(e_controller_index controller, e_gui_dialog_choice choice);

public:
    long m_dialog_name;                         // March: get_dialog_name / set_dialog_name
    long m_dialog_invoker;                      // March: set_dialog_invoker
    long m_title;
    long m_description;
    short m_description_flags;
    short m_padding;
    long m_button_key;
    long m_choices[4];
    long m_choice_count;
    long m_b_button_action;
    bool m_test_mode;                           // March: set_test_mode
    bool m_result_sent;                         // gui_screen_dialog.cpp's name
    long m_dispose_on_success_screen_index;     // March: set_dispose_on_success_screen_index
};

typedef char gui_dialog_screen_widget_offsets[(sizeof(void *) != 4
    || ((offsetof(c_gui_dialog_screen_widget, m_dispose_on_success_screen_index) == 0x1abc) )) ? 1 : -1];

extern "C" long g_gui_last_dialog_name;

#endif // INTERFACE_GUI_SCREENS_DIALOG_GUI_SCREEN_DIALOG_H
