#ifndef INTERFACE_C_START_MENU_ITEM_GUI_DATA_H
#define INTERFACE_C_START_MENU_ITEM_GUI_DATA_H

class c_start_menu_item_gui_data
{
public:
    virtual ~c_start_menu_item_gui_data() {}
    virtual bool initialize(long name) = 0;
    virtual void dispose() = 0;
    virtual void update() = 0;
    virtual long get_current_item_count_internal() = 0;
    virtual long get_current_item_count() = 0;
    virtual long get_first_element_handle() = 0;
    virtual long get_next_element_handle(long element_handle) = 0;
    virtual long get_previous_element_handle(long element_handle) = 0;
    virtual void get_column_names(long *const column_names, long *column_count) = 0;
    virtual bool get_element(long element_handle, void *element, long element_size) = 0;
    virtual bool get_integer_value(long element_handle, long value_name, long *value) = 0;
    virtual bool get_text_value(long element_handle, long value_name, void *value) = 0;
    virtual bool get_string_id_value(long element_handle, long value_name, long *value) = 0;
    virtual bool get_real_value(long element_handle, long value_name, float *value) = 0;
    virtual bool get_qword_value(long element_handle, long value_name, unsigned __int64 *value) = 0;
    virtual bool get_player_appearance(long element_handle, void *appearance) = 0;
    virtual const void *get_gui_selected_item(long element_handle) const = 0;
    virtual long get_maximum_item_count() = 0;
    virtual long add_element() = 0;
    virtual void delete_element(long element_handle) = 0;
    virtual void set_element(long element_handle, const void *element, long element_size) = 0;
    virtual bool get_invoked_control(long element_handle, long *control_name) = 0;
    virtual void clear_disabled_elements() = 0;
    virtual void set_disabled_element(long element_handle, long reason) = 0;
    long m_name;
};

typedef char start_menu_item_gui_data_size[(sizeof(void *) != 4 || sizeof(c_start_menu_item_gui_data) == 8) ? 1 : -1];

#endif // INTERFACE_C_START_MENU_ITEM_GUI_DATA_H
