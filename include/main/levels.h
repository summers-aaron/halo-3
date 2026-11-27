#ifndef MAIN_LEVELS_H
#define MAIN_LEVELS_H

#include "cseries/async_types.h"
#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/levels.cpp that other units call.

struct levels_insertion_datum
{
    short identifier;
    short insertion_count;
    long unique_id;
    wchar_t names[4][32];
    wchar_t descriptions[4][128];
    unsigned char zone_sets[4];
};
struct s_cache_file_resource_element_set;
struct s_data_array;
struct s_datum;
struct s_levels_campaign_source;


extern "C" bool levels_try_and_get_insertion(long unique_id, levels_insertion_datum *result);
extern "C" long levels_find_campaign_for_map_name(const char *name);
extern "C" short levels_get_campaign_map_index(long campaign_id, long map_id);
extern "C" long levels_get_default_multiplayer_map_id(void);
extern "C" bool levels_is_map_type_allowed(long type);
extern "C" void levels_get_multiplayer_map_ids(long *unique_ids, long *count);
extern "C" char *levels_get_map_scenario_path(long campaign_id, long map_id, char *name, long name_size);
extern "C" bool levels_try_and_get_datum(s_data_array *data, int unique_id, s_datum *result);
extern "C" unsigned char levels_is_index_in_range(const s_cache_file_resource_element_set *set, long index);
extern "C" void levels_get_campaign_map_ids(long campaign_id, long *map_ids, long *count);
extern "C" void levels_update(void);
struct s_levels_enumeration_task;
extern "C" e_async_completion levels_enumerate_info_files(s_levels_enumeration_task *task);
extern "C" void levels_add_campaign(const s_levels_campaign_source *source, bool byte_swap, const char *map_directory, bool downloadable);
e_map_id levels_get_campaign_next_map_id(e_campaign_id campaign_id, e_map_id map_id);

void levels_initialize_for_new_map();
void levels_dispose_from_old_map();

extern "C" void levels_initialize(void);
extern "C" void levels_dispose(void);
#endif // MAIN_LEVELS_H
