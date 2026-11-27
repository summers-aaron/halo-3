#include "saved_games/saved_game_item_metadata.h"
#ifndef INTERFACE_GUI_SCREEN_PREGAME_SELECTION_DEPENDENCIES_H
#define INTERFACE_GUI_SCREEN_PREGAME_SELECTION_DEPENDENCIES_H

#include "interface/gui_pregame_setup_manager.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/saved_game_files_io.h"
#include "saved_games/saved_game_files_operations.h"

extern "C" bool network_join_is_busy(void);

struct s_item;
#include "interface/gui_list_widget_queries.h"

class c_gui_widget;
class c_gui_list_widget;
enum e_saved_game_file_category;
enum e_saved_game_file_type;
class c_synchronized_long;

#endif
