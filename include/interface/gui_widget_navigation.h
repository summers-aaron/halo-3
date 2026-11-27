#ifndef INTERFACE_GUI_WIDGET_NAVIGATION_H
#define INTERFACE_GUI_WIDGET_NAVIGATION_H

#include "interface/gui_widget_enums.h"

class c_gui_widget;

extern "C" c_gui_widget *gui_widget_create_and_add_child_list_widget(c_gui_widget *parent, const void *source);
extern "C" c_gui_widget *gui_widget_get_previous_list_item(c_gui_widget *widget, bool require_valid);
extern "C" c_gui_widget *gui_widget_get_next_list_item(c_gui_widget *widget, bool require_valid);
extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type);

#endif // INTERFACE_GUI_WIDGET_NAVIGATION_H
