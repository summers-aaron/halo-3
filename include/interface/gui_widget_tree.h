#ifndef INTERFACE_GUI_WIDGET_TREE_H
#define INTERFACE_GUI_WIDGET_TREE_H

#include "interface/gui_widget_enums.h"

class c_gui_widget;

extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, e_gui_widget_type type);

#endif // INTERFACE_GUI_WIDGET_TREE_H
