#ifndef INTERFACE_GUI_SCREEN_WIDGET_TEXT_H
#define INTERFACE_GUI_SCREEN_WIDGET_TEXT_H

class c_gui_screen_widget;

extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);
extern "C" void gui_screen_widget_set_text_widget_text(c_gui_screen_widget *screen, long name, const wchar_t *text);

#endif // INTERFACE_GUI_SCREEN_WIDGET_TEXT_H
