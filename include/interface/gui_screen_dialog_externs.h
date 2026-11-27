#ifndef INTERFACE_GUI_SCREEN_DIALOG_EXTERNS_H
#define INTERFACE_GUI_SCREEN_DIALOG_EXTERNS_H

#include "game/game_engine_globals.h"
#include "interface/gui_widget_tree.h"
#include "interface/gui_widget_navigation.h"
#include "scenario/scenario_map_variant.h"

class c_gui_screen_widget;
struct s_game_engine_globals;


extern "C" {
int GetProcessHeap();
void RtlFreeHeap(int heap, unsigned int flags, void *memory);
void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);
}

#endif // INTERFACE_GUI_SCREEN_DIALOG_EXTERNS_H
