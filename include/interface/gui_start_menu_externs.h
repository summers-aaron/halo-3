#ifndef INTERFACE_GUI_START_MENU_EXTERNS_H
#define INTERFACE_GUI_START_MENU_EXTERNS_H

#include "interface/c_gui_list_item_widget.h"

class c_gui_widget;
enum e_gui_widget_type;
class c_gui_screen_widget;
struct s_player_identifier;

extern "C" c_gui_widget *gui_widget_get_parent_list_widget(c_gui_widget *widget);
extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, e_gui_widget_type type);
extern "C" c_gui_list_item_widget *gui_widget_get_parent_list_item_widget(c_gui_widget *widget);

#endif // INTERFACE_GUI_START_MENU_EXTERNS_H
