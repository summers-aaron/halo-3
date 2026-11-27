#ifndef INTERFACE_C_GUI_BUTTON_KEY_WIDGET_H
#define INTERFACE_C_GUI_BUTTON_KEY_WIDGET_H

#include "interface/c_gui_group_widget.h"
#include "interface/group_widget_definition.h"

// March 837158a0 constructs the group base; no additional data members.
class c_gui_button_key_widget : public c_gui_group_widget
{
public:
    __forceinline c_gui_button_key_widget() : c_gui_group_widget(_gui_button_key_widget) {}
    virtual ~c_gui_button_key_widget() {}
    virtual void initialize(const s_button_key_block *definition, bool create_children);
    virtual bool get_string_by_string_id(long string_identifier, c_static_wchar_string<1024> *buffer);
};

typedef char gui_button_key_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_button_key_widget) == 0x138))) ? 1 : -1];

#endif
