#ifndef INTERFACE_GUI_SCREEN_OBJECT_PROPERTIES_EXTERNS_H
#define INTERFACE_GUI_SCREEN_OBJECT_PROPERTIES_EXTERNS_H

#include "memory/static_arrays.h"
#include "game/game_engine_globals.h"

struct scenario;

#include "scenario/scenario.h"
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);

long map_variant_value_pair_get_value_index(long property_name, long search_value);

struct s_game_engine_globals;
class c_gui_widget;
class c_gui_screen_widget;
#include "cseries/cseries_unicode.h"

#endif // INTERFACE_GUI_SCREEN_OBJECT_PROPERTIES_EXTERNS_H
