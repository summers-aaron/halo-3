#ifndef INTERFACE_GUI_CAMPAIGN_INSERTION_DATASOURCE_H
#define INTERFACE_GUI_CAMPAIGN_INSERTION_DATASOURCE_H

#include "interface/gui_data.h"
#include <stddef.h>

// Campaign insertion-point selection list.
// Customizes initialization, text lookup, column names and ordered item count.
class c_gui_campaign_insertion_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_campaign_insertion_datasource(long string_list_tag_index)
        : m_count(0), m_flags(1), m_string_list_tag_index(string_list_tag_index) {}
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    short update_insertions(long campaign_id, long map_id);
protected:
    virtual long get_current_item_count_internal();
private:
    wchar_t m_names[4][32];
    wchar_t m_descriptions[4][128];
    short m_count;
    short m_flags;
    long m_string_list_tag_index;
};
typedef char campaign_insertion_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_gui_campaign_insertion_datasource) == 0x614))) ? 1 : -1];

#endif
