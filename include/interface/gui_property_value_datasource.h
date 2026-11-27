#ifndef INTERFACE_GUI_PROPERTY_VALUE_DATASOURCE_H
#define INTERFACE_GUI_PROPERTY_VALUE_DATASOURCE_H

#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include <stddef.h>

struct s_gui_property_value_option { long string_id; long value; };
struct c_gui_property_value_datasource : c_gui_ordered_data {
    __forceinline c_gui_property_value_datasource(long name) : property_name(name), option_count(0) {}
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long column_name, long *value);
    virtual bool get_element(long index, void *element, long element_size);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_string_id_value(long index, long type, long *value);
    s_gui_property_value_option options[32];
    long property_name;
    long option_count;
    bool real_range;
    float minimum;
    float maximum;
protected:
    virtual long get_current_item_count_internal();
};

typedef char gui_property_value_minimum_offset[(sizeof(void *) != 4 || offsetof(c_gui_property_value_datasource, minimum) == 0x218) ? 1 : -1];
typedef char gui_property_value_size[(sizeof(void *) != 4 || sizeof(c_gui_property_value_datasource) == 0x220) ? 1 : -1];

#endif
