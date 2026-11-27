#ifndef INTERFACE_GUI_SCREEN_GAME_OPTIONS_EXTERNS_H
#define INTERFACE_GUI_SCREEN_GAME_OPTIONS_EXTERNS_H

#include "interface/gui_widget_hierarchy.h"

// Functions defined in other units that the game options screen calls.
class c_gui_widget;
class c_game_variant;
class c_game_engine_base_variant;
extern "C" {
void game_engine_variant_set_description(c_game_engine_base_variant *variant, const char *description);
unsigned __int64 game_variant_compute_unique_id(const c_game_variant *variant);
c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
}

#endif // INTERFACE_GUI_SCREEN_GAME_OPTIONS_EXTERNS_H
