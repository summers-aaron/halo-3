#include "interface/gui_selected_item_metadata.h"
#include "saved_games/content_catalogue_iteration.h"
#ifndef INCLUDE_INTERFACE_GUI_SELECTED_ITEMS_MAP_DEPENDENCIES_H
#define INCLUDE_INTERFACE_GUI_SELECTED_ITEMS_MAP_DEPENDENCIES_H

#include "memory/static_arrays.h"
struct s_data_array;
struct levels_map_datum;
struct s_saved_game_item_metadata_packed;
class c_content_catalogue;
extern "C" bool saved_game_files_get_metadata_string(const void *metadata, long string_id, c_static_wchar_string<1024> *string);
extern "C" bool levels_is_map_type_allowed(long type);
extern "C" void levels_get_multiplayer_map_ids(long *unique_ids, long *count);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, levels_map_datum *result);
extern "C" bool content_catalogue_item_metadata_matches(const c_content_catalogue *catalogue,
    long content_item_index, s_saved_game_item_metadata_packed *metadata);


struct s_gui_selected_items_block;

#endif // INCLUDE_INTERFACE_GUI_SELECTED_ITEMS_MAP_DEPENDENCIES_H
