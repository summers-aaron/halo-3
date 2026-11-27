#ifndef INTERFACE_GUI_WIDGET_TREE_QUERIES_H
#define INTERFACE_GUI_WIDGET_TREE_QUERIES_H
#include "interface/gui_widget_types.h"
class c_gui_widget;
extern "C" c_gui_widget *gui_widget_get_previous_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type);
#endif
