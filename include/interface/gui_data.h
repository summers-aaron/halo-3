#ifndef INTERFACE_GUI_DATA_H
#define INTERFACE_GUI_DATA_H

template<long k_maximum_count> class c_static_wchar_string;
struct s_player_appearance;
class c_gui_selected_item;
struct s_data_array;

class c_gui_data {
public:
    __forceinline c_gui_data() : m_name(0) {}
    virtual ~c_gui_data() {}
    virtual bool initialize(long name);
    virtual void dispose();
    virtual void update();
    // Name from the units that call it (gui_screen_pregame_selection, gui_screen_pregame_lobby_theater).
    virtual bool is_busy();
    virtual long get_current_item_count() = 0;
    virtual long get_first_element_handle() = 0;
    virtual long get_next_element_handle(long element_handle) = 0;
    virtual long get_previous_element_handle(long element_handle) = 0;
    virtual void get_column_names(long *const column_names, long *column_count) = 0;
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool get_qword_value(long element_handle, long value_name, unsigned __int64 *value);
    virtual bool get_real_value(long element_handle, long value_name, float *value);
    virtual bool get_player_appearance(long element_handle, s_player_appearance *appearance);
    virtual const c_gui_selected_item *get_gui_selected_item(long element_handle) const;
    virtual long get_maximum_item_count();
    // Default implementation returns -1.
    // March maps its body to user_interface_data.obj, so it may have been out of line there (open; see report).
    virtual long add_element() { return -1; }
    virtual void delete_element(long element_handle);
    virtual void set_element(long element_handle, const void *element, long element_size);
    virtual bool get_invoked_control(long element_handle, long *control_name);
    virtual void clear_disabled_elements();
    virtual void set_disabled_element(long element_handle, long reason);
    virtual void disable_all_elements(long reason);
    virtual void enable_element(long element_handle, long reason);

    __forceinline long get_name() const { return m_name; }
    __forceinline void set_name(long name) { m_name = name; }

    long m_name;
};

class c_gui_data_array : public c_gui_data {
public:
    virtual ~c_gui_data_array();
    virtual bool initialize(long name);
    virtual long add_element();
    virtual void delete_element(long element_handle);
    virtual long get_maximum_item_count();
    virtual long get_current_item_count();
    virtual long get_first_element_handle();
    virtual long get_next_element_handle(long element_handle);
    virtual long get_previous_element_handle(long element_handle);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual void set_element(long element_handle, const void *element, long element_size);

    s_data_array *m_list_data;
};

struct s_disabled_element
{
    long element_handle;
    long reason;
};
enum { k_maximum_ordered_elements = 32 };

class c_gui_ordered_data : public c_gui_data {
public:
    __forceinline c_gui_ordered_data() : m_disabled_element_count(0) {}
    virtual long get_current_item_count();
    virtual long get_first_element_handle();
    virtual long get_next_element_handle(long element_handle);
    virtual long get_previous_element_handle(long element_handle);
    virtual void clear_disabled_elements();
    virtual void set_disabled_element(long element_handle, long reason);
    virtual void disable_all_elements(long reason);
    virtual void enable_element(long element_handle, long reason);

protected:
    virtual long get_current_item_count_internal() = 0;

private:
    bool is_enabled(long element_handle);

public:
    // set_disabled_element (March 0x836ab978) appends {element_handle, reason} at 0x8 + 8 * count while count + 1 < 32;
    // the constructor (March 0x836ab4f0) clears only the count. Element names as user_interface_data.cpp had them.
    s_disabled_element m_disabled_elements[k_maximum_ordered_elements];
    long m_disabled_element_count;
};

typedef char gui_data_size_base[(sizeof(void *) != 4 || ((sizeof(c_gui_data) == 8) )) ? 1 : -1];
typedef char gui_data_size_array[(sizeof(void *) != 4 || ((sizeof(c_gui_data_array) == 12) )) ? 1 : -1];
typedef char gui_data_size_ordered[(sizeof(void *) != 4 || ((sizeof(c_gui_ordered_data) == 0x10c) )) ? 1 : -1];

#endif // INTERFACE_GUI_DATA_H
