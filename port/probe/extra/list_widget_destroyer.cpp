// tmp/probe (GAME-CHANGES F53): the deleting destructor of the list widget hand vtable (F29, DAT_82055fc8 slot 1).
// Retail's slot is a fold shared with c_start_menu_settings_controls_button::`scalar deleting destructor' (same
// code: run the c_gui_widget destructor, free when asked); the PC member of that name destroys a screen widget, and
// c_gui_widget's own one is a trap (abstract class), so the list widget gets this one.
#include "interface/c_list_widget_layout.h"
#include "interface/user_interface_memory.h"

struct probe_list_widget_destroyer
{
    void *destroy(unsigned int flags);
};

void *probe_list_widget_destroyer::destroy(unsigned int flags)
{
    c_list_widget_layout *widget = (c_list_widget_layout *)this;
    widget->c_gui_widget::~c_gui_widget();
    if (flags & 1)
        user_interface_free(widget);
    return widget;
}
