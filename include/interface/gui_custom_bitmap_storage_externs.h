#ifndef INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_EXTERNS_H
#define INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_EXTERNS_H


struct s_autosave_queue;
class c_gui_widget;

extern "C" void autosave_queue_update(s_autosave_queue *manager);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);

#endif // INTERFACE_GUI_CUSTOM_BITMAP_STORAGE_EXTERNS_H
