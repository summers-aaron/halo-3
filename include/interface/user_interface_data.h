#ifndef INTERFACE_USER_INTERFACE_DATA_H
#define INTERFACE_USER_INTERFACE_DATA_H

#include "interface/gui_data.h"
#include "tag_files/tag_groups.h"
#include <stddef.h>

struct s_datasource_definition;

// March asserts in interface/user_interface_data.cpp; no header assertion
// survives. The class lives beside that owner, independently of gui_data.h.
class c_gui_tag_datasource : public c_gui_ordered_data
{
public:
    c_gui_tag_datasource() : m_definition_index(-1) {}
    c_gui_tag_datasource(long definition_index) : m_definition_index(definition_index) {}
    virtual ~c_gui_tag_datasource() {}
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *buffer);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool get_invoked_control(long element_handle, long *control_name);

protected:
    virtual long get_current_item_count_internal();

private:
    const s_datasource_definition *get_definition()
    {
        const s_datasource_definition *definition = 0;
        if (m_definition_index != -1)
            definition = (const s_datasource_definition *)tag_get('dsrc', m_definition_index);
        return definition;
    }

public:
    long m_definition_index;
};

// Datasources live in a 32-entry global array, so the checks below lock the class size and definition-index placement.
typedef char gui_tag_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_gui_tag_datasource) == 0x110) )) ? 1 : -1];
typedef char gui_tag_datasource_definition_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_tag_datasource, m_definition_index) == 0x10c) )) ? 1 : -1];

// A datasource that stands in for another (March interface\user_interface_data.cpp): each query is passed on to
// the base datasource; initialize() is its one new virtual. It passes on no string-id query (slot 13 stays
// c_gui_data's unless a subclass overrides it).
class c_gui_data_proxy : public c_gui_data
{
public:
    virtual long get_current_item_count();
    virtual long get_first_element_handle();
    virtual long get_next_element_handle(long element_handle);
    virtual long get_previous_element_handle(long element_handle);
    virtual bool get_element(long element_handle, void *element, long element_size);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual long get_maximum_item_count();
    virtual void set_element(long element_handle, const void *element, long element_size);
    virtual bool initialize();

    __forceinline c_gui_data *get_base_datasource() { return m_base_datasource; }

    c_gui_data *m_base_datasource;
};

// The text formats the test and tag datasources print with (string literals of user_interface_data.obj).

#endif // INTERFACE_USER_INTERFACE_DATA_H
