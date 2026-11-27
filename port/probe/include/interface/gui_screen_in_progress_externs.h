#ifndef INTERFACE_GUI_SCREEN_IN_PROGRESS_EXTERNS_H
#define INTERFACE_GUI_SCREEN_IN_PROGRESS_EXTERNS_H

class c_gui_screen_widget;

// PC port: text_group.cpp defines extern "C" bool text_group_get_string; declaring int reads dirty upper %eax on i686.
extern "C" bool text_group_get_string(int controller, int id, wchar_t *buffer);
extern "C" void gui_screen_widget_set_text_widget_text(void *screen, int id, const wchar_t *text);
extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);

#endif // INTERFACE_GUI_SCREEN_IN_PROGRESS_EXTERNS_H
