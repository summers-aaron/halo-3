#ifndef SAVED_GAMES_CONTENT_CATALOGUE_EXTERNS_H
#define SAVED_GAMES_CONTENT_CATALOGUE_EXTERNS_H

class c_content_catalogue;
struct s_content_item;
struct s_data_array;

extern "C" void content_catalogue_flush_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous);
extern "C" void content_item_delete_container(s_content_item *item, bool synchronous);
extern "C" bool content_item_load_metadata(s_content_item *content_item, bool synchronous);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, void *result);
extern "C" char *levels_get_map_scenario_path(long campaign_id, long map_id, char *name, long name_size);
extern "C" void levels_get_multiplayer_map_ids(long *unique_ids, long *count);

enum e_game_content_type;
#include "saved_games/content_catalogue_iteration.h"
#include "saved_games/content_catalogue_queries.h"

#endif // SAVED_GAMES_CONTENT_CATALOGUE_EXTERNS_H
