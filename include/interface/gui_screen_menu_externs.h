#ifndef INTERFACE_GUI_SCREEN_MENU_EXTERNS_H
#define INTERFACE_GUI_SCREEN_MENU_EXTERNS_H

#include "main/main_render_loading_types.h"

class c_gui_widget;

extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
extern "C" void controller_get_signed_in_player_identifiers(void *self, void *identifiers);

#endif // INTERFACE_GUI_SCREEN_MENU_EXTERNS_H
