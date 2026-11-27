#ifndef INTERFACE_GUI_WIDGET_HIERARCHY_H
#define INTERFACE_GUI_WIDGET_HIERARCHY_H

class c_gui_widget;
class c_gui_list_widget;

extern "C" c_gui_list_widget *gui_widget_get_parent_list_widget(c_gui_widget *widget);
extern "C" void gui_widget_set_alternate_state(c_gui_widget *widget, unsigned char value);
extern "C" void gui_widget_remove_child(c_gui_widget *parent, c_gui_widget *child);

#endif // INTERFACE_GUI_WIDGET_HIERARCHY_H
