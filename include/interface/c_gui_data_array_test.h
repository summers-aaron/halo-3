#ifndef INTERFACE_C_GUI_DATA_ARRAY_TEST_H
#define INTERFACE_C_GUI_DATA_ARRAY_TEST_H

#include "interface/gui_data.h"

// March user_interface_data.cpp: constructor 836aa378, column names 836aa548.
class c_gui_data_array_test : public c_gui_data_array
{
public:
    __forceinline c_gui_data_array_test() : c_gui_data_array() { m_list_data = 0; }
    virtual bool initialize(long name);
    virtual void get_column_names(long *const names, long *count) { *count = 0; }
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *buffer);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
};

typedef char gui_data_array_test_size[(sizeof(void *) != 4 || sizeof(c_gui_data_array_test) == 12) ? 1 : -1];

#endif
